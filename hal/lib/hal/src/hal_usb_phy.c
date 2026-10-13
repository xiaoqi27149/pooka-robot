/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_PCD_MODULE_ENABLED) || defined(HAL_HCD_MODULE_ENABLED) || defined(HAL_USB_DRD_MODULE_ENABLED) || defined(HAL_USB_XHCI_MODULE_ENABLED) || defined(HAL_EHCI_MODULE_ENABLED) || defined(HAL_OHCI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup USB_PHY
 *  @brief USB PHY HAL module driver
 *  @{
 */

/** @defgroup USB_PHY_How_To_Use How To Use
 *  @{

 The USB PHY HAL driver can be used as follows:

 - Call HAL_USB_HsPhyInit() API to initialize USB high speed phy.

 - Call HAL_USB_SsPhyInit() API to initialize USB Super speed phy.

 - Call HAL_USB_PhySuspend() API to suspend USB phy.

 - Call HAL_USB_PhyResume() API to resume USB phy.

 - Call HAL_USB_PhyBvalidIrq*() APIs to detect USB vbus status.

 @} */

/** @defgroup USB_PHY_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define USB_PHY_CLEAR_BIT(REG, MASK) ((*(volatile uint32_t *)((uintptr_t)(REG))) &= ~(MASK))
#define USB_PHY_SET_BIT(REG, BIT)    ((*(volatile uint32_t *)((uintptr_t)(REG))) |= (BIT))
#define USB_PHY_READ_REG(REG)        ((*(volatile const uint32_t *)((uintptr_t)(REG))))
#define USB_PHY_WRITE_REG(REG, VAL)  ((*(volatile uint32_t *)((uintptr_t)(REG))) = (VAL))
#define USB_PHY_MODIFY_REG(REG, CLEARMASK, SETMASK) \
        USB_PHY_WRITE_REG((REG), (((USB_PHY_READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/********************* Private Structure Definition **************************/
/********************* Private Variable Definition ***************************/
/********************* Private Function Definition ***************************/
#if defined(COMB_PIPE_PHY0_BASE) || defined(COMB_PIPE_PHY1_BASE)
static HAL_Status USB_WaitPipePhyReady(__I uint32_t *reg, uint32_t bit, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i <= timeout; i++) {
        if (!READ_BIT(*reg, bit)) {
            return HAL_OK;
        }
        HAL_DelayUs(1);
    }

    return HAL_TIMEOUT;
}
#endif

/** @} */

/********************* Public Function Definition ****************************/
/** @defgroup USB_PHY_Exported_Functions_Group1 Init and DeInit Functions

 This section provides functions allowing to init and deinit the USB PHY module:
 *  @{
 */

/**
 * @brief  Initialize the USB High Speed PHY.
 * @param  id: USB High Speed PHY id
 * @return HAL status
 * @attention This API allow direct use in the HAL layer.
 */
HAL_Status HAL_USB_HsPhyInit(uint8_t id)
{
#if defined(RKMCU_RK2108) || defined(RKMCU_PISCES)
    /* Deassert PHY reset */
    HAL_CRU_ClkResetDeassert(SRST_USB2PHYPO);

    /* Select USB controller UTMI interface to phy */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_RESUME_MASK,
                      USB_PHY_RESUME_VAL << USB_PHY_CON_SHIFT);

    /* Wait for UTMI clk stable */
    HAL_DelayMs(2);
#elif defined(RKMCU_RK2118)
    /* Set bvalid and iddig comes from USBPHY */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE,
                      GRF_SOC_CON24_USBOTG_UTMI_IDDIG_SEL_MASK |
                      GRF_SOC_CON24_USBOTG_UTMI_BVALID_SEL_MASK,
                      (0x00 << GRF_SOC_CON24_USBOTG_UTMI_IDDIG_SEL_SHIFT) |
                      (0x00 << GRF_SOC_CON24_USBOTG_UTMI_BVALID_SEL_SHIFT));

    /* Set HS disconnect detect mode to single ended detect mode */
    USB_PHY_WRITE_REG(USB_PHY_APB_BASE + 0x0070U, 0xb4);
#elif defined(RKMCU_RK2206)
    /* Reset USB PHY */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_SUSPEND_MASK,
                      USB_PHY_SUSPEND_VAL << USB_PHY_CON_SHIFT);
    HAL_CRU_ClkResetAssert(SRST_OTG_USBPHY);
    HAL_DelayUs(15);
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_RESUME_MASK,
                      USB_PHY_RESUME_VAL << USB_PHY_CON_SHIFT);
    HAL_DelayUs(1500);
    HAL_CRU_ClkResetDeassert(SRST_OTG_USBPHY);

    /* Wait for UTMI clk stable */
    HAL_DelayUs(2);
