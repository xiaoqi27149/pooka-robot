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

#include <time.h>
#include <fstream>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <iomanip>

using namespace std;

struct StereoCalibrationData {
    // IR相机参数
    int l_width, l_height;
    int r_width, r_height;
    cv::Mat K_l, D_l;  // 左相机内参和畸变系数
    cv::Mat K_r, D_r;  // 右相机内参和畸变系数

    // 立体标定参数
    cv::Mat R, T;      // 旋转矩阵和平移向量
    cv::Mat R1, R2;    // 左右相机旋转矩阵
    cv::Mat P1, P2;    // 左右相机投影矩阵
    cv::Mat Q;         // 视差-深度映射矩阵
    cv::Mat invPR1, invPR2; // 反投影矩阵

    // RGB相机参数
    int rgb_width, rgb_height;
    cv::Mat K_rgb, D_rgb; // RGB相机内参和畸变

    // IR-RGB相机间变换
    cv::Mat R_ir2rgb, T_ir2rgb;
    cv::Mat R1_ir2rgb, R2_ir2rgb;
    cv::Mat P1_ir2rgb, P2_ir2rgb;
    cv::Mat Q_ir2rgb;

    // 标定误差
    double stereo_rms;
    double left_rms;
    double right_rms;
    double rgb_rms;
    double ir_rgb_stereo_rms;
};

struct AdjustStereoParams {
    float fx_rgb;
    float fy_rgb;
    float cx_rgb;
    float cy_rgb;

    float fx_ir;
    float fy_ir;
    float cx_ir;
    float cy_ir;
    float depth_multi_ratio;

    int ir_calib_width;
    int rgb_calib_width;

    int ir_width;
    int ir_height;
    int rgb_width;
    int rgb_height;
    int out_width;
    int out_height;
    double baseline;
    double fx_origin;

    cv::Mat K_ir_s;
    cv::Mat K_rgb_s;
    cv::Mat R_ir2rgb_s;
    cv::Mat T_ir2rgb_s;
    cv::Mat K_ir_s_inv;

    cv::Mat R;
    cv::Mat T;
    cv::Mat P1;

    float out_fx;
    float out_fy;
    float out_cx;
    float out_cy;
    int extend_w_pixel;
    int extend_h_pixel;
    int rotateAngle;

    float* LUT;
};

struct StereoParam
{
    float fx_rgb;
    float fy_rgb;
    float cx_rgb;
    float cy_rgb;

    float fx_ir;
    float fy_ir;
    float cx_ir;
    float cy_ir;

    // Eigen::Matrix3f invH;
    // Eigen::Matrix3f R_ir2rgb;
    // Eigen::Vector3f T_ir2rgb;

    // Eigen::Matrix3f K_ir;
    // Eigen::Matrix3f K_rgb;
    // Eigen::Matrix3f R;
    // Eigen::Vector3f T;

    cv::Mat invH;       // 3x3 矩阵
    cv::Mat R_ir2rgb;   // 3x3 旋转矩阵
    cv::Mat T_ir2rgb;   // 3x1 平移向量

    cv::Mat K_ir;       // 3x3 内参矩阵
    cv::Mat K_rgb;      // 3x3 内参矩阵
    cv::Mat R;          // 3x3 矩阵
    cv::Mat T;          // 3x1 向量

    int* LUT_INVH;
    int* LUT_FIXDEPTH;
    float* LUT;
    int ir_calib_width;
    int ir_width;

    int rgb_calib_width;
    int rgb_crop_width;
    int rgb_width;
    int rgb_height;

    int extend_w_pixels;
    int extend_h_pixels;
    int rotateAngle;//counter clock-wise rotate angle, only support 0,90,270

    float depth_multi_ratio;

    float fs;
    float ref_distance;

    float out_fx;
    float out_fy;
    float out_cx;
    float out_cy;
    float out_baseline;
    float out_near_distance;
    float out_far_distance;
    float out_ori_focal;

    string dir;
};

void printStereoCalibrationData(const StereoCalibrationData* data);
void printStereoCalibrationData(const StereoCalibrationData& data);
void PrintAdjustStereoParams(const AdjustStereoParams& params);
