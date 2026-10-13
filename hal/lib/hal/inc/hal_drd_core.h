/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_USB_DRD_MODULE_ENABLED) || defined(HAL_USB_XHCI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup USB_DRD_CORE
 *  @{
 */

#ifndef  _HAL_USB_DRD_CORE_H_
#define  _HAL_USB_DRD_CORE_H_

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup USB_DRD_CORE_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/* Global constants */
#if defined(SOC_RK1820)
#define DRD_ADRHI_GRF      (PHP_GRF_BASE + PHP_GRF_USB3_ADDR_H_OFFSET)
#define DRD_AR_ADDR_H_MASK PHP_GRF_USB3_ADDR_H_USB3_AR_ADDR_H_MASK
#define DRD_AW_ADDR_H_MASK PHP_GRF_USB3_ADDR_H_USB3_AW_ADDR_H_MASK
#endif

#ifdef CACHE_LINE_SIZE
#define DRD_DMA_ALIGN_SIZE CACHE_LINE_SIZE
#else
#define DRD_DMA_ALIGN_SIZE 64
#endif

#define DRD_BOUNCE_SIZE      1024
#define DRD_EP0_SETUP_SIZE   512
#define DRD_ENDPOINTS_NUM    32
#define DRD_ISOC_MAX_RETRIES 50

#define DRD_EVENT_SIZE         4
#define DRD_EVENT_MAX_NUM      64
#define DRD_EVENT_BUFFERS_SIZE (DRD_EVENT_SIZE * DRD_EVENT_MAX_NUM)
#define DRD_EVENT_TYPE_MASK    0xfe

#define DRD_EVENT_TYPE_DEV    0
#define DRD_EVENT_TYPE_CARKIT 3
#define DRD_EVENT_TYPE_I2C    4

#define DRD_DEVICE_EVENT_DISCONNECT         0
#define DRD_DEVICE_EVENT_RESET              1
#define DRD_DEVICE_EVENT_CONNECT_DONE       2
#define DRD_DEVICE_EVENT_LINK_STATUS_CHANGE 3
#define DRD_DEVICE_EVENT_WAKEUP             4
#define DRD_DEVICE_EVENT_HIBER_REQ          5
#define DRD_DEVICE_EVENT_SUSPEND            6
#define DRD_DEVICE_EVENT_SOF                7
#define DRD_DEVICE_EVENT_ERRATIC_ERROR      9
#define DRD_DEVICE_EVENT_CMD_CMPL           10
#define DRD_DEVICE_EVENT_OVERFLOW           11

#define DRD_GEVNTCOUNT_MASK 0xfffc
#define DRD_GSNPSID_MASK    0xffff0000

#define DRD_GSNPSREV_MASK 0xffff
#define DRD_GSNPS_ID(p)   (((p) & DRD_GSNPSID_MASK) >> 16)

/* DRD registers memory space boundaries */
#define DRD_XHCI_REGS_START    0x0
#define DRD_XHCI_REGS_END      0x7fff
#define DRD_GLOBALS_REGS_START 0xc100
#define DRD_GLOBALS_REGS_END   0xc6ff
#define DRD_DEVICE_REGS_START  0xc700
#define DRD_DEVICE_REGS_END    0xcbff
#define DRD_OTG_REGS_START     0xcc00
#define DRD_OTG_REGS_END       0xccff

/* Global Registers */
#define DRD_GSBUSCFG0     0xc100
#define DRD_GSBUSCFG1     0xc104
#define DRD_GTXTHRCFG     0xc108
#define DRD_GRXTHRCFG     0xc10c
#define DRD_GCTL          0xc110
#define DRD_GEVTEN        0xc114
#define DRD_GSTS          0xc118
#define DRD_GUCTL1        0xc11c
#define DRD_GSNPSID       0xc120
#define DRD_GGPIO         0xc124
#define DRD_GUID          0xc128
#define DRD_GUCTL         0xc12c
#define DRD_GBUSERRADDR0  0xc130
#define DRD_GBUSERRADDR1  0xc134
#define DRD_GPRTBIMAP0    0xc138
#define DRD_GPRTBIMAP1    0xc13c
#define DRD_GHWPARAMS0    0xc140
#define DRD_GHWPARAMS1    0xc144
#define DRD_GHWPARAMS2    0xc148
#define DRD_GHWPARAMS3    0xc14c
#define DRD_GHWPARAMS4    0xc150
#define DRD_GHWPARAMS5    0xc154
#define DRD_GHWPARAMS6    0xc158
#define DRD_GHWPARAMS7    0xc15c
#define DRD_GDBGFIFOSPACE 0xc160
#define DRD_GDBGLTSSM     0xc164
#define DRD_GPRTBIMAP_HS0 0xc180
#define DRD_GPRTBIMAP_HS1 0xc184
#define DRD_GPRTBIMAP_FS0 0xc188
#define DRD_GPRTBIMAP_FS1 0xc18c

#define DRD_VER_NUMBER 0xc1a0
#define DRD_VER_TYPE   0xc1a4

#define DRD_GUSB2PHYCFG0 0xc200
#define DRD_GUSB2I2CCTL0 0xc240

#define DRD_GUSB2PHYACC0 0xc280

#define DRD_GUSB3PIPECTL0 0xc2c0

#define DRD_GTXFIFOSIZ(n) (0xc300 + (n * 0x04))
#define DRD_GRXFIFOSIZ(n) (0xc380 + (n * 0x04))

#define DRD_GEVNTADRLO(n) (0xc400 + (n * 0x10))
#define DRD_GEVNTADRHI(n) (0xc404 + (n * 0x10))
#define DRD_GEVNTSIZ(n)   (0xc408 + (n * 0x10))
#define DRD_GEVNTCOUNT(n) (0xc40c + (n * 0x10))

#define DRD_GHWPARAMS8 0xc600

/* Device Registers */
#define DRD_DCFG       0xc700
#define DRD_DCTL       0xc704
#define DRD_DEVTEN     0xc708
#define DRD_DSTS       0xc70c
#define DRD_DGCMDPAR   0xc710
#define DRD_DGCMD      0xc714
#define DRD_DALEPENA   0xc720
#define DRD_DEPCMDPAR2 0xc800
#define DRD_DEPCMDPAR1 0xc804
#define DRD_DEPCMDPAR0 0xc808
#define DRD_DEPCMD     0xc80c

/* OTG Registers */
#define DRD_OCFG   0xcc00
#define DRD_OCTL   0xcc04
#define DRD_OEVT   0xcc08
#define DRD_OEVTEN 0xcc0c
#define DRD_OSTS   0xcc10

/* Bit fields */

/* Global Configuration Register */
#define DRD_GCTL_PWRDNSCALE(n) ((n) << 19)
#define DRD_GCTL_U2RSTECN      HAL_BIT(16)
#define DRD_GCTL_RAMCLKSEL(x)  (((x) & DRD_GCTL_CLK_MASK) << 6)
#define DRD_GCTL_CLK_BUS       (0)
#define DRD_GCTL_CLK_PIPE      (1)
#define DRD_GCTL_CLK_PIPEHALF  (2)
#define DRD_GCTL_CLK_MASK      (3)

#define DRD_GCTL_PRTCAP(n)     (((n) & (3 << 12)) >> 12)
#define DRD_GCTL_PRTCAPDIR(n)  ((n) << 12)
#define DRD_GCTL_PRTCAP_HOST   1
#define DRD_GCTL_PRTCAP_DEVICE 2
#define DRD_GCTL_PRTCAP_OTG    3

#define DRD_GCTL_CORESOFTRESET    HAL_BIT(11)
#define DRD_GCTL_SOFITPSYNC       HAL_BIT(10)
#define DRD_GCTL_SCALEDOWN(n)     ((n) << 4)
#define DRD_GCTL_SCALEDOWN_MASK   DRD_GCTL_SCALEDOWN(3)
#define DRD_GCTL_DISSCRAMBLE      HAL_BIT(3)
#define DRD_GCTL_U2EXIT_LFPS      HAL_BIT(2)
#define DRD_GCTL_GBLHIBERNATIONEN HAL_BIT(1)
#define DRD_GCTL_DSBLCLKGTNG      HAL_BIT(0)

/* Global User Control 1 Register */
#define DRD_GUCTL1_TX_IPGAP_LINECHECK_DIS      HAL_BIT(28)
#define DRD_GUCTL1_DEV_FORCE_20_CLK_FOR_30_CLK HAL_BIT(26)
#define DRD_GUCTL1_PARKMODE_DISABLE_SS         HAL_BIT(17)
#define DRD_GUCTL1_PARKMODE_DISABLE_HS         HAL_BIT(16)

/* Global USB2 PHY Configuration Register */
#define DRD_GUSB2PHYCFG_PHYSOFTRST        HAL_BIT(31)
#define DRD_GUSB2PHYCFG_U2_FREECLK_EXISTS HAL_BIT(30)
#define DRD_GUSB2PHYCFG_ENBLSLPM          HAL_BIT(8)
#define DRD_GUSB2PHYCFG_SUSPHY            HAL_BIT(6)
#define DRD_GUSB2PHYCFG_PHYIF_8BIT        (0 << 3)
#define DRD_GUSB2PHYCFG_PHYIF_16BIT       HAL_BIT(3)
#define DRD_GUSB2PHYCFG_PHYIF(n)          ((n) << 3)
#define DRD_GUSB2PHYCFG_PHYIF_MASK        DRD_GUSB2PHYCFG_PHYIF(1)
#define DRD_GUSB2PHYCFG_USBTRDTIM(n)      ((n) << 10)
#define DRD_GUSB2PHYCFG_USBTRDTIM_MASK    DRD_GUSB2PHYCFG_USBTRDTIM(0xf)
#define USBTRDTIM_UTMI_8_BIT              9
#define USBTRDTIM_UTMI_16_BIT             5
#define UTMI_PHYIF_16_BIT                 1
#define UTMI_PHYIF_8_BIT                  0

/* Global USB3 PIPE Control Register */
#define DRD_GUSB3PIPECTL_PHYSOFTRST    HAL_BIT(31)
#define DRD_GUSB3PIPECTL_U2SSINP3OK    HAL_BIT(29)
#define DRD_GUSB3PIPECTL_DISRXDETINP3  HAL_BIT(28)
#define DRD_GUSB3PIPECTL_REQP1P2P3     HAL_BIT(24)
#define DRD_GUSB3PIPECTL_DEP1P2P3(n)   ((n) << 19)
#define DRD_GUSB3PIPECTL_DEP1P2P3_MASK DRD_GUSB3PIPECTL_DEP1P2P3(7)
#define DRD_GUSB3PIPECTL_DEP1P2P3_EN   DRD_GUSB3PIPECTL_DEP1P2P3(1)
#define DRD_GUSB3PIPECTL_DEPOCHANGE    HAL_BIT(18)
#define DRD_GUSB3PIPECTL_SUSPHY        HAL_BIT(17)
#define DRD_GUSB3PIPECTL_LFPSFILT      HAL_BIT(9)
#define DRD_GUSB3PIPECTL_RX_DETOPOLL   HAL_BIT(8)
#define DRD_GUSB3PIPECTL_TX_DEEPH_MASK DRD_GUSB3PIPECTL_TX_DEEPH(3)
#define DRD_GUSB3PIPECTL_TX_DEEPH(n)   ((n) << 1)

/* Global TX Fifo Size Register */
#define DRD_GTXFIFOSIZ_TXFDEF(n)    ((n) & 0xffff)
#define DRD_GTXFIFOSIZ_TXFSTADDR(n) ((n) & 0xffff0000)

/* Global Event Size Registers */
#define DRD_GEVNTSIZ_INTMASK HAL_BIT(31)
#define DRD_GEVNTSIZ_SIZE(n) ((n) & 0xffff)

/* Global HWPARAMS1 Register */
#define DRD_GHWPARAMS1_EN_PWROPT(n)  (((n) & (3 << 24)) >> 24)
#define DRD_GHWPARAMS1_EN_PWROPT_NO  0
#define DRD_GHWPARAMS1_EN_PWROPT_CLK 1
#define DRD_GHWPARAMS1_EN_PWROPT_HIB 2
#define DRD_GHWPARAMS1_PWROPT(n)     ((n) << 24)
#define DRD_GHWPARAMS1_PWROPT_MASK   DRD_GHWPARAMS1_PWROPT(3)

/* Global HWPARAMS3 Register */
#define DRD_GHWPARAMS3_SSPHY_IFC(n)        ((n) & 3)
#define DRD_GHWPARAMS3_SSPHY_IFC_DIS       0
#define DRD_GHWPARAMS3_SSPHY_IFC_ENA       1
#define DRD_GHWPARAMS3_HSPHY_IFC(n)        (((n) & (3 << 2)) >> 2)
#define DRD_GHWPARAMS3_HSPHY_IFC_DIS       0
#define DRD_GHWPARAMS3_HSPHY_IFC_UTMI      1
#define DRD_GHWPARAMS3_HSPHY_IFC_ULPI      2
#define DRD_GHWPARAMS3_HSPHY_IFC_UTMI_ULPI 3
#define DRD_GHWPARAMS3_FSPHY_IFC(n)        (((n) & (3 << 4)) >> 4)
#define DRD_GHWPARAMS3_FSPHY_IFC_DIS       0
#define DRD_GHWPARAMS3_FSPHY_IFC_ENA       1

/* Global HWPARAMS4 Register */
#define DRD_GHWPARAMS4_HIBER_SCRATCHBUFS(n) (((n) & (0x0f << 13)) >> 13)
#define DRD_MAX_HIBER_SCRATCHBUFS           15

/* Global HWPARAMS6 Register */
#define DRD_GHWPARAMS6_EN_FPGA HAL_BIT(7)

/* Device Configuration Register */
#define DRD_DCFG_DEVADDR(addr) ((addr) << 3)
#define DRD_DCFG_DEVADDR_MASK  DRD_DCFG_DEVADDR(0x7f)

#define DRD_DCFG_SPEED_MASK (7 << 0)
#define DRD_DCFG_SUPERSPEED (4 << 0)
#define DRD_DCFG_HIGHSPEED  (0 << 0)
#define DRD_DCFG_FULLSPEED  HAL_BIT(0)

#define DRD_DCFG_LPM_CAP HAL_BIT(22)

/* Device Control Register */
#define DRD_DCTL_RUN_STOP HAL_BIT(31)
#define DRD_DCTL_CSFTRST  HAL_BIT(30)
#define DRD_DCTL_LSFTRST  HAL_BIT(29)

#define DRD_DCTL_HIRD_THRES_MASK (0x1f << 24)
#define DRD_DCTL_HIRD_THRES(n)   ((n) << 24)

#define DRD_DCTL_APPL1RES HAL_BIT(23)

/* These apply for core versions 1.87a and earlier */
#define DRD_DCTL_TRGTULST_MASK     (0x0f << 17)
#define DRD_DCTL_TRGTULST(n)       ((n) << 17)
#define DRD_DCTL_TRGTULST_U2       (DRD_DCTL_TRGTULST(2))
#define DRD_DCTL_TRGTULST_U3       (DRD_DCTL_TRGTULST(3))
#define DRD_DCTL_TRGTULST_SS_DIS   (DRD_DCTL_TRGTULST(4))
#define DRD_DCTL_TRGTULST_RX_DET   (DRD_DCTL_TRGTULST(5))
#define DRD_DCTL_TRGTULST_SS_INACT (DRD_DCTL_TRGTULST(6))

/* These apply for core versions 1.94a and later */
#define DRD_DCTL_LPM_ERRATA_MASK DRD_DCTL_LPM_ERRATA(0xf)
#define DRD_DCTL_LPM_ERRATA(n)   ((n) << 20)

#define DRD_DCTL_KEEP_CONNECT HAL_BIT(19)
#define DRD_DCTL_L1_HIBER_EN  HAL_BIT(18)
#define DRD_DCTL_CRS          HAL_BIT(17)
#define DRD_DCTL_CSS          HAL_BIT(16)

#define DRD_DCTL_INITU2ENA    HAL_BIT(12)
#define DRD_DCTL_ACCEPTU2ENA  HAL_BIT(11)
#define DRD_DCTL_INITU1ENA    HAL_BIT(10)
#define DRD_DCTL_ACCEPTU1ENA  HAL_BIT(9)
#define DRD_DCTL_TSTCTRL_MASK (0xf << 1)

#define DRD_DCTL_ULSTCHNGREQ_MASK (0x0f << 5)
#define DRD_DCTL_ULSTCHNGREQ(n)   (((n) << 5) & DRD_DCTL_ULSTCHNGREQ_MASK)

#define DRD_DCTL_ULSTCHNG_NO_ACTION   (DRD_DCTL_ULSTCHNGREQ(0))
#define DRD_DCTL_ULSTCHNG_SS_DISABLED (DRD_DCTL_ULSTCHNGREQ(4))
#define DRD_DCTL_ULSTCHNG_RX_DETECT   (DRD_DCTL_ULSTCHNGREQ(5))
#define DRD_DCTL_ULSTCHNG_SS_INACTIVE (DRD_DCTL_ULSTCHNGREQ(6))
#define DRD_DCTL_ULSTCHNG_RECOVERY    (DRD_DCTL_ULSTCHNGREQ(8))
#define DRD_DCTL_ULSTCHNG_COMPLIANCE  (DRD_DCTL_ULSTCHNGREQ(10))
#define DRD_DCTL_ULSTCHNG_LOOPBACK    (DRD_DCTL_ULSTCHNGREQ(11))

/* Device Event Enable Register */
#define DRD_DEVTEN_VNDRDEVTSTRCVEDEN   HAL_BIT(12)
#define DRD_DEVTEN_EVNTOVERFLOWEN      HAL_BIT(11)
#define DRD_DEVTEN_CMDCMPLTEN          HAL_BIT(10)
#define DRD_DEVTEN_ERRTICERREN         HAL_BIT(9)
#define DRD_DEVTEN_SOFEN               HAL_BIT(7)
#define DRD_DEVTEN_U3L2L1SUSPEN        HAL_BIT(6)
#define DRD_DEVTEN_HIBERNATIONREQEVTEN HAL_BIT(5)
#define DRD_DEVTEN_WKUPEVTEN           HAL_BIT(4)
#define DRD_DEVTEN_ULSTCNGEN           HAL_BIT(3)
#define DRD_DEVTEN_CONNECTDONEEN       HAL_BIT(2)
#define DRD_DEVTEN_USBRSTEN            HAL_BIT(1)
#define DRD_DEVTEN_DISCONNEVTEN        HAL_BIT(0)

/* Device Status Register */
#define DRD_DSTS_DCNRD HAL_BIT(29)

/* This applies for core versions 1.87a and earlier */
#define DRD_DSTS_PWRUPREQ HAL_BIT(24)

/* These apply for core versions 1.94a and later */
#define DRD_DSTS_RSS HAL_BIT(25)
#define DRD_DSTS_SSS HAL_BIT(24)

#define DRD_DSTS_COREIDLE   HAL_BIT(23)
#define DRD_DSTS_DEVCTRLHLT HAL_BIT(22)

#define DRD_DSTS_USBLNKST_MASK (0x0f << 18)
#define DRD_DSTS_USBLNKST(n)   (((n) & DRD_DSTS_USBLNKST_MASK) >> 18)

#define DRD_DSTS_RXFIFOEMPTY HAL_BIT(17)

#define DRD_DSTS_SOFFN_MASK (0x3fff << 3)
#define DRD_DSTS_SOFFN(n)   (((n) & DRD_DSTS_SOFFN_MASK) >> 3)

#define DRD_DSTS_CONNECTSPD (7 << 0)

#define DRD_DSTS_SUPERSPEED (4 << 0)
#define DRD_DSTS_HIGHSPEED  (0 << 0)
#define DRD_DSTS_FULLSPEED  HAL_BIT(0)

/* Device Generic Command Register */
#define DRD_DGCMD_SET_LMP          0x01
#define DRD_DGCMD_SET_PERIODIC_PAR 0x02
#define DRD_DGCMD_XMIT_FUNCTION    0x03

/* These apply for core versions 1.94a and later */
#define DRD_DGCMD_SET_SCRATCHPAD_ADDR_LO 0x04
#define DRD_DGCMD_SET_SCRATCHPAD_ADDR_HI 0x05

#define DRD_DGCMD_SELECTED_FIFO_FLUSH  0x09
#define DRD_DGCMD_ALL_FIFO_FLUSH       0x0a
#define DRD_DGCMD_SET_ENDPOINT_NRDY    0x0c
#define DRD_DGCMD_RUN_SOC_BUS_LOOPBACK 0x10

#define DRD_DGCMD_STATUS(n) (((n) >> 15) & 1)
#define DRD_DGCMD_CMDACT    HAL_BIT(10)
#define DRD_DGCMD_CMDIOC    HAL_BIT(8)

/* Device Generic Command Parameter Register */
#define DRD_DGCMDPAR_FORCE_LINKPM_ACCEPT HAL_BIT(0)
#define DRD_DGCMDPAR_FIFO_NUM(n)         ((n) << 0)
#define DRD_DGCMDPAR_RX_FIFO             (0 << 5)
#define DRD_DGCMDPAR_TX_FIFO             HAL_BIT(5)
#define DRD_DGCMDPAR_LOOPBACK_DIS        (0 << 0)
#define DRD_DGCMDPAR_LOOPBACK_ENA        HAL_BIT(0)

/* Device Endpoint Command Register */
#define DRD_DEPCMD_PARAM_SHIFT    16
#define DRD_DEPCMD_PARAM(x)       ((x) << DRD_DEPCMD_PARAM_SHIFT)
#define DRD_DEPCMD_GET_RSC_IDX(x) (((x) >> DRD_DEPCMD_PARAM_SHIFT) & 0x7f)
#define DRD_DEPCMD_STATUS(x)      (((x) >> 12) & 0x0f)
#define DRD_DEPCMD_HIPRI_FORCERM  HAL_BIT(11)
#define DRD_DEPCMD_CLEARPENDIN    HAL_BIT(11)
#define DRD_DEPCMD_CMDACT         HAL_BIT(10)
#define DRD_DEPCMD_CMDIOC         HAL_BIT(8)

#define DRD_DEPCMD_DEPSTARTCFG    (0x09 << 0)
#define DRD_DEPCMD_ENDTRANSFER    (0x08 << 0)
#define DRD_DEPCMD_UPDATETRANSFER (0x07 << 0)
#define DRD_DEPCMD_STARTTRANSFER  (0x06 << 0)
#define DRD_DEPCMD_CLEARSTALL     (0x05 << 0)
#define DRD_DEPCMD_SETSTALL       (0x04 << 0)
/* This applies for core versions 1.90a and earlier */
#define DRD_DEPCMD_GETSEQNUMBER (0x03 << 0)
/* This applies for core versions 1.94a and later */
#define DRD_DEPCMD_GETEPSTATE        (0x03 << 0)
#define DRD_DEPCMD_SETTRANSFRESOURCE (0x02 << 0)
#define DRD_DEPCMD_SETEPCONFIG       (0x01 << 0)

#define DRD_DEPCMD_CMD(x) ((x) & 0xf)

/* The EP number goes 0..31 so ep0 is always out and ep1 is always in */
#define DRD_DALEPENA_EP(n) HAL_BIT(n)

#define DRD_DEPCMD_TYPE_CONTROL 0
#define DRD_DEPCMD_TYPE_ISOC    1
#define DRD_DEPCMD_TYPE_BULK    2
#define DRD_DEPCMD_TYPE_INTR    3

#define USB3DRD_VER_NUMBER        (*(__IO uint32_t *)((uintptr_t)pUSB3DRD - DRD_GLOBALS_REGS_START + DRD_VER_NUMBER))
#define USB3DRD_GUSB2PHYCFG(n)    (*(__IO uint32_t *)((uintptr_t)pUSB3DRD - DRD_GLOBALS_REGS_START + DRD_GUSB2PHYCFG0 + (n * 0x04)))
#define USB3DRD_GUSB3PIPECTL(n)   (*(__IO uint32_t *)((uintptr_t)pUSB3DRD - DRD_GLOBALS_REGS_START + DRD_GUSB3PIPECTL0 + (n * 0x04)))
#define USB3DRD_DRD_DEPCMDPAR0(n) (*(__IO uint32_t *)((uintptr_t)pUSB3DRD - DRD_GLOBALS_REGS_START + DRD_DEPCMDPAR0 + (n * 0x10)))
#define USB3DRD_DRD_DEPCMDPAR1(n) (*(__IO uint32_t *)((uintptr_t)pUSB3DRD - DRD_GLOBALS_REGS_START + DRD_DEPCMDPAR1 + (n * 0x10)))
#define USB3DRD_DRD_DEPCMDPAR2(n) (*(__IO uint32_t *)((uintptr_t)pUSB3DRD - DRD_GLOBALS_REGS_START + DRD_DEPCMDPAR2 + (n * 0x10)))
#define USB3DRD_DRD_DEPCMD(n)     (*(__IO uint32_t *)((uintptr_t)pUSB3DRD - DRD_GLOBALS_REGS_START + DRD_DEPCMD + (n * 0x10)))

typedef enum {
    DRD_USB_SPEED_UNKNOWN = 0,
    DRD_USB_SPEED_LOW,
    DRD_USB_SPEED_FULL,
    DRD_USB_SPEED_HIGH,
    DRD_USB_SPEED_WIRELESS,
    DRD_USB_SPEED_SUPER,
} eUSB_speed;

typedef enum {
    USB_DR_MODE_UNKNOWN,
    USB_DR_MODE_HOST,
    USB_DR_MODE_PERIPHERAL,
    USB_DR_MODE_OTG,
} eUSB_drMode;

typedef enum {
    USBPHY_INTERFACE_MODE_UNKNOWN,
    USBPHY_INTERFACE_MODE_UTMI,
    USBPHY_INTERFACE_MODE_UTMIW,
} eUSBPHY_intfMode;

typedef enum {
    DRD_LINK_STATE_U0       = 0x00,
    DRD_LINK_STATE_U1       = 0x01,
    DRD_LINK_STATE_U2       = 0x02,
    DRD_LINK_STATE_U3       = 0x03,
    DRD_LINK_STATE_SS_DIS   = 0x04,
    DRD_LINK_STATE_RX_DET   = 0x05,
    DRD_LINK_STATE_SS_INACT = 0x06,
    DRD_LINK_STATE_POLL     = 0x07,
    DRD_LINK_STATE_RECOV    = 0x08,
    DRD_LINK_STATE_HRESET   = 0x09,
    DRD_LINK_STATE_CMPLY    = 0x0a,
    DRD_LINK_STATE_LPBK     = 0x0b,
    DRD_LINK_STATE_RESET    = 0x0e,
    DRD_LINK_STATE_RESUME   = 0x0f,
    DRD_LINK_STATE_MASK     = 0x0f,
} eDRD_linkState;

/* TRB Length, PCM and Status */
#define DRD_TRB_SIZE_MASK      (0x00ffffff)
#define DRD_TRB_SIZE_LENGTH(n) ((n) & DRD_TRB_SIZE_MASK)
#define DRD_TRB_SIZE_PCM1(n)   (((n) & 0x03) << 24)
#define DRD_TRB_SIZE_TRBSTS(n) (((n) & (0x0f << 28)) >> 28)

#define DRD_TRBSTS_OK            0
#define DRD_TRBSTS_MISSED_ISOC   1
#define DRD_TRBSTS_SETUP_PENDING 2
#define DRD_TRB_STS_XFER_IN_PROG 4

/* TRB Control */
#define DRD_TRB_CTRL_HWO             HAL_BIT(0)
#define DRD_TRB_CTRL_LST             HAL_BIT(1)
#define DRD_TRB_CTRL_CHN             HAL_BIT(2)
#define DRD_TRB_CTRL_CSP             HAL_BIT(3)
#define DRD_TRB_CTRL_TRBCTL(n)       (((n) & 0x3f) << 4)
#define DRD_TRB_CTRL_ISP_IMI         HAL_BIT(10)
#define DRD_TRB_CTRL_IOC             HAL_BIT(11)
#define DRD_TRB_CTRL_SID_SOFN(n)     (((n) & 0xffff) << 14)
#define DRD_TRB_CTRL_GET_SID_SOFN(n) (((n) & (0xffff << 14)) >> 14)

#define DRD_TRBCTL_NORMAL            DRD_TRB_CTRL_TRBCTL(1)
#define DRD_TRBCTL_CONTROL_SETUP     DRD_TRB_CTRL_TRBCTL(2)
#define DRD_TRBCTL_CONTROL_STATUS2   DRD_TRB_CTRL_TRBCTL(3)
#define DRD_TRBCTL_CONTROL_STATUS3   DRD_TRB_CTRL_TRBCTL(4)
#define DRD_TRBCTL_CONTROL_DATA      DRD_TRB_CTRL_TRBCTL(5)
#define DRD_TRBCTL_ISOCHRONOUS_FIRST DRD_TRB_CTRL_TRBCTL(6)
#define DRD_TRBCTL_ISOCHRONOUS       DRD_TRB_CTRL_TRBCTL(7)
#define DRD_TRBCTL_LINK_TRB          DRD_TRB_CTRL_TRBCTL(8)

/* HWPARAMS0 */
#define DRD_MODE(n) ((n) & 0x7)

#define DRD_MDWIDTH(n) (((n) & 0xff00) >> 8)

/* HWPARAMS1 */
#define DRD_NUM_INT(n) (((n) & (0x3f << 15)) >> 15)

/* HWPARAMS3 */
#define DRD_NUM_IN_EPS_MASK (0x1f << 18)
#define DRD_NUM_EPS_MASK    (0x3f << 12)
#define DRD_NUM_EPS(p)      (((p)->hwparams3 & \
                                (DRD_NUM_EPS_MASK)) >> 12)
