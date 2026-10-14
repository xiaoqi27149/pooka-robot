import os
import argparse
import numpy as np
import cv2
from rknn.api import RKNN
from scipy.special import softmax


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rknn model test')
    parser.add_argument('--model', type=str, help="model path", default='./model/vit_b_16_fp16.rknn')
    # parser.add_argument('--model', type=str, help="model path", default='./model/fastvit_t8_fp16.rknn')
    parser.add_argument('--target', type=str, help="target platform", default='rk3588')
    parser.add_argument('--img', type=str, help="input image", default='./model/dog_224x224.jpg')
    parser.add_argument('--text', type=str, help="input text", default='./model/labels.txt')
    parser.add_argument('--ip', type=str, help="device ip", default='172.16.4.106:5555')

    args = parser.parse_args()
    with open(args.text, 'r', encoding='utf-8') as f:
        CLASSES = [line.strip() for line in f]  

    img = cv2.imread(args.img)
    img = cv2.resize(img, (224, 224))
    img = np.expand_dims(img, 0)
    # Init runtime environment
    print('--> Init runtime environment')
    rknn = RKNN(verbose=False)
    rknn.load_rknn(args.model)
    ret = rknn.init_runtime(target=args.target, device_id=args.ip, perf_debug=False, core_mask=RKNN.NPU_CORE_ALL)

    if ret != 0:
        print('Init runtime environment failed!')
        exit(ret)
    print('done')

    # Eval Perf
    for _ in range(10):
        print('--> Eval Perf')
        rknn.eval_perf()
    print('done')

    # Inference
    print('--> Running model')
    outputs = rknn.inference(inputs=[img])

    # Post Process
    scores = softmax(outputs[0])
    # print the top-5 inferences class
    scores = np.squeeze(scores)
    a = np.argsort(scores)[::-1]
    print('-----TOP 5-----')
    for i in a[0:5]:
        print('[%d] score=%.2f class="%s"' % (i, scores[i], CLASSES[i]))
    print('done')

    # Release
    rknn.release()


