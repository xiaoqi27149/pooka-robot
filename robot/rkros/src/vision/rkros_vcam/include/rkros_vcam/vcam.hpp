// vcam.hpp - virtual camera node header
#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "rkros_dmabuf/dmabuf_msg.hpp"
#include "rkros_vcam/image_generator.hpp"
#include "sensor_msgs/msg/image.hpp"

namespace rkros_vcam {

struct TopicConfig {
    std::string topic_name;
    std::string format;   // "rgb" "yuv" "jpeg"
    std::string msg_type; // "sensor" or "dmabuf"
};

class VCamNode : public rclcpp::Node {
  public:
    explicit VCamNode(const rclcpp::NodeOptions &options = rclcpp::NodeOptions());
    ~VCamNode();

  private:
    void publishFrame();

    // config
    double publish_fps_;
    double gen_fps_;
    int width_;
    int height_;

    std::unordered_map<std::string, std::shared_ptr<ImageGenerator>> generators_;

    // publishers
    struct PubHolder {
        rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr sensor_pub;
        rclcpp::Publisher<rkros_dmabuf::ImageMsg>::SharedPtr dmabuf_pub;
        TopicConfig cfg;
    };
    std::vector<PubHolder> pubs_;

    // publish thread
    std::thread publish_thread_;
    std::atomic<bool> running_{false};
};

} // namespace rkros_vcam
