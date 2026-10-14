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
#include <cmath>
#include <condition_variable>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <linux/input.h>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rk_audio_hci.h"
#include "rkros_audio_msgs/msg/rk_audio.hpp"
#include "std_msgs/msg/float32.hpp"
#include <fstream>

// WAV file utilities (similar to wav_utils.h)
#pragma pack(push, 1)

typedef struct {
    char riff_tag[4];      // "RIFF"
    uint32_t riff_len;     // Remaining length after this, usually file_size - 8
    char wave_tag[4];      // "WAVE"
    char fmt_tag[4];       // "fmt "
    uint32_t fmt_len;      // Usually 16 for PCM
    uint16_t audio_format; // 1 for PCM, 6 for a-Law, 7 for u-Law
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;   // SampleRate * NumChannels * BitsPerSample/8
    uint16_t block_align; // NumChannels * BitsPerSample/8
    uint16_t bits_per_sample;
    char data_tag[4];  // "data"
    uint32_t data_len; // Length of data in bytes
} wav_header_t;

#pragma pack(pop)

// Function declarations for WAV utilities
int write_wav_header(std::ofstream* f, uint32_t sample_rate, uint16_t num_channels,
                     uint16_t bits_per_sample);
int write_wav_data(std::ofstream* f, const void* data, uint32_t num_samples);
int update_wav_header(std::ofstream* f);

// Function to extract first channel from multi-channel audio data
int extract_first_channel(const short* data_in, short* data_out, int data_size);

using namespace std::chrono_literals;

#define SAMPLE_RATE 16000
#define FRAME_LEN 320 // 20ms at 16kHz = 320 samples
#define CHANNELS 10   // Fixed to 10 input channels
#define OUT_NUM 1     // Fixed to 1 output channel

class AudioRkProcessor : public rclcpp::Node {
public:
    AudioRkProcessor() : Node("audio_rk_processor") {
        // Declare parameters with default values
        this->declare_parameter("save_wav_files", false);
        this->get_parameter("save_wav_files", save_wav_files_);

        // Publish processed audio to rkaudio topic (single channel output)
        publisher_ = this->create_publisher<rkros_audio_msgs::msg::RkAudio>("rkaudio", 10);

        // Publish ASR audio data to rkaudio_asr topic
        asr_publisher_ = this->create_publisher<rkros_audio_msgs::msg::RkAudio>("rkaudio_asr", 10);

        // Publish DOA data to rk_doa topic
        doa_publisher_ = this->create_publisher<std_msgs::msg::Float32>("rk_doa", 10);

        // Load Rockchip library dynamically from installed location
        std::string package_share_dir;
        try {
            package_share_dir = ament_index_cpp::get_package_share_directory("rkros_audio");
        } catch (const std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "Cannot find package share directory: %s", e.what());
            RCLCPP_ERROR(
                this->get_logger(),
                "Make sure the ROS2 environment is sourced before running this executable.");
            return;
        }

        // Construct path to config file from the same directory
        std::string original_config_path = package_share_dir + "/para/rk_audio_hci.json";

        // Read the original config file and create a temporary version with absolute paths
        std::ifstream config_file(original_config_path);
        if (!config_file.is_open()) {
            RCLCPP_ERROR(this->get_logger(), "Cannot open config file: %s",
                         original_config_path.c_str());
            return;
        }

        std::stringstream buffer;
        buffer << config_file.rdbuf();
        std::string config_content = buffer.str();
        config_file.close();

        // Replace relative paths with absolute paths in the config content
        std::string para_dir = package_share_dir + "/para/";
        std::string abs_config_content = config_content;

        // Helper function to replace all occurrences of a substring
        auto replace_all = [](std::string& str, const std::string& from, const std::string& to) {
            size_t pos = 0;
            while ((pos = str.find(from, pos)) != std::string::npos) {
                str.replace(pos, from.length(), to);
                pos += to.length();
            }
        };

        // Replace relative paths with absolute paths (replace all occurrences)
        replace_all(abs_config_content, "./para", para_dir);

