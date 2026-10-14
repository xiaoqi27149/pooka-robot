#include "rkros_dmabuf/dmabuf_msg.hpp"

const char *TAG = "rkros_dmabuf::ImageMsg";

void rclcpp::TypeAdapter<rkros_dmabuf::ImageMsg, rkros_zcopy_msgs::msg::Img1080p>::
    convert_to_ros_message(const custom_type &src, ros_message_type &dst) {
    auto time = rclcpp::Clock().now();

    dst.timestamp = src.timestamp;
    dst.frame_no = src.frame_no;
    dst.width = src.width;
    dst.height = src.height;
    dst.stride = src.stride;
    dst.format = src.format;
    dst.data_size = src.stride * src.height;
    if (dst.data_size > dst.MAX_SIZE) {
        RCLCPP_ERROR(rclcpp::get_logger(TAG), "out of memory");
        return;
    }

    if (!src.dmabuf->copyTo(dst.data.data(), dst.data_size)) {
        RCLCPP_ERROR(rclcpp::get_logger(TAG), "dmabuf copy to failed");
        return;
    }

    int64_t delay_us =
        (rclcpp::Clock().now().nanoseconds() - time.nanoseconds()) / 1000;
    RCLCPP_DEBUG(rclcpp::get_logger(TAG), "convert to ros message time: %ld us",
                 delay_us);
}

void rclcpp::TypeAdapter<rkros_dmabuf::ImageMsg, rkros_zcopy_msgs::msg::Img1080p>::
    convert_to_custom(const ros_message_type &src, custom_type &dst) {
    auto time = rclcpp::Clock().now();

    dst.timestamp = src.timestamp;
    dst.frame_no = src.frame_no;
    dst.width = src.width;
    dst.height = src.height;
    dst.stride = src.stride;
    dst.format = src.format;

    std::size_t size = dst.stride * dst.height;
    auto dmabuf = rkros_dmabuf::DmaBuf::allocate(size);
    if (dmabuf == nullptr) {
        RCLCPP_ERROR(rclcpp::get_logger(TAG), "allocate dmabuf failed, size=%zu", size);
        return;
    }

    if (!dmabuf->copyFrom(src.data.data(), src.data_size)) {
        RCLCPP_ERROR(rclcpp::get_logger(TAG), "dmabuf copy from failed");
        return;
    }
    dst.dmabuf = dmabuf;

    int64_t delay_us =
        (rclcpp::Clock().now().nanoseconds() - time.nanoseconds()) / 1000;
    RCLCPP_DEBUG(rclcpp::get_logger(TAG), "convert to custom message time: %ld us",
                 delay_us);
}
