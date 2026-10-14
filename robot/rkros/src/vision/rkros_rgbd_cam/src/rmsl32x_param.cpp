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

#include "rmsl32x_param.h"

using namespace std;

bool parseStereoYAML(const std::string& filename, StereoCalibrationData& data)
{
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Failed to open: " << filename << std::endl;
        return false;
    }

    // 解析IR相机参数
    fs["l_width"] >> data.l_width;
    fs["l_height"] >> data.l_height;
    fs["r_width"] >> data.r_width;
    fs["r_height"] >> data.r_height;
    fs["K_l"] >> data.K_l;
    fs["D_l"] >> data.D_l;
    fs["K_r"] >> data.K_r;
    fs["D_r"] >> data.D_r;

    // 解析立体标定参数
    fs["R"] >> data.R;
    fs["T"] >> data.T;
    fs["R1"] >> data.R1;
    fs["R2"] >> data.R2;
    fs["P1"] >> data.P1;
    fs["P2"] >> data.P2;
    fs["Q"] >> data.Q;
    fs["invPR1"] >> data.invPR1;
    fs["invPR2"] >> data.invPR2;

    // 解析RGB相机参数
    fs["rgb_width"] >> data.rgb_width;
    fs["rgb_height"] >> data.rgb_height;
    fs["K_rgb"] >> data.K_rgb;
    fs["D_rgb"] >> data.D_rgb;

    // 解析IR-RGB变换参数
    fs["R_ir2rgb"] >> data.R_ir2rgb;
    fs["T_ir2rgb"] >> data.T_ir2rgb;
    fs["R1_ir2rgb"] >> data.R1_ir2rgb;
    fs["R2_ir2rgb"] >> data.R2_ir2rgb;
    fs["P1_ir2rgb"] >> data.P1_ir2rgb;
    fs["P2_ir2rgb"] >> data.P2_ir2rgb;
    fs["Q_ir2rgb"] >> data.Q_ir2rgb;

    // 解析标定误差
    fs["stereo_rms"] >> data.stereo_rms;
    fs["left_rms"] >> data.left_rms;
    fs["right_rms"] >> data.right_rms;
    fs["rgb_rms"] >> data.rgb_rms;
    fs["ir_rgb_stereo_rms"] >> data.ir_rgb_stereo_rms;

    fs.release();
    return true;
}

