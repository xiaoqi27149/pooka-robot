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

#pragma once

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <poll.h>

#include <errno.h>
#include <string.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <atomic>

#include <opencv2/opencv.hpp>

#include "rmsl_logger.h"
#include "rmsl32x_usb.h"

class TimeSyncManager {
public:
    TimeSyncManager();
    ~TimeSyncManager();

    void setFileDescriptor(int fd);
    void stop();

private:
    void timeSyncThread();

    std::atomic<bool> run_flag_{true};
    std::thread time_sync_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    int fd_ = -1;
};

inline uint64_t getCurrentTimeUs_uvc() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
}

static inline int xioctl(int fd, int request, void* arg) {
    int r;
    do { r = ::ioctl(fd, request, arg); } while (r == -1 && errno == EINTR);
    return r;
}

struct Frame {
    int stream_id;
    int width, height;
    int bytes_per_line;
    int data_len;
    int pixelformat;
    int buffer_index;
    int64_t timestamp_us;
    uint8_t* data;

    std::function<void(int)> release_fn;

    cv::Mat toCvMat() const;
    void release();
    ~Frame();
};

static std::shared_ptr<Frame> make_frame_shared(Frame* f) {
    return std::shared_ptr<Frame>(f);
}

class V4L2Stream {
public:
    V4L2Stream(int stream_id, const std::string& dev_path, int desired_width,
               int desired_height, int pixfmt, size_t request_buffers,
               size_t max_queue, int fps);
    ~V4L2Stream();

    void openAndInit();
    void start();
    void stop();

    std::shared_ptr<Frame> getFrame(int timeout_ms);
    std::shared_ptr<Frame> peekFrame();
    void pushFrontFrame(std::shared_ptr<Frame> frame);
    void pushFrame(std::shared_ptr<Frame> f);
    bool isRunning() const;
    bool peekEarliestTimestamp(int64_t& ts_us);
    void dropOldest();
    void setTimeOffsetUs(int64_t offset_us);
    int64_t getTimeOffsetUs() const;

private:
    struct MmapBuf {
        uint8_t* start;
        size_t length;
    };

    std::string dev_path_;
    int fd_;
    int stream_id_;
    int width_, height_;
    int pixfmt_;
    size_t request_buffers_;
    std::vector<MmapBuf> buffers_;
    struct pollfd pfd_;
    std::thread capture_thread_;
    std::atomic<bool> running_;
    size_t max_queue_;
    mutable std::mutex control_mutex_;

    std::deque<std::shared_ptr<Frame>> frame_queue_;
    std::mutex queue_mutex_;
    std::condition_variable frame_cv_;

    int64_t time_offset_us_ = 0;
    std::unique_ptr<TimeSyncManager> time_sync_manager_;
    bool enable_time_sync_ = true;
    int fps_;

    void cleanup_fds();
    void captureLoop();
};

class V4L2DualCapture {
public:
    V4L2DualCapture(const std::string& devA, const std::string& devB,
                    int widthA, int heightA, int pixA, int rgb_fps,
                    int widthB, int heightB, int pixB, int dep_fps);
    ~V4L2DualCapture();

    void start();
    void stop();
    void setMaxPairDiffMs(int ms);
    void setTimeOffsetMs(int stream_id, int offset_ms);
    std::shared_ptr<Frame> getFrame(int stream_id, int timeout_ms);
    bool getSyncedPair(std::shared_ptr<Frame>& outA, std::shared_ptr<Frame>& outB, int timeout_ms);
    bool getSyncedPair_Drive_B(std::shared_ptr<Frame>& outA, std::shared_ptr<Frame>& outB, int timeout_ms);
    bool getLatestPair(std::shared_ptr<Frame>& outA, std::shared_ptr<Frame>& outB, int timeout_ms);

private:
    V4L2Stream streamA_, streamB_;
    int max_pair_diff_ms_;
    std::atomic<bool> running_;

    std::shared_ptr<Frame> findAndPopClosestInStream(V4L2Stream& stream, int64_t target_ts_us);
};

class V4L2TripleCapture {
public:
    V4L2TripleCapture(const std::string& devA, const std::string& devB, const std::string& devC,
                    int widthA, int heightA, int pixA, int rgb_fps,
                    int widthB, int heightB, int pixB,
                    int widthC, int heightC, int pixC);
    ~V4L2TripleCapture();

    void start();
    void stop();
    bool getLatestPair_debug(std::shared_ptr<Frame>& outA, std::shared_ptr<Frame>& outB,
                           std::shared_ptr<Frame>& outC, int timeout_ms);

private:
    V4L2Stream streamA_, streamB_, streamC_;
    int max_pair_diff_ms_;
    std::atomic<bool> running_;
};