#define DRD_NUM_IN_EPS(p)   (((p)->hwparams3 & \
                                (DRD_NUM_IN_EPS_MASK)) >> 18)

/* HWPARAMS7 */
#define DRD_RAM1_DEPTH(n) ((n) & 0xffff)

/* Endpoints */
#define DRD_USB_ENDPOINT_NUMBER_MASK 0x0f
#define DRD_USB_ENDPOINT_DIR_MASK    0x80

#define DRD_USB_ENDPOINT_XFERTYPE_MASK 0x03
#define DRD_USB_ENDPOINT_XFER_CONTROL  0
#define DRD_USB_ENDPOINT_XFER_ISOC     1
#define DRD_USB_ENDPOINT_XFER_BULK     2
#define DRD_USB_ENDPOINT_XFER_INT      3

#define DRD_USB_ENDPOINT_MAXP_MASK 0x07ff
#define DRD_USB_EP_MAXP_MULT_SHIFT 11
#define DRD_USB_EP_MAXP_MULT_MASK  (3 << DRD_USB_EP_MAXP_MULT_SHIFT)
#define DRD_USB_EP_MAXP_MULT(m) \
        (((m) & DRD_USB_EP_MAXP_MULT_MASK) >> DRD_USB_EP_MAXP_MULT_SHIFT)

