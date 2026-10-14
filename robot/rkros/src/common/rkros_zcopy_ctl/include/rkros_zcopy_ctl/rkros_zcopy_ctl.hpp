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

#ifndef RKROS_ZCOPY_CTL__RKROS_ZCOPY_CTL_HPP_
#define RKROS_ZCOPY_CTL__RKROS_ZCOPY_CTL_HPP_

#include "rclcpp/rclcpp.hpp"
#include <ament_index_cpp/get_package_prefix.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace rkros_zcopy_ctl {

// 开启进程间零拷贝
inline bool enable() {
    bool ret = false;
    const std::string prefix = ament_index_cpp::get_package_prefix("rkros_zcopy_ctl");
    const std::string xml_path =
        prefix + "/share/rkros_zcopy_ctl/config/fastdds_zcopy.xml";

    ret = ::setenv("RMW_IMPLEMENTATION", "rmw_fastrtps_cpp", 1) == 0 &&
          ::setenv("RMW_FASTRTPS_USE_QOS_FROM_XML", "1", 1) == 0 &&
          ::setenv("ROS_DISABLE_LOANED_MESSAGES", "0", 1) == 0 &&
          ::setenv("FASTRTPS_DEFAULT_PROFILES_FILE", xml_path.c_str(), 1) == 0;

    if (ret) {
        std::cout << ">>> Zero-copy ENABLED <<<" << std::endl;
    } else {
        std::cerr << "Failed to set environment variables" << std::endl;
    }
    return ret;
}

// 判断node是否启用进程内零拷贝
inline bool is_intra_zcopy(const rclcpp::Node *node) {
    return node->get_node_options().use_intra_process_comms();
}

// 判断是否启用了进程间零拷贝
inline bool is_inter_zcopy() {
    char *value = ::getenv("ROS_DISABLE_LOANED_MESSAGES");

    return value && (atoi(value) == 0);
}

} // namespace rkros_zcopy_ctl

#endif // RKROS_ZCOPY_CTL__RKROS_ZCOPY_CTL_HPP_