        // Create a temporary config file with absolute paths
        temp_config_path_ = std::filesystem::temp_directory_path() / "rk_audio_hci_temp.json";
        std::ofstream temp_config_file(temp_config_path_);
        if (!temp_config_file.is_open()) {
            RCLCPP_ERROR(this->get_logger(), "Cannot create temporary config file: %s",
                         temp_config_path_.c_str());
            return;
        }
        temp_config_file << abs_config_content;
        temp_config_file.close();

        std::string config_path = temp_config_path_;

        // Initialize ALSA
        int rc = snd_pcm_open(&capture_handle_, "default", SND_PCM_STREAM_CAPTURE, 0);
        if (rc < 0) {
            RCLCPP_ERROR(this->get_logger(), "Unable to open audio device: %s", snd_strerror(rc));
            return;
        }

        // Set audio parameters (8 channels, 16kHz, 16-bit for Rockchip processing)
        snd_pcm_hw_params_alloca(&hw_params_);
        snd_pcm_hw_params_any(capture_handle_, hw_params_);
        snd_pcm_hw_params_set_access(capture_handle_, hw_params_, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(capture_handle_, hw_params_, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(capture_handle_, hw_params_,
                                       CHANNELS); // 8 channels for Rockchip
        unsigned int sample_rate = SAMPLE_RATE;
        int dir = 0;
        snd_pcm_hw_params_set_rate_near(capture_handle_, hw_params_, &sample_rate, &dir);
        // Set appropriate period and buffer sizes for consistent 20ms timing
        snd_pcm_uframes_t period_size = FRAME_LEN;     // 20ms of audio data
        snd_pcm_uframes_t buffer_size = FRAME_LEN * 4; // 80ms of audio data total buffer
        snd_pcm_hw_params_set_period_size_near(capture_handle_, hw_params_, &period_size, &dir);
        snd_pcm_hw_params_set_buffer_size_near(capture_handle_, hw_params_, &buffer_size);
        snd_pcm_hw_params(capture_handle_, hw_params_);

        snd_pcm_sw_params_alloca(&sw_params_);
        snd_pcm_sw_params_current(capture_handle_, sw_params_);
        snd_pcm_sw_params_set_avail_min(capture_handle_, sw_params_,
                                        FRAME_LEN); // 20ms buffer for 8 channels
        snd_pcm_sw_params(capture_handle_, sw_params_);

        // Initialize Rockchip audio processing library with config from installed location
        RCLCPP_INFO(this->get_logger(),
                    "Attempting to initialize Rockchip Audio HCI with config file: %s",
                    config_path.c_str());
        rk_hci_handle_ = rk_audio_hci_init(config_path.c_str());
        if (!rk_hci_handle_) {
            RCLCPP_ERROR(this->get_logger(),
                         "Failed to initialize Rockchip Audio HCI with config file: %s",
                         config_path.c_str());
            RCLCPP_ERROR(this->get_logger(), "Package share directory: %s",
                         package_share_dir.c_str());
            return;
        }
        RCLCPP_INFO(this->get_logger(), "Successfully initialized Rockchip Audio HCI");

        // Print Rockchip Audio HCI version
        const char* version = rk_audio_hci_get_version(rk_hci_handle_);
        if (version) {
            RCLCPP_INFO(this->get_logger(), "Rockchip Audio HCI Version: %s", version);
        } else {
            RCLCPP_WARN(this->get_logger(), "Could not retrieve Rockchip Audio HCI version");
        }

        // Initialize WAV file recording if enabled
        if (save_wav_files_) {
            // Open WAV files for recording
            unprocessed_wav_file_.open("in.wav", std::ios::binary);
            if (unprocessed_wav_file_.is_open()) {
                write_wav_header(&unprocessed_wav_file_, SAMPLE_RATE, CHANNELS,
                                 16); // SAMPLE_RATE, CHANNELS, 16-bit
                RCLCPP_INFO(this->get_logger(), "Recording unprocessed audio to: in.wav");
            } else {
                RCLCPP_ERROR(this->get_logger(), "Cannot open file for writing: in.wav");
            }

            processed_wav_file_.open("out.wav", std::ios::binary);
            if (processed_wav_file_.is_open()) {
                write_wav_header(&processed_wav_file_, SAMPLE_RATE, OUT_NUM,
                                 16); // SAMPLE_RATE, OUT_NUM, 16-bit
                RCLCPP_INFO(this->get_logger(), "Recording processed audio to: out.wav");
            } else {
                RCLCPP_ERROR(this->get_logger(), "Cannot open file for writing: out.wav");
            }

            doa_wav_file_.open("doa.wav", std::ios::binary);
            if (doa_wav_file_.is_open()) {
                write_wav_header(&doa_wav_file_, SAMPLE_RATE, OUT_NUM,
                                 16); // SAMPLE_RATE, OUT_NUM, 16-bit
                RCLCPP_INFO(this->get_logger(), "Recording DOA audio to: doa.wav");
            } else {
                RCLCPP_ERROR(this->get_logger(), "Cannot open file for writing: doa.wav");
            }
        } else {
            RCLCPP_INFO(this->get_logger(), "WAV file saving is disabled");
        }

        // Initialize key event detection
        initialize_key_event_detection();

        // Initialize ANR level variables
        anr_level_ = 2; // Default to 2 to use Rockchip audio library
        key_press_count_ = 0;

        // Start audio processing threads
        processing_running_ = true;
        audio_capture_thread_ = std::thread(&AudioRkProcessor::audio_capture_loop, this);
        audio_processing_thread_ = std::thread(&AudioRkProcessor::audio_processing_loop, this);

        RCLCPP_INFO(this->get_logger(),
                    "Audio RK processor initialized with 16kHz sample rate, %d channels", CHANNELS);
    }

    ~AudioRkProcessor() {
        // Stop processing threads
        processing_running_ = false;

        // Notify the buffer condition variable to wake up waiting threads
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            buffer_cv_.notify_all();
        }

        if (audio_capture_thread_.joinable()) {
            audio_capture_thread_.join();
        }

        if (audio_processing_thread_.joinable()) {
            audio_processing_thread_.join();
        }

        if (capture_handle_) {
            snd_pcm_drain(capture_handle_);
            snd_pcm_close(capture_handle_);
        }

        if (rk_hci_handle_) {
            rk_audio_hci_destory(rk_hci_handle_);
        }

        // Stop and join key event thread
        if (key_event_thread_running_) {
            key_event_thread_running_ = false;
            if (key_event_thread_.joinable()) {
                key_event_thread_.join();
            }
        }

        // Update WAV headers and close files if saving is enabled
        if (save_wav_files_) {
            if (unprocessed_wav_file_.is_open()) {
                update_wav_header(&unprocessed_wav_file_);
                unprocessed_wav_file_.close();
            }
            if (processed_wav_file_.is_open()) {
                update_wav_header(&processed_wav_file_);
                processed_wav_file_.close();
            }
            if (doa_wav_file_.is_open()) {
                update_wav_header(&doa_wav_file_);
                doa_wav_file_.close();
            }
        }

        // Clean up temporary config file
        if (!temp_config_path_.empty()) {
            try {
                std::filesystem::remove(temp_config_path_);
            } catch (const std::exception& e) {
                RCLCPP_WARN(this->get_logger(), "Failed to remove temporary config file: %s",
                            e.what());
            }
        }
    }

private:
    rclcpp::TimerBase::SharedPtr timer_; // Not used but kept for compatibility
    rclcpp::Publisher<rkros_audio_msgs::msg::RkAudio>::SharedPtr
        publisher_; // Processed audio (to rkaudio topic)
    rclcpp::Publisher<rkros_audio_msgs::msg::RkAudio>::SharedPtr
        asr_publisher_;                                                  // ASR audio data publisher
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr doa_publisher_; // DOA data publisher

