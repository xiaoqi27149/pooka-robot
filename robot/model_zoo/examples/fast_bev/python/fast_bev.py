
from rknn.api import RKNN
import sys
import os
import argparse
import math
import copy
import torch
import numpy as np
from utils.utils import visual, load_ten
from utils.config import Config
import numba
from torchvision import transforms
from PIL import Image

realpath = os.path.abspath(__file__)
_sep = os.path.sep
realpath = realpath.split(_sep)
root = _sep.join(realpath[:len(realpath) - realpath[::-1].index("python")])
sys.path.append(root)

def limit_period(val, offset=0.5, period=np.pi):
    return val - torch.floor(val / period + offset) * period

def get_points(n_voxels, voxel_size, origin):
    points = torch.stack(
        torch.meshgrid(
            [
                torch.arange(n_voxels[0]),
                torch.arange(n_voxels[1]),
                torch.arange(n_voxels[2]),
            ]
        )
    )
    new_origin = origin - n_voxels / 2.0 * voxel_size
    points = points * voxel_size.view(3, 1, 1, 1) + new_origin.view(3, 1, 1, 1)
    return points


def compute_projection(img_meta, stride, noise=0):
    projection = []
    intrinsic = torch.tensor(img_meta["lidar2img"]["intrinsic"][:3, :3])
    intrinsic[:2] /= stride
    extrinsics = map(torch.tensor, img_meta["lidar2img"]["extrinsic"])
    for extrinsic in extrinsics:
        if noise > 0:
            projection.append(intrinsic @ extrinsic[:3] + noise)
        else:
            projection.append(intrinsic @ extrinsic[:3])
    return torch.stack(projection)


def get_project_output(img_metas, mlvl_feats, img_shape, n_voxels, voxel_size):
    stride_i = math.ceil(img_shape[-1] / mlvl_feats.shape[-1])
    mlvl_feat_split = torch.split(mlvl_feats, 6, dim=1)

    volume_list = []
    for seq_id in range(len(mlvl_feat_split)):
        volumes = []
        for batch_id, seq_img_meta in enumerate(img_metas):
            feat_i = mlvl_feat_split[seq_id][batch_id] 
            img_meta = copy.deepcopy(seq_img_meta)
            img_meta["lidar2img"]["extrinsic"] = img_meta["lidar2img"]["extrinsic"][seq_id*6:(seq_id+1)*6]
            if isinstance(img_meta["img_shape"], list):
                img_meta["img_shape"] = img_meta["img_shape"][seq_id*6:(seq_id+1)*6]
                img_meta["img_shape"] = img_meta["img_shape"][0]
            height = math.ceil(img_meta["img_shape"][0] / stride_i)
            width = math.ceil(img_meta["img_shape"][1] / stride_i)

            projection = compute_projection(
                img_meta, stride_i, noise=0).to(feat_i.device)

            points = get_points(
                n_voxels=torch.tensor(n_voxels),
                voxel_size=torch.tensor(voxel_size),
                origin=torch.tensor(img_meta["lidar2img"]["origin"]),
            ).to(feat_i.device)

            volume = backproject_inplace(
                feat_i[:, :, :height, :width], points, projection) 
            volumes.append(volume)
        volume_list.append(torch.stack(volumes))
    mlvl_volumes = torch.cat(volume_list, dim=1)

    return mlvl_volumes


