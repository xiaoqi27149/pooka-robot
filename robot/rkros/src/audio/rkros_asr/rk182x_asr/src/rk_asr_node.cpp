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
#include <chrono>
#include <memory>
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
struct RockXModule* GetRockXModuleSIMT();
}

class RkAsrNode : public rclcpp::Node {
private:
    // Timer for handling end of speech
    rclcpp::TimerBase::SharedPtr speech_end_timer_;
    bool is_processing_speech_;
    bool is_first_audio_data_;
    std::chrono::steady_clock::time_point last_audio_time_;
    static constexpr int SPEECH_END_TIMEOUT_MS = 2000; // 2 seconds of silence to end speech

    void speech_end_callback() {
        // Check if we've had audio recently, if not, send the LAST packet to end the stream
        if (is_processing_speech_) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - last_audio_time_)
                    .count();

            if (elapsed > SPEECH_END_TIMEOUT_MS) {
                // Send LAST packet to indicate end of speech - similar to demo
                send_last_audio_packet();
                is_processing_speech_ = false;
                is_first_audio_data_ = true; // Reset for next audio session
                RCLCPP_INFO(this->get_logger(), "Speech ended, sent LAST packet");
            }
        }
    }

    void send_last_audio_packet() {
        if (handle_) {
            // Create a short silence packet with LAST state
            int16_t* buffer_null =
                (int16_t*)malloc(16000 * 1 * sizeof(int16_t)); // 1 second of silence at 16kHz
            if (buffer_null != nullptr) {
                memset(buffer_null, 0, 16000 * 1 * sizeof(int16_t));

                RockXInput* input_null = RockXInputCreate(handle_);
                if (input_null != nullptr) {
                    RockXPacket* input_packet_null = RockXInputGetPacketAt(input_null, 0);
                    if (input_packet_null != nullptr) {
                        // Set audio packet as LAST
                        RockXPacketSetAudio2(input_packet_null, ROCKX_STREAM_STATE_LAST, 16000, 1,
                                             16000, ROCKX_DATA_TYPE_INT16, buffer_null);

                        // Set release callback to properly free the buffer
                        RockXPacketSetReleaseCallback(input_packet_null, audio_release_callback);

                        // Process the last packet
                        int ret = RockXProcessAsync(handle_, input_null, NULL);
                        if (ret != ROCKX_RET_SUCCESS) {
                            RCLCPP_ERROR(this->get_logger(), "Failed to send LAST packet, ret=%d",
                                         ret);
                        }
                        // Don't destroy input here as it will be handled by RockXProcessAsync
                    } else {
                        free(buffer_null);
                        RockXInputDestroy(handle_, input_null);
                    }
                } else {
                    free(buffer_null);
                }
            }
        }
    }

    bool init_asr() {
        // Register RockX module for RK182X - this appears to be needed in the demo
        RockXModuleGlobalRegister(GetRockXModuleSIMT());

        init_param_ = (RockXAsrInitParam*)malloc(sizeof(RockXAsrInitParam));
        if (init_param_ == NULL) {
            RCLCPP_ERROR(this->get_logger(), "Failed to allocate init_param");
            return false;
        }
        memset(init_param_, 0, sizeof(RockXAsrInitParam));

        // Set up initialization parameters according to the demo
        init_param_->workMode = ROCKASR_MODE_STREAM;
        init_param_->sampleRate = 16000; // Default sample rate for real-time processing
        init_param_->resultCallback = asr_result_callback;
        init_param_->userdata = this;

        // Set model name - this is critical for the SIMT module to work properly
        init_param_->modelName = (char*)"ASR"; // Use literal string as in demo

        // Language settings - adjust based on parameter
        if (language_ == 1) {
            // Chinese to English
            init_param_->langMode.source = ROCKASR_LANG_CHINESE;
            init_param_->langMode.target = ROCKASR_LANG_ENGLISH;
        } else {
            // English to Chinese
            init_param_->langMode.source = ROCKASR_LANG_ENGLISH;
            init_param_->langMode.target = ROCKASR_LANG_CHINESE;
        }

        // Create RockX handle
        handle_ = RockXCreate();
        if (!handle_) {
            RCLCPP_ERROR(this->get_logger(), "Failed to create RockX handle");
            if (init_param_) {
                free(init_param_);
                init_param_ = NULL;
            }
            return false;
        }

        // Set RockX parameters following the exact sequence from the demo
        RockXSetParamString(handle_, NULL, ROCKX_KEY_MODULE_NAME, "SIMT", 0);
        RockXSetParamString(handle_, NULL, ROCKX_KEY_DATA_PATH, data_path_.c_str(), 0);
        RockXSetParamString(handle_, NULL, ROCKX_KEY_AUTH_CONFIG, auth_config_.c_str(), 0);

        // Set initialization parameters to SIMT module
        RockXParamCallback cb;
        cb.OnRelease = param_release_callback;
        RockXSetParamPointer(handle_, "SIMT", "param", init_param_, 1, &cb);

        // Additional SIMT-specific parameters as in the demo
        RockXSetParamInt(handle_, "SIMT", "mode", 0); // Mode 0 as in demo
        RockXSetParamInt(handle_, "SIMT", "result_type",
                         ROCKASR_RESULT_TYPE_TRANSCRIPT);   // ASR only
        RockXSetParamInt(handle_, "SIMT", "chunk_size", 3); // Use chunk size 3 for lower latency

        // Set source sample rate (for real-time processing)
        RockXSetParamInt(handle_, NULL, "src_sample_rate", 16000);

        // Initialize the RockX engine
        int ret = RockXInit(handle_);
        if (ret != ROCKX_RET_SUCCESS) {
            RCLCPP_ERROR(this->get_logger(), "RockXInit failed, ret=%d", ret);
            if (init_param_) {
                free(init_param_);
                init_param_ = NULL;
            }
            return false;
        }

        // Set up output info as in the demo
        const char* out_names[1] = {"result"};
        RockXSetOutputInfo(handle_, "out", out_names, 1, tmp_rockx_result_callback,
                           ROCKX_CALLBACK_ON_SCHED_THREAD, NULL);

        // Initialize audio processing variables
        audio_buffer_.clear();
        is_processing_speech_ = false;
        last_audio_time_ = std::chrono::steady_clock::now();

        RCLCPP_INFO(this->get_logger(), "RK182X ASR initialized successfully");
        return true;
    }

    void audio_callback(const rkros_audio_msgs::msg::RkAudio::SharedPtr msg) {
        RCLCPP_DEBUG(
            this->get_logger(),
            "Received audio data: sample_rate=%u, channels=%u, bits_per_sample=%u, data_size=%zu",
            msg->sample_rate, msg->channels, msg->bits_per_sample, msg->audio_data.size());

        if (msg->bits_per_sample != 16) {
            RCLCPP_WARN(this->get_logger(), "Only 16-bit audio is supported, got %u bits",
                        msg->bits_per_sample);
            return;
        }

        // Update speech state and timer
        last_audio_time_ = std::chrono::steady_clock::now();
        is_processing_speech_ = true;

        // Convert byte array to 16-bit integer
        int16_t* audio_data =
            reinterpret_cast<int16_t*>(const_cast<uint8_t*>(msg->audio_data.data()));
        int sample_count = msg->audio_data.size() / sizeof(int16_t);

        // Process audio data - for rkaudio_asr, each message is typically 20ms of audio (e.g., 320
        // samples at 16kHz)
        if (handle_) {
            // Create RockX input for this audio chunk
            RockXInput* input = RockXInputCreate(handle_);
            if (input == NULL) {
                RCLCPP_ERROR(this->get_logger(), "Failed to create RockX input");
                return;
            }

            RockXPacket* input_packet = RockXInputGetPacketAt(input, 0);
            if (input_packet == NULL) {
                RCLCPP_ERROR(this->get_logger(), "Failed to get RockX packet");
                RockXInputDestroy(handle_, input);
                return;
            }

            // Since rkaudio_asr sends small chunks (typically 20ms), handle the stream state
            // properly For the first block of a speaking session, use FIRST; otherwise use CONTINUE
            RockXStreamState state = ROCKX_STREAM_STATE_CONTINUE;
            if (is_first_audio_data_) {
                state = ROCKX_STREAM_STATE_FIRST; // Mark as first chunk of the speaking session
                is_first_audio_data_ = false;
            }
            // Note: We do not use LAST here because this is a streaming scenario
            // The speech end timer will handle sending LAST when appropriate

            // Instead of processing in sub-chunks like the demo, process the entire message in one
            // go Allocate buffer and copy data
            int16_t* buffer = (int16_t*)malloc(sample_count * sizeof(int16_t));
            if (buffer == NULL) {
                RCLCPP_ERROR(this->get_logger(), "Failed to allocate buffer for audio processing");
                RockXInputDestroy(handle_, input);
                return;
            }
            memcpy(buffer, audio_data, sample_count * sizeof(int16_t));

            // Set audio packet using the same API as the demo
            RockXPacketSetAudio2(input_packet, state, msg->sample_rate, msg->channels, sample_count,
                                 ROCKX_DATA_TYPE_INT16, buffer);

            // Set release callback to properly free the buffer
            RockXPacketSetReleaseCallback(input_packet, audio_release_callback);

            // Asynchronously process the audio chunk
            int ret = RockXProcessAsync(handle_, input, NULL);
            if (ret != ROCKX_RET_SUCCESS) {
                RCLCPP_ERROR(this->get_logger(), "RockXProcessAsync failed, ret=%d", ret);
            }

            // For real-time processing of small chunks, use smaller delay than demo
            // The demo's 500ms delay is too long for 20ms chunks
            // usleep(20 * 1000); // 20ms delay to match chunk size
        }
    }

    // Static callback function, needs to access class members through userdata
    static void asr_result_callback(const RockAsrResult* result, void* userdata) {
        RkAsrNode* node = static_cast<RkAsrNode*>(userdata);
        if (result == NULL) {
            return;
        }

        RCLCPP_INFO(node->get_logger(), "ASR Result - Type: %d, State: %d", result->type,
                    result->state);

        if ((result->type & ROCKASR_RESULT_TYPE_TRANSCRIPT) == ROCKASR_RESULT_TYPE_TRANSCRIPT) {
            if (result->result != NULL) {
                RCLCPP_INFO(node->get_logger(), "Transcription result: %s", result->result);

                // Create and publish ASR result message
                auto asr_result_msg = rkros_asr_msgs::msg::RkAsrResult();
                asr_result_msg.result = "";
                asr_result_msg.new_result = result->result ? std::string(result->result) : "";
                asr_result_msg.state = result->state;
                asr_result_msg.timestamp = node->now();

                node->publisher_->publish(asr_result_msg);

                // Check if result field contains key commands
                std::string result_text = asr_result_msg.result;

                if (!result_text.empty()) {
                    // Convert to lowercase for matching (for English commands)
                    std::string lower_result = result_text;
                    std::transform(lower_result.begin(), lower_result.end(), lower_result.begin(),
                                   [](unsigned char c) { return std::tolower(c); });

                    // Check if contains movement commands
                    if (node->contains_keyword(lower_result, result_text,
                                               {"向左", "turn left", "left"})) {
                        node->publish_twist_command(0.0, 0.3, 0.0, 0.0, 0.0,
                                                    0.5); // Move left and turn
                        RCLCPP_INFO(node->get_logger(), "Publishing LEFT command");
                    } else if (node->contains_keyword(lower_result, result_text,
                                                      {"向右", "turn right", "right"})) {
                        node->publish_twist_command(0.0, -0.3, 0.0, 0.0, 0.0,
                                                    -0.5); // Move right and turn
                        RCLCPP_INFO(node->get_logger(), "Publishing RIGHT command");
                    } else if (node->contains_keyword(lower_result, result_text,
                                                      {"前进", "go forward", "forward", "ahead"})) {
                        node->publish_twist_command(0.3, 0.0, 0.0, 0.0, 0.0, 0.0); // Move forward
                        RCLCPP_INFO(node->get_logger(), "Publishing FORWARD command");
                    } else if (node->contains_keyword(lower_result, result_text,
                                                      {"后退", "go back", "backward", "back"})) {
                        node->publish_twist_command(-0.3, 0.0, 0.0, 0.0, 0.0, 0.0); // Move backward
                        RCLCPP_INFO(node->get_logger(), "Publishing BACKWARD command");
                    } else if (node->contains_keyword(lower_result, result_text,
                                                      {"停", "停止", "stop"})) {
                        node->publish_twist_command(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); // Stop
                        RCLCPP_INFO(node->get_logger(), "Publishing STOP command");
                    }
                }
            }
        }
    }

    static void param_release_callback(struct RockXParam* param) {
        printf("ParamReleaseCallback\n");
        RockXAsrInitParam* asr_param = (RockXAsrInitParam*)RockXParamGetPointer(param, NULL);
        if (asr_param != NULL) {
            // modelName is a string literal, don't free it
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

public:
    RkAsrNode() : Node("rk_asr_node"), is_processing_speech_(false), is_first_audio_data_(true) {
        // Get package share directory to locate config file
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
        // Absolute data path - this will be manually copied to /data/rkaudio/asr_182x
        this->declare_parameter<std::string>("data_path", "/data/rkaudio/asr_182x");
        // Relative authentication config path - using package share directory
        this->declare_parameter<std::string>("auth_config",
                                             package_share_dir + "/config/rockx_auth_config.json");
        // Model name - for RK182X, we use ASR model
        this->declare_parameter<std::string>("model_name", "ASR");

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

        // Create Twist message publisher for movement commands
        twist_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

        // Initialize speech end timer
        speech_end_timer_ =
            this->create_wall_timer(std::chrono::milliseconds(SPEECH_END_TIMEOUT_MS),
                                    std::bind(&RkAsrNode::speech_end_callback, this));

        // Initialize ASR
        if (init_asr()) {
            RCLCPP_INFO(this->get_logger(), "RK182X ASR initialized successfully");
        } else {
            RCLCPP_WARN(this->get_logger(), "RK182X ASR initialization failed, but node will "
                                            "continue to run for testing purposes");
        }

        last_audio_time_ = std::chrono::steady_clock::now();
        RCLCPP_INFO(this->get_logger(), "RK ASR node initialized");
    }
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RkAsrNode>());
    rclcpp::shutdown();
    return 0;
}
