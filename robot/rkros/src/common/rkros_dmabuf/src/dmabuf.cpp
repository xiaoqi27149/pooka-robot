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

#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/dma-heap.h>
#include <linux/types.h>
#include <map>
#include <mutex>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "rkros_common/rk_color.h"
#include "rkros_dmabuf/dmabuf.hpp"

// rockchip defines
#ifndef DMA_BUF_IOCTL_SYNC_PARTIAL
struct dma_buf_sync_partial {
    __u64 flags;
    __u32 offset;
    __u32 len;
};

#define DMA_BUF_IOCTL_SYNC_PARTIAL _IOW(DMA_BUF_BASE, 2, struct dma_buf_sync_partial)
#endif

namespace rkros_dmabuf {
static std::map<std::string, int> heap_list;

DmaBuf::~DmaBuf() {
    // std::cout << "in ~DmaBuf, autoClose is " << autoClose_ << ", fd is " << fd_
    //           << std::endl;
    if (autoClose_) {
        unmap();
        if (rgaHandle_)
            rgaHandleReleaseCB_(rgaHandle_);

        if (oclHandle_) {
            oclHandleReleaseCB_(oclHandle_);
        }

        if (fd_ >= 0)
            ::close(fd_);
    }
}

std::shared_ptr<DmaBuf> DmaBuf::allocate(std::size_t size, const std::string &heapName,
                                         uint32_t heapFlags) {
    if (size == 0) {
        std::cerr << "DmaBuf: invalid size 0" << std::endl;
        return nullptr;
    }

    // 根据heap是否已打开来分别处理
    int heap_fd = -1;
    std::string path = "/dev/dma_heap/" + heapName;
    auto it = heap_list.find(path);
    if (it != heap_list.end()) {
        heap_fd = it->second;
    } else {
        heap_fd = open(path.c_str(), O_RDWR | O_CLOEXEC);
        if (heap_fd < 0) {
            std::cerr << "DmaBuf: open heap " << path << " failed" << std::endl;
            return nullptr;
        }
        // 添加到heap list
        heap_list.emplace(path, heap_fd);
    }

    dma_heap_allocation_data data{};
    data.len = size;
    data.fd_flags = O_CLOEXEC | O_RDWR;
    data.heap_flags = heapFlags;

    int ret = ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &data);
    if (ret != 0) {
        std::cerr << "DmaBuf: allocate failed, size=" << size << " from heap: " << path
                  << " , ret=" << ret << std::endl;
        return nullptr;
    }

    using clock = std::chrono::steady_clock;
    static clock::time_point tp_start{};
    static unsigned call_count = 0;
    static const unsigned threshold = 100; // default threshold
    static std::mutex monitor_mutex;

    {
        std::lock_guard<std::mutex> lk(monitor_mutex);

        auto now = clock::now();
        if (tp_start.time_since_epoch().count() == 0) {
            // first time init
            tp_start = now;
            call_count = 1;
        } else {
            ++call_count;
        }

        auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - tp_start)
                .count();
        if (elapsed >= 1000) {
            if (call_count > threshold) {
                std::cerr << "DmaBuf: WARNING: allocation frequency " << call_count
                          << " in last " << elapsed << "ms exceeds threshold "
                          << threshold << std::endl;
            }
            // reset
            tp_start = clock::time_point{};
            call_count = 0;
        }
    }

    // std::cout << "in allocate, fd is " << data.fd << ", len is " << data.len
    //           << std::endl;

    return std::shared_ptr<DmaBuf>(new DmaBuf(data.fd, data.len));
}

std::shared_ptr<DmaBuf> DmaBuf::make(int fd, std::size_t size, void *addr) {
    if (fd < 0 || size == 0) {
        std::cerr << "DmaBuf: invalid fd or size, fd=" << fd << ", size=" << size
                  << std::endl;
        return nullptr;
    }

    return std::shared_ptr<DmaBuf>(new DmaBuf(fd, size, addr));
}

void *DmaBuf::map() {
    if (addr_ != nullptr) {
        return addr_;
    }
    if (fd_ < 0) {
        std::cerr << "DmaBuf: invalid fd" << fd_ << std::endl;
        return nullptr;
    }
    addr_ = mmap(NULL, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (addr_ == MAP_FAILED) {
        std::cerr << "DmaBuf: map failed, fd=" << fd_ << std::endl;
        return nullptr;
    }
    return addr_;
}

bool DmaBuf::unmap() {
    if (addr_ != nullptr) {
        if (munmap(addr_, size_) != 0) {
            std::cerr << "DmaBuf: unmap failed, addr=" << addr_ << ", size=" << size_
                      << std::endl;
            return false;
        }
        addr_ = nullptr;
    }

    return true;
}

// 把data所有数据拷贝到dmabuf
bool DmaBuf::copyFrom(const void *data, std::size_t size) {
    if (size > size_) {
        std::cerr << "DmaBuf: copyFrom size too large, size=" << size
                  << ", dmabuf size=" << size_ << std::endl;
        return false;
    }

    if (map() == nullptr) {
        return false;
    }

    std::memcpy(addr_, data, size);
    return true;
}

// 把dmabuf所有数据拷贝到data
bool DmaBuf::copyTo(void *data, std::size_t size) {
    if (size < size_) {
        std::cerr << "DmaBuf: copyTo size too small, size=" << size
                  << ", dmabuf size=" << size_ << std::endl;
        return false;
    }

    if (map() == nullptr) {
        return false;
    }

    std::memcpy(data, addr_, size_);
    return true;
}

/*-------------------------------------------------
 * cache 同步
 *------------------------------------------------*/
bool DmaBuf::sync(uint64_t syncFlags) const {
    struct dma_buf_sync sync;
    sync.flags = syncFlags;

    if (ioctl(fd_, DMA_BUF_IOCTL_SYNC, &sync) != 0) {
        std::cerr << "DmaBuf: sync failed, flags=" << syncFlags << std::endl;
        return false;
    }

    return true;
}

bool DmaBuf::syncPartial(uint32_t offset, uint32_t len, uint64_t syncFlags) const {
    struct dma_buf_sync_partial sync_p;

    sync_p.flags = syncFlags;
    sync_p.offset = offset;
    sync_p.len = len;

    if (ioctl(fd_, DMA_BUF_IOCTL_SYNC_PARTIAL, &sync_p) != 0) {
        std::cerr << "DmaBuf: sync partial failed, flags=" << syncFlags
                  << ", offset=" << offset << ", len=" << len << std::endl;
        return false;
    }

    return true;
}

bool Image::create(int width, int height, uint32_t format, int stride,
                   struct Rect roi) {
    if (roi.x < 0 || roi.y < 0 || roi.w < 0 || roi.h < 0 || roi.x + roi.w > width ||
        roi.y + roi.h > height) {
        std::cerr << "Image::create: invalid roi" << std::endl;
        return false;
    }

    if (stride == 0)
        stride = (float)width * rkclr_get_pixel_bytes(format);

    size_t size = stride * height;
    auto buf = DmaBuf::allocate(size);
    if (buf == nullptr) {
        std::cerr << "Image::create: allocate dmabuf failed, size=" << size
                  << std::endl;
        return false;
    }

    this->width = width;
    this->height = height;
    this->format = format;
    this->stride = stride;
    this->dmabuf = buf;
    this->roi = roi;

    return true;
}

} // namespace rkros_dmabuf