def backproject_inplace(features, points, projection):
    '''
    function: 2d feature + predefined point cloud -> 3d volume
    input:
        features: [6, 64, 225, 400]
        points: [3, 200, 200, 12]
        projection: [6, 3, 4]
    output:
        volume: [64, 200, 200, 12]
    '''
    n_images, n_channels, height, width = features.shape
    n_x_voxels, n_y_voxels, n_z_voxels = points.shape[-3:]
    # [3, 200, 200, 12] -> [1, 3, 480000] -> [6, 3, 480000]
    points = points.view(1, 3, -1).expand(n_images, 3, -1)
    # [6, 3, 480000] -> [6, 4, 480000]
    points = torch.cat((points, torch.ones_like(points[:, :1])), dim=1)
    # ego_to_cam
    # [6, 3, 4] * [6, 4, 480000] -> [6, 3, 480000]
    points_2d_3 = torch.bmm(projection, points)  # lidar2img
    x = (points_2d_3[:, 0] / points_2d_3[:, 2]).round().long()  # [6, 480000]
    y = (points_2d_3[:, 1] / points_2d_3[:, 2]).round().long()  # [6, 480000]
    z = points_2d_3[:, 2]  # [6, 480000]
    valid = (x >= 0) & (y >= 0) & (x < width) & (y < height) & (z > 0)  # [6, 480000]

    volume = torch.zeros(
        (n_channels, points.shape[-1]), device=features.device
    ).type_as(features)
    for i in range(n_images):
        volume[:, valid[i]] = features[i, :, y[i, valid[i]], x[i, valid[i]]]

    volume = volume.view(n_channels, n_x_voxels, n_y_voxels, n_z_voxels)
    return volume

@numba.jit(nopython=True)
def circle_nms(dets, thresh, post_max_size=83):
    x1 = dets[:, 0]
    y1 = dets[:, 1]
    scores = dets[:, 2]
    order = scores.argsort()[::-1].astype(np.int32)  # highest->lowest
    ndets = dets.shape[0]
    suppressed = np.zeros((ndets), dtype=np.int32)
    keep = []
    for _i in range(ndets):
        i = order[_i]  # start with highest score box
        if suppressed[
                i] == 1:  # if any box have enough iou with this, remove it
            continue
        keep.append(i)
        for _j in range(_i + 1, ndets):
            j = order[_j]
            if suppressed[j] == 1:
                continue
            # calculate center distance between i and j box
            dist = (x1[i] - x1[j])**2 + (y1[i] - y1[j])**2

            # ovr = inter / areas[j]
            if dist <= thresh:
                suppressed[j] = 1
    return keep[:post_max_size]


