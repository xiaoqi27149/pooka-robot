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
#ifndef _RK_COLOR_H_
#define _RK_COLOR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 定义颜色格式的值
 *  | Reserve(2) | CM(3) | CO(5) | CN(2) | C4(5) | C3(5) | C2(5) | C1(5) |
    CM: Color Model,    3 bits valid: 0~7
    CO: Channel Order,  5 bits valid: 0~31
    CN: Channel Number, 2 bits valid: 0~3 (1~4), Max 4 channels
    CB: Channel n Bits, 5 bits each, valid: 0~31 (1~32), total 5*4 = 20 bits
    Reserve: 2 bits
 */

#define RKCLR_VALUE(cm, co, cn, cb4, cb3, cb2, cb1)                                    \
    ((cm) << 27 | (co) << 22 | ((cn) - 1) << 20 | (cb4) << 15 | (cb3) << 10 |          \
     (cb2) << 5 | (cb1))

#define RKCLR_VALUE_CH1(cm, co, cb1) RKCLR_VALUE(cm, co, 1, 0, 0, 0, (cb1) - 1)
#define RKCLR_VALUE_CH2(cm, co, cb2, cb1)                                              \
    RKCLR_VALUE(cm, co, 2, 0, 0, (cb2) - 1, (cb1) - 1)

#define RKCLR_VALUE_CH3(cm, co, cb3, cb2, cb1)                                         \
    RKCLR_VALUE(cm, co, 3, 0, (cb3) - 1, (cb2) - 1, (cb1) - 1)

#define RKCLR_VALUE_CH4(cm, co, cb4, cb3, cb2, cb1)                                    \
    RKCLR_VALUE(cm, co, 4, (cb4) - 1, (cb3) - 1, (cb2) - 1, (cb1) - 1)

#define RKCLR_GET_CLR_MODEL(value) (((value) >> 27) & 0x7)
#define RKCLR_GET_CH_ORDER(value) (((value) >> 22) & 0x1F)
#define RKCLR_GET_CH_NUM(value) ((((value) >> 20) & 0x3) + 1)
#define RKCLR_GET_CH_BIT(value, n) ((((value) >> (((n) - 1) * 5)) & 0x1F) + 1)

// Color Model (3 bits, max 7)
enum {
    RKCLR_CM_RGB = 0,
    RKCLR_CM_YUV = 1,
    RKCLR_CM_COMP = 2, // compressed format, like: h264, jpeg, etc.
    RKCLR_CM_OTHERS,   // like: CMYK, HSV, Lab, etc.
};

// RGB Channel Order
enum {
    // channel: [R=3 G=2 B=1], memory: [RR GG BB] == 0xBBGGRR
    RKCLR_CO_RGB = 0 << 1,
    RKCLR_CO_BGR = 1 << 1,

    // channel: [R=4 G=3 B=2 X=1], memory: [RR GG BB XX] == 0xXXBBGGRR
    RKCLR_CO_RGBX = 2 << 1,
    RKCLR_CO_BGRX = 3 << 1,
    RKCLR_CO_XRGB = 4 << 1,
    RKCLR_CO_XBGR = 5 << 1,

    // RGB with Alpha
    // channel: [R=4 G=3 B=2 A=1], memory: [RR GG BB AA] == 0xAABBGGRR
    RKCLR_CO_A = 1,
    RKCLR_CO_RGBA = RKCLR_CO_RGBX | 1,
    RKCLR_CO_BGRA = RKCLR_CO_BGRX | 1,
    RKCLR_CO_ARGB = RKCLR_CO_XRGB | 1,
    RKCLR_CO_ABGR = RKCLR_CO_XBGR | 1,

    RKCLR_CO_BUTT = 0x1F // 它是所有Channel Order的最大值
};

// YUV Channel Order
enum {
    RKCLR_CO_LUMA = 0,

    RKCLR_CO_YUV,    // [Y0 U0 V0] [Y1 U1 V1] ...
    RKCLR_CO_YUYV,   // [Y0 U0 Y1 V0] [Y2 U2 Y3 V2] ...
    RKCLR_CO_YVYU,   // [Y0 V0 Y1 U0] [Y2 V2 Y3 U2] ...
    RKCLR_CO_UYVY,   // [U0 Y0 V0 Y1] [U2 Y2 V2 Y3] ...
    RKCLR_CO_VYUY,   // [V0 Y0 U0 Y1] [V2 Y2 U2 Y3] ...
    RKCLR_CO_YUV_P,  // Y... + U... + V...
    RKCLR_CO_YVU_P,  // Y... + V... + U...
    RKCLR_CO_YUV_SP, // Y... + UV...
    RKCLR_CO_YVU_SP, // Y... + VU...
};

// Compressed format (Channel Order)
enum {
    RKCLR_CO_AVC = 0,
    RKCLR_CO_HEVC,
    RKCLR_CO_JPEG,
};

enum {
    RKCLR_UNDEF = 0,

    // RGB 24
    RKCLR_RGB_888 = RKCLR_VALUE_CH3(RKCLR_CM_RGB, RKCLR_CO_RGB, 8, 8, 8),
    RKCLR_BGR_888 = RKCLR_VALUE_CH3(RKCLR_CM_RGB, RKCLR_CO_BGR, 8, 8, 8),

    // RGB 32
    RKCLR_RGBA_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_RGBA, 8, 8, 8, 8),
    RKCLR_BGRA_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_BGRA, 8, 8, 8, 8),
    RKCLR_ARGB_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_ARGB, 8, 8, 8, 8),
    RKCLR_ABGR_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_ABGR, 8, 8, 8, 8),
    RKCLR_RGBX_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_RGBX, 8, 8, 8, 8),
    RKCLR_BGRX_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_BGRX, 8, 8, 8, 8),
    RKCLR_XRGB_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_XRGB, 8, 8, 8, 8),
    RKCLR_XBGR_8888 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_XBGR, 8, 8, 8, 8),

    // RGB 16
    RKCLR_RGB_565 = RKCLR_VALUE_CH3(RKCLR_CM_RGB, RKCLR_CO_RGB, 5, 6, 5),
    RKCLR_BGR_565 = RKCLR_VALUE_CH3(RKCLR_CM_RGB, RKCLR_CO_BGR, 5, 6, 5),
    RKCLR_RGBA_5551 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_RGBA, 5, 5, 5, 1),
    RKCLR_RGBA_4444 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_RGBA, 4, 4, 4, 4),
    RKCLR_BGRA_5551 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_BGRA, 5, 5, 5, 1),
    RKCLR_BGRA_4444 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_BGRA, 4, 4, 4, 4),
    RKCLR_ARGB_5551 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_ARGB, 5, 5, 5, 1),
    RKCLR_ARGB_4444 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_ARGB, 4, 4, 4, 4),
    RKCLR_ABGR_5551 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_ABGR, 5, 5, 5, 1),
    RKCLR_ABGR_4444 = RKCLR_VALUE_CH4(RKCLR_CM_RGB, RKCLR_CO_ABGR, 4, 4, 4, 4),

    RKCLR_RGBA2BPP = RKCLR_VALUE_CH2(RKCLR_CM_RGB, RKCLR_CO_RGBA, 1, 1),

    /* Single channel alpha */
    RKCLR_A8 = RKCLR_VALUE_CH1(RKCLR_CM_RGB, RKCLR_CO_A, 8),

    /* Y only 8-bit */
    RKCLR_YUV400 = RKCLR_VALUE_CH1(RKCLR_CM_YUV, RKCLR_CO_LUMA, 8),

    /* 4-bit Y (luma) */
    RKCLR_Y4 = RKCLR_VALUE_CH1(RKCLR_CM_YUV, RKCLR_CO_LUMA, 4),

    /* YUV 4:2:0 */

    // planar
    RKCLR_YUV420P = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUV_P, 8, 2, 2),
    RKCLR_YVU420P = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVU_P, 8, 2, 2),

    // semi-planar
    RKCLR_YUV420SP = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUV_SP, 8, 2, 2),
    RKCLR_YVU420SP = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVU_SP, 8, 2, 2),
    RKCLR_YUV420SP_10B = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUV_SP, 10, 3, 3),
    RKCLR_YVU420SP_10B = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVU_SP, 10, 3, 3),

    /* YUV 4:2:2 */

    // packed
    RKCLR_YUYV422 = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUYV, 8, 4, 4),
    RKCLR_YVYU422 = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVYU, 8, 4, 4),
    RKCLR_UYVY422 = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_UYVY, 8, 4, 4),
    RKCLR_VYUY422 = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_VYUY, 8, 4, 4),

    // planar
    RKCLR_YUV422P = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUV_P, 8, 4, 4),
    RKCLR_YVU422P = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVU_P, 8, 4, 4),

    // semi-planar
    RKCLR_YUV422SP = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUV_SP, 8, 4, 4),
    RKCLR_YVU422SP = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVU_SP, 8, 4, 4),
    RKCLR_YUV422SP_10B = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUV_SP, 10, 5, 5),
    RKCLR_YVU422SP_10B = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVU_SP, 10, 5, 5),

    /* YUV 4:4:4 (sp and planar) */
    RKCLR_YUV444SP = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YUV_SP, 8, 8, 8),
    RKCLR_YVU444SP = RKCLR_VALUE_CH3(RKCLR_CM_YUV, RKCLR_CO_YVU_SP, 8, 8, 8),

    /* alias */
    RKCLR_I420 = RKCLR_YUV420P,
    RKCLR_NV12 = RKCLR_YUV420SP,
    RKCLR_NV21 = RKCLR_YVU420SP,
    RKCLR_NV16 = RKCLR_YUV422SP,
    RKCLR_NV61 = RKCLR_YVU422SP,
    RKCLR_Y8 = RKCLR_YUV400,

    /* compressed format */
    RKCLR_AVC = RKCLR_VALUE_CH1(RKCLR_CM_COMP, RKCLR_CO_AVC, 1),
    RKCLR_HEVC = RKCLR_VALUE_CH1(RKCLR_CM_COMP, RKCLR_CO_HEVC, 1),
    RKCLR_JPEG = RKCLR_VALUE_CH1(RKCLR_CM_COMP, RKCLR_CO_JPEG, 1),

    RKCLR_BUTT = 0xFFFFFFFF
};

