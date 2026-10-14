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
#include <cmath>
#include <fcntl.h>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_GROUP_NUM 8 // Default to 8 groups for 8mic board
#define MAX_GROUP_NUM 12    // Maximum groups for 6mic board
#define LED_PER_GROUP 3

// Global variables for LED control
uint32_t max_led_num = MAX_GROUP_NUM * LED_PER_GROUP; // Max possible LEDs (for 12 groups)
int* led_fd;
uint8_t* color_cache;
int current_group_num = DEFAULT_GROUP_NUM; // Current group number, default to 8

// RGB color structure
struct light_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Close all opened LED file descriptors
int led_deinit(void) {
    if (led_fd == nullptr || color_cache == nullptr) {
        return 0; // Nothing to deinitialize
    }

    uint32_t i;
    uint32_t current_led_num = current_group_num * LED_PER_GROUP;

    for (i = 0; i < current_led_num; i++) {
        if (led_fd[i] != 0) {
            close(led_fd[i]);
            led_fd[i] = 0;
        }
    }

    // Free memory
    delete[] led_fd;
    delete[] color_cache;
    led_fd = nullptr;
    color_cache = nullptr;

    return 0;
}

// Initialize LED hardware
int led_init(void) {
    uint32_t i;
    char str[64];

    // Allocate memory for LED file descriptors and color cache
    led_fd = new int[max_led_num];
    color_cache = new uint8_t[max_led_num];

    // Initialize arrays
    for (i = 0; i < max_led_num; i++) {
        led_fd[i] = 0;
        color_cache[i] = 0;
    }

    // Open LED device files based on current group number
    uint32_t current_led_num = current_group_num * LED_PER_GROUP;
    for (i = 0; i < current_led_num; i++) {
        memset(str, 0x00, sizeof(str));

        // Use the same format as the original code
        snprintf(str, sizeof(str), "/sys/class/leds/led%d/brightness", i + 1);

        led_fd[i] = open(str, O_WRONLY | O_CREAT, 0644);

        if (led_fd[i] < 0) {
            RCLCPP_ERROR(rclcpp::get_logger("led_controller"), "Error opening file %s", str);
            led_deinit();
            return -1;
        }
    }

    return 0;
}

// Write LED brightness data
int led_write(uint8_t* brightness, uint32_t len) {
    uint32_t i;
    char str[16];
    uint32_t write_num;
    uint32_t update = 0;

    uint32_t current_led_num = current_group_num * LED_PER_GROUP;
    write_num = current_led_num > len ? len : current_led_num;

    // Actual write mode
    for (i = 0; i < write_num; i++) {
        memset(str, 0x00, sizeof(str));
        update = 0;

        snprintf(str, sizeof(str), "%d", brightness[i]);
        if (color_cache[i] != brightness[i]) {
            update = 1;
        }
        color_cache[i] = brightness[i];

        if (update == 1 || color_cache[i] == 0) {
            ssize_t written = write(led_fd[i], str, strlen(str));
            if (written < 0 || (size_t)written != strlen(str)) {
                RCLCPP_ERROR(rclcpp::get_logger("led_controller"), "LED write error for LED %d: %s",
                             i, str);
            } else {
                // Only print for non-zero values to reduce console output
                if (brightness[i] > 0) {
                    RCLCPP_INFO(rclcpp::get_logger("led_controller"),
                                "Successfully wrote %s to LED %d", str, i);
                }
            }
        }
    }

    return 0;
}

// Set colors for multiple groups at once
void set_multiple_groups(const int* group_indices, const struct light_t* group_colors, int count) {
    RCLCPP_INFO(rclcpp::get_logger("led_controller"), "Setting multiple groups, count: %d", count);

    uint32_t current_led_num = current_group_num * LED_PER_GROUP;
    uint8_t* brightness = new uint8_t[current_led_num];

    // Initialize all LEDs to 0 brightness
    for (uint32_t i = 0; i < current_led_num; i++) {
        brightness[i] = 0;
    }

    // Set the specified groups' LED colors
    for (int j = 0; j < count && j < current_group_num; j++) {
        int group_index = group_indices[j];
        if (group_index >= 0 && group_index < current_group_num) {
            // Actual order is GRB (Green, Red, Blue)
            brightness[group_index * 3] = group_colors[j].g;     // First LED - Green component
            brightness[group_index * 3 + 1] = group_colors[j].r; // Second LED - Red component
            brightness[group_index * 3 + 2] = group_colors[j].b; // Third LED - Blue component
            RCLCPP_DEBUG(rclcpp::get_logger("led_controller"),
                         "Setting group %d to RGB(%d, %d, %d)", group_index, group_colors[j].r,
                         group_colors[j].g, group_colors[j].b);
        }
    }

    led_write(brightness, current_led_num);
    delete[] brightness;
}

// Calculate physical group index for 12-group setup (6mic board)
int get_12group_physical_index(int logical_group_index) {
    // Hardware order: 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 2
    // Logical index: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
    // Physical index: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1
    return (logical_group_index + 2) % 12;
}

// Calculate physical group index for 8-group setup (8mic board)
int get_8group_physical_index(int logical_group_index) {
    // For 8mic board, physical order is the same as logical order: 1, 2, 3, 4, 5, 6, 7, 8
    // So physical index is the same as logical index
    return logical_group_index;
}

