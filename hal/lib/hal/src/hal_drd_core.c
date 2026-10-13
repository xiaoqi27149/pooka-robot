/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_USB_DRD_MODULE_ENABLED) || defined(HAL_USB_XHCI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup USB_DRD_CORE
 *  @brief USB DRD CORE HAL module driver
 *  @{
 */

/** @defgroup USB_DRD_CORE_How_To_Use How To Use
 *  @{

 The USB DRD CORE HAL driver can be used as follows:

 - Fill parameters of Init structure in HAL_USBDRD_DEV structure.

 - Call HAL_DRD_CoreInit() API to initialize the DRD Core.

 - Call HAL_DRD_SetMode() API to initialize the DRD as Device mode or Host mode.

 - The upper HAL DRD_Gadget/xHCI driver will call the right routines for its internal processes.

 @} */

/** @defgroup USB_DRD_CORE_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/**
 * @brief  Issues core soft reset and PHY reset.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 * @return HAL status
 */
static HAL_Status DRD_CoreSoftReset(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    uint32_t reg;
    int retries = 1000;

    /*
     * We're resetting only the device side because,
     * if we're in host mode, XHCI driver will reset
     * the host block. If drd was configured for
     * host-only mode, then we can return early.
     */
    if (pCFG->drMode == USB_DR_MODE_HOST) {
        return HAL_OK;
    }

    reg = pUSB3DRD->DCTL;
    reg |= DRD_DCTL_CSFTRST;
    reg &= ~DRD_DCTL_RUN_STOP;
    reg &= ~DRD_DCTL_ULSTCHNGREQ_MASK;
    pUSB3DRD->DCTL = reg;

    /*
     * For DRD_usb31 controller 1.90a and later, the DCTL.CSFRST bit
     * is cleared only after all the clocks are synchronized. This can
     * take a little more than 50ms. Set the polling rate at 20ms
     * for 10 times instead.
     */

    if (DRD_VER_IS_WITHIN(DRD1, 190A, ANY) || DRD_IP_IS(DRD2)) {
        retries = 10;
    }

    do {
        reg = pUSB3DRD->DCTL;
        if (!(reg & DRD_DCTL_CSFTRST)) {
            goto done;
        }

        if (DRD_VER_IS_WITHIN(DRD1, 190A, ANY) || DRD_IP_IS(DRD2)) {
            HAL_DelayMs(20);
        } else {
            HAL_DelayUs(1);
        }
    } while (--retries);

    return HAL_TIMEOUT;

done:
    /*
     * For DRD_usb31 controller 1.80a and prior, once DCTL.CSFRST bit
     * is cleared, we must wait at least 50ms before accessing the PHY
     * domain (synchronization delay).
     */
    if (DRD_VER_IS_WITHIN(DRD1, ANY, 180A)) {
        HAL_DelayMs(50);
    }

    return HAL_OK;
}

/**
 * @brief  Get HW configuration params of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 */
static void DRD_CacheHwparams(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    struct DRD_HWPARAMS *parms = &pCFG->hwparams;

    parms->hwparams0 = pUSB3DRD->GHWPARAMS0;
    parms->hwparams1 = pUSB3DRD->GHWPARAMS1;
    parms->hwparams2 = pUSB3DRD->GHWPARAMS2;
    parms->hwparams3 = pUSB3DRD->GHWPARAMS3;
    parms->hwparams4 = pUSB3DRD->GHWPARAMS4;
    parms->hwparams5 = pUSB3DRD->GHWPARAMS5;
    parms->hwparams6 = pUSB3DRD->GHWPARAMS6;
    parms->hwparams7 = pUSB3DRD->GHWPARAMS7;
    parms->hwparams8 = pUSB3DRD->GHWPARAMS8;
}

/**
 * @brief  Configure USB2 PHY Interface of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 * @return HAL status
 */
static HAL_Status DRD_HsPhySetup(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    uint32_t reg;

    /* Set drd usb2 phy config */
    reg = USB3DRD_GUSB2PHYCFG(0);

    switch (pCFG->hsphyMode) {
    case USBPHY_INTERFACE_MODE_UTMI:
        reg &= ~(DRD_GUSB2PHYCFG_PHYIF_MASK |
                 DRD_GUSB2PHYCFG_USBTRDTIM_MASK);
        reg |= DRD_GUSB2PHYCFG_PHYIF(UTMI_PHYIF_8_BIT) |
               DRD_GUSB2PHYCFG_USBTRDTIM(USBTRDTIM_UTMI_8_BIT);
        break;
    case USBPHY_INTERFACE_MODE_UTMIW:
        reg &= ~(DRD_GUSB2PHYCFG_PHYIF_MASK |
                 DRD_GUSB2PHYCFG_USBTRDTIM_MASK);
        reg |= DRD_GUSB2PHYCFG_PHYIF(UTMI_PHYIF_16_BIT) |
               DRD_GUSB2PHYCFG_USBTRDTIM(USBTRDTIM_UTMI_16_BIT);
        break;
    default:
        break;
    }

    /*
     * Above DRD_usb3.0 1.94a, it is recommended to set
     * DRD_GUSB2PHYCFG_SUSPHY to '0' during coreConsultant configuration.
     * So default value will be '0' when the core is reset. Application
     * needs to set it to '1' after the core initialization is completed.
     *
     * Similarly for DRD controllers, GUSB2PHYCFG.SUSPHY must be cleared
     * after power-on reset, and it can be set after core initialization.
     */
    reg &= ~DRD_GUSB2PHYCFG_SUSPHY;

    if (pCFG->disEnblslpmQuirk) {
        reg &= ~DRD_GUSB2PHYCFG_ENBLSLPM;
    } else {
        reg |= DRD_GUSB2PHYCFG_ENBLSLPM;
    }

    if (pCFG->disU2FreeclkExistsQuirk) {
        reg &= ~DRD_GUSB2PHYCFG_U2_FREECLK_EXISTS;
    }

    USB3DRD_GUSB2PHYCFG(0) = reg;

    return HAL_OK;
}

/**
 * @brief  Configure USB3 PHY Interface of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 * @return HAL status
 */
static HAL_Status DRD_SsPhySetup(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    uint32_t reg;

    reg = USB3DRD_GUSB3PIPECTL(0);

    /*
     * Above DRD_usb3.0 1.94a, it is recommended to set
     * DRD_GUSB3PIPECTL_SUSPHY to '0' during coreConsultant configuration.
     * So default value will be '0' when the core is reset. Application
     * needs to set it to '1' after the core initialization is completed.
     *
     * Similarly for DRD controllers, GUSB3PIPECTL.SUSPENDENABLE must be
     * cleared after power-on reset, and it can be set after core
     * initialization.
     */
    reg &= ~DRD_GUSB3PIPECTL_SUSPHY;

    if (pCFG->u2ssInp3Quirk) {
        reg |= DRD_GUSB3PIPECTL_U2SSINP3OK;
    }

    if (pCFG->disRxdetInp3Quirk) {
        reg |= DRD_GUSB3PIPECTL_DISRXDETINP3;
    }

    if (pCFG->reqP1p2p3Quirk) {
        reg |= DRD_GUSB3PIPECTL_REQP1P2P3;
    }

    if (pCFG->delP1p2p3Quirk) {
        reg |= DRD_GUSB3PIPECTL_DEP1P2P3_EN;
    }

    if (pCFG->delPhyPowerChgQuirk) {
        reg |= DRD_GUSB3PIPECTL_DEPOCHANGE;
    }

    if (pCFG->lfpsFilterQuirk) {
        reg |= DRD_GUSB3PIPECTL_LFPSFILT;
    }

    if (pCFG->rxDetectPollQuirk) {
        reg |= DRD_GUSB3PIPECTL_RX_DETOPOLL;
    }

    if (pCFG->txDeEmphasisQuirk) {
        reg |= DRD_GUSB3PIPECTL_TX_DEEPH(pCFG->txDeEmphasis);
    }

    if (pCFG->disDelPhyPowerChgQuirk) {
        reg &= ~DRD_GUSB3PIPECTL_DEPOCHANGE;
    }

    USB3DRD_GUSB3PIPECTL(0) = reg;

    return HAL_OK;
}

/**
 * @brief  Configure USB PHY Interface of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 * @return HAL status
 */
static HAL_Status DRD_PhySetup(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    int ret;

    ret = DRD_SsPhySetup(pUSB3DRD, pCFG);
    if (ret) {
        return ret;
    }

    ret = DRD_HsPhySetup(pUSB3DRD, pCFG);
    if (ret) {
        return ret;
    }

    return HAL_OK;
}

/**
 * @brief  Get the IP number and revision of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 * @return HAL status
 */
static HAL_Check DRD_CoreIsValid(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    uint32_t reg;

    /* Check the parameters */
    HAL_ASSERT(IS_USB3DRD_INSTANCE(pUSB3DRD));

    /* Check the DRD configuration */
    HAL_ASSERT(pCFG != NULL)

    reg = pUSB3DRD->GSNPSID;
    pCFG->ip = DRD_GSNPS_ID(reg);

    /* This should read as U3 followed by revision number */
    if (DRD_IP_IS(DRD)) {
        pCFG->revision = reg;
    } else if (DRD_IP_IS(DRD1) || DRD_IP_IS(DRD2)) {
        pCFG->revision = USB3DRD_VER_NUMBER;
    } else {
        return HAL_FALSE;
    }

    return HAL_TRUE;
}

/**
 * @brief  Setup the global registers of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 */
static void DRD_CoreSetupGlobalControl(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    uint32_t reg;

    reg = pUSB3DRD->GCTL;
    reg &= ~DRD_GCTL_SCALEDOWN_MASK;

    switch (DRD_GHWPARAMS1_EN_PWROPT(pCFG->hwparams.hwparams1)) {
    case DRD_GHWPARAMS1_EN_PWROPT_CLK:
        /**
         * WORKAROUND: DRD revisions between 2.10a and 2.50a have an
         * issue which would cause xHCI compliance tests to fail.
         *
         * Because of that we cannot enable clock gating on such
         * configurations.
         *
         * Refers to:
         *
         * STAR#9000588375: Clock Gating, SOF Issues when ref_clk-Based
         * SOF/ITP Mode Used
         */
        if ((pCFG->drMode == USB_DR_MODE_HOST ||
             pCFG->drMode == USB_DR_MODE_OTG) &&
            (pCFG->revision >= DRD_REVISION_210A &&
             pCFG->revision <= DRD_REVISION_250A)) {
            reg |= DRD_GCTL_DSBLCLKGTNG | DRD_GCTL_SOFITPSYNC;
        } else {
            reg &= ~DRD_GCTL_DSBLCLKGTNG;
        }
        break;
    case DRD_GHWPARAMS1_EN_PWROPT_HIB:
        /* TODO */
        break;
    default:
        HAL_DBG("No power optimization available\n");
    }

    reg &= ~DRD_GCTL_DISSCRAMBLE;

    if (pCFG->u2exitLfpsQuirk) {
        reg |= DRD_GCTL_U2EXIT_LFPS;
    }

    /*
     * WORKAROUND: DRD revisions <1.90a have a bug
     * where the device can fail to connect at SuperSpeed
     * and falls back to high-speed mode which causes
     * the device to enter a Connect/Disconnect loop
     */
    if (pCFG->revision < DRD_REVISION_190A) {
        reg |= DRD_GCTL_U2RSTECN;
    }

    pUSB3DRD->GCTL = reg;
}

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup USB_DRD_CORE_Exported_Functions_Group1 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:
 *  @{
 */

/**
 * @brief  Low-level initialization of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 * @return HAL status
 */
HAL_Status HAL_DRD_CoreInit(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG)
{
    uint32_t reg;
    uint8_t lpmNyetThreshold;
    uint8_t txDeEmphasis;
    uint8_t hirdThreshold;
    HAL_Status ret;

    if (!DRD_CoreIsValid(pUSB3DRD, pCFG)) {
        HAL_DBG_ERR("this is not a DesignWare USB3 DRD Core\n");

        return HAL_NODEV;
    }

    /* default to highest possible threshold */
    lpmNyetThreshold = 0xff;

    /* default to -3.5dB de-emphasis */
    txDeEmphasis = 1;

    /*
     * default to assert utmi_sleep_n and use maximum
     * allowed HIRD threshold value of 0b1100
     */
    hirdThreshold = 12;

    if (pCFG->lpmNyetThreshold) {
        lpmNyetThreshold = pCFG->lpmNyetThreshold;
    }

    if (pCFG->txDeEmphasis) {
        txDeEmphasis = pCFG->txDeEmphasis;
    }

    if (pCFG->hirdThreshold) {
        hirdThreshold = pCFG->hirdThreshold;
    }

    pCFG->lpmNyetThreshold = lpmNyetThreshold;
    pCFG->txDeEmphasis = txDeEmphasis;
    pCFG->hirdThreshold = hirdThreshold | (pCFG->isUtmiL1Suspend << 4);

    if (pCFG->maximumSpeed == DRD_USB_SPEED_UNKNOWN) {
        pCFG->maximumSpeed = DRD_USB_SPEED_SUPER;
    }

    DRD_CacheHwparams(pUSB3DRD, pCFG);

    ret = DRD_PhySetup(pUSB3DRD, pCFG);
    if (ret) {
        return ret;
    }

    /* Initialize the USB2.0 PHY */
    HAL_USB_HsPhyInit(pCFG->hsphyId);

    /* Initialize the USB3.0 PHY */
    if (pCFG->maximumSpeed == DRD_USB_SPEED_SUPER) {
        HAL_USB_SsPhyInit(pCFG->ssphyId);
    }

    ret = DRD_CoreSoftReset(pUSB3DRD, pCFG);
    if (ret) {
        return ret;
    }

    if (!DRD_VER_IS_PRIOR(DRD, 250A)) {
        reg = pUSB3DRD->GUCTL1;

        if (pCFG->disTxIpGapLinecheckQuirk) {
            reg |= DRD_GUCTL1_TX_IPGAP_LINECHECK_DIS;
        }

        if (pCFG->parkmodeDisableHsQuirk) {
            reg |= DRD_GUCTL1_PARKMODE_DISABLE_HS;
        }

        if (pCFG->parkmodeDisableSsQuirk) {
            reg |= DRD_GUCTL1_PARKMODE_DISABLE_SS;
        }

        if (pCFG->maximumSpeed == DRD_USB_SPEED_HIGH ||
            pCFG->maximumSpeed == DRD_USB_SPEED_FULL) {
            reg |= DRD_GUCTL1_DEV_FORCE_20_CLK_FOR_30_CLK;
        }

        pUSB3DRD->GUCTL1 = reg;
    }

    DRD_CoreSetupGlobalControl(pUSB3DRD, pCFG);

    return HAL_OK;
}

/**
 * @brief  Low-level De-initialization of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @return HAL status
 */
HAL_Status HAL_DRD_CoreDeInit(struct USB3DRD_REG *pUSB3DRD)
{
    /* TODO */

    return HAL_OK;
}

/**
 * @brief  Set USB PHY suspend mode.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  pCFG: USB3DRD configuration
 * @param  enable: Enable suspend phy
 * @return HAL status
 */
HAL_Status HAL_DRD_EnableSusphy(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG, bool enable)
{
    uint32_t reg;

    reg = USB3DRD_GUSB3PIPECTL(0);
    if (enable && !pCFG->disU3SusphyQuirk) {
        reg |= DRD_GUSB3PIPECTL_SUSPHY;
    } else {
        reg &= ~DRD_GUSB3PIPECTL_SUSPHY;
    }

    USB3DRD_GUSB3PIPECTL(0) = reg;

    reg = USB3DRD_GUSB2PHYCFG(0);

    if (enable && !pCFG->disU2SusphyQuirk) {
        reg |= DRD_GUSB2PHYCFG_SUSPHY;
    } else {
        reg &= ~DRD_GUSB2PHYCFG_SUSPHY;
    }

    USB3DRD_GUSB2PHYCFG(0) = reg;

    return HAL_OK;
}

/**
 * @brief  Set the port mode of DRD Core.
 * @param  pUSB3DRD: USB3DRD register base address
 * @param  mode: Port mode
 * @return HAL status
 */
HAL_Status HAL_DRD_SetMode(struct USB3DRD_REG *pUSB3DRD, uint32_t mode)
{
    uint32_t reg;

    reg = pUSB3DRD->GCTL;
    reg &= ~(DRD_GCTL_PRTCAPDIR(DRD_GCTL_PRTCAP_OTG));
    reg |= DRD_GCTL_PRTCAPDIR(mode);
    pUSB3DRD->GCTL = reg;

    return HAL_OK;
}

/**
 * @brief  Allocate a memory block which address is aligned to the
 * specified alignment size.
 * @param  size: The allocated memory block size
 * @param  align: The alignment size
 * @return The memory block address was returned successfully,
 *         otherwise it was returned empty NULL.
 */
__WEAK void *HAL_DRD_MemMallocAlign(uintptr_t size, uintptr_t align)
{
    return NULL;
}

/**
 * @brief Release the memory block, which is allocated by
 * HAL_DRD_MemMallocAlign function and address is aligned.
 * @param ptr: The memory block pointer
 */
__WEAK void HAL_DRD_MemFreeAlign(void *ptr)
{
}

/** @} */

/** @} */

/** @} */

#endif /* defined(HAL_USB_DRD_MODULE_ENABLED) || defined(HAL_USB_XHCI_MODULE_ENABLED) */
