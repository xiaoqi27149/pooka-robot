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
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <cv_bridge/cv_bridge.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include "rclcpp_components/register_node_macro.hpp"

#include "rmsl32x_usb.h"
#include "rmsl32x_param.h"
#include "rmsl_image_processor.h"
#include "rmsl_uvc.h"
#include "rmsl_logger.h"

class MonotonicToRosConverter
{
public:
    MonotonicToRosConverter();

    rclcpp::Time monotonicToRosTime(const timespec& mono_ts);
    rclcpp::Time usToRosTime(uint64_t timestamp_us);
    rclcpp::Time getMonotonicRosTime();

    void calibrate();
    void debugTimeState();

private:
    double initial_offset_ = 0.0;
    double calibration_offset_ = 0.0;

};

class CamInfo
{
public:
    CamInfo() = default;

    void setFromParams(const AdjustStereoParams& param);

    const sensor_msgs::msg::CameraInfo& getIrInfo() const { return cam_info_ir_; }
    const sensor_msgs::msg::CameraInfo& getRgbInfo() const { return cam_info_rgb_; }

    void updateTimestamps(const rclcpp::Time& stamp_rgb, const rclcpp::Time& stamp_ir);

private:
    sensor_msgs::msg::CameraInfo cam_info_ir_;
    sensor_msgs::msg::CameraInfo cam_info_rgb_;
};

class RMSLNode : public rclcpp::Node {
public:
    //RMSLNode();
    explicit RMSLNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

private:
    void timerCallback();
    void loadParameters();

    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr rgb_pub_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr depth_pub_;
    rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr color_info_pub_;
    rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr depth_info_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pointcloud_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr aligned_pointcloud_pub_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr aligned_depth_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::TimerBase::SharedPtr calib_timer_;

    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_tf_broadcaster_;
    void publishStaticTransforms();

    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr raw_pub_;
    double publish_rate_ = 10.0;

    StereoParam param;
    StereoCalibrationData calib_data_;
    AdjustStereoParams adjust_params_;

    bool loadCalibrationFromUVC(const UVC_StereoCameraParams& uvc_params);
    bool AdjustCalibData(StereoCalibrationData& calib_data, AdjustStereoParams& params);

    std::unique_ptr<V4L2DualCapture> cap_;
    std::unique_ptr<V4L2TripleCapture> cap_debug_;
    std::unique_ptr<ImageProcessor> img_proc_;

    std::unique_ptr<TimeSyncManager> time_sync_manager_;
    std::unique_ptr<MonotonicToRosConverter> time_converter_;
    std::unique_ptr<CamInfo> cam_info_;

    int frame_width_ = 640;
    int frame_height_ = 480;
    int fps_ = 30;

    int rgb_width_ = 1280;
    int rgb_height_ = 1080;
    int ir_width_ = 640;
    int ir_height_ = 352;
    int rgb_crop_width_;
    float depth_multi_ratio_ = 1.0;
    int rotateAngle_ = 0;
    int extend_w_pixel_ = 40;
    int extend_h_pixel_ = 40;
    double baseline_, fx_origin_;

    bool enable_depth_ = true;
    bool enable_depth_aligned_ = true;
    bool enable_pointcloud_ = false;
    bool enable_pcl_aligned_ = false;
    bool enable_pointcloud_rgbd_ = false;
    bool enable_depth_pseudo_ = true;
    bool enable_debug_ir_ = false;
    cv::Rect hand_roi_;
};
