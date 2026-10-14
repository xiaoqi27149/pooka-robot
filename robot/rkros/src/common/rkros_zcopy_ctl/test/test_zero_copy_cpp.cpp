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

#include "rkros_zcopy_ctl/rkros_zcopy_ctl.hpp"
#include <iostream>

static void print_env(void) {
    const char *rmw = std::getenv("RMW_IMPLEMENTATION");
    const char *profile = std::getenv("FASTRTPS_DEFAULT_PROFILES_FILE");
    const char *flag = std::getenv("ROS_DISABLE_LOANED_MESSAGES");
    const char *xml = std::getenv("RMW_FASTRTPS_USE_QOS_FROM_XML");
    std::cout << "RMW_IMPLEMENTATION=" << (rmw ? rmw : "<unset>") << '\n'
              << "FASTRTPS_DEFAULT_PROFILES_FILE=" << (profile ? profile : "<unset>")
              << '\n'
              << "RMW_FASTRTPS_USE_QOS_FROM_XML=" << (xml ? xml : "<unset>") << '\n'
              << "ROS_DISABLE_LOANED_MESSAGES=" << (flag ? flag : "<unset>") << '\n'
              << std::endl;
}

int main() {
    rkros_zcopy_ctl::enable();
    print_env();
    return 0;
}