# detr

## Table of contents

- [ detr](#detr)
  - [Table of contents](#table-of-contents)
  - [1. Description](#1-description)
  - [2. Current Support Platform](#2-current-support-platform)
  - [3. Convert to RKNN](#4-convert-to-rknn)
  - [4. Android Demo](#6-android-demo)
      - [4.1 Compile and Build](#61-compile-and-build)
      - [4.2 Push demo files to device](#62-push-demo-files-to-device)
      - [4.3 Run demo](#63-run-demo)
  - [5. Expected Results](#8-expected-results)



## 1. Description
The model used in this example comes from the following open source projects:
https://github.com/facebookresearch/detr


## 2. Current Support Platform and Requirements
RK3576, RK3588

## 3. Convert to RKNN

*Usage:*

```shell
cd python
python convert.py <onnx_model> <TARGET_PLATFORM> <dtype(optional)> <output_rknn_path(optional)>

# such as: 
python convert.py ../model/detr_r50.onnx rk3588 fp
python convert.py ../model/detr_r50.onnx rk3588 i8
```

*Description:*

- `<onnx_model>`: Specify ONNX model path.
- `<TARGET_PLATFORM>`: Specify NPU platform name. Support Platform refer [here](#2-current-support-platform).
- `<dtype>(optional)`: Specify as `i8` or `fp`. `i8` for doing quantization, `fp` for no quantization. yolo_world model default is `i8`, clip_text only support `fp`.
- `<output_rknn_path>(optional)`: Specify save path for the RKNN model, default save in the same directory as ONNX model with name ` detr-11m-seg_int8.rknn`


## 4. Python Demo
*Usage:*

```shell
cd python

# Inference with RKNN model
python detr.py --model_path <rknn_model> --img <image_path> --target <TARGET_PLATFORM>
# such as
python detr.py --model_path ../model/detr_r50_fp16.rknn --img ../model/cat.jpg  --target rk3588
python detr.py --model_path ../model/detr_r50_int8.rknn --img ../model/cat.jpg  --target rk3588 --eval_perf 0
```
*Description:*
- <TARGET_PLATFORM>: Specify NPU platform name. Such as 'rk3576'.
- <rknn_model>: specified as the model path.
- <image_path>: path to image file