// Turn on LEDs in a specific sector (for DOA visualization)
void set_doa_leds(float doa_angle) {
    int group_num = current_group_num;
    float angle_step = 360.0 / group_num;     // 45 degrees for 8 groups, 30 degrees for 12 groups
    float half_angle_step = angle_step / 2.0; // 22.5 for 8 groups, 15 for 12 groups

    // Determine which sector contains this angle
    int logical_group_index = (int)round(doa_angle / angle_step) % group_num;

    // Calculate the center angle for this group
    float group_center = logical_group_index * angle_step;

    // Compute the angular difference, taking into account the circular nature
    float angle_diff = doa_angle - group_center;

    // Handle wrap-around for the circular case
    // If the difference is more than 180 degrees, adjust it
    if (angle_diff > 180) {
        angle_diff -= 360;
    } else if (angle_diff < -180) {
        angle_diff += 360;
    }

    // If the angle is outside the ±half_angle_step degree range of the current group,
    // determine which adjacent group it should belong to
    if (fabs(angle_diff) > half_angle_step) {
        // The angle is outside the current group's range, find the correct group
        if (angle_diff > 0) {
            // The angle is beyond the positive boundary, assign to next group
            logical_group_index = (logical_group_index + 1) % group_num;
        } else {
            // The angle is beyond the negative boundary, assign to previous group
            logical_group_index = (logical_group_index - 1 + group_num) % group_num;
        }

        // Recalculate the angle difference for the new group
        group_center = logical_group_index * angle_step;
        angle_diff = doa_angle - group_center;
        if (angle_diff > 180) {
            angle_diff -= 360;
        } else if (angle_diff < -180) {
            angle_diff += 360;
        }
    }

    // Map logical group index to physical group index based on current configuration
    int physical_group_index;
    if (group_num == 8) {
        physical_group_index = get_8group_physical_index(logical_group_index);
    } else {
        physical_group_index = get_12group_physical_index(logical_group_index);
    }

    // For enhanced visualization, determine which groups to light up
    int group_indices[2];
    struct light_t group_colors[2];
    int count = 0;

    // Always light up the main group with full intensity red
    group_indices[count] = physical_group_index;
    group_colors[count].r = 255;
    group_colors[count].g = 0;
    group_colors[count].b = 0;
    count++;

    // For enhanced visualization, also light up adjacent group if DOA is close to border
#ifdef ENABLE_DOA_BORDER_EFFECT
    float border_threshold = half_angle_step * 0.66; // Adjust threshold based on group size
    if (fabs(angle_diff) > border_threshold) {       // Close to boundary
        int logical_adjacent_group;
        if (angle_diff > 0) {
            // Closer to positive boundary, light up next group
            logical_adjacent_group = (logical_group_index + 1) % group_num;
        } else {
            // Closer to negative boundary, light up previous group
            logical_adjacent_group = (logical_group_index - 1 + group_num) % group_num;
        }

        // Map logical adjacent group to physical group based on current configuration
        int physical_adjacent_group;
        if (group_num == 8) {
            physical_adjacent_group = get_8group_physical_index(logical_adjacent_group);
        } else {
            physical_adjacent_group = get_12group_physical_index(logical_adjacent_group);
        }

        // Add adjacent group to the list with lower intensity red
        group_indices[count] = physical_adjacent_group;
        group_colors[count].r = 100; // Lower intensity
        group_colors[count].g = 0;
        group_colors[count].b = 0;
        count++;
    }
#endif

    // Set multiple groups at once
    set_multiple_groups(group_indices, group_colors, count);
}

class LedController : public rclcpp::Node {
public:
    LedController() : Node("led_controller") {
        // Declare and get the parameter for LED group number
        this->declare_parameter("led_group_num", DEFAULT_GROUP_NUM);
        current_group_num = this->get_parameter("led_group_num").as_int();

        // Validate the parameter
        if (current_group_num != 8 && current_group_num != 12) {
            RCLCPP_WARN(this->get_logger(), "Invalid led_group_num: %d, using default %d",
                        current_group_num, DEFAULT_GROUP_NUM);
            current_group_num = DEFAULT_GROUP_NUM;
        }

        RCLCPP_INFO(this->get_logger(), "LED Controller initialized with %d groups",
                    current_group_num);

        // Initialize LED hardware
        if (led_init() < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to initialize LED hardware");
            return;
        }

        // Create subscription to rk_doa topic
        subscription_ = this->create_subscription<std_msgs::msg::Float32>(
            "rk_doa", 10, std::bind(&LedController::doa_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "LED Controller node started, subscribing to rk_doa topic");
    }

    ~LedController() {
        // Turn off all LEDs on exit
        uint32_t current_led_num = current_group_num * LED_PER_GROUP;
        uint8_t* brightness = new uint8_t[current_led_num];
        memset(brightness, 0, current_led_num * sizeof(uint8_t));
        led_write(brightness, current_led_num);
        delete[] brightness;

        // Deinitialize LED hardware - this also handles memory deallocation
        led_deinit();
    }

private:
    void doa_callback(const std_msgs::msg::Float32::SharedPtr msg) {
        float doa = msg->data;
        RCLCPP_INFO(this->get_logger(), "Received DOA: %.2f", doa);

        // Validate DOA value (should be 0-360)
        if (doa < 0 || doa > 360) {
            RCLCPP_WARN(this->get_logger(), "Invalid DOA value: %.2f, should be between 0-360",
                        doa);
            return;
        }

        // Control the LEDs based on DOA
        set_doa_leds(doa);
    }

    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr subscription_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LedController>());
    rclcpp::shutdown();
    return 0;
}
