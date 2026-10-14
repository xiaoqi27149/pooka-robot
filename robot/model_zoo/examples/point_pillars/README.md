# point pillars

## Table of contents

- [ point pillars](#point-pillars)
  - [Table of contents](#table-of-contents)
  - [1. Description](#1-description)
  - [2. Current Support Platform](#2-current-support-platform)
  - [3. Convert to RKNN](#3-convert-to-rknn)
  - [4. Python Demo](#4-python-demo)


## 1. Description
The model used in this example comes from the following open source projects:
https://github.com/open-mmlab/mmdetection3d/tree/main/configs/pointpillars

## 2. Current Support Platform and Requirements
RK3576, RK3588

## 3. Convert to RKNN

*Usage:*

```shell
cd python
python convert.py <onnx_model> <TARGET_PLATFORM> <dtype(optional)> <output_rknn_path(optional)>

# such as: 
python convert.py ../model/pointpillar_voxel_encoder.onnx rk3588 fp
python convert.py ../model/pointpillar_head.onnx rk3588 fp
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
python point_pillars.py --encoder_path <encoder_rknn_model> --head_path <head_rknn_model> --pcd <clound_points_path> --target <TARGET_PLATFORM>
# such as
python point_pillars.py --encoder_path ../model/pointpillar_voxel_encoder_fp16.rknn --head_path ../model/pointpillar_head_fp16.rknn --pcd ../model/000008.bin --target rk3588
```
