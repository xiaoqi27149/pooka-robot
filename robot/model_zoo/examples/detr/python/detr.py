import numpy as np
import cv2
from rknn.api import RKNN
import argparse

OBJ_THRESH = 0.9

IMG_SIZE = [640, 640]

# COCO classes
CLASSES = [
    'N/A', 'person', 'bicycle', 'car', 'motorcycle', 'airplane', 'bus',
    'train', 'truck', 'boat', 'traffic light', 'fire hydrant', 'N/A',
    'stop sign', 'parking meter', 'bench', 'bird', 'cat', 'dog', 'horse',
    'sheep', 'cow', 'elephant', 'bear', 'zebra', 'giraffe', 'N/A', 'backpack',
    'umbrella', 'N/A', 'N/A', 'handbag', 'tie', 'suitcase', 'frisbee', 'skis',
    'snowboard', 'sports ball', 'kite', 'baseball bat', 'baseball glove',
    'skateboard', 'surfboard', 'tennis racket', 'bottle', 'N/A', 'wine glass',
    'cup', 'fork', 'knife', 'spoon', 'bowl', 'banana', 'apple', 'sandwich',
    'orange', 'broccoli', 'carrot', 'hot dog', 'pizza', 'donut', 'cake',
    'chair', 'couch', 'potted plant', 'bed', 'N/A', 'dining table', 'N/A',
    'N/A', 'toilet', 'N/A', 'tv', 'laptop', 'mouse', 'remote', 'keyboard',
    'cell phone', 'microwave', 'oven', 'toaster', 'sink', 'refrigerator', 'N/A',
    'book', 'clock', 'vase', 'scissors', 'teddy bear', 'hair drier',
    'toothbrush'
]


def letter_box(img, new_shape, pad_color=(0, 0, 0)):
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
    # img = cv2.resize(img, (IMG_SIZE[1], IMG_SIZE[0]))
    img, ratio, padding = letter_box(img.copy(), new_shape=[IMG_SIZE[1], IMG_SIZE[0]], pad_color=(0, 0, 0))

    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = np.array([img]).astype(np.float32)

    return img, ratio, padding


def draw(image, boxes, scores, classes):
    """Draw the boxes on the image.

    # Argument:
        image: original image.
        boxes: ndarray, boxes of objects.
        classes: ndarray, classes of objects.
        scores: ndarray, scores of objects.
        all_classes: all classes name.
    """
    print("{:^12} {:^12}  {}".format('class', 'score', 'xmin, ymin, xmax, ymax'))
    print('-' * 50)
    img_h, img_w = image.shape[:2]
    for box, score, cl in zip(boxes, scores, classes):
        top, left, right, bottom = box
        top = np.clip(int(top), 0, img_w - 1)
        left = np.clip(int(left), 0, img_h - 1)
        right = np.clip(int(right), 0, img_w - 1)
        bottom = np.clip(int(bottom), 0, img_h - 1)

        cv2.rectangle(image, (top, left), (right, bottom), (255, 0, 0), 2)
        cv2.putText(image, '{0} {1:.2f}'.format(CLASSES[cl], score),
                    (top, left - 6),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (0, 0, 255), 2)

        print("{:^12} {:^12.3f} [{:>4}, {:>4}, {:>4}, {:>4}]".format(CLASSES[cl], score, top, left, right, bottom))


def box_cxcywh_to_xyxy(x):
    x_c, y_c, w, h = np.split(x, 4, axis=1)
    b = np.concatenate([
        (x_c - 0.5 * w),
        (y_c - 0.5 * h),
        (x_c + 0.5 * w),
        (y_c + 0.5 * h)
    ], axis=1)
    return b


def postprocess(input_data, img_size, ratio, padding):
    probs = input_data[0]
    bbox = input_data[1]
    keep = probs.max(-1) > OBJ_THRESH
    bbox = bbox[0, keep]

    img_h, img_w = img_size
    b = box_cxcywh_to_xyxy(bbox)
    b = b * np.array([img_w, img_h, img_w, img_h], dtype=np.float32)
    p = input_data[0][keep]

    scores = np.max(p, 1)
    classes = np.argmax(p, 1)
    bbox = b * ratio - np.array([padding[0], padding[1], padding[0], padding[1]], dtype=np.float32)

    return bbox, classes, scores


class DETR():
    def __init__(self, args):
        self.model_path = args.model_path
        self.target = args.target
        self.img = args.img
        self.text = args.text

        self.rknn = RKNN(verbose=False)
        self.rknn.load_rknn(self.model_path)
        self.rknn.init_runtime(target=self.target, core_mask=RKNN.NPU_CORE_ALL)
        if args.eval_perf:
            for _ in range(10):
                self.rknn.eval_perf()

    def run(self):
        img, ratio, padding = img_preprocess(self.img)
        self.ratio = ratio
        self.padding = padding
        outputs = self.rknn.inference(inputs=[img])
        return outputs


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rknn model test')
    parser.add_argument('--model_path', type=str, help="detr model path", default='./detr/model/detr_r50_fp16.rknn')
    parser.add_argument('--target', type=str, help="target platform", default='rk3588')  # required=True)
    parser.add_argument('--img', type=str, help="input image", default='./detr/model/cat.jpg')
    parser.add_argument('--text', type=list, help="input text", default=[CLASSES])
    parser.add_argument('--eval_perf', type=int, help="eval model performance", default=0)

    args = parser.parse_args()
    detr = DETR(args)
    outputs = detr.run()

    # postprocess
    img = cv2.imread(args.img)
    boxes, classes, scores = postprocess(outputs, (IMG_SIZE[1], IMG_SIZE[0]), detr.ratio, detr.padding)

    if boxes is not None:
        draw(img, boxes, scores, classes)
        cv2.imwrite('result.jpg', img)
        print('Save results to result.jpg!')

    detr.rknn.release()
