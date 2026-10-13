/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(SOC_RV1126B) && defined(HAL_CRU_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup CRU
 *  @{
 */

/** @defgroup CRU_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define OSC_32KHZ 32768

/********************* Private Structure Definition **************************/

static struct PLL_CONFIG PLL_TABLE[] = {
    /* _mhz, _refDiv, _fbDiv, _postdDv1, _postDiv2, _dsmpd, _frac */
    RK_PLL_RATE(1200000000, 1, 100, 2, 1, 1, 0),
    RK_PLL_RATE(1188000000, 1, 99, 2, 1, 1, 0),
    RK_PLL_RATE(1000000000, 3, 125, 1, 1, 1, 0),
    RK_PLL_RATE(993484800, 1, 41, 1, 1, 0, 6630355),
    RK_PLL_RATE(983040000, 1, 40, 1, 1, 0, 16106127),
    RK_PLL_RATE(903168000, 1, 75, 2, 1, 0, 4429185),
    RK_PLL_RATE(786432000, 1, 131, 4, 1, 0, 1207959),
    RK_PLL_RATE(722534400, 4, 361, 3, 1, 0, 4482872),
    { /* sentinel */ },
};

static uint32_t s_gpllFreq;
static uint32_t s_cpllFreq;
static uint32_t s_aupllFreq;
static uint32_t s_clkRcOsc = 125000000;
static uint32_t s_fracUart0Freq;
static uint32_t s_fracUart1Freq;
static uint32_t s_fracVoice0Freq;
static uint32_t s_fracVoice1Freq;
static uint32_t s_fracCom0Freq;
static uint32_t s_fracCom1Freq;
static uint32_t s_fracCom2Freq;
static uint32_t s_intVoice0Freq;
static uint32_t s_intVoice1Freq;
static uint32_t s_sai0MclkIoIn;
static uint32_t s_sai1MclkIoIn;
static uint32_t s_sai2MclkIoIn;

static struct PLL_SETUP GPLL = {
    .conOffset0 = &(TOPCRU->GPLL_CON[0]),
    .conOffset1 = &(TOPCRU->GPLL_CON[1]),
    .conOffset2 = &(TOPCRU->GPLL_CON[2]),
    .conOffset3 = &(TOPCRU->GPLL_CON[3]),
    .modeOffset = &(TOPCRU->MODE_CON[0]),
    .modeShift = 2,
    .lockShift = 10,
    .modeMask = 0x3 << 2,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP CPLL = {
    .conOffset0 = &(PERICRU->CPLL_CON[0]),
    .conOffset1 = &(PERICRU->CPLL_CON[1]),
    .conOffset2 = &(PERICRU->CPLL_CON[2]),
    .conOffset3 = &(PERICRU->CPLL_CON[3]),
    .modeOffset = &(TOPCRU->MODE_CON[0]),
    .modeShift = 4,
    .lockShift = 10,
    .modeMask = 0x3 << 4,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP AUPLL = {
    .conOffset0 = &(TOPCRU->AUPLL_CON[0]),
    .conOffset1 = &(TOPCRU->AUPLL_CON[1]),
    .conOffset2 = &(TOPCRU->AUPLL_CON[2]),
    .conOffset3 = &(TOPCRU->AUPLL_CON[3]),
    .modeOffset = &(TOPCRU->MODE_CON[0]),
    .modeShift = 0,
    .lockShift = 10,
    .modeMask = 0x3 << 0,
    .rateTable = PLL_TABLE,
};

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup CRU_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief Get frac clk freq.
 * @param  clockName: CLOCK_Name id
 * @return clk rate.
 * How to calculate the Frac clk divider:
 *     numerator is frac register[31:16]
 *     denominator is frac register[15:0]
 *     clk rate = pRate * numerator / denominator
 *     for a better jitter, pRate > 20 * rate
 */
static uint32_t HAL_CRU_ClkFracGetFreq(eCLOCK_Name clockName)
{
    uint32_t freq = 0;
    uint32_t muxSrc = 0, divFrac = 0;
    uint32_t n, m, pRate, n_h, m_h, n_l, m_l;
    volatile uint32_t *fracOffset;

    switch (clockName) {
    case CLK_CM_FRAC0:
        muxSrc = CLK_GET_MUX(CLK_CM_FRAC0);
        divFrac = CLK_GET_DIV(CLK_CM_FRAC0);
        fracOffset = &(TOPCRU->CLK_CM_FRAC_MATRIX0_DIV_HIGH);
        break;
    case CLK_CM_FRAC1:
        muxSrc = CLK_GET_MUX(CLK_CM_FRAC1);
        divFrac = CLK_GET_DIV(CLK_CM_FRAC1);
        fracOffset = &(TOPCRU->CLK_CM_FRAC_MATRIX1_DIV_HIGH);
        break;
    case CLK_CM_FRAC2:
        muxSrc = CLK_GET_MUX(CLK_CM_FRAC2);
        divFrac = CLK_GET_DIV(CLK_CM_FRAC2);
        fracOffset = &(TOPCRU->CLK_CM_FRAC_MATRIX2_DIV_HIGH);
        break;
    case CLK_UART_FRAC0:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC0);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC0);
        fracOffset = &(TOPCRU->CLK_UART_FRAC_MATRIX0_DIV_HIGH);
        break;
    case CLK_UART_FRAC1:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC1);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC1);
        fracOffset = &(TOPCRU->CLK_UART_FRAC_MATRIX1_DIV_HIGH);
        break;
    case CLK_AUDIO_FRAC0:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC0);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC0);
        fracOffset = &(TOPCRU->CLK_AUDIO_FRAC_MATRIX0_DIV_HIGH);
        break;
    case CLK_AUDIO_FRAC1:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC1);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC1);
        fracOffset = &(TOPCRU->CLK_AUDIO_FRAC_MATRIX1_DIV_HIGH);
        break;

    default:

        return HAL_INVAL;
    }

    switch (clockName) {
    case CLK_CM_FRAC0:
    case CLK_CM_FRAC1:
    case CLK_CM_FRAC2:
        pRate = HAL_CRU_MuxGetFreq4(muxSrc, PLL_INPUT_OSC_RATE, s_gpllFreq, s_aupllFreq, s_cpllFreq);
        HAL_CRU_ClkGetFracDiv(divFrac, &n_l, &m_l);
        n_h = (READ_REG(*fracOffset) & 0xff00) >> 8;
        m_h = (READ_REG(*fracOffset) & 0xff);
        n = (n_h << 16) | n_l;
        m = (m_h << 16) | m_l;
        freq = pRate / m * n;

        return freq;

    case CLK_UART_FRAC0:
    case CLK_UART_FRAC1:
        pRate = HAL_CRU_MuxGetFreq3(muxSrc, PLL_INPUT_OSC_RATE, s_gpllFreq, s_cpllFreq);
        HAL_CRU_ClkGetFracDiv(divFrac, &n_l, &m_l);
        n_h = (READ_REG(*fracOffset) & 0xff00) >> 8;
        m_h = (READ_REG(*fracOffset) & 0xff);
        n = (n_h << 16) | n_l;
        m = (m_h << 16) | m_l;
        freq = pRate / m * n;

        return freq;
    case CLK_AUDIO_FRAC0:
    case CLK_AUDIO_FRAC1:
        pRate = HAL_CRU_MuxGetFreq3(muxSrc, PLL_INPUT_OSC_RATE, s_gpllFreq, s_aupllFreq);
        HAL_CRU_ClkGetFracDiv(divFrac, &n_l, &m_l);
        n_h = (READ_REG(*fracOffset) & 0xff00) >> 8;
        m_h = (READ_REG(*fracOffset) & 0xff);
        n = (n_h << 16) | n_l;
        m = (m_h << 16) | m_l;
        freq = pRate / m * n;

        return freq;

    default:

        return HAL_INVAL;
    }
}