bool LoadParamFromcalibData(StereoParam* param, StereoCalibrationData* calib_data)
{
    // StereoCalibrationData calib_data;
    if (parseStereoYAML(param->dir, *calib_data)) {
        param->ir_calib_width = calib_data->l_width;
        param->rgb_calib_width = calib_data->rgb_width;

        float ir_ratio = (float)param->ir_width / (float)param->ir_calib_width;
        float rgb_ratio = (float)param->rgb_width / (float)param->rgb_crop_width;
        float depth_multi_ratio = param->depth_multi_ratio;

        int cx_rgb_offset = (param->rgb_crop_width - param->rgb_calib_width) / 2;

        param->fx_ir = calib_data->K_l.at<double>(0, 0) * ir_ratio;
        param->fy_ir = calib_data->K_l.at<double>(1, 1) * ir_ratio;
        param->cx_ir = calib_data->K_l.at<double>(0, 2) * ir_ratio;
        param->cy_ir = calib_data->K_l.at<double>(1, 2) * ir_ratio;

        param->fx_rgb = calib_data->K_rgb.at<double>(0, 0) * rgb_ratio;
        param->fy_rgb = calib_data->K_rgb.at<double>(1, 1) * rgb_ratio;
        param->cx_rgb = (calib_data->K_rgb.at<double>(0, 2) + cx_rgb_offset) * rgb_ratio;
        param->cy_rgb = calib_data->K_rgb.at<double>(1, 2) * rgb_ratio;

        // fs and ref_distance

        if (param->rotateAngle == 270) {
            param->out_fx = param->fy_rgb;
            param->out_fy = param->fx_rgb;
            param->out_cx = param->rgb_height - param->cy_rgb;
            param->out_cy = param->cx_rgb;
            //param->out_baseline = 40;
            //param->out_ori_focal = (calib_data[11] + calib_data[12]) / 2;
            //getRealDistanceRange(298, 1065, param);
        }
        else if (param->rotateAngle == 90) {
            param->out_fx = param->fy_rgb;
            param->out_fy = param->fx_rgb;
            param->out_cx = param->cy_rgb;
            param->out_cy = param->rgb_width - param->cx_rgb;
            //param->out_baseline = 40;
            //param->out_ori_focal = (calib_data[11] + calib_data[12]) / 2;
            //getRealDistanceRange(280, 1200, param);
        }

    param->K_ir = (cv::Mat_<float>(3, 3) <<
        param->fx_ir, 0, param->cx_ir,
        0, param->fy_ir, param->cy_ir,
        0, 0, 1);

    // 初始化内参矩阵 K_rgb
    param->K_rgb = (cv::Mat_<float>(3, 3) <<
        param->fx_rgb, 0, param->cx_rgb,
        0, param->fy_rgb, param->cy_rgb,
        0, 0, 1);

    // 初始化 R_ir2rgb
    param->R_ir2rgb = (cv::Mat_<float>(3, 3) <<
        calib_data->R_ir2rgb.at<double>(0, 0), calib_data->R_ir2rgb.at<double>(0, 1), calib_data->R_ir2rgb.at<double>(0, 2),
        calib_data->R_ir2rgb.at<double>(1, 0), calib_data->R_ir2rgb.at<double>(1, 1), calib_data->R_ir2rgb.at<double>(1, 2),
        calib_data->R_ir2rgb.at<double>(2, 0), calib_data->R_ir2rgb.at<double>(2, 1), calib_data->R_ir2rgb.at<double>(2, 2));

    // 初始化 T_ir2rgb
    param->T_ir2rgb = (cv::Mat_<float>(3, 1) <<
        calib_data->T_ir2rgb.at<double>(0) * depth_multi_ratio,
        calib_data->T_ir2rgb.at<double>(1) * depth_multi_ratio,
        calib_data->T_ir2rgb.at<double>(2) * depth_multi_ratio);

    // 计算 K_ir 的逆矩阵
    cv::Mat K_ir_inv = param->K_ir.inv();

    // 计算 R = K_rgb * R_ir2rgb * K_ir_inv
    param->R = param->K_rgb * param->R_ir2rgb * K_ir_inv;

    // 计算 T = K_rgb * T_ir2rgb
    param->T = param->K_rgb * param->T_ir2rgb;
    } else {
        cout << "calibration data do not exist!!!";
        return false;
    }

    return true;
}

void printStereoParam(const StereoParam* param)
{
    using namespace std;

    // 检查空指针
    if (!param) {
        cout << "Error: Null pointer passed to printStereoParam!" << endl;
        return;
    }

    // 设置浮点数输出格式
    cout << fixed << setprecision(6);

    cout << "===== StereoParam Structure Values ====================================" << endl;

    // RGB相机内参
    cout << "\n[RGB Camera Intrinsics]" << endl;
    cout << "fx_rgb: " << param->fx_rgb << endl;
    cout << "fy_rgb: " << param->fy_rgb << endl;
    cout << "cx_rgb: " << param->cx_rgb << endl;
    cout << "cy_rgb: " << param->cy_rgb << endl;

    // IR相机内参
    cout << "\n[IR Camera Intrinsics]" << endl;
    cout << "fx_ir: " << param->fx_ir << endl;
    cout << "fy_ir: " << param->fy_ir << endl;
    cout << "cx_ir: " << param->cx_ir << endl;
    cout << "cy_ir: " << param->cy_ir << endl;

    // 变换矩阵
    cout << "\n[Transformation Matrices]" << endl;
    cout << "invH:\n" << param->invH << endl;
    cout << "R_ir2rgb:\n" << param->R_ir2rgb << endl;
    //cout << "T_ir2rgb: [" << param->T_ir2rgb.transpose() << "]" << endl;

    // 相机矩阵
    cout << "\n[Camera Matrices]" << endl;
    cout << "K_ir:\n" << param->K_ir << endl;
    cout << "K_rgb:\n" << param->K_rgb << endl;
    cout << "R:\n" << param->R << endl;
    //cout << "T: [" << param->T.transpose() << "]" << endl;

    // LUT指针信息
    cout << "\n[Look-Up Tables]" << endl;
    cout << "LUT_INVH address: " << param->LUT_INVH << endl;
    cout << "LUT_FIXDEPTH address: " << param->LUT_FIXDEPTH << endl;
    cout << "LUT address: " << param->LUT << endl;

    // 尺寸参数
    cout << "\n[Size Parameters]" << endl;
    cout << "ir_calib_width: " << param->ir_calib_width << endl;
    cout << "ir_width: " << param->ir_width << endl;
    cout << "rgb_calib_width: " << param->rgb_calib_width << endl;
    cout << "rgb_crop_width: " << param->rgb_crop_width << endl;
    cout << "rgb_width: " << param->rgb_width << endl;
    cout << "rgb_height: " << param->rgb_height << endl;

    // 扩展参数
    cout << "\n[Extension Parameters]" << endl;
    cout << "extend_w_pixels: " << param->extend_w_pixels << endl;
    cout << "extend_h_pixels: " << param->extend_h_pixels << endl;
    cout << "rotateAngle: " << param->rotateAngle << endl;

    // 深度处理参数
    cout << "\n[Depth Processing Parameters]" << endl;
    cout << "depth_multi_ratio: " << param->depth_multi_ratio << endl;

    // 输出参数
    cout << "\n[Output Parameters]" << endl;
    cout << "fs: " << param->fs << endl;
    cout << "ref_distance: " << param->ref_distance << endl;
    cout << "out_fx: " << param->out_fx << endl;
    cout << "out_fy: " << param->out_fy << endl;
    cout << "out_cx: " << param->out_cx << endl;
    cout << "out_cy: " << param->out_cy << endl;
    cout << "out_baseline: " << param->out_baseline << endl;
    cout << "out_near_distance: " << param->out_near_distance << endl;
    cout << "out_far_distance: " << param->out_far_distance << endl;
    cout << "out_ori_focal: " << param->out_ori_focal << endl;

    // 目录路径
    cout << "\n[Directory]" << endl;
    cout << "dir: " << param->dir << endl;

    cout << "\n===== End of StereoParam Structure ======================================" << endl;
}

