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

#include <algorithm>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <cctype>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rkros_asr_msgs/msg/rk_asr_result.hpp"
#include "rkros_audio_msgs/msg/rk_audio.hpp"
#include "rockasr.h"
#include "rockx.h"
#include "rockx_builder.h"
#include "rockx_module.h"

// Declare ASR module getter function
extern "C" {
struct RockXModule* GetRockXModuleASR();
}

class RkAsrNode : public rclcpp::Node {
public:
    RkAsrNode() : Node("rkros_asr") {
        // Get package share directory
        std::string package_share_dir;
        try {
            package_share_dir = ament_index_cpp::get_package_share_directory("rkros_asr");
        } catch (const std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "Cannot find package share directory: %s", e.what());
            RCLCPP_ERROR(
                this->get_logger(),
                "Make sure the ROS2 environment is sourced before running this executable.");
            // Set default path as fallback
            package_share_dir = "/tmp"; // Temporary path, will be replaced in actual deployment
        }

        // Declare parameters
        this->declare_parameter<int>("language", 1); // Default Chinese (0=ENGLISH, 1=CHINESE)
        // Absolute data path
        this->declare_parameter<std::string>("data_path", "/data/rkaudio/asr");
        // Relative authentication config path
        this->declare_parameter<std::string>("auth_config",
                                             package_share_dir + "/config/rockx_auth_config.json");
        // Model name
        this->declare_parameter<std::string>("model_name", "ASR_SS_ZH_EN");

        // Get parameters
        this->get_parameter("language", language_);
        this->get_parameter("data_path", data_path_);
        this->get_parameter("auth_config", auth_config_);
        this->get_parameter("model_name", model_name_);

        // Create subscriber to subscribe to rkros_audio_msgs topic
        subscription_ = this->create_subscription<rkros_audio_msgs::msg::RkAudio>(
            "rkaudio_asr", 10, std::bind(&RkAsrNode::audio_callback, this, std::placeholders::_1));

        // Create publisher to publish ASR results
        publisher_ = this->create_publisher<rkros_asr_msgs::msg::RkAsrResult>("rk_asr_result", 10);

        // Create Twist message publisher
        twist_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

        // Initialize ASR
        if (init_asr()) {
            RCLCPP_INFO(this->get_logger(), "RK ASR initialized successfully");
        } else {
            RCLCPP_WARN(
                this->get_logger(),
                "RK ASR initialization failed, but node will continue to run for testing purposes");
        }

        RCLCPP_INFO(this->get_logger(), "RK ASR node initialized");
    }