/**
 * @brief Set frac clk freq.
 * @param  clockName: CLOCK_Name id.
 * @param  rate: clk set rate
 * @return HAL_Status.
 * How to calculate the Frac clk divider:
 *     if pRate > 20 * rate, select frac divider
 *     else select normal divider, but the clk rate may be not accurate
 */
static HAL_Status HAL_CRU_ClkFracSetFreq(eCLOCK_Name clockName, uint32_t rate)
{
    uint32_t muxSrc = 0, divFrac = 0, mux = 0;
    uint32_t n, m, pRate;
    volatile uint32_t *fracOffset;

    switch (clockName) {
    case CLK_CM_FRAC0:
        muxSrc = CLK_GET_MUX(CLK_CM_FRAC0);
        divFrac = CLK_GET_DIV(CLK_CM_FRAC0);
        fracOffset = &(TOPCRU->CLK_CM_FRAC_MATRIX0_DIV_HIGH);
        break;
    case CLK_CM_FRAC1:
        muxSrc = CLK_GET_MUX(CLK_CM_FRAC1);
        divFrac = CLK_GET_DIV(CLK_CM_FRAC1);
        fracOffset = &(TOPCRU->CLK_CM_FRAC_MATRIX1_DIV_HIGH);
        break;
    case CLK_CM_FRAC2:
        muxSrc = CLK_GET_MUX(CLK_CM_FRAC2);
        divFrac = CLK_GET_DIV(CLK_CM_FRAC2);
        fracOffset = &(TOPCRU->CLK_CM_FRAC_MATRIX2_DIV_HIGH);
        break;
    case CLK_UART_FRAC0:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC0);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC0);
        fracOffset = &(TOPCRU->CLK_UART_FRAC_MATRIX0_DIV_HIGH);
        break;
    case CLK_UART_FRAC1:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC1);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC1);
        fracOffset = &(TOPCRU->CLK_UART_FRAC_MATRIX1_DIV_HIGH);
        break;
    case CLK_AUDIO_FRAC0:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC0);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC0);
        fracOffset = &(TOPCRU->CLK_AUDIO_FRAC_MATRIX0_DIV_HIGH);
        break;
    case CLK_AUDIO_FRAC1:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC1);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC1);
        fracOffset = &(TOPCRU->CLK_AUDIO_FRAC_MATRIX1_DIV_HIGH);
        break;

    default:

        return HAL_INVAL;
    }

    switch (clockName) {
    case CLK_CM_FRAC0:
    case CLK_CM_FRAC1:
    case CLK_CM_FRAC2:
        if ((!(PLL_INPUT_OSC_RATE % rate))) {
            pRate = PLL_INPUT_OSC_RATE;
            mux = 0;
        } else if (!(s_cpllFreq % rate)) {
            pRate = s_cpllFreq;
            mux = 3;
        } else if (!(s_aupllFreq % rate)) {
            pRate = s_aupllFreq;
            mux = 2;
        } else {
            pRate = s_gpllFreq;
            mux = 1;
        }
        HAL_CRU_FracdivGetConfigV2(rate, pRate, &n, &m);
        HAL_CRU_ClkSetMux(muxSrc, mux);
        WRITE_REG(*fracOffset, (((n & 0xff0000) >> 16) << 8) | ((m & 0xff0000) >> 16));
        HAL_CRU_ClkSetFracDiv(divFrac, n & 0xffff, m & 0xffff);

        break;
    case CLK_UART_FRAC0:
    case CLK_UART_FRAC1:
        if ((!(PLL_INPUT_OSC_RATE % rate))) {
            pRate = PLL_INPUT_OSC_RATE;
            mux = 0;
        } else if (!(s_cpllFreq % rate)) {
            pRate = s_cpllFreq;
            mux = 2;
        } else {
            pRate = s_gpllFreq;
            mux = 1;
        }
        HAL_CRU_FracdivGetConfigV2(rate, pRate, &n, &m);
        HAL_CRU_ClkSetMux(muxSrc, mux);
        WRITE_REG(*fracOffset, (((n & 0xff0000) >> 16) << 8) | ((m & 0xff0000) >> 16));
        HAL_CRU_ClkSetFracDiv(divFrac, n & 0xffff, m & 0xffff);

        break;
    case CLK_AUDIO_FRAC0:
    case CLK_AUDIO_FRAC1:
        if ((!(PLL_INPUT_OSC_RATE % rate))) {
            pRate = PLL_INPUT_OSC_RATE;
            mux = 0;
        } else if (!(s_aupllFreq % rate)) {
            pRate = s_aupllFreq;
            mux = 2;
        } else {
            pRate = s_gpllFreq;
            mux = 1;
        }
        HAL_CRU_FracdivGetConfigV2(rate, pRate, &n, &m);
        HAL_CRU_ClkSetMux(muxSrc, mux);
        WRITE_REG(*fracOffset, (((n & 0xff0000) >> 16) << 8) | ((m & 0xff0000) >> 16));
        HAL_CRU_ClkSetFracDiv(divFrac, n & 0xffff, m & 0xffff);

        break;

    default:

        return HAL_INVAL;
    }

    switch (clockName) {
    case CLK_CM_FRAC0:
        s_fracCom0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_CM_FRAC1:
        s_fracCom1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_CM_FRAC2:
        s_fracCom2Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_UART_FRAC0:
        s_fracUart0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_UART_FRAC1:
        s_fracUart1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_AUDIO_FRAC0:
        s_fracVoice0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_AUDIO_FRAC1:
        s_fracVoice1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;

    default:

        return HAL_INVAL;
    }
}

