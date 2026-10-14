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

#ifndef RKROS_TRANSPORT_DEMO__SUBSCRIBER_COMPONENT_HPP_
#define RKROS_TRANSPORT_DEMO__SUBSCRIBER_COMPONENT_HPP_

#include "rclcpp/rclcpp.hpp"
#include "rkros_dmabuf/dmabuf_msg.hpp"
#include "rkros_zcopy_msgs/msg/img1080p.hpp"

namespace rkros_transport_demo {
class SubscriberComponent : public rclcpp::Node {
  public:
    explicit SubscriberComponent(
        const rclcpp::NodeOptions &options = rclcpp::NodeOptions{});

  private:
    const static int MAX_MSG_NUM{3};
    rclcpp::Subscription<rkros_zcopy_msgs::msg::Img1080p>::SharedPtr sub_img1080p_;
    rclcpp::Subscription<rkros_dmabuf::ImageMsg>::SharedPtr sub_dmabuf_;
    void img1080p_callback(const rkros_zcopy_msgs::msg::Img1080p::SharedPtr msg) const;
    void dmabuf_callback(std::shared_ptr<rkros_dmabuf::ImageMsg> msg) const;
};
} // namespace rkros_transport_demo

#endif // RKROS_TRANSPORT_DEMO__SUBSCRIBER_COMPONENT_HPP_