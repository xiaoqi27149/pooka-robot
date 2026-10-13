/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_EDPTX_MODULE_ENABLED) && defined(SOC_RK3576)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup EDPTX
 *  @{
 */

/** @defgroup EDPTX_How_To_Use How To Use
 *  @{

 - Initialize the eDP interface:
     - Invoke HAL_EDPTX_Init() to initialize global parameters
     - Invoke HAL_EDPTX_Reset() and HAL_EDPTX_SwReset() to reset the whole edp module.
     - Invoke HAL_EDPTX_InitAnalogParam() for analog parameters.
     - Invoke HAL_EDPTX_InitInterrupt() to initialize the interrupts.
     - Invoke HAL_EDPTX_ConfigInterrupt() to configurate the interrupts.
     - Invoke HAL_EDPTX_InitAnalogFunc() to initialize the anlalog functions.
     - Invoke HAL_EDPTX_InitHpd() to initialize HPD.
         - (optionally)Use HAL_EDPTX_ForceHpd() to make HPD status always connected.
         - (optionally)Use HAL_EDPTX_MuteHpdInterrupt() / HAL_EDPTX_UnmuteHpdInterrupt() to disable/enable
           HPD interrupt.
         - (optionally)Use HAL_EDPTX_GetPlugInStatus() to check whether HPD status is connected.
         - (optionally)Use HAL_EDPTX_ClearHotplugInterrupts() to clear the HPD interrupt status.
     - Invoke HAL_EDPTX_InitAux() to initialize AUX.
         - (optionally)Use HAL_EDPTX_ResetAux() to reset the AUX.
     - Invoke HAL_EDPTX_SetAnalogPowerUp() / HAL_EDPTX_SetAnalogPowerDown() to power up/down the specific analog block.

 - AUX transmission:
     - Invoke HAL_EDPTX_ReadByteFromDpcd() to read one DPCD.
     - Invoke HAL_EDPTX_ReadBytesFromDpcd() to read several DPCDs.
     - Invoke HAL_EDPTX_WriteByteToDpcd() to write one DPCD.
     - Invoke HAL_EDPTX_WriteBytesToDpcd() to write several DPCDs.
     - Invoke HAL_EDPTX_SelectI2cDevice() to select external I2C device.
     - Invoke HAL_EDPTX_StartAuxTransaction() to start aux transmission.
     - (optionally)Use HAL_EDPTX_AuxTransfer() to apply aux transmission.

 - Link training:
     - Invoke HAL_EDPTX_SetLinkBandwidth() to set the link rate.
     - Invoke HAL_EDPTX_GetLinkBandwidth() to get the link rate.
     - Invoke HAL_EDPTX_SetLaneCount() to set the lane count.
     - Invoke HAL_EDPTX_GetLaneCount() to get the lane count.
     - Invoke HAL_EDPTX_SetLaneLinkTraining() to set PE/VS configurations.
     - Invoke HAL_EDPTX_GetLaneLinkTraining() to get PE/VS configurations.
     - Invoke HAL_EDPTX_SetTrainingPattern() to set the training pattern.

 - Enable the eDP video output:
     - Invoke HAL_EDPTX_EnableSwFunction() to enable SW defined functions.
     - Invoke HAL_EDPTX_InitVideo() to setup the initialzation before video output.
     - Invoke HAL_EDPTX_SetVideoFormat(), HAL_EDPTX_SetVideoTimingMode(), HAL_EDPTX_SetVideoCrMn() and
       HAL_EDPTX_ConfigVideoSlaveMode() to setup display mode.
     - Invoke HAL_EDPTX_SetVideoColorFormat() to setup the color format.
     - Invoke HAL_EDPTX_EnableVideoMute() / HAL_EDPTX_DisableVideoMute() to enable/disable the video mute mode.
     - Invoke HAL_EDPTX_StartVideo() to start the video output.
     - Invoke HAL_EDPTX_StopVideo() to stop the video output.
     - (optionally)Use HAL_EDPTX_IsSlaveVideoStreamClockOn() to check wheter stream clock is available.
     - (optionally)Use HAL_EDPTX_IsVideoStreamOn() to check wheter video stream is available.
     - (optionally)Use HAL_EDPTX_GetPllLockStatus() to check whether the pll is locked.

 - Optional features:
     - Bist mode
         - Invake HAL_EDPTX_VideoBistEnable() to enable bist mode for debug.

     - Enhanced mode
         - Invoke HAL_EDPTX_EnableEnhancedMode() / HAL_EDPTX_DisableEnhancedMode() to enable/disable enhanced mode.
             - (optionally)Use HAL_EDPTX_IsEnhancedMode() to check whether enhanced mode is enabled.

     - Scrambling
        - Invoke HAL_EDPTX_EnableScrambling() / HAL_EDPTX_DisableScrambling() to enable/disable Scrambling.

     - ASSR mode
         - Invoke HAL_EDPTX_EnableAssrMode() / HAL_EDPTX_DisableAssrMode() to enable ASSR mode.
             - (optionally)Use HAL_EDPTX_IsAssrMode() to check whether ASSR mode is enabled.

 @} */

/** @defgroup EDPTX_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
/* TX_SW_RESET */
#define RESET_DP_TX (0x1 << 0)

/* FUNC_EN_1 */
#define MASTER_VID_FUNC_EN_N  (0x1 << 7)
#define RK_VID_CAP_FUNC_EN_N  (0x1 << 6)
#define SLAVE_VID_FUNC_EN_N   (0x1 << 5)
#define RK_VID_FIFO_FUNC_EN_N (0x1 << 5)
#define AUD_FIFO_FUNC_EN_N    (0x1 << 4)
#define AUD_FUNC_EN_N         (0x1 << 3)
#define HDCP_FUNC_EN_N        (0x1 << 2)
#define CRC_FUNC_EN_N         (0x1 << 1)
#define SW_FUNC_EN_N          (0x1 << 0)

/* FUNC_EN_2 */
#define SSC_FUNC_EN_N           (0x1 << 7)
#define AUX_FUNC_EN_N           (0x1 << 2)
#define SERDES_FIFO_FUNC_EN_N   (0x1 << 1)
#define LS_CLK_DOMAIN_FUNC_EN_N (0x1 << 0)

/* VIDEO_CTL_1 */
#define VIDEO_EN        (0x1 << EDPTX_VIDEO_CTL_1_VIDEO_EN_SHIFT)
#define HDCP_VIDEO_MUTE (0x1 << EDPTX_VIDEO_CTL_1_VIDEO_MUTE_SHIFT)

/* VIDEO_CTL_1 */
#define IN_D_RANGE_MASK     (0x1 << 7)
#define IN_D_RANGE_SHIFT    (7)
#define IN_D_RANGE_CEA      (0x1 << 7)
#define IN_D_RANGE_VESA     (0x0 << 7)
#define IN_BPC_MASK         (0x7 << 4)
#define IN_BPC_SHIFT        (4)
#define IN_BPC_12_BITS      (0x3 << 4)
#define IN_BPC_10_BITS      (0x2 << 4)
#define IN_BPC_8_BITS       (0x1 << 4)
#define IN_BPC_6_BITS       (0x0 << 4)
#define IN_COLOR_F_MASK     (0x3 << 0)
#define IN_COLOR_F_SHIFT    (0)
#define IN_COLOR_F_YCBCR444 (0x2 << 0)
#define IN_COLOR_F_YCBCR422 (0x1 << 0)
#define IN_COLOR_F_RGB      (0x0 << 0)

/* VIDEO_CTL_3 */
#define IN_YC_COEFFI_MASK         (0x1 << 7)
#define IN_YC_COEFFI_SHIFT        (7)
#define IN_YC_COEFFI_ITU709       (0x1 << 7)
#define IN_YC_COEFFI_ITU601       (0x0 << 7)
#define VID_CHK_UPDATE_TYPE_MASK  (0x1 << 4)
#define VID_CHK_UPDATE_TYPE_SHIFT (4)
#define VID_CHK_UPDATE_TYPE_1     (0x1 << 4)
#define VID_CHK_UPDATE_TYPE_0     (0x0 << 4)
#define REUSE_SPD_EN              (0x1 << 3)

/* VIDEO_CTL_4 */
#define BIST_EN       (0x1 << 3)
#define BIST_WIDTH(x) (((x) & 0x1) << 2)
#define BIST_TYPE(x)  (((x) & 0x3) << 0)

/* VIDEO_CTL_8 */
#define VID_HRES_TH(x) (((x) & 0xf) << 4)
#define VID_VRES_TH(x) (((x) & 0xf) << 0)

/* VIDEO_CTL_10 */
#define FORMAT_SEL         (0x1 << 4)
#define INTERACE_SCAN_CFG  (0x1 << 2)
#define VSYNC_POLARITY_CFG (0x1 << 1)
#define HSYNC_POLARITY_CFG (0x1 << 0)

/* TOTAL_LINE_CFG_L */
#define TOTAL_LINE_CFG_L(x) (((x) & 0xff) << 0)

/* TOTAL_LINE_CFG_H */
#define TOTAL_LINE_CFG_H(x) (((x) & 0xf) << 0)

/* ACTIVE_LINE_CFG_L */
#define ACTIVE_LINE_CFG_L(x) (((x) & 0xff) << 0)

/* ACTIVE_LINE_CFG_H */
#define ACTIVE_LINE_CFG_H(x) (((x) & 0xf) << 0)

/* V_F_PORCH_CFG */
#define V_F_PORCH_CFG(x) (((x) & 0xff) << 0)

/* V_SYNC_WIDTH_CFG */
#define V_SYNC_WIDTH_CFG(x) (((x) & 0xff) << 0)

/* V_B_PORCH_CFG */
#define V_B_PORCH_CFG(x) (((x) & 0xff) << 0)

/* TOTAL_PIXEL_CFG_L */
#define TOTAL_PIXEL_CFG_L(x) (((x) & 0xff) << 0)

/* TOTAL_PIXEL_CFG_H */
#define TOTAL_PIXEL_CFG_H(x) (((x) & 0x3f) << 0)

/* ACTIVE_PIXEL_CFG_L */
#define ACTIVE_PIXEL_CFG_L(x) (((x) & 0xff) << 0)

/* ACTIVE_PIXEL_CFG_H */
#define ACTIVE_PIXEL_CFG_H(x) (((x) & 0x3f) << 0)

/* H_F_PORCH_CFG_L */
#define H_F_PORCH_CFG_L(x) (((x) & 0xff) << 0)

/* H_F_PORCH_CFG_H */
#define H_F_PORCH_CFG_H(x) (((x) & 0xf) << 0)

/* H_SYNC_CFG_L */
#define H_SYNC_CFG_L(x) (((x) & 0xff) << 0)

/* H_SYNC_CFG_H */
#define H_SYNC_CFG_H(x) (((x) & 0xf) << 0)

/* H_B_PORCH_CFG_L */
#define H_B_PORCH_CFG_L(x) (((x) & 0xff) << 0)

/* H_B_PORCH_CFG_H */
#define H_B_PORCH_CFG_H(x) (((x) & 0xf) << 0)

/* SPDIF_AUDIO_CTL_0 */
#define AUD_SPDIF_EN (0x1 << 7)

/* PLL_REG_1 */
#define REF_CLK_24M  (0x1 << 0)
#define REF_CLK_27M  (0x0 << 0)
#define REF_CLK_MASK (0x1 << 0)

/* PSR_FRAME_UPDATE_CTRL */
#define PSR_FRAME_UP_TYPE_BURST  (0x1 << 0)
#define PSR_FRAME_UP_TYPE_SINGLE (0x0 << 0)
#define PSR_CRC_SEL_HARDWARE     (0x1 << 1)
#define PSR_CRC_SEL_MANUALLY     (0x0 << 1)

/* LANE_MAP */
#define LANE3_MAP_LOGIC_LANE_0 (0x0 << 6)
#define LANE3_MAP_LOGIC_LANE_1 (0x1 << 6)
#define LANE3_MAP_LOGIC_LANE_2 (0x2 << 6)
#define LANE3_MAP_LOGIC_LANE_3 (0x3 << 6)
#define LANE2_MAP_LOGIC_LANE_0 (0x0 << 4)
#define LANE2_MAP_LOGIC_LANE_1 (0x1 << 4)
#define LANE2_MAP_LOGIC_LANE_2 (0x2 << 4)
#define LANE2_MAP_LOGIC_LANE_3 (0x3 << 4)
#define LANE1_MAP_LOGIC_LANE_0 (0x0 << 2)
#define LANE1_MAP_LOGIC_LANE_1 (0x1 << 2)
#define LANE1_MAP_LOGIC_LANE_2 (0x2 << 2)
#define LANE1_MAP_LOGIC_LANE_3 (0x3 << 2)
#define LANE0_MAP_LOGIC_LANE_0 (0x0 << 0)
#define LANE0_MAP_LOGIC_LANE_1 (0x1 << 0)
#define LANE0_MAP_LOGIC_LANE_2 (0x2 << 0)
#define LANE0_MAP_LOGIC_LANE_3 (0x3 << 0)

/* ANALOG_CTL_1 */
#define TX_TERMINAL_CTRL_50_OHM (0x1 << 4)

/* ANALOG_CTL_2 */
#define SEL_24M             (0x1 << 3)
#define TX_DVDD_BIT_1_0625V (0x4 << 0)

/* ANALOG_CTL_3 */
#define DRIVE_DVDD_BIT_1_0625V (0x4 << 5)
#define VCO_BIT_600_MICRO      (0x5 << 0)

/* PLL_FILTER_CTL_1 */
#define PD_RING_OSC              (0x1 << 6)
#define AUX_TERMINAL_CTRL_50_OHM (0x2 << 4)
#define TX_CUR1_2X               (0x1 << 2)
#define TX_CUR_16_MA             (0x3 << 0)

/* TX_AMP_TUNING_CTL */
#define CH3_AMP_400_MV (0x0 << 24)
#define CH2_AMP_400_MV (0x0 << 16)
#define CH1_AMP_400_MV (0x0 << 8)
#define CH0_AMP_400_MV (0x0 << 0)

/* AUX_HW_RETRY_CTL */
#define AUX_BIT_PERIOD_EXPECTED_DELAY(x)        (((x) & 0x7) << 8)
#define AUX_HW_RETRY_INTERVAL_MASK              (0x3 << 3)
#define AUX_HW_RETRY_INTERVAL_600_MICROSECONDS  (0x0 << 3)
#define AUX_HW_RETRY_INTERVAL_800_MICROSECONDS  (0x1 << 3)
#define AUX_HW_RETRY_INTERVAL_1000_MICROSECONDS (0x2 << 3)
#define AUX_HW_RETRY_INTERVAL_1800_MICROSECONDS (0x3 << 3)
#define AUX_HW_RETRY_COUNT_SEL(x)               (((x) & 0x7) << 0)

/* COMMON_INT_STA_1 */
#define VSYNC_DET      (0x1 << 7)
#define PLL_LOCK_CHG   (0x1 << 6)
#define SPDIF_ERR      (0x1 << 5)
#define SPDIF_UNSTBL   (0x1 << 4)
#define VID_FORMAT_CHG (0x1 << 3)
#define AUD_CLK_CHG    (0x1 << 2)
#define VID_CLK_CHG    (0x1 << 1)
#define SW_INT         (0x1 << 0)

/* COMMON_INT_STA_2 */
#define ENC_EN_CHG        (0x1 << 6)
#define HW_BKSV_RDY       (0x1 << 3)
#define HW_SHA_DONE       (0x1 << 2)
#define HW_AUTH_STATE_CHG (0x1 << 1)
#define HW_AUTH_DONE      (0x1 << 0)

/* COMMON_INT_STA_3 */
#define AFIFO_UNDER (0x1 << 7)
#define AFIFO_OVER  (0x1 << 6)
#define R0_CHK_FLAG (0x1 << 5)

/* COMMON_INT_STA_4 */
#define PSR_ACTIVE         (0x1 << 7)
#define PSR_INACTIVE       (0x1 << 6)
#define SPDIF_BI_PHASE_ERR (0x1 << 5)
#define HOTPLUG_CHG        (0x1 << 2)
#define HPD_LOST           (0x1 << 1)
#define PLUG               (0x1 << 0)

/* INT_STA */
#define INT_HPD            (0x1 << 6)
#define HW_TRAINING_FINISH (0x1 << 5)
#define RPLY_RECEIV        (0x1 << 1)
#define AUX_ERR            (0x1 << 0)

/* INT_CTL */
#define SOFT_INT_CTRL (0x1 << 2)
#define INT_POL1      (0x1 << 1)
#define INT_POL0      (0x1 << 0)

/* SYS_CTL_1 */
#define DET_STA   (0x1 << 2)
#define FORCE_DET (0x1 << 1)
#define DET_CTRL  (0x1 << 0)

/* SYS_CTL_2 */
#define CHA_CRI(x) (((x) & 0xf) << 4)
#define CHA_STA    (0x1 << 2)
#define FORCE_CHA  (0x1 << 1)
#define CHA_CTRL   (0x1 << 0)

/* SYS_CTL_3 */
#define HPD_STATUS (0x1 << 6)
#define F_HPD      (0x1 << 5)
#define HPD_CTRL   (0x1 << 4)
#define HDCP_RDY   (0x1 << 3)
#define STRM_VALID (0x1 << 2)
#define F_VALID    (0x1 << 1)
#define VALID_CTRL (0x1 << 0)

/* SYS_CTL_4 */
#define FIX_M_AUD         (0x1 << 4)
#define ENHANCED          (0x1 << 3)
#define FIX_M_VID         (0x1 << 2)
#define M_VID_UPDATE_CTRL (0x3 << 0)

/* AUD_CTL */
#define MISC_CTRL_RESET (0x1 << 4)
#define DP_AUDIO_EN     (0x1 << 0)

/* TRAINING_PTN_SET */
#define SCRAMBLER_TYPE                 (0x1 << 9)
#define HW_LINK_TRAINING_PATTERN       (0x1 << 8)
#define SCRAMBLING_DISABLE             (0x1 << 5)
#define SCRAMBLING_ENABLE              (0x0 << 5)
#define LINK_QUAL_PATTERN_SET_MASK     (0x7 << 2)
#define LINK_QUAL_PATTERN_SET_HBR2     (0x5 << 2)
#define LINK_QUAL_PATTERN_SET_80BIT    (0x4 << 2)
#define LINK_QUAL_PATTERN_SET_PRBS7    (0x3 << 2)
#define LINK_QUAL_PATTERN_SET_D10_2    (0x1 << 2)
#define LINK_QUAL_PATTERN_SET_DISABLE  (0x0 << 2)
#define SW_TRAINING_PATTERN_SET_MASK   (0x3 << 0)
#define SW_TRAINING_PATTERN_SET_PTN3   (0x3 << 0)
#define SW_TRAINING_PATTERN_SET_PTN2   (0x2 << 0)
#define SW_TRAINING_PATTERN_SET_PTN1   (0x1 << 0)
#define SW_TRAINING_PATTERN_SET_NORMAL (0x0 << 0)

/* LN0_LINK_TRAINING_CTL */
#define PRE_EMPHASIS_SET_MASK  (0x3 << 3)
#define PRE_EMPHASIS_SET_SHIFT (3)

/* DEBUG_CTL */
#define PLL_LOCK      (0x1 << 4)
#define F_PLL_LOCK    (0x1 << 3)
#define PLL_LOCK_CTRL (0x1 << 2)
#define PN_INV        (0x1 << 0)

/* PLL_CTL */
#define DP_PLL_PD               (0x1 << 7)
#define DP_PLL_RESET            (0x1 << 6)
#define DP_PLL_LOOP_BIT_DEFAULT (0x1 << 4)
#define DP_PLL_REF_BIT_1_1250V  (0x5 << 0)
#define DP_PLL_REF_BIT_1_2500V  (0x7 << 0)

/* PHY_PD */
#define DP_INC_BG (0x1 << 7)
#define DP_EXP_BG (0x1 << 6)
#define DP_PHY_PD (0x1 << 5)
#define RK_AUX_PD (0x1 << 5)
#define AUX_PD    (0x1 << 4)
#define RK_PLL_PD (0x1 << 4)
#define CH3_PD    (0x1 << 3)
#define CH2_PD    (0x1 << 2)
#define CH1_PD    (0x1 << 1)
#define CH0_PD    (0x1 << 0)
#define DP_ALL_PD (0xff)

/* PHY_TEST */
#define MACRO_RST (0x1 << 5)
#define CH1_TEST  (0x1 << 1)
#define CH0_TEST  (0x1 << 0)

/* AUX_CH_STA */
#define AUX_BUSY        (0x1 << 4)
#define AUX_STATUS_MASK (0xf << 0)

/* AUX_CH_DEFER_CTL */
#define DEFER_CTRL_EN  (0x1 << 7)
#define DEFER_COUNT(x) (((x) & 0x7f) << 0)

/* AUX_RX_COMM */
#define AUX_RX_COMM_I2C_DEFER (0x2 << 2)
#define AUX_RX_COMM_AUX_DEFER (0x2 << 0)

/* BUFFER_DATA_CTL */
#define BUF_CLR           (0x1 << 7)
#define BUF_DATA_COUNT(x) (((x) & 0x1f) << 0)

/* AUX_CH_CTL_1 */
#define AUX_LENGTH(x)               (((x - 1) & 0xf) << 4)
#define AUX_TX_COMM_MASK            (0xf << 0)
#define AUX_TX_COMM_DP_TRANSACTION  (0x1 << 3)
#define AUX_TX_COMM_I2C_TRANSACTION (0x0 << 3)
#define AUX_TX_COMM_MOT             (0x1 << 2)
#define AUX_TX_COMM_WRITE           (0x0 << 0)
#define AUX_TX_COMM_READ            (0x1 << 0)

/* AUX_ADDR_7_0 */
#define AUX_ADDR_7_0(x) (((x) >> 0) & 0xff)

/* AUX_ADDR_15_8 */
#define AUX_ADDR_15_8(x) (((x) >> 8) & 0xff)

/* AUX_ADDR_19_16 */
#define AUX_ADDR_19_16(x) (((x) >> 16) & 0x0f)

/* AUX_CH_CTL_2 */
#define ADDR_ONLY (0x1 << 1)
#define AUX_EN    (0x1 << 0)

/* SOC_GENERAL_CTL */
#define AUDIO_MODE_SPDIF_MODE     (0x1 << 8)
#define AUDIO_MODE_MASTER_MODE    (0x0 << 8)
#define MASTER_VIDEO_INTERLACE_EN (0x1 << 4)
#define VIDEO_MASTER_CLK_SEL      (0x1 << 2)
#define VIDEO_MASTER_MODE_EN      (0x1 << 1)
#define VIDEO_MODE_MASK           (0x1 << 0)
#define VIDEO_MODE_SLAVE_MODE     (0x1 << 0)
#define VIDEO_MODE_MASTER_MODE    (0x0 << 0)

/* AUD_CHANNEL_CTL */
#define AUD_CHANNEL_COUNT_6 (0x5 << 0)
#define AUD_CHANNEL_COUNT_4 (0x3 << 0)
#define AUD_CHANNEL_COUNT_2 (0x1 << 0)

/* PKT_SEND_CTL */
#define IF_UP (0x1 << 4)
#define IF_EN (0x1 << 0)

/* CRC_CON */
#define PSR_VID_CRC_FLUSH  (0x1 << 2)
#define PSR_VID_CRC_ENABLE (0x1 << 0)

/* I2S_CTRL */
#define I2S_EN (0x1 << 4)

/* LINK_POLICY */
#define ALTERNATE_SR_ENABLE (0x1 << 7)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
static void HAL_EDPTX_SetLaneMap(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg = pEDPTX->pReg;
    uint32_t reg = 0;

    reg = LANE3_MAP_LOGIC_LANE_3 | LANE2_MAP_LOGIC_LANE_2 |
          LANE1_MAP_LOGIC_LANE_1 | LANE0_MAP_LOGIC_LANE_0;

    WRITE_REG(pReg->LANE_MAP, reg);
}

/********************* Public Function Definition ***************************/
/**
 * @brief  Stop the video.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_StopVideo(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->VIDEO_CTL_1);
    reg &= ~VIDEO_EN;
    WRITE_REG(pReg->VIDEO_CTL_1, reg);
}

/**
 * @brief  Enable the video mute.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_EnableVideoMute(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->VIDEO_CTL_1);
    reg |= HDCP_VIDEO_MUTE;
    WRITE_REG(pReg->VIDEO_CTL_1, reg);
}

/**
 * @brief  Disable the video mute.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_DisableVideoMute(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->VIDEO_CTL_1);
    reg &= ~HDCP_VIDEO_MUTE;
    WRITE_REG(pReg->VIDEO_CTL_1, reg);
}

/**
 * @brief  Initialize the analog parameters.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_InitAnalogParam(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = TX_TERMINAL_CTRL_50_OHM;
    WRITE_REG(pReg->RESERVED0360[4], reg);

    reg = SEL_24M | TX_DVDD_BIT_1_0625V;
    WRITE_REG(pReg->RESERVED0360[5], reg);

    reg = REF_CLK_24M;
    WRITE_REG(pReg->RESERVED00FC[0], reg);

    reg = DRIVE_DVDD_BIT_1_0625V | VCO_BIT_600_MICRO;
    WRITE_REG(pReg->RESERVED0360[6], reg);

    reg = PD_RING_OSC | AUX_TERMINAL_CTRL_50_OHM |
          TX_CUR1_2X | TX_CUR_16_MA;
    WRITE_REG(pReg->RESERVED0360[7], reg);

    reg = CH3_AMP_400_MV | CH2_AMP_400_MV |
          CH1_AMP_400_MV | CH0_AMP_400_MV;
    WRITE_REG(pReg->RESERVED0360[8], reg);
}

/**
 * @brief  Initialize the interrupt configurations.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_InitInterrupt(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* Set interrupt pin assertion polarity as high */
    WRITE_REG(pReg->INT_CTL, INT_POL1 | INT_POL0);

    /* Clear pending regisers */
    WRITE_REG(pReg->COMMON_INT_STA_1, 0xff);
    WRITE_REG(pReg->COMMON_INT_STA_2, 0x4f);
    WRITE_REG(pReg->COMMON_INT_STA_3, 0xe0);
    WRITE_REG(pReg->COMMON_INT_STA_4, 0xe7);
    WRITE_REG(pReg->DP_INT_STA, 0x63);

    /* 0:mask,1: unmask */
    WRITE_REG(pReg->COMMON_INT_MASK_1, 0x00);
    WRITE_REG(pReg->COMMON_INT_MASK_2, 0x00);
    WRITE_REG(pReg->COMMON_INT_MASK_3, 0x00);
    WRITE_REG(pReg->COMMON_INT_MASK_4, 0x00);
    WRITE_REG(pReg->DP_INT_STA_MASK, 0x00);
}