static uint32_t HAL_CRU_ClkGetOtherFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = s_gpllFreq, freq;
    uint32_t uartTable[6] = { PLL_INPUT_OSC_RATE, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_fracUart0Freq, s_fracUart1Freq };
    uint32_t voiceTable0[9] = { PLL_INPUT_OSC_RATE, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                                s_intVoice0Freq, s_intVoice1Freq, s_sai0MclkIoIn };
    uint32_t voiceTable1[11] = { PLL_INPUT_OSC_RATE, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                                 s_intVoice0Freq, s_intVoice1Freq, s_sai0MclkIoIn, s_sai1MclkIoIn, s_sai2MclkIoIn };

    switch (clockName) {
    case SCLK_UART0_SRC:
    case SCLK_UART1:
    case SCLK_UART2:
    case SCLK_UART3:
    case SCLK_UART4:
    case SCLK_UART5:
    case SCLK_UART6:
    case SCLK_UART7:
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, uartTable, 6);
        break;
    case MCLK_SAI0:
        voiceTable0[8] = s_sai0MclkIoIn;
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, voiceTable0, 9);
        break;
    case MCLK_SAI1:
        voiceTable0[8] = s_sai1MclkIoIn;
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, voiceTable0, 9);
        break;
    case MCLK_SAI2:
        voiceTable0[8] = s_sai2MclkIoIn;
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, voiceTable0, 9);
        break;

    case MCLK_ASRC0:
    case MCLK_ASRC1:
    case MCLK_ASRC2:
    case MCLK_ASRC3:
    case MCLK_PDM:
    case CLKOUT_PDM:
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, voiceTable1, 11);
        break;
    default:
        break;
    }
    if (clkDiv) {
        freq = pRate / (HAL_CRU_ClkGetDiv(clkDiv));
    } else {
        freq = pRate;
    }

    return freq;
}