def box3d_multiclass_scale_nms(mlvl_bboxes,
                               mlvl_bboxes_for_nms,
                               mlvl_scores,
                               score_thr,
                               max_num,
                               cfg,
                               mlvl_dir_scores=None,
                               mlvl_attr_scores=None,
                               mlvl_bboxes2d=None):
    # do multi class nms
    # the fg class id range: [0, num_classes-1]
    num_classes = mlvl_scores.shape[1] - 1
    bboxes = []
    scores = []
    labels = []
    dir_scores = []
    attr_scores = []
    bboxes2d = []

    for i in range(0, num_classes):
        # get bboxes and scores of this class
        cls_inds = mlvl_scores[:, i] > score_thr
        if not cls_inds.any():
            continue

        _scores = mlvl_scores[cls_inds, i]
        _bboxes_for_nms = mlvl_bboxes_for_nms[cls_inds, :]
        _mlvl_bboxes = mlvl_bboxes[cls_inds, :]


        nms_radius_thre = cfg.nms_radius_thr_list[i]
        nms_rescale = cfg.nms_rescale_factor[i]
        _bboxes_for_nms[:, 2:4] *= nms_rescale

        _centers = _bboxes_for_nms[:, [0, 1]]
        _bboxes_for_nms = torch.cat([_centers, _scores.view(-1, 1)], dim=1)
        selected = circle_nms(_bboxes_for_nms.detach().cpu().numpy(), nms_radius_thre)
        selected = torch.tensor(selected, dtype=torch.long, device=_bboxes_for_nms.device)

        bboxes.append(_mlvl_bboxes[selected])
        scores.append(_scores[selected])
        cls_label = mlvl_bboxes.new_full((len(selected), ), i, dtype=torch.long)
        labels.append(cls_label)

        if mlvl_dir_scores is not None:
            _mlvl_dir_scores = mlvl_dir_scores[cls_inds]
            dir_scores.append(_mlvl_dir_scores[selected])
        if mlvl_attr_scores is not None:
            _mlvl_attr_scores = mlvl_attr_scores[cls_inds]
            attr_scores.append(_mlvl_attr_scores[selected])
        if mlvl_bboxes2d is not None:
            _mlvl_bboxes2d = mlvl_bboxes2d[cls_inds]
            bboxes2d.append(_mlvl_bboxes2d[selected])

    if bboxes:
        bboxes = torch.cat(bboxes, dim=0)
        scores = torch.cat(scores, dim=0)
        labels = torch.cat(labels, dim=0)
        if mlvl_dir_scores is not None:
            dir_scores = torch.cat(dir_scores, dim=0)
        if mlvl_attr_scores is not None:
            attr_scores = torch.cat(attr_scores, dim=0)
        if mlvl_bboxes2d is not None:
            bboxes2d = torch.cat(bboxes2d, dim=0)
        if bboxes.shape[0] > max_num:
            _, inds = scores.sort(descending=True)
            inds = inds[:max_num]
            bboxes = bboxes[inds, :]
            labels = labels[inds]
            scores = scores[inds]
            if mlvl_dir_scores is not None:
                dir_scores = dir_scores[inds]
            if mlvl_attr_scores is not None:
                attr_scores = attr_scores[inds]
            if mlvl_bboxes2d is not None:
                bboxes2d = bboxes2d[inds]
    else:
        bboxes = mlvl_scores.new_zeros((0, mlvl_bboxes.size(-1)))
        scores = mlvl_scores.new_zeros((0, ))
        labels = mlvl_scores.new_zeros((0, ), dtype=torch.long)
        if mlvl_dir_scores is not None:
            dir_scores = mlvl_scores.new_zeros((0, ))
        if mlvl_attr_scores is not None:
            attr_scores = mlvl_scores.new_zeros((0, ))
        if mlvl_bboxes2d is not None:
            bboxes2d = mlvl_scores.new_zeros((0, 4))

    results = (bboxes, scores, labels)

    if mlvl_dir_scores is not None:
        results = results + (dir_scores, )
    if mlvl_attr_scores is not None:
        results = results + (attr_scores, )
    if mlvl_bboxes2d is not None:
        results = results + (bboxes2d, )

    return results


def decode(anchors, deltas):
    """Apply transformation `deltas` (dx, dy, dz, dw, dh, dl, dr, dv*) to
    `boxes`.

    Args:
        anchors (torch.Tensor): Parameters of anchors with shape (N, 7).
        deltas (torch.Tensor): Encoded boxes with shape
            (N, 7+n) [x, y, z, w, l, h, r, velo*].

    Returns:
        torch.Tensor: Decoded boxes.
    """
    cas, cts = [], []
    xa, ya, za, wa, la, ha, ra, *cas = torch.split(anchors, 1, dim=-1)
    xt, yt, zt, wt, lt, ht, rt, *cts = torch.split(deltas, 1, dim=-1)

    za = za + ha / 2
    diagonal = torch.sqrt(la**2 + wa**2)
    xg = xt * diagonal + xa
    yg = yt * diagonal + ya
    zg = zt * ha + za

    lg = torch.exp(lt) * la
    wg = torch.exp(wt) * wa
    hg = torch.exp(ht) * ha
    rg = rt + ra
    zg = zg - hg / 2
    cgs = [t + a for t, a in zip(cts, cas)]
    return torch.cat([xg, yg, zg, wg, lg, hg, rg, *cgs], dim=-1)

