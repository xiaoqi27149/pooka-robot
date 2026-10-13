/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_FLEXBUS_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS
 *  @{
 */

/** @defgroup FLEXBUS_How_To_Use How To Use
 *  @{

 The FLEXBUS driver can be used as follows:

 - Declare a FLEXBUS_HANDLE handle structure, for example:
   ```
   FLEXBUS_HANDLE instance;
   ```

 - Invoke HAL_FLEXBUS_Init() API to configure config FLEXBUS0_OpMode and FLEXBUS1_OpMode.

 - Invoke HAL_FLEXBUS_DeInit() if necessary.

 @} */

/** @defgroup FLEXBUS_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Variable Definition ***************************/

#if (FLEXBUS_LARGE_VERSION_NUMBER == 0)
static const struct HAL_FLEXBUS_DFS_REG s_flexbusDfsReg = {
    .dfs2Bit = 0x0,
    .dfs4Bit = 0x1,
    .dfs8Bit = 0x2,
    .dfs16Bit = 0x3,
    .dfsMask = 0x3,
};
#else
static const struct HAL_FLEXBUS_DFS_REG s_flexbusDfsReg = {
    .dfs1Bit = (0x0 << 29),
    .dfs2Bit = (0x1 << 29),
    .dfs4Bit = (0x2 << 29),
    .dfs8Bit = (0x3 << 29),
    .dfs16Bit = (0x4 << 29),
    .dfsMask = (0x7 << 29),
};
#endif

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup FLEXBUS_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/**
 * @brief  Set FLEXBUS_RMIO.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @param  rmio: FLEXBUS_RMIO to be set.
 * @param  pin_sel: FLEXBUS_LS pin_sel.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_SetRmio(struct FLEXBUS_HANDLE *pFB, uint32_t rmio, uint32_t pin_sel)
{
#if (FLEXBUS_LARGE_VERSION_NUMBER >= 2)
    HAL_ASSERT(pFB != NULL);

    if (rmio >= FLEXBUS_RMIO_NUM) {
        HAL_DBG_ERR("%s, unknown RMIO D%lu!\n", __func__, rmio);

        return HAL_INVAL;
    }
    if (pin_sel > FLEXBUS_LS_PIN_SEL_MAX) {
        HAL_DBG_ERR("%s, unknown ls_pin_sel 0x%lx!\n", __func__, pin_sel);

        return HAL_INVAL;
    }
    if (READ_REG(pFB->pRmioReg->SEL_D[rmio]) != FLEXBUS_LS_PIN_SEL_NO_PIN) {
        HAL_DBG_ERR("%s, RMIO D%lu is selected!\n", __func__, rmio);

        return HAL_INVAL;
    }

    WRITE_REG(pFB->pRmioReg->SEL_D[rmio], pin_sel);
#endif

    return HAL_OK;
}

/**
 * @brief  Set FLEXBUS FIFO_REGION for LS.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @param  lsCh: FLEXBUS_LS_CH.
 * @param  txEn: Use tx FIFO or not.
 * @param  rxEn: Use rx FIFO or not.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_SetFifoRegion(struct FLEXBUS_HANDLE *pFB, uint32_t lsCh, bool txEn,
                                     bool rxEn)
{
    HAL_Status ret = HAL_OK;

#if (FLEXBUS_LARGE_VERSION_NUMBER >= 2)
    uint32_t val;
    uint32_t region;
    int32_t i;

    HAL_ASSERT(pFB != NULL);

    if (lsCh >= FLEXBUS_LS_CH_NUM || pFB->pOpMode->opModeLs[lsCh] == FLEXBUS_LS_OPMODE_NULL ||
        pFB->pOpMode->opModeLs[lsCh] >= FLEXBUS_LS_OPMODE_MAX) {
        HAL_DBG_ERR("%s, unknown lsCh %lu!\n", __func__, lsCh);

        return HAL_INVAL;
    }

    if (txEn) {
        val = READ_REG(pFB->pReg->TXFIFO_REGION);
        for (i = 0; i < FLEXBUS_LS_CH_NUM; i++) {
            region = FLEXBUS_FIFO_REGION(val, i);
            if (region == lsCh + 1) {
                HAL_DBG_WRN("%s, Tx FIFO region%ld is used for LS_CH%lu\n", __func__, i, lsCh);
                break;
            }
            if (region == 0) {
                MODIFY_REG(pFB->pReg->TXFIFO_REGION, 0xf << (i * 4), (lsCh + 1) << (i * 4));
                WRITE_REG_MASK_WE(pFB->pReg->HS_LS_CTL, FLEXBUS_HS_LS_CTL_LS_RD_USE_BANDWIDTH_MASK,
                                  FLEXBUS_LS_OCCUPY_25_BANDWIDTH <<
                                  FLEXBUS_HS_LS_CTL_LS_RD_USE_BANDWIDTH_SHIFT);
                break;
            }
        }

        if (i >= FLEXBUS_LS_CH_NUM) {
            HAL_DBG_ERR("%s, cannot get Tx FIFO for LS_CH%lu!\n", __func__, lsCh);
            ret = HAL_ERROR;
        }
    }

    if (rxEn) {
        val = READ_REG(pFB->pReg->RXFIFO_REGION);
        for (i = 0; i < FLEXBUS_LS_CH_NUM; i++) {
            region = FLEXBUS_FIFO_REGION(val, i);
            if (region == lsCh + 1) {
                HAL_DBG_WRN("%s, Rx FIFO region%ld is used for LS_CH%lu\n", __func__, i, lsCh);
                break;
            }
            if (region == 0) {
                MODIFY_REG(pFB->pReg->RXFIFO_REGION, 0xf << (i * 4), (lsCh + 1) << (i * 4));
                WRITE_REG_MASK_WE(pFB->pReg->HS_LS_CTL, FLEXBUS_HS_LS_CTL_LS_WR_USE_BANDWIDTH_MASK,
                                  FLEXBUS_LS_OCCUPY_25_BANDWIDTH <<
                                  FLEXBUS_HS_LS_CTL_LS_WR_USE_BANDWIDTH_SHIFT);
                break;
            }
        }

        if (i >= FLEXBUS_LS_CH_NUM) {
            HAL_DBG_ERR("%s, cannot get Rx FIFO for LS_CH%lu!\n", __func__, lsCh);
            ret = HAL_ERROR;
        }
    }
#endif

    return ret;
}

/**
 * @brief  Set FLEXBUS Tx FIFO num.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_SetTxFifoNum(struct FLEXBUS_HANDLE *pFB)
{
#if (FLEXBUS_LARGE_VERSION_NUMBER >= 2)
    uint32_t val;
    uint32_t i;

    HAL_ASSERT(pFB != NULL);

    val = READ_REG(pFB->pReg->TXFIFO_REGION);
    for (i = 0; i < FLEXBUS_LS_CH_NUM; i++) {
        if (FLEXBUS_FIFO_REGION(val, i) == 0) {
            break;
        }
    }

    WRITE_REG_MASK_WE(pFB->pReg->HS_LS_CTL, FLEXBUS_HS_LS_CTL_TXFIFO_USE_LS_NUM_MASK,
                      i << FLEXBUS_HS_LS_CTL_TXFIFO_USE_LS_NUM_SHIFT);
#endif

    return HAL_OK;
}

/**
 * @brief  Set FLEXBUS Rx FIFO num.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_SetRxFifoNum(struct FLEXBUS_HANDLE *pFB)
{
#if (FLEXBUS_LARGE_VERSION_NUMBER >= 2)
    uint32_t val;
    uint32_t i;

    HAL_ASSERT(pFB != NULL);

    val = READ_REG(pFB->pReg->RXFIFO_REGION);
    for (i = 0; i < FLEXBUS_LS_CH_NUM; i++) {
        if (FLEXBUS_FIFO_REGION(val, i) == 0) {
            break;
        }
    }

    WRITE_REG_MASK_WE(pFB->pReg->HS_LS_CTL, FLEXBUS_HS_LS_CTL_RXFIFO_USE_LS_NUM_MASK,
                      i << FLEXBUS_HS_LS_CTL_RXFIFO_USE_LS_NUM_SHIFT);
#endif

    return HAL_OK;
}

/** @} */

/** @defgroup FLEXBUS_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit module as follows:

 *  @{
 */

/**
 * @brief  Initialize the FLEXBUS.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @param  pReg: FLEXBUS reg base.
 * @param  pOpMode: FLEXBUS opmode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_Init(struct FLEXBUS_HANDLE *pFB, struct FLEXBUS_REG *pReg,
                            const struct HAL_FLEXBUS_OPMODE *pOpMode)
{
    HAL_ASSERT(pFB != NULL);

    pFB->pReg = pReg;
    HAL_ASSERT(IS_FLEXBUS_INSTANCE(pFB->pReg));
#if (FLEXBUS_LARGE_VERSION_NUMBER >= 2)
    pFB->pRmioReg = (struct FLEXBUS_RMIO_REG *)((void *)pReg + 0x3000);
    HAL_ASSERT(IS_FLEXBUS_RMIO_INSTANCE(pFB->pRmioReg));
#endif

    pFB->pOpMode = pOpMode;

    HAL_FLEXBUS_InitConfig(pFB);

    if (pFB->pOpMode->opMode0 != FLEXBUS0_OPMODE_NULL &&
        pFB->pOpMode->opMode1 != FLEXBUS1_OPMODE_NULL) {
        WRITE_REG(pFB->pReg->COM_CTL, FLEXBUS_TX_AND_RX);
    } else if (pFB->pOpMode->opMode0 != FLEXBUS0_OPMODE_NULL) {
        WRITE_REG(pFB->pReg->COM_CTL, FLEXBUS_TX_ONLY);
    } else {
        WRITE_REG(pFB->pReg->COM_CTL, FLEXBUS_RX_ONLY);
    }

    pFB->pDfsReg = &s_flexbusDfsReg;

    return HAL_OK;
}

/**
 * @brief  DeInitialize the FLEXBUS.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DeInit(struct FLEXBUS_HANDLE *pFB)
{
    HAL_ASSERT(pFB != NULL);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED */
