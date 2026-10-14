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

#include "rkros_common/rk_color.h"
#include "rga/rga.h"

int rkclr_to_rga_format(uint32_t cf) {
    // Map RKCLR formats (rk_color.h) to RGA formats (rga.h)
    // Return RK_FORMAT_UNKNOWN when no matching format is found.
    switch (cf) {
    // RGB
    case RKCLR_RGBA_8888:
        return RK_FORMAT_RGBA_8888;
    case RKCLR_BGRA_8888:
        return RK_FORMAT_BGRA_8888;
    case RKCLR_ARGB_8888:
        return RK_FORMAT_ARGB_8888;
    case RKCLR_ABGR_8888:
        return RK_FORMAT_ABGR_8888;
    case RKCLR_RGBX_8888:
        return RK_FORMAT_RGBX_8888;
    case RKCLR_BGRX_8888:
        return RK_FORMAT_BGRX_8888;
    case RKCLR_XRGB_8888:
        return RK_FORMAT_XRGB_8888;
    case RKCLR_XBGR_8888:
        return RK_FORMAT_XBGR_8888;
    case RKCLR_RGB_888:
        return RK_FORMAT_RGB_888;
    case RKCLR_BGR_888:
        return RK_FORMAT_BGR_888;
    case RKCLR_RGB_565:
        return RK_FORMAT_RGB_565;
    case RKCLR_BGR_565:
        return RK_FORMAT_BGR_565;
    case RKCLR_RGBA_5551:
        return RK_FORMAT_RGBA_5551;
    case RKCLR_RGBA_4444:
        return RK_FORMAT_RGBA_4444;
    case RKCLR_BGRA_5551:
        return RK_FORMAT_BGRA_5551;
    case RKCLR_BGRA_4444:
        return RK_FORMAT_BGRA_4444;
    case RKCLR_ARGB_5551:
        return RK_FORMAT_ARGB_5551;
    case RKCLR_ARGB_4444:
        return RK_FORMAT_ARGB_4444;
    case RKCLR_ABGR_5551:
        return RK_FORMAT_ABGR_5551;
    case RKCLR_ABGR_4444:
        return RK_FORMAT_ABGR_4444;

    case RKCLR_RGBA2BPP:
        return RK_FORMAT_RGBA2BPP;
    case RKCLR_A8:
        return RK_FORMAT_A8;

    // Gray
    case RKCLR_Y4:
        return RK_FORMAT_Y4;

    case RKCLR_YUV400: // Y8
        return RK_FORMAT_YCbCr_400;

    // YUV 4:2:0
    case RKCLR_YUV420SP: // NV12
        return RK_FORMAT_YCbCr_420_SP;
    case RKCLR_YVU420SP: // NV21
        return RK_FORMAT_YCrCb_420_SP;
    case RKCLR_YUV420P: // I420
        return RK_FORMAT_YCbCr_420_P;
    case RKCLR_YVU420P:
        return RK_FORMAT_YCrCb_420_P;
    case RKCLR_YUV420SP_10B:
        return RK_FORMAT_YCbCr_420_SP_10B;
    case RKCLR_YVU420SP_10B:
        return RK_FORMAT_YCrCb_420_SP_10B;

    // YUV 4:2:2 packed
    case RKCLR_YUYV422:
        return RK_FORMAT_YUYV_422;
    case RKCLR_YVYU422:
        return RK_FORMAT_YVYU_422;
    case RKCLR_UYVY422:
        return RK_FORMAT_UYVY_422;
    case RKCLR_VYUY422:
        return RK_FORMAT_VYUY_422;

    // YUV 4:2:2 semi-planar/planar
    case RKCLR_YUV422SP:
        return RK_FORMAT_YCbCr_422_SP;
    case RKCLR_YVU422SP:
        return RK_FORMAT_YCrCb_422_SP;
    case RKCLR_YUV422P:
        return RK_FORMAT_YCbCr_422_P;
    case RKCLR_YVU422P:
        return RK_FORMAT_YCrCb_422_P;
    case RKCLR_YUV422SP_10B:
        return RK_FORMAT_YCbCr_422_SP_10B;
    case RKCLR_YVU422SP_10B:
        return RK_FORMAT_YCrCb_422_SP_10B;

    // YUV 4:4:4
    case RKCLR_YUV444SP:
        return RK_FORMAT_YCbCr_444_SP;
    case RKCLR_YVU444SP:
        return RK_FORMAT_YCrCb_444_SP;

    // Compressed
    case RKCLR_AVC:
    case RKCLR_HEVC:
    case RKCLR_JPEG:

    default:
        return RK_FORMAT_UNKNOWN;
    }
}

