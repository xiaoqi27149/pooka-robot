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

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rk_tts_rknn.h"
#include "rkros_audio_msgs/msg/rk_audio.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class TtsNode : public rclcpp::Node {
public:
    TtsNode() : Node("rktts_node") {
        // Create subscriber for text input
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "rktts_text", 10, std::bind(&TtsNode::text_callback, this, std::placeholders::_1));

        // Create publisher for audio output
        publisher_ = this->create_publisher<rkros_audio_msgs::msg::RkAudio>("rkaudio", 10);

        // Initialize TTS engine
        if (init_tts_engine() != 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to initialize TTS engine");
            return;
        }

        RCLCPP_INFO(this->get_logger(), "TTS node initialized successfully");
    }

private:
    void text_callback(const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "Received text for TTS: %s", msg->data.c_str());

        // Convert text to audio using TTS engine
        std::lock_guard<std::mutex> lock(tts_mutex_);
        int ret = tts_demo_->inference(msg->data.c_str(), alpha_, true);
        if (ret != 0) {
            RCLCPP_ERROR(this->get_logger(), "TTS inference failed with error code: %d", ret);
            return;
        }

        // Wait a bit for processing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    int init_tts_engine() {
        tts_demo_ = new rk_tts_interface;

        // Get the package share directory for this package
        std::string pkg_share_dir = ament_index_cpp::get_package_share_directory("rkros_tts");

        // Configuration parameters
        const char* key_lic_path = "{ \"trial\": 1, \"license_path\": \"/data/key_tts.lic\"}";
        std::string models_path = "/data/rkaudio/tts";
        std::string frontend_extra_path = pkg_share_dir + "/frontend_extras";
        // Speech rate setting (alpha): controls speed of speech
        alpha_ = 1.0;
        sample_rate_ = 24000;
        // Speaker ID: only supports 0, 1, 2
        speaker_id_ = 0;

        int ret = tts_demo_->init(key_lic_path, models_path.c_str(), frontend_extra_path.c_str(),
                                  speaker_id_, alpha_, sample_rate_,
                                  &TtsNode::audio_callback_static, static_cast<void*>(this));

        if (ret != 0) {
            RCLCPP_ERROR(this->get_logger(), "TTS initialization failed with error code: %d", ret);
            RCLCPP_ERROR(this->get_logger(), "Model path: %s", models_path.c_str());
            RCLCPP_ERROR(this->get_logger(), "Frontend extras path: %s",
                         frontend_extra_path.c_str());
            RCLCPP_ERROR(this->get_logger(), "Key lic path: %s", key_lic_path);
            delete tts_demo_;
            tts_demo_ = nullptr;
            return ret;
        }

        return 0;
    }

    // Static callback function to interface with C-style callback
    static void audio_callback_static(const int16_t* each_output, int wav_len, bool is_last_frame,
                                      void* userdata) {
        TtsNode* node = static_cast<TtsNode*>(userdata);
        node->audio_callback(each_output, wav_len, is_last_frame);
    }

    void audio_callback(const int16_t* each_output, int wav_len, bool is_last_frame) {
        // Append the results to our audio buffer
        std::lock_guard<std::mutex> lock(audio_buffer_mutex_);
        audio_buffer_.insert(audio_buffer_.end(), each_output, each_output + wav_len);

        // Publish audio data
        if (wav_len > 0) {
            publish_audio_data(each_output, wav_len);
        }

        // Use the parameter to avoid warning
        (void)is_last_frame;
    }

    void publish_audio_data(const int16_t* audio_data, int len) {
        // Create RkAudio message
        auto audio_msg = rkros_audio_msgs::msg::RkAudio();

        // Convert int16_t array to uint8_t vector (as expected by RkAudio message)
        size_t byte_size = len * sizeof(int16_t);
        audio_msg.audio_data.resize(byte_size);

        // Copy data
        std::memcpy(audio_msg.audio_data.data(), audio_data, byte_size);

        audio_msg.sample_rate = sample_rate_;
        audio_msg.channels = 1;
        audio_msg.bits_per_sample = 16;

        // Publish the message
        publisher_->publish(audio_msg);

        RCLCPP_DEBUG(this->get_logger(), "Published TTS audio data: %d samples", len);
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::Publisher<rkros_audio_msgs::msg::RkAudio>::SharedPtr publisher_;

    rk_tts_interface* tts_demo_;
    float alpha_;
    int sample_rate_;
    int speaker_id_;
    std::mutex tts_mutex_;

    // Audio buffer and related members
    std::vector<int16_t> audio_buffer_;
    std::mutex audio_buffer_mutex_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TtsNode>());
    rclcpp::shutdown();
    return 0;
}
