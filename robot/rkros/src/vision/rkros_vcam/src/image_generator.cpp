#include <algorithm>
#include <chrono>
#include <cstring>
#include <memory>

#include "rkros_common/rk_color.h"
#include "rkros_image_proc/image_proc.hpp"
#include "rkros_opencv/rkcv.hpp"
#include "rkros_vcam/image_generator.hpp"
#include "rkros_vcam/vcam.hpp"

using namespace std::chrono_literals;

namespace rkros_vcam {

static inline void draw_black_block_rgb(uint8_t *buf, int w, int h, int bx, int by,
                                        int bw, int bh) {
    memset(buf, 0xFF, w * h * 3);
    for (int y = by; y < by + bh && y < h; ++y) {
        for (int x = bx; x < bx + bw && x < w; ++x) {
            int idx = (y * w + x) * 3;
            buf[idx + 0] = 0;
            buf[idx + 1] = 0;
            buf[idx + 2] = 0;
        }
    }
}

static bool rgb_to_nv12(cv::UMat &rgb, cv::UMat &yuv) {
    /**
     * opencv4.5.1还不支持rgb转nv12，因而通过rkros_image_proc::cvtColor来转
     */
    uint32_t format = RKCLR_NV12;

    // 1. 把cv::Mat拷贝到rkros_dmabuf::Image
    rkros_dmabuf::Image src_img = rkcv::getImage(rgb);

    // 2. 使用image proc把rgb转成nv12
    rkros_dmabuf::Image dst_img = rkcv::getImage(yuv);
    dst_img.format = format;

    if (!rkros_image_proc::cvtColor(src_img, dst_img)) {
        std::cerr << "convert color failed" << std::endl;
        return false;
    }

    return true;
}

ImageGenerator::ImageGenerator(int width, int height, double fps,
                               const std::string &format)
    : width_(width), height_(height), fps_(fps), format_(format) {
    latest_frame_.img.width = width_;
    latest_frame_.img.height = height_;
    latest_frame_.img.dmabuf = nullptr;
}

ImageGenerator::~ImageGenerator() {
    stop();
}

void ImageGenerator::start() {
    if (running_)
        return;
    running_ = true;
    th_ = std::thread(&ImageGenerator::thread_func, this);
}

void ImageGenerator::stop() {
    if (!running_)
        return;
    running_ = false;
    if (th_.joinable())
        th_.join();
}

void ImageGenerator::thread_func() {
    using clock = std::chrono::steady_clock;
    auto interval_d = std::chrono::duration<double>(1.0 / std::max(1e-6, fps_));
    auto interval = std::chrono::duration_cast<clock::duration>(interval_d);

    int block_w = std::max(1, width_ / 32);
    int block_h = std::max(1, height_ / 32);
    int cols = (width_ + block_w - 1) / block_w;
    int rows = (height_ + block_h - 1) / block_h;
    int total_positions = cols * rows;
    cv::UMat rgb = rkcv::createUMat(height_, width_, CV_8UC3);

    auto next_time = clock::now();
    while (running_) {
        // generate frame into local rgb buffer
        int pos = frame_no_ % total_positions;
        int bx = (pos % cols) * block_w;
        int by = (pos / cols) * block_h;
        rkcv::DmabufPtr buf = rkcv::getDmabuf(rgb);
        draw_black_block_rgb((uint8_t *)buf->map(), width_, height_, bx, by, block_w,
                             block_h);

        // convert according to format_
        cv::UMat out;
        ImageGenerator::Frame f;
        f.img.width = width_;
        f.img.height = height_;
        f.frame_no = frame_no_;
        if (format_ == "yuv") {
            // convert to yuv
            out = rkcv::createUMat(height_ * 3 / 2, width_, CV_8UC1);
            if (!rgb_to_nv12(rgb, out))
                continue;

            f.encoding = "yuv420sp";
            f.img.format = RKCLR_NV12;
        } else if (format_ == "jpeg") {
            // encode to jpeg
            std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 80};
            std::vector<uint8_t> data;
            if (!cv::imencode(".jpg", rgb, data, params)) {
                continue;
            }
            out = rkcv::createUMat();
            cv::Mat temp(1, data.size(), CV_8UC1, data.data());
            temp.copyTo(out);
            f.encoding = "jpeg";
            f.img.format = RKCLR_JPEG;
        } else {
            // default to rgb
            out = rkcv::createUMat();
            rgb.copyTo(out);
            f.encoding = "rgb8";
            f.img.format = RKCLR_RGB_888;
        }
        // 等待gpu任务完成
        cv::ocl::finish();
        // 从cv::Mat取出dmabuf并保存在Frame
        f.img.stride = out.step;
        f.img.dmabuf = rkcv::getDmabuf(out);

        // std::cout << "generate frame " << frame_no_ << std::endl;
        //  publish latest
        {
            std::lock_guard<std::mutex> lk(mtx_);
            latest_frame_ = std::move(f);
            frame_no_++;
        }

        next_time += interval;
        std::this_thread::sleep_until(next_time);
    }
}

bool ImageGenerator::get_latest_frame(Frame &out_frame) {
    std::lock_guard<std::mutex> lk(mtx_);
    if (latest_frame_.img.dmabuf == nullptr)
        return false;
    out_frame = latest_frame_;
    return true;
}

} // namespace rkros_vcam