/**
 * @brief  Reset operation.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_Reset(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    HAL_CRU_ClkResetAssert(pEDPTX->clkReset);
    HAL_DelayUs(20);
    HAL_CRU_ClkResetDeassert(pEDPTX->clkReset);

    HAL_CRU_ClkResetAssert(pEDPTX->apbClkReset);
    HAL_DelayUs(20);
    HAL_CRU_ClkResetDeassert(pEDPTX->apbClkReset);

    HAL_EDPTX_StopVideo(pEDPTX);
    HAL_EDPTX_DisableVideoMute(pEDPTX);

    reg = MASTER_VID_FUNC_EN_N | SLAVE_VID_FUNC_EN_N |
          AUD_FIFO_FUNC_EN_N | AUD_FUNC_EN_N |
          HDCP_FUNC_EN_N | SW_FUNC_EN_N;
    WRITE_REG(pReg->FUNC_EN_1, reg);

    reg = SSC_FUNC_EN_N | AUX_FUNC_EN_N |
          SERDES_FIFO_FUNC_EN_N |
          LS_CLK_DOMAIN_FUNC_EN_N;
    WRITE_REG(pReg->FUNC_EN_2, reg);

    HAL_DelayUs(30);

    HAL_EDPTX_SetLaneMap(pEDPTX);

    WRITE_REG(pReg->SYS_CTL_1, 0x0);
    WRITE_REG(pReg->SYS_CTL_2, 0x40);
    WRITE_REG(pReg->SYS_CTL_3, 0x0);
    WRITE_REG(pReg->SYS_CTL_4, 0x0);

    WRITE_REG(pReg->PKT_SEND_CTL, 0x0);
    WRITE_REG(pReg->DP_HDCP_CTL, 0x0);

    WRITE_REG(pReg->HPD_DEGLITCH_L, 0x5e);
    WRITE_REG(pReg->HPD_DEGLITCH_H, 0x1a);

    WRITE_REG(pReg->DP_LINK_DEBUG_CTL, 0x10);

    WRITE_REG(pReg->RESERVED071C[2], 0x0);

    WRITE_REG(pReg->DP_VIDEO_FIFO_THRD, 0x0);
    WRITE_REG(pReg->DP_AUDIO_MARGIN, 0x20);

    WRITE_REG(pReg->M_VID_GEN_FILTER_TH, 0x4);
    WRITE_REG(pReg->M_AUD_GEN_FILTER_TH, 0x2);

    WRITE_REG(pReg->SOC_GENERAL_CTL, 0x00000101);
}

/**
 * @brief  SW reset operation.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_SwReset(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    WRITE_REG(pReg->RESERVED0014, RESET_DP_TX);
}

/**
 * @brief  Update interrupt configurations.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  forceHpd: force HPD is enabled or not.
 * @retval void.
 */