static HAL_Status HAL_CRU_ClkSetOtherFreq(eCLOCK_Name clockName, uint32_t rate)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = s_gpllFreq, mux = 0, div;
    uint32_t uartTable[6] = { PLL_INPUT_OSC_RATE, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_fracUart0Freq, s_fracUart1Freq };
    uint32_t voiceTable0[9] = { PLL_INPUT_OSC_RATE, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                                s_intVoice0Freq, s_intVoice1Freq, s_sai0MclkIoIn };
    uint32_t voiceTable1[11] = { PLL_INPUT_OSC_RATE, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                                 s_intVoice0Freq, s_intVoice1Freq, s_sai0MclkIoIn, s_sai1MclkIoIn, s_sai2MclkIoIn };

    switch (clockName) {
    case SCLK_UART0_SRC:
    case SCLK_UART1:
    case SCLK_UART2:
    case SCLK_UART3:
    case SCLK_UART4:
    case SCLK_UART5:
    case SCLK_UART6:
    case SCLK_UART7:
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, uartTable, 6, &pRate, 1);
        break;
    case MCLK_SAI0:
        voiceTable0[8] = s_sai0MclkIoIn;
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, voiceTable0, 9, &pRate, 1);
        break;
    case MCLK_SAI1:
        voiceTable0[8] = s_sai1MclkIoIn;
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, voiceTable0, 9, &pRate, 1);
        break;
    case MCLK_SAI2:
        voiceTable0[8] = s_sai2MclkIoIn;
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, voiceTable0, 9, &pRate, 1);
        break;

    case MCLK_ASRC0:
    case MCLK_ASRC1:
    case MCLK_ASRC2:
    case MCLK_ASRC3:
    case MCLK_PDM:
    case CLKOUT_PDM:
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, voiceTable1, 11, &pRate, 1);
        break;
    default:
        break;
    }

    div = HAL_DIV_ROUND_UP(pRate, rate);
    if (clkMux) {
        HAL_CRU_ClkSetMux(clkMux, mux);
    }
    if (clkDiv) {
        HAL_CRU_ClkSetDiv(clkDiv, div);
    }

    return HAL_OK;
}

