/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_VOP2_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup VOP2
 *  @{
 */

/** @defgroup VOP2_How_To_Use How To Use
 *  @{

 The VOP2 driver can be used as follows:

 @} */

#include "hal_display.h"
#include "hal_vop2.h"
#include "hal_math.h"

/** @defgroup VOP2_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */

/** @defgroup VOP2_Public_Definition Public Definition
 *  @{
 */
/********************* Public Function Definition ****************************/
eVOP2_scaleMode VOP2_GetSclMode(int src, int dst)
{
    if (src < dst) {
        return SCALE_UP;
    } else if (src > dst) {
        return SCALE_DOWN;
    }

    return SCALE_NONE;
}

HAL_Check VOP2_IsClusterWin(uint8_t physId)
{
    if (physId == ROCKCHIP_VOP2_CLUSTER0 || physId == ROCKCHIP_VOP2_CLUSTER1) {
        return HAL_TRUE;
    } else {
        return HAL_FALSE;
    }
}

HAL_Check VOP2_IsEsmartWin(uint8_t physId)
{
    if (physId == ROCKCHIP_VOP2_ESMART0 || physId == ROCKCHIP_VOP2_ESMART1 ||
        physId == ROCKCHIP_VOP2_SMART0 || physId == ROCKCHIP_VOP2_SMART1) {
        return HAL_TRUE;
    } else {
        return HAL_FALSE;
    }
}

HAL_Check VOP2_IsOutputYcSwap(uint32_t busFormat)
{
    switch (busFormat) {
    case MEDIA_BUS_FMT_YUYV8_1X16:
    case MEDIA_BUS_FMT_YVYU8_1X16:
    case MEDIA_BUS_FMT_YUYV8_2X8:
    case MEDIA_BUS_FMT_YVYU8_2X8:
        return HAL_TRUE;
    default:
        return HAL_FALSE;
    }
}

HAL_Check VOP2_IsUvSwap(uint32_t busFormat, uint32_t outputMode)
{
    /*
     * FIXME:
     *
     * There is no media type for YUV444 output,
     * so when out_mode is AAAA or P888, assume output is YUV444 on
     * yuv format.
     *
     * From H/W testing, YUV444 mode need a rb swap.
     */
    if (busFormat == MEDIA_BUS_FMT_YVYU8_1X16 ||
        busFormat == MEDIA_BUS_FMT_VYUY8_1X16 ||
        busFormat == MEDIA_BUS_FMT_YVYU8_2X8 ||
        busFormat == MEDIA_BUS_FMT_VYUY8_2X8 ||
        ((busFormat == MEDIA_BUS_FMT_YUV8_1X24 ||
          busFormat == MEDIA_BUS_FMT_YUV10_1X30) &&
         (outputMode == ROCKCHIP_OUT_MODE_AAAA ||
          outputMode == ROCKCHIP_OUT_MODE_P888))) {
        return HAL_TRUE;
    } else {
        return HAL_FALSE;
    }
}

HAL_Check VOP2_IsYuvOutput(uint32_t busFormat)
{
    switch (busFormat) {
    case MEDIA_BUS_FMT_YUV8_1X24:
    case MEDIA_BUS_FMT_YUV10_1X30:
    case MEDIA_BUS_FMT_YUYV10_1X20:
    case MEDIA_BUS_FMT_UYYVYY8_0_5X24:
    case MEDIA_BUS_FMT_UYYVYY10_0_5X30:
    case MEDIA_BUS_FMT_YUYV8_2X8:
    case MEDIA_BUS_FMT_YVYU8_2X8:
    case MEDIA_BUS_FMT_UYVY8_2X8:
    case MEDIA_BUS_FMT_VYUY8_2X8:
    case MEDIA_BUS_FMT_YUYV8_1X16:
    case MEDIA_BUS_FMT_YVYU8_1X16:
    case MEDIA_BUS_FMT_UYVY8_1X16:
    case MEDIA_BUS_FMT_VYUY8_1X16:

        return HAL_TRUE;
    default:

        return HAL_FALSE;
    }
}

int VOP2_RectWidth(const struct WIN_RECT *r)
{
    return r->x2 - r->x1;
}

int VOP2_RectHeight(const struct WIN_RECT *r)
{
    return r->y2 - r->y1;
}

uint32_t VOP2_SclCalScale(uint32_t src, uint32_t dst, uint32_t shift)
{
    return ((src * 2 - 3) << (shift - 1)) / (dst - 1);
}

uint32_t VOP2_SclCalScale2(uint32_t src, uint32_t dst)
{
    return ((src - 1) << 12) / (dst - 1);
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_VOP2_MODULE_ENABLED */