#elif defined(SOC_RK1820)
    /* Turn off HS differential receiver by default to save power */
    USB_PHY_CLEAR_BIT(USB_PHY_APB_BASE + 0x0030U, HAL_BIT(2));
    USB_PHY_CLEAR_BIT(USB_PHY_APB_BASE + 0x0430U, HAL_BIT(2));

    /* Enable HS pre-emphasis during non-chirp phase */
    USB_PHY_MODIFY_REG(USB_PHY_APB_BASE, HAL_GENMASK(2, 0), 0x04);
    USB_PHY_MODIFY_REG(USB_PHY_APB_BASE + 0x0400U, HAL_GENMASK(2, 0), HAL_BIT(2));

    /* Set HS eye height to 425mv(default is 400mv) */
    USB_PHY_MODIFY_REG(USB_PHY_APB_BASE + 0x0030U, HAL_GENMASK(6, 4), (0x05 << 4));
    USB_PHY_MODIFY_REG(USB_PHY_APB_BASE + 0x0430U, HAL_GENMASK(6, 4), (0x05 << 4));
#elif defined(SOC_RK3568)
    USB_PHY_WRITE_REG(USB_PHY0_BASE + 0x0004U, USB_PHY_RESUME_VAL);
    USB_PHY_WRITE_REG(USB_PHY1_BASE, USB_PHY_RESUME_VAL);
    USB_PHY_WRITE_REG(USB_PHY1_BASE + 0x0004U, USB_PHY_RESUME_VAL);
#elif defined(SOC_SWALLOW) && defined(IS_FPGA)
    /*
     * Reset USB PHY only for Swallow FPGA
     * USB_PHY_WRITE_REG(CRU_BASE + 0x1004U, 0x1);
     * HAL_DelayUs(500);
     * USB_PHY_WRITE_REG(CRU_BASE + 0x1004U, 0x0);
     * HAL_DelayUs(2000);
     */
#endif

    return HAL_OK;
}

/**
 * @brief  Initialize the USB Super Speed PHY.
 * @param  id: USB Super Speed PHY id
 * @return HAL status
 * @attention This API allow direct use in the HAL layer.
 */
HAL_Status HAL_USB_SsPhyInit(uint8_t id)
{
    HAL_Status ret = HAL_OK;

    switch (id) {
    case 0:
#ifdef COMB_PIPE_PHY0_BASE
        /* Set SSC downward spread spectrum */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY0_BASE + 0x007CU, HAL_GENMASK(5, 4), HAL_BIT(4));

        /* Enable adaptive CTLE for USB3.0 Rx */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY0_BASE + 0x0038U, HAL_GENMASK(0, 0), HAL_BIT(0));

        /* Set PLL KVCO fine tuning signals */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY0_BASE + 0x0080U, HAL_GENMASK(4, 2), HAL_BIT(3));

        /* Set PLL input clock divider 1/2 */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY0_BASE + 0x0014U, HAL_GENMASK(7, 6), HAL_BIT(6));

        /* Set PLL LPF R1 to su_trim[10:7]=1001 */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY0_BASE + 0x002CU, HAL_BIT(2));

        /* Set PLL loop divider */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY0_BASE + 0x0044U, 0x32);

        /* Set PLL KVCO to min and set PLL charge pump current to max */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY0_BASE + 0x0028U, 0xF0);

        /* Set Rx squelch input filler bandwidth */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY0_BASE + 0x0050, 0x0E);

        /* Set PIPE PHY GRF registers */
        WRITE_REG_MASK_WE(PIPE_PHY_GRF0->PIPE_CON0,
                          PIPE_PHY_GRF_PIPE_CON0_PIPE_PHYMODE_MASK |
                          PIPE_PHY_GRF_PIPE_CON0_PIPE_RATE_MASK,
                          (0x01 << PIPE_PHY_GRF_PIPE_CON0_PIPE_PHYMODE_SHIFT) |
                          (0x00 << PIPE_PHY_GRF_PIPE_CON0_PIPE_RATE_SHIFT));
        WRITE_REG_MASK_WE(PIPE_PHY_GRF0->PIPE_CON1,
                          PIPE_PHY_GRF_PIPE_CON1_PHY_CLK_SEL_MASK,
                          0x02 << PIPE_PHY_GRF_PIPE_CON1_PHY_CLK_SEL_SHIFT);
        WRITE_REG_MASK_WE(PIPE_PHY_GRF0->PIPE_CON2,
                          PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXELECIDLE_MASK |
                          PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXCOMPLIANCE_MASK,
                          (0x00 << PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXELECIDLE_SHIFT) |
                          (0x00 << PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXCOMPLIANCE_SHIFT));
        WRITE_REG_MASK_WE(PIPE_PHY_GRF0->PIPE_CON3,
                          PIPE_PHY_GRF_PIPE_CON3_PIPE_SEL_MASK,
                          0x01 << PIPE_PHY_GRF_PIPE_CON3_PIPE_SEL_SHIFT);

        ret = USB_WaitPipePhyReady(&PIPE_PHY_GRF0->PIPE_STATUS1,
                                   PIPE_PHY_GRF_PIPE_STATUS1_PIPE_PHYSTATUS_O_MASK,
                                   1000);
        if (ret) {
            HAL_DBG_ERR("wait combphy%d status ready timeout\n", id);

            return ret;
        }
