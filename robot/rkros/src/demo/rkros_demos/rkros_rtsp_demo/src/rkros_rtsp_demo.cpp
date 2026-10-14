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

#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"

#include "rockit_routeline.hpp"

rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;

class RockitVencNode : public rclcpp::Node {
public:
    explicit RockitVencNode(const rclcpp::NodeOptions& options)
        : Node("rockit_venc_node", options) {

        // 声明参数
        this->declare_parameter<int>("width", 1920);
        this->declare_parameter<int>("height", 1080);
        this->declare_parameter<int>("disp_width", 1280);
        this->declare_parameter<int>("disp_height", 720);
        this->declare_parameter<int>("channel_id", 0);
        this->declare_parameter<std::string>("encode_type", "h264");
        this->declare_parameter<int>("frame_count", -1);
        this->declare_parameter<bool>("enable_publish_encoder", true);

        // 获取参数
        auto width = this->get_parameter("width").as_int();
        auto height = this->get_parameter("height").as_int();
        auto disp_width = this->get_parameter("disp_width").as_int();
        auto disp_height = this->get_parameter("disp_height").as_int();
        auto channel_id = this->get_parameter("channel_id").as_int();
        auto encode_type = this->get_parameter("encode_type").as_string();
        g_s32FrameCnt = this->get_parameter("frame_count").as_int();
        auto enable_publish_encoder = this->get_parameter("enable_publish_encoder").as_bool();

        RK_CODEC_ID_E enCodecType;
        // 设置编码类型
        if (encode_type == "h264") {
            enCodecType = RK_VIDEO_ID_AVC;
        } else if (encode_type == "h265") {
            enCodecType = RK_VIDEO_ID_HEVC;
        } else if (encode_type == "mjpeg") {
            enCodecType = RK_VIDEO_ID_MJPEG;
        } else {
            RK_LOGE("Invalid encode type: %s", encode_type.c_str());
            return;
        }

        // 创建发布者
        if (enable_publish_encoder) {
            publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/camera/encoded", 10);
        }

        // 启动视频处理线程
        video_thread_ = std::thread([this, width, height, disp_width, disp_height,
                                    channel_id, enCodecType, enable_publish_encoder]() {
            process_video(width, height, disp_width, disp_height, channel_id, enCodecType, this, enable_publish_encoder);
        });
    }

    ~RockitVencNode() {
        quit = true;
        if (video_thread_.joinable()) {
            video_thread_.join();
        }
    }

private:
    std::thread video_thread_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RockitVencNode>(rclcpp::NodeOptions()));
    rclcpp::shutdown();
    return 0;
}
