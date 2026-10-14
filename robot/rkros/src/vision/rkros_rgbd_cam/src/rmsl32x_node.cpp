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

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "rmsl32x_node.h"

MonotonicToRosConverter::MonotonicToRosConverter() {
    timespec mono_ts;
    clock_gettime(CLOCK_MONOTONIC, &mono_ts);

    // ROS2 时间获取方式
    auto ros_now = rclcpp::Clock().now();
    initial_offset_ = ros_now.seconds() - (mono_ts.tv_sec + mono_ts.tv_nsec * 1e-9);

    RMSL_INFO("Simple time converter initialized:");
    RMSL_INFO("  Monotonic time: %ld.%09ld", mono_ts.tv_sec, mono_ts.tv_nsec);
    RMSL_INFO("  ROS time: %.9f", ros_now.seconds());
    RMSL_INFO("  Initial offset: %.9f seconds", initial_offset_);
}

rclcpp::Time MonotonicToRosConverter::monotonicToRosTime(const timespec& mono_ts) {
    double monotonic_seconds = mono_ts.tv_sec + mono_ts.tv_nsec * 1e-9;
    double ros_seconds = monotonic_seconds + initial_offset_ + calibration_offset_;

    // ROS2 时间构造方式
    int64_t nanoseconds = static_cast<int64_t>(ros_seconds * 1e9);
    return rclcpp::Time(nanoseconds);
}

rclcpp::Time MonotonicToRosConverter::usToRosTime(uint64_t timestamp_us) {
    double timestamp_seconds = timestamp_us * 1e-6;
    double ros_seconds = timestamp_seconds + initial_offset_ + calibration_offset_;

    int64_t nanoseconds = static_cast<int64_t>(ros_seconds * 1e9);
    return rclcpp::Time(nanoseconds);
}

rclcpp::Time MonotonicToRosConverter::getMonotonicRosTime() {
    timespec mono_ts;
    clock_gettime(CLOCK_MONOTONIC, &mono_ts);
    return monotonicToRosTime(mono_ts);
}

void MonotonicToRosConverter::calibrate() {
    timespec mono_ts;
    clock_gettime(CLOCK_MONOTONIC, &mono_ts);

    rclcpp::Time expected = monotonicToRosTime(mono_ts);
    rclcpp::Time actual = rclcpp::Clock().now();

    // ROS2 时间差计算方式
    double error = (actual - expected).seconds();

    RMSL_INFO("Calibration - error: %.6f seconds (%.3f ms)", error, error * 1000);

    const double CALIBRATION_THRESHOLD = 0.01;
    if (std::abs(error) > CALIBRATION_THRESHOLD) {
        calibration_offset_ += error * 0.5;
        RMSL_INFO("Adjusted calibration offset to: %.9f seconds", calibration_offset_);
    }
}

void MonotonicToRosConverter::debugTimeState() {
    timespec mono_ts;
    clock_gettime(CLOCK_MONOTONIC, &mono_ts);

    rclcpp::Time monotonic_time = monotonicToRosTime(mono_ts);
    rclcpp::Time system_time = rclcpp::Clock().now();
}

void CamInfo::setFromParams(const AdjustStereoParams& param)
{
    // 填充 IR 相机信息
    cam_info_ir_.header.frame_id = "ir_optical_frame";
    cam_info_ir_.width  = param.ir_width;
    cam_info_ir_.height = param.ir_height;
    cam_info_ir_.distortion_model = "plumb_bob";
    cam_info_ir_.d = {}; // 若无畸变参数，可为空

    // 内参矩阵 K_ir
    cam_info_ir_.k = {
        param.fx_ir, 0.0, param.cx_ir,
        0.0, param.fy_ir, param.cy_ir,
        0.0, 0.0, 1.0
    };

    // 投影矩阵 P_ir
    cam_info_ir_.p = {
        param.fx_ir, 0.0, param.cx_ir, 0.0,
        0.0, param.fy_ir, param.cy_ir, 0.0,
        0.0, 0.0, 1.0, 0.0
    };

    cam_info_ir_.r = {1.0, 0.0, 0.0,
                      0.0, 1.0, 0.0,
                      0.0, 0.0, 1.0};

    // 填充 RGB 相机信息
    cam_info_rgb_.header.frame_id = "rgb_optical_frame";
    cam_info_rgb_.width  = param.rgb_width;
    cam_info_rgb_.height = param.rgb_height;
    cam_info_rgb_.distortion_model = "plumb_bob";
    cam_info_rgb_.d = {};

    cam_info_rgb_.k = {
        param.fx_rgb, 0.0, param.cx_rgb,
        0.0, param.fy_rgb, param.cy_rgb,
        0.0, 0.0, 1.0
    };

    // 考虑双目基线影响（投影矩阵）
    // baseline 单位：米（左为参考，右为 baseline 偏移）
    double Tx = -param.fx_rgb * param.baseline;
    cam_info_rgb_.p = {
        param.fx_rgb, 0.0, param.cx_rgb, Tx,
        0.0, param.fy_rgb, param.cy_rgb, 0.0,
        0.0, 0.0, 1.0, 0.0
    };

    cam_info_rgb_.r = {1.0, 0.0, 0.0,
                       0.0, 1.0, 0.0,
                       0.0, 0.0, 1.0};
}

