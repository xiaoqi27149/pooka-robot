/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_USB_DRD_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup USB_DRD_GADGET
 *  @{
 */

#ifndef  _HAL_USBD_DRD_H_
#define  _HAL_USBD_DRD_H_

#include "hal_def.h"
#include "hal_drd_core.h"

/***************************** MACRO Definition ******************************/
/** @defgroup USB_DRD_GADGET_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/* DEPCFG parameter 1 */
#define DRD_DEPCFG_INT_NUM(n)          (((n) & 0x1f) << 0)
#define DRD_DEPCFG_XFER_COMPLETE_EN    HAL_BIT(8)
#define DRD_DEPCFG_XFER_IN_PROGRESS_EN HAL_BIT(9)
#define DRD_DEPCFG_XFER_NOT_READY_EN   HAL_BIT(10)
#define DRD_DEPCFG_FIFO_ERROR_EN       HAL_BIT(11)
#define DRD_DEPCFG_STREAM_EVENT_EN     HAL_BIT(13)
#define DRD_DEPCFG_BINTERVAL_M1(n)     (((n) & 0xff) << 16)
#define DRD_DEPCFG_STREAM_CAPABLE      HAL_BIT(24)
#define DRD_DEPCFG_EP_NUMBER(n)        (((n) & 0x1f) << 25)
#define DRD_DEPCFG_BULK_BASED          HAL_BIT(30)
#define DRD_DEPCFG_FIFO_BASED          HAL_BIT(31)

/* DEPCFG parameter 0 */
#define DRD_DEPCFG_EP_TYPE(n)         (((n) & 0x3) << 1)
#define DRD_DEPCFG_MAX_PACKET_SIZE(n) (((n) & 0x7ff) << 3)
#define DRD_DEPCFG_FIFO_NUMBER(n)     (((n) & 0x1f) << 17)
#define DRD_DEPCFG_BURST_SIZE(n)      (((n) & 0xf) << 22)
#define DRD_DEPCFG_DATA_SEQ_NUM(n)    ((n) << 26)
/* This applies for core versions earlier than 1.94a */
#define DRD_DEPCFG_IGN_SEQ_NUM HAL_BIT(31)
/* These apply for core versions 1.94a and later */
#define DRD_DEPCFG_ACTION_INIT    (0 << 30)
#define DRD_DEPCFG_ACTION_RESTORE HAL_BIT(30)
#define DRD_DEPCFG_ACTION_MODIFY  (2 << 30)

/* DEPXFERCFG parameter 0 */
#define DRD_DEPXFERCFG_NUM_XFER_RES(n) ((n) & 0xffff)

/* U1 Device exit Latency */
#define DRD_DEFAULT_U1_DEV_EXIT_LAT 0x0A

/* U2 Device exit Latency */
#define DRD_DEFAULT_U2_DEV_EXIT_LAT 0x1FF

/* Frame/Microframe Number Mask */
#define DRD_FRNUMBER_MASK 0x3fff

#define DRD_ALIGN_FRAME(d, n) (((d)->frameNumber + ((d)->interval * (n))) \
                & ~((d)->interval - 1))

#define DRD_EP_FLAG_STALLED HAL_BIT(0)
#define DRD_EP_FLAG_WEDGED  HAL_BIT(1)

#define DRD_EP_DIRECTION_TX true
#define DRD_EP_DIRECTION_RX false

#define DRD_EP0_TRB_NUM 4
#define DRD_TRB_NUM     32
#define DRD_TRB_MASK    (DRD_TRB_NUM - 1)

#define DRD_DEPEVT_XFERCOMPLETE   0x01
#define DRD_DEPEVT_XFERINPROGRESS 0x02
#define DRD_DEPEVT_XFERNOTREADY   0x03
#define DRD_DEPEVT_RXTXFIFOEVT    0x04
#define DRD_DEPEVT_STREAMEVT      0x06
#define DRD_DEPEVT_EPCMDCMPLT     0x07

/***************************** Structure Definition **************************/
typedef enum {
    DRD_EP0_UNKNOWN = 0,
    DRD_EP0_COMPLETE,
    DRD_EP0_NRDY_DATA,
    DRD_EP0_NRDY_STATUS,
} eDRD_ep0Next;