void HAL_EDPTX_ConfigInterrupt(struct EDPTX_HANDLE *pEDPTX, HAL_Check forceHpd)
{
    struct EDPTX_REG *pReg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* 0: mask, 1: unmask */
    WRITE_REG(pReg->COMMON_INT_MASK_1, 0);
    WRITE_REG(pReg->COMMON_INT_MASK_2, 0);
    WRITE_REG(pReg->COMMON_INT_MASK_3, 0);

    if (forceHpd) {
        HAL_EDPTX_MuteHpdInterrupt(pEDPTX);
    } else {
        HAL_EDPTX_UnmuteHpdInterrupt(pEDPTX);
    }
}

/**
 * @brief  Mute HPD interrupt.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_MuteHpdInterrupt(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* 0: mask, 1: unmask */
    reg = READ_REG(pReg->COMMON_INT_MASK_4);
    reg &= ~HOTPLUG_CHG;
    WRITE_REG(pReg->COMMON_INT_MASK_4, reg);

    reg = READ_REG(pReg->DP_INT_STA_MASK);
    reg &= ~INT_HPD;
    WRITE_REG(pReg->DP_INT_STA_MASK, reg);
}

/**
 * @brief  Unmute HPD interrupt.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_UnmuteHpdInterrupt(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* 0: mask, 1: unmask */
    reg = READ_REG(pReg->COMMON_INT_MASK_4);
    reg |= HOTPLUG_CHG;
    WRITE_REG(pReg->COMMON_INT_MASK_4, reg);

    reg = READ_REG(pReg->COMMON_INT_MASK_4);
    reg |= INT_HPD;
    WRITE_REG(pReg->DP_INT_STA_MASK, reg);
}

