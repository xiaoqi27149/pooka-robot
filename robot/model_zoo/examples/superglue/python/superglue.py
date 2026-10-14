import numpy as np
import cv2
from rknn.api import RKNN
import argparse
import torch
import matplotlib.cm as cm
import os
import sys
realpath = os.path.abspath(__file__)
_sep = os.path.sep
realpath = realpath.split(_sep)
root = _sep.join(realpath[:len(realpath) - realpath[::-1].index("python")])
sys.path.append(root)

from post_proc import simple_nms, sample_descriptors, make_matching_plot_fast, remove_borders
from post_proc import arange_like, log_optimal_transport

IMG_SIZE = [480, 640]

class SuperGlue(object):
    def __init__(self, args):

        self.target = args.target
        self.point_path = args.point_path
        self.glue_path = args.glue_path
        self.bin_score = 2.3457148

        self.point_rknn = RKNN(verbose=False)
        self.point_rknn.load_rknn(self.point_path)
        self.point_rknn.init_runtime(target=self.target, core_mask=RKNN.NPU_CORE_ALL)
        print("load point rknn file from {}".format(self.point_path))

        self.glue_rknn = RKNN(verbose=False)
        self.glue_rknn.load_rknn(self.glue_path)
        self.glue_rknn.init_runtime(target=self.target, core_mask=RKNN.NPU_CORE_ALL)
        print("load glue rknn file from {}".format(self.glue_path))

        if args.eval_perf:
            for _ in range(10):
                self.point_rknn.eval_perf()
            for _ in range(10):
                self.glue_rknn.eval_perf()  

    def run(self, img0, img1):
        # superpoint
        out0 = self.point_rknn.inference([img0])
        out1 = self.point_rknn.inference([img1])

        pred0 = self.point_post_proc(out0[0], out0[1])
        pred1 = self.point_post_proc(out1[0], out1[1])

        # superglue
        desc0 = pred0['descriptors'].numpy()
        kpts0 = pred0['keypoints'].numpy()
        scores0 = pred0['scores'].numpy()
        desc1 = pred1['descriptors'].numpy()
        kpts1 = pred1['keypoints'].numpy()
        scores1 = pred1['scores'].numpy()
        input_dict = {
            "desc0": desc0, "desc1": desc1, "kpts0": kpts0,
            "kpts1": kpts1, "scores0": scores0, "scores1": scores1
        }
        outputs = self.glue_rknn.inference([desc0, desc1, kpts0, kpts1, scores0, scores1])
        # superglue post proc
        indices0, indices1, mscores0, mscores1 = self.glue_post_proc(outputs[0])
        glue_out = {
            'matches0': indices0,  # use -1 for invalid match
            'matches1': indices1,  # use -1 for invalid match
            'matching_scores0': mscores0,
            'matching_scores1': mscores1,
        }

        out = {**input_dict, **glue_out}
        return out
    
    def release(self):
        self.glue_rknn.release()
        self.point_rknn.release()

    def point_post_proc(self, scores, descriptors):
        self.config = config['superpoint']
        scores = torch.from_numpy(scores)
        descriptors = torch.from_numpy(descriptors)
        b, _, h, w = scores.shape
        scores = scores.permute(0, 2, 3, 1).reshape(b, h, w, 8, 8)
        scores = scores.permute(0, 1, 3, 2, 4).reshape(b, h * 8, w * 8)
        scores = simple_nms(scores, self.config['nms_radius'])

        # Extract keypoints
        keypoints = [
            torch.nonzero(s > self.config['keypoint_threshold'])
            for s in scores]
        scores = [s[tuple(k.t())] for s, k in zip(scores, keypoints)]

        # Discard keypoints near the image borders
        keypoints, scores = list(zip(*[
            remove_borders(k, s, self.config['remove_borders'], h * 8, w * 8)
            for k, s in zip(keypoints, scores)]))

        # Convert (h, w) to (x, y)
        keypoints = [torch.flip(k, [1]).float() for k in keypoints]

        # Extract descriptors
        descriptors = [sample_descriptors(k[None], d[None], 8)[0]
                       for k, d in zip(keypoints, descriptors)]

        # align data
        align_num = self.config['align_point']
        kpt_num = keypoints[0].shape[0]
        if kpt_num >= align_num:
            keypoints[0] = keypoints[0][:align_num]
            scores = tuple([scores[0][:align_num]])
            descriptors[0] = descriptors[0][:, :align_num]
        else:
            kpt_aln = torch.zeros((align_num, 2))
            scr_aln = torch.zeros(align_num)
            des_aln = torch.zeros((256, align_num))
            kpt_aln[:kpt_num] = keypoints[0]
            scr_aln[:kpt_num] = scores[0]
            des_aln[:, :kpt_num] = descriptors[0]

            keypoints[0] = kpt_aln
            scores = tuple([scr_aln])
            descriptors[0] = des_aln

        return {
            'keypoints': torch.stack(keypoints),
            'scores': torch.stack(scores),
            'descriptors': torch.stack(descriptors),
        }

    def glue_post_proc(self, scores):
        scores = torch.from_numpy(scores)
        self.config = config['superglue']
        iters = self.config['sinkhorn_iterations']

        # Run the optimal transport.
        scores = log_optimal_transport(scores, torch.tensor(self.bin_score), iters=iters)
        # Get the matches with score above "match_threshold".
        max0, max1 = scores[:, :-1, :-1].max(2), scores[:, :-1, :-1].max(1)

        indices0, indices1 = max0.indices, max1.indices
        mutual0 = arange_like(indices0, 1)[None] == indices1.gather(1, indices0)
        mutual1 = arange_like(indices1, 1)[None] == indices0.gather(1, indices1)
        zero = scores.new_tensor(0)
        mscores0 = torch.where(mutual0, max0.values.exp(), zero)
        mscores1 = torch.where(mutual1, mscores0.gather(1, indices1), zero)
        valid0 = mutual0 & (mscores0 > self.config['match_threshold'])
        valid1 = mutual1 & valid0.gather(1, indices1)
        indices0 = torch.where(valid0, indices0, indices0.new_tensor(-1))
        indices1 = torch.where(valid1, indices1, indices1.new_tensor(-1))

        return indices0.numpy(), indices1.numpy(), mscores0.numpy(), mscores1.numpy()
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rknn model test')
    parser.add_argument('--point_path', type=str, help="superpoint model path", default='./superglue/model/superpoint_int8.rknn')
    parser.add_argument('--glue_path', type=str, help="superglue model path", default='./superglue//model/superglue_fp16.rknn')

    parser.add_argument('--target', type=str, help="target platform", default='rk3588')  # required=True)
    parser.add_argument('--img0', type=str, help="input image0", default='./superglue/model/scene0711_00_frame-001680.jpg')
    parser.add_argument('--img1', type=str, help="input image1", default='./superglue/model/scene0711_00_frame-001995.jpg')

    parser.add_argument('--eval_perf', type=int, help="eval model performance", default=0)

    args = parser.parse_args()
    config = {
        'superglue': {'match_threshold': 0.2, 'sinkhorn_iterations': 20, 'weights': 'indoor'},
        'superpoint': {'keypoint_threshold': 0.005, 'max_keypoints': -1,
                       'nms_radius': 4, 'remove_borders': 4, 'align_point': 384}
    }

    superglue = SuperGlue(args)
    
    # read images
    img0 = cv2.imread(args.img0, 0)
    img1 = cv2.imread(args.img1, 0)
    img0 = cv2.resize(img0, (IMG_SIZE[1], IMG_SIZE[0]))
    img1 = cv2.resize(img1, (IMG_SIZE[1], IMG_SIZE[0]))
    img0_ten = np.float32(img0[None, :, :, None])
    img1_ten = np.float32(img1[None, :, :, None])

    # inference
    prediction = superglue.run(img0_ten, img1_ten)

    # visual result
    kpts0 = prediction['kpts0'][0]
    kpts1 = prediction['kpts1'][0]
    matches = prediction['matches0'][0]
    confidence = prediction['matching_scores0'][0]
    valid = matches > -1
    mkpts0 = kpts0[valid]
    mkpts1 = kpts1[matches[valid]]
    color = cm.jet(confidence[valid])
    text = [
        'SuperGlue',
        'Keypoints: {}:{}'.format(len(kpts0), len(kpts1)),
        'Matches: {}'.format(len(mkpts0))
    ]
    k_thresh = config['superpoint']['keypoint_threshold']
    m_thresh = config['superglue']['match_threshold']
    small_text = [
        'Keypoint Threshold: {:.4f}'.format(k_thresh),
        'Match Threshold: {:.2f}'.format(m_thresh),
        'Image Pair: {:06}:{:06}'.format(0, 1),
    ]
    out = make_matching_plot_fast(
        img0, img1, kpts0, kpts1, mkpts0, mkpts1, color, text,
        path=None, show_keypoints=True, small_text=small_text)

    cv2.imwrite('result.jpg', out)
    print('Save results to result.jpg!')

    superglue.release()
