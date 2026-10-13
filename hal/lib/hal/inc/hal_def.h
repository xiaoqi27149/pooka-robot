/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup HAL_DEF
 *  @{
 */

#ifndef _HAL_DEF_H_
#define _HAL_DEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"
#include "hal_list.h"

/***************************** MACRO Definition ******************************/
/** @defgroup HAL_DEF_Exported_Definition_Group1 Basic Definition
 *  @{
 */

#define SET_BIT(REG, BIT)    ((*(volatile uint32_t *)&(REG)) |= (BIT))        /**< Set 1 to the register specific bit field */
#define CLEAR_BIT(REG, MASK) ((*(volatile uint32_t *)&(REG)) &= ~(MASK))      /**< Clear the specific bits filed from the register */
#define READ_BIT(REG, MASK)  ((*(volatile const uint32_t *)&(REG)) & (MASK))  /**< Read the value of a specific bits field from the register */
#define CLEAR_REG(REG)       ((*(volatile uint32_t *)&(REG)) = (0x0))         /**< Write 0 to the register */
#define WRITE_REG(REG, VAL)  ((*(volatile uint32_t *)&(REG)) = (VAL))         /**< Write the register */
#define READ_REG(REG)        ((*(volatile const uint32_t *)&(REG)))           /**< Read the register */
#define MODIFY_REG(REG, CLEARMASK, SETMASK) \
        WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))    /**< Clear and set the value of a specific bits field from the register */
#define POSITION_VAL(VAL) (__CLZ(__RBIT(VAL)))

#if defined(__GNUC__) || defined(__CC_ARM)
#define MASK_TO_WE(msk) (__builtin_constant_p(msk) ? ((msk) > 0xFFFFU ? 0 : ((msk) << 16)) : ((msk) << 16))
#else
#define MASK_TO_WE(msk) ((msk) << 16)
#endif
#define VAL_MASK_WE(msk, val)            ((MASK_TO_WE(msk)) | (val))
#define WRITE_REG_MASK_WE(reg, msk, val) WRITE_REG(reg, (VAL_MASK_WE(msk, val)))

/* Misc OPS Marco */
#define HAL_MAX_DELAY 0xFFFFFFFFU

#define RESET                     0
#define HAL_IS_BIT_SET(REG, MASK) (((*(volatile uint32_t *)&(REG)) & (MASK)) != RESET)  /**< Check if the the specific bits filed from the register is valid */
#define HAL_IS_BIT_CLR(REG, MASK) (((*(volatile uint32_t *)&(REG)) & (MASK)) == RESET)  /**< Check if the the specific bits filed from the register is isvalid */

#define HAL_BIT(nr)       (1UL << (nr))
#define HAL_BIT64(nr)     (1ULL << (nr))
#define HAL_ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#define HAL_MAX(x, y)     ((x) > (y) ? (x) : (y))
#define HAL_MIN(x, y)     ((x) < (y) ? (x) : (y))

#define HAL_DIV_ROUND_UP(x, y) (((x) + (y) - 1) / (y))

#define HAL_DIV_ROUND_CLOSEST(x, divisor)(         \
{                                                  \
    __typeof__(x) __x = x;                         \
    __typeof__(divisor) __d = divisor;             \
    (((__typeof__(x))-1) > 0 ||                    \
     ((__typeof__(divisor))-1) > 0 || (__x) > 0) ? \
        (((__x) + ((__d) / 2)) / (__d)) :          \
        (((__x) - ((__d) / 2)) / (__d));           \
}                                                  \
)

/** round up to the next specified multiple */
#define HAL_ROUND_UP(x, y) (         \
{                                    \
    __typeof__(y) __y = y;           \
    (((x) + (__y - 1)) / __y) * __y; \
}                                    \
)

/** round down to the next specified multiple */
#define HAL_ROUND_DOWN(x, y) ( \
{                              \
    __typeof__(x) __x = (x);   \
    __x - (__x % (y));         \
}                              \
)

#define HAL_ReadPollTimeoutAtomic(op, val, cond, delayUs, timeoutUs,      \
                                  delayBeforeRead, args...)               \
({                                                                        \
    uint64_t __timeoutUs = (timeoutUs);                                   \
    uint64_t __delayUs = (delayUs);                                       \
    uint64_t __startUs = (HAL_GetTickUs());                               \
    if (delayBeforeRead && __delayUs) {                                   \
        HAL_DelayUs(__delayUs);                                           \
    }                                                                     \
    for (;;) {                                                            \
        (val) = op(args);                                                 \
        if (cond) {                                                       \
            break;                                                        \
        }                                                                 \
        if (__timeoutUs && (HAL_GetTickUs() - __startUs) > __timeoutUs) { \
            (val) = op(args);                                             \
            break;                                                        \
        }                                                                 \
        if (__delayUs) {                                                  \
            HAL_DelayUs(__delayUs);                                       \
        }                                                                 \
    }                                                                     \
    (cond) ? 0 : HAL_TIMEOUT;                                             \
})