/**
 * User interface
 */

// 获取每个像素占用的比特数
static inline int rkclr_get_pixel_bits(uint32_t cf) {
    return (cf & 0x1F) + ((cf >> 5) & 0x1F) + ((cf >> 10) & 0x1F) +
           ((cf >> 15) & 0x1F) + RKCLR_GET_CH_NUM(cf);
}

// 获取每个像素占用的字节数
static inline float rkclr_get_pixel_bytes(uint32_t cf) {
    return rkclr_get_pixel_bits(cf) / 8.0f;
}

// 获取通道数： 1~4
static inline int rkclr_get_ch_num(uint32_t cf) {
    return RKCLR_GET_CH_NUM(cf);
}

// 获取第n个通道的位数： 1~32, ch=1~4
static inline int rkclr_get_ch_bit(uint32_t cf, int ch) {
    if (ch < 1 || ch > 4) {
        return -1;
    }
    return RKCLR_GET_CH_BIT(cf, ch);
}

// 判断是否为yuv系列格式
static inline bool rkclr_is_yuv(uint32_t cf) {
    return cf && (RKCLR_GET_CLR_MODEL(cf) == RKCLR_CM_YUV);
}

// 判断是否为yuv的P或者SP系列格式
static inline bool rkclr_is_yuv_p_sp(uint32_t cf) {
    int order = RKCLR_GET_CH_ORDER(cf);
    return rkclr_is_yuv(cf) &&
           ((order == RKCLR_CO_YUV_P) || (order == RKCLR_CO_YUV_SP) ||
            (order == RKCLR_CO_YVU_P) || (order == RKCLR_CO_YVU_SP));
}

// 判断是否为rgb系列格式
static inline bool rkclr_is_rgb(uint32_t cf) {
    return cf && (RKCLR_GET_CLR_MODEL(cf) == RKCLR_CM_RGB);
}

// 判断是否为灰度格式
static inline bool rkclr_is_gray(uint32_t cf) {
    return rkclr_is_yuv(cf) && (RKCLR_GET_CH_ORDER(cf) == RKCLR_CO_LUMA);
}

// 判断是否为压缩格式
static inline bool rkclr_is_compressed(uint32_t cf) {
    return cf && (RKCLR_GET_CLR_MODEL(cf) == RKCLR_CM_COMP);
}

// 判断是否有alpha通道
static inline bool rkclr_with_alpha(uint32_t cf) {
    return (rkclr_is_rgb(cf) && (RKCLR_GET_CH_ORDER(cf) & 0x1));
}

// rk color 转成 rga format
int rkclr_to_rga_format(uint32_t cf);

// 颜色值转名称字符串
const char *rkclr_to_string(uint32_t cf);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif // _RK_COLOR_H_
