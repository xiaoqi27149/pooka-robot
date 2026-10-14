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

#ifndef RKROS_TRANSPORT_DEMO__PUBLISHER_COMPONENT_HPP_
#define RKROS_TRANSPORT_DEMO__PUBLISHER_COMPONENT_HPP_

#include "rclcpp/rclcpp.hpp"
#include "rkros_dmabuf/dmabuf_msg.hpp"
#include "rkros_zcopy_msgs/msg/img1080p.hpp"

namespace rkros_transport_demo {
class PublisherComponent : public rclcpp::Node {
  public:
    explicit PublisherComponent(
        const rclcpp::NodeOptions &options = rclcpp::NodeOptions{});

  private:
    const static int MAX_MSG_NUM{3};
    rclcpp::Publisher<rkros_zcopy_msgs::msg::Img1080p>::SharedPtr pub_img1080p_;
    rclcpp::Publisher<rkros_dmabuf::ImageMsg>::SharedPtr pub_dmabuf_;
    rclcpp::TimerBase::SharedPtr timer_;
    rkros_zcopy_msgs::msg::Img1080p msg_[MAX_MSG_NUM];
    rkros_dmabuf::ImageMsg msg_dmabuf_[MAX_MSG_NUM];
    enum class MsgType { Img1080p, DmaBuf } msg_type_;
    size_t index_;
    void publish_msg();
    void publish_intra_zcopy_msg();
    void publish_inter_zcopy_msg();
    bool dmabuf_to_img1080p(const rkros_dmabuf::ImageMsg &src,
                            rkros_zcopy_msgs::msg::Img1080p &dst);
};
} // namespace rkros_transport_demo

#endif // RKROS_TRANSPORT_DEMO__PUBLISHER_COMPONENT_HPP_