import torch
from torch import nn
import os
import sys
from rknn.api import RKNN

# add path
realpath = os.path.abspath(__file__)
_sep = os.path.sep
realpath = realpath.split(_sep)
DATASET_DINOV3B = _sep.join(realpath[:len(realpath) - realpath[::-1].index("examples") ]) + '/vit/model/dataset-vitb16.txt'
DATASET_DINOV3S = _sep.join(realpath[:len(realpath) - realpath[::-1].index("examples") ]) + '/vit/model/dataset-vits16.txt'
DATASET_CONVNEXT = _sep.join(realpath[:len(realpath) - realpath[::-1].index("examples") ]) + '/vit/model/dataset.txt'
DATASET = _sep.join(realpath[:len(realpath) - realpath[::-1].index("examples") - 1]) + '/datasets/imagenet/ILSVRC2012_img_val_samples/dataset_20.txt'

DEFAULT_QUANT = True
DEFAULT_RKNN_PATH = './model/vit_model.rknn'

def parse_arg():
    if len(sys.argv) < 3:
        print("Usage: python3 {} onnx_model_path [platform] [dtype(optional)] [output_rknn_path(optional)]".format(
            sys.argv[0]))
        print("       platform choose from [rk3562, rk3566, rk3568, rk3576, rk3588, rv1126b]")
        print("       dtype choose from    [fp, i8]")
        exit(1)

    model_path = sys.argv[1]
    platform = sys.argv[2]

    do_quant = DEFAULT_QUANT
    if len(sys.argv) > 3:
        model_type = sys.argv[3]
        if model_type not in ['i8', 'fp']:
            print("ERROR: Invalid model type: {}".format(model_type))
            exit(1)
        elif model_type in ['i8']:
            do_quant = True
        else:
            do_quant = False

    if len(sys.argv) > 4:
        output_path = sys.argv[4]
    else:
        model_name, ext = os.path.splitext(model_path)
        if do_quant:
            output_path = "{}_int8.rknn".format(model_name)
        else:
            output_path = "{}_fp16.rknn".format(model_name)

    return model_path, platform, do_quant, output_path


def export_rknn(model_path, platform, do_quant, output_path):
    # Create RKNN object
    rknn = RKNN(verbose=True)

    # Pre-process config
    print('--> Config model')
    rknn.config(mean_values=[[255*0.485, 255*0.456, 255*0.406]],
                std_values=[[255*0.229, 255*0.224, 255*0.225]],
                target_platform=platform)
    print('done')

    # Load model
    print('--> Loading model')
    print(model_path)
    ret = rknn.load_onnx(model=model_path)
    if ret != 0:
        print('Load model failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')

    if 'dinov3' in model_path:
        if 'vitb' in model_path:
            print("do_quantization for dinov3 vitb...")
            ret = rknn.build(do_quantization=do_quant, dataset=DATASET_DINOV3B)
        elif 'vits' in model_path:
            print("do_quantization for dinov3 vits...")
            ret = rknn.build(do_quantization=do_quant, dataset=DATASET_DINOV3S)
        else:
            print("do_quantization for dinov3 convnext...")
            ret = rknn.build(do_quantization=do_quant, dataset=DATASET_CONVNEXT)
    else:
        ret = rknn.build(do_quantization=do_quant, dataset=DATASET)
    if ret != 0:
        print('Build model failed!')
        exit(ret)

    # Export rknn model
    print('--> Export rknn model to {}'.format(output_path))
    ret = rknn.export_rknn(output_path)
    if ret != 0:
        print('Export rknn model failed!')
        exit(ret)

    print('done')
    rknn.release()


if __name__ == "__main__":
    model_path, platform, do_quant, output_path = parse_arg()

    export_rknn(model_path, platform, do_quant, output_path)