/**
 * @brief  Get PLL lock status.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval pll status.
 */
eEDPTX_pllStatus HAL_EDPTX_GetPllLockStatus(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->RESERVED06A4[7]);
    if (reg & PLL_LOCK) {
        return PLL_LOCKED;
    } else {
        return PLL_UNLOCKED;
    }
}

/**
 * @brief  Set PLL power down.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_SetPllPowerDown(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->RESERVED071C[0]);
    reg |= DP_PLL_PD;
    WRITE_REG(pReg->RESERVED071C[0], reg);
}

/**
 * @brief  Set PLL power up.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_SetPllPowerUp(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->RESERVED071C[0]);
    reg &= ~DP_PLL_PD;
    WRITE_REG(pReg->RESERVED071C[0], reg);
}

/**
 * @brief  Set analog power down.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  block: analog power block type.
 * @retval void.
 */
void HAL_EDPTX_SetAnalogPowerDown(struct EDPTX_HANDLE *pEDPTX, eEDPTX_analogPowerBlock block)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    switch (block) {
    case AUX_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg |= AUX_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH0_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg |= CH0_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH1_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg |= CH1_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH2_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg |= CH2_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH3_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg |= CH3_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case ANALOG_TOTAL:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg |= DP_PHY_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case POWER_ALL:
        reg = DP_PHY_PD | AUX_PD | CH3_PD | CH2_PD |
              CH1_PD | CH0_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    default:
        break;
    }
}

/**
 * @brief  Set analog power up.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  block: analog power block type.
 * @retval void.
 */
void HAL_EDPTX_SetAnalogPowerUp(struct EDPTX_HANDLE *pEDPTX, eEDPTX_analogPowerBlock block)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    switch (block) {
    case AUX_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg &= ~AUX_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH0_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg &= ~CH0_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH1_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg &= ~CH1_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH2_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg &= ~CH2_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case CH3_BLOCK:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg &= ~CH3_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case ANALOG_TOTAL:
        reg = READ_REG(pReg->RESERVED00FC[12]);
        reg &= ~DP_PHY_PD;
        WRITE_REG(pReg->RESERVED00FC[12], reg);
        break;
    case POWER_ALL:
        WRITE_REG(pReg->RESERVED00FC[12], 0x00);
        break;
    default:
        break;
    }
}

/**
 * @brief  Initialize analog functions.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_InitAnalogFunc(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    HAL_EDPTX_SetAnalogPowerUp(pEDPTX, POWER_ALL);

    reg = PLL_LOCK_CHG;
    WRITE_REG(pReg->COMMON_INT_STA_1, reg);

    reg = READ_REG(pReg->RESERVED06A4[7]);
    reg &= ~(F_PLL_LOCK | PLL_LOCK_CTRL);
    WRITE_REG(pReg->RESERVED06A4[7], reg);

    /* Power up PLL */
    HAL_EDPTX_SetPllPowerUp(pEDPTX);

    /* Enable Serdes FIFO function and Link symbol clock domain module */
    reg = READ_REG(pReg->FUNC_EN_2);
    reg &= ~(SERDES_FIFO_FUNC_EN_N | LS_CLK_DOMAIN_FUNC_EN_N
             | AUX_FUNC_EN_N);
    WRITE_REG(pReg->FUNC_EN_2, reg);
}

/**
 * @brief  Clear HPD interrupt.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_ClearHotplugInterrupts(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = HOTPLUG_CHG | HPD_LOST | PLUG;
    WRITE_REG(pReg->COMMON_INT_STA_4, reg);

    reg = INT_HPD;
    WRITE_REG(pReg->DP_INT_STA, reg);
}

/**
 * @brief  Initialize HPD interrupt.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_InitHpd(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    WRITE_REG(pReg->COMMON_INT_STA_4, 0xbb);
    WRITE_REG(pReg->COMMON_INT_STA_4, 0x80);

    reg = READ_REG(pReg->SYS_CTL_3);
    reg &= ~(F_HPD | HPD_CTRL);
    WRITE_REG(pReg->SYS_CTL_3, reg);
}

/**
 * @brief  Set force HPD mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_ForceHpd(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SYS_CTL_3);
    reg |= (F_HPD | HPD_CTRL);
    WRITE_REG(pReg->SYS_CTL_3, reg);
}

/**
 * @brief  Reset the AUX.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_ResetAux(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* Disable AUX channel module */
    reg = READ_REG(pReg->FUNC_EN_2);
    reg |= AUX_FUNC_EN_N;
    WRITE_REG(pReg->FUNC_EN_2, reg);
}

/**
 * @brief  Initialize the AUX.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_InitAux(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* Clear inerrupts related to AUX channel */
    reg = RPLY_RECEIV | AUX_ERR;
    WRITE_REG(pReg->DP_INT_STA, reg);

    HAL_EDPTX_SetAnalogPowerDown(pEDPTX, AUX_BLOCK);
    HAL_DelayUs(20);
    HAL_EDPTX_SetAnalogPowerUp(pEDPTX, AUX_BLOCK);

    HAL_EDPTX_ResetAux(pEDPTX);

    reg = AUX_BIT_PERIOD_EXPECTED_DELAY(0) |
          AUX_HW_RETRY_COUNT_SEL(3) |
          AUX_HW_RETRY_INTERVAL_600_MICROSECONDS;
    WRITE_REG(pReg->RESERVED0360[12], reg);

    /* Receive AUX Channel DEFER commands equal to DEFFER_COUNT*64 */
    reg = DEFER_CTRL_EN | DEFER_COUNT(1);
    WRITE_REG(pReg->AUX_CH_DEFER_CTL, reg);

    /* Enable AUX channel module */
    reg = READ_REG(pReg->FUNC_EN_2);
    reg &= ~AUX_FUNC_EN_N;
    WRITE_REG(pReg->FUNC_EN_2, reg);
}

