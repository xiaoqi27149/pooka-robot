/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_PCD_MODULE_ENABLED) || defined(HAL_HCD_MODULE_ENABLED) || defined(HAL_USB_DRD_MODULE_ENABLED) || defined(HAL_USB_XHCI_MODULE_ENABLED) || defined(HAL_EHCI_MODULE_ENABLED) || defined(HAL_OHCI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup USB_PHY
 *  @{
 */

#ifndef _HAL_USB_PHY_H_
#define _HAL_USB_PHY_H_

#include "hal_def.h"

/***************************** Structure Definition **************************/
/***************************** Function Declare ******************************/
/** @defgroup USB_PHY_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_USB_HsPhyInit(uint8_t id);
HAL_Status HAL_USB_SsPhyInit(uint8_t id);
HAL_Status HAL_USB_PhySuspend(uint8_t id);
HAL_Status HAL_USB_PhyResume(uint8_t id);
HAL_Status HAL_USB_PhyBvalidIrqEnable(void);
HAL_Status HAL_USB_PhyBvalidIrqDisable(void);
uint8_t HAL_USB_PhyBvalidIrqRiseStatus(void);
uint8_t HAL_USB_PhyBvalidIrqFallStatus(void);
void HAL_USB_PhyBvalidIrqRiseStatusClear(void);
void HAL_USB_PhyBvalidIrqFallStatusClear(void);
/** @} */

#endif /* _HAL_USB_PHY_H_ */

/** @} */

/** @} */

#endif /* HAL_PCD_MODULE_ENABLED || HAL_HCD_MODULE_ENABLED || HAL_USB_DRD_MODULE_ENABLED || HAL_USB_XHCI_MODULE_ENABLED || HAL_EHCI_MODULE_ENABLED || HAL_OHCI_MODULE_ENABLED */
