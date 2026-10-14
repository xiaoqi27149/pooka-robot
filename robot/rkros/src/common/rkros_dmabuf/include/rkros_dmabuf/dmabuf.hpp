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

#ifndef RKROS_DMABUF__DMABUF_HPP_
#define RKROS_DMABUF__DMABUF_HPP_

#include <functional>
#include <linux/dma-buf.h>
#include <memory>
#include <string>
#include <sys/types.h>

namespace rkros_dmabuf {

class DmaBuf {
  public:
    // 禁止拷贝
    DmaBuf(const DmaBuf &) = delete;
    DmaBuf &operator=(const DmaBuf &) = delete;

    ~DmaBuf();

    /**
     * 分配一个新的dmabuf
     * @param size 申请的内存大小，单位字节
     * @param heapName 使用的heap名称，默认system
     * @param heapFlags 传给dma_heap_alloc_flags的flags，默认0
     * @return 成功返回DmaBuf对象，失败返回nullptr
     */
    static std::shared_ptr<DmaBuf> allocate(std::size_t size,
                                            const std::string &heapName = "system",
                                            uint32_t heapFlags = 0);

    /**
     * 封装已有的dmabuf
     * @param fd dmabuf的文件描述符
     * @param size dmabuf的大小，单位字节
     * @param addr 如果已经mmap过，可以传入addr，避免重复mmap
     * @return 成功返回DmaBuf对象，失败返回nullptr
     */
    static std::shared_ptr<DmaBuf> make(int fd, std::size_t size, void *addr = nullptr);

    int fd() const noexcept {
        return fd_;
    }
    std::size_t size() const noexcept {
        return size_;
    }
    void *addr() const noexcept {
        return addr_;
    }

    void *map();
    bool unmap();
    bool isMapped() const noexcept {
        return addr_ != nullptr;
    }

    /**
     * 把data所有数据拷贝到dmabuf
     * @param data 源数据指针
     * @param size 拷贝的数据大小，单位字节，不能超过dmabuf的size
     * @return 成功返回true，失败返回false
     */
    bool copyFrom(const void *data, std::size_t size);

    /**
     * 把dmabuf的数据拷贝到data
     * @param data 目标数据指针
     * @param size 拷贝的数据大小，单位字节，不能超过dmabuf的size
     * @return 成功返回true，失败返回false
     */
    bool copyTo(void *data, std::size_t size);

    void setAutoClose(bool on) noexcept {
        autoClose_ = on;
    }

    /**
     * 对dmabuf的整个区域进行同步cache操作
     * @param syncFlags 同步操作标志，参考linux/dma-buf.h
     * @return 成功返回true，失败返回false
     */
    inline bool syncStart(uint64_t syncFlags = DMA_BUF_SYNC_RW) const {
        return sync(syncFlags | DMA_BUF_SYNC_START);
    };
    inline bool syncEnd(uint64_t syncFlags = DMA_BUF_SYNC_RW) const {
        return sync(syncFlags | DMA_BUF_SYNC_END);
    };

    /**
     * 对dmabuf的局部区域进行同步cache操作
     * @param offset 偏移位置，单位字节
     * @param len 同步长度，单位字节
     * @param syncFlags 同步操作标志，参考linux/dma-buf.h
     * @return 成功返回true，失败返回false
     */
    inline bool syncPartialStart(uint32_t offset = 0, uint32_t len = 0,
                                 uint64_t syncFlags = DMA_BUF_SYNC_RW) const {
        return (len == 0) ? syncStart(syncFlags)
                          : syncPartial(offset, len, syncFlags | DMA_BUF_SYNC_START);
    };
    inline bool syncPartialEnd(uint32_t offset = 0, uint32_t len = 0,
                               uint64_t syncFlags = DMA_BUF_SYNC_RW) const {
        return (len == 0) ? syncEnd(syncFlags)
                          : syncPartial(offset, len, syncFlags | DMA_BUF_SYNC_END);
    };

    /**
     * 记录该dmabuf对应的RGA handle及其释放回调
     * @param rgaHandle RGA的handle
     * @param releaseCB 释放回调函数
     * Note: RGA的handle只能由RGA本身释放，不能在DmaBuf析构时释放
     * 因此需要传入一个释放回调函数
     */
    inline void setRgaHandle(uint32_t rgaHandle,
                             std::function<void(uint32_t)> releaseCB) {
        rgaHandle_ = rgaHandle;
        rgaHandleReleaseCB_ = releaseCB;
    }
    inline uint32_t getRgaHandle(void) {
        return rgaHandle_;
    }

    /**
     * 记录该dmabuf对应的opencl cl_mem及其释放回调
     * @param oclHandle cl_mem对象指针
     * @param releaseCB 释放回调函数
     */
    inline void setOclHandle(void *oclHandle, std::function<void(void *)> releaseCB) {
        oclHandle_ = oclHandle;
        oclHandleReleaseCB_ = releaseCB;
    }

    inline void *getOclHandle(void) {
        return oclHandle_;
    }

  private:
    explicit DmaBuf(int fd, std::size_t size, void *addr = nullptr)
        : fd_(fd), size_(size), addr_(addr) {};

    bool sync(uint64_t syncFlags) const;
    bool syncPartial(uint32_t offset, uint32_t len, uint64_t syncFlags) const;

    int fd_{-1};
    std::size_t size_{0};
    void *addr_{nullptr};
    bool autoClose_{true};

    // rga handle及释放回调
    uint32_t rgaHandle_{0};
    std::function<void(uint32_t)> rgaHandleReleaseCB_{nullptr};

    // cl_mem及释放回调
    void *oclHandle_{nullptr};
    std::function<void(void *)> oclHandleReleaseCB_{nullptr};
};

struct Image {
    int32_t width{0};
    int32_t height{0};
    /**
     * stride是图像中每行占用的字节数
     * 对于rgb格式，其stride=(width+padding)*3
     * 对于yuv系列格式，把uv平摊到每个像素来算。比如yuv420，其stride=(width+padding)*1.5
     */
    uint32_t stride{0};
    uint32_t format{0};
    std::shared_ptr<DmaBuf> dmabuf{nullptr};

    struct Rect {
        int32_t x{0};
        int32_t y{0};
        int32_t w{0};
        int32_t h{0};
    } roi;

    bool create(int width, int height, uint32_t format, int stride = 0,
                struct Rect roi = {0, 0, 0, 0});
};

} // namespace rkros_dmabuf

#endif // RKROS_DMABUF__DMABUF_HPP_