#endif
        break;
    case 1:
#ifdef COMB_PIPE_PHY1_BASE
        /* Set SSC downward spread spectrum */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY1_BASE + 0x007CU, HAL_GENMASK(5, 4), HAL_BIT(4));

        /* Enable adaptive CTLE for USB3.0 Rx */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY1_BASE + 0x0038U, HAL_GENMASK(0, 0), HAL_BIT(0));

        /* Set PLL KVCO fine tuning signals */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY1_BASE + 0x0080U, HAL_GENMASK(4, 2), HAL_BIT(3));

        /* Set PLL input clock divider 1/2 */
        USB_PHY_MODIFY_REG(COMB_PIPE_PHY1_BASE + 0x0014U, HAL_GENMASK(7, 6), HAL_BIT(6));

        /* Set PLL LPF R1 to su_trim[10:7]=1001 */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY1_BASE + 0x002CU, HAL_BIT(2));

        /* Set PLL loop divider */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY1_BASE + 0x0044U, 0x32);

        /* Set PLL KVCO to min and set PLL charge pump current to max */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY1_BASE + 0x0028U, 0xF0);

        /* Set Rx squelch input filler bandwidth */
        USB_PHY_WRITE_REG(COMB_PIPE_PHY1_BASE + 0x0050, 0x0E);

        /* Set PIPE PHY GRF registers */
        WRITE_REG_MASK_WE(PIPE_PHY_GRF1->PIPE_CON0,
                          PIPE_PHY_GRF_PIPE_CON0_PIPE_PHYMODE_MASK |
                          PIPE_PHY_GRF_PIPE_CON0_PIPE_RATE_MASK,
                          (0x01 << PIPE_PHY_GRF_PIPE_CON0_PIPE_PHYMODE_SHIFT) |
                          (0x00 << PIPE_PHY_GRF_PIPE_CON0_PIPE_RATE_SHIFT));
        WRITE_REG_MASK_WE(PIPE_PHY_GRF1->PIPE_CON1,
                          PIPE_PHY_GRF_PIPE_CON1_PHY_CLK_SEL_MASK,
                          0x02 << PIPE_PHY_GRF_PIPE_CON1_PHY_CLK_SEL_SHIFT);
        WRITE_REG_MASK_WE(PIPE_PHY_GRF1->PIPE_CON2,
                          PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXELECIDLE_MASK |
                          PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXCOMPLIANCE_MASK,
                          (0x00 << PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXELECIDLE_SHIFT) |
                          (0x00 << PIPE_PHY_GRF_PIPE_CON2_SEL_PIPE_TXCOMPLIANCE_SHIFT));
        WRITE_REG_MASK_WE(PIPE_PHY_GRF1->PIPE_CON3,
                          PIPE_PHY_GRF_PIPE_CON3_PIPE_SEL_MASK,
                          0x01 << PIPE_PHY_GRF_PIPE_CON3_PIPE_SEL_SHIFT);

        ret = USB_WaitPipePhyReady(&PIPE_PHY_GRF1->PIPE_STATUS1,
                                   PIPE_PHY_GRF_PIPE_STATUS1_PIPE_PHYSTATUS_O_MASK,
                                   1000);
        if (ret) {
            HAL_DBG_ERR("wait combphy%d status ready timeout\n", id);

            return ret;
        }
#endif
        break;
    default:
        break;
    }

    return ret;
}

/** @} */

/** @defgroup USB_PHY_Exported_Functions_Group2 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the USB PHY module:

 *  @{
 */

/**
 * @brief  Suspend the USB PHY.
 * @param  id: USB PHY id
 * @return HAL status
 */
HAL_Status HAL_USB_PhySuspend(uint8_t id)
{
    switch (id) {
    case 0:
#ifdef USB_PHY_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_SUSPEND_MASK,
                          USB_PHY_SUSPEND_VAL << USB_PHY_CON_SHIFT);
#endif
        break;
    case 1:
#ifdef USB_PHY1_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY1_CON_BASE, USB_PHY1_SUSPEND_MASK,
                          USB_PHY1_SUSPEND_VAL << USB_PHY1_CON_SHIFT);
#endif
        break;
    default:
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Resume the USB PHY.
 * @param  id: USB PHY id
 * @return HAL status
 */
