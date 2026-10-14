#ifndef RKROS_DMABUF__DMABUF_IMAGE_MSG_HPP_
#define RKROS_DMABUF__DMABUF_IMAGE_MSG_HPP_

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/type_adapter.hpp"
#include "rkros_dmabuf/dmabuf.hpp"
#include "rkros_zcopy_msgs/msg/img1080p.hpp"

namespace rkros_dmabuf {
struct ImageMsg : Image {
    uint64_t frame_no{0};
    builtin_interfaces::msg::Time timestamp;
};
} // namespace rkros_dmabuf

template <>
struct rclcpp::TypeAdapter<rkros_dmabuf::ImageMsg, rkros_zcopy_msgs::msg::Img1080p> {
    using is_specialized = std::true_type;
    using custom_type = rkros_dmabuf::ImageMsg;
    using ros_message_type = rkros_zcopy_msgs::msg::Img1080p;

    static void convert_to_ros_message(const custom_type &source,
                                       ros_message_type &destination);
    static void convert_to_custom(const ros_message_type &source,
                                  custom_type &destination);
};

RCLCPP_USING_CUSTOM_TYPE_AS_ROS_MESSAGE_TYPE(rkros_dmabuf::ImageMsg,
                                             rkros_zcopy_msgs::msg::Img1080p);

#endif