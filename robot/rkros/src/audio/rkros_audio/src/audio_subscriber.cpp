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
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include "rclcpp/rclcpp.hpp"
#include "rkros_audio_msgs/msg/rk_audio.hpp"

class AudioSubscriber : public rclcpp::Node {
public:
    AudioSubscriber() : Node("audio_subscriber") {
        subscription_ = this->create_subscription<rkros_audio_msgs::msg::RkAudio>(
            "rkaudio", 10,
            std::bind(&AudioSubscriber::audio_callback, this, std::placeholders::_1));

        // Initialize ALSA playback
        int rc = snd_pcm_open(&playback_handle_, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if (rc < 0) {
            RCLCPP_ERROR(this->get_logger(), "Unable to open audio device for playback: %s",
                         snd_strerror(rc));
            return;
        }

        // Set audio parameters (matching publisher: 16kHz, 16-bit, mono)
        snd_pcm_hw_params_alloca(&hw_params_);
        snd_pcm_hw_params_any(playback_handle_, hw_params_);
        snd_pcm_hw_params_set_access(playback_handle_, hw_params_, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(playback_handle_, hw_params_, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(playback_handle_, hw_params_, 1);
        unsigned int sample_rate = 16000;
        int dir = 0;
        snd_pcm_hw_params_set_rate_near(playback_handle_, hw_params_, &sample_rate, &dir);

        // Set larger hardware buffer to handle timing variations
        snd_pcm_uframes_t period_size = 320;  // 20ms
        snd_pcm_uframes_t buffer_size = 1280; // 80ms, 4x period size
        snd_pcm_hw_params_set_period_size_near(playback_handle_, hw_params_, &period_size, &dir);
        snd_pcm_hw_params_set_buffer_size_near(playback_handle_, hw_params_, &buffer_size);
        snd_pcm_hw_params(playback_handle_, hw_params_);

        snd_pcm_sw_params_alloca(&sw_params_);
        snd_pcm_sw_params_current(playback_handle_, sw_params_);
        snd_pcm_sw_params_set_avail_min(playback_handle_, sw_params_,
                                        period_size / 2); // Reduce minimum available frames
        snd_pcm_sw_params(playback_handle_, sw_params_);

        // Start audio playback thread
        playback_running_ = true;
        audio_playback_thread_ = std::thread(&AudioSubscriber::audio_playback_loop, this);

        RCLCPP_INFO(this->get_logger(),
                    "Audio subscriber initialized with 16kHz sample rate and async playback");
    }

    ~AudioSubscriber() {
        playback_running_ = false;
        buffer_cv_.notify_all(); // Wake up waiting threads

        if (audio_playback_thread_.joinable()) {
            audio_playback_thread_.join();
        }

        if (playback_handle_) {
            snd_pcm_drain(playback_handle_); // Ensure all buffered data is played
            snd_pcm_close(playback_handle_);
        }

        // Free resampling context
        if (swr_ctx_) {
            swr_free(&swr_ctx_);
        }
    }

private:
    std::vector<uint8_t> resample_audio(const std::vector<uint8_t>& input_data, int src_rate,
                                        int dst_rate) {
        if (src_rate == dst_rate) {
            // No resampling needed
            return input_data;
        }

        std::lock_guard<std::mutex> lock(resample_mutex_);

        // Initialize resampler if needed or if source rate changes
        if (!swr_ctx_ || source_sample_rate_ != src_rate) {
            if (swr_ctx_) {
                swr_free(&swr_ctx_);
            }

            swr_ctx_ = swr_alloc();
            if (!swr_ctx_) {
                RCLCPP_ERROR(this->get_logger(), "Failed to allocate SWR context");
                return input_data; // Return original data if resampling fails
            }

            // Set parameters for resampling
            av_opt_set_int(swr_ctx_, "in_channel_layout", AV_CH_LAYOUT_MONO, 0);
            av_opt_set_int(swr_ctx_, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
            av_opt_set_int(swr_ctx_, "in_sample_rate", src_rate, 0);
            av_opt_set_int(swr_ctx_, "out_sample_rate", dst_rate, 0);
            av_opt_set_int(swr_ctx_, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
            av_opt_set_int(swr_ctx_, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

            int ret = swr_init(swr_ctx_);
            if (ret < 0) {
                RCLCPP_ERROR(this->get_logger(), "Failed to initialize SWR context: %d", ret);
                swr_free(&swr_ctx_);
                return input_data; // Return original data if resampling fails
            }

            source_sample_rate_ = src_rate;
        }

        // Calculate input frames
        int input_samples = input_data.size() / sizeof(int16_t);
        const uint8_t* input_ptr = reinterpret_cast<const uint8_t*>(input_data.data());

        // Calculate maximum possible output samples
        int max_output_samples =
            static_cast<int>(av_rescale_rnd(input_samples, dst_rate, src_rate, AV_ROUND_UP));

        // Prepare output buffer
        std::vector<uint8_t> output_buffer(max_output_samples * sizeof(int16_t));
        uint8_t* output_ptr = output_buffer.data();

        // Perform resampling
        const uint8_t* input_array[] = {input_ptr};
        uint8_t* output_array[] = {output_ptr};

        int input_frame_count = input_samples;
        int output_frame_count = max_output_samples;

        int ret =
            swr_convert(swr_ctx_, output_array, output_frame_count, input_array, input_frame_count);

        if (ret < 0) {
            RCLCPP_ERROR(this->get_logger(), "SWR conversion failed: %d", ret);
            return input_data; // Return original data if resampling fails
        }

        // Resize output buffer to actual result size
        output_buffer.resize(ret * sizeof(int16_t));
        return output_buffer;
    }

    void audio_callback(const rkros_audio_msgs::msg::RkAudio::SharedPtr msg) {
        // Resample audio data if necessary
        std::vector<uint8_t> processed_data;
        if (msg->sample_rate != target_sample_rate_) {
            // Perform resampling to target sample rate (16kHz)
            processed_data = resample_audio(msg->audio_data, msg->sample_rate, target_sample_rate_);
        } else {
            // No resampling needed
            processed_data = msg->audio_data;
        }

        // Put processed data into buffer instead of playing immediately
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            audio_buffer_.push(processed_data);

            // Limit buffer size to prevent memory overflow
            if (audio_buffer_.size() > 10) { // Keep at most 10 frames
                audio_buffer_.pop();
            }
        }
        buffer_cv_.notify_one();
    }

    void audio_playback_loop() {
        while (playback_running_) {
            std::vector<uint8_t> audio_data;

            // Wait for audio data
            {
                std::unique_lock<std::mutex> lock(buffer_mutex_);
                buffer_cv_.wait(lock,
                                [this] { return !audio_buffer_.empty() || !playback_running_; });

                if (!playback_running_ && audio_buffer_.empty()) {
                    break;
                }

                if (!audio_buffer_.empty()) {
                    audio_data = audio_buffer_.front();
                    audio_buffer_.pop();
                }
            }

            if (!audio_data.empty() && playback_handle_) {
                // Record snd_pcm_writei call time
                auto write_start_time = std::chrono::steady_clock::now();

                // Need to convert uint8_t to correct audio data format (int16_t)
                // Since the original audio data is int16_t (16-bit), conversion is needed
                int frame_count = audio_data.size() / sizeof(int16_t);
                int16_t* audio_data_ptr = reinterpret_cast<int16_t*>(audio_data.data());

                int rc = snd_pcm_writei(playback_handle_, audio_data_ptr, frame_count);

                auto write_end_time = std::chrono::steady_clock::now();
                auto write_duration = std::chrono::duration_cast<std::chrono::microseconds>(
                    write_end_time - write_start_time);
                double write_duration_ms = write_duration.count() / 1000.0;

                RCLCPP_DEBUG(this->get_logger(), "snd_pcm_writei processing time: %.2f ms",
                             write_duration_ms);

                if (rc == -EPIPE) {
                    RCLCPP_WARN(this->get_logger(), "Underrun occurred");
                    snd_pcm_prepare(playback_handle_);
                    rc = snd_pcm_writei(playback_handle_, audio_data_ptr, frame_count);
                    if (rc == -EPIPE) {
                        RCLCPP_WARN(this->get_logger(),
                                    "Secondary underrun, attempting aggressive recovery");
                        snd_pcm_drop(playback_handle_);
                        snd_pcm_prepare(playback_handle_);
                        rc = snd_pcm_writei(playback_handle_, audio_data_ptr, frame_count);
                    }
                } else if (rc < 0) {
                    RCLCPP_ERROR(this->get_logger(), "Error from write: %s", snd_strerror(rc));
                } else if (rc != frame_count) {
                    RCLCPP_WARN(this->get_logger(), "Short write, wrote %d frames", rc);
                }
            }
        }
    }

    rclcpp::Subscription<rkros_audio_msgs::msg::RkAudio>::SharedPtr subscription_;

    snd_pcm_t* playback_handle_;
    snd_pcm_hw_params_t* hw_params_;
    snd_pcm_sw_params_t* sw_params_;

    // Buffer and synchronization mechanism for asynchronous audio playback
    std::queue<std::vector<uint8_t>>
        audio_buffer_;                          // Audio data buffer (matching RkAudio message type)
    std::mutex buffer_mutex_;                   // Buffer mutex lock
    std::condition_variable buffer_cv_;         // Buffer condition variable
    std::thread audio_playback_thread_;         // Audio playback thread
    std::atomic<bool> playback_running_{false}; // Playback thread running flag

    // Resampling related members
    SwrContext* swr_ctx_{nullptr};
    int source_sample_rate_{0};
    int target_sample_rate_{16000}; // Fixed target sample rate
    std::mutex resample_mutex_;     // Mutex for resampling operations
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AudioSubscriber>());
    rclcpp::shutdown();
    return 0;
}