/* CONTROL REQUEST SUPPORT */
#define DRD_USB_DIR_OUT 0
#define DRD_USB_DIR_IN  0x80

#define DRD_USB_TYPE_MASK     (0x03 << 5)
#define DRD_USB_TYPE_STANDARD (0x00 << 5)
#define DRD_USB_TYPE_CLASS    (0x01 << 5)
#define DRD_USB_TYPE_VENDOR   (0x02 << 5)
#define DRD_USB_TYPE_RESERVED (0x03 << 5)

#define DRD_USB_REQ_SET_SEL         0x30
#define DRD_USB_REQ_SET_ISOCH_DELAY 0x31

#define DRD_IP_IS(_ip) \
    (pCFG->ip == _ip##_IP)

#define DRD_VER_IS(_ip, _ver) \
    (DRD_IP_IS(_ip) && pCFG->revision == _ip##_REVISION_##_ver)

#define DRD_VER_IS_PRIOR(_ip, _ver) \
    (DRD_IP_IS(_ip) && pCFG->revision < _ip##_REVISION_##_ver)

#define DRD_VER_IS_WITHIN(_ip, _from, _to)       \
    (DRD_IP_IS(_ip) &&                           \
     pCFG->revision >= _ip##_REVISION_##_from && \
     (!(_ip##_REVISION_##_to) ||                 \
      pCFG->revision <= _ip##_REVISION_##_to))

/***************************** Structure Definition **************************/
struct DRD_HWPARAMS {
    uint32_t hwparams0;  /*!< DRD Hardware Parameter0 */
    uint32_t hwparams1;  /*!< DRD Hardware Parameter1 */
    uint32_t hwparams2;  /*!< DRD Hardware Parameter2 */
    uint32_t hwparams3;  /*!< DRD Hardware Parameter3 */
    uint32_t hwparams4;  /*!< DRD Hardware Parameter4 */
    uint32_t hwparams5;  /*!< DRD Hardware Parameter5 */
    uint32_t hwparams6;  /*!< DRD Hardware Parameter6 */
    uint32_t hwparams7;  /*!< DRD Hardware Parameter7 */
    uint32_t hwparams8;  /*!< DRD Hardware Parameter8 */
};

/** USB Initialization Structure definition */
struct USB_DRD_CFG {
    eUSB_drMode drMode;         /*!< USB Role Mode             */
    eUSBPHY_intfMode hsphyMode; /*!< USB HS PHY Interface Mode */
    uint8_t maximumSpeed;       /*!< Maximum Speed             */
    uint8_t hsphyId;            /*!< USB HS PHY ID             */
    uint8_t ssphyId;            /*!< USB SS PHY ID             */

    uint32_t ip;                /*!< DRD IP verison            */

#define DRD_IP  0x5533
#define DRD1_IP 0x3331
#define DRD2_IP 0x3332

    uint32_t revision;          /*!< DRD revision              */

#define DRD_REVISION_ANY  0x0
#define DRD_REVISION_173A 0x5533173a
#define DRD_REVISION_175A 0x5533175a
#define DRD_REVISION_180A 0x5533180a
#define DRD_REVISION_183A 0x5533183a
#define DRD_REVISION_185A 0x5533185a
#define DRD_REVISION_187A 0x5533187a
#define DRD_REVISION_188A 0x5533188a
#define DRD_REVISION_190A 0x5533190a
#define DRD_REVISION_194A 0x5533194a
#define DRD_REVISION_200A 0x5533200a
#define DRD_REVISION_202A 0x5533202a
#define DRD_REVISION_210A 0x5533210a
#define DRD_REVISION_220A 0x5533220a
#define DRD_REVISION_230A 0x5533230a
#define DRD_REVISION_240A 0x5533240a
#define DRD_REVISION_250A 0x5533250a
#define DRD_REVISION_260A 0x5533260a
#define DRD_REVISION_270A 0x5533270a
#define DRD_REVISION_280A 0x5533280a
#define DRD_REVISION_290A 0x5533290a
#define DRD_REVISION_300A 0x5533300a
#define DRD_REVISION_310A 0x5533310a
#define DRD_REVISION_320A 0x5533320a
#define DRD_REVISION_330A 0x5533330a

#define DRD1_REVISION_ANY  0x0
#define DRD1_REVISION_110A 0x3131302a
#define DRD1_REVISION_120A 0x3132302a
#define DRD1_REVISION_160A 0x3136302a
#define DRD1_REVISION_170A 0x3137302a
#define DRD1_REVISION_180A 0x3138302a
#define DRD1_REVISION_190A 0x3139302a

#define DRD2_REVISION_ANY  0x0
#define DRD2_REVISION_100A 0x3130302a

    struct DRD_HWPARAMS hwparams;          /*!< DRD Hardware Parameters       */

    uint8_t lpmNyetThreshold;               /*!< LPM Nyet Threshold             */
    uint8_t hirdThreshold;                  /*!< Hird Threshold                 */

    unsigned hasAwidth64 : 1;               /*!< Enable Address 64-bit          */
    unsigned hasHibernation : 1;            /*!< Enable Hibernation             */
    unsigned hasLpmErratum : 1;             /*!< Enable LPM Erratum             */
    unsigned isUtmiL1Suspend : 1;           /*!< Enable Utmi L1 Suspend         */
    unsigned isSelfpowered : 1;             /*!< Enable Self Powered            */
    unsigned needsFifoResize : 1;           /*!< Enable TxFIFO Resize           */
    unsigned delP1p2p3Quirk : 1;            /*!< Enable Delay P1P2P3            */
    unsigned delPhyPowerChgQuirk : 1;       /*!< Enalbe Delay PHY Power change  */
    unsigned disDelPhyPowerChgQuirk : 1;    /*!< Disable Delay PHY Power change */
    unsigned disEnblslpmQuirk : 1;          /*!< Disable USB2 PHY Sleep         */
    unsigned disRxdetInp3Quirk : 1;         /*!< Disable Rx Detect in P3        */
    unsigned disTxIpGapLinecheckQuirk : 1;  /*!< Disable Tx IPGAP Line Check    */
    unsigned disU1u2Quirk : 1;              /*!< Disable U1U2 state             */
    unsigned disU2SusphyQuirk : 1;          /*!< Disable USB2 PHY Suspend       */
    unsigned disU2FreeclkExistsQuirk : 1;   /*!< Disable USB2 Free clk Exist    */
    unsigned disU3SusphyQuirk : 1;          /*!< Disable USB3 PHY Suspend       */
    unsigned lfpsFilterQuirk : 1;           /*!< Set LFPS Filter                */
    unsigned parkmodeDisableHsQuirk : 1;    /*!< Disable HS Park Mode           */
    unsigned parkmodeDisableSsQuirk : 1;    /*!< Disable SS Park Mode           */
    unsigned reqP1p2p3Quirk : 1;            /*!< Enable Request P1P2P3          */
    unsigned rxDetectPollQuirk : 1;         /*!< Disable RxDet delay to Polling */
    unsigned txDeEmphasisQuirk : 1;         /*!< Enable to Set Tx DeEmphasis    */
    unsigned txDeEmphasis : 2;              /*!< Tx DeEmphasis value            */
    unsigned u2exitLfpsQuirk : 1;           /*!< Wait for LPFS befor U2 Exit    */
    unsigned u2ssInp3Quirk : 1;             /*!< Enable to P3 during SSInactive */
    unsigned u2lpmDisable : 1;              /*!< Disable USB2 LPM               */
};

/* USB DRD HW information definition on a soc */
struct HAL_USBDRD_DEV {
    struct USB3DRD_REG *pReg;  /*!< Register base address  */
    IRQn_Type irqNum;          /*!< DRD IRQ Number        */
    uint32_t busGateID;        /*!< DRD BUS Clk Gate      */
    uint32_t refGateID;        /*!< DRD Ref Clk Gate      */
    uint32_t suspendGateID;    /*!< DRD Suspend Clk Gate  */
    struct USB_DRD_CFG cfg;   /*!< DRD config parameters */
};

struct DRD_USB_CTRLREQUEST {
    uint8_t bRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __PACKED;

/** @} */

/***************************** Function Declare ******************************/
/** @defgroup USB_DRD_CORE_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_DRD_CoreInit(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG);
HAL_Status HAL_DRD_CoreDeInit(struct USB3DRD_REG *pUSB3DRD);
HAL_Status HAL_DRD_EnableSusphy(struct USB3DRD_REG *pUSB3DRD, struct USB_DRD_CFG *pCFG, bool enable);
HAL_Status HAL_DRD_SetMode(struct USB3DRD_REG *pUSB3DRD, uint32_t mode);
void *HAL_DRD_MemMallocAlign(uintptr_t size, uintptr_t align);
void HAL_DRD_MemFreeAlign(void *ptr);

/** @} */

#endif /** _HAL_USB_DRD_CORE_H_ */

/** @} */

/** @} */

#endif /** defined(HAL_USB_DRD_MODULE_ENABLED) || defined(HAL_USB_XHCI_MODULE_ENABLED) */