typedef enum {
    EP0_UNCONNECTED = 0,
    EP0_SETUP_PHASE,
    EP0_DATA_PHASE,
    EP0_STATUS_PHASE,
} eDRD_ep0State;

struct DRD_TRB {
    uint32_t bpl;
    uint32_t bph;
    uint32_t size;
    uint32_t ctrl;
} __PACKED;

struct DRD_EVENT_BUFFER {
    void *buf;
    uint32_t length;
    uint32_t lpos;
    uint32_t count;
    uint32_t flags;

#define DRD_EVENT_PENDING (1UL << 0)

    uintptr_t dmaAddr;
};

struct DRD_EP_INFO {
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
    uint8_t bMaxBurst;
};

struct DRD_EP {
    struct DRD_TRB *trbPool;
    struct DRD_EP_INFO epInfo;
    uintptr_t trbPoolDma;
    uintptr_t trbDma;
    uintptr_t dmaAddr;
    uint8_t *pxferBuff;
    uint8_t trbEnqueue;
    uint8_t trbDequeue;

    uint32_t flags;
#define DRD_EP_ENABLED                HAL_BIT(0)
#define DRD_EP_STALL                  HAL_BIT(1)
#define DRD_EP_WEDGE                  HAL_BIT(2)
#define DRD_EP_TRANSFER_STARTED       HAL_BIT(3)
#define DRD_EP_END_TRANSFER_PENDING   HAL_BIT(4)
#define DRD_EP_PENDING_REQUEST        HAL_BIT(5)
#define DRD_EP_DELAY_START            HAL_BIT(6)
#define DRD_EP_WAIT_TRANSFER_COMPLETE HAL_BIT(7)
#define DRD_EP_IGNORE_NEXT_NOSTREAM   HAL_BIT(8)
#define DRD_EP_FORCE_RESTART_STREAM   HAL_BIT(9)
#define DRD_EP_FIRST_STREAM_PRIMED    HAL_BIT(10)
#define DRD_EP_PENDING_CLEAR_STALL    HAL_BIT(11)
#define DRD_EP_TXFIFO_RESIZED         HAL_BIT(12)
#define DRD_EP_DELAY_STOP             HAL_BIT(13)
#define DRD_EP_TRANSFER_QUEUED        HAL_BIT(14)

    /* This last one is specific to EP0 */
#define DRD_EP0_DIR_IN HAL_BIT(31)

    uint8_t number;
    uint8_t resourceIndex;
    uint8_t type;
    uint8_t maxBurst;
    uint16_t maxPacket;
    uint32_t frameNumber;
    uint32_t interval;
    uint32_t actualLen;
    uint32_t xferLen;
    uint32_t numTrbs;
    uint32_t direction : 1;
    uint32_t streamCapable : 1;
    uint32_t zero : 1;
    uint32_t needsExtraTrb : 1;
};

struct DRD_EVENT_TYPE {
    uint32_t isDevSpec : 1;
    uint32_t type : 7;
    uint32_t reserved8_31 : 24;
} __PACKED;

struct DRD_EVENT_DEPEVT {
    uint32_t oneBit : 1;
    uint32_t endpointNumber : 5;
    uint32_t endpointEvent : 4;
    uint32_t reserved11_10 : 2;
    uint32_t status : 4;

/* Within XferNotReady */
#define DEPEVT_STATUS_TRANSFER_ACTIVE HAL_BIT(3)

/* Within XferComplete or XferInProgress */
#define DEPEVT_STATUS_BUSERR      HAL_BIT(0)
#define DEPEVT_STATUS_SHORT       HAL_BIT(1)
#define DEPEVT_STATUS_IOC         HAL_BIT(2)
#define DEPEVT_STATUS_LST         HAL_BIT(3)
#define DEPEVT_STATUS_MISSED_ISOC HAL_BIT(3)

/* Stream event only */
#define DEPEVT_STREAMEVT_FOUND    1
#define DEPEVT_STREAMEVT_NOTFOUND 2

/* Control-only Status */
#define DEPEVT_STATUS_CONTROL_DATA   1
#define DEPEVT_STATUS_CONTROL_STATUS 2

/* In response to Start Transfer */
#define DEPEVT_TRANSFER_NO_RESOURCE 1
#define DEPEVT_TRANSFER_BUS_EXPIRY  2