    snd_pcm_t* capture_handle_;
    snd_pcm_hw_params_t* hw_params_;
    snd_pcm_sw_params_t* sw_params_;
    void* rk_hci_handle_;          // Rockchip audio processing handle
    std::string temp_config_path_; // Path to temporary config file

    // WAV file recording variables
    bool save_wav_files_;                // Switch to enable/disable WAV file saving
    std::ofstream unprocessed_wav_file_; // File for unprocessed audio (in.wav)
    std::ofstream processed_wav_file_;   // File for processed audio (out.wav)
    std::ofstream doa_wav_file_;         // File for DOA data (doa.wav)

    // Key event detection variables
    std::thread key_event_thread_;
    bool key_event_thread_running_;
    std::mutex key_event_mutex_;
    std::string key_device_path_;

    // ANR level variables
    int anr_level_;
    int key_press_count_;

    // Asynchronous processing variables
    std::queue<std::vector<short>> raw_audio_buffer_;
    std::mutex buffer_mutex_;
    std::condition_variable buffer_cv_;
    std::thread audio_capture_thread_;
    std::thread audio_processing_thread_;
    std::atomic<bool> processing_running_{false};

    void initialize_key_event_detection();
    void key_event_detection_loop();
    void detect_key_events();
    void handle_key_press(int key_code);
    void set_led_brightness(const std::string& led_name, int brightness);
    void update_leds_for_anr_level();