/**
 * @brief  Get the plug status.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_GetPlugInStatus(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SYS_CTL_3);
    if (reg & HPD_STATUS) {
        return HAL_OK;
    }

    return HAL_INVAL;
}

/**
 * @brief  Enable sw function.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_EnableSwFunction(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->FUNC_EN_1);
    reg &= ~SW_FUNC_EN_N;
    WRITE_REG(pReg->FUNC_EN_1, reg);
}

/**
 * @brief  Start AUX transaction.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_StartAuxTransaction(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;
    int retval = 0;
    int timeoutLoop = 0;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* Enable AUX CH operation */
    reg = READ_REG(pReg->AUX_CH_CTL_2);
    reg |= AUX_EN;
    WRITE_REG(pReg->AUX_CH_CTL_2, reg);

    /* Is AUX CH command reply received? */
    reg = READ_REG(pReg->DP_INT_STA);
    while (!(reg & RPLY_RECEIV)) {
        timeoutLoop++;
        if (DP_TIMEOUT_LOOP_COUNT < timeoutLoop) {
            HAL_DBG_ERR("AUX CH command reply failed!\n");

            return HAL_TIMEOUT;
        }

        reg = READ_REG(pReg->DP_INT_STA);
        HAL_DelayUs(11);
    }

    /* Clear interrupt source for AUX CH command reply */
    WRITE_REG(pReg->DP_INT_STA, reg);

    /* Clear interrupt source for AUX CH access error */
    reg = READ_REG(pReg->DP_INT_STA);
    if (reg & AUX_ERR) {
        WRITE_REG(pReg->DP_INT_STA, AUX_ERR);

        return HAL_ERROR;
    }

    /* Check AUX CH error access status */
    reg = READ_REG(pReg->AUX_CH_STA);
    if ((reg & AUX_STATUS_MASK) != 0) {
        HAL_DBG_ERR("AUX CH error happens: %" PRId32 "\n", reg & AUX_STATUS_MASK);

        return HAL_ERROR;
    }

    return retval;
}

/**
 * @brief  Select I2C device.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  deviceAddr: I2C device address.
 * @param  regAddr: the offset of base address.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_SelectI2cDevice(struct EDPTX_HANDLE *pEDPTX,
                                     uint32_t deviceAddr,
                                     uint32_t regAddr)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;
    int retval;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* Set EDID device address */
    reg = deviceAddr;
    WRITE_REG(pReg->AUX_ADDR_7_0, reg);
    WRITE_REG(pReg->AUX_ADDR_15_8, 0x0);
    WRITE_REG(pReg->AUX_ADDR_19_16, 0x0);

    /* Set offset from base address of EDID device */
    WRITE_REG(pReg->BUF_DATA, regAddr);

    /*
     * Set I2C transaction and write address
     * If bit 3 is 1, DisplayPort transaction.
     * If Bit 3 is 0, I2C transaction.
     */
    reg = AUX_TX_COMM_I2C_TRANSACTION | AUX_TX_COMM_MOT |
          AUX_TX_COMM_WRITE;
    WRITE_REG(pReg->AUX_CH_CTL_1, reg);

    /* Start AUX transaction */
    retval = HAL_EDPTX_StartAuxTransaction(pEDPTX);
    if (retval < 0) {
        return retval;
    }

    return 0;
}

/**
 * @brief  Read byte from DPCD.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  regAddr: the offset of base address.
 * @param  data: data.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_ReadByteFromDpcd(struct EDPTX_HANDLE *pEDPTX,
                                      uint32_t regAddr, uint8_t *data)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;
    int i;
    int retval;

    HAL_ASSERT(pEDPTX != NULL);
    HAL_ASSERT(data != NULL);

    pReg = pEDPTX->pReg;

    for (i = 0; i < 3; i++) {
        /* Clear AUX CH data buffer */
        reg = BUF_CLR;
        WRITE_REG(pReg->BUFFER_DATA_CTL, reg);

        /* Select DPCD device address */
        reg = AUX_ADDR_7_0(regAddr);
        WRITE_REG(pReg->AUX_ADDR_7_0, reg);
        reg = AUX_ADDR_15_8(regAddr);
        WRITE_REG(pReg->AUX_ADDR_15_8, reg);
        reg = AUX_ADDR_19_16(regAddr);
        WRITE_REG(pReg->AUX_ADDR_19_16, reg);

        /*
         * Set DisplayPort transaction and read 1 byte
         * If bit 3 is 1, DisplayPort transaction.
         * If Bit 3 is 0, I2C transaction.
         */
        reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_READ;
        WRITE_REG(pReg->AUX_CH_CTL_1, reg);

        /* Start AUX transaction */
        retval = HAL_EDPTX_StartAuxTransaction(pEDPTX);
        if (retval == 0) {
            break;
        }
    }

    /* Read data buffer */
    reg = READ_REG(pReg->BUF_DATA);
    *data = (unsigned char)(reg & 0xff);

    return retval;
}

/**
 * @brief  Read bytes from DPCD.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  regAddr: the offset of base address.
 * @param  count: data size.
 * @param  data: data.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_ReadBytesFromDpcd(struct EDPTX_HANDLE *pEDPTX, uint32_t regAddr,
                                       uint32_t count, uint8_t *data)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;
    uint32_t startOffset;
    uint32_t curDataCount;
    uint32_t curDataIdx;
    int i;
    int retval = 0;

    HAL_ASSERT(pEDPTX != NULL);
    HAL_ASSERT(data != NULL);

    pReg = pEDPTX->pReg;

    /* Clear AUX CH data buffer */
    reg = BUF_CLR;
    WRITE_REG(pReg->BUFFER_DATA_CTL, reg);

    startOffset = 0;
    while (startOffset < count) {
        /* Buffer size of AUX CH is 16 * 4bytes */
        if ((count - startOffset) > 16) {
            curDataCount = 16;
        } else {
            curDataCount = count - startOffset;
        }

        /* AUX CH Request Transaction process */
        for (i = 0; i < 3; i++) {
            /* Select DPCD device address */
            reg = AUX_ADDR_7_0(regAddr + startOffset);
            WRITE_REG(pReg->AUX_ADDR_7_0, reg);
            reg = AUX_ADDR_15_8(regAddr + startOffset);
            WRITE_REG(pReg->AUX_ADDR_15_8, reg);
            reg = AUX_ADDR_19_16(regAddr + startOffset);
            WRITE_REG(pReg->AUX_ADDR_19_16, reg);

            /*
             * Set DisplayPort transaction and read
             * If bit 3 is 1, DisplayPort transaction.
             * If Bit 3 is 0, I2C transaction.
             */
            reg = AUX_LENGTH(curDataCount) |
                  AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_READ;
            WRITE_REG(pReg->AUX_CH_CTL_1, reg);

            /* Start AUX transaction */
            retval = HAL_EDPTX_StartAuxTransaction(pEDPTX);
            if (retval == 0) {
                break;
            }
        }

        for (curDataIdx = 0; curDataIdx < curDataCount; curDataIdx++) {
            if (curDataIdx == 0) {
                reg = READ_REG(pReg->BUF_DATA);
            } else {
                reg = READ_REG(pReg->RESERVED07C4[curDataIdx - 1]);
            }
            data[startOffset + curDataIdx] = (unsigned char)reg;
        }

        startOffset += curDataCount;
    }

    return retval;
}

/**
 * @brief  Write byte to DPCD.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  regAddr: the offset of base address.
 * @param  data: data.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_WriteByteToDpcd(struct EDPTX_HANDLE *pEDPTX,
                                     uint32_t regAddr, uint8_t data)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;
    int i;
    int retval;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    for (i = 0; i < 3; i++) {
        /* Clear AUX CH data buffer */
        reg = BUF_CLR;
        WRITE_REG(pReg->BUFFER_DATA_CTL, reg);

        /* Select DPCD device address */
        reg = AUX_ADDR_7_0(regAddr);
        WRITE_REG(pReg->AUX_ADDR_7_0, reg);
        reg = AUX_ADDR_15_8(regAddr);
        WRITE_REG(pReg->AUX_ADDR_15_8, reg);
        reg = AUX_ADDR_19_16(regAddr);
        WRITE_REG(pReg->AUX_ADDR_19_16, reg);

        /* Write data buffer */
        reg = (uint32_t)data;
        WRITE_REG(pReg->BUF_DATA, reg);

        /*
         * Set DisplayPort transaction and write 1 byte
         * If bit 3 is 1, DisplayPort transaction.
         * If Bit 3 is 0, I2C transaction.
         */
        reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_WRITE;
        WRITE_REG(pReg->AUX_CH_CTL_1, reg);

        /* Start AUX transaction */
        retval = HAL_EDPTX_StartAuxTransaction(pEDPTX);
        if (retval == 0) {
            break;
        }
    }

    return retval;
}

