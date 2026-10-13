/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#ifdef HAL_CANFD_MODULE_ENABLED
const struct HAL_CANFD_DEV g_can0Dev =
{
    .pReg = CAN0,
    .sclkID = CLK_CAN0,
    .sclkGateID = CLK_CAN0_SRC_GATE,
    .pclkGateID = HCLK_CAN0_GATE,
    .irqNum = CAN0_IRQn,
};

const struct HAL_CANFD_DEV g_can1Dev =
{
    .pReg = CAN1,
    .sclkID = CLK_CAN1,
    .sclkGateID = CLK_CAN1_SRC_GATE,
    .pclkGateID = HCLK_CAN1_GATE,
    .irqNum = CAN1_IRQn,
};
#endif

#ifdef HAL_CRU_MODULE_ENABLED
static struct CRU_BANK_INFO cruBanks[] = {
    CRU_BANK_CFG_FLAGS(TOPCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(BUSCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PERICRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(CORECRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMUCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMU1CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDRCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SUBDDRCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(VICRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(VEPUCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(NPUCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(VDOCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(VCPCRU_BASE, 0x300, 0x800, 0xa00),
};

const struct HAL_CRU_DEV g_cruDev = {
    .banks = cruBanks,
    .banksNum = HAL_ARRAY_SIZE(cruBanks),
};
#endif

#ifdef HAL_I2C_MODULE_ENABLED
const struct HAL_I2C_DEV g_i2c0Dev =
{
    .pReg = I2C0,
    .clkID = CLK_I2C_BUS,
    .clkGateID = CLK_I2C0_GATE,
    .pclkGateID = PCLK_I2C0_GATE,
    .irqNum = I2C0_IRQn,
    .runtimeID = PM_RUNTIME_ID_I2C0,
};

const struct HAL_I2C_DEV g_i2c1Dev =
{
    .pReg = I2C1,
    .clkID = CLK_I2C_BUS,
    .clkGateID = CLK_I2C1_GATE,
    .pclkGateID = PCLK_I2C1_GATE,
    .irqNum = I2C1_IRQn,
    .runtimeID = PM_RUNTIME_ID_I2C1,
};

const struct HAL_I2C_DEV g_i2c2Dev =
{
    .pReg = I2C2,
    .clkID = CLK_I2C2,
    .clkGateID = CLK_I2C2_GATE,
    .pclkGateID = PCLK_I2C2_GATE,
    .irqNum = I2C2_IRQn,
    .runtimeID = PM_RUNTIME_ID_I2C2,
};

const struct HAL_I2C_DEV g_i2c3Dev =
{
    .pReg = I2C3,
    .clkID = CLK_I2C_BUS,
    .clkGateID = CLK_I2C3_GATE,
    .pclkGateID = PCLK_I2C3_GATE,
    .irqNum = I2C3_IRQn,
    .runtimeID = PM_RUNTIME_ID_I2C3,
};

const struct HAL_I2C_DEV g_i2c4Dev =
{
    .pReg = I2C4,
    .clkID = CLK_I2C_BUS,
    .clkGateID = CLK_I2C4_GATE,
    .pclkGateID = PCLK_I2C4_GATE,
    .irqNum = I2C4_IRQn,
    .runtimeID = PM_RUNTIME_ID_I2C4,
};

const struct HAL_I2C_DEV g_i2c5Dev =
{
    .pReg = I2C5,
    .clkID = CLK_I2C_BUS,
    .clkGateID = CLK_I2C5_GATE,
    .pclkGateID = PCLK_I2C5_GATE,
    .irqNum = I2C5_IRQn,
    .runtimeID = PM_RUNTIME_ID_I2C5,
};
#endif

#ifdef HAL_PWM_MODULE_ENABLED
const struct HAL_PWM_DEV g_pwm0Dev =
{
    .pReg = PWM0,
    .clkID = CLK_PWM0,
    .clkGateID = CLK_PWM0_SRC_GATE,
    .pclkGateID = PCLK_PWM0_GATE,
    .irqNum[0] = PWM0_CH0_IRQn,
    .irqNum[1] = PWM0_CH1_IRQn,
    .irqNum[2] = PWM0_CH2_IRQn,
    .irqNum[3] = PWM0_CH3_IRQn,
    .irqNum[4] = PWM0_CH4_IRQn,
    .irqNum[5] = PWM0_CH5_IRQn,
    .irqNum[6] = PWM0_CH6_IRQn,
    .irqNum[7] = PWM0_CH7_IRQn,
};

const struct HAL_PWM_DEV g_pwm1Dev =
{
    .pReg = PWM1,
    .clkID = CLK_PWM1,
    .clkGateID = CLK_PWM1_GATE,
    .pclkGateID = PCLK_PWM1_GATE,
    .irqNum[0] = PWM1_CH0_IRQn,
    .irqNum[1] = PWM1_CH1_IRQn,
    .irqNum[2] = PWM1_CH2_IRQn,
    .irqNum[3] = PWM1_CH3_IRQn,
};

const struct HAL_PWM_DEV g_pwm2Dev =
{
    .pReg = PWM2,
    .clkID = CLK_PWM2,
    .clkGateID = CLK_PWM2_SRC_GATE,
    .pclkGateID = PCLK_PWM2_GATE,
    .irqNum[0] = PWM2_CH0_IRQn,
    .irqNum[1] = PWM2_CH1_IRQn,
    .irqNum[2] = PWM2_CH2_IRQn,
    .irqNum[3] = PWM2_CH3_IRQn,
    .irqNum[4] = PWM2_CH4_IRQn,
    .irqNum[5] = PWM2_CH5_IRQn,
    .irqNum[6] = PWM2_CH6_IRQn,
    .irqNum[7] = PWM2_CH7_IRQn,
};

const struct HAL_PWM_DEV g_pwm3Dev =
{
    .pReg = PWM3,
    .clkID = CLK_PWM3,
    .clkGateID = CLK_PWM3_SRC_GATE,
    .pclkGateID = PCLK_PWM3_GATE,
    .irqNum[0] = PWM3_CH0_IRQn,
    .irqNum[1] = PWM3_CH1_IRQn,
    .irqNum[2] = PWM3_CH2_IRQn,
    .irqNum[3] = PWM3_CH3_IRQn,
    .irqNum[4] = PWM3_CH4_IRQn,
    .irqNum[5] = PWM3_CH5_IRQn,
    .irqNum[6] = PWM3_CH6_IRQn,
    .irqNum[7] = PWM3_CH7_IRQn,
};
#endif

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART0,
    .sclkGateID = SCLK_UART0_SRC_GATE,
    .pclkGateID = PCLK_UART0_GATE,
#endif
    .irqNum = UART0_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart1Dev =
{
    .pReg = UART1,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART1,
    .sclkGateID = SCLK_UART1_SRC_GATE,
    .pclkGateID = PCLK_UART1_GATE,
#endif
    .irqNum = UART1_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART2,
    .sclkGateID = SCLK_UART2_SRC_GATE,
    .pclkGateID = PCLK_UART2_GATE,
#endif
    .irqNum = UART2_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart3Dev =
{
    .pReg = UART3,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART3,
    .sclkGateID = SCLK_UART3_SRC_GATE,
    .pclkGateID = PCLK_UART3_GATE,
#endif
    .irqNum = UART3_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart4Dev =
{
    .pReg = UART4,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART4,
    .sclkGateID = SCLK_UART4_SRC_GATE,
    .pclkGateID = PCLK_UART4_GATE,
#endif
    .irqNum = UART4_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart5Dev =
{
    .pReg = UART5,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART5,
    .sclkGateID = SCLK_UART5_SRC_GATE,
    .pclkGateID = PCLK_UART5_GATE,
#endif
    .irqNum = UART5_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart6Dev =
{
    .pReg = UART6,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART6,
    .sclkGateID = SCLK_UART6_SRC_GATE,
    .pclkGateID = PCLK_UART6_GATE,
#endif
    .irqNum = UART6_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart7Dev =
{
    .pReg = UART7,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = SCLK_UART7,
    .sclkGateID = SCLK_UART7_SRC_GATE,
    .pclkGateID = PCLK_UART7_GATE,
#endif
    .irqNum = UART7_IRQn,
    .isAutoFlow = false,
};
#endif

void BSP_Init(void)
{
}