    // Audio processing helper functions
    void audio_capture_loop();    // New function for capturing audio in separate thread
    void audio_processing_loop(); // New function for processing audio in separate thread
    int perform_audio_processing(const std::vector<short>& input_buffer,
                                 std::vector<short>& output_buffer);
    void publish_processed_audio(const std::vector<short>& processed_buffer, int out_size);
    void publish_asr_audio();
    void save_audio_data(const std::vector<short>& input_buffer,
                         const std::vector<short>& processed_buffer, int out_size);
    void handle_doa_data();
    void handle_vad_data();
    void handle_wakeup_data();
    void handle_sed_data();
};

// Audio processing helper functions
void AudioRkProcessor::audio_capture_loop() {
    // Set the PCM device to non-blocking mode to ensure consistent timing
    snd_pcm_nonblock(capture_handle_,
                     0); // 0 = blocking mode, which is what we want for consistent timing

    while (processing_running_) {
        // Buffer to store audio data (20ms of audio at 16kHz = FRAME_LEN samples per channel *
        // CHANNELS channels)
        std::vector<short> buffer(FRAME_LEN * CHANNELS); // FRAME_LEN samples * CHANNELS channels

        int rc = snd_pcm_readi(capture_handle_, buffer.data(), FRAME_LEN);

        if (rc == -EPIPE) {
            RCLCPP_WARN(this->get_logger(), "Overrun occurred");
            snd_pcm_prepare(capture_handle_);
        } else if (rc < 0) {
            RCLCPP_ERROR(this->get_logger(), "Error from read: %s", snd_strerror(rc));
        } else if (rc != FRAME_LEN) {
            RCLCPP_WARN(this->get_logger(), "Short read, read %d frames", rc);
        } else {
            // Quickly put the raw audio data into the buffer without processing
            {
                std::lock_guard<std::mutex> lock(buffer_mutex_);
                raw_audio_buffer_.push(buffer);
            }
            // Notify processing thread that new data is available
            buffer_cv_.notify_one();
        }
        // Removed the sleep to maintain proper audio timing - ALSA blocking read provides the
        // timing
    }
}