/**
 * @brief  Write bytes from DPCD.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  regAddr: the offset of base address.
 * @param  count: data size.
 * @param  data: data.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_WriteBytesToDpcd(struct EDPTX_HANDLE *pEDPTX, uint32_t regAddr,
                                      uint32_t count, uint8_t *data)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;
    uint32_t startOffset;
    uint32_t curDataCount;
    uint32_t curDataIdx;
    int i;
    int retval = 0;

    HAL_ASSERT(pEDPTX != NULL);
    HAL_ASSERT(data != NULL);

    pReg = pEDPTX->pReg;

    /* Clear AUX CH data buffer */
    reg = BUF_CLR;
    WRITE_REG(pReg->BUFFER_DATA_CTL, reg);

    startOffset = 0;
    while (startOffset < count) {
        /* Buffer size of AUX CH is 16 * 4bytes */
        if ((count - startOffset) > 16) {
            curDataCount = 16;
        } else {
            curDataCount = count - startOffset;
        }

        for (i = 0; i < 3; i++) {
            /* Select DPCD device address */
            reg = AUX_ADDR_7_0(regAddr + startOffset);
            WRITE_REG(pReg->AUX_ADDR_7_0, reg);
            reg = AUX_ADDR_15_8(regAddr + startOffset);
            WRITE_REG(pReg->AUX_ADDR_15_8, reg);
            reg = AUX_ADDR_19_16(regAddr + startOffset);
            WRITE_REG(pReg->AUX_ADDR_19_16, reg);

            for (curDataIdx = 0; curDataIdx < curDataCount; curDataIdx++) {
                reg = data[startOffset + curDataIdx];
                if (curDataIdx == 0) {
                    WRITE_REG(pReg->BUF_DATA, reg);
                } else {
                    WRITE_REG(pReg->RESERVED07C4[curDataIdx - 1], reg);
                }
            }

            /*
             * Set DisplayPort transaction and write
             * If bit 3 is 1, DisplayPort transaction.
             * If Bit 3 is 0, I2C transaction.
             */
            reg = AUX_LENGTH(curDataCount) |
                  AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_WRITE;
            WRITE_REG(pReg->AUX_CH_CTL_1, reg);

            /* Start AUX transaction */
            retval = HAL_EDPTX_StartAuxTransaction(pEDPTX);
            if (retval == 0) {
                break;
            }
        }

        startOffset += curDataCount;
    }

    return retval;
}

/**
 * @brief  Set link bandwidth.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  bwType: bandwidth type.
 * @retval void.
 */
void HAL_EDPTX_SetLinkBandwidth(struct EDPTX_HANDLE *pEDPTX, uint8_t bwType)
{
    struct EDPTX_REG *pReg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    WRITE_REG(pReg->LINK_BW_SET, bwType);
}

/**
 * @brief  Get link bandwidth.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  bwType: bandwidth type.
 * @retval void.
 */
void HAL_EDPTX_GetLinkBandwidth(struct EDPTX_HANDLE *pEDPTX, uint8_t *bwType)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);
    HAL_ASSERT(bwType != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->LINK_BW_SET);
    *bwType = reg;
}

/**
 * @brief  Set lane count.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  count: lane count.
 * @retval void.
 */
void HAL_EDPTX_SetLaneCount(struct EDPTX_HANDLE *pEDPTX, uint32_t count)
{
    struct EDPTX_REG *pReg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    WRITE_REG(pReg->LANE_COUNT_SET, count);
}

/**
 * @brief  Set lane count.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  count: lane count.
 * @retval void.
 */
void HAL_EDPTX_GetLaneCount(struct EDPTX_HANDLE *pEDPTX, uint8_t *count)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);
    HAL_ASSERT(count != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->LANE_COUNT_SET);
    *count = reg;
}

/**
 * @brief  Set lane link training.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  lane: lane index.
 * @param  laneCfg: lane configurations
 * @retval void.
 */
void HAL_EDPTX_SetLaneLinkTraining(struct EDPTX_HANDLE *pEDPTX, uint8_t lane, uint8_t laneCfg)
{
    struct EDPTX_REG *pReg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    switch (lane) {
    case 0:
        WRITE_REG(pReg->DP_LN0_LINK_TRAINING_CTL, laneCfg);
        break;
    case 1:
        WRITE_REG(pReg->DP_LN1_LINK_TRAINING_CTL, laneCfg);
        break;
    case 2:
        WRITE_REG(pReg->DP_LN2_LINK_TRAINING_CTL, laneCfg);
        break;
    case 3:
        WRITE_REG(pReg->DP_LN3_LINK_TRAINING_CTL, laneCfg);
        break;
    }
}

/**
 * @brief  Get lane link training.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  lane: lane index.
 * @retval link training parameters.
 */
uint32_t HAL_EDPTX_GetLaneLinkTraining(struct EDPTX_HANDLE *pEDPTX, uint8_t lane)
{
    struct EDPTX_REG *pReg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    switch (lane) {
    case 0:
        return READ_REG(pReg->DP_LN0_LINK_TRAINING_CTL);
    case 1:
        return READ_REG(pReg->DP_LN1_LINK_TRAINING_CTL);
    case 2:
        return READ_REG(pReg->DP_LN2_LINK_TRAINING_CTL);
    case 3:
        return READ_REG(pReg->DP_LN3_LINK_TRAINING_CTL);
    default:
        return 0;
    }
}

/**
 * @brief  Enable enhanced mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_EnableEnhancedMode(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SYS_CTL_4);
    reg |= ENHANCED;
    WRITE_REG(pReg->SYS_CTL_4, reg);
}

/**
 * @brief  Disable enhanced mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_DisableEnhancedMode(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SYS_CTL_4);
    reg &= ~ENHANCED;
    WRITE_REG(pReg->SYS_CTL_4, reg);
}

/**
 * @brief  Get enhanced mode status.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval enhanced mode is enabled or not.
 */
HAL_Check HAL_EDPTX_IsEnhancedMode(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SYS_CTL_4);

    return !!(reg & ENHANCED);
}

/**
 * @brief  Set link training pattern.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  pattern: link training pattern type.
 * @retval void.
 */
void HAL_EDPTX_SetTrainingPattern(struct EDPTX_HANDLE *pEDPTX, eEDPTX_patternSet pattern)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    switch (pattern) {
    case PRBS7:
        reg = SCRAMBLING_ENABLE | LINK_QUAL_PATTERN_SET_PRBS7;
        WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
        break;
    case D10_2:
        reg = SCRAMBLING_ENABLE | LINK_QUAL_PATTERN_SET_D10_2;
        WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
        break;
    case TRAINING_PTN1:
        reg = SCRAMBLING_DISABLE | SW_TRAINING_PATTERN_SET_PTN1;
        WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
        break;
    case TRAINING_PTN2:
        reg = SCRAMBLING_DISABLE | SW_TRAINING_PATTERN_SET_PTN2;
        WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
        break;
    case TRAINING_PTN3:
        reg = SCRAMBLING_DISABLE | SW_TRAINING_PATTERN_SET_PTN3;
        WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
        break;
    case DP_NONE:
        reg = SCRAMBLING_ENABLE |
              LINK_QUAL_PATTERN_SET_DISABLE |
              SW_TRAINING_PATTERN_SET_NORMAL;
        WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
        break;
    default:
        break;
    }
}

/**
 * @brief  Reset macro.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_ResetMacro(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->RESERVED071C[2]);
    reg |= MACRO_RST;
    WRITE_REG(pReg->RESERVED071C[2], reg);

    /* 10 us is the minimum reset time. */
    HAL_DelayUs(20);

    reg &= ~MACRO_RST;
    WRITE_REG(pReg->RESERVED071C[2], reg);
}

/**
 * @brief  Initialize video.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  forceStreamValid: force stream valid is enabled or not.
 * @retval void.
 */
void HAL_EDPTX_InitVideo(struct EDPTX_HANDLE *pEDPTX, HAL_Check forceStreamValid)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = VSYNC_DET | VID_FORMAT_CHG | VID_CLK_CHG;
    WRITE_REG(pReg->COMMON_INT_STA_1, reg);

    reg = 0x0;
    WRITE_REG(pReg->SYS_CTL_1, reg);

    reg = CHA_CRI(4) | CHA_CTRL;
    WRITE_REG(pReg->SYS_CTL_2, reg);

    if (forceStreamValid) {
        reg = READ_REG(pReg->SYS_CTL_3);
        reg |= VALID_CTRL | F_VALID;
        WRITE_REG(pReg->SYS_CTL_3, reg);
    }

    reg = VID_HRES_TH(2) | VID_VRES_TH(0);
    WRITE_REG(pReg->VIDEO_CTL_8, reg);
}

/**
 * @brief  Set video color format.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  dynamicRange: dynamic range.
 * @param  colorDepth: color depth.
 * @param  colorSpace: color space.
 * @param  ycbcrCoeff: YCbCr coeff.
 * @retval void.
 */
void HAL_EDPTX_SetVideoColorFormat(struct EDPTX_HANDLE *pEDPTX, uint8_t dynamicRange, uint8_t colorDepth,
                                   uint8_t colorSpace, uint8_t ycbcrCoeff)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* Configure the input color depth, color space, dynamic range */
    reg = (dynamicRange << IN_D_RANGE_SHIFT) |
          (colorDepth << IN_BPC_SHIFT) |
          (colorSpace << IN_COLOR_F_SHIFT);
    WRITE_REG(pReg->VIDEO_CTL_2, reg);

    /* Set Input Color YCbCr Coefficients to ITU601 or ITU709 */
    reg = READ_REG(pReg->VIDEO_CTL_3);
    reg &= ~IN_YC_COEFFI_MASK;
    if (ycbcrCoeff) {
        reg |= IN_YC_COEFFI_ITU709;
    } else {
        reg |= IN_YC_COEFFI_ITU601;
    }
    WRITE_REG(pReg->VIDEO_CTL_3, reg);
}

