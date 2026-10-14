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

#include "rkros_transport_demo/subscriber_component.hpp"
#include <chrono>

namespace rkros_transport_demo {
SubscriberComponent::SubscriberComponent(const rclcpp::NodeOptions &options)
    : Node("rkros_transport_sub", options) {
    std::string msg_type = this->declare_parameter<std::string>("msg_type", "dmabuf");
    RCLCPP_INFO(this->get_logger(), "Subscribe msg_type: %s", msg_type.c_str());

    if (msg_type == "dmabuf") {
        sub_dmabuf_ = create_subscription<rkros_dmabuf::ImageMsg>(
            "rkros_transport_dmabuf", rclcpp::SensorDataQoS(),
            std::bind(&SubscriberComponent::dmabuf_callback, this,
                      std::placeholders::_1));
    } else if (msg_type == "img1080p") {
        sub_img1080p_ = create_subscription<rkros_zcopy_msgs::msg::Img1080p>(
            "rkros_transport", rclcpp::SensorDataQoS(),
            std::bind(&SubscriberComponent::img1080p_callback, this,
                      std::placeholders::_1));
    } else {
        RCLCPP_ERROR(this->get_logger(), "Unknown msg_type: %s", msg_type.c_str());
    }
}

void SubscriberComponent::img1080p_callback(
    const rkros_zcopy_msgs::msg::Img1080p::SharedPtr msg) const {
    int64_t t_now_ns = now().nanoseconds();
    int64_t delay_us = (t_now_ns - rclcpp::Time(msg->timestamp).nanoseconds()) / 1000;
    RCLCPP_INFO(this->get_logger(), "Sub Img1080p: %ld, Trans time: %ld us",
                msg->frame_no, delay_us);

    // verify the first 100 bytes
    for (int i = 0; i < 100; i++) {
        if (msg->data[i] != (msg->frame_no % MAX_MSG_NUM)) {
            RCLCPP_WARN(this->get_logger(), "mis-match in %d, data: %d", i,
                        msg->data[i]);
            break;
        }
    }
}

void SubscriberComponent::dmabuf_callback(
    const std::shared_ptr<rkros_dmabuf::ImageMsg> msg) const {
    int64_t t_now_ns = now().nanoseconds();
    int64_t delay_us = (t_now_ns - rclcpp::Time(msg->timestamp).nanoseconds()) / 1000;
    RCLCPP_INFO(this->get_logger(), "Sub Dmabuf: %ld, Trans time: %ld us",
                msg->frame_no, delay_us);

    uint8_t *data = (uint8_t *)msg->dmabuf->map();
    if (data == nullptr) {
        RCLCPP_ERROR(this->get_logger(), "dmabuf map failed");
        return;
    }

    // verify the first 100 bytes
    for (int i = 0; i < 100; i++) {
        if (data[i] != (msg->frame_no % MAX_MSG_NUM)) {
            RCLCPP_WARN(this->get_logger(), "mis-match in %d, data: %d", i, data[i]);
            break;
        }
    }
}
} // namespace rkros_transport_demo

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(rkros_transport_demo::SubscriberComponent)