void AudioRkProcessor::audio_processing_loop() {
    while (processing_running_) {
        std::vector<short> input_buffer;

        // Wait for audio data to be available in the buffer
        {
            std::unique_lock<std::mutex> lock(buffer_mutex_);
            buffer_cv_.wait(lock,
                            [this] { return !raw_audio_buffer_.empty() || !processing_running_; });

            if (!processing_running_ && raw_audio_buffer_.empty()) {
                break;
            }

            if (!raw_audio_buffer_.empty()) {
                input_buffer = raw_audio_buffer_.front();
                raw_audio_buffer_.pop();
            }
        }

        if (!input_buffer.empty()) {
            // Create output buffer for processed audio (single channel output)
            std::vector<short> processed_buffer(
                FRAME_LEN); // FRAME_LEN samples for single channel output

            int out_size = perform_audio_processing(input_buffer, processed_buffer);

            if (out_size > 0) {
                publish_processed_audio(processed_buffer, out_size);
                save_audio_data(input_buffer, processed_buffer, out_size);
                if (anr_level_ == 2) {
                    publish_asr_audio();
                    handle_doa_data();
                    handle_vad_data();
                    handle_wakeup_data();
                    handle_sed_data();
                }
            } else {
                RCLCPP_ERROR(this->get_logger(), "Error in audio processing");
            }
        }
    }
}

int AudioRkProcessor::perform_audio_processing(const std::vector<short>& input_buffer,
                                               std::vector<short>& output_buffer) {
    int out_size = 0;
    int data_size = FRAME_LEN * CHANNELS * sizeof(short); // Input size in bytes

    // Check anr_level_ to determine which processing method to use
    if (anr_level_ == 0) {
        // Use direct first channel extraction when anr_level_ is 0
        out_size = extract_first_channel(input_buffer.data(), output_buffer.data(), data_size);
    } else {
        // Process with Rockchip audio library for default or anr_level_ 2
        if (rk_hci_handle_) {
            // Add timing measurements around the rk_audio_hci_process call
            auto start_time = std::chrono::high_resolution_clock::now();
            out_size = rk_audio_hci_process(input_buffer.data(), output_buffer.data(), data_size,
                                            rk_hci_handle_);
            auto end_time = std::chrono::high_resolution_clock::now();

            auto duration =
                std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            auto duration_ms = duration.count() / 1000.0; // Convert microseconds to milliseconds

            // Only print if processing takes more than 20ms
            if (duration_ms > 20.0) {
                RCLCPP_ERROR(this->get_logger(), "rk_audio_hci_process took %.2f milliseconds",
                             duration_ms);
            }
        } else {
            RCLCPP_ERROR(this->get_logger(), "Rockchip audio library not initialized");
            return -1;
        }
    }

    return out_size;
}

void AudioRkProcessor::publish_processed_audio(const std::vector<short>& processed_buffer,
                                               int out_size) {
    // Create message for processed audio data
    rkros_audio_msgs::msg::RkAudio processed_msg;
    processed_msg.sample_rate = SAMPLE_RATE; // 16kHz
    processed_msg.channels = OUT_NUM;        // Single channel after processing
    processed_msg.bits_per_sample = 16;      // 16-bit

    // Copy processed audio data to message
    processed_msg.audio_data.resize(out_size); // out_size is in bytes
    std::copy(reinterpret_cast<const char*>(processed_buffer.data()),
              reinterpret_cast<const char*>(processed_buffer.data()) + out_size,
              processed_msg.audio_data.begin());

    // Publish processed audio data to rkaudio topic (this is the main output)
    publisher_->publish(processed_msg);
}

void AudioRkProcessor::publish_asr_audio() {
    // Get and publish ASR audio data if the function is available
    if (rk_hci_handle_) {
        std::vector<short> asr_buffer(FRAME_LEN); // FRAME_LEN samples for single channel output
        int asr_data_size = rk_audio_hci_post_out(asr_buffer.data(), rk_hci_handle_);

        if (asr_data_size > 0) {
            // Create message for ASR audio data
            rkros_audio_msgs::msg::RkAudio asr_msg;
            asr_msg.sample_rate = SAMPLE_RATE; // 16kHz
            asr_msg.channels = OUT_NUM;        // Single channel after processing
            asr_msg.bits_per_sample = 16;      // 16-bit

            // Copy ASR audio data to message
            asr_msg.audio_data.resize(asr_data_size); // asr_data_size is in bytes
            std::copy(reinterpret_cast<const char*>(asr_buffer.data()),
                      reinterpret_cast<const char*>(asr_buffer.data()) + asr_data_size,
                      asr_msg.audio_data.begin());

            // Publish ASR audio data to rkaudio_asr topic
            asr_publisher_->publish(asr_msg);
        }
    }
}