/**
 * @brief init pll freq.
 */
static void CRU_InitPlls(void)
{
    s_cpllFreq = HAL_CRU_GetPllFreq(&CPLL);
    s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);
    s_aupllFreq = HAL_CRU_GetPllFreq(&AUPLL);
}

/**
 * @brief Get clk freq.
 * @param  clockName: CLOCK_Name id.
 * @return rate.
 * @attention these APIs allow direct use in the HAL layer.
 */
uint32_t HAL_CRU_ClkGetFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = 0, freq;

    if (!s_gpllFreq) {
        CRU_InitPlls();
    }

    switch (clockName) {
    case PLL_GPLL:
        freq = HAL_CRU_GetPllFreq(&GPLL);
        s_gpllFreq = freq;

        return freq;
    case PLL_CPLL:
        freq = HAL_CRU_GetPllFreq(&CPLL);
        s_cpllFreq = freq;

        return freq;
    case PLL_AUPLL:
        freq = HAL_CRU_GetPllFreq(&AUPLL);
        s_aupllFreq = freq;

        return freq;

    case CLK_CM_FRAC0:
    case CLK_CM_FRAC1:
    case CLK_CM_FRAC2:
    case CLK_UART_FRAC0:
    case CLK_UART_FRAC1:
    case CLK_AUDIO_FRAC0:
    case CLK_AUDIO_FRAC1:
        freq = HAL_CRU_ClkFracGetFreq(clockName);

        return freq;
    case CLK_AUDIO_INT0:
    case CLK_AUDIO_INT1:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, PLL_INPUT_OSC_RATE, s_gpllFreq, s_aupllFreq);
        break;
    case CLK_ASRC0:
    case CLK_ASRC1:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq, s_aupllFreq);
        break;
    case CLK_CORE_RGA:
    case ACLK_VDO:
    case ACLK_RKVDEC:
    case DCLK_VOP:
    case ACLK_SYSMEM:
    case CLK_CORE_FEC:
    case DCLK_OOC:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq, s_cpllFreq);
        break;
    case CLK_SYS_DSMC:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, PLL_INPUT_OSC_RATE, s_gpllFreq, s_cpllFreq);
        break;
    case ACLK_VEPU:
    case ACLK_VCP:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_cpllFreq / 2, s_gpllFreq / 3, s_gpllFreq / 6);
        break;
    case ACLK_GMAC:
    case CLK_PKA_RKCE:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 4, s_gpllFreq / 6);
        break;
    case ACLK_VI:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_cpllFreq / 2, s_gpllFreq / 3, s_gpllFreq / 4);
        break;
    case DCLK_VICAP:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_cpllFreq / 3, s_gpllFreq / 6);
        break;
    case ACLK_TOP:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq / 2, s_gpllFreq / 3, s_gpllFreq / 6);
        break;
    case ACLK_BUS:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq / 3, s_gpllFreq / 4, s_gpllFreq / 6);
        break;
    case HCLK_BUS:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 6, s_cpllFreq / 10);
        break;
    case CCLK_SDMMC0:
    case CCLK_SDMMC1:
    case CCLK_EMMC:
    case SCLK_2X_FSPI0:
    case CLK_CAN0:
    case CLK_CAN1:
    case CLK_MAC_OUT2IO:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_cpllFreq, PLL_INPUT_OSC_RATE);
        break;
    case CLK_GMAC_PTP_REF:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_cpllFreq, PLL_INPUT_OSC_RATE);
        break;
    case CLK_TIMER:
    case TCLK_WDT_NS:
    case TCLK_WDT_S:
    case TCLK_WDT_HPMCU:
    case PCLK_PERI:
    case CLK_PWM0:
    case CLK_PWM2:
    case CLK_PWM3:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_cpllFreq / 10, PLL_INPUT_OSC_RATE);
        break;
    case ACLK_PERI:
    case CLK_I2C_BUS:
    case ACLK_RKCE:
    case CLK_SARADC0:
    case CLK_SARADC1:
    case CLK_SARADC2:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 6, PLL_INPUT_OSC_RATE);
        break;
    case CLK_SPI0:
    case CLK_SPI1:
        pRate = HAL_CRU_MuxGetFreq4(clkMux, s_gpllFreq / 6, s_cpllFreq / 10, s_cpllFreq / 20, PLL_INPUT_OSC_RATE);
        break;
    case CLK_ISP_PLL:
    case CLK_AISP_PLL:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_aupllFreq, s_cpllFreq);
        break;
    case CLK_MIPI0_OUT2IO:
    case CLK_MIPI1_OUT2IO:
    case CLK_MIPI2_OUT2IO:
    case CLK_MIPI3_OUT2IO:
    case CLK_CIF_OUT2IO:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 2, PLL_INPUT_OSC_RATE);
        break;
    case MCLK_SAI0_OUT2IO:
        pRate = HAL_CRU_ClkGetFreq(MCLK_SAI0);
        break;
    case MCLK_SAI1_OUT2IO:
        pRate = HAL_CRU_ClkGetFreq(MCLK_SAI1);
        break;
    case MCLK_SAI2_OUT2IO:
        pRate = HAL_CRU_ClkGetFreq(MCLK_SAI2);
        break;
    case CLK_PWM1:
    case CLK_I2C2:
    case TCLK_WDT_LPMCU:
    case SCLK_1X_FSPI1:
    case MCLK_LPSAI:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, PLL_INPUT_OSC_RATE, s_clkRcOsc, 32768);
        break;
    case SCLK_UART0:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, HAL_CRU_ClkGetFreq(SCLK_UART0_SRC), PLL_INPUT_OSC_RATE, s_clkRcOsc);
        break;
    default:
        return HAL_CRU_ClkGetOtherFreq(clockName);
    }

    if ((clkMux == 0) && (clkDiv == 0)) {
        return 0;
    }

    if (clkDiv) {
        freq = pRate / (HAL_CRU_ClkGetDiv(clkDiv));
    } else {
        freq = pRate;
    }

    return freq;
}

