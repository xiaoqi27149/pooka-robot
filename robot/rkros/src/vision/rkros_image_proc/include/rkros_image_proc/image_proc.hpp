/*
 * Copyright (C) 2025 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RKROS_IMAGE_PROC__IMAGE_PROC_HPP_
#define RKROS_IMAGE_PROC__IMAGE_PROC_HPP_

#include <memory>

#include "rga/RgaApi.h"
#include "rga/RgaUtils.h"
#include "rga/im2d.hpp"
#include "rkros_common/rk_color.h"
#include "rkros_dmabuf/dmabuf.hpp"

namespace rkros_image_proc {

enum {
    FLIP_H = IM_HAL_TRANSFORM_FLIP_H,
    FLIP_V = IM_HAL_TRANSFORM_FLIP_V,
    FLIP_H_V = IM_HAL_TRANSFORM_FLIP_H_V,
};

enum {
    INTERP_DEFAULT = IM_INTERP_DEFAULT,
    INTERP_LINEAR = IM_INTERP_LINEAR,
    INTERP_CUBIC = IM_INTERP_CUBIC,
    INTERP_AVERAGE = IM_INTERP_AVERAGE,
};

enum {
    ROTATION_90 = IM_HAL_TRANSFORM_ROT_90,
    ROTATION_180 = IM_HAL_TRANSFORM_ROT_180,
    ROTATION_270 = IM_HAL_TRANSFORM_ROT_270,
};

// RGA对图像宽度有对齐要求，使用getStridePixel可得到对齐后的宽度，以像素为单位
int getStridePixel(int width, uint32_t rkclr_fmt);

// RGA对图像宽度有对齐要求，使用getStride可得到对齐后的宽度，以字节为单位
// 注意对于yuv 420 p/sp系列格式，其每个像素占用1.5字节
int getStrideBytes(int width, uint32_t rkclr_fmt);

/**
 * @brief 使用RGA进行图像复合处理
 * @param src 输入图像
 * @param dst 输出图像
 * @param autoSync 是否自动进行同步操作，默认true
 * @param opt 复合操作选项
 * @param usage 复合操作使用的RGA功能，默认0
 * @return 成功返回true，失败返回false
 */
bool process(const rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst,
             bool autoSync = true, im_opt_t opt = {}, int usage = 0);

/**
 * @brief 使用RGA进行图像缩放
 * @param src 输入图像
 * @param dst 输出图像
 * @param interp 插值方法，默认INTERP_DEFAULT
 * @param autoSync 是否自动进行同步操作，默认true
 * @return 成功返回true，失败返回false
 */
bool resize(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst,
            int interp = INTERP_DEFAULT, bool autoSync = true);

/**
 * @brief 使用RGA进行图像拷贝
 * @param src 输入图像
 * @param dst 输出图像
 * @param autoSync 是否自动进行同步操作，默认true
 * @return 成功返回true，失败返回false
 */
bool copy(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, bool autoSync = true);

/**
 * @brief 使用RGA进行图像旋转
 * @param src 输入图像
 * @param dst 输出图像
 * @param rotate_mode 旋转模式，支持ROTATION_90, ROTATION_180, ROTATION_270
 * @param autoSync 是否自动进行同步操作，默认true
 * @return 成功返回true，失败返回false
 */
bool rotate(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, int rotate_mode,
            bool autoSync = true);

/**
 * @brief 使用RGA进行图像裁剪
 * @param src 输入图像
 * @param dst 输出图像
 * @param autoSync 是否自动进行同步操作，默认true
 * @return 成功返回true，失败返回false
 */
bool crop(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, bool autoSync = true);

/**
 * @brief 使用RGA进行图像翻转（镜像）
 * @param src 输入图像
 * @param dst 输出图像
 * @param flip_code 翻转模式，支持FLIP_H, FLIP_V, FLIP_H_V
 * @param autoSync 是否自动进行同步操作，默认true
 * @return 成功返回true，失败返回false
 */
bool flip(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, int flip_code,
          bool autoSync = true);

/**
 * @brief 使用RGA进行图像颜色空间转换
 * @param src 输入图像
 * @param dst 输出图像
 * @param autoSync 是否自动进行同步操作，默认true
 * @return 成功返回true，失败返回false
 */
bool cvtColor(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, bool autoSync = true);

} // namespace rkros_image_proc

#endif // RKROS_IMAGE_PROC__IMAGE_PROC_HPP_