void AudioRkProcessor::save_audio_data(const std::vector<short>& input_buffer,
                                       const std::vector<short>& processed_buffer, int out_size) {
    // Save unprocessed audio to WAV file if save option is enabled
    if (save_wav_files_ && unprocessed_wav_file_.is_open()) {
        write_wav_data(&unprocessed_wav_file_, input_buffer.data(),
                       FRAME_LEN * CHANNELS); // FRAME_LEN samples * CHANNELS channels
    }

    // Save processed audio to WAV file if save option is enabled
    if (save_wav_files_ && processed_wav_file_.is_open()) {
        // Calculate number of samples from out_size in bytes
        int num_samples = out_size / sizeof(short);
        write_wav_data(&processed_wav_file_, processed_buffer.data(), num_samples);
    }
}

void AudioRkProcessor::handle_doa_data() {
    // Get and print DOA (Direction of Arrival) if available
    // Note: DOA is only available when using Rockchip library
    if (anr_level_ != 0) {
        float doa = rk_audio_hci_get_doa(rk_hci_handle_);
        if (doa >= 0) {
            static float last_doa = -1.0;
            // Use a small epsilon for floating point comparison to handle minor fluctuations
            if (std::abs(doa - last_doa) > 0.01f) {
                RCLCPP_INFO(this->get_logger(), "DOA: %.2f degrees", doa);

                // Publish DOA value to rk_doa topic
                auto doa_msg = std_msgs::msg::Float32();
                doa_msg.data = static_cast<float>(doa);
                doa_publisher_->publish(doa_msg);

                last_doa = doa;
            }
        }

        // Save DOA data to WAV file if save option is enabled
        if (save_wav_files_ && doa_wav_file_.is_open()) {
            // Convert the DOA value (0.0-360.0 range) to short values for WAV file
            short int doa_data[FRAME_LEN * OUT_NUM]; // FRAME_LEN samples for the frame
            float doa_scale = 360.0f;                // Maximum DOA value
            short int scaled_doa = static_cast<short int>((doa / doa_scale) * 32767.0f);

            for (int i = 0; i < FRAME_LEN * OUT_NUM; i++) {
                doa_data[i] = scaled_doa;
            }

            write_wav_data(&doa_wav_file_, doa_data, FRAME_LEN * OUT_NUM);
        }
    }
}

void AudioRkProcessor::handle_vad_data() {
    // Get and print VAD (Voice Activity Detection) if available
    if (rk_hci_handle_) {
        int vad = rk_audio_hci_get_vad(rk_hci_handle_);
        // VAD value is in [0, 1] according to header file
        RCLCPP_DEBUG(this->get_logger(), "VAD: %d", vad);
    }
}

void AudioRkProcessor::handle_wakeup_data() {
    // Get and print wakeup status if available
    if (rk_hci_handle_) {
        int kws_doa = 0;
        int wakeup_status = rk_audio_hci_get_wakeup(&kws_doa, rk_hci_handle_);
        // Wakeup status: 0 means no wakeup, 1 means wakeup word 1, 2 means wakeup word 2, etc.
        if (wakeup_status != 0) {
            RCLCPP_INFO(this->get_logger(), "Wakeup word %d detected, DOA: %d\n", wakeup_status,
                        kws_doa);
        }
    }
}

void AudioRkProcessor::handle_sed_data() {
    // Get and print SED (Scene Event Detection) status if available
    if (rk_hci_handle_) {
        int sed_status = rk_audio_hci_get_sed(rk_hci_handle_);
        // SED status: 0 means nothing detected, 1 means dog barking detected, 2 means baby crying
        // detected
        if (sed_status != 0) {
            std::string sed_event;
            switch (sed_status) {
            case 1:
                sed_event = "Dog barking";
                break;
            case 2:
                sed_event = "Baby crying";
                break;
            default:
                sed_event = "Unknown SED event";
                break;
            }
            RCLCPP_INFO(this->get_logger(), "SED Event: %s detected (code: %d)", sed_event.c_str(),
                        sed_status);
        }
    }
}

