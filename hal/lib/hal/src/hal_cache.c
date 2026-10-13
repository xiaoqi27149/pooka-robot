/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup CACHE
 *  @{
 */

/** @defgroup CACHE_How_To_Use How To Use
 *  @{

 The internal cortex-m MCU of SOC often has an address mapping unit, and all accesses
 sent by MCU will be mapped, so the address seen by MCU is different from that seen by
 cache, which requires us to convert the address when maintain.
 We provide HAL_CACHE_DECODED_ADDR_BASE macro, and its logic is as follows:

 1. HAL_CACHE_DECODED_ADDR_BASE is undefined: you don't need to define this macro for
    mcu without address mapping unit.

 2. HAL_CACHE_DECODED_ADDR_BASE is defined, and the value is not equal to 0: only one
    continuous address is mapped, that is, the 0 address of mcu is mapped to the value
    of HAL_CACHE_DECODED_ADDR_BASE.

 3. HAL_CACHE_DECODED_ADDR_BASE is defined, and the value is equal to 0: which maps multiple
    addresses. You need to drive yourself to dynamically parse according to the address range.
    It is important to note that if your mcu runs in an unsafe state, you need to implement
    two additional functions(for example, you can let the previous firmware pass to mcu through ATAG):

    - HAL_CACHE_GetCodeAddrBase: get the base address of 0 address mapping.
    - HAL_CACHE_GetSRAMAddrBase: get the base address of 0x20000000 address mapping.

 @} */

#include "hal_base.h"
#include "hal_cache.h"

/** @defgroup CACHE_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

/********************** Global Variable Definition **************************/
static uintptr_t code_addr_base = 0;
static uintptr_t sram_addr_base = 0;
static uint32_t addr_base_init = 0;

/********************* Private Function Definition ***************************/

/**
 * @brief  translate the cpuAddr to cacheAddr
 * @param cpuAddr: the address seen by cpu
 * @return cacheAddr: the address seen by cache
 */
HAL_UNUSED static uintptr_t HAL_CpuAddrToCacheAddr(uintptr_t cpuAddr)
{
    if (!addr_base_init) {
        code_addr_base = HAL_CACHE_GetCodeAddrBase();
        sram_addr_base = HAL_CACHE_GetSRAMAddrBase();
        addr_base_init = 1;
    }

    if (cpuAddr < 0x20000000) {
        return cpuAddr + code_addr_base;
    } else if (cpuAddr >= 0x20000000 && cpuAddr < 0x40000000) {
        return cpuAddr - 0x20000000 + sram_addr_base;
    } else {
        return cpuAddr;
    }
}

#if defined(__CORTEX_M)
__STATIC_INLINE unsigned long HAL_SYS_EnterCriticalSection(void)
{
    unsigned long flags;

    flags = __get_PRIMASK();
    __disable_irq();

    return flags;
}

__STATIC_INLINE void HAL_SYS_ExitCriticalSection(unsigned long flags)
{
    __set_PRIMASK(flags);
}
#elif defined(__RISC_V)
__STATIC_INLINE unsigned long HAL_SYS_EnterCriticalSection(void)
{
    unsigned long flags;

    __asm volatile ("csrrci %0, mstatus, 8"
                    : "=&r" (flags)
                    :
                    : "memory");

    return flags;
}

__STATIC_INLINE void HAL_SYS_ExitCriticalSection(unsigned long flags)
{
    __asm volatile ("csrw mstatus, %0"
                    :
                    : "r" (flags)
                    : "memory");
}
#else
__STATIC_INLINE unsigned long HAL_SYS_EnterCriticalSection(void)
{
    return 0;
}

__STATIC_INLINE void HAL_SYS_ExitCriticalSection(unsigned long flags)
{
}
#endif

/***************************** MACRO Definition ******************************/
#ifdef HAL_CACHE_DECODED_ADDR_BASE
#if HAL_CACHE_DECODED_ADDR_BASE
#define DECODED_ADDR(x) (x + HAL_CACHE_DECODED_ADDR_BASE)
#else
#define DECODED_ADDR(x) HAL_CpuAddrToCacheAddr(x)
#endif
#else
#define DECODED_ADDR(x) (x)
#endif

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup CACHE_Exported_Functions_Group5 Other Functions
 *  @attention these APIs allow direct use in the HAL layer
 *  @{
 */

/**
 * @brief get the code address base(after mapped)
 *
 * @return the address mapped to 0(seen by cpu)
 */
__WEAK uintptr_t HAL_CACHE_GetCodeAddrBase(void)
{
#ifdef CODE_ADDR_MAP_START
    volatile uint32_t *ptr = (volatile uint32_t *)CODE_ADDR_MAP_START;

#if defined(SOC_RK3588)

    return ((uintptr_t)*ptr) << 16;
#else

    return (uintptr_t)*ptr;
#endif
#endif

    return 0;
}

/**
 * @brief get the sram address base(after mapped)
 *
 * @return the address mapped to 0x20000000(seen by cpu)
 */
__WEAK uintptr_t HAL_CACHE_GetSRAMAddrBase(void)
{
#ifdef SRAM_ADDR_MAP_START
    volatile uint32_t *ptr = (volatile uint32_t *)SRAM_ADDR_MAP_START;

#if defined(SOC_RK3588)

    return ((uintptr_t)*ptr) << 16;
#else

    return (uintptr_t)*ptr;
#endif
#endif

    return 0x20000000;
}

/**
 * @brief  translate the cpuAddr to sramAddr
 * @param cpuAddr: the address mapping to sram, only can be accessed by cpu
 * @return sramAddr: the real address of sram, it can be accessed by cpu & device
 */
uintptr_t HAL_CpuAddrToDmaAddr(uintptr_t cpuAddr)
{
    uintptr_t sramAddr = cpuAddr;

#ifdef SRAM_IADDR_TO_DADDR_OFFSET

    if (cpuAddr >= SRAM_IADDR_START
        && cpuAddr < (SRAM_IADDR_START + SRAM_SIZE)) {
        sramAddr = cpuAddr + SRAM_IADDR_TO_DADDR_OFFSET;
    }

#endif

#ifdef XIP_NOR_IADDR_TO_DADDR_OFFSET

    if (cpuAddr >= XIP_NOR_IADDR_START
        && cpuAddr < (XIP_NOR_IADDR_START + XIP_NOR_SIZE)) {
        sramAddr = cpuAddr + XIP_NOR_IADDR_TO_DADDR_OFFSET;
    }

#endif

#ifdef XIP_PSRAM_IADDR_TO_DADDR_OFFSET

    if (cpuAddr >= XIP_PSRAM_IADDR_START
        && cpuAddr < (XIP_PSRAM_IADDR_START + XIP_PSRAM_SIZE)) {
        sramAddr = cpuAddr + XIP_PSRAM_IADDR_TO_DADDR_OFFSET;
    }

#endif

    return sramAddr;
}

#ifdef MPU
/**
 * @brief  check mpu is enable.
 * @return HAL_ENABLE if mpu enable.
 */
__STATIC_INLINE HAL_FuncStatus HAL_MPU_IsEnable(void)
{
    HAL_FuncStatus ret = HAL_DISABLE;

    if (MPU->CTRL & MPU_CTRL_ENABLE_Msk) {
        ret = HAL_ENABLE;
    }

    return ret;
}
#endif

/**
 * @brief  enable icache.
 * @return HAL_OK if success.
 * @attention The MPU must be configured before cache if you need the function.
 */
HAL_Status HAL_ICACHE_Enable(void)
{
#if defined(HAL_ICACHE_MODULE_ENABLED)

#if defined(ICACHE)
#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t status;
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    /* config icache: mpu disable, stb disable, write through, hot buffer enable,
       prefetch enable */
    ICACHE->CACHE_CTRL |=
        (ICACHE_CACHE_CTRL_CACHE_EN_MASK | ICACHE_CACHE_CTRL_CACHE_WT_EN_MASK);
    ICACHE->CACHE_CTRL &= (~ICACHE_CACHE_CTRL_CACHE_STB_EN_MASK);

    /* if mpu has been enable, we will enable cache mpu function */
#ifdef MPU
    if (HAL_MPU_IsEnable()) {
        ICACHE->CACHE_CTRL |= ICACHE_CACHE_CTRL_CACHE_MPU_MODE_MASK;
    }
#endif

    do {
        status =
            ICACHE->CACHE_STATUS & ICACHE_CACHE_STATUS_CACHE_INIT_FINISH_MASK;
    } while (status == 0);

    ICACHE->CACHE_CTRL &= ~ICACHE_CACHE_CTRL_CACHE_BYPASS_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    SCB_EnableICache();

#endif

#endif /* HAL_ICACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  disable icache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_Disable(void)
{
#if defined(HAL_ICACHE_MODULE_ENABLED)

#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_CTRL |= ICACHE_CACHE_CTRL_CACHE_BYPASS_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    SCB_DisableICache();

#endif

#endif /* HAL_ICACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  invalidate all of the icache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_Invalidate(void)
{
#if defined(HAL_ICACHE_MODULE_ENABLED)

#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t status = 0;
    unsigned long flags;

    if ((uint32_t)DCACHE == (uint32_t)ICACHE) {
        if (DCACHE->CACHE_CTRL & DCACHE_CACHE_CTRL_CACHE_WT_EN_MASK) {
            return HAL_OK;
        } else {
            return HAL_INVAL;
        }
    }

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_MAINTAIN[0] =
        CACHE_M_INVALID_ALL | ICACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK;

    do {
        status = ICACHE->CACHE_STATUS &
                 ICACHE_CACHE_STATUS_CACHE_M_BUSY_MASK;
    } while (status);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    SCB_InvalidateICache();

#endif

#endif /* HAL_ICACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  invalidate the specified region of the icache.
 * @param  address: the start address of specified region which you want invalidate.
 * @param  sizeByte: the length in bytes of invalidate range.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_InvalidateByRange(uintptr_t address,
                                        uint32_t sizeByte)
{
#if defined(HAL_ICACHE_MODULE_ENABLED)

#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t value = 0;
    uint32_t offset = 0;
    uint32_t status = 0;
    unsigned long flags;

    if (sizeByte == 0) {
        return HAL_OK;
    }

    address = HAL_CpuAddrToDmaAddr(address);
    address = DECODED_ADDR(address);

    offset = ((address & (CACHE_LINE_SIZE - 1)) + sizeByte - 1) >> CACHE_LINE_SHIFT;
    value = (address & ICACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_MASK) |
            CACHE_M_INVALID | ICACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK;

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_MAINTAIN[1] = offset;
    ICACHE->CACHE_MAINTAIN[0] = value;

    do {
        status = ICACHE->CACHE_STATUS &
                 ICACHE_CACHE_STATUS_CACHE_M_BUSY_MASK;
    } while (status);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    SCB_InvalidateICache_by_Addr((void *)address, (int32_t)sizeByte);

#endif

#endif /* HAL_ICACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  enable performance measurement unit of icache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_EnablePMU(void)
{
#if defined(HAL_ICACHE_MODULE_ENABLED) && defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_CTRL |= ICACHE_CACHE_CTRL_CACHE_PMU_EN_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  disable performance measurement unit of icache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_DisablePMU(void)
{
#if defined(HAL_ICACHE_MODULE_ENABLED) && defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_CTRL &= (~ICACHE_CACHE_CTRL_CACHE_PMU_EN_MASK);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  get current state of performance measurement unit in icache.
 * @param  stat: return the current state if success.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_GetPMU(struct CACHE_PMU_CNT *stat)
{
#if defined(HAL_ICACHE_MODULE_ENABLED) && defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    HAL_ASSERT(stat != NULL);

    flags = HAL_SYS_EnterCriticalSection();

    stat->rdNum = ICACHE->PMU_RD_NUM_CNT;
    stat->wrNum = ICACHE->PMU_WR_NUM_CNT;
    stat->sramRdHit = ICACHE->PMU_SRAM_RD_HIT_CNT;
    stat->hbRdHit = ICACHE->PMU_HB_RD_HIT_CNT;
    stat->stbRdHit = ICACHE->PMU_STB_RD_HIT_CNT;
    stat->rdHit = ICACHE->PMU_RD_HIT_CNT;
    stat->wrHit = ICACHE->PMU_WR_HIT_CNT;
    stat->rdMissPenalty = ICACHE->PMU_RD_MISS_PENALTY_CNT;
    stat->wrMissPenalty = ICACHE->PMU_WR_MISS_PENALTY_CNT;
    stat->rdLat = ICACHE->PMU_RD_LAT_CNT;
    stat->wrLat = ICACHE->PMU_WR_LAT_CNT;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  Enable the interrupt of icache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_EnableInt(void)
{
#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_INT_EN |= ICACHE_CACHE_INT_EN_ERR_RECORD_EN_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  Disable the interrupt of icache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_DisableInt(void)
{
#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_INT_EN &= ~ICACHE_CACHE_INT_EN_ERR_RECORD_EN_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  Get the interrupt status of icache.
 * @return HAL_TRUE if ahb error occur.
 */
HAL_Check HAL_ICACHE_GetInt(void)
{
    uint32_t status = 0;

#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)

    status = ICACHE->CACHE_INT_ST & ICACHE_CACHE_INT_ST_AHB_ERROR_STATUS_MASK;

#endif

#endif

    return status ? HAL_TRUE : HAL_FALSE;
}

/**
 * @brief  Get the ahb bus error address of icache.
 * @return ahb buss error address if success.
 * @attention The return value is only valid if you get a ahb error from CACHE_INT_ST
 */
uintptr_t HAL_ICACHE_GetErrAddr(void)
{
    uintptr_t address = -1;

#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)

    address = ICACHE->CACHE_ERR_HADDR;

#endif

#endif

    return address;
}

/**
 * @brief  Clear the interrupt status of icache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_ICACHE_ClearInt(void)
{
#if defined(ICACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    ICACHE->CACHE_INT_ST |= ICACHE_CACHE_INT_ST_AHB_ERROR_STATUS_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief     enable dcache.
 * @return    HAL_OK if success.
 * @attention The MPU must be configured before cache if you need the feature.
 * @attention Dynamically disable and enable dcache is not allowed.
 */
HAL_Status HAL_DCACHE_Enable(void)
{
#if defined(HAL_DCACHE_MODULE_ENABLED)

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t status;
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    /* stb enable, stb_entry=7, stb_timeout enable, write back, prefetch enable */
    DCACHE->CACHE_CTRL |=
        DCACHE_CACHE_CTRL_CACHE_EN_MASK
        | (7U << DCACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_SHIFT)
        | DCACHE_CACHE_CTRL_STB_TIMEOUT_EN_MASK;
    DCACHE->STB_TIMEOUT_CTRL = 1;

    /* if mpu has been enable, we will enable cache mpu function */
#ifdef MPU
    if (HAL_MPU_IsEnable()) {
        DCACHE->CACHE_CTRL |= DCACHE_CACHE_CTRL_CACHE_MPU_MODE_MASK;
    }
#endif

    do {
        status =
            DCACHE->CACHE_STATUS & DCACHE_CACHE_STATUS_CACHE_INIT_FINISH_MASK;
    } while (status == 0);

    DCACHE->CACHE_CTRL &= ~DCACHE_CACHE_CTRL_CACHE_BYPASS_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_EnableDCache();

#endif

#endif /* HAL_DCACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  disable dcache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_Disable(void)
{
#if defined(HAL_DCACHE_MODULE_ENABLED)

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_CTRL |= DCACHE_CACHE_CTRL_CACHE_BYPASS_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_DisableDCache();

#endif

#endif /* HAL_DCACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  invalidate all of the dcache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_Invalidate(void)
{
#if defined(HAL_DCACHE_MODULE_ENABLED)

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t status = 0;
    unsigned long flags;

    if ((uint32_t)DCACHE == (uint32_t)ICACHE) {
        if (DCACHE->CACHE_CTRL & DCACHE_CACHE_CTRL_CACHE_WT_EN_MASK) {
            return HAL_OK;
        } else {
            return HAL_INVAL;
        }
    }

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_MAINTAIN[0] =
        CACHE_M_INVALID_ALL | DCACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK;

    do {
        status = DCACHE->CACHE_STATUS &
                 DCACHE_CACHE_STATUS_CACHE_M_BUSY_MASK;
    } while (status);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_InvalidateDCache();

#elif defined(__CORTEX_A)
    L1C_InvalidateDCacheAll();

#endif

#endif /* HAL_DCACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  invalidate the specified region of the dcache.
 * @param  address: the start address of specified region which you want invalidate.
 * @param  sizeByte: the length in bytes of invalidate range.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_InvalidateByRange(uintptr_t address,
                                        uint32_t sizeByte)
{
#if defined(HAL_DCACHE_MODULE_ENABLED)

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t value = 0;
    uint32_t offset = 0;
    uint32_t status = 0;
    unsigned long flags;

    if (sizeByte == 0) {
        return HAL_OK;
    }

    address = DECODED_ADDR(address);

    offset = ((address & (CACHE_LINE_SIZE - 1)) + sizeByte - 1) >> CACHE_LINE_SHIFT;
    value = (address & DCACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_MASK) |
            CACHE_M_INVALID | DCACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_MAINTAIN[1] = offset;
    DCACHE->CACHE_MAINTAIN[0] = value;

    do {
        status = DCACHE->CACHE_STATUS &
                 DCACHE_CACHE_STATUS_CACHE_M_BUSY_MASK;
    } while (status);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_InvalidateDCache_by_Addr((void *)address, (int32_t)sizeByte);

#elif defined(__CORTEX_A)
    uintptr_t start, stop, addr;

    if (sizeByte == 0) {
        return HAL_OK;
    }

    start = address;
    stop = start + sizeByte;

    if (start & (CACHE_LINE_SIZE - 1)) {
        addr = start & ~(CACHE_LINE_SIZE - 1);
#ifdef __aarch64__
        asm volatile ("DC CIVAC, %0" : : "r" (addr) : "memory");
#else
        __set_DCCIMVAC((uint32_t)addr);
#endif
        start = addr + CACHE_LINE_SIZE;
    }

    if (stop & (CACHE_LINE_SIZE - 1)) {
        addr = stop & ~(CACHE_LINE_SIZE - 1);
#ifdef __aarch64__
        asm volatile ("DC CIVAC, %0" : : "r" (addr) : "memory");
#else
        __set_DCCIMVAC((uint32_t)addr);
#endif
        stop = addr;
    }

    for (addr = start; addr < stop; addr += CACHE_LINE_SIZE) {
#ifdef __aarch64__
        asm volatile ("DC IVAC, %0" : : "r" (addr) : "memory");
#else
        __set_DCIMVAC((uint32_t)addr);
#endif
    }

    __DMB();     //ensure the ordering of data cache maintenance operations and their effects

#endif

#endif /* HAL_DCACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  clean the specified region of the dcache, it will flush the dirty cache in
 *         the specified region.
 * @param  address: the start address of specified region which you want invalidate.
 * @param  sizeByte: the length in bytes of invalidate range.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_CleanByRange(uintptr_t address,
                                   uint32_t sizeByte)
{
#if defined(HAL_DCACHE_MODULE_ENABLED)

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t value = 0;
    uint32_t offset = 0;
    uint32_t status = 0;
    unsigned long flags;

    if (sizeByte == 0) {
        return HAL_OK;
    }

    address = DECODED_ADDR(address);

    offset = ((address & (CACHE_LINE_SIZE - 1)) + sizeByte - 1) >> CACHE_LINE_SHIFT;
    value = (address & DCACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_MASK) |
            CACHE_M_CLEAN | DCACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_MAINTAIN[1] = offset;
    DCACHE->CACHE_MAINTAIN[0] = value;

    do {
        status = DCACHE->CACHE_STATUS &
                 DCACHE_CACHE_STATUS_CACHE_M_BUSY_MASK;
    } while (status);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanDCache_by_Addr((void *)address, (int32_t)sizeByte);

#elif defined(__CORTEX_A)
    uintptr_t start, stop, addr;

    if (sizeByte == 0) {
        return HAL_OK;
    }

    start = address & ~(CACHE_LINE_SIZE - 1);
    stop = (address + sizeByte) & ~(CACHE_LINE_SIZE - 1);

    for (addr = start; addr <= stop; addr += CACHE_LINE_SIZE) {
#ifdef __aarch64__
        asm volatile ("DC CVAC, %0" : : "r" (addr) : "memory");
#else
        __set_DCCMVAC((uint32_t)addr);
#endif
    }

    __DMB();     //ensure the ordering of data cache maintenance operations and their effects

#endif

#endif /* HAL_DCACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  clean and invalidate the specified region of the dcache, it will flush the dirty
 *         cache in the specified region, and set the cache line to invalid state.
 * @param  address: the start address of specified region which you want invalidate.
 * @param  sizeByte: the length in bytes of invalidate range.
 * @return HAL_OK if success.
 */
HAL_Status
HAL_DCACHE_CleanInvalidateByRange(uintptr_t address, uint32_t sizeByte)
{
#if defined(HAL_DCACHE_MODULE_ENABLED)

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t value = 0;
    uint32_t offset = 0;
    uint32_t status = 0;
    unsigned long flags;

    if (sizeByte == 0) {
        return HAL_OK;
    }

    address = DECODED_ADDR(address);

    offset = ((address & (CACHE_LINE_SIZE - 1)) + sizeByte - 1) >> CACHE_LINE_SHIFT;
    value = (address & DCACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_MASK) |
            CACHE_M_CLEAN_INVALID | DCACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_MAINTAIN[1] = offset;
    DCACHE->CACHE_MAINTAIN[0] = value;

    do {
        status = DCACHE->CACHE_STATUS &
                 DCACHE_CACHE_STATUS_CACHE_M_BUSY_MASK;
    } while (status);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache_by_Addr((void *)address, (int32_t)sizeByte);

#elif defined(__CORTEX_A)
    uintptr_t start, stop, addr;

    if (sizeByte == 0) {
        return HAL_OK;
    }

    start = address & ~(CACHE_LINE_SIZE - 1);
    stop = (address + sizeByte) & ~(CACHE_LINE_SIZE - 1);

    for (addr = start; addr <= stop; addr += CACHE_LINE_SIZE) {
#ifdef __aarch64__
        asm volatile ("DC CIVAC, %0" : : "r" (addr) : "memory");
#else
        __set_DCCIMVAC((uint32_t)addr);
#endif
    }

    __DMB();     //ensure the ordering of data cache maintenance operations and their effects

#endif

#endif /* HAL_DCACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  clean and invalidate all of the dcache, the dirty cache line will be flush
 *         to external memory, and all of the cache line will be set invalid.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_CleanInvalidate(void)
{
#if defined(HAL_DCACHE_MODULE_ENABLED)

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    uint32_t status = 0;
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_CTRL |= DCACHE_CACHE_CTRL_CACHE_FLUSH_MASK;

    do {
        status =
            DCACHE->CACHE_STATUS & DCACHE_CACHE_STATUS_CACHE_FLUSH_DONE_MASK;
    } while (!status);

    DCACHE->CACHE_CTRL &= ~DCACHE_CACHE_CTRL_CACHE_FLUSH_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#elif defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache();

#elif defined(__CORTEX_A)
    L1C_CleanInvalidateDCacheAll();

#endif

#endif /* HAL_DCACHE_MODULE_ENABLED */

    return HAL_OK;
}

/**
 * @brief  enable performance measurement unit of dcache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_EnablePMU(void)
{
#if defined(HAL_DCACHE_MODULE_ENABLED) && defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_CTRL |= DCACHE_CACHE_CTRL_CACHE_PMU_EN_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  disable performance measurement unit of dcache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_DisablePMU(void)
{
#if defined(HAL_DCACHE_MODULE_ENABLED) && defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_CTRL &= (~DCACHE_CACHE_CTRL_CACHE_PMU_EN_MASK);

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  get current state of performance measurement unit in icache.
 * @param  stat: return the current state if success.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_GetPMU(struct CACHE_PMU_CNT *stat)
{
#if defined(HAL_DCACHE_MODULE_ENABLED) && defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    HAL_ASSERT(stat != NULL);

    flags = HAL_SYS_EnterCriticalSection();

    stat->rdNum = DCACHE->PMU_RD_NUM_CNT;
    stat->wrNum = DCACHE->PMU_WR_NUM_CNT;
    stat->sramRdHit = DCACHE->PMU_SRAM_RD_HIT_CNT;
    stat->hbRdHit = DCACHE->PMU_HB_RD_HIT_CNT;
    stat->stbRdHit = DCACHE->PMU_STB_RD_HIT_CNT;
    stat->rdHit = DCACHE->PMU_RD_HIT_CNT;
    stat->wrHit = DCACHE->PMU_WR_HIT_CNT;
    stat->rdMissPenalty = DCACHE->PMU_RD_MISS_PENALTY_CNT;
    stat->wrMissPenalty = DCACHE->PMU_WR_MISS_PENALTY_CNT;
    stat->rdLat = DCACHE->PMU_RD_LAT_CNT;
    stat->wrLat = DCACHE->PMU_WR_LAT_CNT;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  Enable the interrupt of dcache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_EnableInt(void)
{
#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_INT_EN |= DCACHE_CACHE_INT_EN_ERR_RECORD_EN_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  Disable the interrupt of dcache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_DisableInt(void)
{
#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_INT_EN &= ~DCACHE_CACHE_INT_EN_ERR_RECORD_EN_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  Get the interrupt status of dcache.
 * @return HAL_TRUE if ahb error occur.
 */
HAL_Check HAL_DCACHE_GetInt(void)
{
    uint32_t status = 0;

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)

    status = DCACHE->CACHE_INT_ST & DCACHE_CACHE_INT_ST_AHB_ERROR_STATUS_MASK;

#endif

#endif

    return status ? HAL_TRUE : HAL_FALSE;
}

/**
 * @brief  Clear the interrupt status of dcache.
 * @return HAL_OK if success.
 */
HAL_Status HAL_DCACHE_ClearInt(void)
{
#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)
    unsigned long flags;

    flags = HAL_SYS_EnterCriticalSection();

    DCACHE->CACHE_INT_ST |= DCACHE_CACHE_INT_ST_AHB_ERROR_STATUS_MASK;

    HAL_SYS_ExitCriticalSection(flags);
#endif

#endif

    return HAL_OK;
}

/**
 * @brief  Get the ahb bus error address of dcache.
 * @return ahb buss error address if success.
 * @attention The return value is only valid if you get a ahb error from CACHE_INT_ST
 */
uintptr_t HAL_DCACHE_GetErrAddr(void)
{
    uintptr_t address = -1;

#if defined(DCACHE)

#if defined(CACHE_REVISION) && (CACHE_REVISION == 0x00000100U)

    address = DCACHE->CACHE_ERR_HADDR;

#endif

#endif

    return address;
}

/** @} */

/** @} */

/** @} */
