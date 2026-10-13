/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_FLEXBUS_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS
 *  @{
 */

#ifndef __HAL_FLEXBUS_H
#define __HAL_FLEXBUS_H

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup FLEXBUS_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/* FLEXBUS Large version number */
#define FLEXBUS_LARGE_VERSION_NUMBER ((FLEXBUS_REVISION >> 24) & 0xff)

/* Macro for FLEXBUS_LS and FLEXBUS_RMIO */
#if defined(RKMCU_RK2116)
#define FLEXBUS_LS_CH_NUM 2
#define FLEXBUS_RMIO_NUM  6
#else
#define FLEXBUS_LS_CH_NUM 0
#define FLEXBUS_RMIO_NUM  0
#endif

/* FLEXBUS tx_rx_mode */
#define FLEXBUS_TX_AND_RX  (0x0 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)
#define FLEXBUS_TX_ONLY    (0x1 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)
#define FLEXBUS_RX_ONLY    (0x2 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)
#define FLEXBUS_TX_THEN_RX (0x3 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)

/* FLEXBUS ls_rd/wr_use_bandwidth */
#define FLEXBUS_LS_OCCUPY_0_BANDWIDTH  0x0
#define FLEXBUS_LS_OCCUPY_25_BANDWIDTH 0x1
#define FLEXBUS_LS_OCCUPY_37_BANDWIDTH 0x2
#define FLEXBUS_LS_OCCUPY_50_BANDWIDTH 0x3

/* FLEXBUS TX/RXFIFO_REGION */
#define FLEXBUS_FIFO_REGION(val, n) (((val) >> ((n) * 4)) & 0xf)

/* FLEXBUS_RMIO select */
#define FLEXBUS_LS_PIN_SEL_NO_PIN          0
#define FLEXBUS_LS_PIN_SEL_CH_SIG(ch, sig) ((ch) * 4 + (sig) + 1)
#define FLEXBUS_LS_PIN_SEL_MAX             FLEXBUS_LS_PIN_SEL_CH_SIG(FLEXBUS_LS_CH_NUM, 3)

/***************************** Structure Definition **************************/

/** @brief  FLEXBUS0 opmode definition */
typedef enum {
    FLEXBUS0_OPMODE_NULL = 0,
    FLEXBUS0_OPMODE_DAC,
    FLEXBUS0_OPMODE_SPI,
    FLEXBUS0_OPMODE_MAX
} eFLEXBUS0_OpMode;

/** @brief  FLEXBUS1 opmode definition */
typedef enum {
    FLEXBUS1_OPMODE_NULL = 0,
    FLEXBUS1_OPMODE_ADC,
    FLEXBUS1_OPMODE_CIF,
    FLEXBUS1_OPMODE_MAX
} eFLEXBUS1_OpMode;

/** @brief  FLEXBUS_LS opmode definition */
typedef enum {
    FLEXBUS_LS_OPMODE_NULL = 0,
    FLEXBUS_LS_OPMODE_UART,
    FLEXBUS_LS_OPMODE_SPI,
    FLEXBUS_LS_OPMODE_I2C,
    FLEXBUS_LS_OPMODE_I2S,
    FLEXBUS_LS_OPMODE_MAX
} eFLEXBUS_LS_OpMode;

/** @brief  FLEXBUS opmode definition */
struct HAL_FLEXBUS_OPMODE {
    eFLEXBUS0_OpMode opMode0;
    eFLEXBUS1_OpMode opMode1;
#if (FLEXBUS_LARGE_VERSION_NUMBER >= 2)
    eFLEXBUS_LS_OpMode opModeLs[FLEXBUS_LS_CH_NUM];
#endif
};

/** @brief  FLEXBUS HW information definition */
struct HAL_FLEXBUS_DEV {
    struct FLEXBUS_REG *pReg;
    eCLOCK_Name txclkID;
    eCLOCK_Name rxclkID;
    uint32_t txclkGateID;
    uint32_t rxclkGateID;
    uint32_t aclkGateID;
    uint32_t hclkGateID;
    IRQn_Type irqNum;
    struct HAL_FLEXBUS_OPMODE opMode;
};

/** @brief  FLEXBUS DFS reg definition */
struct HAL_FLEXBUS_DFS_REG {
    uint32_t dfs1Bit;
    uint32_t dfs2Bit;
    uint32_t dfs4Bit;
    uint32_t dfs8Bit;
    uint32_t dfs16Bit;
    uint32_t dfsMask;
};

/** @brief  FLEXBUS Configuration Structure definition */
struct FLEXBUS_HANDLE {
    struct FLEXBUS_REG *pReg;
#if (FLEXBUS_LARGE_VERSION_NUMBER >= 2)
    struct FLEXBUS_RMIO_REG *pRmioReg;
#endif
    const struct HAL_FLEXBUS_OPMODE *pOpMode;
    const struct HAL_FLEXBUS_DFS_REG *pDfsReg;
    uint32_t txFifoNum;
    uint32_t rxFifoNum;
};

/** @} */

/***************************** Function Declare ******************************/
/** @defgroup FLEXBUS_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_FLEXBUS_SetRmio(struct FLEXBUS_HANDLE *pFB, uint32_t rmio, uint32_t pinSel);
HAL_Status HAL_FLEXBUS_SetFifoRegion(struct FLEXBUS_HANDLE *pFB, uint32_t lsCh, bool txEn,
                                     bool rxEn);
HAL_Status HAL_FLEXBUS_SetLsFifo(struct FLEXBUS_HANDLE *pFB, uint32_t lsCh, bool txEn, bool rxEn);
HAL_Status HAL_FLEXBUS_SetTxFifoNum(struct FLEXBUS_HANDLE *pFB);
HAL_Status HAL_FLEXBUS_SetRxFifoNum(struct FLEXBUS_HANDLE *pFB);
HAL_Status HAL_FLEXBUS_Init(struct FLEXBUS_HANDLE *pFB, struct FLEXBUS_REG *pReg,
                            const struct HAL_FLEXBUS_OPMODE *pOpMode);
HAL_Status HAL_FLEXBUS_DeInit(struct FLEXBUS_HANDLE *pFB);
void HAL_FLEXBUS_InitConfig(struct FLEXBUS_HANDLE *pFB);
void HAL_FLEXBUS_GrfConfig(struct FLEXBUS_HANDLE *pFB, bool slaveMode, bool cPol, bool cPha);

/** @} */

#endif

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED */
