import numpy as np
import cv2
from rknn.api import RKNN
import argparse
import itertools
from transformers import AutoTokenizer
import os
import torch
import torch.nn.functional as F

OBJ_THRESH = 0.25
NMS_THRESH = 0.45

IMG_SIZE = [640, 640]
SEQUENCE_LEN = 20
PAD_VALUE = 49407

CLASSES = ('persimmon', 'banana', 'yellow banana', 'strawberry', 'red strawberry', 'yellow towel', 'towel', 'egg', 'blue plate',
 'grape', 'yellow block', 'green block', 'red direction', 'small block', 'fruit', 'food', 'person', 'bus', '', '')

def text_tokenizer(text, model_name):
    tokenizer = AutoTokenizer.from_pretrained(model_name)
    text = list(itertools.chain(*text))
    text = tokenizer(text=text, return_tensors='pt', padding=True)

    return np.array(text['input_ids'])

def letter_box(img, new_shape, pad_color=(0,0,0)):
    # Resize and pad image while meeting stride-multiple constraints
    shape = img.shape[:2]  # current shape [height, width]
    if isinstance(new_shape, int):
        new_shape = (new_shape, new_shape)

    # Scale ratio
    r = min(new_shape[0] / shape[0], new_shape[1] / shape[1])

    # Compute padding
    ratio = r  # width, height ratios
    new_unpad = int(round(shape[1] * r)), int(round(shape[0] * r))
    dw, dh = new_shape[1] - new_unpad[0], new_shape[0] - new_unpad[1]  # wh padding

    dw /= 2  # divide padding into 2 sides
    dh /= 2

    if shape[::-1] != new_unpad:  # resize
        img = cv2.resize(img, new_unpad, interpolation=cv2.INTER_LINEAR)
    top, bottom = int(round(dh - 0.1)), int(round(dh + 0.1))
    left, right = int(round(dw - 0.1)), int(round(dw + 0.1))
    img = cv2.copyMakeBorder(img, top, bottom, left, right, cv2.BORDER_CONSTANT, value=pad_color)  # add border

    return img, ratio, (dw, dh)

def img_preprocess(img_path):
    img = cv2.imread(img_path)
    img, _, _ = letter_box(img.copy(), new_shape=[IMG_SIZE[0], IMG_SIZE[1]], pad_color=(0, 0, 0))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = np.array([img]).astype(np.float32)

    return img

class Colors:
    # Ultralytics color palette https://ultralytics.com/
    def __init__(self):
        # hex = matplotlib.colors.TABLEAU_COLORS.values()
        hexs = ('FF3838', 'FF9D97', 'FF701F', 'FFB21D', 'CFD231', '48F90A', '92CC17', '3DDB86', '1A9334', '00D4BB',
                '2C99A8', '00C2FF', '344593', '6473FF', '0018EC', '8438FF', '520085', 'CB38FF', 'FF95C8', 'FF37C7')
        self.palette = [self.hex2rgb(f'#{c}') for c in hexs]
        self.n = len(self.palette)

    def __call__(self, i, bgr=False):
        c = self.palette[int(i) % self.n]
        return (c[2], c[1], c[0]) if bgr else c

    @staticmethod
    def hex2rgb(h):  # rgb order (PIL)
        return tuple(int(h[1 + i:1 + i + 2], 16) for i in (0, 2, 4))

def sigmoid(x):
    return 1 / (1 + np.exp(-x))

def filter_boxes(boxes, box_confidences, box_class_probs, masks):
    """Filter boxes with object threshold.
    """
    box_confidences = box_confidences.reshape(-1)
    candidate, class_num = box_class_probs.shape

    class_max_score = np.max(box_class_probs, axis=-1)
    classes = np.argmax(box_class_probs, axis=-1)

    _class_pos = np.where(class_max_score * box_confidences >= OBJ_THRESH)
    scores = (class_max_score * box_confidences)[_class_pos]

    boxes = boxes[_class_pos]
    classes = classes[_class_pos]
    masks = (masks * box_confidences.reshape(-1, 1))[_class_pos]
    return boxes, classes, scores, masks


def nms_boxes(boxes, scores):
    """Suppress non-maximal boxes.
    # Returns
        keep: ndarray, index of effective boxes.
    """
    x = boxes[:, 0]
    y = boxes[:, 1]
    w = boxes[:, 2] - boxes[:, 0]
    h = boxes[:, 3] - boxes[:, 1]

    areas = w * h
    order = scores.argsort()[::-1]

    keep = []
    while order.size > 0:
        i = order[0]
        keep.append(i)

        xx1 = np.maximum(x[i], x[order[1:]])
        yy1 = np.maximum(y[i], y[order[1:]])
        xx2 = np.minimum(x[i] + w[i], x[order[1:]] + w[order[1:]])
        yy2 = np.minimum(y[i] + h[i], y[order[1:]] + h[order[1:]])

        w1 = np.maximum(0.0, xx2 - xx1 + 0.00001)
        h1 = np.maximum(0.0, yy2 - yy1 + 0.00001)
        inter = w1 * h1

        ovr = inter / (areas[i] + areas[order[1:]] - inter)
        inds = np.where(ovr <= NMS_THRESH)[0]
        order = order[inds + 1]
    keep = np.array(keep)
    return keep


def dfl(position):
    x = np.array(position) 
    n, c, h, w = x.shape
    p_num = 4  
    mc = c // p_num 

    y = x.reshape(n, p_num, mc, h, w)

    y = np.exp(y - np.max(y, axis=2, keepdims=True))  
    y = y / np.sum(y, axis=2, keepdims=True)

    acc_matrix = np.arange(mc, dtype=np.float32).reshape(1, 1, mc, 1, 1)

    y = np.sum(y * acc_matrix, axis=2)

    return y


