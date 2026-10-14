## 1. Convert to RKNN

python python/convert.py <onnx_model> <TARGET_PLATFORM> <dtype(optional)> <output_rknn_path(optional)>
**(output model will be saved as ./model/vit_b_16_int8.rknn others command)**

```shell

### such as: 
python python/convert.py ./model/vit_b_16.onnx rk3588 i8
python python/convert.py ./model/fastvit_t8.onnx rk3588 i8

# dinov2
python python/convert.py ./model/dinov2_vits14.onnx rk3588 i8
python python/convert.py ./model/dinov2_vits14.onnx rk3588 fp

python python/convert.py ./model/dinov2_vitb14.onnx rk3588 fp
# dinov3
python python/convert.py ./model/dinov3_vitb16.onnx rk3588 fp
python python/convert.py ./model/dinov3_vits16.onnx rk3588 fp

# dinov3 for 640x640 size
python python/convert.py ./model/dinov3_vits16_640x640.onnx rk3588 fp

#dinov3 convnext
python python/convert.py ./model/dinov3_convnext_tiny_640x640.onnx rk3588 fp
python python/convert.py ./model/dinov3_convnext_tiny_640x640.onnx rk3588 i8

```

## 2. Python Demo for vit
### 2.1 demo for fastvit_t8 and vit_b_16
```shell
python python/vit.py --model <rknn_model>

# such as
python python/vit.py --model ../model/vit_b_16_fp16.rknn
```

### 2.2 demo for dinov
```shell
# for dinov2
python python/dino.py --model <rknn_model> --img <img_file>

# for dinov3
python python/dino.py --model <rknn_model> --cls_token <cls_token> --storage_tokens <storage_tokens> --img <img_file>

# such as dinov2
python python/dino.py --model ./model/dinov2_vits14_int8.rknn --img ./model/cat_224x224.jpg
python python/dino.py --model ./model/dinov2_vitb14_fp16.rknn --img ./model/cat_224x224.jpg

# such as dinov3 transformer
python python/dino.py --model ./model/dinov3_vitb16_fp16.rknn --cls_token ./model/dinov3_vitb16_cls_token.npy --storage_tokens ./model/dinov3_vitb16_storage_tokens.npy --img ./model/cat_224x224.jpg

python python/dino.py --model ./model/dinov3_vits16_fp16.rknn --cls_token ./model/dinov3_vits16_cls_token.npy --storage_tokens ./model/dinov3_vits16_storage_tokens.npy --img ./model/cat_224x224.jpg

python python/dino.py --model ./model/dinov3_vits16_640x640_fp16.rknn --cls_token ./model/dinov3_vits16_cls_token.npy --storage_tokens ./model/dinov3_vits16_storage_tokens.npy --img ./model/zoo.jpg --size 640

# such as dinov3 convnext
python python/dino.py --model ./model/dinov3_convnext_tiny_640x640_fp16.rknn --img ./model/zoo.jpg --size 640
```

## 3. Android Demo
​	*todo*
