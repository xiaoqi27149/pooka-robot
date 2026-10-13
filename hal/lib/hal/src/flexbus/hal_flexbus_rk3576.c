/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(SOC_RK3576) && defined(HAL_FLEXBUS_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS
 *  @{
 */

/** @defgroup FLEXBUS_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup FLEXBUS_Exported_Functions_Group5 Other Functions

 *  @{
 */

/**
 * @brief  Platform-related Initial Configuration.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 */
void HAL_FLEXBUS_InitConfig(struct FLEXBUS_HANDLE *pFB)
{
    HAL_ASSERT(pFB != NULL);

    pFB->txFifoNum = 512;
    pFB->rxFifoNum = 512;
}

/**
 * @brief  Platform-related Grf Configuration.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @param  slaveMode: slave mode of FLEXBUS rx
 * @param  cPol: clock polarity of FLEXBUS rx
 * @param  cPha: clock phase of FLEXBUS rx
 */
void HAL_FLEXBUS_GrfConfig(struct FLEXBUS_HANDLE *pFB, bool slaveMode, bool cPol, bool cPha)
{
    uint32_t val = 0;

    HAL_ASSERT(pFB != NULL);

    if (slaveMode) {
        val |= GPIO1_IOC_MISC_CON0_FLEXBUS_DATA_PIPE_SEL_MASK;
        if ((!cPol && cPha) || (cPol && !cPha)) {
            val |= GPIO1_IOC_MISC_CON0_FLEXBUS_CLK_INV_SEL_MASK;
        }
    }
    WRITE_REG_MASK_WE(GPIO1_IOC->MISC_CON0,
                      GPIO1_IOC_MISC_CON0_FLEXBUS_CLK_INV_SEL_MASK |
                      GPIO1_IOC_MISC_CON0_FLEXBUS_DATA_PIPE_SEL_MASK,
                      val);
}

/** @} */

/** @} */

/** @} */

#endif  /* SOC_RK3506 && HAL_FLEXBUS_MODULE_ENABLED */
