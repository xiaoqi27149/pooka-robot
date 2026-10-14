#include "rkros_vcam/vcam.hpp"
#include <rclcpp/rclcpp.hpp>

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::NodeOptions options;
    auto node = std::make_shared<rkros_vcam::VCamNode>(options);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