void CamInfo::updateTimestamps(const rclcpp::Time& stamp_rgb, const rclcpp::Time& stamp_ir)
{
    cam_info_ir_.header.stamp = stamp_ir;
    cam_info_rgb_.header.stamp = stamp_rgb;
}

RMSLNode::RMSLNode(const rclcpp::NodeOptions & options)
: Node("rmsl32x_node", options) {

    time_converter_ = std::make_unique<MonotonicToRosConverter>();
    cam_info_ = std::make_unique<CamInfo>();

    this->declare_parameter("rgb_width", 640);
    this->declare_parameter("rgb_height", 540);
    this->declare_parameter("ir_width", 640);
    this->declare_parameter("ir_height", 352);

    this->declare_parameter("publish_rate", 10.0);
    this->declare_parameter("depth_multi_ratio", 1.0);
    this->declare_parameter("rotateAngle", 0);
    this->declare_parameter("cam_name", "rmsl32x_cam");
    this->declare_parameter("rgb_fps", 10);
    this->declare_parameter("enable_calib_time", true);
    this->declare_parameter("calib_interval", 60.0);

    this->declare_parameter("enable_depth", true);
    this->declare_parameter("enable_depth_pseudo", false);
    this->declare_parameter("enable_depth_aligned", true);
    this->declare_parameter("enable_pointcloud", false);
    this->declare_parameter("enable_pointcloud_aligned", false);
    this->declare_parameter("enable_pointcloud_rgbd", false);
    this->declare_parameter("enable_debug_ir", false);

    rgb_crop_width_         = 1280;
    rgb_width_              = this->get_parameter("rgb_width").as_int();
    rgb_height_             = this->get_parameter("rgb_height").as_int();
    ir_width_               = this->get_parameter("ir_width").as_int();
    ir_height_              = this->get_parameter("ir_height").as_int();
    rotateAngle_            = this->get_parameter("rotateAngle").as_int();
    publish_rate_           = this->get_parameter("publish_rate").as_double();
    depth_multi_ratio_      = this->get_parameter("depth_multi_ratio").as_double();

    enable_depth_           = this->get_parameter("enable_depth").as_bool();
    enable_depth_pseudo_    = this->get_parameter("enable_depth_pseudo").as_bool();
    enable_depth_aligned_   = this->get_parameter("enable_depth_aligned").as_bool();
    enable_pointcloud_      = this->get_parameter("enable_pointcloud").as_bool();
    enable_pointcloud_rgbd_ = this->get_parameter("enable_pointcloud_rgbd").as_bool();
    enable_pcl_aligned_     = this->get_parameter("enable_pointcloud_aligned").as_bool();
    enable_debug_ir_        = this->get_parameter("enable_debug_ir").as_bool();
    std::string cam_name    = this->get_parameter("cam_name").as_string();
    int rgb_fps             = this->get_parameter("rgb_fps").as_int();
    bool enable_calib_time  = this->get_parameter("enable_calib_time").as_bool();
    double calib_interval   = this->get_parameter("calib_interval").as_double();

    char dev_depth[32] = {0};
    char dev_rgb[32]   = {0};
    char dev_ir[32]    = {0};

    int debug_uvc = 1;
    int ret = rmsl_get_devices(dev_depth, dev_rgb, dev_ir, debug_uvc);
    if (ret) {
        RCLCPP_INFO(this->get_logger(), "Failed to get RMSL32x devices");
        return ;
    }

    RCLCPP_INFO(this->get_logger(), "get 1. color:%s  2. depth:%s  3.ir:%s", dev_rgb, dev_depth, dev_ir);

    int rgb_fd = open(dev_rgb, O_RDWR | O_CLOEXEC, 0);
    char version[RMSL_DATA_SIZE_QUERY] = {0};
    ret = rmsl_get_version(rgb_fd, version, RMSL_DATA_SIZE_QUERY);
    if (ret) {
        RCLCPP_ERROR(this->get_logger(), "Failed to get RMSL32x version");
    } else {
        RCLCPP_INFO(this->get_logger(), "RMSL32x Camera version: %s", version);
    }

    UVC_StereoCameraParams param;
    ret = rmsl_get_calib_data(rgb_fd, &param, debug_uvc);
    if (ret) {
        RMSL_ERROR("Failed to get calibration data from UVC");
    }

    //rmsl_reboot_to_loader(rgb_fd);
    close(rgb_fd);

    ret = loadCalibrationFromUVC(param);
    //printStereoCalibrationData(calib_data_);
    ret = AdjustCalibData(calib_data_, adjust_params_);
    //PrintAdjustStereoParams(adjust_params_);
    cam_info_->setFromParams(adjust_params_);

    try {
        img_proc_ = std::make_unique<ImageProcessor>(adjust_params_);
    } catch (const std::exception& e) {
        RCLCPP_FATAL(this->get_logger(), "Failed to initialize params: %s", e.what());
        throw;
    }

    RCLCPP_INFO(this->get_logger(), "Initializing RMSLNode...");

    if (enable_debug_ir_) {
        RCLCPP_INFO(this->get_logger(), "RMSL32x Debug IR mode enabled");
        try {
            cap_debug_ = std::make_unique<V4L2TripleCapture>(
                dev_rgb,dev_depth, dev_ir,
                rgb_width_, rgb_height_,V4L2_PIX_FMT_NV12,rgb_fps,
                ir_width_, ir_height_, V4L2_PIX_FMT_YUYV,
                1080,1280, V4L2_PIX_FMT_NV12);
        } catch (const std::exception& e) {
            RCLCPP_FATAL(this->get_logger(), "Debug IR: Failed to initialize rmsl camera: %s", e.what());
            throw;
        }
        //cap_debug_->setMaxPairDiffMs(5);
        cap_debug_->start();
    } else {
        try {
            cap_ = std::make_unique<V4L2DualCapture>(
                dev_rgb, dev_depth,
                rgb_width_, rgb_height_,V4L2_PIX_FMT_NV12, rgb_fps,
                ir_width_, ir_height_, V4L2_PIX_FMT_YUYV, rgb_fps);
        } catch (const std::exception& e) {
            RCLCPP_FATAL(this->get_logger(), "Failed to initialize rmsl camera: %s", e.what());
            throw;
        }
        cap_->setMaxPairDiffMs(5); // 单位ms::容忍 5ms 内的 timestamp 差
        // 若摄像头 A 比 B 滞后 ~200ms（A 的图像时间晚于 B），为了把 A 与 B 对齐，把 A 的时间戳减去 200ms (即 offset = -200)
        //cap_->setTimeOffsetMs(0, 50); // stream 0 (A) 时间戳修正：减去 200ms
        cap_->start();
    }

    RCLCPP_INFO(this->get_logger(), "RMSL321X_ cap started successfully.");

    std:: string topic;
    auto qos = rclcpp::QoS(10);
    topic = cam_name + "/color/image_rgb";
    this->rgb_pub_ = this->create_publisher<sensor_msgs::msg::Image>(topic, qos);
    topic = cam_name + "/color/camera_info";
    this->color_info_pub_ = this->create_publisher<sensor_msgs::msg::CameraInfo>(topic, qos);

    topic = cam_name + "/depth/image_depth";
    this->depth_pub_ = this->create_publisher<sensor_msgs::msg::Image>(topic, qos);
    topic = cam_name + "/depth/camera_info";
    this->depth_info_pub_ = this->create_publisher<sensor_msgs::msg::CameraInfo>(topic, qos);

    if (enable_pointcloud_) {
        topic = cam_name + "/depth/points";
        this->pointcloud_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(topic, qos);
    }

    if (enable_depth_aligned_) {
        topic = cam_name + "/depth_registered/depth_aligned";
        this->aligned_depth_pub_ = this->create_publisher<sensor_msgs::msg::Image>(topic, qos);
    }

    if (enable_pcl_aligned_) {
        topic = cam_name + "/depth_registered/points_aligned";
        this->aligned_pointcloud_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(topic, qos);
    }

    if (enable_depth_pseudo_) {
        topic = cam_name + "/depth/depth_pseudo";
        this->raw_pub_ = this->create_publisher<sensor_msgs::msg::Image>(topic, qos);
    }

    // 创建 TF 广播器
    //this->tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    static_tf_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
    publishStaticTransforms();

    if (enable_calib_time) {
        // 立即执行首次校准
        time_converter_->calibrate();

        // 创建周期性校准定时器
        calib_timer_ = this->create_wall_timer(
            std::chrono::duration<double>(calib_interval),
            [this]() {
                RCLCPP_DEBUG(this->get_logger(), "Performing periodic time calibration");
                try {
                    time_converter_->calibrate();
                } catch (const std::exception& e) {
                    RCLCPP_ERROR(this->get_logger(), "Calibration failed: %s", e.what());
                }
            }
        );
        RCLCPP_INFO(this->get_logger(),
                    "Time calibration enabled with %.1f second interval",
                    calib_interval);
    } else {
        RCLCPP_INFO(this->get_logger(), "Time calibration disabled");
    }

    if (enable_pcl_aligned_) {
        hand_roi_ = cv::Rect(0, rgb_height_ == 1080 ? 188 : 94,
                             rgb_width_, rgb_height_ == 1080 ? 704 : 352);
        RMSL_INFO("PCL CROP Params - x: %d, y: %d, width: %d, height: %d",
            hand_roi_.x, hand_roi_.y, hand_roi_.width, hand_roi_.height);
    }

    // the main timer for publishing data
    double interval_ms = 1000.0 / this->publish_rate_;
    this->timer_ = this->create_wall_timer(
        std::chrono::milliseconds(static_cast<int>(interval_ms)),
        std::bind(&RMSLNode::timerCallback, this));
    RCLCPP_INFO(this->get_logger(), "RMSLNode initialized, publishing at %.2f Hz", this->publish_rate_);
}

