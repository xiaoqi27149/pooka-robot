/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_EDPTX_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup EDPTX
 *  @{
 */

#ifndef _HAL_EDPTX_RK3288_H_
#define _HAL_EDPTX_RK3288_H_

/***************************** MACRO Definition ******************************/
/** @defgroup EDPTX_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/
/**
  * @brief EDPTX pll status
  */
typedef enum {
    PLL_UNLOCKED,
    PLL_LOCKED
} eEDPTX_pllStatus;

/**
  * @brief EDPTX analog power block type
  */
typedef enum {
    AUX_BLOCK,
    CH0_BLOCK,
    CH1_BLOCK,
    CH2_BLOCK,
    CH3_BLOCK,
    ANALOG_TOTAL,
    POWER_ALL
} eEDPTX_analogPowerBlock;

/**
  * @brief EDPTX training pattern type
  */
typedef enum {
    PRBS7,
    D10_2,
    TRAINING_PTN1,
    TRAINING_PTN2,
    TRAINING_PTN3,
    TEST_PATTERN_80BIT,
    TEST_PATTERN_HBR2,
    DP_NONE
} eEDPTX_patternSet;

/**
  * @brief EDPTX clock recovery M value type
  */
typedef enum {
    CALCULATED_M,
    REGISTER_M
} eEDPTX_clockRecoveryMValueType;

/**
  * @brief EDPTX video timing recognition type
  */
typedef enum {
    VIDEO_TIMING_FROM_CAPTURE,
    VIDEO_TIMING_FROM_REGISTER
} eEDPTX_videoTimingRecognitionType;

/**
  * @brief EDPTX AUX status
  */
typedef enum {
    AUX_STATUS_OK,
    AUX_STATUS_NACK_ERROR,
    AUX_STATUS_TIMEOUT_ERROR,
    AUX_STATUS_UNKNOWN_ERROR,
    AUX_STATUS_MUCH_DEFER_ERROR,
    AUX_STATUS_TX_SHORT_ERROR,
    AUX_STATUS_RX_SHORT_ERROR,
    AUX_STATUS_NACK_WITHOUT_M_ERROR,
    AUX_STATUS_I2C_NACK_ERROR
} eEDPTX_auxStatus;

/**
  * @brief EDPTX voltage level
  */
typedef enum {
    VOLTAGE_LEVEL_0,
    VOLTAGE_LEVEL_1,
    VOLTAGE_LEVEL_2,
    VOLTAGE_LEVEL_3,
} eEDPTX_voltageSwingLevel;

/**
  * @brief EDPTX pre emphasis level
  */
typedef enum {
    PRE_EMPHASIS_LEVEL_0,
    PRE_EMPHASIS_LEVEL_1,
    PRE_EMPHASIS_LEVEL_2,
    PRE_EMPHASIS_LEVEL_3,
} eEDPTX_preEmphasisLevel;

/**
  * @brief EDPTX handle structure definition
  */
struct EDPTX_HANDLE {
    struct EDPTX_REG *pReg;
    uint32_t clkReset;
    uint32_t apbClkReset;
};

/**
  * @brief EDPTX HW information definition
  */
struct HAL_EDPTX_DEV {
    struct EDPTX_REG *pReg;
    uint32_t clkReset;
    uint32_t apbClkReset;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup EDPTX_Public_Function_Declare Public Function Declare
 *  @{
 */
void HAL_EDPTX_EnableVideoMute(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_DisableVideoMute(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_StopVideo(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_InitAnalogParam(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_InitInterrupt(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_Reset(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_SwReset(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_ConfigInterrupt(struct EDPTX_HANDLE *pEDPTX, HAL_Check forceHpd);
void HAL_EDPTX_MuteHpdInterrupt(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_UnmuteHpdInterrupt(struct EDPTX_HANDLE *pEDPTX);
eEDPTX_pllStatus HAL_EDPTX_GetPllLockStatus(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_SetPllPowerDown(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_SetPllPowerUp(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_SetAnalogPowerDown(struct EDPTX_HANDLE *pEDPTX, eEDPTX_analogPowerBlock block);
void HAL_EDPTX_SetAnalogPowerUp(struct EDPTX_HANDLE *pEDPTX, eEDPTX_analogPowerBlock block);
void HAL_EDPTX_InitAnalogFunc(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_ClearHotplugInterrupts(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_InitHpd(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_ForceHpd(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_ResetAux(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_InitAux(struct EDPTX_HANDLE *pEDPTX);
HAL_Status HAL_EDPTX_GetPlugInStatus(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_EnableSwFunction(struct EDPTX_HANDLE *pEDPTX);
HAL_Status HAL_EDPTX_StartAuxTransaction(struct EDPTX_HANDLE *pEDPTX);
HAL_Status HAL_EDPTX_SelectI2cDevice(struct EDPTX_HANDLE *pEDPTX, uint32_t deviceAddr, uint32_t regAddr);
HAL_Status HAL_EDPTX_ReadByteFromDpcd(struct EDPTX_HANDLE *pEDPTX, uint32_t regAddr, uint8_t *data);
HAL_Status HAL_EDPTX_ReadBytesFromDpcd(struct EDPTX_HANDLE *pEDPTX, uint32_t regAddr, uint32_t count, uint8_t *data);
HAL_Status HAL_EDPTX_WriteByteToDpcd(struct EDPTX_HANDLE *pEDPTX, uint32_t regAddr, uint8_t data);
HAL_Status HAL_EDPTX_WriteBytesToDpcd(struct EDPTX_HANDLE *pEDPTX, uint32_t regAddr, uint32_t count, uint8_t *data);
void HAL_EDPTX_SetLinkBandwidth(struct EDPTX_HANDLE *pEDPTX, uint8_t bwType);
void HAL_EDPTX_GetLinkBandwidth(struct EDPTX_HANDLE *pEDPTX, uint8_t *bwType);
void HAL_EDPTX_SetLaneCount(struct EDPTX_HANDLE *pEDPTX, uint32_t count);
void HAL_EDPTX_GetLaneCount(struct EDPTX_HANDLE *pEDPTX, uint8_t *count);
void HAL_EDPTX_SetLaneLinkTraining(struct EDPTX_HANDLE *pEDPTX, uint8_t lane, uint8_t laneCfg);
uint32_t HAL_EDPTX_GetLaneLinkTraining(struct EDPTX_HANDLE *pEDPTX, uint8_t lane);
void HAL_EDPTX_EnableEnhancedMode(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_DisableEnhancedMode(struct EDPTX_HANDLE *pEDPTX);
HAL_Check HAL_EDPTX_IsEnhancedMode(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_SetTrainingPattern(struct EDPTX_HANDLE *pEDPTX, eEDPTX_patternSet pattern);
void HAL_EDPTX_ResetMacro(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_InitVideo(struct EDPTX_HANDLE *pEDPTX, HAL_Check forceStreamValid);
void HAL_EDPTX_SetVideoColorFormat(struct EDPTX_HANDLE *pEDPTX, uint8_t dynamicRange, uint8_t colorDepth,
                                   uint8_t colorSpace, uint8_t ycbcrCoeff);
HAL_Status HAL_EDPTX_IsSlaveVideoStreamClockOn(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_SetVideoCrMn(struct EDPTX_HANDLE *pEDPTX, eEDPTX_clockRecoveryMValueType type, uint32_t mValue, uint32_t nValue);
void HAL_EDPTX_SetVideoTimingMode(struct EDPTX_HANDLE *pEDPTX, eEDPTX_videoTimingRecognitionType type);
void HAL_EDPTX_EnableVideoMaster(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_DisableVideoMaster(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_StartVideo(struct EDPTX_HANDLE *pEDPTX);
HAL_Status HAL_EDPTX_IsVideoStreamOn(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_ConfigVideoSlaveMode(struct EDPTX_HANDLE *pEDPTX, HAL_Check interlaced,
                                    HAL_Check vsyncPolarity, HAL_Check hsyncPolarity);
void HAL_EDPTX_EnableScrambling(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_DisableScrambling(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_SetVideoFormat(struct EDPTX_HANDLE *pEDPTX, const struct DISPLAY_MODE_INFO *mode,
                              HAL_Check interlaced, HAL_Check vsyncPolarity, HAL_Check hsyncPolarity);
void HAL_EDPTX_VideoBistEnable(struct EDPTX_HANDLE *pEDPTX);
int32_t HAL_EDPTX_AuxTransfer(struct EDPTX_HANDLE *pEDPTX, struct DP_AUX *aux, struct DP_AUX_MSG *msg);
void HAL_EDPTX_EnableAssrMode(struct EDPTX_HANDLE *pEDPTX);
void HAL_EDPTX_DisableAssrMode(struct EDPTX_HANDLE *pEDPTX);
HAL_Check HAL_EDPTX_IsAssrMode(struct EDPTX_HANDLE *pEDPTX);
HAL_Status HAL_EDPTX_Init(struct EDPTX_HANDLE *pEDPTX, const struct HAL_EDPTX_DEV *dev);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_EDPTX_MODULE_ENABLED */
