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

#ifndef RKROS_OPENCV__RKCV_HPP_
#define RKROS_OPENCV__RKCV_HPP_

#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>

#include "rkros_dmabuf/dmabuf.hpp"

namespace rkcv {

enum { AUTO_STEP = 0 };

using DmabufPtr = std::shared_ptr<rkros_dmabuf::DmaBuf>;

/**
 * 创建一个支持dmabuf的空的UMat
 */
cv::UMat createUMat();

/**
 * 创建一个使用dmabuf的UMat
 * @param rows 行数
 * @param cols 列数
 * @param type opencv的type
 */
cv::UMat createUMat(int rows, int cols, int type);

/**
 * 对dmabuf进行封装后得到一个UMat
 * @param rows 行数
 * @param cols 列数
 * @param type opencv的type
 * @param dmabuf dmabuf指针
 * @param step 步长，AUTO_STEP表示自动计算
 */
cv::UMat createUMat(int rows, int cols, int type, DmabufPtr dmabuf,
                    size_t step = AUTO_STEP);

/**
 * 对image进行封装后得到一个UMat
 * @param img 包含了dmabuf内存和图像信息的rkros_dmabuf::Image
 */
cv::UMat createUMat(rkros_dmabuf::Image &img);

/**
 * 从UMat中获取Image封装，自动做同步。若UMat不支持或为空，则返回空对象
 */
rkros_dmabuf::Image getImage(const cv::UMat &um);

/**
 * 从UMat中获取Dmabuf，自动做同步。若UMat不支持或为空，则返回空对象
 */
DmabufPtr getDmabuf(const cv::UMat &um);

/**
 * 判断UMat是否支持dmabuf
 */
bool isSupportMat(const cv::UMat &um);
} // namespace rkcv

#endif // RKROS_OPENCV__RKCV_HPP_