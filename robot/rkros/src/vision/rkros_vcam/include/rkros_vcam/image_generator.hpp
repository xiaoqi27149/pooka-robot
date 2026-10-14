// image_generator.hpp
#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <vector>

#include "rkros_dmabuf/dmabuf.hpp"

namespace rkros_vcam {

class ImageGenerator {
  public:
    struct Frame {
        rkros_dmabuf::Image img;
        std::string encoding;
        uint64_t frame_no{0};
    };

    ImageGenerator(int width, int height, double fps,
                   const std::string &format = "rgb");
    ~ImageGenerator();

    void start();
    void stop();

    // retrieve latest frame (thread-safe). Returns true if frame available
    bool get_latest_frame(Frame &out_frame);

  private:
    void thread_func();

    int width_;
    int height_;
    double fps_;
    std::string format_;

    std::thread th_;
    std::atomic<bool> running_{false};

    std::mutex mtx_;
    Frame latest_frame_;
    uint64_t frame_no_{0};
};

} // namespace rkros_vcam