/**
 * @brief Set clk freq.
 * @param  clockName: CLOCK_Name id.
 * @param  rate: clk rate.
 * @return HAL_Status.
 * @attention these APIs allow direct use in the HAL layer.
 */
HAL_Status HAL_CRU_ClkSetFreq(eCLOCK_Name clockName, uint32_t rate)
{
    HAL_Status error = HAL_OK;
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t mux = 0, div = 0, pRate = 0;

    if (!s_gpllFreq) {
        CRU_InitPlls();
    }

    switch (clockName) {
    case PLL_GPLL:
        error = HAL_CRU_SetPllFreq(&GPLL, rate);
        s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);

        return error;
    case PLL_CPLL:
        error = HAL_CRU_SetPllFreq(&CPLL, rate);
        s_cpllFreq = HAL_CRU_GetPllFreq(&CPLL);

        return error;
    case PLL_AUPLL:
        error = HAL_CRU_SetPllFreq(&AUPLL, rate);
        s_aupllFreq = HAL_CRU_GetPllFreq(&AUPLL);

        return error;

    case CLK_CM_FRAC0:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracCom0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return error;
    case CLK_CM_FRAC1:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracCom1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return error;
    case CLK_CM_FRAC2:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracCom2Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return error;
    case CLK_UART_FRAC0:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracUart0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return error;
    case CLK_UART_FRAC1:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracUart1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return error;
    case CLK_AUDIO_FRAC0:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracVoice0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return error;
    case CLK_AUDIO_FRAC1:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracVoice1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return error;
    case CLK_AUDIO_INT0:
        mux = HAL_CRU_RoundFreqGetMux3(rate, PLL_INPUT_OSC_RATE, s_gpllFreq, s_aupllFreq, &pRate);
        div = HAL_DIV_ROUND_UP(pRate, rate);
        HAL_CRU_ClkSetMux(clkMux, mux);
        HAL_CRU_ClkSetDiv(clkDiv, div);
        s_intVoice0Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT0);

        return HAL_OK;

    case CLK_AUDIO_INT1:
        mux = HAL_CRU_RoundFreqGetMux3(rate, PLL_INPUT_OSC_RATE, s_gpllFreq, s_aupllFreq, &pRate);
        div = HAL_DIV_ROUND_UP(pRate, rate);
        HAL_CRU_ClkSetMux(clkMux, mux);
        HAL_CRU_ClkSetDiv(clkDiv, div);
        s_intVoice1Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT1);

        return HAL_OK;

    case CLK_ASRC0:
    case CLK_ASRC1:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq, s_aupllFreq, &pRate);
        break;
    case CLK_CORE_RGA:
    case ACLK_VDO:
    case ACLK_RKVDEC:
    case DCLK_VOP:
    case ACLK_SYSMEM:
    case CLK_CORE_FEC:
    case DCLK_OOC:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq, s_cpllFreq, &pRate);
        break;
    case CLK_SYS_DSMC:
        mux = HAL_CRU_RoundFreqGetMux3(rate, PLL_INPUT_OSC_RATE, s_gpllFreq, s_cpllFreq, &pRate);
        break;
    case ACLK_VEPU:
    case ACLK_VCP:
        mux = HAL_CRU_FreqGetMux3(rate, s_cpllFreq / 2, s_gpllFreq / 3, s_gpllFreq / 6);
        break;
    case ACLK_GMAC:
    case CLK_PKA_RKCE:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 4, s_gpllFreq / 6);
        break;
    case ACLK_VI:
        mux = HAL_CRU_FreqGetMux3(rate, s_cpllFreq / 2, s_gpllFreq / 3, s_gpllFreq / 4);
        break;
    case DCLK_VICAP:
        mux = HAL_CRU_FreqGetMux2(rate, s_cpllFreq / 3, s_gpllFreq / 6);
        break;
    case ACLK_TOP:
        mux = HAL_CRU_FreqGetMux3(rate, s_gpllFreq / 2, s_gpllFreq / 3, s_gpllFreq / 6);
        break;
    case ACLK_BUS:
        mux = HAL_CRU_FreqGetMux3(rate, s_gpllFreq / 3, s_gpllFreq / 4, s_gpllFreq / 6);
        break;
    case HCLK_BUS:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 6, s_cpllFreq / 10);
        break;
    case CCLK_SDMMC0:
    case CCLK_SDMMC1:
    case CCLK_EMMC:
    case SCLK_2X_FSPI0:
    case CLK_CAN0:
    case CLK_CAN1:
    case CLK_MAC_OUT2IO:
        mux = HAL_CRU_RoundFreqGetMux3(rate, s_gpllFreq, s_cpllFreq, PLL_INPUT_OSC_RATE, &pRate);
        break;
    case CLK_GMAC_PTP_REF:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_cpllFreq, PLL_INPUT_OSC_RATE, &pRate);
        break;
    case CLK_TIMER:
    case TCLK_WDT_NS:
    case TCLK_WDT_S:
    case TCLK_WDT_HPMCU:
    case PCLK_PERI:
    case CLK_PWM0:
    case CLK_PWM2:
    case CLK_PWM3:
        mux = HAL_CRU_FreqGetMux2(rate, s_cpllFreq / 10, PLL_INPUT_OSC_RATE);
        break;
    case ACLK_PERI:
    case CLK_I2C_BUS:
    case ACLK_RKCE:
    case CLK_SARADC0:
    case CLK_SARADC1:
    case CLK_SARADC2:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 6, PLL_INPUT_OSC_RATE);
        break;
    case CLK_SPI0:
    case CLK_SPI1:
        mux = HAL_CRU_FreqGetMux4(rate, s_gpllFreq / 6, s_cpllFreq / 10, s_cpllFreq / 20, PLL_INPUT_OSC_RATE);
        break;
    case CLK_ISP_PLL:
    case CLK_AISP_PLL:
        mux = HAL_CRU_RoundFreqGetMux3(rate, s_gpllFreq, s_aupllFreq, s_cpllFreq, &pRate);
        break;
    case CLK_MIPI0_OUT2IO:
    case CLK_MIPI1_OUT2IO:
    case CLK_MIPI2_OUT2IO:
    case CLK_MIPI3_OUT2IO:
    case CLK_CIF_OUT2IO:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq / 2, PLL_INPUT_OSC_RATE, &pRate);
        break;
    case MCLK_SAI0_OUT2IO:
        pRate = HAL_CRU_ClkGetFreq(MCLK_SAI0);
        break;
    case MCLK_SAI1_OUT2IO:
        pRate = HAL_CRU_ClkGetFreq(MCLK_SAI1);
        break;
    case MCLK_SAI2_OUT2IO:
        pRate = HAL_CRU_ClkGetFreq(MCLK_SAI2);
        break;

    case CLK_PWM1:
    case CLK_I2C2:
    case TCLK_WDT_LPMCU:
    case SCLK_1X_FSPI1:
    case MCLK_LPSAI:
        mux = HAL_CRU_RoundFreqGetMux3(rate, PLL_INPUT_OSC_RATE, s_clkRcOsc, 32768, &pRate);
        break;
    case SCLK_UART0:
        mux = HAL_CRU_FreqGetMux3(rate, HAL_CRU_ClkGetFreq(SCLK_UART0_SRC), PLL_INPUT_OSC_RATE, s_clkRcOsc);
        break;

    default:
        return HAL_CRU_ClkSetOtherFreq(clockName, rate);
    }

    if ((clkMux == 0) && (clkDiv == 0)) {
        return HAL_INVAL;
    }

    div = HAL_DIV_ROUND_UP(pRate, rate);
    if (clkMux) {
        HAL_CRU_ClkSetMux(clkMux, mux);
    }
    if (clkDiv) {
        HAL_CRU_ClkSetDiv(clkDiv, div);
    }

    return HAL_OK;
}

