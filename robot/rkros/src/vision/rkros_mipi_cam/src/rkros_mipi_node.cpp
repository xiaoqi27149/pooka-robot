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

#include "rclcpp_components/register_node_macro.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "rockit_capture.h"
#include "rkros_zcopy_msgs/msg/img1080p.hpp"
#include "rkros_common/rk_color.h"


class RockitCameraNode : public rclcpp::Node {
public:
    explicit RockitCameraNode(const rclcpp::NodeOptions& options)
        : Node("mipi_camera_node", options), capture_() {

        this->declare_parameter<int>("dev_id", 0);
        this->declare_parameter<int>("pipe_id", 0);
        this->declare_parameter<int>("channel_id", 1);
        this->declare_parameter<int>("width", 1280);
        this->declare_parameter<int>("height", 960);
        this->declare_parameter<int>("loop_count", 0);
        this->declare_parameter<int>("pixel_format", RK_FMT_YUV420SP);
        this->declare_parameter<int>("buffer_count", 3);
        this->declare_parameter<int>("memory_type", VI_V4L2_MEMORY_TYPE_DMABUF);
        this->declare_parameter<int>("capture_type", VI_V4L2_CAPTURE_TYPE_VIDEO_CAPTURE);
        this->declare_parameter<bool>("enable_region", false);
        this->declare_parameter<int>("region_count", 1);
        this->declare_parameter<bool>("publish_rgb", true);
        this->declare_parameter<bool>("enable_rkros_zcopy", false);

        auto dev_id = this->get_parameter("dev_id").as_int();
        auto pipe_id = this->get_parameter("pipe_id").as_int();
        auto channel_id = this->get_parameter("channel_id").as_int();
        auto width = this->get_parameter("width").as_int();
        auto height = this->get_parameter("height").as_int();
        auto loop_count = this->get_parameter("loop_count").as_int();
        auto pixel_format = static_cast<rkPIXEL_FORMAT_E>(this->get_parameter("pixel_format").as_int());
        auto buffer_count = this->get_parameter("buffer_count").as_int();
        auto memory_type = static_cast<rkVI_V4L2_MEMORY_TYPE>(this->get_parameter("memory_type").as_int());
        auto capture_type = static_cast<rkVI_V4L2_CAPTURE_TYPE>(this->get_parameter("capture_type").as_int());
        auto enable_region = this->get_parameter("enable_region").as_bool();
        auto region_count = this->get_parameter("region_count").as_int();
        auto publish_rgb = this->get_parameter("publish_rgb").as_bool();
        auto enable_rkros_zcopy = this->get_parameter("enable_rkros_zcopy").as_bool();

        publish_rgb_ = publish_rgb;
        enable_rkros_zcopy_ = enable_rkros_zcopy;

        std::string topic = "/rkros/mipi_cam/image_raw";
        auto qos = rclcpp::QoS(10);
        if (enable_rkros_zcopy) {
            pub_zcopy_ = this->create_publisher<rkros_zcopy_msgs::msg::Img1080p>(topic, qos);
        } else {
            pub_senser_ = this->create_publisher<sensor_msgs::msg::Image>(topic, qos);
        }

        capture_.configure(
            dev_id,
            pipe_id,
            channel_id,
            width,
            height,
            loop_count,
            buffer_count,
            pixel_format,
            memory_type,
            capture_type,
            enable_region,
            region_count);

        capture_.init();
        capture_.start([this](const void* data, size_t len, int width, int height, int64_t pts) {
            this->publishFrame(data, len, width, height, pts);
        });
    }

    ~RockitCameraNode() {
        capture_.stop();
    }

private:
    void publishFrame(const void* data, size_t len, int width, int height, int64_t pts) {
        // 根据需要选择发布RGB或NV12格式
        if (publish_rgb_) {
            publishFrame_RGB(data, len, width, height, pts);
        }
        else {
            publishFrame_NV12(data, len, width, height, pts);
        }
    }

    void publishFrame_RGB(const void* data, size_t len, int width, int height, int64_t pts) {

        if (enable_rkros_zcopy_) {
            // 1. 借出消息（零拷贝优化）
            auto loaned_msg = pub_zcopy_->borrow_loaned_message();
            auto& msg = loaned_msg.get();

            msg.width = width;
            msg.height = height;
            msg.format = RKCLR_RGB_888;
            msg.stride = width * rkclr_get_pixel_bytes(msg.format);
            msg.data_size = height * msg.stride;

            cv::Mat nv12Mat(height * 3 / 2, width, CV_8UC1, const_cast<void*>(data));
            cv::Mat rgbMat(height, width, CV_8UC3, msg.data.data());  // 直接使用消息缓冲区
            cv::cvtColor(nv12Mat, rgbMat, cv::COLOR_YUV2RGB_NV12);
            pub_zcopy_->publish(std::move(loaned_msg));
        } else {
            // 2. 直接创建并发布普通消息
            auto msg = std::make_unique<sensor_msgs::msg::Image>();

            msg->header.stamp = rclcpp::Time(pts);
            msg->header.frame_id = "mipi_camera";
            msg->height = height;
            msg->width = width;
            msg->encoding = "rgb8";
            msg->step = width * 3;  // RGB8 每行字节数 = 宽 × 3通道
            // 确保缓冲区足够大
            const size_t required_size = height * msg->step;
            msg->data.resize(required_size);
            // 直接在借出的缓冲区进行颜色转换
            cv::Mat nv12Mat(height * 3 / 2, width, CV_8UC1, const_cast<void*>(data));
            cv::Mat rgbMat(height, width, CV_8UC3, msg->data.data());  // 直接使用消息缓冲区
            // 单步转换：NV12 → RGB (直接写入借出的消息缓冲区)
            cv::cvtColor(nv12Mat, rgbMat, cv::COLOR_YUV2RGB_NV12);
            pub_senser_->publish(std::move(msg));
        }

    }

    void publishFrame_NV12(const void* data, size_t len, int width, int height, int64_t pts) {

        if (enable_rkros_zcopy_) {
            auto loaned_msg = pub_zcopy_->borrow_loaned_message();
            auto& msg = loaned_msg.get();

            msg.width = width;
            msg.height = height;
            msg.format = RKCLR_YUV420SP;
            msg.stride = width * rkclr_get_pixel_bytes(msg.format);
            msg.data_size = height * msg.stride;

            memcpy(msg.data.data(), data, msg.data_size);
            pub_zcopy_->publish(std::move(loaned_msg));
        } else {
            auto msg = std::make_unique<sensor_msgs::msg::Image>();

            msg->header.stamp = rclcpp::Time(pts);
            msg->header.frame_id = "mipi_camera";
            msg->height = height;
            msg->width = width;
            msg->encoding = "NV12";
            msg->step = width;
            const size_t required_size = height * width * 3 / 2;  // YUV420格式
            msg->data.resize(required_size);

            memcpy(msg->data.data(), data, required_size);
            pub_senser_->publish(std::move(msg));
        }
    }

    bool publish_rgb_;
    bool enable_rkros_zcopy_;
    RockitCapture capture_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_senser_;
    rclcpp::Publisher<rkros_zcopy_msgs::msg::Img1080p>::SharedPtr pub_zcopy_;
};

// -----------components registration -----------
RCLCPP_COMPONENTS_REGISTER_NODE(RockitCameraNode)

// ------------------- main -------------------
#ifndef COMPOSITION_BUILD

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RockitCameraNode>(rclcpp::NodeOptions()));
  rclcpp::shutdown();
  return 0;
}

#endif