def box_process(position, hgt, wid):
    grid_h, grid_w = position.shape[2:4]
    col, row = np.meshgrid(np.arange(0, grid_w), np.arange(0, grid_h))
    col = col.reshape(1, 1, grid_h, grid_w)
    row = row.reshape(1, 1, grid_h, grid_w)
    grid = np.concatenate((col, row), axis=1)
    stride = np.array([hgt // grid_h, wid // grid_w]).reshape(1, 2, 1, 1)

    position = dfl(position)
    box_xy = grid + 0.5 - position[:, 0:2, :, :]
    box_xy2 = grid + 0.5 + position[:, 2:4, :, :]
    xyxy = np.concatenate((box_xy * stride, box_xy2 * stride), axis=1)

    return xyxy


def sp_flatten(_in):
    ch = _in.shape[1]
    _in = _in.transpose(0, 2, 3, 1)
    return _in.reshape(-1, ch)


def sigmoid(x):
    return 1 / (1 + np.exp(-x))


def post_process(input_data, hgt, wid):
    boxes, scores, classes_conf, masks = [], [], [], []
    defualt_branch = 3
    pair_per_branch = len(input_data) // defualt_branch
    # Python 忽略 score_sum 输出
    for i in range(defualt_branch):
        boxes.append(box_process(input_data[pair_per_branch * i], hgt, wid))
        classes_conf.append(input_data[pair_per_branch * i + 1])
        scores.append(np.ones_like(input_data[pair_per_branch * i + 1][:, :1, :, :], dtype=np.float32))
        masks.append(input_data[pair_per_branch * i + 3])

    boxes = [sp_flatten(_v) for _v in boxes]
    classes_conf = [sp_flatten(_v) for _v in classes_conf]
    scores = [sp_flatten(_v) for _v in scores]
    masks = [sp_flatten(_v) for _v in masks]
    proto = input_data[-1]
    ph, pw = proto.shape[-2:]
    proto = proto.reshape(proto.shape[1], -1)

    boxes = np.concatenate(boxes)
    classes_conf = np.concatenate(classes_conf)
    scores = np.concatenate(scores)
    masks = np.concatenate(masks)

    # filter according to threshold
    boxes, classes, scores, masks = filter_boxes(boxes, scores, classes_conf, masks)

    # nms
    nboxes, nclasses, nscores, nmasks = [], [], [], []
    for c in set(classes):
        inds = np.where(classes == c)
        b = boxes[inds]
        c = classes[inds]
        s = scores[inds]
        m = masks[inds]
        keep = nms_boxes(b, s)

        if len(keep) != 0:
            nboxes.append(b[keep])
            nclasses.append(c[keep])
            nscores.append(s[keep])

            mask_vect = m[keep]
            nmask = np.matmul(mask_vect, proto)
            nmask = nmask.reshape(-1, ph, pw).transpose(1, 2, 0)
            # nmask = sigmoid(nmask)
            nmask = np.maximum(nmask, 0)
            nmask = cv2.resize(nmask, [wid, hgt])
            nmasks.append(nmask)

    return nboxes, nclasses, nscores, nmasks

def draw(image, boxes, scores, classes, masks, alpha=0.7):
    print("{:^12} {:^12}  {}".format('class', 'score', 'xmin, ymin, xmax, ymax'))
    print('-' * 50)

    # 创建带掩膜的可视化图像
    mask_layer = image.copy()
    output = image.copy()

    cls_num = len(boxes)
    for k in range(cls_num):
        box_num = len(boxes[k])
        for m in range(box_num):
            score = scores[k][m]
            cl = int(classes[k][m])  # 确保类别是整数
            bbox = np.int32(boxes[k][m])
            top, left, right, bottom = np.maximum(bbox, 0)

            # 生成随机颜色 (按类别固定颜色更佳)
            color = np.random.randint(0, 255, 3).tolist()

            # 获取当前物体的掩膜 (处理单通道和多通道情况)
            if box_num > 1:
                mask = masks[k][:, :, m]
            else:
                mask = masks[k]

            crop_mask = mask[left:bottom, top:right]
            crop_mask = crop_mask / np.max(crop_mask)
            crop_mask = crop_mask[:, :, np.newaxis]


            colr_mask = crop_mask * color
            colored_mask = crop_mask * colr_mask + image[left:bottom, top:right] * (1 - crop_mask)
            output[left:bottom, top:right] = colored_mask * alpha + (1 - alpha) * output[left:bottom, top:right]

            # 2. 绘制边界框
            cv2.rectangle(output, (top, left), (right, bottom), color, 2)

            # 3. 绘制类别标签和置信度
            label = f"{CLASSES[cl]} {score:.2f}"
            (text_w, text_h), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.6, 2)
            cv2.rectangle(output, (top, left - text_h - 10), (top + text_w, left), color, -1)
            cv2.putText(output, label, (top, left - 5),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)

            print("{:^12} {:^12.3f} [{:>4}, {:>4}, {:>4}, {:>4}]".format(
                CLASSES[cl], score, top, left, right, bottom))

    return output

class YOLOE():
    def __init__(self, args):
        self.text_model = args.text_model
        self.yoloe = args.yoloe
        self.target = args.target
        self.img = args.img
        self.text = args.text
        self.save_text_outputs = args.save_text_outputs

        self.yoloe_eng = RKNN(verbose=False)
        self.yoloe_eng.load_rknn(self.yoloe)
        self.yoloe_eng.init_runtime(target=self.target, core_mask=RKNN.NPU_CORE_ALL)

    def clip_text_run(self):
        input_ids = text_tokenizer(self.text, "openai/clip-vit-base-patch32")
        text_num, seq_len = input_ids.shape
        if seq_len >= SEQUENCE_LEN:
            input_data = input_ids[:, :SEQUENCE_LEN]
        else:
            input_data = np.zeros((text_num, SEQUENCE_LEN)).astype(np.float32)
            input_data[:, :seq_len] = input_ids
            input_data[:, seq_len:] = PAD_VALUE

        rknn = RKNN(verbose=False)
        rknn.load_rknn(self.text_model)
        rknn.init_runtime(target=self.target, core_mask=RKNN.NPU_CORE_ALL)
        outputs = []
        for i in range(text_num):
            text_feat = rknn.inference(inputs=[input_data[i:i+1, :]])[0]
            outputs.append(text_feat)
        outputs = np.concatenate(outputs, axis=1)

        if args.save_text_outputs:
            np.save('../model/clip_feats.npy', outputs)

        if args.eval_perf:
            for _ in range(10):
                rknn.eval_perf()
            for _ in range(10):
                self.yoloe_eng.eval_perf()
        
        rknn.release()

        return outputs

    def run_images(self, img_file):
        text_outputs = self.clip_text_run()
        img = img_preprocess(img_file)
        outputs = self.yoloe_eng.inference(inputs=[img, text_outputs])

        # postprocess
        boxes, classes, scores, masks = post_process(outputs, img.shape[1], img.shape[2])
        
        # visual
        if boxes is not None:
            img = np.uint8(img[0])
            vis_img = draw(img, boxes, scores, classes, masks)
            vis_img = cv2.cvtColor(vis_img, cv2.COLOR_RGB2BGR)
            cv2.imwrite("result.png", vis_img)
            print('Save results to result.png!')

        return outputs

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rknn model test')
    parser.add_argument('--text_model', type=str, help="clip_text model path", default='../model/mobileclip.rknn')
    parser.add_argument('--yoloe', type=str, help="yoloe model path", default='../model/yoloe-11m-seg_fp16.rknn')
    parser.add_argument('--target', type=str, help="target platform", default='rk3588') #required=True)
    parser.add_argument('--img', type=str, help="input image", default='../model/bus.jpg')
    parser.add_argument('--text', type=list, help="input text", default=[CLASSES])
    parser.add_argument('--save_text_outputs', type=bool, help="save text outputs for yoloe quant dataset", default=False)
    parser.add_argument('--eval_perf', type=int, help="eval rknn performance", default=0)

    args = parser.parse_args()
    yoloe = YOLOE(args)

    yoloe.run_images(args.img)