void RMSLNode::timerCallback() {
    std::shared_ptr<Frame> a, b, c;
    bool ok;
    if (enable_debug_ir_) {
        ok = cap_debug_->getLatestPair_debug(a, b, c, 300);
    } else {
        ok = cap_->getSyncedPair_Drive_B(a, b, 300);
    }

    //std::shared_ptr<Frame> a, b;
    //bool ok = cap_->getSyncedPair_Drive_B(a, b, 300); // 等待最多 300ms

    if (!ok) {
        RMSL_WARN("No synced pair");
        return;
    } else {
        RMSL_DEBUG("Get synced pair: A ts=%lu, B ts=%lu, diff=%ld us",
                   a->timestamp_us, b->timestamp_us,
                   std::labs((int64_t)a->timestamp_us - (int64_t)b->timestamp_us));
    }
    // 使用 a->toCvMat() 等读取数据（注意 YUYV 需 cvtColor）
    cv::Mat Araw = a->toCvMat();
    cv::Mat Braw = b->toCvMat();
    cv::Mat Aimg, Bimg;
    cv::Mat depth_aligned;
    rclcpp::Time ros_time_a, ros_time_b, ros_time_c;

    cv::Mat Craw;
    if (enable_debug_ir_) {
        Craw = c->toCvMat();
    }

    auto rgb_info = cam_info_->getRgbInfo();
    auto ir_info  = cam_info_->getIrInfo();

    if (!Araw.empty() && !Braw.empty()) {
        if (a->pixelformat == V4L2_PIX_FMT_NV12) {
            cv::cvtColor(Araw, Aimg, cv::COLOR_YUV2BGR_NV12);

            auto rgb_msg = cv_bridge::CvImage(
                std_msgs::msg::Header(), "bgr8", Aimg).toImageMsg();
            ros_time_a = time_converter_->usToRosTime(a->timestamp_us);
            rgb_msg->header.stamp = ros_time_a;

            // test the dalay time
            // uint64_t us_a = getCurrentTimeUs_uvc();
            // RCLCPP_INFO(this->get_logger(), "a.timestamp_us: %llu, cur_time: %llu", a->timestamp_us, us_a);
            // uint64_t diff_us = us_a - b->timestamp_us;
            // RCLCPP_INFO(this->get_logger(), "delay_ : %.3f ms", (us_a- b->timestamp_us)/1e3);

            rgb_msg->header.frame_id = "camera";
            rgb_pub_->publish(*rgb_msg);

        } else if (a->pixelformat == V4L2_PIX_FMT_MJPEG) {
            Aimg = cv::imdecode(Araw, cv::IMREAD_COLOR);
            if (Aimg.empty()) {
                RMSL_ERROR("Failed to decode MJPEG image");
                return;
            }

            auto rgb_msg = cv_bridge::CvImage(
                std_msgs::msg::Header(), "bgr8", Aimg).toImageMsg();
            ros_time_a = time_converter_->usToRosTime(a->timestamp_us);
            rgb_msg->header.stamp = ros_time_a;
            rgb_msg->header.frame_id = "camera";
            rgb_pub_->publish(*rgb_msg);
        }

        if (b->pixelformat == V4L2_PIX_FMT_YUYV) {
            Bimg = img_proc_->disp_to_depth_16u(Braw, baseline_, fx_origin_);

            auto depth_msg = cv_bridge::CvImage(
                std_msgs::msg::Header(), "16UC1", Bimg).toImageMsg();
            rclcpp::Time ros_time_b = time_converter_->usToRosTime(b->timestamp_us);

            // publish rgb and depth cam_info after both time are ready
            cam_info_->updateTimestamps(ros_time_a, ros_time_b);
            color_info_pub_->publish(rgb_info);
            depth_info_pub_->publish(ir_info);

            depth_msg->header.stamp = ros_time_b;
            depth_msg->header.frame_id = "camera";
            if (!enable_debug_ir_) {
                depth_pub_->publish(*depth_msg);
            }
            //depth_pub_->publish(*depth_msg);

            if (enable_depth_aligned_) {
                depth_aligned = img_proc_->alignDepthToRGB(Bimg);
                auto aligned_depth_msg = cv_bridge::CvImage(
                    std_msgs::msg::Header(), "16UC1", depth_aligned).toImageMsg();
                aligned_depth_msg->header.stamp = ros_time_b;
                aligned_depth_msg->header.frame_id = "camera";
                if (!enable_debug_ir_) {
                    aligned_depth_pub_->publish(*aligned_depth_msg);
                }
            }

            if (enable_depth_pseudo_) {
                auto color_dp = img_proc_->convertDepthToPseudoColor(Bimg);
                auto color_dp_msg = cv_bridge::CvImage(
                    std_msgs::msg::Header(), "bgr8", color_dp).toImageMsg();
                //rclcpp::Time ros_time = time_converter_->usToRosTime(b->timestamp_us);
                color_dp_msg->header.stamp = ros_time_b;
                color_dp_msg->header.frame_id = "camera";
                raw_pub_->publish(*color_dp_msg);
            }

            if (enable_pointcloud_) {
                std::shared_ptr<sensor_msgs::msg::PointCloud2> pcl_msg;
                pcl_msg = img_proc_->convertDepthToPointCloud_depthonly(
                    Bimg, "camera_frame", false, cv::Rect());
                if (pcl_msg) {
                    pcl_msg->header.stamp = ros_time_b;
                    pointcloud_pub_->publish(*pcl_msg);
                }
            }

            if (enable_pcl_aligned_ && enable_depth_) {
                std::shared_ptr<sensor_msgs::msg::PointCloud2> aligned_pcl_msg;
                if (enable_pointcloud_rgbd_ &&  enable_depth_aligned_) {
                    aligned_pcl_msg = img_proc_->convertDepthToPointCloud_rgbd(
                        depth_aligned, Aimg, "camera_frame", true, hand_roi_);
                } else {
                    aligned_pcl_msg = img_proc_->convertDepthToPointCloud_depthonly(
                        Bimg, "camera_frame", true, hand_roi_);
                }

                if (aligned_pcl_msg) {
                    aligned_pcl_msg->header.stamp = ros_time_b;
                    aligned_pointcloud_pub_->publish(*aligned_pcl_msg);
                }
            }
        }
        // when debug ir, borrowed the publisher of depth and depth_aligned to publish debug ir image and aligned depth
        // so need to turn off the publisher in depth
        if (enable_debug_ir_) {
            rclcpp::Time ros_time_c = ros_time_a;
            int original_height = 1280;
            int original_width = 1080;

            if (Craw.rows != original_height * 3 / 2 || Craw.cols != original_width) {
                RCLCPP_ERROR(this->get_logger(),
                            "NV12 size is not expected: expect %dx%d, actually %dx%d",
                            original_width, original_height * 3 / 2,
                            Craw.cols, Craw.rows);
            }

            // 提取 Y 平面
            cv::Mat y_plane = Craw(cv::Rect(0, 0, original_width, original_height));

            // 步骤1: 缩放到 540x640
            cv::Mat Y_resized;
            cv::resize(y_plane, Y_resized, cv::Size(540, 640), 0, 0, cv::INTER_LINEAR);

            int crop_left = (540 - 352) / 2;  // 计算左右裁剪的偏移量
            if (crop_left < 0 || crop_left + 352 > Y_resized.cols) {
                RCLCPP_ERROR(this->get_logger(),
                            "裁剪区域超出图像宽度范围: crop_left=%d, 图像宽度=%d",
                            crop_left, Y_resized.cols);
                return;
            }

            // 裁剪区域: x=crop_left, y=0, 宽度=352, 高度=640
            cv::Rect crop_roi(crop_left, 0, 352, 640);
            cv::Mat Y_cropped = Y_resized(crop_roi);

            // 步骤3: 旋转90度得到 640x352
            cv::Mat Y_rotated;
            cv::rotate(Y_cropped, Y_rotated, cv::ROTATE_90_CLOCKWISE);

            cv::Mat Cimg_8c;
            Y_rotated.convertTo(Cimg_8c, CV_8U);
            RMSL_INFO("Debug IR 图像转换完成，尺寸: %dx%d", Cimg_8c.cols, Cimg_8c.rows);

            cv::Mat map1, map2;
            cv::Mat cameraMatrix = calib_data_.K_l;
            cv::Mat distCoeffs = calib_data_.D_l;
            cv::Size calibSize(640, 352);
            cv::Mat R = calib_data_.R1;
            cv::Mat P = calib_data_.P1;
            if(map1.empty() || map2.empty()) {
                initUndistortRectifyMap(cameraMatrix, distCoeffs, R, P, calibSize, CV_32FC1, map1, map2);
            }
            cv::Mat dst, res;
            remap(Cimg_8c, dst, map1, map2, cv::INTER_LINEAR);
            res = img_proc_->alignDepthIRToRGB(Bimg, dst);

            auto depth_msg = cv_bridge::CvImage(
                std_msgs::msg::Header(), "mono8", Cimg_8c).toImageMsg();
            depth_msg->header.stamp = ros_time_c;
            depth_msg->header.frame_id = "camera";
            depth_pub_->publish(*depth_msg);
            RMSL_DEBUG("IR align to rgb is done");

            if (!res.empty()) {
                double min_val, max_val;
                cv::minMaxLoc(res, &min_val, &max_val);

                if (min_val >= 0 && max_val <= 255) {
                    cv::Mat depth_8uc1;
                    res.convertTo(depth_8uc1, CV_8U);
                    RMSL_DEBUG("Aligned depth image converted to 8UC1 format");

                    // 发布8UC1深度图像
                    auto aligned_depth_msg = cv_bridge::CvImage(
                        std_msgs::msg::Header(), "mono8", depth_8uc1).toImageMsg();
                    aligned_depth_msg->header.stamp = ros_time_c;
                    aligned_depth_msg->header.frame_id = "camera";
                    aligned_depth_pub_->publish(*aligned_depth_msg);
                } else {
                    // 值超出8位范围，无法保持数值不变
                    RMSL_WARN("the range of depth values is [%.0f, %.0f], which exceeds the 8-bit range", min_val, max_val);
                }
            }
        }
    }
}

