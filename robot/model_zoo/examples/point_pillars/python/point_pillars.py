
import numpy as np
import cv2
from rknn.api import RKNN
import argparse
import torch
import os
import sys
realpath = os.path.abspath(__file__)
_sep = os.path.sep
realpath = realpath.split(_sep)
root = _sep.join(realpath[:len(realpath) - realpath[::-1].index("python")])
sys.path.append(root)

from utils.voxelize import VoxelGenerator
from utils.utils import py_cpu_nms
from utils.anchor_3d_generator import Anchor3DRangeGenerator
import open3d as o3d
from utils.show_lidar_box import show_box


class PointPillar(object):
    def __init__(self, args):
        super().__init__()
        self.target = args.target
        
        self.encoder = RKNN(verbose=False)
        self.encoder.load_rknn(args.encoder_path)
        self.encoder.init_runtime(target=self.target, core_mask=RKNN.NPU_CORE_ALL, perf_debug=False)
        print("load voxel encoder file from {}".format(args.encoder_path))

        self.head = RKNN(verbose=False)
        self.head.load_rknn(args.head_path)
        self.head.init_runtime(target=self.target, core_mask=RKNN.NPU_CORE_ALL, perf_debug=False)
        print("load head file from {}".format(args.head_path))

        # pre process param
        point_cloud_range = [0, -39.68, -3, 69.12, 39.68, 1]

        voxel_size = [0.16, 0.16, 4]
        max_points = 32
        self.max_voxels = 20000
        self.voxel_model = VoxelGenerator(voxel_size, point_cloud_range, max_points, self.max_voxels)

        # post process param
        self.box_code_size = 7
        self.num_classes = 1
        self.nms_pre = 100
        self.max_num = 50
        self.score_thr = 0.5
        self.nms_thr = 0.01

        self.anchor3d_generater = Anchor3DRangeGenerator(
            [[0, -39.68, -1.78, 69.12, 39.68, -1.78]],
            sizes=[[3.9, 1.6, 1.56]],
            scales=[1],
            rotations=[0, 1.57],
            reshape_out=True,
            size_per_range=True
        )
        self.mlvl_priors = self.anchor3d_generater.grid_anchors([(248, 216)], device="cpu")
        self.mlvl_priors = [prior.reshape(-1, self.box_code_size) for prior in self.mlvl_priors]

        if args.eval_perf:
            num = 1
            for _ in range(num):
                self.encoder.eval_perf(is_print=True)
            for _ in range(num):
                self.head.eval_perf(is_print=True)


    def pre_process(self, points):
        voxel_dict = self.voxel_model.generate(points, self.max_voxels)
        return voxel_dict

    def xywhr2xyxyr(self, boxes_xywhr):
        boxes = torch.zeros_like(boxes_xywhr)
        half_w = boxes_xywhr[..., 2] / 2
        half_h = boxes_xywhr[..., 3] / 2
        boxes[..., 0] = boxes_xywhr[..., 0] - half_w
        boxes[..., 1] = boxes_xywhr[..., 1] - half_h
        boxes[..., 2] = boxes_xywhr[..., 0] + half_w
        boxes[..., 3] = boxes_xywhr[..., 1] + half_h
        boxes[..., 4] = boxes_xywhr[..., 4]
        return boxes

    def box3d_multiclass_nms(self, mlvl_bboxes, mlvl_bboxes_for_nms, mlvl_scores, mlvl_dir_scores):
        num_classes = mlvl_scores.shape[1] - 1
        bboxes = []
        scores = []
        labels = []
        dir_scores = []
        for i in range(0, num_classes):
            cls_inds = mlvl_scores[:, i] > self.score_thr
            if not cls_inds.any():
                continue
            _scores = mlvl_scores[cls_inds, i]
            _bboxes_for_nms = mlvl_bboxes_for_nms[cls_inds, :]
            selected = py_cpu_nms(_bboxes_for_nms, _scores, self.nms_thr)
            bboxes.append(mlvl_bboxes[selected])
            scores.append(_scores[selected])
            cls_label = mlvl_bboxes.new_full((len(selected),), i, dtype=torch.long)
            labels.append(cls_label)
            dir_scores.append(mlvl_dir_scores[selected])
        if bboxes:
            bboxes = torch.cat(bboxes, dim=0)
            scores = torch.cat(scores, dim=0)
            labels = torch.cat(labels, dim=0)
            dir_scores = torch.cat(dir_scores, dim=0)
            if bboxes.shape[0] > self.max_num:
                _, inds = scores.sort(descending=True)
                inds = inds[:self.max_num]
                bboxes = bboxes[inds, :]
                labels = labels[inds]
                scores = scores[inds]
                dir_scores = dir_scores[inds]
        else:
            bboxes = mlvl_scores.new_zeros((0, mlvl_bboxes.size(-1)))
            scores = mlvl_scores.new_zeros((0,))
            labels = mlvl_scores.new_zeros((0,), dtype=torch.long)
            dir_scores = mlvl_scores.new_zeros((0,))
        return bboxes, scores, labels, dir_scores

    def decode(self, anchors, deltas):
        xa, ya, za, wa, la, ha, ra = torch.split(anchors, 1, dim=-1)
        xt, yt, zt, wt, lt, ht, rt = torch.split(deltas, 1, dim=-1)
        za = za + ha / 2
        diagonal = torch.sqrt(la ** 2 + wa ** 2)
        xg = xt * diagonal + xa
        yg = yt * diagonal + ya
        zg = zt * ha + za
        lg = torch.exp(lt) * la
        wg = torch.exp(wt) * wa
        hg = torch.exp(ht) * ha
        rg = rt + ra
        zg = zg - hg / 2
        return torch.cat([xg, yg, zg, wg, lg, hg, rg], dim=-1)

    def obb2aabb(self, pcd, obboxes):
        aabb_bev_boxes = []
        for data in obboxes:
            center = data[:3]
            extent = data[3: 6]
            # 定义旋转轴和旋转角度（使用弧度）
            axis = np.array([0, 0, data[6]])  # 例如，绕x轴旋转
            # 从轴角获取旋转矩阵
            R = o3d.geometry.get_rotation_matrix_from_xyz(axis)
            obbox = o3d.geometry.OrientedBoundingBox(center=center, R=R, extent=extent)
            inside_points = pcd.select_by_index(obbox.get_point_indices_within_bounding_box(pcd.points))
            box = inside_points.get_axis_aligned_bounding_box()
            aabb_center = box.get_center().tolist()
            aabb_scale = box.get_extent().tolist()
            bev_box = aabb_center[:2] + aabb_scale[:2] + [data[6]]
            aabb_bev_boxes.append(bev_box)
        return torch.tensor(aabb_bev_boxes)

    def predict_by_feat_single(self, pcd, cls_score, bbox_pred, dir_cls_pred):
        priors = self.mlvl_priors[0]
        dir_cls_pred = dir_cls_pred.permute(1, 2, 0).reshape(-1, 2)
        dir_cls_scores = torch.max(dir_cls_pred, dim=-1)[1]
        cls_score = cls_score.permute(1, 2, 0).reshape(-1, self.num_classes)
        scores = cls_score.sigmoid()
        bbox_pred = bbox_pred.permute(1, 2, 0).reshape(-1, self.box_code_size)
        max_scores, _ = scores.max(dim=1)
        _, topk_inds = max_scores.topk(self.nms_pre)
        priors = priors[topk_inds, :].cpu()
        bbox_pred = bbox_pred[topk_inds, :]
        scores = scores[topk_inds, :]
        dir_cls_scores = dir_cls_scores[topk_inds]
        bboxes = self.decode(priors, bbox_pred)
        aabboxes = self.obb2aabb(pcd, bboxes)
        # mlvl_bboxes_bev = torch.cat([bboxes[:, 0:2], bboxes[:, 3:5], bboxes[:, 6:]], dim=1)
        mlvl_bboxes_for_nms = self.xywhr2xyxyr(aabboxes)
        padding = scores.new_zeros(scores.shape[0], 1)
        scores = torch.cat([scores, padding], dim=1)
        results = self.box3d_multiclass_nms(bboxes, mlvl_bboxes_for_nms, scores, dir_cls_scores)
        bboxes, scores, labels, dir_scores = results
        if bboxes.shape[0] > 0:
            dir_rot = bboxes[..., 6] + np.pi / 2 - torch.floor(bboxes[..., 6] + np.pi / 2 / np.pi) * np.pi
            bboxes[..., 6] = (dir_rot - np.pi / 2 + np.pi * dir_scores.to(bboxes.dtype))
        return bboxes, scores, labels

    def run(self, points):
        # pre proc
        voxel_dict = self.pre_process(points)

        # onnx inference
        canvas = voxel_dict['canvas']
        indices = voxel_dict['indices']
        features = np.float32(voxel_dict['feat'])

        num = len(indices)
        max_num = 20000
        feat_pad = np.zeros((max_num, 32, 10), dtype=np.float32)
        ind_pad = np.zeros((max_num), dtype=np.int32)
        feat_pad[:num] = features[:num]
        ind_pad[:num] = indices[:num]
        features = feat_pad
        indices = ind_pad
        print("padding voxel to {}, valid voxel num {}".format(max_num, num))

        # inference
        voxel = self.encoder.inference([features])[0]

        voxels = voxel.T
        canvas[:, indices] = voxels
        x = canvas.reshape(1, 64, 496, 432)

        results = self.head.inference([x], data_format='nchw')

        # post proc
        xyz = points[:, :3]
        pcd = o3d.geometry.PointCloud()
        pcd.points = o3d.utility.Vector3dVector(xyz)
        cls_score = torch.from_numpy(results[0]).squeeze()
        bbox_pred = torch.from_numpy(results[1]).squeeze()
        dir_cls_pred =  torch.from_numpy(results[2]).squeeze()
        result = self.predict_by_feat_single(pcd, cls_score, bbox_pred, dir_cls_pred)

        print(result[0])
        save_file = 'pcd_ret.png'
        show_box(points, result[0].numpy(), save_file)
    
    def release(self):
        self.encoder.release()
        self.head.release()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rknn model test')
    parser.add_argument('--encoder_path', type=str, help="point pillar model path", default='./point_pillars/model/pointpillar_voxel_encoder_fp16.rknn')
    parser.add_argument('--head_path', type=str, help="point pillar model path", default='./point_pillars/model/pointpillar_head_fp16.rknn')

    parser.add_argument('--target', type=str, help="target platform", default='rk3588')  # required=True)
    parser.add_argument('--pcd', type=str, help="point clound file", default='./point_pillars/model/006867.bin')

    parser.add_argument('--eval_perf', type=int, help="eval model performance", default=1)

    args = parser.parse_args()

    point_pillars = PointPillar(args)

    # read point clound file
    points = np.fromfile(args.pcd, dtype=np.float32, count=-1).reshape([-1, 4])

    # inference
    point_pillars.run(points)


    point_pillars.release()
