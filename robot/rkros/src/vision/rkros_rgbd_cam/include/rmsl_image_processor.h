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

#pragma once
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <rclcpp/rclcpp.hpp>

#include "rmsl32x_param.h"
#include "rmsl_logger.h"

enum PCLMode {
    DEPTH_ONLY = 0,
    RGBD = 1
};

class ImageProcessor {
public:
    explicit ImageProcessor(const AdjustStereoParams& params);
    //ImageProcessor();  // 构造函数可添加配置参数
    //ImageProcessor(const StereoParam& params);
    ~ImageProcessor() = default;

    // disp to depth
    cv::Mat disp_to_depth_16u(const cv::Mat& disparity, float baseline, float fx, uint16_t max_depth = 65535);

    cv::Mat alignDepthToRGB(cv::Mat& depth_mm);
    cv::Mat alignDepthIRToRGB(cv::Mat& depth_mm, cv::Mat& ir_img);
    cv::Mat convertDepthToPseudoColor(const cv::Mat& depthMap);

    sensor_msgs::msg::PointCloud2::SharedPtr convertDepthToPointCloud_depthonly(
        const cv::Mat& depth_image,
        const std::string& frame_id,
        bool aligned,
        const cv::Rect& roi);

    sensor_msgs::msg::PointCloud2::SharedPtr convertDepthToPointCloud_rgbd(
        const cv::Mat& depth_image,
        const cv::Mat& color_image,
        const std::string& frame_id,
        bool aligned,
        const cv::Rect& roi);

    cv::Mat process(const cv::Mat& input);

private:
    AdjustStereoParams params_;

    int m_ir_width;
    int m_ir_height;
    int m_out_width;
    int m_out_height;
    int m_extend_h_pixel;
    int m_extend_w_pixel;
    int m_rgb_width;
    int m_rgb_height;
    float cx_rgb_;
    float cy_rgb_;
    float fx_rgb_;
    float fy_rgb_;
    float p1_02_;
    float p1_00_;
    float p1_12_;
    float p1_11_;

    std::unique_ptr<float[]> m_lut;
    std::unique_ptr<unsigned short[]> m_depth;
    std::unique_ptr<unsigned short[]> m_depth_remap;
    std::unique_ptr<unsigned short[]> m_depth_result;
    std::unique_ptr<unsigned short[]> m_depth_out;

    std::unique_ptr<unsigned char[]> m_ir;
    std::unique_ptr<unsigned char[]> m_ir_result;
    std::unique_ptr<unsigned char[]> m_ir_out;

    void initLut(AdjustStereoParams* param, int src_width, int src_height);

};