private:
    bool init_asr() {
        // In some RockX environments, ASR module needs to be registered first
        RockXModuleGlobalRegister(GetRockXModuleASR());

        init_param_ = (RockXAsrInitParam*)malloc(sizeof(RockXAsrInitParam));
        if (init_param_ == NULL) {
            RCLCPP_ERROR(this->get_logger(), "Failed to allocate init_param");
            return false;
        }
        memset(init_param_, 0, sizeof(RockXAsrInitParam));
        init_param_->workMode = ROCKASR_MODE_STREAM;
        init_param_->sampleRate = 16000; // Default sample rate
        init_param_->modelName = strdup(model_name_.c_str());

        if (language_ == 1) {
            init_param_->sourceLanguage = ROCKASR_LANG_CHINESE;
        } else {
            init_param_->sourceLanguage = ROCKASR_LANG_ENGLISH;
        }
        init_param_->resultCallback = asr_result_callback;
        init_param_->userdata = this;

        handle_ = RockXCreate(1, NULL);
        if (!handle_) {
            RCLCPP_ERROR(this->get_logger(), "Failed to create RockX handle");
            free(init_param_);
            init_param_ = NULL;
            return false;
        }

        RockXSetParamString(handle_, NULL, ROCKX_KEY_MODULE_NAME, "ASR", 0);
        RockXSetParamString(handle_, NULL, ROCKX_KEY_DATA_PATH, data_path_.c_str(), 0);
        RockXSetParamString(handle_, NULL, ROCKX_KEY_AUTH_CONFIG, auth_config_.c_str(), 0);
        RockXSetParamInt(handle_, NULL, "src_sample_rate", 16000); // Default 16kHz

        RockXParamCallback cb;
        cb.OnRelease = param_release_callback;
        RockXSetParamPointer(handle_, "ASR", "param", init_param_, 1, &cb);

        int ret = RockXInit(handle_);
        if (ret != ROCKX_RET_SUCCESS) {
            RCLCPP_ERROR(this->get_logger(), "RockXInit failed, ret=%d", ret);
            // Even if initialization fails, we continue because the node may only be used for
            // testing
            return false;
        }

        const char* out_names[1] = {"asr_result"};
        RockXSetOutputInfo(handle_, "out", out_names, 1, tmp_rockx_result_callback,
                           ROCKX_CALLBACK_ON_SCHED_THREAD);

        // Set audio data buffer
        audio_buffer_.clear();
        is_first_chunk_ = true;

        RCLCPP_INFO(this->get_logger(), "ASR initialized successfully");
        return true;
    }

    void audio_callback(const rkros_audio_msgs::msg::RkAudio::SharedPtr msg) {
        RCLCPP_DEBUG(
            this->get_logger(),
            "Received audio data: sample_rate=%u, channels=%u, bits_per_sample=%u, data_size=%zu",
            msg->sample_rate, msg->channels, msg->bits_per_sample, msg->audio_data.size());

        // Add received audio data to buffer
        if (msg->bits_per_sample != 16) {
            RCLCPP_WARN(this->get_logger(), "Only 16-bit audio is supported, got %u bits",
                        msg->bits_per_sample);
            return;
        }

        // Convert byte array to 16-bit integer
        int16_t* audio_data =
            reinterpret_cast<int16_t*>(const_cast<uint8_t*>(msg->audio_data.data()));
        int sample_count = msg->audio_data.size() / sizeof(int16_t);

        // Process audio data chunks (streaming processing)
        if (handle_) {
            // Calculate 20ms audio chunk size (16kHz * 20ms = 320 samples)
            int chunk_size = 20 * msg->sample_rate / 1000;

            for (int i = 0; i < sample_count; i += chunk_size) {
                int current_chunk_size =
                    (i + chunk_size > sample_count) ? (sample_count - i) : chunk_size;

                int16_t* buffer = (int16_t*)malloc(current_chunk_size * sizeof(int16_t));
                memcpy(buffer, &audio_data[i], current_chunk_size * sizeof(int16_t));

                RockXInput* input = RockXInputCreate(handle_);

                RockXPacket* input_packet = RockXInputGetPacketAt(input, 0);
                RockXStreamState state = ROCKX_STREAM_STATE_CONTINUE;
                if (i == 0) {
                    state = ROCKX_STREAM_STATE_FIRST;
                } else if (i + current_chunk_size >= sample_count) {
                    state = ROCKX_STREAM_STATE_LAST;
                }

                RockXAudioWav wav_input;
                memset(&wav_input, 0, sizeof(RockXAudioWav));
                wav_input.sampleNum = current_chunk_size;
                wav_input.data = buffer;
                wav_input.dtype = ROCKX_DATA_TYPE_INT16;
                wav_input.channels = msg->channels;
                wav_input.sampleRate = msg->sample_rate;
                wav_input.state = state;
                RockXPacketSetAudio(input_packet, &wav_input);

                // Set release callback
                RockXPacketSetReleaseCallback(input_packet, audio_release_callback);

                RockXProcessAsync(handle_, input, NULL);

                // usleep(20 * 1000); // Wait for 20ms, simulate real-time processing
            }
        }
    }

    // Static callback function, needs to access class members through userdata
    static void asr_result_callback(const RockAsrResult* result, void* userdata) {
        RkAsrNode* node = static_cast<RkAsrNode*>(userdata);
        if (result == NULL) {
            // RCLCPP_DEBUG(node->get_logger(), "asr_output is NULL");
            return;
        }

        RCLCPP_INFO(node->get_logger(), "ASR Result - State: %d, Output: %s, New Output: %s",
                    result->state, result->result, result->newResult);

        // Create and publish ASR result message
        auto asr_result_msg = rkros_asr_msgs::msg::RkAsrResult();
        asr_result_msg.result = result->result ? std::string(result->result) : "";
        asr_result_msg.new_result = result->newResult ? std::string(result->newResult) : "";
        asr_result_msg.state = result->state;
        asr_result_msg.timestamp = node->now();

        node->publisher_->publish(asr_result_msg);

        // Check if new_result field contains key commands
        std::string new_result = asr_result_msg.new_result;
        // RCLCPP_INFO(node->get_logger(), "new_result value: '%s', length: %zu, empty: %s",
        //            new_result.c_str(), new_result.length(), new_result.empty() ? "true" :
        //            "false");

        if (!new_result.empty()) {
            // RCLCPP_INFO(node->get_logger(), "Processing non-empty new_result");
            // Convert to lowercase for matching (for English commands)
            std::string lower_result = new_result;
            std::transform(lower_result.begin(), lower_result.end(), lower_result.begin(),
                           [](unsigned char c) { return std::tolower(c); });

            // Check if contains movement commands
            if (node->contains_keyword(lower_result, new_result, {"向左", "turn left", "left"})) {
                node->publish_twist_command(0.0, 0.3, 0.0, 0.0, 0.0, 0.5); // Move left and turn
                RCLCPP_INFO(node->get_logger(), "Publishing LEFT command");
            } else if (node->contains_keyword(lower_result, new_result,
                                              {"向右", "turn right", "right"})) {
                node->publish_twist_command(0.0, -0.3, 0.0, 0.0, 0.0, -0.5); // Move right and turn
                RCLCPP_INFO(node->get_logger(), "Publishing RIGHT command");
            } else if (node->contains_keyword(lower_result, new_result,
                                              {"前进", "go forward", "forward", "ahead"})) {
                node->publish_twist_command(0.3, 0.0, 0.0, 0.0, 0.0, 0.0); // Move forward
                RCLCPP_INFO(node->get_logger(), "Publishing FORWARD command");
            } else if (node->contains_keyword(lower_result, new_result,
                                              {"后退", "go back", "backward", "back"})) {
                node->publish_twist_command(-0.3, 0.0, 0.0, 0.0, 0.0, 0.0); // Move backward
                RCLCPP_INFO(node->get_logger(), "Publishing BACKWARD command");
            } else if (node->contains_keyword(lower_result, new_result, {"停", "停止", "stop"})) {
                node->publish_twist_command(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); // Stop
                RCLCPP_INFO(node->get_logger(), "Publishing STOP command");
            }
        } else {
            // RCLCPP_INFO(node->get_logger(), "new_result is empty, skipping keyword check");
        }
    }

    static void param_release_callback(struct RockXParam* param) {
        printf("ParamReleaseCallback\n");
        RockXAsrInitParam* asr_param = (RockXAsrInitParam*)RockXParamGetPointer(param, NULL);
        if (asr_param != NULL) {
            if (asr_param->modelName != NULL) {
                free(asr_param->modelName);
            }
            free(asr_param);
        }
    }

    static void audio_release_callback(struct RockXPacket* packet, void* /*userdata*/) {
        // printf("AudioReleaseCallback\n");
        RockXAudioWav* audio = (RockXAudioWav*)RockXPacketGetAudio(packet);
        if (audio != NULL) {
            if (audio->data != NULL) {
                free(audio->data);
            }
        }
    }

    static void tmp_rockx_result_callback(RockXHandle /*handle*/, RockXOutput* /*output*/,
                                          void* /*userdata*/) {
        return;
    }

    // Check if string contains specified keywords (Chinese and English)
    bool contains_keyword(const std::string& lower_text, const std::string& original_text,
                          const std::vector<std::string>& keywords) {
        // RCLCPP_INFO(this->get_logger(), "Checking keywords: lower_text='%s', original_text='%s'",
        //             lower_text.c_str(), original_text.c_str());

        for (const auto& keyword : keywords) {
            // For English keywords, use lowercase matching
            if (keyword == "left" || keyword == "right" || keyword == "forward" ||
                keyword == "back" || keyword == "stop" || keyword == "ahead") {
                if (lower_text.find(keyword) != std::string::npos) {
                    RCLCPP_INFO(this->get_logger(), "Found English keyword: %s", keyword.c_str());
                    return true;
                }
            } else {
                // For Chinese keywords, use original text matching
                if (original_text.find(keyword) != std::string::npos) {
                    RCLCPP_INFO(this->get_logger(), "Found Chinese keyword: %s", keyword.c_str());
                    return true;
                }
            }
        }
        // RCLCPP_INFO(this->get_logger(), "No keywords found");
        return false;
    }

    // Publish Twist command
    void publish_twist_command(double linear_x, double linear_y, double linear_z, double angular_x,
                               double angular_y, double angular_z) {
        auto twist_msg = geometry_msgs::msg::Twist();
        twist_msg.linear.x = linear_x;
        twist_msg.linear.y = linear_y;
        twist_msg.linear.z = linear_z;
        twist_msg.angular.x = angular_x;
        twist_msg.angular.y = angular_y;
        twist_msg.angular.z = angular_z;

        twist_publisher_->publish(twist_msg);
    }

    rclcpp::Subscription<rkros_audio_msgs::msg::RkAudio>::SharedPtr subscription_;
    rclcpp::Publisher<rkros_asr_msgs::msg::RkAsrResult>::SharedPtr publisher_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twist_publisher_;

    // ASR related variables
    RockXHandle handle_;
    RockXAsrInitParam* init_param_;
    int language_;
    std::string data_path_;
    std::string auth_config_;
    std::string model_name_;

    // Audio buffer
    std::vector<int8_t> audio_buffer_;
    bool is_first_chunk_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RkAsrNode>());
    rclcpp::shutdown();
    return 0;
}