/**
 * @brief wdt glbrst enable.
 * @param  wdtType: wdt reset type.
 * @return HAL_OK.
 * @attention these APIs allow direct use in the HAL layer.
 */
HAL_Status HAL_CRU_WdtGlbRstEnable(eCRU_WdtRstType wdtType)
{
    uint32_t mask = 0, val = 0, mask_en = 0, val_en = 0;

    switch (wdtType) {
    case GLB_RST_FST_WDT0:
        mask = TOPCRU_CRU_GLB_RST_CON1_WDT_GLB_SRST_CTRL_MASK;
        val = (1 << TOPCRU_CRU_GLB_RST_CON1_WDT_GLB_SRST_CTRL_SHIFT);
        mask_en = TOPCRU_CRU_GLB_RST_CON2_WDT_GLB_SRST_EN_MASK;
        val_en = (1 << TOPCRU_CRU_GLB_RST_CON2_WDT_GLB_SRST_EN_SHIFT);
        break;
    case GLB_RST_SND_WDT0:
        mask = TOPCRU_CRU_GLB_RST_CON1_WDT_GLB_SRST_CTRL_MASK;
        mask_en = TOPCRU_CRU_GLB_RST_CON2_WDT_GLB_SRST_EN_MASK;
        val_en = (1 << TOPCRU_CRU_GLB_RST_CON2_WDT_GLB_SRST_EN_SHIFT);
        break;

    default:

        return HAL_INVAL;
    }

    TOPCRU->CRU_GLB_RST_CON1 = VAL_MASK_WE(mask, val);
    TOPCRU->CRU_GLB_RST_CON2 = VAL_MASK_WE(mask_en, val_en);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* SOC_RV1126B && HAL_CRU_MODULE_ENABLED */