const char *rkclr_to_string(uint32_t cf) {
    switch (cf) {
    case RKCLR_UNDEF:
        return "UNDEFINED";
    case RKCLR_YUV400:
        return "RKCLR_YUV400";
    case RKCLR_Y4:
        return "RKCLR_Y4";
    case RKCLR_YUV420P:
        return "RKCLR_YUV420P";
    case RKCLR_YVU420P:
        return "RKCLR_YVU420P";
    case RKCLR_YUV420SP:
        return "RKCLR_YUV420SP";
    case RKCLR_YVU420SP:
        return "RKCLR_YVU420SP";
    case RKCLR_YUV420SP_10B:
        return "RKCLR_YUV420SP_10B";
    case RKCLR_YVU420SP_10B:
        return "RKCLR_YVU420SP_10B";
    case RKCLR_YUYV422:
        return "RKCLR_YUYV422";
    case RKCLR_YVYU422:
        return "RKCLR_YVYU422";
    case RKCLR_UYVY422:
        return "RKCLR_UYVY422";
    case RKCLR_VYUY422:
        return "RKCLR_VYUY422";
    case RKCLR_YUV422P:
        return "RKCLR_YUV422P";
    case RKCLR_YVU422P:
        return "RKCLR_YVU422P";
    case RKCLR_YUV422SP:
        return "RKCLR_YUV422SP";
    case RKCLR_YVU422SP:
        return "RKCLR_YVU422SP";
    case RKCLR_YUV422SP_10B:
        return "RKCLR_YUV422SP_10B";
    case RKCLR_YVU422SP_10B:
        return "RKCLR_YVU422SP_10B";
    case RKCLR_YUV444SP:
        return "RKCLR_YUV444SP";
    case RKCLR_YVU444SP:
        return "RKCLR_YVU444SP";
    case RKCLR_RGBA_8888:
        return "RKCLR_RGBA_8888";
    case RKCLR_BGRA_8888:
        return "RKCLR_BGRA_8888";
    case RKCLR_ARGB_8888:
        return "RKCLR_ARGB_8888";
    case RKCLR_ABGR_8888:
        return "RKCLR_ABGR_8888";
    case RKCLR_RGBX_8888:
        return "RKCLR_RGBX_8888";
    case RKCLR_BGRX_8888:
        return "RKCLR_BGRX_8888";
    case RKCLR_XRGB_8888:
        return "RKCLR_XRGB_8888";
    case RKCLR_XBGR_8888:
        return "RKCLR_XBGR_8888";
    case RKCLR_RGB_888:
        return "RKCLR_RGB_888";
    case RKCLR_BGR_888:
        return "RKCLR_BGR_888";
    case RKCLR_RGB_565:
        return "RKCLR_RGB_565";
    case RKCLR_BGR_565:
        return "RKCLR_BGR_565";
    case RKCLR_RGBA_5551:
        return "RKCLR_RGBA_5551";
    case RKCLR_RGBA_4444:
        return "RKCLR_RGBA_4444";
    case RKCLR_BGRA_5551:
        return "RKCLR_BGRA_5551";
    case RKCLR_BGRA_4444:
        return "RKCLR_BGRA_4444";
    case RKCLR_ARGB_5551:
        return "RKCLR_ARGB_5551";
    case RKCLR_ARGB_4444:
        return "RKCLR_ARGB_4444";
    case RKCLR_ABGR_5551:
        return "RKCLR_ABGR_5551";
    case RKCLR_ABGR_4444:
        return "RKCLR_ABGR_4444";
    case RKCLR_RGBA2BPP:
        return "RKCLR_RGBA2BPP";
    case RKCLR_A8:
        return "RKCLR_A8";
    case RKCLR_AVC:
        return "RKCLR_AVC";
    case RKCLR_HEVC:
        return "RKCLR_HEVC";
    case RKCLR_JPEG:
        return "RKCLR_JPEG";
    default:
        return "UNKNOWN";
    }
}