void printStereoCalibrationData(const StereoCalibrationData* data)
{
    if (!data) {
        std::cerr << "Error: Null pointer provided!" << std::endl;
        return;
    }

    std::cout << "===== StereoCalibrationData ==========================================\n";

    // IR相机参数
    std::cout << "\n[IR Camera Parameters]\n";
    std::cout << "Left Camera Resolution: " << data->l_width << "x" << data->l_height << "\n";
    std::cout << "Right Camera Resolution: " << data->r_width << "x" << data->r_height << "\n";

    std::cout << "\nLeft Camera Intrinsics (K_l):\n" << data->K_l << "\n";
    std::cout << "Left Camera Distortion (D_l):\n" << data->D_l << "\n";
    std::cout << "Right Camera Intrinsics (K_r):\n" << data->K_r << "\n";
    std::cout << "Right Camera Distortion (D_r):\n" << data->D_r << "\n";

    // 立体标定参数
    std::cout << "\n[Stereo Calibration Parameters]\n";
    std::cout << "Rotation Matrix (R):\n" << data->R << "\n";
    std::cout << "Translation Vector (T):\n" << data->T << "\n";
    std::cout << "Rectified Left Rotation (R1):\n" << data->R1 << "\n";
    std::cout << "Rectified Right Rotation (R2):\n" << data->R2 << "\n";
    std::cout << "Left Projection Matrix (P1):\n" << data->P1 << "\n";
    std::cout << "Right Projection Matrix (P2):\n" << data->P2 << "\n";
    std::cout << "Disparity-to-Depth Matrix (Q):\n" << data->Q << "\n";
    std::cout << "Left Inverse Projection (invPR1):\n" << data->invPR1 << "\n";
    std::cout << "Right Inverse Projection (invPR2):\n" << data->invPR2 << "\n";

    // RGB相机参数
    std::cout << "\n[RGB Camera Parameters]\n";
    std::cout << "RGB Resolution: " << data->rgb_width << "x" << data->rgb_height << "\n";
    std::cout << "RGB Intrinsics (K_rgb):\n" << data->K_rgb << "\n";
    std::cout << "RGB Distortion (D_rgb):\n" << data->D_rgb << "\n";

    // IR-RGB变换参数
    std::cout << "\n[IR-to-RGB Transformation]\n";
    std::cout << "Rotation (R_ir2rgb):\n" << data->R_ir2rgb << "\n";
    std::cout << "Translation (T_ir2rgb):\n" << data->T_ir2rgb << "\n";
    std::cout << "Rectified Left Rotation (R1_ir2rgb):\n" << data->R1_ir2rgb << "\n";
    std::cout << "Rectified Right Rotation (R2_ir2rgb):\n" << data->R2_ir2rgb << "\n";
    std::cout << "Left Projection (P1_ir2rgb):\n" << data->P1_ir2rgb << "\n";
    std::cout << "Right Projection (P2_ir2rgb):\n" << data->P2_ir2rgb << "\n";
    std::cout << "Disparity-to-Depth Matrix (Q_ir2rgb):\n" << data->Q_ir2rgb << "\n";

    // 标定误差
    std::cout << "\n[Calibration Errors]\n";
    std::cout << "Stereo RMS Error: " << data->stereo_rms << "\n";
    std::cout << "Left Camera RMS Error: " << data->left_rms << "\n";
    std::cout << "Right Camera RMS Error: " << data->right_rms << "\n";
    std::cout << "RGB Camera RMS Error: " << data->rgb_rms << "\n";
    std::cout << "IR-RGB Stereo RMS Error: " << data->ir_rgb_stereo_rms << "\n";

    std::cout << "===== End of Data =====================================================\n";
}

