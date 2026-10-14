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

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
#include <chrono>
#include <random>

class DoaPublisher : public rclcpp::Node {
public:
    DoaPublisher() : Node("doa_publisher") {
        publisher_ = this->create_publisher<std_msgs::msg::Float32>("rk_doa", 10);

        // Create a timer to publish DOA values periodically
        timer_ = this->create_wall_timer(std::chrono::milliseconds(2000), // Publish every 2 seconds
                                         std::bind(&DoaPublisher::publish_doa, this));

        RCLCPP_INFO(this->get_logger(), "DOA Publisher node started, publishing to rk_doa topic");

        // Initialize random number generator
        random_engine_.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }

private:
    void publish_doa() {
        auto message = std_msgs::msg::Float32();

        // Generate a random DOA value between 0 and 360 degrees
        std::uniform_real_distribution<float> distribution(0.0, 360.0);
        message.data = distribution(random_engine_);

        RCLCPP_INFO(this->get_logger(), "Publishing DOA: %.2f", message.data);
        publisher_->publish(message);
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
    std::random_device random_device_;
    std::mt19937 random_engine_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DoaPublisher>());
    rclcpp::shutdown();
    return 0;
}