/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_VOP_MODULE_ENABLED) || defined(HAL_VOP2_MODULE_ENABLED) || defined(HAL_DSI_MODULE_ENABLED) || defined(HAL_DW_MIPI_DSI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DISPLAY
 *  @{
 */

#ifndef _HAL_DISPLAY_H_
#define _HAL_DISPLAY_H_

/***************************** MACRO Definition ******************************/
/** @defgroup DISPLAY_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/** RK display connector type */
#define RK_DISPLAY_CONNECTOR_UNKNOWN 0
#define RK_DISPLAY_CONNECTOR_RGB     1
#define RK_DISPLAY_CONNECTOR_DSI     2

/** video mode flag default polarity is positive */
#define VIDEO_MODE_FLAG_NHSYNC    HAL_BIT(0)
#define VIDEO_MODE_FLAG_NVSYNC    HAL_BIT(1)
#define VIDEO_MODE_FLAG_NPIXDATA  HAL_BIT(2)
#define VIDEO_MODE_FLAG_DEN       HAL_BIT(3)
#define VIDEO_MODE_FLAG_INTERLACE HAL_BIT(4)
#define VIDEO_MODE_FLAG_DBLSCAN   HAL_BIT(5)
#define VIDEO_MODE_FLAG_CSYNC     HAL_BIT(6)
#define VIDEO_MODE_FLAG_PCSYNC    HAL_BIT(7)
#define VIDEO_MODE_FLAG_NCSYNC    HAL_BIT(8)
#define VIDEO_MODE_FLAG_HSKEW     HAL_BIT(9) /* hskew provided */
#define VIDEO_MODE_FLAG_BCAST     HAL_BIT(10)
#define VIDEO_MODE_FLAG_PIXMUX    HAL_BIT(11)
#define VIDEO_MODE_FLAG_DBLCLK    HAL_BIT(12)
#define VIDEO_MODE_FLAG_CLKDIV2   HAL_BIT(13)
/** video mode */
#define DSI_MODE_VIDEO HAL_BIT(4)
/** video burst mode */
#define DSI_MODE_VIDEO_BURST HAL_BIT(5)
/** video pulse mode */
#define DSI_MODE_VIDEO_SYNC_PULSE HAL_BIT(6)
/** enable auto vertical count mode */
#define DSI_MODE_VIDEO_AUTO_VERT HAL_BIT(7)
/** disable EoT packets in HS mode */
#define DSI_MODE_EOT_PACKET HAL_BIT(8)
/** device supports non-continuous clock behavior (DSI spec 5.6.1) */
#define DSI_CLOCK_NON_CONTINUOUS HAL_BIT(9)
/** transmit data in low power */
#define DSI_MODE_LPM HAL_BIT(10)
#define DSC_ENABLE   HAL_BIT(11)

/** media bus format from DRM define */
/* RGB - next is	0x1024 */
#define MEDIA_BUS_FMT_RGB444_1X12           0x1016
#define MEDIA_BUS_FMT_RGB444_2X8_PADHI_BE   0x1001
#define MEDIA_BUS_FMT_RGB444_2X8_PADHI_LE   0x1002
#define MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE   0x1003
#define MEDIA_BUS_FMT_RGB555_2X8_PADHI_LE   0x1004
#define MEDIA_BUS_FMT_RGB565_1X16           0x1017
#define MEDIA_BUS_FMT_BGR565_2X8_BE         0x1005
#define MEDIA_BUS_FMT_BGR565_2X8_LE         0x1006
#define MEDIA_BUS_FMT_RGB565_2X8_BE         0x1007
#define MEDIA_BUS_FMT_RGB565_2X8_LE         0x1008
#define MEDIA_BUS_FMT_RGB666_1X18           0x1009
#define MEDIA_BUS_FMT_RBG888_1X24           0x100e
#define MEDIA_BUS_FMT_RGB666_1X24_CPADHI    0x1015
#define MEDIA_BUS_FMT_RGB666_1X7X3_SPWG     0x1010
#define MEDIA_BUS_FMT_RGB666_1X7X3_JEIDA    0x101b
#define MEDIA_BUS_FMT_BGR888_1X24           0x1013
#define MEDIA_BUS_FMT_SRGB666_3X8           0x101b
#define MEDIA_BUS_FMT_BGR888_3X8            0x101b
#define MEDIA_BUS_FMT_GBR888_1X24           0x1014
#define MEDIA_BUS_FMT_RGB888_1X24           0x100a
#define MEDIA_BUS_FMT_RGB888_2X12_BE        0x100b
#define MEDIA_BUS_FMT_RGB888_2X12_LE        0x100c
#define MEDIA_BUS_FMT_SRGB888_3X8           0x101c
#define MEDIA_BUS_FMT_RGB888_3X8            0x101c
#define MEDIA_BUS_FMT_SBGR888_3X8           0x101d
#define MEDIA_BUS_FMT_SRBG888_3X8           0x101e
#define MEDIA_BUS_FMT_RGB888_1X7X4_SPWG     0x1011
#define MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA    0x1012
#define MEDIA_BUS_FMT_ARGB8888_1X32         0x100d
#define MEDIA_BUS_FMT_RGB888_1X32_PADHI     0x100f
#define MEDIA_BUS_FMT_RGB101010_1X30        0x1018
#define MEDIA_BUS_FMT_RGB121212_1X36        0x1019
#define MEDIA_BUS_FMT_RGB161616_1X48        0x101a
#define MEDIA_BUS_FMT_SRGB888_DUMMY_4X8     0x101f
#define MEDIA_BUS_FMT_RGB888_DUMMY_4X8      0x101f
#define MEDIA_BUS_FMT_SBGR888_DUMMY_4X8     0x1020
#define MEDIA_BUS_FMT_BGR888_DUMMY_4X8      0x1020
#define MEDIA_BUS_FMT_SRBG888_DUMMY_4X8     0x1021
#define MEDIA_BUS_FMT_RGB101010_1X7X5_SPWG  0x1022
#define MEDIA_BUS_FMT_RGB101010_1X7X5_JEIDA 0x1023
#define MEDIA_BUS_FMT_SRGB666_3X6           0x1100
#define MEDIA_BUS_FMT_RGB666_3X6            0x1100

/* YUV (including grey) - next is	0x202e */
#define MEDIA_BUS_FMT_Y8_1X8           0x2001
#define MEDIA_BUS_FMT_UV8_1X8          0x2015
#define MEDIA_BUS_FMT_UYVY8_1_5X8      0x2002
#define MEDIA_BUS_FMT_VYUY8_1_5X8      0x2003
#define MEDIA_BUS_FMT_YUYV8_1_5X8      0x2004
#define MEDIA_BUS_FMT_YVYU8_1_5X8      0x2005
#define MEDIA_BUS_FMT_UYVY8_2X8        0x2006
#define MEDIA_BUS_FMT_VYUY8_2X8        0x2007
#define MEDIA_BUS_FMT_YUYV8_2X8        0x2008
#define MEDIA_BUS_FMT_YVYU8_2X8        0x2009
#define MEDIA_BUS_FMT_Y10_1X10         0x200a
#define MEDIA_BUS_FMT_Y10_2X8_PADHI_LE 0x202c
#define MEDIA_BUS_FMT_UYVY10_2X10      0x2018
#define MEDIA_BUS_FMT_VYUY10_2X10      0x2019
#define MEDIA_BUS_FMT_YUYV10_2X10      0x200b
#define MEDIA_BUS_FMT_YVYU10_2X10      0x200c
#define MEDIA_BUS_FMT_Y12_1X12         0x2013
#define MEDIA_BUS_FMT_UYVY12_2X12      0x201c
#define MEDIA_BUS_FMT_VYUY12_2X12      0x201d
#define MEDIA_BUS_FMT_YUYV12_2X12      0x201e
#define MEDIA_BUS_FMT_YVYU12_2X12      0x201f
#define MEDIA_BUS_FMT_Y14_1X14         0x202d
#define MEDIA_BUS_FMT_UYVY8_1X16       0x200f
#define MEDIA_BUS_FMT_VYUY8_1X16       0x2010
#define MEDIA_BUS_FMT_YUYV8_1X16       0x2011
#define MEDIA_BUS_FMT_YVYU8_1X16       0x2012
#define MEDIA_BUS_FMT_YDYUYDYV8_1X16   0x2014
#define MEDIA_BUS_FMT_UYVY10_1X20      0x201a
#define MEDIA_BUS_FMT_VYUY10_1X20      0x201b
#define MEDIA_BUS_FMT_YUYV10_1X20      0x200d
#define MEDIA_BUS_FMT_YVYU10_1X20      0x200e
#define MEDIA_BUS_FMT_VUY8_1X24        0x2024
#define MEDIA_BUS_FMT_YUV8_1X24        0x2025
#define MEDIA_BUS_FMT_UYYVYY8_0_5X24   0x2026
#define MEDIA_BUS_FMT_UYVY12_1X24      0x2020
#define MEDIA_BUS_FMT_VYUY12_1X24      0x2021
#define MEDIA_BUS_FMT_YUYV12_1X24      0x2022
#define MEDIA_BUS_FMT_YVYU12_1X24      0x2023
#define MEDIA_BUS_FMT_YUV10_1X30       0x2016
#define MEDIA_BUS_FMT_UYYVYY10_0_5X30  0x2027
#define MEDIA_BUS_FMT_AYUV8_1X32       0x2017
#define MEDIA_BUS_FMT_UYYVYY12_0_5X36  0x2028
#define MEDIA_BUS_FMT_YUV12_1X36       0x2029
#define MEDIA_BUS_FMT_YUV16_1X48       0x202a
#define MEDIA_BUS_FMT_UYYVYY16_0_5X48  0x202b

#define fourcc_code(a, b, c, d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | \
                 ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define DRM_FORMAT_BIG_ENDIAN (1U<<31) /* format is big endian instead of little endian */

/* Reserve 0 for the invalid format specifier */
#define DRM_FORMAT_INVALID 0

/* color index */
#define DRM_FORMAT_C1 fourcc_code('C', '1', ' ', ' ')       /* [7:0] C0:C1:C2:C3:C4:C5:C6:C7 1:1:1:1:1:1:1:1 eight pixels/byte */
#define DRM_FORMAT_C2 fourcc_code('C', '2', ' ', ' ')       /* [7:0] C0:C1:C2:C3 2:2:2:2 four pixels/byte */
#define DRM_FORMAT_C4 fourcc_code('C', '4', ' ', ' ')       /* [7:0] C0:C1 4:4 two pixels/byte */
#define DRM_FORMAT_C8 fourcc_code('C', '8', ' ', ' ')       /* [7:0] C */

/* 1 bpp Darkness (inverse relationship between channel value and brightness) */
#define DRM_FORMAT_D1 fourcc_code('D', '1', ' ', ' ')       /* [7:0] D0:D1:D2:D3:D4:D5:D6:D7 1:1:1:1:1:1:1:1 eight pixels/byte */

/* 2 bpp Darkness (inverse relationship between channel value and brightness) */
#define DRM_FORMAT_D2 fourcc_code('D', '2', ' ', ' ')       /* [7:0] D0:D1:D2:D3 2:2:2:2 four pixels/byte */

/* 4 bpp Darkness (inverse relationship between channel value and brightness) */
#define DRM_FORMAT_D4 fourcc_code('D', '4', ' ', ' ')       /* [7:0] D0:D1 4:4 two pixels/byte */

/* 8 bpp Darkness (inverse relationship between channel value and brightness) */
#define DRM_FORMAT_D8 fourcc_code('D', '8', ' ', ' ')       /* [7:0] D */

/* 1 bpp Red (direct relationship between channel value and brightness) */
#define DRM_FORMAT_R1 fourcc_code('R', '1', ' ', ' ')       /* [7:0] R0:R1:R2:R3:R4:R5:R6:R7 1:1:1:1:1:1:1:1 eight pixels/byte */

/* 2 bpp Red (direct relationship between channel value and brightness) */
#define DRM_FORMAT_R2 fourcc_code('R', '2', ' ', ' ')       /* [7:0] R0:R1:R2:R3 2:2:2:2 four pixels/byte */

/* 4 bpp Red (direct relationship between channel value and brightness) */
#define DRM_FORMAT_R4 fourcc_code('R', '4', ' ', ' ')       /* [7:0] R0:R1 4:4 two pixels/byte */

/* 8 bpp Red (direct relationship between channel value and brightness) */
#define DRM_FORMAT_R8 fourcc_code('R', '8', ' ', ' ')       /* [7:0] R */

/* 10 bpp Red (direct relationship between channel value and brightness) */
#define DRM_FORMAT_R10 fourcc_code('R', '1', '0', ' ')      /* [15:0] x:R 6:10 little endian */

/* 12 bpp Red (direct relationship between channel value and brightness) */
#define DRM_FORMAT_R12 fourcc_code('R', '1', '2', ' ')      /* [15:0] x:R 4:12 little endian */

/* 16 bpp Red (direct relationship between channel value and brightness) */
#define DRM_FORMAT_R16 fourcc_code('R', '1', '6', ' ')      /* [15:0] R little endian */

/* 16 bpp RG */
#define DRM_FORMAT_RG88 fourcc_code('R', 'G', '8', '8')     /* [15:0] R:G 8:8 little endian */
#define DRM_FORMAT_GR88 fourcc_code('G', 'R', '8', '8')     /* [15:0] G:R 8:8 little endian */

/* 32 bpp RG */
#define DRM_FORMAT_RG1616 fourcc_code('R', 'G', '3', '2')   /* [31:0] R:G 16:16 little endian */
#define DRM_FORMAT_GR1616 fourcc_code('G', 'R', '3', '2')   /* [31:0] G:R 16:16 little endian */

/* 8 bpp RGB */
#define DRM_FORMAT_RGB332 fourcc_code('R', 'G', 'B', '8')   /* [7:0] R:G:B 3:3:2 */
#define DRM_FORMAT_BGR233 fourcc_code('B', 'G', 'R', '8')   /* [7:0] B:G:R 2:3:3 */

/* 16 bpp RGB */
#define DRM_FORMAT_XRGB4444 fourcc_code('X', 'R', '1', '2') /* [15:0] x:R:G:B 4:4:4:4 little endian */
#define DRM_FORMAT_XBGR4444 fourcc_code('X', 'B', '1', '2') /* [15:0] x:B:G:R 4:4:4:4 little endian */
#define DRM_FORMAT_RGBX4444 fourcc_code('R', 'X', '1', '2') /* [15:0] R:G:B:x 4:4:4:4 little endian */
#define DRM_FORMAT_BGRX4444 fourcc_code('B', 'X', '1', '2') /* [15:0] B:G:R:x 4:4:4:4 little endian */

#define DRM_FORMAT_ARGB4444 fourcc_code('A', 'R', '1', '2') /* [15:0] A:R:G:B 4:4:4:4 little endian */
#define DRM_FORMAT_ABGR4444 fourcc_code('A', 'B', '1', '2') /* [15:0] A:B:G:R 4:4:4:4 little endian */
#define DRM_FORMAT_RGBA4444 fourcc_code('R', 'A', '1', '2') /* [15:0] R:G:B:A 4:4:4:4 little endian */
#define DRM_FORMAT_BGRA4444 fourcc_code('B', 'A', '1', '2') /* [15:0] B:G:R:A 4:4:4:4 little endian */

#define DRM_FORMAT_XRGB1555 fourcc_code('X', 'R', '1', '5') /* [15:0] x:R:G:B 1:5:5:5 little endian */
#define DRM_FORMAT_XBGR1555 fourcc_code('X', 'B', '1', '5') /* [15:0] x:B:G:R 1:5:5:5 little endian */
#define DRM_FORMAT_RGBX5551 fourcc_code('R', 'X', '1', '5') /* [15:0] R:G:B:x 5:5:5:1 little endian */
#define DRM_FORMAT_BGRX5551 fourcc_code('B', 'X', '1', '5') /* [15:0] B:G:R:x 5:5:5:1 little endian */

#define DRM_FORMAT_ARGB1555 fourcc_code('A', 'R', '1', '5') /* [15:0] A:R:G:B 1:5:5:5 little endian */
#define DRM_FORMAT_ABGR1555 fourcc_code('A', 'B', '1', '5') /* [15:0] A:B:G:R 1:5:5:5 little endian */
#define DRM_FORMAT_RGBA5551 fourcc_code('R', 'A', '1', '5') /* [15:0] R:G:B:A 5:5:5:1 little endian */
#define DRM_FORMAT_BGRA5551 fourcc_code('B', 'A', '1', '5') /* [15:0] B:G:R:A 5:5:5:1 little endian */

#define DRM_FORMAT_RGB565 fourcc_code('R', 'G', '1', '6')   /* [15:0] R:G:B 5:6:5 little endian */
#define DRM_FORMAT_BGR565 fourcc_code('B', 'G', '1', '6')   /* [15:0] B:G:R 5:6:5 little endian */

/* 24 bpp RGB */
#define DRM_FORMAT_RGB888 fourcc_code('R', 'G', '2', '4')   /* [23:0] R:G:B little endian */
#define DRM_FORMAT_BGR888 fourcc_code('B', 'G', '2', '4')   /* [23:0] B:G:R little endian */

/* 32 bpp RGB */
#define DRM_FORMAT_XRGB8888 fourcc_code('X', 'R', '2', '4') /* [31:0] x:R:G:B 8:8:8:8 little endian */
#define DRM_FORMAT_XBGR8888 fourcc_code('X', 'B', '2', '4') /* [31:0] x:B:G:R 8:8:8:8 little endian */
#define DRM_FORMAT_RGBX8888 fourcc_code('R', 'X', '2', '4') /* [31:0] R:G:B:x 8:8:8:8 little endian */
#define DRM_FORMAT_BGRX8888 fourcc_code('B', 'X', '2', '4') /* [31:0] B:G:R:x 8:8:8:8 little endian */

#define DRM_FORMAT_ARGB8888 fourcc_code('A', 'R', '2', '4') /* [31:0] A:R:G:B 8:8:8:8 little endian */
#define DRM_FORMAT_ABGR8888 fourcc_code('A', 'B', '2', '4') /* [31:0] A:B:G:R 8:8:8:8 little endian */
#define DRM_FORMAT_RGBA8888 fourcc_code('R', 'A', '2', '4') /* [31:0] R:G:B:A 8:8:8:8 little endian */
#define DRM_FORMAT_BGRA8888 fourcc_code('B', 'A', '2', '4') /* [31:0] B:G:R:A 8:8:8:8 little endian */

#define DRM_FORMAT_XRGB2101010 fourcc_code('X', 'R', '3', '0')  /* [31:0] x:R:G:B 2:10:10:10 little endian */
#define DRM_FORMAT_XBGR2101010 fourcc_code('X', 'B', '3', '0')  /* [31:0] x:B:G:R 2:10:10:10 little endian */
#define DRM_FORMAT_RGBX1010102 fourcc_code('R', 'X', '3', '0')  /* [31:0] R:G:B:x 10:10:10:2 little endian */
#define DRM_FORMAT_BGRX1010102 fourcc_code('B', 'X', '3', '0')  /* [31:0] B:G:R:x 10:10:10:2 little endian */

#define DRM_FORMAT_ARGB2101010 fourcc_code('A', 'R', '3', '0')  /* [31:0] A:R:G:B 2:10:10:10 little endian */
#define DRM_FORMAT_ABGR2101010 fourcc_code('A', 'B', '3', '0')  /* [31:0] A:B:G:R 2:10:10:10 little endian */
#define DRM_FORMAT_RGBA1010102 fourcc_code('R', 'A', '3', '0')  /* [31:0] R:G:B:A 10:10:10:2 little endian */
#define DRM_FORMAT_BGRA1010102 fourcc_code('B', 'A', '3', '0')  /* [31:0] B:G:R:A 10:10:10:2 little endian */

/* 64 bpp RGB */
#define DRM_FORMAT_XRGB16161616 fourcc_code('X', 'R', '4', '8') /* [63:0] x:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_XBGR16161616 fourcc_code('X', 'B', '4', '8') /* [63:0] x:B:G:R 16:16:16:16 little endian */

#define DRM_FORMAT_ARGB16161616 fourcc_code('A', 'R', '4', '8') /* [63:0] A:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_ABGR16161616 fourcc_code('A', 'B', '4', '8') /* [63:0] A:B:G:R 16:16:16:16 little endian */

/*
 * Floating point 64bpp RGB
 * IEEE 754-2008 binary16 half-precision float
 * [15:0] sign:exponent:mantissa 1:5:10
 */
#define DRM_FORMAT_XRGB16161616F fourcc_code('X', 'R', '4', 'H') /* [63:0] x:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_XBGR16161616F fourcc_code('X', 'B', '4', 'H') /* [63:0] x:B:G:R 16:16:16:16 little endian */

#define DRM_FORMAT_ARGB16161616F fourcc_code('A', 'R', '4', 'H') /* [63:0] A:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_ABGR16161616F fourcc_code('A', 'B', '4', 'H') /* [63:0] A:B:G:R 16:16:16:16 little endian */

/*
 * RGBA format with 10-bit components packed in 64-bit per pixel, with 6 bits
 * of unused padding per component:
 */
#define DRM_FORMAT_AXBXGXRX106106106106 fourcc_code('A', 'B', '1', '0') /* [63:0] A:x:B:x:G:x:R:x 10:6:10:6:10:6:10:6 little endian */

/* packed YCbCr */
#define DRM_FORMAT_YUYV fourcc_code('Y', 'U', 'Y', 'V')     /* [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian */
#define DRM_FORMAT_YVYU fourcc_code('Y', 'V', 'Y', 'U')     /* [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian */
#define DRM_FORMAT_UYVY fourcc_code('U', 'Y', 'V', 'Y')     /* [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian */
#define DRM_FORMAT_VYUY fourcc_code('V', 'Y', 'U', 'Y')     /* [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian */

#define DRM_FORMAT_AYUV      fourcc_code('A', 'Y', 'U', 'V')/* [31:0] A:Y:Cb:Cr 8:8:8:8 little endian */
#define DRM_FORMAT_AVUY8888  fourcc_code('A', 'V', 'U', 'Y')/* [31:0] A:Cr:Cb:Y 8:8:8:8 little endian */
#define DRM_FORMAT_XYUV8888  fourcc_code('X', 'Y', 'U', 'V')/* [31:0] X:Y:Cb:Cr 8:8:8:8 little endian */
#define DRM_FORMAT_XVUY8888  fourcc_code('X', 'V', 'U', 'Y')/* [31:0] X:Cr:Cb:Y 8:8:8:8 little endian */
#define DRM_FORMAT_VUY888    fourcc_code('V', 'U', '2', '4')/* [23:0] Cr:Cb:Y 8:8:8 little endian */
#define DRM_FORMAT_VUY101010 fourcc_code('V', 'U', '3', '0')    /* Y followed by U then V, 10:10:10. Non-linear modifier only */

/*
 * packed Y2xx indicate for each component, xx valid data occupy msb
 * 16-xx padding occupy lsb
 */
#define DRM_FORMAT_Y210 fourcc_code('Y', '2', '1', '0')         /* [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y pixels */
#define DRM_FORMAT_Y212 fourcc_code('Y', '2', '1', '2')         /* [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y pixels */
#define DRM_FORMAT_Y216 fourcc_code('Y', '2', '1', '6')         /* [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels */

/*
 * packed Y4xx indicate for each component, xx valid data occupy msb
 * 16-xx padding occupy lsb except Y410
 */
#define DRM_FORMAT_Y410 fourcc_code('Y', '4', '1', '0')         /* [31:0] A:Cr:Y:Cb 2:10:10:10 little endian */
#define DRM_FORMAT_Y412 fourcc_code('Y', '4', '1', '2')         /* [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian */
#define DRM_FORMAT_Y416 fourcc_code('Y', '4', '1', '6')         /* [63:0] A:Cr:Y:Cb 16:16:16:16 little endian */

#define DRM_FORMAT_XVYU2101010     fourcc_code('X', 'V', '3', '0')/* [31:0] X:Cr:Y:Cb 2:10:10:10 little endian */
#define DRM_FORMAT_XVYU12_16161616 fourcc_code('X', 'V', '3', '6')  /* [63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian */
#define DRM_FORMAT_XVYU16161616    fourcc_code('X', 'V', '4', '8')/* [63:0] X:Cr:Y:Cb 16:16:16:16 little endian */

/*
 * packed YCbCr420 2x2 tiled formats
 * first 64 bits will contain Y,Cb,Cr components for a 2x2 tile
 */
/* [63:0]   A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0  1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian */
#define DRM_FORMAT_Y0L0 fourcc_code('Y', '0', 'L', '0')
/* [63:0]   X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0  1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian */
#define DRM_FORMAT_X0L0 fourcc_code('X', '0', 'L', '0')

/* [63:0]   A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian */
#define DRM_FORMAT_Y0L2 fourcc_code('Y', '0', 'L', '2')
/* [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian */
#define DRM_FORMAT_X0L2 fourcc_code('X', '0', 'L', '2')

/*
 * 1-plane YUV 4:2:0
 * In these formats, the component ordering is specified (Y, followed by U
 * then V), but the exact Linear layout is undefined.
 * These formats can only be used with a non-Linear modifier.
 */
#define DRM_FORMAT_YUV420_8BIT  fourcc_code('Y', 'U', '0', '8')
#define DRM_FORMAT_YUV420_10BIT fourcc_code('Y', 'U', '1', '0')

/*
 * 2 plane RGB + A
 * index 0 = RGB plane, same format as the corresponding non _A8 format has
 * index 1 = A plane, [7:0] A
 */
#define DRM_FORMAT_XRGB8888_A8 fourcc_code('X', 'R', 'A', '8')
#define DRM_FORMAT_XBGR8888_A8 fourcc_code('X', 'B', 'A', '8')
#define DRM_FORMAT_RGBX8888_A8 fourcc_code('R', 'X', 'A', '8')
#define DRM_FORMAT_BGRX8888_A8 fourcc_code('B', 'X', 'A', '8')
#define DRM_FORMAT_RGB888_A8   fourcc_code('R', '8', 'A', '8')
#define DRM_FORMAT_BGR888_A8   fourcc_code('B', '8', 'A', '8')
#define DRM_FORMAT_RGB565_A8   fourcc_code('R', '5', 'A', '8')
#define DRM_FORMAT_BGR565_A8   fourcc_code('B', '5', 'A', '8')

/*
 * 2 plane YCbCr
 * index 0 = Y plane, [7:0] Y
 * index 1 = Cr:Cb plane, [15:0] Cr:Cb little endian
 * or
 * index 1 = Cb:Cr plane, [15:0] Cb:Cr little endian
 */
#define DRM_FORMAT_NV12 fourcc_code('N', 'V', '1', '2')     /* 2x2 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV21 fourcc_code('N', 'V', '2', '1')     /* 2x2 subsampled Cb:Cr plane */
#define DRM_FORMAT_NV16 fourcc_code('N', 'V', '1', '6')     /* 2x1 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV61 fourcc_code('N', 'V', '6', '1')     /* 2x1 subsampled Cb:Cr plane */
#define DRM_FORMAT_NV24 fourcc_code('N', 'V', '2', '4')     /* non-subsampled Cr:Cb plane */
#define DRM_FORMAT_NV42 fourcc_code('N', 'V', '4', '2')     /* non-subsampled Cb:Cr plane */
/*
 * 2 plane YCbCr
 * index 0 = Y plane, [39:0] Y3:Y2:Y1:Y0 little endian
 * index 1 = Cr:Cb plane, [39:0] Cr1:Cb1:Cr0:Cb0 little endian
 */
#define DRM_FORMAT_NV15 fourcc_code('N', 'V', '1', '5')     /* 2x2 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV20 fourcc_code('N', 'V', '2', '0')     /* 2x1 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV30 fourcc_code('N', 'V', '3', '0')     /* non-subsampled Cr:Cb plane */

/*
 * 2 plane YCbCr MSB aligned
 * index 0 = Y plane, [15:0] Y:x [10:6] little endian
 * index 1 = Cr:Cb plane, [31:0] Cr:x:Cb:x [10:6:10:6] little endian
 */
#define DRM_FORMAT_P210 fourcc_code('P', '2', '1', '0')     /* 2x1 subsampled Cr:Cb plane, 10 bit per channel */

/*
 * 2 plane YCbCr MSB aligned
 * index 0 = Y plane, [15:0] Y:x [10:6] little endian
 * index 1 = Cr:Cb plane, [31:0] Cr:x:Cb:x [10:6:10:6] little endian
 */
#define DRM_FORMAT_P010 fourcc_code('P', '0', '1', '0')     /* 2x2 subsampled Cr:Cb plane 10 bits per channel */

/*
 * 2 plane YCbCr MSB aligned
 * index 0 = Y plane, [15:0] Y:x [12:4] little endian
 * index 1 = Cr:Cb plane, [31:0] Cr:x:Cb:x [12:4:12:4] little endian
 */
#define DRM_FORMAT_P012 fourcc_code('P', '0', '1', '2')     /* 2x2 subsampled Cr:Cb plane 12 bits per channel */

/*
 * 2 plane YCbCr MSB aligned
 * index 0 = Y plane, [15:0] Y little endian
 * index 1 = Cr:Cb plane, [31:0] Cr:Cb [16:16] little endian
 */
#define DRM_FORMAT_P016 fourcc_code('P', '0', '1', '6')     /* 2x2 subsampled Cr:Cb plane 16 bits per channel */

/* 2 plane YCbCr420.
 * 3 10 bit components and 2 padding bits packed into 4 bytes.
 * index 0 = Y plane, [31:0] x:Y2:Y1:Y0 2:10:10:10 little endian
 * index 1 = Cr:Cb plane, [63:0] x:Cr2:Cb2:Cr1:x:Cb1:Cr0:Cb0 [2:10:10:10:2:10:10:10] little endian
 */
#define DRM_FORMAT_P030 fourcc_code('P', '0', '3', '0')     /* 2x2 subsampled Cr:Cb plane 10 bits per channel packed */

/* 3 plane non-subsampled (444) YCbCr
 * 16 bits per component, but only 10 bits are used and 6 bits are padded
 * index 0: Y plane, [15:0] Y:x [10:6] little endian
 * index 1: Cb plane, [15:0] Cb:x [10:6] little endian
 * index 2: Cr plane, [15:0] Cr:x [10:6] little endian
 */
#define DRM_FORMAT_Q410 fourcc_code('Q', '4', '1', '0')

/* 3 plane non-subsampled (444) YCrCb
 * 16 bits per component, but only 10 bits are used and 6 bits are padded
 * index 0: Y plane, [15:0] Y:x [10:6] little endian
 * index 1: Cr plane, [15:0] Cr:x [10:6] little endian
 * index 2: Cb plane, [15:0] Cb:x [10:6] little endian
 */
#define DRM_FORMAT_Q401 fourcc_code('Q', '4', '0', '1')

/*
 * 3 plane YCbCr
 * index 0: Y plane, [7:0] Y
 * index 1: Cb plane, [7:0] Cb
 * index 2: Cr plane, [7:0] Cr
 * or
 * index 1: Cr plane, [7:0] Cr
 * index 2: Cb plane, [7:0] Cb
 */
#define DRM_FORMAT_YUV410 fourcc_code('Y', 'U', 'V', '9')   /* 4x4 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU410 fourcc_code('Y', 'V', 'U', '9')   /* 4x4 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV411 fourcc_code('Y', 'U', '1', '1')   /* 4x1 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU411 fourcc_code('Y', 'V', '1', '1')   /* 4x1 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV420 fourcc_code('Y', 'U', '1', '2')   /* 2x2 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU420 fourcc_code('Y', 'V', '1', '2')   /* 2x2 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV422 fourcc_code('Y', 'U', '1', '6')   /* 2x1 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU422 fourcc_code('Y', 'V', '1', '6')   /* 2x1 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV444 fourcc_code('Y', 'U', '2', '4')   /* non-subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU444 fourcc_code('Y', 'V', '2', '4')   /* non-subsampled Cr (1) and Cb (2) planes */

#define DP_AUX_MAX_PAYLOAD_BYTES 16

#define DP_AUX_I2C_WRITE               0x0
#define DP_AUX_I2C_READ                0x1
#define DP_AUX_I2C_WRITE_STATUS_UPDATE 0x2
#define DP_AUX_I2C_MOT                 0x4
#define DP_AUX_NATIVE_WRITE            0x8
#define DP_AUX_NATIVE_READ             0x9

#define DP_AUX_NATIVE_REPLY_ACK   (0x0 << 0)
#define DP_AUX_NATIVE_REPLY_NACK  (0x1 << 0)
#define DP_AUX_NATIVE_REPLY_DEFER (0x2 << 0)
#define DP_AUX_NATIVE_REPLY_MASK  (0x3 << 0)

#define DP_AUX_I2C_REPLY_ACK   (0x0 << 2)
#define DP_AUX_I2C_REPLY_NACK  (0x1 << 2)
#define DP_AUX_I2C_REPLY_DEFER (0x2 << 2)
#define DP_AUX_I2C_REPLY_MASK  (0x3 << 2)

#define DP_TRAINING_LANE0_SET 0x103
#define DP_TRAINING_LANE1_SET 0x104
#define DP_TRAINING_LANE2_SET 0x105
#define DP_TRAINING_LANE3_SET 0x106

#define DP_TRAIN_VOLTAGE_SWING_MASK    0x3
#define DP_TRAIN_VOLTAGE_SWING_SHIFT   0
#define DP_TRAIN_MAX_SWING_REACHED     (1 << 2)
#define DP_TRAIN_VOLTAGE_SWING_LEVEL_0 (0 << 0)
#define DP_TRAIN_VOLTAGE_SWING_LEVEL_1 (1 << 0)
#define DP_TRAIN_VOLTAGE_SWING_LEVEL_2 (2 << 0)
#define DP_TRAIN_VOLTAGE_SWING_LEVEL_3 (3 << 0)

#define DP_TRAIN_PRE_EMPHASIS_MASK (3 << 3)
#define DP_TRAIN_PRE_EMPH_LEVEL_0  (0 << 3)
#define DP_TRAIN_PRE_EMPH_LEVEL_1  (1 << 3)
#define DP_TRAIN_PRE_EMPH_LEVEL_2  (2 << 3)
#define DP_TRAIN_PRE_EMPH_LEVEL_3  (3 << 3)

#define DP_TRAIN_PRE_EMPHASIS_SHIFT       3
#define DP_TRAIN_MAX_PRE_EMPHASIS_REACHED (1 << 5)

#define DP_TIMEOUT_LOOP_COUNT 100
#define MAX_CR_LOOP           5
#define MAX_EQ_LOOP           5

/***************************** Structure Definition **************************/

/**
 *  Add multi line brief to here
 *  ...
 */
struct DISPLAY_MODE_INFO {
    uint32_t clock;
    uint16_t hdisplay;
    uint16_t hsyncStart;
    uint16_t hsyncEnd;
    uint16_t htotal;
    uint16_t vdisplay;
    uint16_t vsyncStart;
    uint16_t vsyncEnd;
    uint16_t vtotal;
    uint32_t crtcClock;
    uint16_t crtcHdisplay;
    uint16_t crtcHsyncStart;
    uint16_t crtcHsyncEnd;
    uint16_t crtcHtotal;
    uint16_t crtcVdisplay;
    uint16_t crtcVsyncStart;
    uint16_t crtcVsyncEnd;
    uint16_t crtcVtotal;
    uint16_t flags;
    uint16_t vrefresh;
    uint16_t mcuCsRw;
    uint16_t mcuRwPw;
    uint16_t mcuRwCs;
};

/**
 * struct DISPLAY_RECT
 */
struct DISPLAY_RECT {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
};

/**
  * @brief  DP AUX message structure definition
  */
struct DP_AUX_MSG {
    uint32_t address;
    uint8_t request;
    uint8_t reply;
    void *buffer;
    uint32_t size;
};

/**
  * @brief  DP AUX structure definition
  */
struct DP_AUX {
    const char *name;
    int32_t (*transfer)(struct DP_AUX *aux,
                        struct DP_AUX_MSG *msg);
    uint32_t i2cNackCount;
    uint32_t i2cDeferCount;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup DISPLAY_Public_Function_Declare Public Function Declare
 *  @{
 */

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_VOP_MODULE_ENABLED || HAL_VOP2_MODULE_ENABLED || HAL_DSI_MODULE_ENABLED || HAL_DW_MIPI_DSI_MODULE_ENABLED */