void printStereoCalibrationData(const StereoCalibrationData& data)
{
    // 辅助函数：打印cv::Mat矩阵
    auto printMat = [](const std::string& name, const cv::Mat& mat) {
        std::cout << name << " (" << mat.rows << "x" << mat.cols
                  << ", type: " << mat.type() << "):\n";

        if (mat.empty()) {
            std::cout << "  [Empty Matrix]\n";
            return;
        }

        // 根据矩阵大小决定打印方式
        if (mat.rows <= 10 && mat.cols <= 10) {
            std::cout << mat << "\n";
        } else {
            // 打印部分内容
            cv::Rect roi(0, 0, std::min(5, mat.cols), std::min(3, mat.rows));
            std::cout << mat(roi) << " ...\n";
        }
    };

    // 打印基础信息
    std::cout << "========== IR Camera Parameters ==========\n";
    std::cout << "Left Camera:  " << data.l_width << "x" << data.l_height << "\n";
    std::cout << "Right Camera: " << data.r_width << "x" << data.r_height << "\n";
    printMat("K_l", data.K_l);
    printMat("D_l", data.D_l);
    printMat("K_r", data.K_r);
    printMat("D_r", data.D_r);

    std::cout << "\n========== Stereo Calibration ==========\n";
    printMat("R", data.R);
    printMat("T", data.T);
    printMat("R1", data.R1);
    printMat("R2", data.R2);
    printMat("P1", data.P1);
    printMat("P2", data.P2);
    printMat("Q", data.Q);
    printMat("invPR1", data.invPR1);
    printMat("invPR2", data.invPR2);

    std::cout << "\n========== RGB Camera Parameters ==========\n";
    std::cout << "RGB Camera: " << data.rgb_width << "x" << data.rgb_height << "\n";
    printMat("K_rgb", data.K_rgb);
    printMat("D_rgb", data.D_rgb);

    std::cout << "\n========== IR-RGB Transformation ==========\n";
    printMat("R_ir2rgb", data.R_ir2rgb);
    printMat("T_ir2rgb", data.T_ir2rgb);
    printMat("R1_ir2rgb", data.R1_ir2rgb);
    printMat("R2_ir2rgb", data.R2_ir2rgb);
    printMat("P1_ir2rgb", data.P1_ir2rgb);
    printMat("P2_ir2rgb", data.P2_ir2rgb);
    printMat("Q_ir2rgb", data.Q_ir2rgb);

    std::cout << "\n========== Calibration Errors ==========\n";
    std::cout << "Stereo RMS: " << data.stereo_rms << "\n";
    std::cout << "Left RMS:   " << data.left_rms << "\n";
    std::cout << "Right RMS:  " << data.right_rms << "\n";
    std::cout << "RGB RMS:    " << data.rgb_rms << "\n";
    std::cout << "IR-RGB RMS: " << data.ir_rgb_stereo_rms << "\n";
}

