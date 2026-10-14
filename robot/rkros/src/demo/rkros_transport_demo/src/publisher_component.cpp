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

#include "rkros_transport_demo/publisher_component.hpp"
#include "rkros_common/rk_color.h"
#include "rkros_zcopy_ctl/rkros_zcopy_ctl.hpp"
#include <chrono>

namespace rkros_transport_demo {
PublisherComponent::PublisherComponent(const rclcpp::NodeOptions &options)
    : Node("rkros_transport_pub", options) {
    std::string msg_type_str =
        this->declare_parameter<std::string>("msg_type", "dmabuf");
    RCLCPP_INFO(this->get_logger(), "Publish msg_type: %s", msg_type_str.c_str());
    uint32_t format = RKCLR_RGB_888;
    if (msg_type_str == "dmabuf") {
        msg_type_ = MsgType::DmaBuf;

        // 准备测试用的消息数据
        for (int i = 0; i < MAX_MSG_NUM; i++) {
            auto msg = &msg_dmabuf_[i];
            msg->frame_no = 0;
            msg->width = 1920;
            msg->height = 1080;
            msg->format = format;
            msg->stride = 1920 * rkclr_get_pixel_bytes(msg->format);

            std::size_t size = msg->height * msg->stride;
            auto buf = rkros_dmabuf::DmaBuf::allocate(size);
            if (buf == nullptr) {
                break;
            }
            memset(buf->map(), i, size);
            msg->dmabuf = buf;
        }

        pub_dmabuf_ = create_publisher<rkros_dmabuf::ImageMsg>("rkros_transport_dmabuf",
                                                               rclcpp::SensorDataQoS());
    } else if (msg_type_str == "img1080p") {
        msg_type_ = MsgType::Img1080p;

        // 准备测试用的消息数据
        for (int i = 0; i < MAX_MSG_NUM; i++) {
            auto msg = &msg_[i];
            msg->frame_no = 0;
            msg->width = 1920;
            msg->height = 1080;
            msg->format = format;
            msg->stride = 1920 * rkclr_get_pixel_bytes(msg->format);
            msg->data_size = msg->stride * msg->height;
            memset(msg->data.data(), i, msg->data_size);
        }

        pub_img1080p_ = create_publisher<rkros_zcopy_msgs::msg::Img1080p>(
            "rkros_transport", rclcpp::SensorDataQoS());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Unknown msg_type: %s", msg_type_str.c_str());
    }

    // 使用定时器 定时发布消息
    timer_ = create_wall_timer(
        std::chrono::milliseconds(100),       // 100 ms => 10 Hz
        rkros_zcopy_ctl::is_intra_zcopy(this) // 判断是否开启了进程内零拷贝
            ? std::bind(&PublisherComponent::publish_intra_zcopy_msg, this)
            : (rkros_zcopy_ctl::is_inter_zcopy() // 判断是否开启了进程间零拷贝
                   ? std::bind(&PublisherComponent::publish_inter_zcopy_msg, this)
                   : std::bind(&PublisherComponent::publish_msg, this)));
}

// 未开启零拷贝，发布的消息会有多次拷贝
void PublisherComponent::publish_msg() {
    if (msg_type_ == MsgType::Img1080p) {
        auto time = now();

        auto msg = &msg_[index_ % MAX_MSG_NUM];
        msg->frame_no = index_;

        int64_t delay_us = (now().nanoseconds() - time.nanoseconds()) / 1000;
        RCLCPP_INFO(this->get_logger(), "Pub Img1080p: %ld, Prepare %ld us", index_,
                    delay_us);

        msg->timestamp = now();
        pub_img1080p_->publish(*msg);
    } else if (msg_type_ == MsgType::DmaBuf) {
        auto time = now();

        auto msg = &msg_dmabuf_[index_ % MAX_MSG_NUM];
        msg->frame_no = index_;

        int64_t delay_us = (now().nanoseconds() - time.nanoseconds()) / 1000;
        RCLCPP_INFO(this->get_logger(), "Pub Dmabuf: %ld, Prepare %ld us", index_,
                    delay_us);

        msg->timestamp = now();
        pub_dmabuf_->publish(*msg);
    }

    index_++;
}

// 开启了进程内零拷贝
void PublisherComponent::publish_intra_zcopy_msg() {
    if (msg_type_ == MsgType::Img1080p) {
        auto time = now();

        // 对消息做一次拷贝，消息含有data数据，这是一次深拷贝
        auto msg = std::make_unique<rkros_zcopy_msgs::msg::Img1080p>(
            msg_[index_ % MAX_MSG_NUM]);
        msg->frame_no = index_;

        int64_t delay_us = (now().nanoseconds() - time.nanoseconds()) / 1000;
        RCLCPP_INFO(this->get_logger(), "Pub-Intra Img1080p: %ld, Prepare %ld us",
                    index_, delay_us);

        // 发布消息时候不会对消息进行拷贝
        msg->timestamp = now();
        pub_img1080p_->publish(std::move(msg));
    } else if (msg_type_ == MsgType::DmaBuf) {
        auto time = now();

        // 对消息做一次拷贝，消息不含data数据，这是一次浅拷贝
        auto msg =
            std::make_unique<rkros_dmabuf::ImageMsg>(msg_dmabuf_[index_ % MAX_MSG_NUM]);
        msg->frame_no = index_;

        int64_t delay_us = (now().nanoseconds() - time.nanoseconds()) / 1000;
        RCLCPP_INFO(this->get_logger(), "Pub-Intra Dmabuf: %ld, Prepare %ld us", index_,
                    delay_us);

        // 发布消息时候不会对消息进行拷贝
        msg->timestamp = now();
        pub_dmabuf_->publish(std::move(msg));
    }

    index_++;
}

bool PublisherComponent::dmabuf_to_img1080p(const rkros_dmabuf::ImageMsg &src,
                                            rkros_zcopy_msgs::msg::Img1080p &dst) {
    dst.timestamp = src.timestamp;
    dst.frame_no = src.frame_no;
    dst.width = src.width;
    dst.height = src.height;
    dst.stride = src.stride;
    dst.format = src.format;
    dst.data_size = src.stride * src.height;
    if (!src.dmabuf->copyTo(dst.data.data(), dst.data_size)) {
        RCLCPP_ERROR(this->get_logger(), "dmabuf copy to failed");
        return false;
    }

    return true;
}

// 开启了进程间零拷贝
void PublisherComponent::publish_inter_zcopy_msg() {
    if (msg_type_ == MsgType::Img1080p) {
        auto loanedMsg = pub_img1080p_->borrow_loaned_message();
        if (loanedMsg.is_valid()) {
            auto time = now();

            auto &msg = loanedMsg.get();
            // 填充ros消息的数据，这是一次深拷贝
            memcpy(&msg, &msg_[index_ % MAX_MSG_NUM],
                   sizeof(rkros_zcopy_msgs::msg::Img1080p));
            msg.frame_no = index_;

            int64_t delay_us = (now().nanoseconds() - time.nanoseconds()) / 1000;
            RCLCPP_INFO(this->get_logger(), "Pub-Inter Img1080p: %ld, Prepare %ld us",
                        index_, delay_us);

            msg.timestamp = now();
            pub_img1080p_->publish(std::move(loanedMsg));
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to get Loaned Message");
        }
    } else if (msg_type_ == MsgType::DmaBuf) {
        /**
         * rkros_dmabuf::Image是自适应消息类型，其对应的ros消息类型是
         * rkros_zcopy_msgs::msg::Img1080p，当使用借用消息时候，取得的消息类型是
         * rkros_zcopy_msgs::msg::Img1080p，在送到subscriber端时，会自动转换为
         * rkros_dmabuf::ImageMsg
         */
        auto loanedMsg = pub_dmabuf_->borrow_loaned_message();
        if (loanedMsg.is_valid()) {
            auto time = now();

            auto &msg = loanedMsg.get();
            // 填充ros消息，这是一次深拷贝
            dmabuf_to_img1080p(msg_dmabuf_[index_ % MAX_MSG_NUM], msg);
            msg.frame_no = index_;

            int64_t delay_us = (now().nanoseconds() - time.nanoseconds()) / 1000;
            RCLCPP_INFO(this->get_logger(), "Pub-Inter Dmabuf: %ld, Prepare %ld us",
                        index_, delay_us);

            msg.timestamp = now();
            pub_dmabuf_->publish(std::move(loanedMsg));
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to get Loaned Message");
        }
    }
    index_++;
}
} // namespace rkros_transport_demo

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(rkros_transport_demo::PublisherComponent)