// WAV file utilities implementation
int write_wav_header(std::ofstream* f, uint32_t sample_rate, uint16_t num_channels,
                     uint16_t bits_per_sample) {
    wav_header_t header;
    size_t written;

    // Initialize the header
    memcpy(header.riff_tag, "RIFF", 4);
    header.riff_len = 0; // We'll update this later if needed
    memcpy(header.wave_tag, "WAVE", 4);
    memcpy(header.fmt_tag, "fmt ", 4);
    header.fmt_len = 16;     // PCM format is 16 bytes
    header.audio_format = 1; // PCM
    header.num_channels = num_channels;
    header.sample_rate = sample_rate;
    header.bits_per_sample = bits_per_sample;
    header.block_align = num_channels * (bits_per_sample / 8);
    header.byte_rate = sample_rate * num_channels * (bits_per_sample / 8);
    memcpy(header.data_tag, "data", 4);
    header.data_len = 0; // We'll update this later if needed

    // Write the header to file
    f->write(reinterpret_cast<const char*>(&header), sizeof(wav_header_t));
    return f->good() ? 0 : -1;
}

int write_wav_data(std::ofstream* f, const void* data, uint32_t num_samples) {
    f->write(reinterpret_cast<const char*>(data), sizeof(short) * num_samples);
    return f->good() ? 0 : -1;
}

int update_wav_header(std::ofstream* f) {
    // Get current file position
    std::streampos current_pos = f->tellp();

    // Get file size by seeking to end
    f->seekp(0, std::ios::end);
    std::streampos file_size = f->tellp();

    // Calculate the actual data size
    // WAV header is 44 bytes
    uint32_t data_size = static_cast<uint32_t>(static_cast<long>(file_size) - 44);
    uint32_t riff_len = static_cast<uint32_t>(
        static_cast<long>(file_size) - 8); // Total file size minus "RIFF" tag and riff_len field

    // Go back to beginning to rewrite header
    f->seekp(0, std::ios::beg);

    // Instead of rewriting the whole header, let's just update the length fields directly
    // RIFF length is at offset 4 (4 bytes)
    // DATA length is at offset 40 (4 bytes after the other header fields)
    f->seekp(4, std::ios::beg);
    f->write(reinterpret_cast<const char*>(&riff_len), sizeof(uint32_t));

    f->seekp(40, std::ios::beg);
    f->write(reinterpret_cast<const char*>(&data_size), sizeof(uint32_t));

    // Return to original position
    f->seekp(current_pos, std::ios::beg);

    return 0;
}

// Function to extract first channel from multi-channel audio data
// data_in: input buffer with interleaved multi-channel data (8 channels)
// data_out: output buffer for single channel data
// data_size: total size in bytes of input buffer (FRAME_LEN * CHANNELS * sizeof(short))
// Returns: number of bytes in output buffer (frame_count * sizeof(short))
int extract_first_channel(const short* data_in, short* data_out, int data_size) {
    int total_samples = data_size / sizeof(short); // Convert bytes to number of samples
    int frame_count = total_samples / CHANNELS;    // Number of frames (samples per channel)

    // Extract every CHANNELS'th sample (first channel from interleaved multi-channel data)
    for (int i = 0; i < frame_count; i++) {
        data_out[i] = data_in[i * CHANNELS]; // Extract first channel (index 0, CHANNELS,
                                             // 2*CHANNELS, 3*CHANNELS...)
    }

    return frame_count * sizeof(short); // Return number of output bytes
}

