# super point + glue

## Table of contents

- [ super point + glue](#super-point-glu)
  - [Table of contents](#table-of-contents)
  - [1. Description](#1-description)
  - [2. Current Support Platform](#2-current-support-platform)
  - [3. Convert to RKNN](#3-convert-to-rknn)
  - [4. Python Demo](#4-python-demo)
  - [5. Expected Results](#5-expected-results)



## 1. Description
The model used in this example comes from the following open source projects:
https://github.com/magicleap/supergluepretrainednetwork

## 2. Current Support Platform and Requirements
RK3576, RK3588

## 3. Convert to RKNN

*Usage:*

```shell
cd python
python xxx/convert.py <onnx_model> <TARGET_PLATFORM> <dtype(optional)> <output_rknn_path(optional)>

# such as: 
python superglue/convert.py ../model/superglue.onnx rk3588 fp
python superpoint/convert.py ../model/superpoint.onnx rk3588 fp
python superpoint/convert.py ../model/superpoint.onnx rk3588 i8
```

*Description:*

- `<onnx_model>`: Specify ONNX model path.
- `<TARGET_PLATFORM>`: Specify NPU platform name. Support Platform refer [here](#2-current-support-platform).
- `<dtype>(optional)`: Specify as `i8` or `fp`. `i8` for doing quantization, `fp` for no quantization. yolo_world model default is `i8`, clip_text only support `fp`.
- `<output_rknn_path>(optional)`: Specify save path for the RKNN model, default save in the same directory as ONNX model.


## 4. Python Demo
*Usage:*

```shell
cd python

# Inference with RKNN model
python superglue.py --point_path <point_rknn_model> --glue_path <point_rknn_model> --img0 <image0_path> --img1 <image1_path> --target <TARGET_PLATFORM>
# such as
python superglue.py --point_path ../model/superpoint_fp16.rknn --glue_path ../model/superglue_fp16.rknn --img0 ../model/scene0711_00_frame-001680.jpg  --img1 ../model/scene0711_00_frame-001995.jpg --target rk3588
```
*Description:*
- <TARGET_PLATFORM>: Specify NPU platform name. Such as 'rk3576'.
- <rknn_model>: specified as the model path.
- <image_path>: path to image file