#define HAL_ReadxPollTimeoutAtomic(op, addr, val, cond, delayUs, timeoutUs) \
    HAL_ReadPollTimeoutAtomic(op, val, cond, delayUs, timeoutUs, false, addr)

#define HAL_ReadRegPollTimeoutAtomic(addr, val, cond, delayUs, timeoutUs) \
    HAL_ReadPollTimeoutAtomic(READ_REG, val, cond, delayUs, timeoutUs, false, addr)

#define HAL_IS_ALIGNED(x, a) (((x) & (a - 1)) == 0)
#ifdef CACHE_LINE_SIZE
#define HAL_IS_CACHELINE_ALIGNED(x) HAL_IS_ALIGNED((uintptr_t)(x), CACHE_LINE_SIZE)
#else
#define HAL_IS_CACHELINE_ALIGNED(x) HAL_IS_ALIGNED((uintptr_t)(x), 4)
#endif

#define HAL_ALIGN(x, val)      HAL_ALIGN_MASK(x, (__typeof__(x))(val) - 1)
#define HAL_ALIGN_MASK(x, msk) (((x) + (msk)) & ~(msk))

/**
 * @brief  Returns 32-63 bits of a number.
 * @param  n : Number being accessed.
 * @return Bits 32-63 of number.
 *
 * @note    A basic shift-right of a 64- or 32-bit quantity.
 *          Use this to suppress the "right shift count >= width of type"
 *          warning when that quantity is 32-bits.
 */
#define HAL_UPPER_32_BITS(n) ((uint32_t)(((n) >> 16) >> 16))

/**
 * @brief  Returns 0-31 bits of a number
 * @param  n : Number being accessed.
 * @return Bits 0-31 of number
 */
#define HAL_LOWER_32_BITS(n) ((uint32_t)((n) & 0xffffffff))

/**
 * @brief  Returns 16-31 bits of a number
 * @param  n : Number being accessed.
 * @return Bits 16-31 of number
 */
#define HAL_UPPER_16_BITS(n) ((uint16_t)((n) >> 16))

/**
 * @brief  Returns 0-15 bits of a number
 * @param  n : Number being accessed.
 * @return Bits 0-15 of number
 */
#define HAL_LOWER_16_BITS(n) ((uint16_t)((n) & 0xffff))

/* Compiller Macro */
#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM) || defined(__ICCARM__)
#define HAL_UNUSED __attribute__((__unused__))
#else
#define HAL_UNUSED
#endif

#if __has_attribute(__externally_visible__)
#define HAL_VISIBLE __attribute__((__externally_visible__))
#else
#define HAL_VISIBLE
#endif

#ifdef CACHE_LINE_SIZE
#define HAL_CACHELINE_ALIGNED __ALIGNED(CACHE_LINE_SIZE)
#else
#define HAL_CACHELINE_ALIGNED
#endif

#ifdef HAL_SRAM_SECTION_ENABLED
#define HAL_SECTION_SRAM_CODE   __attribute__((section(".sram_code")))
#define HAL_SECTION_SRAM_RODATA __attribute__((section(".sram_rodata")))
#define HAL_SECTION_SRAM_DATA   __attribute__((section(".sram_data")))
#define HAL_SECTION_SRAM_BSS    __attribute__((section(".sram_bss")))
#else
#define HAL_SECTION_SRAM_CODE
#define HAL_SECTION_SRAM_RODATA
#define HAL_SECTION_SRAM_DATA
#define HAL_SECTION_SRAM_BSS
#endif

#ifdef HAL_PSRAM_SECTION_ENABLED
#define HAL_SECTION_PSRAM_CODE   __attribute__((section(".psram_code")))
#define HAL_SECTION_PSRAM_RODATA __attribute__((section(".psram_rodata")))
#define HAL_SECTION_PSRAM_DATA   __attribute__((section(".psram_data")))
#define HAL_SECTION_PSRAM_BSS    __attribute__((section(".psram_bss")))
#else
#define HAL_SECTION_PSRAM_CODE
#define HAL_SECTION_PSRAM_RODATA
#define HAL_SECTION_PSRAM_DATA
#define HAL_SECTION_PSRAM_BSS
#endif

#ifdef HAL_XIP_SECTION_ENABLED
#define HAL_SECTION_XIP_CODE   __attribute__((section(".xip_code")))
#define HAL_SECTION_XIP_RODATA __attribute__((section(".xip_rodata")))
#else
#define HAL_SECTION_XIP_CODE
#define HAL_SECTION_XIP_RODATA
#endif

#define HAL_GENMASK(h, l)       (((~0U) << (l)) & (~0U >> (32 - 1 - (h))))
#define HAL_GENVAL(x, h, l)     ((uint32_t)(((x) & HAL_GENMASK(h, l)) >> (l)))
#define HAL_GENMASK_ULL(h, l)   (((~0ULL) << (l)) & (~0ULL >> (64 - 1 - (h))))
#define HAL_GENVAL_ULL(x, h, l) ((uint64_t)(((x) & HAL_GENMASK_ULL(h, l)) >> (l)))

