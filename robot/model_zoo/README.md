# RKNN Robot Model Zoo

## Description

`RKNN Robot Model Zoo` is developed based on the RKNPU SDK toolchain and provides deployment examples for current
mainstream algorithms. Include the process of `exporting the RKNN model` and using `Python API` and `CAPI` to infer the
RKNN model.

- Support  `RK3576`, `RK3588`   platforms.

## Dependency library installation

`RKNN Robot Model Zoo` relies on `RKNN-Toolkit2` for model conversion, and is a branch of rknn model
zoo (https://github.com/airockchip/rknn_model_zoo?tab=readme-ov-file). The Android compilation tool chain is required
when compiling the Android demo, For the installation of these dependencies, please refer to the `Quick Start`
documentation at https://github.com/airockchip/rknn-toolkit2/tree/master/doc.

## Model support

| Category               | Name                                                         | Dtype     | Model Download Link                                          | Support platform |
| ---------------------- | ------------------------------------------------------------ | --------- | ------------------------------------------------------------ | ---------------- |
| ViT                    | [ViT_b_16](https://github.com/google-research/vision_transformer) | FP16/INT8 | [ViT_b_16-int8.rknn]()<br/>[ViT_b_16-fp16.rknn]()            | RK3576\|RK3588   |
| ViT                    | [dinov2](https://github.com/facebookresearch/dinov2 )        | FP16/INT8 | [dinov2_vits14_int8.rknn]()<br/>[dinov2_vits14_fp16.rknn]()<br/>[dinov2_vitb14_fp16.rknn]() | RK3576\|RK3588   |
| ViT                    | [dinov3](https://github.com/facebookresearch/dinov2 )        | FP16/INT8 | [dinov3_vitb16_fp16.rknn]()<br/>[dinov3_vits16_fp16.rknn]()<br/>[dinov3_convnext_tiny_640x640_fp16.rknn]()<br/>[dinov3_convnext_tiny_640x640_int8.rknn]() | RK3576\|RK3588   |
| ViT                    | [FastVit_t8](https://github.com/apple/ml-fastvit/tree/main?tab=readme-ov-file) | FP16/INT8 | [fastvit_t8_int8.rknn]()<br/>[fastvit_t8_fp16.rknn]()        | RK3576\|RK3588   |
| Object Detection       | [yolo_world](https://github.com/AILab-CVC/YOLO-World)        | FP16/INT8 | [yolo_world_v2s_int8.rknn]()<br/>[yolo_world_v2s_fp16.rknn]()<br/>[clip_text.rknn]() | RK3576\|RK3588   |
| Image Segmentation     | [mobilesam](https://github.com/ChaoningZhang/MobileSAM)      | FP16      | [mobilesam_encoder.rknn]()<br/>[mobilesam_decoder.rknn]()    | RK3576\|RK3588   |
| Object Detection       | [yoloe](https://github.com/ultralytics/ultralytics)          | FP16/INT8 | [yoloe-11m-seg_int8.rknn]()<br/>[yoloe-11m-seg_fp16.rknn]()<br/>[mobileclip.rknn]() | RK3576\|RK3588   |
| Object Detection       | [RTDETR V2](https://github.com/lyuwenyu/RT-DETR)             | FP16      | [rtdetrv2_r18vd_dsp_fp16.rknn]()                             | RK3576\|RK3588   |
| Object Detection       | [DETR](https://github.com/facebookresearch/detr)             | FP16/INT8 | [detr_r50_fp16.rknn]()<br/>[detr_r50_int8.rknn]()            | RK3576\|RK3588   |
| VLM                    | [Qwen2-VL-2B](https://huggingface.co/Qwen/Qwen2-VL-2B-Instruct) | w8a8      | [qwen2_vl_2b_vision_rk3588.rknn]()<br/>[qwen2_vl_2b_instruct.rkllm]() | RK3588           |
| Feature Point          | [superpoint](https://github.com/magicleap/supergluepretrainednetwork) | FP16/INT8 | [superpoint_fp16.rknn]()<br/>[superpoint_int8.rknn]()        | RK3576\|RK3588   |
| Feature Point          | [superglue](https://github.com/magicleap/supergluepretrainednetwork) | FP16      | [superglue_fp16.rknn]()                                      | RK3576\|RK3588   |
| Point Clound Detection | [pointpillars](https://github.com/open-mmlab/mmdetection3d/tree/main/configs/pointpillars) | FP16      | [pointpillar_voxel_encoder_fp16.rknn]()<br/>[pointpillar_head_fp16.rknn]() | RK3576\|RK3588   |
| Bird's-Eye-View        | [fast_bev](https://github.com/open-mmlab/mmdetection3d/tree/main/configs/pointpillars) | FP16      | [fastbev_pre_fp16.rknn]()<br/>[fastbev_post_fp16.rknn]()<br/>[fastbev_pre_int8.rknn]()<br/>[fastbev_post_int8.rknn]() | RK3576\|RK3588   |

## Model performance benchmark(ms)

| demo          | model_name                  | inputs_shape&nbsp;&nbsp;&nbsp;&nbsp;                                                               | dtype | RK3588                                   | dytpe | RK3588 |
|---------------|-----------------------------|----------------------------------------------------------------------------------------------------|-------|------------------------------------------|-------|--------|
| ViT_b_16      | ViT_b_16                    | [1, 3, 224, 224]                                                                                   | INT8  | 64.4                                     | FP16  | 98.5   |
| FastVit_t8    | FastVit_t8                  | [1, 3, 224, 224]                                                                                   | INT8  | 10.1                                     | FP16  | 15.4   |
| DINOV2        | dinov2_vits14               | [1, 3, 224, 224]                                                                                   | INT8  | 39.4                                     | FP16  | 45.7   |
|               | dinov2_vitb14               | [1, 3, 224, 224]                                                                                   | INT8  | ——                                       | FP16  | 122.3  |
| DINOV3        | dinov3_vits16               | [1, 3, 224, 224]                                                                                   | INT8  | ——                                       | FP16  | 47.9   |
|               | dinov3_vitb16               | [1, 3, 224, 224]                                                                                   | INT8  | ——                                       | FP16  | 113.7  |
| | dinov3_vits16 | [1, 3, 640, 640] | INT8 | —— | FP16 | 802.3 |
| | dinov3_convnext_tiny | [1, 3, 640, 640] | INT8 | 132.1 | FP16 | 192.3 |
| yolov_world   | yolo_world_v2s              | [1, 3, 640, 640]                                                                                   | INT8  | 64.7                                     | FP16  | 108.9  |
|               | clip_text                   | [1, 20]（token id）                                                                                  | INT8  | ——                                       | FP16  | 13.6   |
| mobilesam     | mobilesam_encoder           | [1, 3, 448, 448]                                                                                   | INT8  | ——                                       | FP16  | 100.3  |
|               | mobilesam_decoder           | [1, 1, 112, 112] (mask promot)<br/>[1, 256, 28, 28] (encoder feat)<br/>[1, 1, 2, 2] (point promot) | INT8  | ——                                       | FP16  | 12.5   |
| yoloe         | yoloe-11m                   | [1, 3, 640, 640] (image)<br/>[1, 20, 512] (text)                                                   | INT8  | 116.4                                    | FP16  | 272.9  |
|               | mobile_clip                 | [1, 20]（token id）                                                                                  | INT8  | ——                                       | FP16  | 14.3   |
| RTDETR-V2     | rtdetrv2_r18vd_dsp_3x       | [1, 3, 640, 640]                                                                                   | INT8  | ——                                       | FP16  | 204.2  |
| DETR          | DETR-Res50                  | [1, 3, 640, 640]                                                                                   | INT8  | 81.3                                     | FP16  | 112.8  |
| Qwen2-VL-2B   | vision                      | [1, 3, 392, 392]                                                                                   | INT8  | ——                                       | FP16  | 3340   |
|               | llm                         |                                                                                                    | w8a8  | Generate:14.88TPS<br/>Prefill: 305.18TPS | FP16  | ——     |
| superpoint    | superpoint                  | [1, 3, 480, 640]                                                                                   | INT8  | 23.5                                     | FP16  | 44.9   |
| superglue     | superglue                   | [1, 256, 384] (des)<br/>[1, 2, 384] (kpt)<br/>[1, 384] (score)                                     | INT8  | ——                                       | FP16  | 100.4  |
| point pillars | point pillars voxel encoder | 20000x32x10 (voxel)                                                                                | INT8  | ——                                       | FP16  | 454.7  |
|               | point pillars voxel head    | 496x432x64                                                                                         | INT8  | ——                                       | FP16  | 132.0  |
| fast_bev_r18  | fast bev pre model          | 6x256x704x3                                                                                        | INT8  | 47.2                                 | FP16  | 111.3 |
|               | fast bev post model         | 1x200x200x256                                                                                      | INT8  | 40.2                                   | FP16  | 89.6 |
| fast_bev_r50  | fast bev pre model          | 24x320x880x3                                                                                       | INT8  | ——                                       | FP16  | 1453.3  |
|               | fast bev post model         | 1x250x250x1536                                                                                     | INT8  | ——                                       | FP16  | 553.2  |

- This performance data are collected based on the maximum NPU frequency of each platform.
- This performance data calculate the time-consuming of model inference. Does not include the time-consuming of
  pre-processing and post-processing if not specified.
- The performance data is measured by averaging 100 times.

## Compile Demo

For Android development board:

```sh
# For Android develop boards, it's require to set path for Android NDK compilation tool path according to the user environment
export ANDROID_NDK_PATH=~/opts/ndk/android-ndk-r18b
./build-android.sh -t <target> -a <arch> -d <build_demo_name> [-b <build_type>] [-m]
    -t : target (rk3576/rk3588)
    -a : arch (arm64-v8a/armeabi-v7a)
    -d : demo name
    -b : build_type (Debug/Release)
    -m : enable address sanitizer, build_type need set to Debug

# Here is an example for compiling yolov5 demo for 64-bit Android RK3566.
./build-android.sh -t rk3588 -a arm64-v8a -d yoloe
```

## Environment dependencies

All demos in `RKNN Model Zoo` are verified based on the latest RKNPU SDK. If using a lower version for verification, the
inference performance and inference results may be wrong.

| Version | RKNPU2 SDK |
|---------|------------|
| 2.3.2   | >=2.3.2    |
| 2.3.0   | >=2.3.0    |

## RKNPU Resource

- RKNPU2 SDK: https://github.com/airockchip/rknn-toolkit2
- RKNPU1 SDK: https://github.com/airockchip/rknn-toolkit

## License

[Apache License 2.0](./LICENSE)

