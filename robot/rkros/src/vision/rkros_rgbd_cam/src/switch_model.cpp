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

#include <rclcpp/rclcpp.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "rmsl32x_usb.h"

class RmslCameraNode : public rclcpp::Node
{
public:
    RmslCameraNode() : Node("rmsl_camera_node")
    {
        // 声明参数
        this->declare_parameter<int>("model_mode", -1); // 默认值设为-1表示未设置

        // 获取参数
        int model_mode = this->get_parameter("model_mode").as_int();

        if (model_mode == -1) {
            RCLCPP_ERROR(this->get_logger(), "param 'model_mode' is not be setted!");
            RCLCPP_INFO(this->get_logger(), "Using:  ros2 run rkros_rgbd_cam switch_model --ros-args -p model_mode:=<value>");
            RCLCPP_INFO(this->get_logger(), "Model_mode optional values:");
            RCLCPP_INFO(this->get_logger(), "  0 - Performance Mode");
            RCLCPP_INFO(this->get_logger(), "  1 - Quality mode");
            return;
        }

        const char* mode_name = (model_mode == 0) ? "Performance Mode" : "Quality mode";

        if (model_mode != 0 && model_mode != 1) {
            RCLCPP_ERROR(this->get_logger(), "Invalid model_mode: %d. Valid values are 0 (Performance Mode) or 1 (Quality mode).", model_mode);
            return;
        }

        // 设备发现和初始化逻辑
        if (!initializeCamera(model_mode, mode_name)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to initialize RMSL32x camera");
        }
    }

private:
    bool initializeCamera(int model_mode, const char* mode_name)
    {
        char dev_depth[32] = {0};
        char dev_rgb[32]   = {0};
        char dev_ir[32]    = {0};

        int debug_uvc = 1;
        int ret = rmsl_get_devices(dev_depth, dev_rgb, dev_ir, debug_uvc);
        if (ret) {
            RCLCPP_ERROR(this->get_logger(), "Failed to get RMSL32x devices");
            return false;
        }

        RCLCPP_INFO(this->get_logger(), "get 1. color:%s  2. depth:%s  3.ir:%s", dev_rgb, dev_depth, dev_ir);

        int rgb_fd = open(dev_rgb, O_RDWR | O_CLOEXEC, 0);
        if (rgb_fd < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open RGB device: %s", dev_rgb);
            return false;
        }

        char version[RMSL_DATA_SIZE_QUERY] = {0};
        ret = rmsl_get_version(rgb_fd, version, RMSL_DATA_SIZE_QUERY);
        if (ret) {
            RCLCPP_ERROR(this->get_logger(), "Failed to get RMSL32x version");
        } else {
            RCLCPP_INFO(this->get_logger(), "RMSL32x Camera version: %s", version);
        }

        ret = rmsl_set_model_mode(rgb_fd, model_mode);
        if (ret) {
            RCLCPP_ERROR(this->get_logger(), "Failed to set RMSL32x model mode to %s(%d)", mode_name, model_mode);
            close(rgb_fd);
            return false;
        } else {
            RCLCPP_INFO(this->get_logger(), "Successfully set RMSL32x model mode to %s(%d)", mode_name, model_mode);
        }

        ret = rmsl_module_reboot(rgb_fd);
        if (ret) {
            RCLCPP_ERROR(this->get_logger(), "Failed to reboot RMSL32x module");
        } else {
            RCLCPP_INFO(this->get_logger(), "Successfully reboot RMSL32x module");
        }

        close(rgb_fd);
        return true;
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RmslCameraNode>();

    rclcpp::spin_some(node);

    rclcpp::shutdown();
    return 0;
}