void AudioRkProcessor::initialize_key_event_detection() {
    // Set default key device path - can be parameterized later
    key_device_path_ = "/dev/input/event5"; // Updated to listen on /dev/input/event5

    // Check if the default device exists, if not try to find a suitable input device
    if (access(key_device_path_.c_str(), F_OK) != 0) {
        // Try to find an appropriate input device
        for (int i = 0; i < 10; i++) {
            std::string device_path = "/dev/input/event" + std::to_string(i);
            if (access(device_path.c_str(), F_OK) == 0) {
                key_device_path_ = device_path;
                RCLCPP_INFO(this->get_logger(), "Found input device at: %s", device_path.c_str());
                break;
            }
        }
    }

    key_event_thread_running_ = true;
    key_event_thread_ = std::thread(&AudioRkProcessor::key_event_detection_loop, this);

    RCLCPP_INFO(this->get_logger(), "Key event detection initialized with device: %s",
                key_device_path_.c_str());
}

void AudioRkProcessor::key_event_detection_loop() {
    while (key_event_thread_running_) {
        detect_key_events();
        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void AudioRkProcessor::set_led_brightness(const std::string& led_name, int brightness) {
    std::string led_path = "/sys/class/leds/" + led_name + "/brightness";
    std::ofstream led_file(led_path);
    if (led_file.is_open()) {
        led_file << brightness << std::endl;
        led_file.close();
    } else {
        RCLCPP_WARN(this->get_logger(), "Cannot open %s for writing", led_path.c_str());
    }
}

void AudioRkProcessor::update_leds_for_anr_level() {
    if (anr_level_ == 2) {
        // ANR level 2: led19 on, led21 off
        set_led_brightness("led19", 255);
        set_led_brightness("led21", 0);
    } else if (anr_level_ == 0) {
        // ANR level 0: led19 off, led21 on
        set_led_brightness("led19", 0);
        set_led_brightness("led21", 255);
    }
}

void AudioRkProcessor::handle_key_press(int key_code) {
    // Check if the pressed key is 115 (typically corresponds to 'F4' or 'Volume Down' depending on
    // the device)
    if (key_code != 115) {
        return;
    }

    // Toggle ANR level between 0 and 2
    if (key_press_count_ % 2 == 0) {
        anr_level_ = 0;
    } else {
        anr_level_ = 2;
    }

    // Call the function to set the ANR level
    if (rk_hci_handle_) {
        int result = rk_audio_hci_set_anr_level(rk_hci_handle_, anr_level_);
        if (result == 0) {
            RCLCPP_INFO(this->get_logger(), "ANR level set to: %d", anr_level_);
            update_leds_for_anr_level();
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to set ANR level: %d", result);
        }
    } else {
        RCLCPP_ERROR(this->get_logger(), "rk_audio_hci_set_anr_level function not available");
    }

    // Increment the key press counter
    key_press_count_++;
}

void AudioRkProcessor::detect_key_events() {
    int fd = open(key_device_path_.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        // Only log error once every few seconds to avoid spam
        static auto last_error_time = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_error_time).count() > 5) {
            RCLCPP_ERROR(this->get_logger(), "Cannot open input device %s: %s",
                         key_device_path_.c_str(), strerror(errno));
            last_error_time = now;
        }
        return;
    }

    struct input_event ev;
    ssize_t n;

    while (key_event_thread_running_) {
        n = read(fd, &ev, sizeof(struct input_event));
        if (n == (ssize_t)-1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No events available, sleep briefly and continue
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            } else {
                // Error occurred
                RCLCPP_ERROR(this->get_logger(), "Error reading input device: %s", strerror(errno));
                break;
            }
        } else if (n != sizeof(struct input_event)) {
            // Partial read, continue
            continue;
        }

        // Check if this is a key event
        if (ev.type == EV_KEY) {
            std::lock_guard<std::mutex> lock(key_event_mutex_);

            // Print key event information
            if (ev.value == 0) {
                RCLCPP_INFO(this->get_logger(), "Key released: %d", ev.code);
            } else if (ev.value == 1) {
                RCLCPP_INFO(this->get_logger(), "Key pressed: %d", ev.code);
                handle_key_press(ev.code);
            } else if (ev.value == 2) {
                RCLCPP_INFO(this->get_logger(), "Key repeated: %d", ev.code);
            }
        }
    }

    close(fd);
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AudioRkProcessor>());
    rclcpp::shutdown();
    return 0;
}