HAL_Status HAL_USB_PhyResume(uint8_t id)
{
    switch (id) {
    case 0:
#ifdef USB_PHY_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_RESUME_MASK,
                          USB_PHY_RESUME_VAL << USB_PHY_CON_SHIFT);
#endif
        break;
    case 1:
#ifdef USB_PHY1_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY1_CON_BASE, USB_PHY1_RESUME_MASK,
                          USB_PHY1_RESUME_VAL << USB_PHY1_CON_SHIFT);
#endif
        break;
    default:
        break;
    }

    return HAL_OK;
}

/** @} */

/** @defgroup USB_PHY_Exported_Functions_Group3 Bvalid Irq Functions

 This section provides functions allowing to manage bvalid irq:

 *  @{
 */

/**
 * @brief  Enable USB PHY bvalid irq.
 * @return HAL status
 */
HAL_Status HAL_USB_PhyBvalidIrqEnable(void)
{
#ifdef USB_PHY_BVALID_IRQ_CON_BASE
    /* Clear bvalid rise and fall irq state */
    WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CLR_BASE,
                      USB_PHY_BVALID_RISE_IRQ_CLR_MASK |
                      USB_PHY_BVALID_FALL_IRQ_CLR_MASK,
                      USB_PHY_BVALID_RISE_IRQ_CLR_EN |
                      USB_PHY_BVALID_FALL_IRQ_CLR_EN);

    /* Enable bvalid rise and fall irq */
    WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CON_BASE,
                      USB_PHY_BVALID_RISE_IRQ_CON_MASK |
                      USB_PHY_BVALID_FALL_IRQ_CON_MASK,
                      USB_PHY_BVALID_RISE_IRQ_CON_EN |
                      USB_PHY_BVALID_FALL_IRQ_CON_EN);
#endif

    return HAL_OK;
}

/**
 * @brief  Disable USB PHY bvalid irq.
 * @return HAL status
 */
HAL_Status HAL_USB_PhyBvalidIrqDisable(void)
{
#ifdef USB_PHY_BVALID_IRQ_CON_BASE
    /* Disable bvalid rise and fall irq */
    WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CON_BASE,
                      USB_PHY_BVALID_RISE_IRQ_CON_MASK |
                      USB_PHY_BVALID_FALL_IRQ_CON_MASK,
                      0);
#endif

    return HAL_OK;
}

/**
 * @brief  Get USB PHY bvalid irq rise status.
 * @return Bvalid irq rise status.
 */
uint8_t HAL_USB_PhyBvalidIrqRiseStatus(void)
{
    uint8_t irq_status = 0;

#ifdef USB_PHY_BVALID_IRQ_STATUS_BASE
    irq_status = READ_BIT(USB_PHY_BVALID_IRQ_STATUS_BASE,
                          USB_PHY_BVALID_RISE_IRQ_STATUS_MASK);
    irq_status >>= USB_PHY_BVALID_RISE_IRQ_STATUS_SHIFT;
#endif

    return irq_status;
}

/**
 * @brief  Get USB PHY bvalid irq fall status.
 * @return Bvalid irq fall status.
 */
uint8_t HAL_USB_PhyBvalidIrqFallStatus(void)
{
    uint8_t irq_status = 0;

#ifdef USB_PHY_BVALID_IRQ_STATUS_BASE
    irq_status = READ_BIT(USB_PHY_BVALID_IRQ_STATUS_BASE,
                          USB_PHY_BVALID_FALL_IRQ_STATUS_MASK);
    irq_status >>= USB_PHY_BVALID_FALL_IRQ_STATUS_SHIFT;
#endif

    return irq_status;
}

/**
 * @brief Clear USB PHY bvalid irq rise status.
 */
void HAL_USB_PhyBvalidIrqRiseStatusClear(void)
{
#ifdef USB_PHY_BVALID_IRQ_CLR_BASE
    WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CLR_BASE,
                      USB_PHY_BVALID_RISE_IRQ_CLR_MASK,
                      USB_PHY_BVALID_RISE_IRQ_CLR_EN);
#endif
}

/**
 * @brief Clear USB PHY bvalid irq fall status.
 */
void HAL_USB_PhyBvalidIrqFallStatusClear(void)
{
#ifdef USB_PHY_BVALID_IRQ_CLR_BASE
    WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CLR_BASE,
                      USB_PHY_BVALID_FALL_IRQ_CLR_MASK,
                      USB_PHY_BVALID_FALL_IRQ_CLR_EN);
#endif
}

/** @} */

/** @} */

/** @} */

#endif /* defined (HAL_PCD_MODULE_ENABLED) || defined (HAL_HCD_MODULE_ENABLED) || defined(HAL_USB_DRD_MODULE_ENABLED) || defined(HAL_USB_XHCI_MODULE_ENABLED) || defined(HAL_EHCI_MODULE_ENABLED) || defined(HAL_OHCI_MODULE_ENABLED) */
