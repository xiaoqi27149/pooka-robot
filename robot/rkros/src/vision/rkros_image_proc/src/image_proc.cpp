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

#include <cstdint>
#include <cstring>
#include <iostream>
#include <numeric>

#include "rkros_image_proc/image_proc.hpp"
#include "sensor_msgs/image_encodings.hpp"

namespace rkros_image_proc {

static int rga_byte_stride = 16;

static inline int align_up(int value, int align) {
    return ((value) + (align)-1) & ~((align)-1);
}

static inline int align_down(int value, int align) {
    return (value) & ~((align)-1);
}

static void rga_init(void) {
    static bool rga_inited = false;

    if (rga_inited)
        return;

    rga_inited = true;
    std::cout << "=== RGA Information ===" << std::endl;
    std::cout << querystring(RGA_ALL) << std::endl;

    int rga_stride;
    const char *str = querystring(RGA_BYTE_STRIDE);
    if (sscanf(str, "%*[^:]: %d", &rga_stride) == 1) {
        rga_byte_stride = rga_stride;
    }

    return;
}

int getStridePixel(int width, uint32_t rkclr_fmt) {
    int format = rkclr_to_rga_format(rkclr_fmt);
    if (format == RK_FORMAT_UNKNOWN) {
        return 0;
    }

    rga_init();

    /* align pixel = lcm(bpp，byte_stride * 8) / pixel_stride */
    int pixel_stride = get_perPixel_stride_from_format(format);
    int align_pixel = std::lcm(pixel_stride, rga_byte_stride * 8) / pixel_stride;

    return align_up(width, align_pixel);
}

int getStrideBytes(int width, uint32_t rkclr_fmt) {
    return getStridePixel(width, rkclr_fmt) * rkclr_get_pixel_bytes(rkclr_fmt);
}

static void rga_handle_release(rga_buffer_handle_t handle) {
    if (handle) {
        releasebuffer_handle(handle);
    }
}

static bool setup_rga_buffer(rga_buffer_t &rga_buf, const rkros_dmabuf::Image &img) {
    rga_buffer_handle_t rga_handle;
    int format = rkclr_to_rga_format(img.format);
    if (format == RK_FORMAT_UNKNOWN) {
        std::cerr << "unsupport image format: " << img.format << std::endl;
        return false;
    }

    rga_handle = img.dmabuf->getRgaHandle();
    if (rga_handle == 0) {
        rga_handle = importbuffer_fd(img.dmabuf->fd(), img.dmabuf->size());

        if (rga_handle == 0) {
            std::cerr << "importbuffer failed!" << std::endl;
            return false;
        }
        img.dmabuf->setRgaHandle(rga_handle, rga_handle_release);
    }

    int stride_pixels = getStridePixel(img.width, img.format);
    rga_buf = wrapbuffer_handle(rga_handle, img.width, img.height, format,
                                stride_pixels, img.height);

    return true;
}

static bool rga_process(rga_buffer_t &src_img, rga_buffer_t &dst_img, im_rect &srect,
                        im_rect &drect, im_opt_t opt = {}, int usage = 0) {
    int ret = 0;
    rga_buffer_t pat_img{};
    im_rect pat_rect{};

    // sync process
    usage |= IM_SYNC;

    // TODO: 在非rk3588平台上使用时候，这里面需要做进一步判断处理
    // only use rga3, it is support IOMMU(40bits)
    opt.core = IM_SCHEDULER_RGA3_CORE0 | IM_SCHEDULER_RGA3_CORE1;

    // std::cout << "src: " << srect << std::endl;
    // std::cout << "dst: " << drect << std::endl;
    ret = imcheck_t(src_img, dst_img, pat_img, srect, drect, pat_rect, usage);
    if (ret == IM_STATUS_NOERROR) {
        ret = improcess(src_img, dst_img, pat_img, srect, drect, pat_rect, -1, NULL,
                        &opt, usage);
        if (ret != IM_STATUS_SUCCESS) {
            std::cerr << "rga process failed! " << imStrError((IM_STATUS)ret)
                      << std::endl;
        }
    } else {
        std::cerr << "rga check error! " << imStrError((IM_STATUS)ret) << std::endl;
    }

    return ret == IM_STATUS_SUCCESS;
}

// 判断一个rect是否是图像的子区域
static inline bool isSubRect(const rkros_dmabuf::Image &img) {
    if (img.roi.x < 0 || img.roi.y < 0 || img.roi.w <= 0 || img.roi.h <= 0 ||
        img.roi.x + img.roi.w > img.width || img.roi.y + img.roi.h > img.height) {
        return false;
    }

    return true;
}

// 计算指定区域在图像内存中的偏移和长度(包含padding)
static inline void getOffsetAndLen(const rkros_dmabuf::Image &img, uint32_t &offset,
                                   uint32_t &len) {
    uint32_t bpp = rkclr_get_pixel_bits(img.format);
    offset = img.roi.y * img.stride + (img.roi.x * bpp) / 8;
    len = (img.roi.h - 1) * img.stride + (img.roi.w * bpp) / 8;
}

static inline bool deviceAccessStart(const rkros_dmabuf::Image &img,
                                     uint32_t offset = 0, uint32_t len = 0,
                                     uint64_t syncFlags = DMA_BUF_SYNC_RW) {
    return (len == 0) ? img.dmabuf->syncEnd(syncFlags)
                      : img.dmabuf->syncPartialEnd(offset, len, syncFlags);
}

static inline bool deviceAccessEnd(const rkros_dmabuf::Image &img, uint32_t offset = 0,
                                   uint32_t len = 0,
                                   uint64_t syncFlags = DMA_BUF_SYNC_RW) {
    return (len == 0) ? img.dmabuf->syncStart(syncFlags)
                      : img.dmabuf->syncPartialStart(offset, len, syncFlags);
}

bool process(const rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, bool autoSync,
             im_opt_t opt, int usage) {
    rga_buffer_t src_img, dst_img;
    if (src.dmabuf == nullptr && dst.dmabuf == nullptr) {
        std::cerr << "dmabuf is null" << std::endl;
        return false;
    }

    if (!setup_rga_buffer(src_img, src)) {
        std::cerr << "setup src rga buffer failed" << std::endl;
        return false;
    }

    if (!setup_rga_buffer(dst_img, dst)) {
        std::cerr << "setup dst rga buffer failed" << std::endl;
        return false;
    }

    uint32_t s_offset = 0, d_offset = 0;
    uint32_t s_len = 0, d_len = 0;
    if (autoSync) {
        if (!rkclr_is_yuv_p_sp(src.format) && isSubRect(src)) {
            getOffsetAndLen(src, s_offset, s_len);
        }
        deviceAccessStart(src, s_offset, s_len);

        if (!rkclr_is_yuv_p_sp(dst.format) && isSubRect(dst)) {
            getOffsetAndLen(dst, d_offset, d_len);
        }
        deviceAccessStart(dst, d_offset, d_len);
    }

    im_rect srect{src.roi.x, src.roi.y, src.roi.w, src.roi.h};
    im_rect drect{dst.roi.x, dst.roi.y, dst.roi.w, dst.roi.h};
    bool ret = rga_process(src_img, dst_img, srect, drect, opt, usage);

    if (autoSync) {
        deviceAccessEnd(src, s_offset, s_len);
        deviceAccessEnd(dst, d_offset, d_len);
    }

    return ret;
}

bool resize(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, int interp,
            bool autoSync) {
    if (src.roi.w == 0 || src.roi.h == 0) {
        src.roi = {0, 0, src.width, src.height};
    }
    if (dst.roi.w == 0 || dst.roi.h == 0) {
        dst.roi = {0, 0, dst.width, dst.height};
    }

    im_opt_t opt = {};

    opt.version = RGA_CURRENT_API_VERSION;
    opt.interp = interp;

    return process(src, dst, autoSync, opt);
}

bool copy(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, bool autoSync) {
    if (src.roi.w == 0 || src.roi.h == 0) {
        src.roi = {0, 0, src.width, src.height};
    }
    if (dst.roi.w == 0 || dst.roi.h == 0) {
        dst.roi = {0, 0, dst.width, dst.height};
    }
    if (src.roi.w != dst.roi.w || src.roi.h != dst.roi.h) {
        return false;
    }
    return process(src, dst, autoSync);
}

bool crop(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, bool autoSync) {
    if (src.roi.w == 0 || src.roi.h == 0) {
        src.roi = {0, 0, src.width, src.height};
    }
    dst.roi = {0, 0, src.roi.w, src.roi.h};
    if (dst.roi.w > dst.width || dst.roi.h > dst.height) {
        return false;
    }

    return process(src, dst, autoSync);
}

bool rotate(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, int rotate_mode,
            bool autoSync) {
    if (src.roi.w == 0 || src.roi.h == 0) {
        src.roi = {0, 0, src.width, src.height};
    }
    if (dst.roi.w == 0 || dst.roi.h == 0) {
        dst.roi = {0, 0, dst.width, dst.height};
    }
    if ((rotate_mode == ROTATION_90 || rotate_mode == ROTATION_270) &&
        (src.roi.w != dst.roi.h || src.roi.h != dst.roi.w)) {
        return false;
    }
    if ((rotate_mode == ROTATION_180) &&
        (src.roi.w != dst.roi.w || src.roi.h != dst.roi.h)) {
        return false;
    }

    return process(src, dst, autoSync, {}, rotate_mode);
}

bool flip(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, int flip_code,
          bool autoSync) {
    if (src.roi.w == 0 || src.roi.h == 0) {
        src.roi = {0, 0, src.width, src.height};
    }
    if (dst.roi.w == 0 || dst.roi.h == 0) {
        dst.roi = {0, 0, dst.width, dst.height};
    }
    if (src.roi.w != dst.roi.w || src.roi.h != dst.roi.h) {
        return false;
    }
    return process(src, dst, autoSync, {}, flip_code);
}

bool cvtColor(rkros_dmabuf::Image &src, rkros_dmabuf::Image &dst, bool autoSync) {
    if (src.roi.w == 0 || src.roi.h == 0) {
        src.roi = {0, 0, src.width, src.height};
    }
    if (dst.roi.w == 0 || dst.roi.h == 0) {
        dst.roi = {0, 0, dst.width, dst.height};
    }
    if (src.roi.w != dst.roi.w || src.roi.h != dst.roi.h) {
        return false;
    }
    return process(src, dst, autoSync);
}

} // namespace rkros_image_proc
