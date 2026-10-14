# fast bev

## Table of contents

- [fast bev](#fast-bev)
  - [Table of contents](#table-of-contents)
  - [1. Description](#1-description)
  - [2. Current Support Platform](#2-current-support-platform)
  - [3. Convert to RKNN](#3-convert-to-rknn)
  - [4. Python Demo](#4-python-demo)


## 1. Description
The model used in this example comes from the following open source projects:
https://github.com/Sense-GVT/Fast-BEV

To convert PyTorch model to ONNX, can refer to the following open source projects:
https://github.com/Mandylove1993/CUDA-FastBEV

## 2. Current Support Platform and Requirements
RK3576, RK3588

numba==0.62.1
torchvision==0.19.0

## 3. Convert to RKNN

*Usage:*

```shell
cd python
python convert.py <onnx_model> <TARGET_PLATFORM> <dtype(optional)> <output_rknn_path(optional)>

# such as: 
python convert.py ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_pre.onnx rk3588 fp
python convert.py ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_post.onnx rk3588 fp

python convert.py ../model/fastbev_m4_r50_s320x880_v250x250x6_c256_d6_f4/fastbev_pre.onnx rk3588 fp
python convert.py ../model/fastbev_m4_r50_s320x880_v250x250x6_c256_d6_f4/fastbev_post.onnx rk3588 fp

# i8
python convert.py ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_pre.onnx rk3588 i8
python convert.py ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_post.onnx rk3588 i8
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
python fast_bev.py --pre_path <fast_bev_pre_rknn_model> --post_path <fast_bev_post_rknn_model> --anchors_file <anchors file> --meta_file <meta file> --config_file <config_file> --target <TARGET_PLATFORM>
# such as
python fast_bev.py \
  --pre_path ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_pre_fp16.rknn \
  --post_path ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_post_fp16.rknn \
  --anchors_file ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/anchors.tensor \
  --meta_file ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/img_metas.pth \
  --config_file configs/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1.py \
  --target rk3588

python fast_bev.py \
  --pre_path ../model/fastbev_m4_r50_s320x880_v250x250x6_c256_d6_f4/fastbev_pre_fp16.rknn \
  --post_path ../model/fastbev_m4_r50_s320x880_v250x250x6_c256_d6_f4/fastbev_post_fp16.rknn \
  --anchors_file ../model/fastbev_m4_r50_s320x880_v250x250x6_c256_d6_f4/anchors.tensor \
  --meta_file ../model/fastbev_m4_r50_s320x880_v250x250x6_c256_d6_f4/img_metas.pth \
  --config_file configs/fastbev_m4_r50_s320x880_v250x250x6_c256_d6_f4.py \
  --target rk3588

# int8 model
python fast_bev.py \
  --pre_path ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_pre_int8.rknn \
  --post_path ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/fastbev_post_int8.rknn \
  --anchors_file ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/anchors.tensor \
  --meta_file ../model/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1/img_metas.pth \
  --config_file configs/fastbev_m0_r18_s256x704_v200x200x4_c192_d2_f1.py \
  --target rk3588
```