/**
 * @brief  Set video color format.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval HAL status
 */
HAL_Status HAL_EDPTX_IsSlaveVideoStreamClockOn(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SYS_CTL_1);
    WRITE_REG(pReg->SYS_CTL_1, reg);

    reg = READ_REG(pReg->SYS_CTL_1);

    if (!(reg & DET_STA)) {
        HAL_DBG_ERR("Input stream clock not detected.\n");

        return HAL_INVAL;
    }

    reg = READ_REG(pReg->SYS_CTL_2);
    WRITE_REG(pReg->SYS_CTL_2, reg);

    reg = READ_REG(pReg->SYS_CTL_2);

    if (reg & CHA_STA) {
        HAL_DBG_ERR("Input stream clk is changing.\n");

        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief  Set video clock recovery M/N value.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  type: clock recovery M value type.
 * @param  mValue: M value.
 * @param  nValue: N value.
 * @retval void
 */
void HAL_EDPTX_SetVideoCrMn(struct EDPTX_HANDLE *pEDPTX,
                            eEDPTX_clockRecoveryMValueType type,
                            uint32_t mValue, uint32_t nValue)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    if (type == REGISTER_M) {
        reg = READ_REG(pReg->SYS_CTL_4);
        reg |= FIX_M_VID;
        WRITE_REG(pReg->SYS_CTL_4, reg);
        reg = mValue & 0xff;
        WRITE_REG(pReg->M_VID_0, reg);
        reg = (mValue >> 8) & 0xff;
        WRITE_REG(pReg->M_VID_1, reg);
        reg = (mValue >> 16) & 0xff;
        WRITE_REG(pReg->M_VID_2, reg);

        reg = nValue & 0xff;
        WRITE_REG(pReg->N_VID_0, reg);
        reg = (nValue >> 8) & 0xff;
        WRITE_REG(pReg->N_VID_1, reg);
        reg = (nValue >> 16) & 0xff;
        WRITE_REG(pReg->N_VID_2, reg);
    } else {
        reg = READ_REG(pReg->SYS_CTL_4);
        reg &= ~FIX_M_VID;
        WRITE_REG(pReg->SYS_CTL_4, reg);

        WRITE_REG(pReg->N_VID_0, 0x00);
        WRITE_REG(pReg->N_VID_1, 0x80);
        WRITE_REG(pReg->N_VID_2, 0x00);
    }
}

/**
 * @brief  Set video timing mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  type: video timing recognition type.
 * @retval void
 */
void HAL_EDPTX_SetVideoTimingMode(struct EDPTX_HANDLE *pEDPTX, eEDPTX_videoTimingRecognitionType type)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    if (type == VIDEO_TIMING_FROM_CAPTURE) {
        reg = READ_REG(pReg->VIDEO_CTL_10);
        reg &= ~FORMAT_SEL;
        WRITE_REG(pReg->VIDEO_CTL_10, reg);
    } else {
        reg = READ_REG(pReg->VIDEO_CTL_10);
        reg |= FORMAT_SEL;
        WRITE_REG(pReg->VIDEO_CTL_10, reg);
    }
}

/**
 * @brief  Enable video mater.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void
 */
void HAL_EDPTX_EnableVideoMaster(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SOC_GENERAL_CTL);
    reg &= ~VIDEO_MODE_MASK;
    reg |= VIDEO_MASTER_MODE_EN | VIDEO_MODE_MASTER_MODE;
    WRITE_REG(pReg->SOC_GENERAL_CTL, reg);
}

/**
 * @brief  Disable video mater.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void
 */
void HAL_EDPTX_DisableVideoMaster(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SOC_GENERAL_CTL);
    reg &= ~VIDEO_MODE_MASK;
    reg |= VIDEO_MODE_SLAVE_MODE;
    WRITE_REG(pReg->SOC_GENERAL_CTL, reg);
}

/**
 * @brief  Start video.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void
 */
void HAL_EDPTX_StartVideo(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->VIDEO_CTL_1);
    reg |= VIDEO_EN;
    WRITE_REG(pReg->VIDEO_CTL_1, reg);
}

/**
 * @brief  Check if video stream is on.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval 0 if video stream is on.
 */
HAL_Status HAL_EDPTX_IsVideoStreamOn(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->SYS_CTL_3);
    WRITE_REG(pReg->SYS_CTL_3, reg);

    reg = READ_REG(pReg->SYS_CTL_3);
    if (!(reg & STRM_VALID)) {
        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief  Configurate video slave mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  interlaced: display interlaced mode is enabled or not.
 * @param  vsyncPolarity: vsync polarity.
 * @param  hsyncPolarity: hsync polarity.
 * @retval void.
 */
void HAL_EDPTX_ConfigVideoSlaveMode(struct EDPTX_HANDLE *pEDPTX, HAL_Check interlaced,
                                    HAL_Check vsyncPolarity, HAL_Check hsyncPolarity)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->FUNC_EN_1);
    reg &= ~(MASTER_VID_FUNC_EN_N | SLAVE_VID_FUNC_EN_N);
    reg |= MASTER_VID_FUNC_EN_N;
    WRITE_REG(pReg->FUNC_EN_1, reg);

    reg = READ_REG(pReg->VIDEO_CTL_10);
    reg &= ~INTERACE_SCAN_CFG;
    reg |= (interlaced << 2);
    WRITE_REG(pReg->VIDEO_CTL_10, reg);

    reg = READ_REG(pReg->VIDEO_CTL_10);
    reg &= ~VSYNC_POLARITY_CFG;
    reg |= (vsyncPolarity << 1);
    WRITE_REG(pReg->VIDEO_CTL_10, reg);

    reg = READ_REG(pReg->VIDEO_CTL_10);
    reg &= ~HSYNC_POLARITY_CFG;
    reg |= (hsyncPolarity << 0);
    WRITE_REG(pReg->VIDEO_CTL_10, reg);

    reg = AUDIO_MODE_SPDIF_MODE | VIDEO_MODE_SLAVE_MODE;
    WRITE_REG(pReg->SOC_GENERAL_CTL, reg);
}

/**
 * @brief  Enable scrambling.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_EnableScrambling(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->DP_TRAINING_PTN_SET);
    reg &= ~SCRAMBLING_DISABLE;
    WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
}

/**
 * @brief  Disable scrambling.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_DisableScrambling(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->DP_TRAINING_PTN_SET);
    reg |= SCRAMBLING_DISABLE;
    WRITE_REG(pReg->DP_TRAINING_PTN_SET, reg);
}

/**
 * @brief  Set video format.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  mode: display mode information
 * @param  interlaced: display interlaced mode is enabled or not.
 * @param  vsyncPolarity: vsync polarity.
 * @param  hsyncPolarity: hsync polarity.
 * @retval void.
 */
void HAL_EDPTX_SetVideoFormat(struct EDPTX_HANDLE *pEDPTX, const struct DISPLAY_MODE_INFO *mode,
                              HAL_Check interlaced, HAL_Check vsyncPolarity, HAL_Check hsyncPolarity)
{
    struct EDPTX_REG *pReg;
    uint32_t hsw, hfp, hbp, vsw, vfp, vbp;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    interlaced = !!(mode->flags & VIDEO_MODE_FLAG_INTERLACE);

    vsyncPolarity = !!(mode->flags & VIDEO_MODE_FLAG_NVSYNC);
    hsyncPolarity = !!(mode->flags & VIDEO_MODE_FLAG_NHSYNC);

    hsw = mode->hsyncEnd - mode->hsyncStart;
    hfp = mode->hsyncStart - mode->hdisplay;
    hbp = mode->htotal - mode->hsyncEnd;
    vsw = mode->vsyncEnd - mode->vsyncStart;
    vfp = mode->vsyncStart - mode->vdisplay;
    vbp = mode->vtotal - mode->vsyncEnd;

    /* Set Video Format Parameters */
    WRITE_REG(pReg->TOTAL_LINE_CFG_L,
              TOTAL_LINE_CFG_L(mode->vtotal));
    WRITE_REG(pReg->TOTAL_LINE_CFG_H,
              TOTAL_LINE_CFG_H(mode->vtotal >> 8));
    WRITE_REG(pReg->ACTIVE_LINE_CFG_L,
              ACTIVE_LINE_CFG_L(mode->vdisplay));
    WRITE_REG(pReg->ACTIVE_LINE_CFG_H,
              ACTIVE_LINE_CFG_H(mode->vdisplay >> 8));
    WRITE_REG(pReg->V_F_PORCH_CFG,
              V_F_PORCH_CFG(vfp));
    WRITE_REG(pReg->V_SYNC_WIDTH_CFG,
              V_SYNC_WIDTH_CFG(vsw));
    WRITE_REG(pReg->V_B_PORCH_CFG,
              V_B_PORCH_CFG(vbp));
    WRITE_REG(pReg->TOTAL_PIXEL_CFG_L,
              TOTAL_PIXEL_CFG_L(mode->htotal));
    WRITE_REG(pReg->TOTAL_PIXEL_CFG_H,
              TOTAL_PIXEL_CFG_H(mode->htotal >> 8));
    WRITE_REG(pReg->ACTIVE_PIXEL_CFG_L,
              ACTIVE_PIXEL_CFG_L(mode->hdisplay));
    WRITE_REG(pReg->ACTIVE_PIXEL_CFG_H,
              ACTIVE_PIXEL_CFG_H(mode->hdisplay >> 8));
    WRITE_REG(pReg->H_F_PORCH_CFG_L,
              H_F_PORCH_CFG_L(hfp));
    WRITE_REG(pReg->H_F_PORCH_CFG_H,
              H_F_PORCH_CFG_H(hfp >> 8));
    WRITE_REG(pReg->H_SYNC_CFG_L,
              H_SYNC_CFG_L(hsw));
    WRITE_REG(pReg->H_SYNC_CFG_H,
              H_SYNC_CFG_H(hsw >> 8));
    WRITE_REG(pReg->H_B_PORCH_CFG_L,
              H_B_PORCH_CFG_L(hbp));
    WRITE_REG(pReg->H_B_PORCH_CFG_H,
              H_B_PORCH_CFG_H(hbp >> 8));
}

