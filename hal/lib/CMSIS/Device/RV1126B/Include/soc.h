/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#ifndef __SOC_H
#define __SOC_H
#ifdef __cplusplus
  extern "C" {
#endif

#include "hal_conf.h"

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define   __I     volatile             /*!< brief Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< brief Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< brief Defines 'write only' permissions */
#define     __IO    volatile             /*!< brief Defines 'read / write' permissions */

/* ================================================================================ */
/* ================                    DMA REQ                      =============== */
/* ================================================================================ */
typedef enum {
    DMA_REQ_UART0_RX = 0,
    DMA_REQ_UART0_TX = 1,
    DMA_REQ_UART1_RX = 2,
    DMA_REQ_UART1_TX = 3,
    DMA_REQ_UART2_RX = 4,
    DMA_REQ_UART2_TX = 5,
    DMA_REQ_UART3_RX = 6,
    DMA_REQ_UART3_TX = 7,
    DMA_REQ_UART4_RX = 8,
    DMA_REQ_UART4_TX = 9,
    DMA_REQ_UART5_RX = 10,
    DMA_REQ_UART5_TX = 11,
    DMA_REQ_UART6_RX = 12,
    DMA_REQ_UART6_TX = 13,
    DMA_REQ_UART7_RX = 14,
    DMA_REQ_UART7_TX = 15,
    DMA_REQ_SAI0_RX  = 16,
    DMA_REQ_SAI0_TX  = 17,
    DMA_REQ_SAI1_RX  = 18,
    DMA_REQ_SAI1_TX  = 19,
    DMA_REQ_SAI2_RX  = 20,
    DMA_REQ_SAI2_TX  = 21,
    DMA_REQ_ASRC0_RX = 22,
    DMA_REQ_ASRC0_TX = 23,
    DMA_REQ_ASRC1_RX = 24,
    DMA_REQ_ASRC1_TX = 25,
    DMA_REQ_PDM_RX   = 26,
    DMA_REQ_I2C0_RX  = 28,
    DMA_REQ_I2C0_TX  = 29,
    DMA_REQ_I2C1_RX  = 30,
    DMA_REQ_I2C1_TX  = 31,
    DMA_REQ_I2C3_RX  = 34,
    DMA_REQ_I2C3_TX  = 35,
    DMA_REQ_I2C4_RX  = 36,
    DMA_REQ_I2C4_TX  = 37,
    DMA_REQ_I2C5_RX  = 38,
    DMA_REQ_I2C5_TX  = 39,
    DMA_REQ_SPI0_RX  = 40,
    DMA_REQ_SPI0_TX  = 41,
    DMA_REQ_SPI1_RX  = 42,
    DMA_REQ_SPI1_TX  = 43,
    DMA_REQ_CAN0_RX  = 44,
    DMA_REQ_CAN1_RX  = 45,
    DMA_REQ_DSMC0    = 46,
    DMA_REQ_DSMC1    = 47,
} DMA_REQ_Type;

/* ================================================================================ */
/* ================                       IRQ                      ================ */
/* ================================================================================ */
#ifdef HAL_MCU_CORE
typedef enum
{
/******  Platform Exceptions Numbers ***************************************************/
    GPIO0_IRQn           = 0,
    GPIO0_EXP1_IRQn      = 1,
    GPIO0_EXP2_IRQn      = 2,
    GPIO0_EXP3_IRQn      = 3,
    GPIO1_IRQn           = 4,
    GPIO1_EXP1_IRQn      = 5,
    GPIO1_EXP2_IRQn      = 6,
    GPIO1_EXP3_IRQn      = 7,
    GPIO2_IRQn           = 8,
    GPIO2_EXP1_IRQn      = 9,
    GPIO2_EXP2_IRQn      = 10,
    GPIO2_EXP3_IRQn      = 11,
    GPIO3_IRQn           = 12,
    GPIO3_EXP1_IRQn      = 13,
    GPIO3_EXP2_IRQn      = 14,
    GPIO3_EXP3_IRQn      = 15,
    GPIO4_IRQn           = 16,
    GPIO4_EXP1_IRQn      = 17,
    GPIO4_EXP2_IRQn      = 18,
    GPIO4_EXP3_IRQn      = 19,
    GPIO5_IRQn           = 20,
    GPIO5_EXP1_IRQn      = 21,
    GPIO5_EXP2_IRQn      = 22,
    GPIO5_EXP3_IRQn      = 23,
    GPIO6_IRQn           = 24,
    GPIO6_EXP1_IRQn      = 25,
    GPIO6_EXP2_IRQn      = 26,
    GPIO6_EXP3_IRQn      = 27,
    GPIO7_IRQn           = 28,
    GPIO7_EXP1_IRQn      = 29,
    GPIO7_EXP2_IRQn      = 30,
    GPIO7_EXP3_IRQn      = 31,
    PWM0_CH0_IRQn        = 40,
    PWM0_CH1_IRQn        = 41,
    PWM0_CH2_IRQn        = 42,
    PWM0_CH3_IRQn        = 43,
    PWM0_CH4_IRQn        = 44,
    PWM0_CH5_IRQn        = 45,
    PWM0_CH6_IRQn        = 46,
    PWM0_CH7_IRQn        = 47,
    I2C0_IRQn            = 48,
    I2C1_IRQn            = 49,
    I2C2_IRQn            = 50,
    I2C3_IRQn            = 51,
    I2C4_IRQn            = 52,
    I2C5_IRQn            = 53,
    UART0_IRQn           = 56,
    UART1_IRQn           = 57,
    UART2_IRQn           = 58,
    UART3_IRQn           = 59,
    UART4_IRQn           = 60,
    UART5_IRQn           = 61,
    UART6_IRQn           = 62,
    UART7_IRQn           = 63,
    TIMER0_IRQn          = 67,
    TIMER1_IRQn          = 68,
    TIMER2_IRQn          = 69,
    TIMER3_IRQn          = 70,
    TIMER4_IRQn          = 71,
    TIMER5_IRQn          = 72,
    RKDMA_IRQn           = 81,
    HPMCU_MBOX0_AP_IRQn  = 109,
    HPMCU_MBOX1_AP_IRQn  = 110,
    HPMCU_MBOX2_AP_IRQn  = 111,
    HPMCU_MBOX3_AP_IRQn  = 112,
    HPMCU_MBOX0_BB_IRQn  = 113,
    HPMCU_MBOX1_BB_IRQn  = 114,
    HPMCU_MBOX2_BB_IRQn  = 115,
    HPMCU_MBOX3_BB_IRQn  = 116,
    VICAP_IRQn           = 153,
    MIPI_ISP_IRQn        = 156,
    MI_ISP_IRQn          = 157,
    ISP_IRQn             = 158,
    CAN0_IRQn            = 204,
    CAN1_IRQn            = 205,
    SARADC0_IRQn         = 211,
    SARADC1_IRQn         = 212,
    SARADC2_IRQn         = 213,
    PWM1_CH0_IRQn        = 220,
    PWM1_CH1_IRQn        = 221,
    PWM1_CH2_IRQn        = 222,
    PWM1_CH3_IRQn        = 223,
    PWM2_CH0_IRQn        = 224,
    PWM2_CH1_IRQn        = 225,
    PWM2_CH2_IRQn        = 226,
    PWM2_CH3_IRQn        = 227,
    PWM2_CH4_IRQn        = 228,
    PWM2_CH5_IRQn        = 229,
    PWM2_CH6_IRQn        = 230,
    PWM2_CH7_IRQn        = 231,
    PWM3_CH0_IRQn        = 232,
    PWM3_CH1_IRQn        = 233,
    PWM3_CH2_IRQn        = 234,
    PWM3_CH3_IRQn        = 235,
    PWM3_CH4_IRQn        = 236,
    PWM3_CH5_IRQn        = 237,
    PWM3_CH6_IRQn        = 238,
    PWM3_CH7_IRQn        = 239,
    NUM_INTERRUPTS       = 256,
} IRQn_Type;
#endif
/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */
#ifdef HAL_MCU_CORE
#define __RISC_V
#endif

#ifndef __ASSEMBLY__
#include "cmsis_compiler.h"               /* CMSIS compiler specific defines */
#include "system_rv1126b.h"
#endif /* __ASSEMBLY__ */
#include "rv1126b.h"
#include "rv1126b_cru.h"

/*****************************************CACHE*****************************************/
#ifdef HAL_AP_CORE
/* CACHE LINE SIZE */
#define CACHE_LINE_SHIFT (6U)
#define CACHE_LINE_SIZE  (0x1U << CACHE_LINE_SHIFT)
#else
/* CACHE LINE SIZE */
#define CACHE_LINE_SHIFT                (5U)
#define CACHE_LINE_SIZE                 (0x1U << CACHE_LINE_SHIFT)
#define CACHE_LINE_ADDR_MASK            (0xFFFFFFFFU << CACHE_LINE_SHIFT)
#define CACHE_M_CLEAN                   0x0U
#define CACHE_M_INVALID                 0x2U
#define CACHE_M_CLEAN_INVALID           0x4U
#define CACHE_M_INVALID_ALL             0x6U
#define CACHE_REVISION                  (0x00000100U)
#endif

/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************CRU*******************************************/
#define PLL_INPUT_OSC_RATE (24 * 1000 * 1000)
#define CRU_CLK_USE_CON_BANK
#define CLK64(mux, div) ((((mux) & 0xffffffffULL) << 32) | ((div) & 0xffffffffULL))

#ifndef __ASSEMBLY__
typedef enum CLOCK_Name {
    CLK_INVALID = 0U,
    PLL_GPLL,
    PLL_CPLL,
    PLL_AUPLL,

    /* TOP */
    CLK_CM_FRAC0 = CLK64(CLK_CM_FRAC_MATRIX0_SEL, CLK_CM_FRAC_MATRIX0_DIV),
    CLK_CM_FRAC1 = CLK64(CLK_CM_FRAC_MATRIX1_SEL, CLK_CM_FRAC_MATRIX1_DIV),
    CLK_CM_FRAC2 = CLK64(CLK_CM_FRAC_MATRIX2_SEL, CLK_CM_FRAC_MATRIX2_DIV),

    CLK_UART_FRAC0  = CLK64(CLK_UART_FRAC_MATRIX0_SEL, CLK_UART_FRAC_MATRIX0_DIV),
    CLK_UART_FRAC1  = CLK64(CLK_UART_FRAC_MATRIX1_SEL, CLK_UART_FRAC_MATRIX1_DIV),
    CLK_AUDIO_FRAC0 = CLK64(CLK_AUDIO_FRAC_MATRIX0_SEL, CLK_AUDIO_FRAC_MATRIX0_DIV),
    CLK_AUDIO_FRAC1 = CLK64(CLK_AUDIO_FRAC_MATRIX1_SEL, CLK_AUDIO_FRAC_MATRIX1_DIV),

    CLK_AUDIO_INT0 = CLK64(CLK_AUDIO_INT_MATRIX0_SEL, CLK_AUDIO_INT_MATRIX0_DIV),
    CLK_AUDIO_INT1 = CLK64(CLK_AUDIO_INT_MATRIX1_SEL, CLK_AUDIO_INT_MATRIX1_DIV),

    SCLK_UART0_SRC = CLK64(SCLK_UART0_SRC_SEL, SCLK_UART0_SRC_DIV),
    SCLK_UART1     = CLK64(SCLK_UART1_SRC_SEL, SCLK_UART1_SRC_DIV),
    SCLK_UART2     = CLK64(SCLK_UART2_SRC_SEL, SCLK_UART2_SRC_DIV),
    SCLK_UART3     = CLK64(SCLK_UART3_SRC_SEL, SCLK_UART3_SRC_DIV),
    SCLK_UART4     = CLK64(SCLK_UART4_SRC_SEL, SCLK_UART4_SRC_DIV),
    SCLK_UART5     = CLK64(SCLK_UART5_SRC_SEL, SCLK_UART5_SRC_DIV),
    SCLK_UART6     = CLK64(SCLK_UART6_SRC_SEL, SCLK_UART6_SRC_DIV),
    SCLK_UART7     = CLK64(SCLK_UART7_SRC_SEL, SCLK_UART7_SRC_DIV),

    MCLK_SAI0  = CLK64(MCLK_SAI0_SRC_SEL, MCLK_SAI0_SRC_DIV),
    MCLK_ASRC0 = CLK64(MCLK_ASRC0_SRC_SEL, 0U),
    MCLK_SAI1  = CLK64(MCLK_SAI1_SRC_SEL, MCLK_SAI1_SRC_DIV),
    MCLK_ASRC1 = CLK64(MCLK_ASRC1_SRC_SEL, 0U),
    MCLK_SAI2  = CLK64(MCLK_SAI2_SRC_SEL, MCLK_SAI2_SRC_DIV),
    MCLK_ASRC2 = CLK64(MCLK_ASRC2_SRC_SEL, 0U),
    MCLK_PDM   = CLK64(MCLK_PDM_SRC_SEL, MCLK_PDM_SRC_DIV),
    MCLK_ASRC3 = CLK64(MCLK_ASRC3_SRC_SEL, 0U),
    CLKOUT_PDM = CLK64(CLKOUT_PDM_SRC_SEL, CLKOUT_PDM_SRC_DIV),
    CLK_ASRC0  = CLK64(CLK_ASRC0_SRC_SEL, CLK_ASRC0_SRC_DIV),
    CLK_ASRC1  = CLK64(CLK_ASRC1_SRC_SEL, CLK_ASRC1_SRC_DIV),

    CLK_CORE_RGA = CLK64(CLK_CORE_RGA_SRC_SEL, CLK_CORE_RGA_SRC_DIV),
    CLK_SYS_DSMC = CLK64(CLK_SYS_DSMC_SRC_SEL, CLK_SYS_DSMC_SRC_DIV),
    ACLK_VEPU    = CLK64(ACLK_VEPU_SRC_SEL, 0U),
    ACLK_GMAC    = CLK64(ACLK_GMAC_SRC_SEL, 0U),
    ACLK_VI      = CLK64(ACLK_VI_SRC_SEL, 0U),
    ACLK_VDO     = CLK64(ACLK_VDO_SRC_SEL, ACLK_VDO_SRC_DIV),
    ACLK_RKVDEC  = CLK64(ACLK_RKVDEC_SRC_SEL, ACLK_RKVDEC_SRC_DIV),
    DCLK_VICAP   = CLK64(DCLK_VICAP_SRC_SEL, 0U),

    DCLK_VOP    = CLK64(DCLK_VOP_SRC_SEL, DCLK_VOP_SRC_DIV),
    ACLK_SYSMEM = CLK64(ACLK_SYSMEM_SRC_SEL, ACLK_SYSMEM_SRC_DIV),
    ACLK_TOP    = CLK64(ACLK_TOP_SRC_SEL, 0U),
    ACLK_BUS    = CLK64(ACLK_BUS_SRC_SEL, 0U),
    HCLK_BUS    = CLK64(HCLK_BUS_SRC_SEL, 0U),

    CLK_GMAC_PTP_REF = CLK64(CLK_GMAC_PTP_REF_SRC_SEL, CLK_GMAC_PTP_REF_SRC_DIV),
    CCLK_SDMMC0      = CLK64(CCLK_SDMMC0_SRC_SEL, CCLK_SDMMC0_SRC_DIV),
    CCLK_SDMMC1      = CLK64(CCLK_SDMMC1_SRC_SEL, CCLK_SDMMC1_SRC_DIV),
    CLK_TIMER        = CLK64(CLK_TIMER_SRC_SEL, 0U),
    TCLK_WDT_NS      = CLK64(TCLK_WDT_NS_SRC_SEL, 0U),
    TCLK_WDT_S       = CLK64(TCLK_WDT_S_SRC_SEL, 0U),
    TCLK_WDT_HPMCU   = CLK64(TCLK_WDT_HPMCU_SRC_SEL, 0U),
    CCLK_EMMC        = CLK64(CCLK_EMMC_SRC_SEL, CCLK_EMMC_SRC_DIV),
    PCLK_PERI        = CLK64(PCLK_PERI_SRC_SEL, 0U),
    ACLK_PERI        = CLK64(ACLK_PERI_SRC_SEL, 0U),
    SCLK_2X_FSPI0    = CLK64(SCLK_2X_FSPI0_SRC_SEL, SCLK_2X_FSPI0_SRC_DIV),
    ACLK_VCP         = CLK64(ACLK_VCP_SRC_SEL, 0U),
    CLK_CAN0         = CLK64(CLK_CAN0_SRC_SEL, CLK_CAN0_SRC_DIV),
    CLK_CAN1         = CLK64(CLK_CAN1_SRC_SEL, CLK_CAN1_SRC_DIV),

    CLK_I2C_BUS = CLK64(CLK_I2C_BUS_SRC_SEL, 0U),
    CLK_SPI0    = CLK64(CLK_SPI0_SRC_SEL, 0U),
    CLK_SPI1    = CLK64(CLK_SPI1_SRC_SEL, 0U),
    CLK_PWM0    = CLK64(CLK_PWM0_SRC_SEL, 0U),
    CLK_PWM2    = CLK64(CLK_PWM2_SRC_SEL, 0U),
    CLK_PWM3    = CLK64(CLK_PWM3_SRC_SEL, 0U),

    CLK_PKA_RKCE = CLK64(CLK_PKA_RKCE_SRC_SEL, 0U),
    ACLK_RKCE    = CLK64(ACLK_RKCE_SRC_SEL, 0U),
    CLK_CORE_FEC = CLK64(CLK_CORE_FEC_SRC_SEL, CLK_CORE_FEC_SRC_DIV),

    CLK_ISP_PLL  = CLK64(CLK_ISP_PLL_SRC_SEL, CLK_ISP_PLL_SRC_DIV),
    CLK_AISP_PLL = CLK64(CLK_AISP_PLL_SRC_SEL, CLK_AISP_PLL_SRC_DIV),
    DCLK_OOC     = CLK64(DCLK_OOC_SRC_SEL, DCLK_OOC_SRC_DIV),

    CLK_SARADC0 = CLK64(CLK_SARADC0_SRC_SEL, CLK_SARADC0_SRC_DIV),
    CLK_SARADC1 = CLK64(CLK_SARADC1_SRC_SEL, CLK_SARADC1_SRC_DIV),
    CLK_SARADC2 = CLK64(CLK_SARADC2_SRC_SEL, CLK_SARADC2_SRC_DIV),

    CLK_MIPI0_OUT2IO = CLK64(CLK_MIPI0_OUT2IO_SEL, CLK_MIPI0_OUT2IO_DIV),
    CLK_MIPI1_OUT2IO = CLK64(CLK_MIPI1_OUT2IO_SEL, CLK_MIPI1_OUT2IO_DIV),
    CLK_MIPI2_OUT2IO = CLK64(CLK_MIPI2_OUT2IO_SEL, CLK_MIPI2_OUT2IO_DIV),
    CLK_MIPI3_OUT2IO = CLK64(CLK_MIPI3_OUT2IO_SEL, CLK_MIPI3_OUT2IO_DIV),
    CLK_CIF_OUT2IO   = CLK64(CLK_CIF_OUT2IO_SEL, CLK_CIF_OUT2IO_DIV),
    CLK_MAC_OUT2IO   = CLK64(CLK_MAC_OUT2IO_SEL, CLK_MAC_OUT2IO_DIV),
    MCLK_SAI0_OUT2IO = CLK64(0U, MCLK_SAI0_OUT2IO_DIV),
    MCLK_SAI1_OUT2IO = CLK64(0U, MCLK_SAI1_OUT2IO_DIV),
    MCLK_SAI2_OUT2IO = CLK64(0U, MCLK_SAI2_OUT2IO_DIV),
    LRCK_SRC_ASRC0   = CLK64(LRCK_SRC_ASRC0_SEL, 0U),
    LRCK_DST_ASRC0   = CLK64(LRCK_DST_ASRC0_SEL, 0U),
    LRCK_SRC_ASRC1   = CLK64(LRCK_SRC_ASRC1_SEL, 0U),
    LRCK_DST_ASRC1   = CLK64(LRCK_DST_ASRC1_SEL, 0U),

    CLK_PWM1       = CLK64(CLK_PWM1_SEL, CLK_PWM1_DIV),
    CLK_I2C2       = CLK64(CLK_I2C2_SEL, CLK_I2C2_DIV),
    SCLK_UART0     = CLK64(SCLK_UART0_SEL, 0U),
    TCLK_WDT_LPMCU = CLK64(TCLK_WDT_LPMCU_SEL, 0U),

    SCLK_1X_FSPI1 = CLK64(SCLK_1X_FSPI1_SEL, SCLK_1X_FSPI1_DIV),
    MCLK_LPSAI    = CLK64(MCLK_LPSAI_SEL, MCLK_LPSAI_DIV),
} eCLOCK_Name;
#endif /* __ASSEMBLY__ */

/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/

/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************MBOX******************************************/
#define LPMCU_MBOX0    MBOX0
#define LPMCU_MBOX1    MBOX1
#define LPMCU_MBOX2    MBOX2
#define LPMCU_MBOX3    MBOX3
#define HPMCU_MBOX0    MBOX4
#define HPMCU_MBOX1    MBOX5
#define HPMCU_MBOX2    MBOX6
#define HPMCU_MBOX3    MBOX7
#define MBOX_CNT       8         /* Total Mailbox in SoC */

/******************************************SARADC****************************************/
#define PCLK_SARADC_CONTROL_GATE0 CLK_SARADC0_SRC_GATE
#define PCLK_SARADC_CONTROL_GATE1 CLK_SARADC1_SRC_GATE
#define PCLK_SARADC_CONTROL_GATE2 CLK_SARADC2_SRC_GATE

#define CLK_SARADC_GATE0 CLK_SARADC0
#define CLK_SARADC_GATE1 CLK_SARADC1
#define CLK_SARADC_GATE2 CLK_SARADC2

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __SOC_H */