class FastBEV(object):
    def __init__(self, args, cfg):
        super().__init__()
        self.target = args.target
        self.num = 1
        self.pre_model = RKNN(verbose=False)
        self.pre_model.load_rknn(args.pre_path)
        self.pre_model.init_runtime(target=self.target, device_id="172.16.4.60:5555", core_mask=RKNN.NPU_CORE_ALL, perf_debug=False)
        if args.eval_perf:
            for _ in range(self.num):
                self.pre_model.eval_perf(is_print=True)
        print("load pre model file from {}".format(args.pre_path))

        self.post_model = RKNN(verbose=False)
        self.post_model.load_rknn(args.post_path)
        self.post_model.init_runtime(target=self.target, device_id="172.16.4.60:5555", core_mask=RKNN.NPU_CORE_ALL, perf_debug=False)
        
        if args.eval_perf:    
            for _ in range(self.num):
                self.post_model.eval_perf(is_print=True)        
        print("load post model file from {}".format(args.post_path))

        self.nms_pre = 1000
        self.anchors = load_ten(args.anchors_file, return_torch=True) 
        cfg_n_voxels = cfg['model']['n_voxels']
        cfg_voxel_size = cfg['model']['voxel_size']
        self.cfg_n_voxels = cfg_n_voxels
        self.cfg_voxel_size = cfg_voxel_size
        self.data_config = cfg['data_config']
        self.nms_cfg = cfg['model']['test_cfg']
        self.nms_cfg.score_thr = 0.5

    def run(self, image, img_metas):
        # pre proc
        mlvl_feat = self.pre_model.inference([image], data_format='nchw')[0]

        # calc mlvl_volumes
        mlvl_feat = torch.from_numpy(mlvl_feat)
        img_shape = image.shape
        _, c_, h_, w_ = mlvl_feat.shape
        mlvl_feat = mlvl_feat.reshape(1, -1, c_, h_, w_ )
        mlvl_volumes = get_project_output(img_metas, mlvl_feat, img_shape, self.cfg_n_voxels[0], self.cfg_voxel_size[0])
        N, C, X, Y, Z = mlvl_volumes.shape
        mlvl_volumes = mlvl_volumes.permute(0, 2, 3, 4, 1).reshape(N, X, Y, Z*C).permute(0, 3, 1, 2)

        # post proc
        cls_score, bbox_pred, dir_cls_preds = self.post_model.inference([mlvl_volumes.numpy()], data_format='nchw')

        # decode bbox
        mlvl_scores = torch.tensor(cls_score)
        mlvl_bboxes = torch.tensor(bbox_pred)
        mlvl_dir_scores = torch.tensor(dir_cls_preds)

        max_scores, _ = mlvl_scores.max(dim=1)
        _, topk_inds = max_scores.topk(self.nms_pre)
        anchors = self.anchors[topk_inds, :]
        mlvl_bboxes = mlvl_bboxes[topk_inds, :]
        mlvl_scores = mlvl_scores[topk_inds, :]
        mlvl_dir_scores = mlvl_dir_scores[topk_inds]
        mlvl_bboxes = decode(anchors, mlvl_bboxes)

        # Add a dummy background class to the front when using sigmoid
        padding = mlvl_scores.new_zeros(mlvl_scores.shape[0], 1)
        mlvl_scores = torch.cat([mlvl_scores, padding], dim=1)
        score_thr = 0.5
        mlvl_bboxes_for_nms = mlvl_bboxes[:, [0, 1, 3, 4, 6]]

        bboxes, scores, labels, dir_scores = box3d_multiclass_scale_nms(mlvl_bboxes, mlvl_bboxes_for_nms,
                                                mlvl_scores, score_thr, self.nms_cfg.max_num,
                                                self.nms_cfg, mlvl_dir_scores)
        dir_offset = 0.7854
        dir_limit_offset = 0
        if bboxes.shape[0] > 0:
            dir_rot = limit_period(bboxes[..., 6] - dir_offset, dir_limit_offset, np.pi)
            bboxes[..., 6] = (dir_rot + dir_offset + np.pi * dir_scores.to(bboxes.dtype))
        return bboxes, scores, labels, dir_scores


    def sample_augmentation(self, H, W):
        fH, fW = self.data_config['test_input_size']
        resize = float(fW)/float(W)
        resize += self.data_config.get('test_resize', 0.0)
        resize_dims = (int(W * resize), int(H * resize))

        newW, newH = resize_dims
        crop_h_start = (newH - fH) // 2
        crop_w_start = (newW - fW) // 2
        crop = (crop_w_start, crop_h_start, crop_w_start + fW, crop_h_start + fH)

        flip = self.data_config['test_flip']
        rotate = self.data_config['test_rotate']

        pad_data = self.data_config['pad']
        pad_divisor = self.data_config['pad_divisor']
        pad_color = self.data_config['pad_color']
        pad = (pad_data, pad_color)

        return resize, resize_dims, crop, flip, rotate, pad

    def img_transform_core(self, img, resize_dims, crop, flip, rotate, pad):
        img = img.resize(resize_dims)
        img = img.crop(crop)
        if flip:
            img = img.transpose(method=Image.FLIP_LEFT_RIGHT)
        img = img.rotate(rotate)

        top, right, bottom, left = pad[0]
        pad_color = pad[1]
        img = self.img_pad(img, top, right, bottom, left, pad_color)
        return img
    
    def img_pad(self, pil_img, top, right, bottom, left, color):
        if top == right == bottom == left == 0:
            return pil_img
        assert top == bottom, (top, right, bottom, left)
        assert left == right, (top, right, bottom, left)
        width, height = pil_img.size
        new_width = width + right + left
        new_height = height + top + bottom
        result = Image.new(pil_img.mode, (new_width, new_height), color)
        result.paste(pil_img, (left, top))
        return result
    
    def read_images(self, img_meta):
        img_meta = img_meta[0]
        img_info = img_meta['img_info']
        img_norm = img_meta['img_norm_cfg']
        img_mean = img_norm['mean'] / 255
        img_std = img_norm['std'] / 255
        preprocess = transforms.Compose([
            transforms.ToTensor(),          
            transforms.Normalize(mean=img_mean, std=img_std)
        ])
        img_list = []
        for n in range(len(img_info)):
            filename = img_info[n]['filename']
            filename = "{}/{}".format(root, filename[1:])
            img_info[n]['filename'] = filename
            pil_img = Image.open(filename).convert("RGB")
            resize, resize_dims, crop, flip, rotate, pad = self.sample_augmentation(
                H=pil_img.height,
                W=pil_img.width,
            )
            post_pil_img = self.img_transform_core(pil_img, resize_dims, crop, flip, rotate, pad)
            img_ten = preprocess(post_pil_img)
            img_list.append(img_ten)
        return img_list     
    
    def release(self):
        self.pre_model.release()
        self.post_model.release()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rknn model test')
    parser.add_argument('--pre_path', type=str, help="fast bev pre model path", default='./examples/fast_bev/model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_pre_fp16.rknn')
    parser.add_argument('--post_path', type=str, help="fast bev post model path", default='./examples/fast_bev/model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_post_fp16.rknn')
    parser.add_argument('--anchors_file', type=str, help="anchor file path", default='./examples/fast_bev/model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/anchors.tensor')
    parser.add_argument('--meta_file', type=str, help="meta file path", default='./examples/fast_bev/model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/img_metas.pth')
    parser.add_argument('--config_file', type=str, help="config file path", default='./examples/fast_bev/python/configs/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1.py')
    
    parser.add_argument('--target', type=str, help="target platform", default='rk3588')  # required=True)
    parser.add_argument('--eval_perf', type=int, help="eval model performance", default=0)

    args = parser.parse_args()
    cfg = Config.fromfile(args.config_file)
    fast_bev = FastBEV(args, cfg)

    # read data
    img_metas = torch.load(args.meta_file, weights_only=False)
    img_list = fast_bev.read_images(img_metas)
    image_ten = torch.stack(img_list, dim=0)
    image_input = image_ten.numpy()

    # inference
    bboxes, scores, labels, dir_scores = fast_bev.run(image_input, img_metas)

    # save result
    vis_path = os.path.join(root, "sample0_vis_rknn.png")
    visual(img_metas, bboxes, scores, vis_path)

    # release rknn model
    fast_bev.release()
