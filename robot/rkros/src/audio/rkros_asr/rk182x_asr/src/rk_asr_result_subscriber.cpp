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

#include <iostream>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "rkros_asr_msgs/msg/rk_asr_result.hpp"

class RkAsrResultSubscriber : public rclcpp::Node {
public:
    RkAsrResultSubscriber() : Node("rk_asr_result_subscriber") {
        subscription_ = this->create_subscription<rkros_asr_msgs::msg::RkAsrResult>(
            "rk_asr_result", 10,
            std::bind(&RkAsrResultSubscriber::asr_result_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "RK ASR Result Subscriber node initialized");
    }

private:
    void asr_result_callback(const rkros_asr_msgs::msg::RkAsrResult::SharedPtr msg) {
        RCLCPP_INFO(
            this->get_logger(),
            "Received ASR Result - State: %d, Output: '%s', New Output: '%s', Timestamp: %d.%09u",
            msg->state, msg->result.c_str(), msg->new_result.c_str(), msg->timestamp.sec,
            msg->timestamp.nanosec);
    }

    rclcpp::Subscription<rkros_asr_msgs::msg::RkAsrResult>::SharedPtr subscription_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RkAsrResultSubscriber>());
    rclcpp::shutdown();
    return 0;
}