/** MCU systick clock source */
typedef enum {
    HAL_SYSTICK_CLKSRC_CORE,
    HAL_SYSTICK_CLKSRC_EXT
} eHAL_systickClkSource;

/** Check if is MCU systick clock source */
#define IS_SYSTICK_SOURCE(s) (((s) == HAL_SYSTICK_CLKSRC_CORE) || ((s) == HAL_SYSTICK_CLKSRC_EXT))

/***************************** Structure Definition **************************/
/** HAL boolean type definition */
typedef enum {
    HAL_FALSE = 0x00U,
    HAL_TRUE  = 0x01U
} HAL_Check;

/** HAL error code definition */
typedef enum {
    HAL_OK      = 0x00U,
    HAL_ERROR   = (-1),
    HAL_BUSY    = (-16),
    HAL_NODEV   = (-19),
    HAL_INVAL   = (-22),
    HAL_NOSYS   = (-38),
    HAL_TIMEOUT = (-110)
} HAL_Status;

/** HAL functional status definition */
typedef enum {
    HAL_DISABLE = 0x00U,
    HAL_ENABLE  = 0x01U
} HAL_FuncStatus;

/** HAL lock structures definition */
typedef enum {
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U
} HAL_LockStatus;

/** RK GPIO bank definition */
/** The BANK_PIN macro in hal_gpio.h relies on sequential values of
 *  GPIO_BANK0~7 to calculate interrupt pin numbers correctly.
 *  So must placing all base banks (GPIO_BANK0~7) first in the enum
 */
typedef enum {
#ifdef GPIO0
    GPIO_BANK0,
#endif
#ifdef GPIO1
    GPIO_BANK1,
#endif
#ifdef GPIO2
    GPIO_BANK2,
#endif
#ifdef GPIO3
    GPIO_BANK3,
#endif
#ifdef GPIO4
    GPIO_BANK4,
#endif
#ifdef GPIO5
    GPIO_BANK5,
#endif
#ifdef GPIO6
    GPIO_BANK6,
#endif
#ifdef GPIO7
    GPIO_BANK7,
#endif
#if defined(GPIO0_EXP) || defined(GPIO0_EXP1)
    GPIO_BANK0_EXP1,
#endif
#if defined(GPIO1_EXP) || defined(GPIO1_EXP1)
    GPIO_BANK1_EXP1,
#endif
#if defined(GPIO2_EXP) || defined(GPIO2_EXP1)
    GPIO_BANK2_EXP1,
#endif
#if defined(GPIO3_EXP) || defined(GPIO3_EXP1)
    GPIO_BANK3_EXP1,
#endif
#if defined(GPIO4_EXP) || defined(GPIO4_EXP1)
    GPIO_BANK4_EXP1,
#endif
#if defined(GPIO5_EXP) || defined(GPIO5_EXP1)
    GPIO_BANK5_EXP1,
#endif
#if defined(GPIO6_EXP) || defined(GPIO6_EXP1)
    GPIO_BANK6_EXP1,
#endif
#if defined(GPIO7_EXP) || defined(GPIO7_EXP1)
    GPIO_BANK7_EXP1,
#endif
#ifdef GPIO0_EXP2
    GPIO_BANK0_EXP2,
#endif
#ifdef GPIO1_EXP2
    GPIO_BANK1_EXP2,
#endif
#ifdef GPIO2_EXP2
    GPIO_BANK2_EXP2,
#endif
#ifdef GPIO3_EXP2
    GPIO_BANK3_EXP2,
#endif
#ifdef GPIO4_EXP2
    GPIO_BANK4_EXP2,
#endif
#ifdef GPIO5_EXP2
    GPIO_BANK5_EXP2,
#endif
#ifdef GPIO6_EXP2
    GPIO_BANK6_EXP2,
#endif
#ifdef GPIO7_EXP2
    GPIO_BANK7_EXP2,
#endif
#ifdef GPIO0_EXP3
    GPIO_BANK0_EXP3,
#endif
#ifdef GPIO1_EXP3
    GPIO_BANK1_EXP3,
#endif
#ifdef GPIO2_EXP3
    GPIO_BANK2_EXP3,
#endif
#ifdef GPIO3_EXP3
    GPIO_BANK3_EXP3,
#endif
#ifdef GPIO4_EXP3
    GPIO_BANK4_EXP3,
#endif
#ifdef GPIO5_EXP3
    GPIO_BANK5_EXP3,
#endif
#ifdef GPIO6_EXP3
    GPIO_BANK6_EXP3,
#endif
#ifdef GPIO7_EXP3
    GPIO_BANK7_EXP3,
#endif
    GPIO_BANK_NUM
} eGPIO_bankId;

/** HAL function type definition */
typedef void (*pFunc)(void);

/** @} */
/***************************** Function Declare ******************************/

#endif

/** @} */

/** @} */