bool RMSLNode::loadCalibrationFromUVC(const UVC_StereoCameraParams& uvc_params) {

    baseline_ = std::abs(uvc_params.params.T[0]); // baseline in mm
    fx_origin_ = uvc_params.params.P1[0]; // fx
    // 复制基本尺寸参数
    calib_data_.l_width = uvc_params.params.l_width;
    calib_data_.l_height = uvc_params.params.l_height;
    calib_data_.r_width = uvc_params.params.r_width;
    calib_data_.r_height = uvc_params.params.r_height;
    calib_data_.rgb_width = uvc_params.params.rgb_width;
    calib_data_.rgb_height = uvc_params.params.rgb_height;

    // 复制标定误差
    calib_data_.stereo_rms = uvc_params.params.stereo_rms;
    calib_data_.left_rms = uvc_params.params.left_rms;
    calib_data_.right_rms = uvc_params.params.right_rms;
    calib_data_.rgb_rms = uvc_params.params.rgb_rms;
    calib_data_.ir_rgb_stereo_rms = uvc_params.params.ir_rgb_stereo_rms;

    // 转换内参矩阵 (3x3)
    calib_data_.K_l = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.K_l)).clone();
    calib_data_.K_r = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.K_r)).clone();
    calib_data_.K_rgb = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.K_rgb)).clone();

    // 转换畸变系数
    calib_data_.D_l = cv::Mat(1, VEC14_SIZE, CV_64F, const_cast<double*>(uvc_params.params.D_l)).clone();
    calib_data_.D_r = cv::Mat(1, VEC14_SIZE, CV_64F, const_cast<double*>(uvc_params.params.D_r)).clone();
    calib_data_.D_rgb = cv::Mat(1, VEC12_SIZE, CV_64F, const_cast<double*>(uvc_params.params.D_rgb)).clone();

    // 转换立体标定参数
    calib_data_.R = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.R)).clone();
    calib_data_.T = cv::Mat(3, 1, CV_64F, const_cast<double*>(uvc_params.params.T)).clone();
    calib_data_.R1 = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.R1)).clone();
    calib_data_.R2 = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.R2)).clone();

    // 转换投影矩阵 (3x4)
    calib_data_.P1 = cv::Mat(3, 4, CV_64F, const_cast<double*>(uvc_params.params.P1)).clone();
    calib_data_.P2 = cv::Mat(3, 4, CV_64F, const_cast<double*>(uvc_params.params.P2)).clone();

    // 转换重投影矩阵 (4x4)
    calib_data_.Q = cv::Mat(4, 4, CV_64F, const_cast<double*>(uvc_params.params.Q)).clone();

    // 转换逆投影矩阵 (3x3)
    calib_data_.invPR1 = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.invPR1)).clone();
    calib_data_.invPR2 = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.invPR2)).clone();

    // 转换IR-RGB变换参数
    calib_data_.R_ir2rgb = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.R_ir2rgb)).clone();
    calib_data_.T_ir2rgb = cv::Mat(3, 1, CV_64F, const_cast<double*>(uvc_params.params.T_ir2rgb)).clone();
    calib_data_.R1_ir2rgb = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.R1_ir2rgb)).clone();
    calib_data_.R2_ir2rgb = cv::Mat(3, 3, CV_64F, const_cast<double*>(uvc_params.params.R2_ir2rgb)).clone();
    calib_data_.P1_ir2rgb = cv::Mat(3, 4, CV_64F, const_cast<double*>(uvc_params.params.P1_ir2rgb)).clone();
    calib_data_.P2_ir2rgb = cv::Mat(3, 4, CV_64F, const_cast<double*>(uvc_params.params.P2_ir2rgb)).clone();
    calib_data_.Q_ir2rgb = cv::Mat(4, 4, CV_64F, const_cast<double*>(uvc_params.params.Q_ir2rgb)).clone();

    return true;
}

