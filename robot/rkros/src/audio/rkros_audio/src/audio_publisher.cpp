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

#include <alsa/asoundlib.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "rkros_audio_msgs/msg/rk_audio.hpp"

using namespace std::chrono_literals;

class AudioPublisher : public rclcpp::Node {
public:
    AudioPublisher() : Node("audio_publisher") {
        publisher_ = this->create_publisher<rkros_audio_msgs::msg::RkAudio>("rkaudio", 10);

        // Initialize ALSA
        int rc = snd_pcm_open(&capture_handle_, "default", SND_PCM_STREAM_CAPTURE, 0);
        if (rc < 0) {
            RCLCPP_ERROR(this->get_logger(), "Unable to open audio device: %s", snd_strerror(rc));
            return;
        }

        // Set audio parameters (16kHz, 16-bit, mono)
        snd_pcm_hw_params_alloca(&hw_params_);
        snd_pcm_hw_params_any(capture_handle_, hw_params_);
        snd_pcm_hw_params_set_access(capture_handle_, hw_params_, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(capture_handle_, hw_params_, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(capture_handle_, hw_params_, 1);
        unsigned int sample_rate = 16000;
        int dir = 0;
        snd_pcm_hw_params_set_rate_near(capture_handle_, hw_params_, &sample_rate, &dir);
        snd_pcm_hw_params(capture_handle_, hw_params_);

        snd_pcm_sw_params_alloca(&sw_params_);
        snd_pcm_sw_params_current(capture_handle_, sw_params_);
        snd_pcm_sw_params_set_avail_min(capture_handle_, sw_params_, 160); // 10ms buffer
        snd_pcm_sw_params(capture_handle_, sw_params_);

        timer_ = this->create_wall_timer(10ms, std::bind(&AudioPublisher::timer_callback, this));

        RCLCPP_INFO(this->get_logger(), "Audio publisher initialized with 16kHz sample rate");
    }

    ~AudioPublisher() {
        if (capture_handle_) {
            snd_pcm_drain(capture_handle_);
            snd_pcm_close(capture_handle_);
        }
    }

private:
    void timer_callback() {
        rkros_audio_msgs::msg::RkAudio msg;
        msg.sample_rate = 16000;  // 16kHz
        msg.channels = 1;         // mono
        msg.bits_per_sample = 16; // 16-bit

        // Buffer to store audio data (10ms of audio at 16kHz = 160 samples)
        std::vector<char> buffer(160 * 2); // 160 samples * 2 bytes per sample

        int rc = snd_pcm_readi(capture_handle_, buffer.data(), 160);
        if (rc == -EPIPE) {
            RCLCPP_WARN(this->get_logger(), "Overrun occurred");
            snd_pcm_prepare(capture_handle_);
        } else if (rc < 0) {
            RCLCPP_ERROR(this->get_logger(), "Error from read: %s", snd_strerror(rc));
        } else if (rc != 160) {
            RCLCPP_WARN(this->get_logger(), "Short read, read %d frames", rc);
        } else {
            // Copy audio data to message
            msg.audio_data.resize(rc * 2); // 2 bytes per sample
            std::copy(buffer.begin(), buffer.begin() + (rc * 2), msg.audio_data.begin());

            publisher_->publish(msg);
        }
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<rkros_audio_msgs::msg::RkAudio>::SharedPtr publisher_;

    snd_pcm_t* capture_handle_;
    snd_pcm_hw_params_t* hw_params_;
    snd_pcm_sw_params_t* sw_params_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AudioPublisher>());
    rclcpp::shutdown();
    return 0;
}
