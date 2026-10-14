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
#include "rkros_transport_demo/publisher_component.hpp"
#include "rkros_transport_demo/subscriber_component.hpp"
#include <memory>

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    // 线程池大小，≥2 即可
    rclcpp::executors::MultiThreadedExecutor exec(rclcpp::ExecutorOptions(), 2);
    rclcpp::NodeOptions options;
    if (argc > 1 && strcmp(argv[1], "no_intra") == 0)
        options.use_intra_process_comms(false);
    else
        options.use_intra_process_comms(true);

    auto pub = std::make_shared<rkros_transport_demo::PublisherComponent>(options);
    auto sub = std::make_shared<rkros_transport_demo::SubscriberComponent>(options);

    exec.add_node(pub);
    exec.add_node(sub);
    exec.spin();

    rclcpp::shutdown();
    return 0;
}