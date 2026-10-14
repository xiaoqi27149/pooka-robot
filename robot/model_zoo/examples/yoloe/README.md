# yoloe

## Table of contents

- [yoloe](#yoloe)
  - [Table of contents](#table-of-contents)
  - [1. Description](#1-description)
  - [2. Current Support Platform](#2-current-support-platform)
  - [3. Convert to RKNN](#4-convert-to-rknn)
  - [4. Python Demo](#5-python-demo)
  - [5. Android Demo](#6-android-demo)
      - [5.1 Compile and Build](#61-compile-and-build)
      - [5.2 Push demo files to device](#62-push-demo-files-to-device)
      - [5.3 Run demo](#63-run-demo)
  - [6. Expected Results](#8-expected-results)



## 1. Description
The model used in this example comes from the following open source projects:
https://github.com/ultralytics/ultralytics



## 2. Current Support Platform

RK3576, RK3588


## 3. Convert to RKNN

*Usage:*

```shell
cd python
python xxx/convert.py <onnx_model> <TARGET_PLATFORM> <dtype(optional)> <output_rknn_path(optional)>

# such as: 
python yoloe/convert.py ../model/yoloe-11m-seg.onnx rk3588 i8
python yoloe/convert.py ../model/yoloe-11m-seg.onnx rk3588 fp
# output model will be saved as ../model/yoloe-11m-seg_fp16.rknn
python mobile_clip/convert.py ../model/mobileclip.onnx rk3588 fp

```

*Description:*

- `<onnx_model>`: Specify ONNX model path.
- `<TARGET_PLATFORM>`: Specify NPU platform name. Support Platform refer [here](#2-current-support-platform).
- `<dtype>(optional)`: Specify as `i8` or `fp`. `i8` for doing quantization, `fp` for no quantization. yoloe model default is `i8`, clip_text only support `fp`.
- `<output_rknn_path>(optional)`: Specify save path for the RKNN model, default save in the same directory as ONNX model with name `yoloe-11m-seg_int8.rknn`

## 4. Python Demo

*Usage:*

```shell
cd python

# Inference with RKNN model
python yoloe.py --text_model <rknn_model> --yoloe <rknn_model> --target <TARGET_PLATFORM>
# such as
python yoloe.py --text_model ../model/mobileclip.rknn --yoloe ../model/yoloe-11m-seg_int8.rknn --target rk3588 --eval_perf 0
```
*Description:*
- <TARGET_PLATFORM>: Specify NPU platform name. Such as 'rk3576'.
- <rknn_model>: specified as the model path.



## 5. Android Demo

#### 5.1 Compile and Build

*Usage:*

```sh
# go back to the rknn_model_zoo root directory
cd ../../
export ANDROID_NDK_PATH=<android_ndk_path>

./build-android.sh -t <TARGET_PLATFORM> -a <ARCH> -d yoloe

# such as 
export ANDROID_NDK_PATH = vendor/lib64
./build-android.sh -t rk3588 -a arm64-v8a -d yoloe
```

*Description:*
- `<android_ndk_path>`: Specify Android NDK path.
- `<TARGET_PLATFORM>`: Specify NPU platform name. Support Platform refer [here](#2-current-support-platform).
- `<ARCH>`: Specify device system architecture. To query device architecture, refer to the following command:
	```shell
	# Query architecture. For Android, ['arm64-v8a' or 'armeabi-v7a'] should shown in log.
	adb shell cat /proc/version
	```

#### 5.2 Push demo files to device

With device connected via USB port, push demo files to devices:

```shell
adb root
adb remount
adb push install/<TARGET_PLATFORM>_android_<ARCH>/rknn_yoloe_demo/ /data/
# such as
adb push install/rk3588_android_arm64-v8a/rknn_yoloe_demo/ /data/
```

#### 5.3 Run demo

```sh
adb shell
cd /data/rknn_yoloe_demo

export LD_LIBRARY_PATH=./lib # or export LD_LIBRARY_PATH=/vendor/lib64/
./rknn_yoloe_demo model/mobileclip.rknn model/detect_classes.txt model/yoloe-11m-seg_fp16.rknn model/bus.jpg
```

- After running, the result was saved as out.png. To check the result on host PC, pull back result referring to the following command: 

  ```sh
  adb pull /data/rknn_yoloe_demo/out.png
  ```


## 6. Expected Results

This example will print the labels and corresponding scores of the test image detect results, as follows:
```
person @ (475 233 559 521) 0.918
person @ (109 236 226 535) 0.918
bus @ (99 136 554 435) 0.918
person @ (211 242 282 509) 0.918
person @ (79 325 125 515) 0.625

```

<img src="result.png">
<br>

- Note: Different platforms, different versions of tools and drivers may have slightly different results.
