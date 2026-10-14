import os
import argparse
import numpy as np
import cv2
from rknn.api import RKNN
from sklearn.decomposition import PCA
from sklearn.preprocessing import MinMaxScaler

DEFAULT_IMG_SIZE = (224, 224)

def visualize_features(features, patch_size, frame_dim, seperate_foreground):
    if patch_size is not None:
        patch_h = int(frame_dim[0] / patch_size)
        patch_w = int(frame_dim[1] / patch_size)
        patch_features = features.reshape(patch_h * patch_w, -1)
    else:
        patch_features = features[0]
        patch = np.sqrt(patch_features.shape[0])
        patch_h = int(patch)
        patch_w = int(patch)
        print('patch_size is None for convnext')

    # Apply PCA and MinMaxScaler
    n_components = 3
    pca = PCA(n_components=n_components)
    scaler = MinMaxScaler(clip=True)
    pca.fit(patch_features)
    pca_features = pca.transform(patch_features)
    scaler.fit(pca_features)
    pca_features = scaler.transform(pca_features)

    # Separate background and foreground
    threshold = 0.5
    if seperate_foreground:
        pca_background = pca_features[:, 0] > threshold
        pca_foreground = ~pca_background

        # Refit PCA for foreground
        pca.fit(patch_features[pca_foreground])
        pca_features_rem = pca.transform(patch_features[pca_foreground])
        scaler.fit(pca_features_rem)
        pca_features_rem = scaler.transform(pca_features_rem)

        pca_features_rgb = np.zeros([patch_h * patch_w, n_components])
        pca_features_rgb[pca_background] = 0
        pca_features_rgb[pca_foreground] = pca_features_rem
        pca_features = pca_features_rgb.reshape(patch_h, patch_w, n_components)

    # Convert the PCA features to an image
    pca_features = pca_features.reshape(patch_h, patch_w, n_components)
    return pca_features


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='rknn model test')
    parser.add_argument('--model', type=str, help="model path", default='./examples/vit/model/dinov3_vits16_640x640_int8.rknn')
    parser.add_argument('--cls_token', type=str, help="cls_token path", default='./examples/vit/model/dinov3_vits16_cls_token.npy')
    parser.add_argument('--storage_tokens', type=str, help="storage_tokens path", default='./examples/vit/model/dinov3_vits16_storage_tokens.npy')
    parser.add_argument('--target', type=str, help="target platform", default='rk3588')
    parser.add_argument('--img', type=str, help="input image", default='./examples/vit/model/zoo.jpg')
    parser.add_argument('--ip', type=str, help="device ip", default='172.16.4.92:5555')
    parser.add_argument('--size', type=int, help="network input size", default=None)
    parser.add_argument('--eval', type=str, help="eval flag", default=0)

    args = parser.parse_args()
    if args.size is not None:
        IMG_SIZE = (args.size, args.size)
    else:
        IMG_SIZE = DEFAULT_IMG_SIZE

    img = cv2.imread(args.img)
    img = cv2.resize(img, IMG_SIZE)
    img = np.expand_dims(img, 0)
    img = np.float32(img)

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
    if args.eval:
        num = 10
        sum_time = 0
        for _ in range(num):
            print('--> Eval Perf')
            perf_info = rknn.eval_perf(is_print=True)

    # Inference
    print('--> Running model')
    if 'v3' in args.model:
        if 'conv' in args.model:
            patch_size = None
            outputs = rknn.inference(inputs=[img], data_format='nhwc')
        else:
            patch_size = 16
            cls_token = np.load(args.cls_token)
            storage_tokens = np.load(args.storage_tokens)
            outputs = rknn.inference(inputs=[img, cls_token, storage_tokens], data_format='nhwc')
    else:
        patch_size = 14
        outputs = rknn.inference(inputs=[img],  data_format='nhwc')

    norm_clstoken = outputs[0]
    norm_patchtokens = outputs[1]

    # Vis Feature
    feature_frame = visualize_features(norm_patchtokens, patch_size, IMG_SIZE, False)
    feature_frame = (feature_frame * 255).astype(np.uint8)
    feature_frame = cv2.resize(feature_frame, IMG_SIZE, cv2.INTER_NEAREST)
    cv2.imwrite("vis_feature.png", cv2.cvtColor(feature_frame, cv2.COLOR_RGB2BGR))
    print('Save results to vis_feature.png!')
    print('done')

    # Release
    rknn.release()