    uint32_t parameters : 16;

/* For Command Complete Events */
#define DEPEVT_PARAMETER_CMD(n) (((n) & (0xf << 8)) >> 8)
} __PACKED;

struct DRD_EVENT_DEVT {
    uint32_t oneBit : 1;
    uint32_t deviceEvent : 7;
    uint32_t type : 4;
    uint32_t reserved15_12 : 4;
    uint32_t eventInfo : 9;
    uint32_t reserved31_25 : 7;
} __PACKED;

struct DRD_EVENT_GEVT {
    uint32_t oneBit : 1;
    uint32_t deviceEvent : 7;
    uint32_t phyPortNumber : 4;
    uint32_t reserved31_12 : 20;
} __PACKED;

union DRD_EVENT {
    uint32_t raw;
    struct DRD_EVENT_TYPE type;
    struct DRD_EVENT_DEPEVT depevt;
    struct DRD_EVENT_DEVT devt;
    struct DRD_EVENT_GEVT gevt;
};

struct DRD_GADGET_EP_CMD_PARAMS {
    uint32_t param2;
    uint32_t param1;
    uint32_t param0;
};

/** USB DRD Handle Structure definition */
struct USB_DRD_HANDLE {
    struct USB3DRD_REG *pReg;
    struct USB_DRD_CFG cfg;
    struct DRD_TRB *ep0Trb;
    struct DRD_EVENT_BUFFER evBuf;
    struct DRD_EP eps[DRD_ENDPOINTS_NUM];
    eDRD_ep0Next ep0NextEvent;
    eDRD_ep0State ep0State;
    eDRD_linkState linkState;
    void *bounce;
    uint8_t *setupBuf;
    uintptr_t ep0TrbAddr;
    uintptr_t bounceAddr;
    uint8_t numOutEps;
    uint8_t numInEps;
    uint8_t enumSpeed;
    uint8_t addrH;
    uint16_t isochDelay;
    uint32_t ep0Bounced : 1;
    uint32_t ep0ExpectIn : 1;
    uint32_t pullupsConnected : 1;
    uint32_t resizeFifos : 1;
    uint32_t setupPacketPending : 1;
    uint32_t threeStageSetup : 1;
    uint32_t connected : 1;
    uint32_t suspended : 1;
};

/** @} */

/***************************** Function Declare ******************************/
/** @defgroup USB_DRD_GADGET_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_DRD_GadgetInit(struct USB_DRD_HANDLE *pDRD);
HAL_Status HAL_DRD_GadgetDeInit(struct USB_DRD_HANDLE *pDRD);
HAL_Status HAL_DRD_IRQHandler(struct USB_DRD_HANDLE *pDRD);
HAL_Status HAL_DRD_GadgetStart(struct USB_DRD_HANDLE *pDRD);
HAL_Status HAL_DRD_GadgetStop(struct USB_DRD_HANDLE *pDRD);
HAL_Status HAL_DRD_EPOpen(struct USB_DRD_HANDLE *pDRD, struct DRD_EP_INFO *pEpInfo);
HAL_Status HAL_DRD_EPClose(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr);
HAL_Status HAL_DRD_EPReceive(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint8_t *pBuf, uint32_t len);
HAL_Status HAL_DRD_EPTransmit(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint8_t *pBuf, uint32_t len);
HAL_Status HAL_DRD_EPSetStall(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr);
HAL_Status HAL_DRD_EPClrStall(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr);
HAL_Status HAL_DRD_SetAddress(struct USB_DRD_HANDLE *pDRD, uint8_t addr);
void HAL_DRD_ResetCallback(struct USB_DRD_HANDLE *pDRD);
void HAL_DRD_SetupStageCallback(struct USB_DRD_HANDLE *pDRD);
void HAL_DRD_SuspendCallback(struct USB_DRD_HANDLE *pDRD);
void HAL_DRD_DataOutStageCallback(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint32_t nbytes);
void HAL_DRD_DataInStageCallback(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint32_t nbytes);
void HAL_DRD_DisconnectCallback(struct USB_DRD_HANDLE *pDRD);

/** @} */

#endif /** _HAL_USBD_DRD_H_ */

/** @} */

/** @} */

#endif /** HAL_USB_DRD_MODULE_ENABLED */