/**
 * @brief  Enable bist mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_VideoBistEnable(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    /* Enable Video BIST */
    WRITE_REG(pReg->VIDEO_CTL_4, BIST_EN);

    /*
     * Note that if BIST_EN is set to 1, F_SEL must be cleared to 0
     * although video format information comes from registers set by user.
     */
    reg = READ_REG(pReg->VIDEO_CTL_10);
    reg &= ~FORMAT_SEL;
    WRITE_REG(pReg->VIDEO_CTL_10, reg);
}

/**
 * @brief  AUX transfer.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @param  aux: pointer to a DP_AUX structure.
 * @param  msg: pointer to a DP_AUX_MSG structure.
 * @retval return length of transfered message if successful, and return error HAL_Status if failed.
 */
int32_t HAL_EDPTX_AuxTransfer(struct EDPTX_HANDLE *pEDPTX, struct DP_AUX *aux, struct DP_AUX_MSG *msg)
{
    struct EDPTX_REG *pReg;
    HAL_Status ret;
    uint32_t reg;
    uint8_t *buffer = msg->buffer;
    uint32_t i;
    int32_t numTransferred = 0;

    HAL_ASSERT(pEDPTX != NULL);
    HAL_ASSERT(aux != NULL);
    HAL_ASSERT(msg != NULL);

    pReg = pEDPTX->pReg;

    /* Buffer size of AUX CH is 16 bytes */
    if (msg->size > 16) {
        return HAL_ERROR;
    }

    /* Clear AUX CH data buffer */
    reg = BUF_CLR;
    WRITE_REG(pReg->BUFFER_DATA_CTL, reg);

    switch (msg->request & ~DP_AUX_I2C_MOT) {
    case DP_AUX_I2C_WRITE:
        reg = AUX_TX_COMM_WRITE | AUX_TX_COMM_I2C_TRANSACTION;
        if (msg->request & DP_AUX_I2C_MOT) {
            reg |= AUX_TX_COMM_MOT;
        }
        break;

    case DP_AUX_I2C_READ:
        reg = AUX_TX_COMM_READ | AUX_TX_COMM_I2C_TRANSACTION;
        if (msg->request & DP_AUX_I2C_MOT) {
            reg |= AUX_TX_COMM_MOT;
        }
        break;

    case DP_AUX_NATIVE_WRITE:
        reg = AUX_TX_COMM_WRITE | AUX_TX_COMM_DP_TRANSACTION;
        break;

    case DP_AUX_NATIVE_READ:
        reg = AUX_TX_COMM_READ | AUX_TX_COMM_DP_TRANSACTION;
        break;

    default:
        return HAL_INVAL;
    }

    reg |= AUX_LENGTH(msg->size);
    WRITE_REG(pReg->AUX_CH_CTL_1, reg);

    /* Select DPCD device address */
    reg = AUX_ADDR_7_0(msg->address);
    WRITE_REG(pReg->AUX_ADDR_7_0, reg);
    reg = AUX_ADDR_15_8(msg->address);
    WRITE_REG(pReg->AUX_ADDR_15_8, reg);
    reg = AUX_ADDR_19_16(msg->address);
    WRITE_REG(pReg->AUX_ADDR_19_16, reg);

    if (!(msg->request & DP_AUX_I2C_READ)) {
        for (i = 0; i < msg->size; i++) {
            reg = buffer[i];
            if (i == 0) {
                WRITE_REG(pReg->BUF_DATA, reg);
            } else {
                WRITE_REG(pReg->RESERVED07C4[i - 1], reg);
            }
            numTransferred++;
        }
    }

    /* Enable AUX CH operation */
    reg = AUX_EN;

    /* Zero-sized messages specify address-only transactions. */
    if (msg->size < 1) {
        reg |= ADDR_ONLY;
    }

    WRITE_REG(pReg->AUX_CH_CTL_2, reg);

    ret = HAL_ReadRegPollTimeoutAtomic(pReg->AUX_CH_CTL_2, reg, !(reg & AUX_EN), 25, 500 * 1000);
    if (ret) {
        HAL_DBG_ERR("AUX CH enable timeout!\n");
        goto aux_error;
    }

    ret = HAL_ReadRegPollTimeoutAtomic(pReg->INT_STATE, reg, (reg & RPLY_RECEIV), 10, 20 * 1000);

    if (ret) {
        HAL_DBG_ERR("AUX CH cmd reply timeout!\n");
        goto aux_error;
    }

    /* Clear interrupt source for AUX CH command reply */
    WRITE_REG(pReg->INT_STATE, RPLY_RECEIV);

    reg = READ_REG(pReg->AUX_CH_STA);
    if ((reg & AUX_STATUS_MASK) == AUX_STATUS_TIMEOUT_ERROR) {
        return HAL_TIMEOUT;
    }

    if (msg->request & DP_AUX_I2C_READ) {
        size_t buf_data_count;

        reg = READ_REG(pReg->BUFFER_DATA_CTL);
        buf_data_count = BUF_DATA_COUNT(reg);

        if (buf_data_count != msg->size) {
            return HAL_BUSY;
        }

        for (i = 0; i < msg->size; i++) {
            if (i == 0) {
                reg = READ_REG(pReg->BUF_DATA);
            } else {
                reg = READ_REG(pReg->RESERVED07C4[i - 1]);
            }
            buffer[i] = (unsigned char)reg;
            numTransferred++;
        }
    }

    /* Check if Rx sends defer */
    reg = READ_REG(pReg->AUX_RX_COMM);
    if (reg == AUX_RX_COMM_AUX_DEFER) {
        msg->reply = DP_AUX_NATIVE_REPLY_DEFER;
    } else if (reg == AUX_RX_COMM_I2C_DEFER) {
        msg->reply = DP_AUX_I2C_REPLY_DEFER;
    } else if ((msg->request & ~DP_AUX_I2C_MOT) == DP_AUX_I2C_WRITE ||
               (msg->request & ~DP_AUX_I2C_MOT) == DP_AUX_I2C_READ) {
        msg->reply = DP_AUX_I2C_REPLY_ACK;
    } else if ((msg->request & ~DP_AUX_I2C_MOT) == DP_AUX_NATIVE_WRITE ||
               (msg->request & ~DP_AUX_I2C_MOT) == DP_AUX_NATIVE_READ) {
        msg->reply = DP_AUX_NATIVE_REPLY_ACK;
    }

    return (numTransferred == msg->size) ? numTransferred : HAL_BUSY;

aux_error:
    /* if aux err happen, reset aux */
    HAL_EDPTX_InitAux(pEDPTX);

    return HAL_ERROR;
}

/**
 * @brief  Enable ASSR mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_EnableAssrMode(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->LINK_POLICY);
    reg |= ALTERNATE_SR_ENABLE;
    WRITE_REG(pReg->LINK_POLICY, reg);
}

/**
 * @brief  Disable ASSR mode.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
void HAL_EDPTX_DisableAssrMode(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->LINK_POLICY);
    reg &= ~ALTERNATE_SR_ENABLE;
    WRITE_REG(pReg->LINK_POLICY, reg);
}

/**
 * @brief  Get ASSR mode status.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval ASSR is enabled or not.
 */
HAL_Check HAL_EDPTX_IsAssrMode(struct EDPTX_HANDLE *pEDPTX)
{
    struct EDPTX_REG *pReg;
    uint32_t reg;

    HAL_ASSERT(pEDPTX != NULL);

    pReg = pEDPTX->pReg;

    reg = READ_REG(pReg->LINK_POLICY);

    return !!(reg & ALTERNATE_SR_ENABLE);
}

/**
 * @brief  Global initialization.
 * @param  pEDPTX: pointer to a EDPTX_HANDLE structure.
 * @retval void.
 */
HAL_Status HAL_EDPTX_Init(struct EDPTX_HANDLE *pEDPTX, const struct HAL_EDPTX_DEV *dev)
{
    HAL_ASSERT(pEDPTX != NULL);
    HAL_ASSERT(dev != NULL);
    HAL_ASSERT(IS_EDPTX_INSTANCE(dev->pReg));

    pEDPTX->pReg = dev->pReg;
    pEDPTX->clkReset = dev->clkReset;
    pEDPTX->apbClkReset = dev->apbClkReset;

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_EDPTX_MODULE_ENABLED */