void PrintAdjustStereoParams(const AdjustStereoParams& params)
{
    std::cout << "=========================== AdjustStereoParams =======================" << std::endl;

    // 打印基本参数
    std::cout << "\n[Camera Parameters]" << std::endl;
    std::cout << "RGB Camera: fx=" << params.fx_rgb << ", fy=" << params.fy_rgb
              << ", cx=" << params.cx_rgb << ", cy=" << params.cy_rgb << std::endl;
    std::cout << "IR Camera: fx=" << params.fx_ir << ", fy=" << params.fy_ir
              << ", cx=" << params.cx_ir << ", cy=" << params.cy_ir
              << ", depth_multi_ratio=" << params.depth_multi_ratio << std::endl;

    // 打印尺寸信息
    std::cout << "\n[Dimensions]" << std::endl;
    std::cout << "IR Calib Width: " << params.ir_calib_width << std::endl;
    std::cout << "RGB Calib Width: " << params.rgb_calib_width << std::endl;
    std::cout << "IR Resolution: " << params.ir_width << "x" << params.ir_height << std::endl;
    std::cout << "RGB Resolution: " << params.rgb_width << "x" << params.rgb_height << std::endl;
    std::cout << "Output Resolution: " << params.out_width << "x" << params.out_height << std::endl;

    // 打印标定参数
    std::cout << "\n[Calibration]" << std::endl;
    std::cout << "Baseline: " << params.baseline << std::endl;
    std::cout << "fx_origin: " << params.fx_origin << std::endl;

    // 打印输出相机参数
    std::cout << "\n[Output Camera]" << std::endl;
    std::cout << "Output fx: " << params.out_fx << std::endl;
    std::cout << "Output fy: " << params.out_fy << std::endl;
    std::cout << "Output cx: " << params.out_cx << std::endl;
    std::cout << "Output cy: " << params.out_cy << std::endl;

    // 打印扩展和旋转参数
    std::cout << "\n[Extension & Rotation]" << std::endl;
    std::cout << "Extend Width Pixels: " << params.extend_w_pixel << std::endl;
    std::cout << "Extend Height Pixels: " << params.extend_h_pixel << std::endl;
    std::cout << "Rotation Angle: " << params.rotateAngle << " degrees" << std::endl;

    // 打印OpenCV矩阵
    auto printMat = [](const std::string& name, const cv::Mat& mat) {
        std::cout << "\n[" << name << "]" << std::endl;
        if (mat.empty()) {
            std::cout << "  (Empty Matrix)" << std::endl;
            return;
        }

        std::cout << "  Size: " << mat.rows << "x" << mat.cols << std::endl;
        std::cout << "  Type: " << mat.type() << " (";
        switch (mat.type() & CV_MAT_DEPTH_MASK) {
            case CV_8U: std::cout << "8U"; break;
            case CV_8S: std::cout << "8S"; break;
            case CV_16U: std::cout << "16U"; break;
            case CV_16S: std::cout << "16S"; break;
            case CV_32S: std::cout << "32S"; break;
            case CV_32F: std::cout << "32F"; break;
            case CV_64F: std::cout << "64F"; break;
            default: std::cout << "Unknown"; break;
        }
        std::cout << ")" << std::endl;

        // 打印矩阵内容（最多3x3）
        int rows = std::min(mat.rows, 3);
        int cols = std::min(mat.cols, 3);

        for (int r = 0; r < rows; r++) {
            std::cout << "  ";
            for (int c = 0; c < cols; c++) {
                if (mat.type() == CV_32F) {
                    std::cout << mat.at<float>(r, c);
                } else if (mat.type() == CV_64F) {
                    std::cout << mat.at<double>(r, c);
                } else if (mat.type() == CV_32S) {
                    std::cout << mat.at<int>(r, c);
                } else {
                    std::cout << "(value)";
                }
                if (c < cols - 1) std::cout << ", ";
            }
            if (mat.cols > cols) std::cout << ", ...";
            std::cout << std::endl;
        }
        if (mat.rows > rows) std::cout << "  ..." << std::endl;
    };

    printMat("K_ir_s", params.K_ir_s);
    printMat("K_rgb_s", params.K_rgb_s);
    printMat("R_ir2rgb_s", params.R_ir2rgb_s);
    printMat("T_ir2rgb_s", params.T_ir2rgb_s);
    printMat("K_ir_s_inv", params.K_ir_s_inv);
    printMat("R", params.R);
    printMat("T", params.T);

    // 打印LUT（查找表）
    std::cout << "\n[LUT]" << std::endl;
    if (params.LUT == nullptr) {
        std::cout << "  LUT pointer is null" << std::endl;
    } else {
        int lutSize = params.ir_width * params.ir_height * 3;
        std::cout << "  LUT size: " << lutSize << " elements" << std::endl;

        // 打印前几个元素
        std::cout << "  First 6 elements: ";
        for (int i = 0; i < std::min(6, lutSize); i++) {
            std::cout << params.LUT[i];
            if (i < 5) std::cout << ", ";
        }
        if (lutSize > 6) std::cout << ", ...";
        std::cout << std::endl;
    }

    std::cout << "==================== END PRINT OF AdjustStereoParams ==================" << std::endl;
}
