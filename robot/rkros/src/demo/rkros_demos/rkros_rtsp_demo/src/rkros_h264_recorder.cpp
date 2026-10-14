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
#include <fstream>
#include <ctime>
#include <sstream>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

class H264RecorderNode : public rclcpp::Node {
public:
    explicit H264RecorderNode(const rclcpp::NodeOptions& options)
        : Node("h264_recorder_node", options) {

        // 声明参数
        this->declare_parameter<std::string>("output_directory", "/tmp");
        this->declare_parameter<std::string>("filename_prefix", "record");
        this->declare_parameter<std::string>("subscription_topic", "/camera/encoded");

        // 获取参数
        output_directory_ = this->get_parameter("output_directory").as_string();
        filename_prefix_ = this->get_parameter("filename_prefix").as_string();
        subscription_topic_ = this->get_parameter("subscription_topic").as_string();

        // 确保目录末尾有斜杠
        if (!output_directory_.empty() && output_directory_.back() != '/') {
            output_directory_ += "/";
        }

        // 创建订阅者
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            subscription_topic_, 10,
            [this](const sensor_msgs::msg::Image::SharedPtr msg) {
                this->h264_callback(msg);
            });

        RCLCPP_INFO(this->get_logger(), "H264 Recorder Node initialized.");
        RCLCPP_INFO(this->get_logger(), "Subscribing to topic: %s", subscription_topic_.c_str());
        RCLCPP_INFO(this->get_logger(), "Saving files to directory: %s", output_directory_.c_str());
    }

private:
    void h264_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        // 如果是第一条消息，则创建新文件
        if (!outfile_.is_open()) {
            create_new_file();
        }

        // 写入数据
        if (outfile_.is_open()) {
            outfile_.write(reinterpret_cast<const char*>(msg->data.data()), msg->data.size());
            outfile_.flush();

            RCLCPP_DEBUG(this->get_logger(), "Written %zu bytes to file", msg->data.size());
        }
    }

    void create_new_file() {
        std::time_t now = std::time(nullptr);
        std::stringstream ss;
        ss << output_directory_ << filename_prefix_ << "_" << now << ".264";
        std::string filename = ss.str();

        outfile_.open(filename, std::ios::binary);
        if (outfile_.is_open()) {
            RCLCPP_INFO(this->get_logger(), "Created new recording file: %s", filename.c_str());
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to create file: %s", filename.c_str());
        }
    }

    std::string output_directory_;
    std::string filename_prefix_;
    std::string subscription_topic_;
    std::ofstream outfile_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<H264RecorderNode>(rclcpp::NodeOptions()));
    rclcpp::shutdown();
    return 0;
}