bool RMSLNode::AdjustCalibData(StereoCalibrationData& calib_data, AdjustStereoParams& params) {
    params.baseline = std::abs(calib_data.T.at<double>(0)); // baseline in mm
    params.fx_origin = calib_data.P1.at<double>(0, 0); // fx

    params.ir_width = ir_width_;
    params.ir_height = ir_height_;
    params.rgb_width = rgb_width_;
    params.rgb_height = rgb_height_;
    params.rotateAngle = rotateAngle_;
    params.depth_multi_ratio = depth_multi_ratio_;
    params.extend_w_pixel = extend_w_pixel_;
    params.extend_h_pixel = extend_h_pixel_;

    if (params.rotateAngle == 0 || params.rotateAngle == 180) {
        params.out_width = rgb_width_ + extend_w_pixel_ * 2;
        params.out_height = rgb_height_ + extend_h_pixel_ * 2;
    } else {
        params.out_width = rgb_height_ + extend_w_pixel_ * 2;
        params.out_height = rgb_width_ + extend_h_pixel_ * 2;
    }

    params.ir_calib_width = calib_data.l_width;
    params.rgb_calib_width = calib_data.rgb_width;
    float ir_ratio = (float)ir_width_ / (float)params.ir_calib_width;
    float rgb_ratio = (float)rgb_width_ / (float)rgb_crop_width_;
    float depth_multi_ratio = depth_multi_ratio_;

    // std:: cout << "ir_ratio: " << ir_ratio << ", rgb_ratio: " << rgb_ratio << std::endl;
    // std:: cout << "rgb_crop_width_: " << rgb_crop_width_ << ", params.rgb_calib_width: " << params.rgb_calib_width << std::endl;
    // std:: cout << "ir_width_: " << ir_width_ << ", params.ir_calib_width: " << params.ir_calib_width << std::endl;

    int cx_rgb_offset = (rgb_crop_width_ - params.rgb_calib_width) / 2;

    params.fx_ir = calib_data.K_l.at<double>(0, 0) * ir_ratio;
    params.fy_ir = calib_data.K_l.at<double>(1, 1) * ir_ratio;
    params.cx_ir = calib_data.K_l.at<double>(0, 2) * ir_ratio;
    params.cy_ir = calib_data.K_l.at<double>(1, 2) * ir_ratio;
    params.fx_rgb = calib_data.K_rgb.at<double>(0, 0) * rgb_ratio;
    params.fy_rgb = calib_data.K_rgb.at<double>(1, 1) * rgb_ratio;
    params.cx_rgb = (calib_data.K_rgb.at<double>(0, 2) + cx_rgb_offset) * rgb_ratio;
    params.cy_rgb = calib_data.K_rgb.at<double>(1, 2) * rgb_ratio;

    params.P1 = calib_data.P1;

    if (params.rotateAngle == 270) {
        params.out_fx = params.fy_rgb;
        params.out_fy = params.fx_rgb;
        params.out_cx = params.rgb_height - params.cy_rgb;
        params.out_cy = params.cx_rgb;
    } else if (params.rotateAngle == 90) {
        params.out_fx = params.fy_rgb;
        params.out_fy = params.fx_rgb;
        params.out_cx = params.cy_rgb;
        params.out_cy = params.rgb_width - params.cx_rgb;
    }

    params.K_ir_s = (cv::Mat_<float>(3, 3) <<
        params.fx_ir, 0, params.cx_ir,
        0, params.fy_ir, params.cy_ir,
        0, 0, 1);
    params.K_rgb_s = (cv::Mat_<float>(3, 3) <<
        params.fx_rgb, 0, params.cx_rgb,
        0, params.fy_rgb, params.cy_rgb,
        0, 0, 1);

    cv::Mat invPR;
    invPR = (cv::Mat_<float>(3, 3) <<
        calib_data.invPR1.at<double>(0, 0), calib_data.invPR1.at<double>(0, 1), calib_data.invPR1.at<double>(0, 2),
        calib_data.invPR1.at<double>(1, 0), calib_data.invPR1.at<double>(1, 1), calib_data.invPR1.at<double>(1, 2),
        calib_data.invPR1.at<double>(2, 0), calib_data.invPR1.at<double>(2, 1), calib_data.invPR1.at<double>(2, 2));

    cv::Mat ratio2;
    ratio2 = (cv::Mat_<float>(3, 3) <<
        1 / ir_ratio, 0, 0,
        0, 1 / ir_ratio, 0,
        0, 0, 1);
    invPR = invPR * ratio2;


    params.R_ir2rgb_s = (cv::Mat_<float>(3, 3) <<
        calib_data.R_ir2rgb.at<double>(0, 0), calib_data.R_ir2rgb.at<double>(0, 1), calib_data.R_ir2rgb.at<double>(0, 2),
        calib_data.R_ir2rgb.at<double>(1, 0), calib_data.R_ir2rgb.at<double>(1, 1), calib_data.R_ir2rgb.at<double>(1, 2),
        calib_data.R_ir2rgb.at<double>(2, 0), calib_data.R_ir2rgb.at<double>(2, 1), calib_data.R_ir2rgb.at<double>(2, 2));
    params.T_ir2rgb_s = (cv::Mat_<float>(3, 1) <<
        calib_data.T_ir2rgb.at<double>(0) * depth_multi_ratio,
        calib_data.T_ir2rgb.at<double>(1) * depth_multi_ratio,
        calib_data.T_ir2rgb.at<double>(2) * depth_multi_ratio);
    params.K_ir_s_inv = params.K_ir_s.inv();

    params.R = params.K_rgb_s * params.R_ir2rgb_s * invPR;
    //params.R = params.K_rgb_s * params.R_ir2rgb_s * params.K_ir_s_inv;
    params.T = params.K_rgb_s * params.T_ir2rgb_s;
    //params.T = params.T_ir2rgb_s;

    return true;
}

void RMSLNode::publishStaticTransforms() {
    std::vector<geometry_msgs::msg::TransformStamped> transforms;

    // 1. map -> camera_frame
    geometry_msgs::msg::TransformStamped transform;
    transform.header.stamp = this->now();
    transform.header.frame_id = "map";
    transform.child_frame_id = "camera_frame";

    transform.transform.translation.x = 0.0;
    transform.transform.translation.y = 0.0;
    transform.transform.translation.z = 0.0;

    tf2::Quaternion q;
    // default no rotation
    q.setRPY(0, 0, 0);
    transform.transform.rotation.x = q.x();
    transform.transform.rotation.y = q.y();
    transform.transform.rotation.z = q.z();
    transform.transform.rotation.w = q.w();

    transforms.push_back(transform);

    // 2. camera_frame -> camera_depth_optical_frame
    // 3. camera_frame -> camera_rgb_optical_frame
    // 添加其他变换...

    // publish all transforms
    static_tf_broadcaster_->sendTransform(transforms);
}

RCLCPP_COMPONENTS_REGISTER_NODE(RMSLNode)

#ifndef COMPOSITION_BUILD
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RMSLNode>(rclcpp::NodeOptions()));
    rclcpp::shutdown();
    return 0;
}
#endif
