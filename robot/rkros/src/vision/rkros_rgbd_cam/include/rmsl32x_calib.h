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

#define MAT3_SIZE 9    // 3x3矩阵元素个数
#define MAT4_SIZE 16   // 4x4矩阵元素个数
#define VEC3_SIZE 3    // 3维向量
#define VEC14_SIZE 14  // 14维向量
#define VEC12_SIZE 12  // 12维向量

typedef struct {
    // 红外相机参数
    int l_width, l_height;
    int r_width, r_height;

    // 相机内参矩阵 (3x3)
    double K_l[MAT3_SIZE];
    double K_r[MAT3_SIZE];

    // 畸变系数
    double D_l[VEC14_SIZE];
    double D_r[VEC14_SIZE];

    // 立体校正参数
    double R[MAT3_SIZE];      // 旋转矩阵
    double T[VEC3_SIZE];      // 平移向量

    double R1[MAT3_SIZE];     // 左相机旋转矩阵
    double R2[MAT3_SIZE];     // 右相机旋转矩阵

    // 投影矩阵 (3x4)
    double P1[12];
    double P2[12];

    // 重投影矩阵 (4x4)
    double Q[MAT4_SIZE];

    // 逆投影矩阵 (3x3)
    double invPR1[MAT3_SIZE];
    double invPR2[MAT3_SIZE];

    // 标定误差
    double stereo_rms;
    double left_rms;
    double right_rms;

    // RGB相机参数
    int rgb_width, rgb_height;
    double K_rgb[MAT3_SIZE];
    double D_rgb[VEC12_SIZE];

    // IR到RGB的变换参数
    double R_ir2rgb[MAT3_SIZE];
    double T_ir2rgb[VEC3_SIZE];

    double R1_ir2rgb[MAT3_SIZE];
    double R2_ir2rgb[MAT3_SIZE];

    double P1_ir2rgb[12];
    double P2_ir2rgb[12];

    double Q_ir2rgb[MAT4_SIZE];

    // RGB相机标定误差
    double rgb_rms;
    double ir_rgb_stereo_rms;
} StereoCameraParams;

typedef struct {
    unsigned int param_size;
    unsigned int param_checkSum;
    StereoCameraParams params;
} UVC_StereoCameraParams;
