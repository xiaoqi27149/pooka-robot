
/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_PINCTRL_MODULE_ENABLED) && defined(SOC_RV1126B)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup PINCTRL
 *  @{
 */

/** @defgroup PINCTRL_How_To_Use How To Use
 *  @{

 The pinctrl setting registers actually is bus grf registers, which include
 iomux, drive strength, pull mode, slew rate and schmitt trigger.

 The pinctrl driver provides APIs:
   - HAL_PINCTRL_SetIOMUX() to set pin iomux
   - HAL_PINCTRL_SetParam() to set pin iomux/pull/drive strength/schmitt trigger

 Example:

     HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                          GPIO_PIN_A5,  // UART5_TX_M2
                          PIN_CONFIG_MUX_FUNC9);

     HAL_PINCTRL_SetParam(GPIO_BANK2,
                          GPIO_PIN_A5,  // UART5_TX_M2
                          PIN_CONFIG_MUX_FUNC9 |
                          PIN_CONFIG_PUL_UP |
                          PIN_CONFIG_DRV_LEVEL2);
 @} */

/** @defgroup PINCTRL_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define _TO_MASK(w)          ((1U << (w)) - 1U)
#define _TO_OFFSET(gp, w)    ((gp) * (w))
#define RK_GEN_VAL(gp, v, w) ((_TO_MASK(w) << (_TO_OFFSET(gp, w) + 16)) | (((v) & _TO_MASK(w)) << _TO_OFFSET(gp, w)))
#define PIN_NUM_PER_GROUP    8

/*
 * Use HAL_DBG("pinctrl: write val = 0x%lx to register %p\n", VAL, &REG);
 * and HAL_DBG("pinctrl: readback register %p = 0x%lx\n", &REG, REG);
 * for debug
 */
#define _PINCTRL_WRITE(REG, VAL) \
{                                \
    REG = VAL;                   \
}

#define IOMUX_4_BIT_PER_PIN    (4)
#define IOMUX_4_PIN_PER_REG    (4)
#define IOMUX_0(__B, __G)      (GPIO##__B##_IOC->GPIO##__B##__G##_IOMUX_SEL_0)
#define IOMUX_N(__B, __G, __N) (GPIO##__B##_IOC->GPIO##__B##__G##_IOMUX_SEL_##__N)
#define SET_IOMUX_0(_B, _G, _GP, _V, _W)                     \
do {                                                         \
    _PINCTRL_WRITE(IOMUX_0(_B, _G), RK_GEN_VAL(_GP, _V, _W)) \
} while (0)
#define SET_IOMUX_1(_B, _G, _N, _GP, _V, _W)                        \
do {                                                                \
    if (_N == 0)                                                    \
        _PINCTRL_WRITE(IOMUX_N(_B, _G, 0), RK_GEN_VAL(_GP, _V, _W)) \
    else if (_N == 1)                                               \
        _PINCTRL_WRITE(IOMUX_N(_B, _G, 1), RK_GEN_VAL(_GP, _V, _W)) \
    else                                                            \
        HAL_DBG("unknown IOMUX_SEL_%d \n", _N);                     \
} while (0)

#define PULL_2_BIT_PER_PIN (2)
#define PULL_8_PIN_PER_REG (8)
#define PULL(__B, __G)     (GPIO##__B##_IOC->GPIO##__B##__G##_PULL)
#define SET_PULL(_B, _G, _GP, _V, _W)                     \
do {                                                      \
    _PINCTRL_WRITE(PULL(_B, _G), RK_GEN_VAL(_GP, _V, _W)) \
} while (0)

#define DRV_8_BIT_PER_PIN    (8)
#define DRV_2_PIN_PER_REG    (2)
#define DRV_N(__B, __G, __N) (GPIO##__B##_IOC->GPIO##__B##__G##_DS_##__N)
#define SET_DRV_0(_B, _G, _N, _GP, _V, _W)                        \
do {                                                              \
    if (_N == 0)                                                  \
        _PINCTRL_WRITE(DRV_N(_B, _G, 0), RK_GEN_VAL(_GP, _V, _W)) \
    else                                                          \
        HAL_DBG("unknown DS_%d \n", _N);                          \
} while (0)
#define SET_DRV_1(_B, _G, _N, _GP, _V, _W)                        \
do {                                                              \
    if (_N == 0)                                                  \
        _PINCTRL_WRITE(DRV_N(_B, _G, 0), RK_GEN_VAL(_GP, _V, _W)) \
    else if (_N == 1)                                             \
        _PINCTRL_WRITE(DRV_N(_B, _G, 1), RK_GEN_VAL(_GP, _V, _W)) \
    else                                                          \
        HAL_DBG("unknown DS_%d \n", _N);                          \
} while (0)
#define SET_DRV_2(_B, _G, _N, _GP, _V, _W)                        \
do {                                                              \
    if (_N == 0)                                                  \
        _PINCTRL_WRITE(DRV_N(_B, _G, 0), RK_GEN_VAL(_GP, _V, _W)) \
    else if (_N == 1)                                             \
        _PINCTRL_WRITE(DRV_N(_B, _G, 1), RK_GEN_VAL(_GP, _V, _W)) \
    else if (_N == 2)                                             \
        _PINCTRL_WRITE(DRV_N(_B, _G, 2), RK_GEN_VAL(_GP, _V, _W)) \
    else                                                          \
        HAL_DBG("unknown DS_%d \n", _N);                          \
} while (0)
#define SET_DRV_3(_B, _G, _N, _GP, _V, _W)                        \
do {                                                              \
    if (_N == 0)                                                  \
        _PINCTRL_WRITE(DRV_N(_B, _G, 0), RK_GEN_VAL(_GP, _V, _W)) \
    else if (_N == 1)                                             \
        _PINCTRL_WRITE(DRV_N(_B, _G, 1), RK_GEN_VAL(_GP, _V, _W)) \
    else if (_N == 2)                                             \
        _PINCTRL_WRITE(DRV_N(_B, _G, 2), RK_GEN_VAL(_GP, _V, _W)) \
    else if (_N == 3)                                             \
        _PINCTRL_WRITE(DRV_N(_B, _G, 3), RK_GEN_VAL(_GP, _V, _W)) \
    else                                                          \
        HAL_DBG("unknown DS_%d \n", _N);                          \
} while (0)

#define SR_2_BIT_PER_PIN (2)
#define SR_8_PIN_PER_REG (8)
#define SR(__B, __G)     (GPIO##__B##_IOC->GPIO##__B##__G##_SL)
#define SET_SR(_B, _G, _GP, _V, _W)                     \
do {                                                    \
    _PINCTRL_WRITE(SR(_B, _G), RK_GEN_VAL(_GP, _V, _W)) \
} while (0)

#define SMT_1_BIT_PER_PIN (1)
#define SMT_8_PIN_PER_REG (8)
#define SMT(__B, __G)     (GPIO##__B##_IOC->GPIO##__B##__G##_SMT)
#define SET_SMT(_B, _G, _GP, _V, _W)                     \
do {                                                     \
    _PINCTRL_WRITE(SMT(_B, _G), RK_GEN_VAL(_GP, _V, _W)) \
} while (0)

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/**
 * @brief  Private function to set pin iomux.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetIOMUX(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case GPIO_BANK0:
        if (pin < 8) {
            SET_IOMUX_1(0, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 13) {
            SET_IOMUX_1(0, B, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin >= 16 && pin < 24) {
            SET_IOMUX_1(0, C, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin >= 24 && pin < 26) {
            SET_IOMUX_0(0, D, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK1:
        if (pin < 8) {
            SET_IOMUX_1(1, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_IOMUX_1(1, B, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK2:
        if (pin < 6) {
            SET_IOMUX_1(2, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK3:
        if (pin < 8) {
            SET_IOMUX_1(3, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_IOMUX_1(3, B, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK4:
        if (pin < 8) {
            SET_IOMUX_1(4, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_IOMUX_0(4, B, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK5:
        if (pin < 8) {
            SET_IOMUX_1(5, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_IOMUX_1(5, B, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_IOMUX_1(5, C, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 32) {
            SET_IOMUX_1(5, D, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK6:
        if (pin < 8) {
            SET_IOMUX_1(6, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_IOMUX_1(6, B, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 20) {
            SET_IOMUX_0(6, C, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK7:
        if (pin < 8) {
            SET_IOMUX_1(7, A, pin % PIN_NUM_PER_GROUP / IOMUX_4_PIN_PER_REG, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_IOMUX_0(7, B, pin % IOMUX_4_PIN_PER_REG, val, IOMUX_4_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin pull.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetPULL(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case GPIO_BANK0:
        if (pin < 8) {
            SET_PULL(0, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 13) {
            SET_PULL(0, B, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin >= 16 && pin < 24) {
            SET_PULL(0, C, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin >= 24 && pin < 26) {
            SET_PULL(0, D, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK1:
        if (pin < 8) {
            SET_PULL(1, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_PULL(1, B, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK2:
        if (pin < 6) {
            SET_PULL(2, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK3:
        if (pin < 8) {
            SET_PULL(3, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_PULL(3, B, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK4:
        if (pin < 8) {
            SET_PULL(4, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_PULL(4, B, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK5:
        if (pin < 8) {
            SET_PULL(5, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_PULL(5, B, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_PULL(5, C, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 32) {
            SET_PULL(5, D, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK6:
        if (pin < 8) {
            SET_PULL(6, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_PULL(6, B, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_PULL(6, C, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK7:
        if (pin < 8) {
            SET_PULL(7, A, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_PULL(7, B, pin % PULL_8_PIN_PER_REG, val, PULL_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin drive strength.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetDRV(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case GPIO_BANK0:
        if (pin < 8) {
            SET_DRV_3(0, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 13) {
            SET_DRV_2(0, B, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin >= 16 && pin < 24) {
            SET_DRV_3(0, C, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin >= 24 && pin < 26) {
            SET_DRV_0(0, C, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK1:
        if (pin < 8) {
            SET_DRV_3(1, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_DRV_3(1, B, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK2:
        if (pin < 6) {
            SET_DRV_2(2, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK3:
        if (pin < 8) {
            SET_DRV_3(3, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_DRV_3(3, B, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK4:
        if (pin < 8) {
            SET_DRV_3(1, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_DRV_0(1, B, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK5:
        if (pin < 8) {
            SET_DRV_3(5, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_DRV_3(5, B, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_DRV_3(5, C, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 32) {
            SET_DRV_3(5, C, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK6:
        if (pin < 8) {
            SET_DRV_3(6, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_DRV_3(6, B, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_DRV_3(6, C, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK7:
        if (pin < 8) {
            SET_DRV_3(7, A, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_DRV_0(7, B, pin % PIN_NUM_PER_GROUP / DRV_2_PIN_PER_REG, pin % DRV_2_PIN_PER_REG, val, DRV_8_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin slew rate.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetSR(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case GPIO_BANK0:
        if (pin < 8) {
            SET_SR(0, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 13) {
            SET_SR(0, B, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin >= 16 && pin < 24) {
            SET_SR(0, C, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin >= 24 && pin < 26) {
            SET_SR(0, D, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK1:
        if (pin < 8) {
            SET_SR(1, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SR(1, B, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK2:
        if (pin < 6) {
            SET_SR(2, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK3:
        if (pin < 8) {
            SET_SR(3, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SR(3, B, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK4:
        if (pin < 8) {
            SET_SR(4, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_SR(4, B, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK5:
        if (pin < 8) {
            SET_SR(5, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SR(5, B, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_SR(5, C, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 32) {
            SET_SR(5, D, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK6:
        if (pin < 8) {
            SET_SR(6, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SR(6, B, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_SR(6, C, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK7:
        if (pin < 8) {
            SET_SR(7, A, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_SR(7, B, pin % SR_8_PIN_PER_REG, val, SR_2_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin schmitt trigger.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetSMT(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case GPIO_BANK0:
        if (pin < 8) {
            SET_SMT(0, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 13) {
            SET_SMT(0, B, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin >= 16 && pin < 24) {
            SET_SMT(0, C, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin >= 24 && pin < 26) {
            SET_SMT(0, D, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK1:
        if (pin < 8) {
            SET_SMT(1, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SMT(1, B, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK2:
        if (pin < 6) {
            SET_SMT(2, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK3:
        if (pin < 8) {
            SET_SMT(3, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SMT(3, B, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK4:
        if (pin < 8) {
            SET_SMT(4, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_SMT(4, B, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK5:
        if (pin < 8) {
            SET_SMT(5, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SMT(5, B, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_SMT(5, C, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 32) {
            SET_SMT(5, D, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK6:
        if (pin < 8) {
            SET_SMT(6, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 16) {
            SET_SMT(6, B, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 24) {
            SET_SMT(6, C, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    case GPIO_BANK7:
        if (pin < 8) {
            SET_SMT(7, A, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else if (pin < 10) {
            SET_SMT(7, B, pin % SMT_8_PIN_PER_REG, val, SMT_1_BIT_PER_PIN);
        } else {
            HAL_DBG("unknown gpio %d pin %d\n", bank, pin);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin parameter.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  param: multi parameters defined in @ref ePINCTRL_configParam,
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetPinParam(eGPIO_bankId bank, uint8_t pin, uint32_t param)
{
    HAL_Status rc = HAL_OK;

    if (param & FLAG_MUX) {
        rc |= PINCTRL_SetIOMUX(bank, pin, (uint8_t)((param & MASK_MUX) >> SHIFT_MUX));
    }

    if (param & FLAG_PUL) {
        rc |= PINCTRL_SetPULL(bank, pin, (uint8_t)((param & MASK_PUL) >> SHIFT_PUL));
    }

    if (param & FLAG_DRV) {
        rc |= PINCTRL_SetDRV(bank, pin, (uint8_t)((param & MASK_DRV) >> SHIFT_DRV));
    }

    if (param & FLAG_SRT) {
        rc |= PINCTRL_SetSR(bank, pin, (uint8_t)((param & MASK_SRT) >> SHIFT_SRT));
    }

    if (param & FLAG_SMT) {
        rc |= PINCTRL_SetSMT(bank, pin, (uint8_t)((param & MASK_SMT) >> SHIFT_SMT));
    }

    return rc;
}

/** @} */

/********************* Public Function Definition ****************************/

/** @defgroup PINCTRL_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 *  @{
 */

/** @} */

/** @defgroup PINCTRL_Exported_Functions_Group2 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */
HAL_Status HAL_PINCTRL_Init(void)
{
    return HAL_OK;
}

HAL_Status HAL_PINCTRL_DeInit(void)
{
    return HAL_OK;
}
/** @} */

/** @defgroup PINCTRL_Exported_Functions_Group3 IO Functions

 *  @{
 */

/**
 * @brief  Public function to set pin parameter for multi pins.
 * @param  bank: pin bank.
 * @param  mPins: multi pins defined in @ref ePINCTRL_GPIO_PINS.
 * @param  param: multi parameters defined in @ref ePINCTRL_configParam,
 * @return HAL_Status.
 */
HAL_Status HAL_PINCTRL_SetParam(eGPIO_bankId bank, uint32_t mPins, ePINCTRL_configParam param)
{
    uint8_t pin;
    uint32_t remainPins = mPins;
    HAL_Status rc;

    HAL_ASSERT(bank < GPIO_BANK_NUM);

    if (!(param & (FLAG_MUX | FLAG_PUL | FLAG_DRV | FLAG_SRT | FLAG_SMT))) {
        HAL_DBG("pinctrl: no parameter!\n");

        return HAL_ERROR;
    }

    while (remainPins) {
        pin = __builtin_ffs(remainPins) - 1;
        rc = PINCTRL_SetPinParam(bank, pin, param);
        if (rc) {
            return rc;
        }
        remainPins &= ~(1 << pin);
    }

    return HAL_OK;
}

/**
 * @brief  Public function to set pin iomux for multi pins.
 * @param  bank: pin bank.
 * @param  mPins: multi pins defined in @ref ePINCTRL_GPIO_PINS.
 * @param  param: multi parameters defined in @ref ePINCTRL_configParam,
 * @return HAL_Status.
 */
HAL_Status HAL_PINCTRL_SetIOMUX(eGPIO_bankId bank, uint32_t mPins, ePINCTRL_configParam param)
{
    return HAL_PINCTRL_SetParam(bank, mPins, param);
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_PINCTRL_MODULE_ENABLED */
