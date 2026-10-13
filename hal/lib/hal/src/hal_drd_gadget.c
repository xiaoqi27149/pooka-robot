/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_USB_DRD_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup USB_DRD_GADGET
 *  @brief USB DRD_GADGET HAL module driver
 *  @{
 */

/** @defgroup USB_DRD_GADGET_How_To_Use How To Use
 *  @{

 The USB_DRD_GADGET HAL driver can be used as follows:

 - Declare a struct USB_DRD_HANDLE handle structure, for example:
   ```
   struct USB_DRD_HANDLE pDRD;
   ```
 - Fill parameters of Init structure in DRD handle

 - Initialize the DRD low level resources:
     - Enable the DRD Low Level interface clocks;
     - Configure DRD NVIC interrupt;

 - Call HAL_DRD_GadgetInit() API to initialize the DRD Device Mode

 - Enable DRD Gadget transmission and reception:
   ```
   HAL_DRD_GadgetStart();
   ```

 @} */

/** @defgroup USB_DRD_GADGET_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

static const char * const speedNames[] = {
    [DRD_USB_SPEED_UNKNOWN] = "UNKNOWN",
    [DRD_USB_SPEED_LOW] = "low-speed",
    [DRD_USB_SPEED_FULL] = "full-speed",
    [DRD_USB_SPEED_HIGH] = "high-speed",
    [DRD_USB_SPEED_WIRELESS] = "wireless",
    [DRD_USB_SPEED_SUPER] = "super-speed",
};

static const char *DRD_SpeedString(eUSB_speed speed)
{
    if (speed < 0 || speed >= HAL_ARRAY_SIZE(speedNames)) {
        speed = DRD_USB_SPEED_UNKNOWN;
    }

    return speedNames[speed];
}

static HAL_Status DRD_AllocTrbPool(struct DRD_EP *dep)
{
    dep->trbPool = HAL_DRD_MemMallocAlign(sizeof(struct DRD_TRB) * DRD_TRB_NUM, DRD_DMA_ALIGN_SIZE);

    if (dep->trbPool == NULL) {
        HAL_DBG_ERR("DRD failed to allocate trb pool for ep%d%s\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");

        return HAL_ERROR;
    }

    memset(dep->trbPool, 0x00, sizeof(struct DRD_TRB) * DRD_TRB_NUM);
    dep->trbPoolDma = HAL_CpuAddrToDmaAddr((uintptr_t)dep->trbPool);

    return HAL_OK;
}

static void DRD_FreeTrbPool(struct DRD_EP *dep)
{
    HAL_DRD_MemFreeAlign(dep->trbPool);

    dep->trbPool = NULL;
    dep->trbPoolDma = 0;
}

/**
 * @brief Free allocated event buffer.
 * @param pDRD: DRD handle
 */
static void DRD_FreeEventBuffer(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EVENT_BUFFER *evt = &pDRD->evBuf;

    HAL_DRD_MemFreeAlign(evt->buf);
}

/**
 * @brief  Allocates event buffer of size @length.
 * @param  pDRD: DRD handle
 * @param  length: Size of event buffer
 * @return HAL status
 */
static HAL_Status DRD_AllocEventBuffer(struct USB_DRD_HANDLE *pDRD, uint32_t length)
{
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    struct DRD_EVENT_BUFFER *evt = &pDRD->evBuf;
    uint8_t addrH;

    evt->length = length;
    evt->buf = HAL_DRD_MemMallocAlign(length, DRD_DMA_ALIGN_SIZE);
    if (evt->buf == NULL) {
        HAL_DBG_ERR("DRD failed to allocate event buffer\n");

        return HAL_ERROR;
    }

    evt->dmaAddr = HAL_CpuAddrToDmaAddr((uintptr_t)evt->buf);

    HAL_DBG("DRD evt buf=0x%" PRIxPTR ", evt dmaAddr 0x%" PRIxPTR "\n", (uintptr_t)evt->buf, evt->dmaAddr);

    if (pCFG->hasAwidth64) {
        addrH = (uint8_t)HAL_UPPER_32_BITS(evt->dmaAddr);
        pDRD->addrH = addrH;

#if defined(DRD_ADRHI_GRF)
        HAL_DBG("addr_h %x DRD_ADRHI_GRF=0x%" PRIxPTR "\n", addrH, (uintptr_t)(uint32_t *)DRD_ADRHI_GRF);

        WRITE_REG(*(uint32_t *)DRD_ADRHI_GRF, ((DRD_AW_ADDR_H_MASK | DRD_AR_ADDR_H_MASK) << 16 | addrH << 8 | addrH));
#endif
    }

    return HAL_OK;
}

/**
 * @brief  Setup allocated event buffer.
 * @param  pDRD: DRD handle
 * @return HAL status
 */
static HAL_Status DRD_EventBufferSetup(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EVENT_BUFFER *evt = &pDRD->evBuf;
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;

    evt->lpos = 0;
    memset(evt->buf, 0, evt->length);
    pUSB3DRD->GEVNTADRLO0 = HAL_LOWER_32_BITS(evt->dmaAddr);
    pUSB3DRD->GEVNTADRHI0 = HAL_UPPER_32_BITS(evt->dmaAddr);
    pUSB3DRD->GEVNTSIZ0 = DRD_GEVNTSIZ_SIZE(evt->length);

    /* Clear any stale event */
    reg = pUSB3DRD->GEVNTCOUNT0;
    pUSB3DRD->GEVNTCOUNT0 = reg;

    return HAL_OK;
}

/**
 * @brief Cleanup allocated event buffer
 * @param pDRD: DRD handle
 */
static void DRD_EventBufferCleanup(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EVENT_BUFFER *evt = &pDRD->evBuf;
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;

    evt->lpos = 0;
    pUSB3DRD->GEVNTADRLO0 = 0;
    pUSB3DRD->GEVNTADRHI0 = 0;
    pUSB3DRD->GEVNTSIZ0 = (DRD_GEVNTSIZ_INTMASK | DRD_GEVNTSIZ_SIZE(0));

    /* Clear any stale event */
    reg = pUSB3DRD->GEVNTCOUNT0;
    pUSB3DRD->GEVNTCOUNT0 = reg;
}

static HAL_Status DRD_SendGadgetEpCmd(struct USB_DRD_HANDLE *pDRD,
                                      struct DRD_EP *dep, uint32_t cmd,
                                      struct DRD_GADGET_EP_CMD_PARAMS *params);

static HAL_Status DRD_GadgetEpSetHalt(struct USB_DRD_HANDLE *pDRD,
                                      struct DRD_EP *dep, uint32_t value,
                                      uint32_t protocol);

static HAL_Status DRD_StopActiveTransfer(struct USB_DRD_HANDLE *pDRD,
                                         struct DRD_EP *dep, bool force,
                                         bool interrupt);

/**
 * @brief Get the number of IN and OUT endpoints.
 * @param pDRD: DRD handle
 */
static void DRD_CoreNumEps(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_HWPARAMS *parms = &pDRD->cfg.hwparams;

    pDRD->numInEps = DRD_NUM_IN_EPS(parms);
    pDRD->numOutEps = DRD_NUM_EPS(parms) - pDRD->numInEps;

    HAL_DBG("DRD found %d IN and %d OUT endpoints\n", pDRD->numInEps, pDRD->numOutEps);
}

static void DRD_Ep0PrepareOneTrb(struct USB_DRD_HANDLE *pDRD,
                                 struct DRD_EP *dep, uintptr_t bufDma,
                                 uint32_t len, uint32_t type, bool chain)
{
    struct DRD_TRB *trb;

    trb = &pDRD->ep0Trb[dep->trbEnqueue];

    if (chain) {
        dep->trbEnqueue++;
    }

    trb->bpl = HAL_LOWER_32_BITS(bufDma);
    trb->bph = HAL_UPPER_32_BITS(bufDma);
    trb->size = len;
    trb->ctrl = type;

    trb->ctrl |= (DRD_TRB_CTRL_HWO | DRD_TRB_CTRL_ISP_IMI);

    if (chain) {
        trb->ctrl |= DRD_TRB_CTRL_CHN;
    } else {
        trb->ctrl |= (DRD_TRB_CTRL_IOC | DRD_TRB_CTRL_LST);
    }

    HAL_DCACHE_CleanByRange((uintptr_t)trb, sizeof(struct DRD_TRB) * DRD_EP0_TRB_NUM);

    if (dep->direction && len) {
        HAL_DCACHE_CleanByRange((uintptr_t)bufDma, len);
    }
}

static HAL_Status DRD_Ep0StartTrans(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;
    HAL_Status ret;

    if (dep->flags & DRD_EP_TRANSFER_STARTED) {
        return 0;
    }

    memset(&params, 0, sizeof(params));

    params.param0 = HAL_UPPER_32_BITS(pDRD->ep0TrbAddr);
    params.param1 = HAL_LOWER_32_BITS(pDRD->ep0TrbAddr);

    ret = DRD_SendGadgetEpCmd(pDRD, dep, DRD_DEPCMD_STARTTRANSFER, &params);
    if (ret) {
        HAL_DBG_ERR("DRD ep0 failed to start trans\n");

        return ret;
    }

    pDRD->ep0NextEvent = DRD_EP0_COMPLETE;

    return HAL_OK;
}

static void DRD_Ep0DoControlData(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    uint32_t trbLength = 0;
    HAL_Status ret;

    if (dep->xferLen == 0) {
        if (!dep->direction) {
            trbLength = dep->maxPacket;
        }

        DRD_Ep0PrepareOneTrb(pDRD, dep, pDRD->bounceAddr, trbLength, DRD_TRBCTL_CONTROL_DATA, false);
        ret = DRD_Ep0StartTrans(pDRD, dep);
    } else if (!HAL_IS_ALIGNED(dep->xferLen, dep->maxPacket) && (dep->number == 0)) {
        uint32_t maxPacket;
        uint32_t rem;

        maxPacket = dep->maxPacket;
        rem = dep->xferLen % maxPacket;
        pDRD->ep0Bounced = true;

        /* prepare normal TRB */
        DRD_Ep0PrepareOneTrb(pDRD, dep, dep->dmaAddr, dep->xferLen, DRD_TRBCTL_CONTROL_DATA, true);

        /* Now prepare one extra TRB to align transfer size */
        DRD_Ep0PrepareOneTrb(pDRD, dep, pDRD->bounceAddr, maxPacket - rem, DRD_TRBCTL_CONTROL_DATA, false);
        ret = DRD_Ep0StartTrans(pDRD, dep);
    } else if (HAL_IS_ALIGNED(dep->xferLen, dep->maxPacket) && dep->xferLen && dep->zero) {
        /* TODO for dep->zero*/

        /* prepare normal TRB */
        DRD_Ep0PrepareOneTrb(pDRD, dep, dep->dmaAddr, dep->xferLen, DRD_TRBCTL_CONTROL_DATA, true);

        if (!dep->direction) {
            trbLength = dep->maxPacket;
        }

        /* Now prepare one extra TRB to align transfer size */
        DRD_Ep0PrepareOneTrb(pDRD, dep, pDRD->bounceAddr, trbLength, DRD_TRBCTL_CONTROL_DATA, false);
        ret = DRD_Ep0StartTrans(pDRD, dep);
    } else {
        DRD_Ep0PrepareOneTrb(pDRD, dep, dep->dmaAddr, dep->xferLen, DRD_TRBCTL_CONTROL_DATA, false);

        ret = DRD_Ep0StartTrans(pDRD, dep);
    }

    if (ret) {
        HAL_DBG_ERR("DRD ep0 failed to do control data\n");
    }
}

static HAL_Status DRD_EP0StartXfer(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    dep->flags &= ~DRD_EP_TRANSFER_QUEUED;

    /* EP-IN endpoint */
    if (dep->direction && dep->xferLen > 0) {
        HAL_DCACHE_CleanByRange((uintptr_t)dep->pxferBuff, dep->xferLen);
    }

    if (pDRD->ep0NextEvent == DRD_EP0_NRDY_STATUS) {
        /* STATUS phase will be processed in ep0_xfernotready */
        return HAL_OK;
    }

    if (pDRD->threeStageSetup) {
        pDRD->ep0State = EP0_DATA_PHASE;

        DRD_Ep0DoControlData(pDRD, dep);
    }

    return HAL_OK;
}

static void DRD_Ep0DoControlStatus(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    HAL_Status ret;
    uint32_t type;

    type = pDRD->threeStageSetup ? DRD_TRBCTL_CONTROL_STATUS3 : DRD_TRBCTL_CONTROL_STATUS2;

    DRD_Ep0PrepareOneTrb(pDRD, dep, pDRD->ep0TrbAddr, 0, type, false);

    ret = DRD_Ep0StartTrans(pDRD, dep);
    if (ret) {
        HAL_DBG_ERR("DRD ep0 failed to do control status\n");
    }
}

static HAL_Status DRD_Ep0OutStart(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EP *dep;
    uint32_t epNum;
    HAL_Status ret;

    dep = &pDRD->eps[0];
    DRD_Ep0PrepareOneTrb(pDRD, dep, pDRD->ep0TrbAddr, 8, DRD_TRBCTL_CONTROL_SETUP, false);
    ret = DRD_Ep0StartTrans(pDRD, dep);
    if (ret) {
        HAL_DBG_ERR("DRD ep0 out start fail\n");
    }

    for (epNum = 2; epNum < DRD_ENDPOINTS_NUM; epNum++) {
        dep = &pDRD->eps[epNum];
        if (!dep->trbPool) {
            continue;
        }

        if (!(dep->flags & DRD_EP_DELAY_STOP)) {
            continue;
        }

        dep->flags &= ~DRD_EP_DELAY_STOP;
        if (pDRD->connected) {
            DRD_StopActiveTransfer(pDRD, dep, true, true);
        } else {
            DRD_StopActiveTransfer(pDRD, dep, true, false);
        }
    }

    return ret;
}

static void DRD_Ep0EndControlData(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;
    uint32_t cmd;

    if (!dep->resourceIndex && dep->number) {
        return;
    }

    cmd = DRD_DEPCMD_ENDTRANSFER;
    cmd |= DRD_DEPCMD_CMDIOC;
    cmd |= DRD_DEPCMD_PARAM(dep->resourceIndex);
    memset(&params, 0, sizeof(params));
    DRD_SendGadgetEpCmd(pDRD, dep, cmd, &params);
    dep->resourceIndex = 0;
}

static void DRD_Ep0StallAndRestart(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EP *dep;

    /* reinitialize physical ep1 */
    dep = &pDRD->eps[1];
    dep->flags = DRD_EP_ENABLED;

    /* stall is always issued on EP0 */
    dep = &pDRD->eps[0];
    DRD_GadgetEpSetHalt(pDRD, dep, 1, false);
    dep->flags = DRD_EP_ENABLED;

    pDRD->eps[0].trbEnqueue = 0;
    pDRD->eps[1].trbEnqueue = 0;
    pDRD->ep0State = EP0_SETUP_PHASE;
    DRD_Ep0OutStart(pDRD);
}

static void DRD_Ep0ResetState(struct USB_DRD_HANDLE *pDRD)
{
    uint32_t dir;

    if (pDRD->ep0State != EP0_SETUP_PHASE) {
        dir = !!pDRD->ep0ExpectIn;
        if (pDRD->ep0State == EP0_DATA_PHASE) {
            DRD_Ep0EndControlData(pDRD, &pDRD->eps[dir]);
        } else {
            DRD_Ep0EndControlData(pDRD, &pDRD->eps[!dir]);
        }

        pDRD->eps[0].trbEnqueue = 0;
        pDRD->eps[1].trbEnqueue = 0;

        DRD_Ep0StallAndRestart(pDRD);
    }
}

static void DRD_Ep0InspectSetup(struct USB_DRD_HANDLE *pDRD, const struct DRD_EVENT_DEPEVT *event)
{
    struct DRD_USB_CTRLREQUEST *ctrl = (void *)pDRD->ep0Trb;
    uint32_t len;

    if (!pDRD->connected) {
        return;
    }

    HAL_DCACHE_InvalidateByRange((uintptr_t)pDRD->ep0Trb, DRD_DMA_ALIGN_SIZE);

    len = ctrl->wLength;
    if (!len) {
        pDRD->threeStageSetup = false;
        pDRD->ep0ExpectIn = false;
        pDRD->ep0NextEvent = DRD_EP0_NRDY_STATUS;
    } else {
        pDRD->threeStageSetup = true;
        pDRD->ep0ExpectIn = !!(ctrl->bRequestType & DRD_USB_DIR_IN);
        pDRD->ep0NextEvent = DRD_EP0_NRDY_DATA;
    }

    if ((ctrl->bRequestType & DRD_USB_TYPE_MASK) == DRD_USB_TYPE_STANDARD) {
        switch (ctrl->bRequest) {
        case DRD_USB_REQ_SET_ISOCH_DELAY:
            /* TODO: store isoch delay */
            break;
        case DRD_USB_REQ_SET_SEL:
            /* Not support U1 and U2 State for Device */
            DRD_Ep0StallAndRestart(pDRD);
            break;
        default:
            HAL_DRD_SetupStageCallback(pDRD);
            break;
        }
    } else {
        HAL_DRD_SetupStageCallback(pDRD);
    }
}

static void DRD_Ep0XferNotReady(struct USB_DRD_HANDLE *pDRD, const struct DRD_EVENT_DEPEVT *event)
{
    struct DRD_EP *dep;

    switch (event->status) {
    case DEPEVT_STATUS_CONTROL_DATA:
        if (!pDRD->connected) {
            return;
        }

        if (pDRD->ep0ExpectIn != event->endpointNumber) {
            dep = &pDRD->eps[pDRD->ep0ExpectIn];

            HAL_DBG_ERR("DRD unexpected direction for Data Phase\n");
            DRD_Ep0EndControlData(pDRD, dep);
            DRD_Ep0StallAndRestart(pDRD);

            return;
        }

        break;
    case DEPEVT_STATUS_CONTROL_STATUS:
        if (pDRD->ep0NextEvent != DRD_EP0_NRDY_STATUS) {
            return;
        }

        if (pDRD->setupPacketPending) {
            DRD_Ep0StallAndRestart(pDRD);

            return;
        }

        pDRD->ep0State = EP0_STATUS_PHASE;

        dep = &pDRD->eps[event->endpointNumber];
        DRD_Ep0DoControlStatus(pDRD, dep);
    }
}

static void DRD_Ep0CompleteData(struct USB_DRD_HANDLE *pDRD, const struct DRD_EVENT_DEPEVT *event)
{
    struct DRD_TRB *trb;
    struct DRD_EP *dep;
    uint32_t transferred = 0;
    uint32_t status;
    uint8_t length;
    uint8_t epNum;

    epNum = event->endpointNumber;
    dep = &pDRD->eps[epNum];
    pDRD->ep0NextEvent = DRD_EP0_NRDY_STATUS;
    trb = pDRD->ep0Trb;

    HAL_DCACHE_InvalidateByRange((uintptr_t)trb, sizeof(struct DRD_TRB) * DRD_EP0_TRB_NUM);

    status = DRD_TRB_SIZE_TRBSTS(trb->size);
    if (status == DRD_TRBSTS_SETUP_PENDING) {
        pDRD->setupPacketPending = true;
        HAL_DBG("DRD setup pending received\n");

        return;
    }

    length = trb->size & DRD_TRB_SIZE_MASK;
    transferred = dep->xferLen - length;
    dep->actualLen += transferred;

    HAL_DBG("DRD ep%d%s xferLen %d, actualLen %d\n", epNum >> 1, (epNum & 1) ? "in" : "out", dep->xferLen, dep->actualLen);

    if ((HAL_IS_ALIGNED(dep->xferLen, dep->maxPacket) && dep->xferLen && dep->zero) || pDRD->ep0Bounced) {
        trb++;
        trb->ctrl &= ~DRD_TRB_CTRL_HWO;

        HAL_DCACHE_CleanByRange((uintptr_t)trb, sizeof(struct DRD_TRB));

        dep->trbEnqueue = 0;
        pDRD->ep0Bounced = false;
    }

    if ((epNum & 1) && dep->actualLen < dep->xferLen) {
        DRD_Ep0StallAndRestart(pDRD);
    } else {
        epNum = (epNum & 1) ? 0x80 : 0x00;

        if (dep->direction) {
            epNum = 0x80;
            HAL_DRD_DataInStageCallback(pDRD, epNum, dep->actualLen);
        } else {
            epNum = 0x00;
            if (dep->actualLen) {
                HAL_DCACHE_InvalidateByRange((uintptr_t)dep->pxferBuff, dep->actualLen);
            }
            HAL_DRD_DataOutStageCallback(pDRD, epNum, dep->actualLen);
        }
    }
}

static void DRD_Ep0CompleteStatus(struct USB_DRD_HANDLE *pDRD, const struct DRD_EVENT_DEPEVT *event)
{
    struct DRD_TRB *trb;
    uint32_t status;

    trb = pDRD->ep0Trb;

    HAL_DCACHE_InvalidateByRange((uintptr_t)trb, sizeof(struct DRD_TRB) * DRD_EP0_TRB_NUM);

    status = DRD_TRB_SIZE_TRBSTS(trb->size);
    if (status == DRD_TRBSTS_SETUP_PENDING) {
        pDRD->setupPacketPending = true;
    }

    pDRD->ep0State = EP0_SETUP_PHASE;
    DRD_Ep0OutStart(pDRD);
}

static void DRD_Ep0XferComplete(struct USB_DRD_HANDLE *pDRD, const struct DRD_EVENT_DEPEVT *event)
{
    struct DRD_EP *dep = &pDRD->eps[event->endpointNumber];

    dep->flags &= ~DRD_EP_TRANSFER_STARTED;
    dep->resourceIndex = 0;
    pDRD->setupPacketPending = false;

    switch (pDRD->ep0State) {
    case EP0_SETUP_PHASE:
        DRD_Ep0InspectSetup(pDRD, event);
        break;
    case EP0_DATA_PHASE:
        DRD_Ep0CompleteData(pDRD, event);
        break;
    case EP0_STATUS_PHASE:
        DRD_Ep0CompleteStatus(pDRD, event);
        break;
    default:
        HAL_DBG("DRD UNKNOWN ep0state %d\n", pDRD->ep0State);
    }
}

static void DRD_Ep0Interrupt(struct USB_DRD_HANDLE *pDRD, const struct DRD_EVENT_DEPEVT *event)
{
    struct DRD_EP *dep = &pDRD->eps[event->endpointNumber];
    uint8_t cmd;

    switch (event->endpointEvent) {
    case DRD_DEPEVT_XFERCOMPLETE:
        DRD_Ep0XferComplete(pDRD, event);
        break;
    case DRD_DEPEVT_XFERNOTREADY:
        DRD_Ep0XferNotReady(pDRD, event);
        break;
    case DRD_DEPEVT_XFERINPROGRESS:
    case DRD_DEPEVT_RXTXFIFOEVT:
    case DRD_DEPEVT_STREAMEVT:
        break;
    case DRD_DEPEVT_EPCMDCMPLT:
        cmd = DEPEVT_PARAMETER_CMD(event->parameters);

        if (cmd == DRD_DEPCMD_ENDTRANSFER) {
            dep->flags &= ~DRD_EP_END_TRANSFER_PENDING;
            dep->flags &= ~DRD_EP_TRANSFER_STARTED;
        }
        break;
    }
}

static struct DRD_TRB *DRD_EpPrevTrb(struct DRD_EP *dep, uint8_t index)
{
    uint8_t tmp = index;

    if (!tmp) {
        tmp = DRD_TRB_NUM - 1;
    }

    return &dep->trbPool[tmp - 1];
}

static void DRD_EpIncTrb(uint8_t *index)
{
    (*index)++;
    if (*index == (DRD_TRB_NUM - 1)) {
        *index = 0;
    }
}

static void DRD_EpIncEnq(struct DRD_EP *dep)
{
    DRD_EpIncTrb(&dep->trbEnqueue);
}

static void DRD_EpIncDeq(struct DRD_EP *dep)
{
    DRD_EpIncTrb(&dep->trbDequeue);
}

static uint32_t DRD_GadgetGetFrame(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;

    reg = pUSB3DRD->DSTS;

    return DRD_DSTS_SOFFN(reg);
}

static uint32_t DRD_GadgetGetLinkState(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;

    reg = pUSB3DRD->DSTS;

    return DRD_DSTS_USBLNKST(reg);
}

static HAL_Status DRD_GadgetSetLinkState(struct USB_DRD_HANDLE *pDRD, eDRD_linkState state)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    uint32_t retries = 10000;
    uint32_t reg;

    /*
     * Wait until device controller is ready.
     * Only applies to 1.94a and later RTL.
     */
    if (!DRD_VER_IS_PRIOR(DRD, 194A)) {
        while (--retries) {
            reg = pUSB3DRD->DSTS;
            if (reg & DRD_DSTS_DCNRD) {
                HAL_DelayUs(5);
            } else {
                break;
            }
        }

        if (retries <= 0) {
            return HAL_TIMEOUT;
        }
    }

    reg = pUSB3DRD->DCTL;
    reg &= ~DRD_DCTL_ULSTCHNGREQ_MASK;

    /* set no action before sending new link state change */
    pUSB3DRD->DCTL = reg;

    /* set requested state */
    reg |= DRD_DCTL_ULSTCHNGREQ(state);
    pUSB3DRD->DCTL = reg;

    if (!DRD_VER_IS_PRIOR(DRD, 194A)) {
        return HAL_OK;
    }

    /* wait for a change in DSTS */
    retries = 10000;
    while (--retries) {
        reg = pUSB3DRD->DSTS;

        if (DRD_DSTS_USBLNKST(reg) == state) {
            return HAL_OK;
        }

        HAL_DelayUs(5);
    }

    return HAL_TIMEOUT;
}

static HAL_Status DRD_GadgetInitHwEndpoints(struct USB_DRD_HANDLE *pDRD, uint8_t num, uint32_t direction)
{
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    struct DRD_EP *dep;
    uint8_t i;
    HAL_Status ret;

    for (i = 0; i < num; i++) {
        uint8_t epNum = (i << 1) | (!!direction);

        dep = &pDRD->eps[epNum];

        dep->number = epNum;
        dep->direction = !!direction;

        HAL_DBG("DRD init ep%d%s\n", epNum >> 1, (epNum & 1) ? "in" : "out");

        if (epNum == 0 || epNum == 1) {
            if (pCFG->maximumSpeed < DRD_USB_SPEED_SUPER) {
                dep->maxPacket = 64;
            } else {
                dep->maxPacket = 512;
            }
            dep->type = DRD_USB_ENDPOINT_XFER_CONTROL;
            dep->maxBurst = 1;
        } else {
            if (pCFG->maximumSpeed < DRD_USB_SPEED_SUPER) {
                dep->maxPacket = 512;
            } else {
                dep->maxPacket = 1024;
            }

            ret = DRD_AllocTrbPool(dep);
            if (ret) {
                return ret;
            }
        }
    }

    return HAL_OK;
}

static HAL_Status DRD_GadgetInitEndpoints(struct USB_DRD_HANDLE *pDRD)
{
    HAL_Status ret;

    ret = DRD_GadgetInitHwEndpoints(pDRD, pDRD->numOutEps, 0);
    if (ret) {
        HAL_DBG_ERR("DRD failed to allocate OUT endpoints\n");

        return ret;
    }

    ret = DRD_GadgetInitHwEndpoints(pDRD, pDRD->numInEps, 1);
    if (ret) {
        HAL_DBG_ERR("DRD failed to allocate IN endpoints\n");

        return ret;
    }

    return HAL_OK;
}

static void DRD_GadgetFreeEndpoints(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EP *dep;
    uint8_t epNum;

    for (epNum = 0; epNum < DRD_ENDPOINTS_NUM; epNum++) {
        dep = &pDRD->eps[epNum];
        if (!dep->trbPool) {
            continue;
        }

        if (epNum != 0 && epNum != 1) {
            DRD_FreeTrbPool(dep);
        }
    }
}

static void DRD_GadgetSetSpeed(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    uint32_t reg;

    reg = pUSB3DRD->DCFG;
    reg &= ~(DRD_DCFG_SPEED_MASK);

    if (DRD_VER_IS_PRIOR(DRD, 220A)) {
        reg |= DRD_DCFG_SUPERSPEED;
    } else {
        switch (pCFG->maximumSpeed) {
        case DRD_USB_SPEED_FULL:
            reg |= DRD_DCFG_FULLSPEED;
            break;
        case DRD_USB_SPEED_HIGH:
            reg |= DRD_DCFG_HIGHSPEED;
            break;
        case DRD_USB_SPEED_SUPER:
            reg |= DRD_DCFG_SUPERSPEED;
            break;
        default:
            HAL_DBG_ERR("DRD invalid speed (%d)\n", pCFG->maximumSpeed);
            reg |= DRD_DCFG_SUPERSPEED;
        }
    }

    pUSB3DRD->DCFG = reg;
}

static HAL_Status DRD_GadgetResizeTxFifos(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    /* TODO */
    return HAL_OK;
}

static HAL_Status DRD_GadgetWakeup(struct USB_DRD_HANDLE *pDRD)
{
    /* TODO */
    return HAL_OK;
}

static void DRD_GadgetEpGetTransferIndex(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t resId;

    resId = USB3DRD_DRD_DEPCMD(dep->number);
    dep->resourceIndex = DRD_DEPCMD_GET_RSC_IDX(resId);
}

static HAL_Status DRD_SendClearStallEpCmd(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    struct DRD_GADGET_EP_CMD_PARAMS params;
    uint32_t cmd = DRD_DEPCMD_CLEARSTALL;

    if (dep->direction && !DRD_VER_IS_PRIOR(DRD, 260A) && (pDRD->enumSpeed >= DRD_USB_SPEED_SUPER)) {
        cmd |= DRD_DEPCMD_CLEARPENDIN;
    }

    memset(&params, 0, sizeof(params));

    return DRD_SendGadgetEpCmd(pDRD, dep, cmd, &params);
}

static HAL_Status DRD_SendGadgetEpCmd(struct USB_DRD_HANDLE *pDRD,
                                      struct DRD_EP *dep, uint32_t cmd,
                                      struct DRD_GADGET_EP_CMD_PARAMS *params)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t timeout = 5000;
    uint32_t savedConfig = 0;
    uint32_t cmdStatus = 0;
    uint32_t reg;
    HAL_Status ret = HAL_OK;

    /*
     * When operating in USB 2.0 speeds (HS/FS), if GUSB2PHYCFG.ENBLSLPM or
     * GUSB2PHYCFG.SUSPHY is set, it must be cleared before issuing an
     * endpoint command.
     *
     * Save and clear both GUSB2PHYCFG.ENBLSLPM and GUSB2PHYCFG.SUSPHY
     * settings. Restore them after the command is completed.
     *
     * DRD_usb3 3.30a and DRD_usb31 1.90a programming guide section 3.2.2
     */
    if (pDRD->enumSpeed <= DRD_USB_SPEED_HIGH || DRD_DEPCMD_CMD(cmd) == DRD_DEPCMD_ENDTRANSFER) {
        reg = USB3DRD_GUSB2PHYCFG(0);
        if (reg & DRD_GUSB2PHYCFG_SUSPHY) {
            savedConfig |= DRD_GUSB2PHYCFG_SUSPHY;
            reg &= ~DRD_GUSB2PHYCFG_SUSPHY;
        }

        if (reg & DRD_GUSB2PHYCFG_ENBLSLPM) {
            savedConfig |= DRD_GUSB2PHYCFG_ENBLSLPM;
            reg &= ~DRD_GUSB2PHYCFG_ENBLSLPM;
        }

        if (savedConfig) {
            USB3DRD_GUSB2PHYCFG(0) = reg;
        }
    }

    if (DRD_DEPCMD_CMD(cmd) == DRD_DEPCMD_STARTTRANSFER) {
        uint32_t linkState;

        linkState = DRD_GadgetGetLinkState(pDRD);
        switch (linkState) {
        case DRD_LINK_STATE_U2:
            if (pDRD->enumSpeed >= DRD_USB_SPEED_SUPER) {
                break;
            }
        /* fallthrough */
        case DRD_LINK_STATE_U3:
            DRD_GadgetWakeup(pDRD);
            break;
        }
    }

    /*
     * For some commands such as Update Transfer command, DEPCMDPARn
     * registers are reserved. Since the driver often sends Update
     * Transfer command, don't write to DEPCMDPARn to avoid register
     * write delays and improve performance.
     */
    if (DRD_DEPCMD_CMD(cmd) != DRD_DEPCMD_UPDATETRANSFER) {
        USB3DRD_DRD_DEPCMDPAR0(dep->number) = params->param0;
        USB3DRD_DRD_DEPCMDPAR1(dep->number) = params->param1;
        USB3DRD_DRD_DEPCMDPAR2(dep->number) = params->param2;
    }

    if (DRD_DEPCMD_CMD(cmd) == DRD_DEPCMD_UPDATETRANSFER && dep->type != DRD_USB_ENDPOINT_XFER_ISOC) {
        cmd &= ~(DRD_DEPCMD_CMDIOC | DRD_DEPCMD_CMDACT);
    } else {
        cmd |= DRD_DEPCMD_CMDACT;
    }

    USB3DRD_DRD_DEPCMD(dep->number) = cmd;

    if (!(cmd & DRD_DEPCMD_CMDACT) || (DRD_DEPCMD_CMD(cmd) == DRD_DEPCMD_ENDTRANSFER && !(cmd & DRD_DEPCMD_CMDIOC))) {
        ret = HAL_OK;
        goto skipStatus;
    }

    do {
        reg = USB3DRD_DRD_DEPCMD(dep->number);
        if (!(reg & DRD_DEPCMD_CMDACT)) {
            cmdStatus = DRD_DEPCMD_STATUS(reg);

            switch (cmdStatus) {
            case 0:
                ret = HAL_OK;
                break;
            case DEPEVT_TRANSFER_NO_RESOURCE:
                HAL_DBG_ERR("DRD No resource for ep%d%s\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");
                ret = HAL_INVAL;
                break;
            case DEPEVT_TRANSFER_BUS_EXPIRY:
                ret = HAL_BUSY;
                break;
            default:
                HAL_DBG("DRD UNKNOWN cmd status\n");
            }

            break;
        }
    } while (--timeout);

    if (timeout == 0) {
        ret = HAL_TIMEOUT;
    }

skipStatus:
    if (DRD_DEPCMD_CMD(cmd) == DRD_DEPCMD_STARTTRANSFER) {
        if (ret == HAL_OK) {
            dep->flags |= DRD_EP_TRANSFER_STARTED;
        }

        if (ret != HAL_TIMEOUT) {
            DRD_GadgetEpGetTransferIndex(pDRD, dep);
        }
    }

    if (DRD_DEPCMD_CMD(cmd) == DRD_DEPCMD_ENDTRANSFER && !(cmd & DRD_DEPCMD_CMDIOC)) {
        HAL_DelayMs(1);
    }

    if (savedConfig) {
        reg = USB3DRD_GUSB2PHYCFG(0);
        reg |= savedConfig;
        USB3DRD_GUSB2PHYCFG(0) = reg;
    }

    return ret;
}

static uintptr_t DRD_TrbDmaOffset(struct DRD_EP *dep, struct DRD_TRB *trb)
{
    uintptr_t offset = (uintptr_t)((char *)trb - (char *)dep->trbPool);

    return dep->trbPoolDma + offset;
}

static HAL_Status DRD_GadgetSetXferResource(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;

    memset(&params, 0x00, sizeof(params));

    params.param0 = DRD_DEPXFERCFG_NUM_XFER_RES(1);

    return DRD_SendGadgetEpCmd(pDRD, dep, DRD_DEPCMD_SETTRANSFRESOURCE, &params);
}

static HAL_Status DRD_GadgetStartConfig(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;
    uint32_t cmd;
    uint8_t epNum;
    HAL_Status ret;

    /*
     * This function is called for each endpoint when it is
     * being enabled but is triggered only when called for
     * EP0-out, which always happens first, and which should
     * only happen in one of the above conditions.
     */
    if (dep->number) {
        return 0;
    }

    memset(&params, 0x00, sizeof(params));
    cmd = DRD_DEPCMD_DEPSTARTCFG;

    ret = DRD_SendGadgetEpCmd(pDRD, dep, cmd, &params);
    if (ret) {
        return ret;
    }

    for (epNum = 0; epNum < DRD_ENDPOINTS_NUM; epNum++) {
        struct DRD_EP *dep = &pDRD->eps[epNum];

        if (!dep->trbPool && (epNum > 1)) {
            continue;
        }

        ret = DRD_GadgetSetXferResource(pDRD, dep);
        if (ret) {
            return ret;
        }
    }

    return HAL_OK;
}

static HAL_Status DRD_StopActiveTransfer(struct USB_DRD_HANDLE *pDRD,
                                         struct DRD_EP *dep, bool force,
                                         bool interrupt)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;
    uint32_t cmd;
    HAL_Status ret = HAL_OK;

    /*
     * Only issue End Transfer command to the control endpoint
     * of a started Data Phase.
     */
    if (dep->number <= 1 && pDRD->ep0State != EP0_DATA_PHASE) {
        return ret;
    }

    if (interrupt && (dep->flags & DRD_EP_DELAY_STOP)) {
        return ret;
    }

    if (!(dep->flags & DRD_EP_TRANSFER_STARTED) || (dep->flags & DRD_EP_END_TRANSFER_PENDING)) {
        return ret;
    }

    /*
     *  If a Setup packet is received but yet to DMA out,
     *  the controller will not process the End Transfer
     *  command of any endpoint. Polling of DEPCMD.CmdAct
     *  may block setting up TRB for Setup packet, causing
     *  a timeout. Delay issuing the End Transfer command
     *  until the Setup TRB is prepared.
     */
    if (pDRD->ep0State != EP0_SETUP_PHASE) {
        dep->flags |= DRD_EP_DELAY_STOP;

        return ret;
    }

    cmd = DRD_DEPCMD_ENDTRANSFER;
    cmd |= force ? DRD_DEPCMD_HIPRI_FORCERM : 0;
    cmd |= interrupt ? DRD_DEPCMD_CMDIOC : 0;
    cmd |= DRD_DEPCMD_PARAM(dep->resourceIndex);
    memset(&params, 0x00, sizeof(params));
    ret = DRD_SendGadgetEpCmd(pDRD, dep, cmd, &params);
    if (ret == HAL_TIMEOUT && pDRD->ep0State != EP0_SETUP_PHASE) {
        dep->flags |= DRD_EP_DELAY_STOP;

        return HAL_OK;
    }

    dep->resourceIndex = 0;
    if (!interrupt) {
        dep->flags &= ~DRD_EP_TRANSFER_STARTED;
    } else {
        dep->flags |= DRD_EP_END_TRANSFER_PENDING;
    }

    dep->flags &= ~DRD_EP_DELAY_STOP;

    return ret;
}

static void DRD_StopActiveTransfers(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EP *dep;
    uint32_t epNum;

    for (epNum = 2; epNum < DRD_ENDPOINTS_NUM; epNum++) {
        dep = &pDRD->eps[epNum];
        if (!dep->trbPool) {
            continue;
        }

        if (!(dep->flags & DRD_EP_ENABLED)) {
            continue;
        }

        DRD_StopActiveTransfer(pDRD, dep, true, false);
    }
}

static HAL_Status DRD_GadgetSetEpConfig(struct USB_DRD_HANDLE *pDRD,
                                        struct DRD_EP *dep, uint32_t action)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;

    memset(&params, 0x00, sizeof(params));

    params.param0 = DRD_DEPCFG_EP_TYPE(dep->type) | DRD_DEPCFG_MAX_PACKET_SIZE(dep->maxPacket);

    /* Burst size is only needed in SuperSpeed mode */
    if (pDRD->enumSpeed >= DRD_USB_SPEED_SUPER) {
        uint32_t burst = dep->maxBurst;

        params.param0 |= DRD_DEPCFG_BURST_SIZE(burst - 1);
    }

    params.param0 |= action;

    if (dep->type == DRD_USB_ENDPOINT_XFER_CONTROL) {
        params.param1 = DRD_DEPCFG_XFER_COMPLETE_EN;
    }

    if (dep->number <= 1 || (dep->type == DRD_USB_ENDPOINT_XFER_ISOC)) {
        params.param1 |= DRD_DEPCFG_XFER_NOT_READY_EN;
    }

    if (dep->type != DRD_USB_ENDPOINT_XFER_CONTROL) {
        params.param1 |= DRD_DEPCFG_XFER_IN_PROGRESS_EN;
    }

    /*
     * We are doing 1:1 mapping for endpoints, meaning
     * Physical Endpoints 2 maps to Logical Endpoint 2 and
     * so on. We consider the direction bit as part of the physical
     * endpoint number. So USB endpoint 0x81 is 0x03.
     */
    params.param1 |= DRD_DEPCFG_EP_NUMBER(dep->number);

    /*
     * We must use the lower 16 TX FIFOs even though
     * HW might have more
     */
    if (dep->direction) {
        params.param0 |= DRD_DEPCFG_FIFO_NUMBER(dep->number >> 1);
    }

    if (dep->epInfo.bInterval) {
        uint8_t bIntervalM1;

        bIntervalM1 = HAL_MIN(dep->epInfo.bInterval - 1, 13);

        if (dep->type == DRD_USB_ENDPOINT_XFER_INT && pDRD->enumSpeed == DRD_USB_SPEED_FULL) {
            dep->interval = dep->epInfo.bInterval;
        } else {
            dep->interval = 1 << (dep->epInfo.bInterval - 1);
        }

        params.param1 |= DRD_DEPCFG_BINTERVAL_M1(bIntervalM1);
    }

    return DRD_SendGadgetEpCmd(pDRD, dep, DRD_DEPCMD_SETEPCONFIG, &params);
}

static HAL_Status DRD_GadgetEpEnable(struct USB_DRD_HANDLE *pDRD,
                                     struct DRD_EP *dep, uint32_t action)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;
    HAL_Status ret;

    if (!(dep->flags & DRD_EP_ENABLED)) {
        ret = DRD_GadgetResizeTxFifos(pDRD, dep);
        if (ret) {
            return ret;
        }

        ret = DRD_GadgetStartConfig(pDRD, dep);
        if (ret) {
            return ret;
        }
    }

    ret = DRD_GadgetSetEpConfig(pDRD, dep, action);
    if (ret) {
        return ret;
    }

    if (!(dep->flags & DRD_EP_ENABLED)) {
        struct DRD_TRB *trbStHw;
        struct DRD_TRB *trbLink;

        dep->flags |= DRD_EP_ENABLED;

        reg = pUSB3DRD->DALEPENA;
        reg |= DRD_DALEPENA_EP(dep->number);
        pUSB3DRD->DALEPENA = reg;

        dep->trbDequeue = 0;
        dep->trbEnqueue = 0;

        if (dep->type == DRD_USB_ENDPOINT_XFER_CONTROL) {
            goto out;
        }

        /* Initialize the TRB ring */
        memset(dep->trbPool, 0x00, sizeof(struct DRD_TRB) * DRD_TRB_NUM);

        /* Link TRB. The HWO bit is never reset */
        trbStHw = &dep->trbPool[0];

        trbLink = &dep->trbPool[DRD_TRB_NUM - 1];
        trbLink->bpl = HAL_LOWER_32_BITS(DRD_TrbDmaOffset(dep, trbStHw));
        trbLink->bph = HAL_UPPER_32_BITS(DRD_TrbDmaOffset(dep, trbStHw));
        trbLink->ctrl |= DRD_TRBCTL_LINK_TRB;
        trbLink->ctrl |= DRD_TRB_CTRL_HWO;
        HAL_DCACHE_CleanByRange((uintptr_t)trbStHw, sizeof(struct DRD_TRB) * DRD_TRB_NUM);
    }

    /*
     * Issue StartTransfer here with no-op TRB so we can
     * always rely on No Response Update Transfer command.
     */
    if (dep->type == DRD_USB_ENDPOINT_XFER_BULK || dep->type == DRD_USB_ENDPOINT_XFER_INT) {
        struct DRD_GADGET_EP_CMD_PARAMS params;
        struct DRD_TRB *trb;
        uintptr_t trbDma;
        uint32_t cmd;

        memset(&params, 0x00, sizeof(params));
        trb = &dep->trbPool[0];
        trbDma = DRD_TrbDmaOffset(dep, trb);

        params.param0 = HAL_UPPER_32_BITS(trbDma);
        params.param1 = HAL_LOWER_32_BITS(trbDma);

        cmd = DRD_DEPCMD_STARTTRANSFER;
        ret = DRD_SendGadgetEpCmd(pDRD, dep, cmd, &params);
        if (ret) {
            return ret;
        }
    }
out:

    return HAL_OK;
}

static HAL_Status DRD_GadgetEpDisable(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;
    uint32_t mask;

    /* make sure HW endpoint isn't stalled */
    if (dep->flags & DRD_EP_STALL) {
        DRD_GadgetEpSetHalt(pDRD, dep, 1, false);
    }

    reg = pUSB3DRD->DALEPENA;
    reg &= ~DRD_DALEPENA_EP(dep->number);
    pUSB3DRD->DALEPENA = reg;

    DRD_StopActiveTransfer(pDRD, dep, true, false);

    dep->streamCapable = false;
    dep->type = 0;
    mask = DRD_EP_TXFIFO_RESIZED;

    /*
     * Do not clear DEP flags, so that the end transfer command will
     * be reattempted during the next SETUP stage.
     */
    if (dep->flags & DRD_EP_DELAY_STOP) {
        mask |= (DRD_EP_DELAY_STOP | DRD_EP_TRANSFER_STARTED);
    }
    dep->flags &= mask;

    /* Clear out the ep descriptors for non-ep0 */
    if (dep->number > 1) {
        memset(&dep->epInfo, 0x00, sizeof(struct DRD_EP_INFO));
    }

    return HAL_OK;
}

static HAL_Status DRD_GadgetEpSetHalt(struct USB_DRD_HANDLE *pDRD,
                                      struct DRD_EP *dep, uint32_t value,
                                      uint32_t protocol)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;
    HAL_Status ret;

    if (dep->type == DRD_USB_ENDPOINT_XFER_ISOC) {
        return HAL_INVAL;
    }

    memset(&params, 0x00, sizeof(params));

    if (value) {
        struct DRD_TRB *trb;
        unsigned int transferInFlight;

        if (dep->number > 1) {
            trb = DRD_EpPrevTrb(dep, dep->trbEnqueue);
        } else {
            trb = &pDRD->ep0Trb[dep->trbEnqueue];
        }

        HAL_DCACHE_InvalidateByRange((uintptr_t)trb, sizeof(struct DRD_TRB));

        transferInFlight = trb->ctrl & DRD_TRB_CTRL_HWO;
        if (!protocol && (dep->direction && transferInFlight)) {
            HAL_DBG_ERR("DRD in flight ep%d%s cannot halt\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");

            return HAL_BUSY;
        }

        ret = DRD_SendGadgetEpCmd(pDRD, dep, DRD_DEPCMD_SETSTALL, &params);
        if (ret) {
            HAL_DBG_ERR("DRD failed to set STALL ep%d%s\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");
        } else {
            dep->flags |= DRD_EP_STALL;
        }
    } else {
        /*
         * Don't issue CLEAR_STALL command to control endpoints.
         * The controller automatically clears the STALL when it
         * receives the SETUP token.
         */
        if (dep->number <= 1) {
            dep->flags &= ~(DRD_EP_STALL | DRD_EP_WEDGE);

            return HAL_OK;
        }

        DRD_StopActiveTransfer(pDRD, dep, true, true);

        ret = DRD_SendClearStallEpCmd(pDRD, dep);
        if (ret) {
            HAL_DBG_ERR("DRD failed to clear STALL ep%d%s\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");

            return HAL_ERROR;
        }

        dep->flags &= ~(DRD_EP_STALL | DRD_EP_WEDGE);
    }

    return ret;
}

static void DRD_GadgetEnableIrq(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    uint32_t reg;

    /* Enable all but Start and End of Frame IRQs */
    reg = (DRD_DEVTEN_EVNTOVERFLOWEN |
           DRD_DEVTEN_CMDCMPLTEN |
           DRD_DEVTEN_ERRTICERREN |
           DRD_DEVTEN_WKUPEVTEN |
           DRD_DEVTEN_CONNECTDONEEN |
           DRD_DEVTEN_USBRSTEN |
           DRD_DEVTEN_DISCONNEVTEN);

    if (DRD_VER_IS_PRIOR(DRD, 250A)) {
        reg |= DRD_DEVTEN_ULSTCNGEN;
    }

    if (!DRD_VER_IS_PRIOR(DRD, 230A)) {
        reg |= DRD_DEVTEN_U3L2L1SUSPEN;
    }

    pUSB3DRD->DEVTEN = reg;
}

static void DRD_GadgetDisableIrq(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;

    /* mask all interrupts */
    pUSB3DRD->DEVTEN = 0x00;
}

static HAL_Status DRD_GadgetRunStop(struct USB_DRD_HANDLE *pDRD, bool isOn)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    uint32_t reg;
    uint32_t timeout = 2000;

    if (pDRD->pullupsConnected == isOn) {
        return HAL_OK;
    }

    reg = pUSB3DRD->DCTL;
    if (isOn) {
        if (DRD_VER_IS_WITHIN(DRD, ANY, 187A)) {
            reg &= ~DRD_DCTL_TRGTULST_MASK;
            reg |= DRD_DCTL_TRGTULST_RX_DET;
        }

        if (!DRD_VER_IS_PRIOR(DRD, 194A)) {
            reg &= ~DRD_DCTL_KEEP_CONNECT;
        }

        reg |= DRD_DCTL_RUN_STOP;

        pDRD->pullupsConnected = true;
    } else {
        reg &= ~DRD_DCTL_RUN_STOP;

        pDRD->pullupsConnected = false;
    }

    reg &= ~DRD_DCTL_ULSTCHNGREQ_MASK;
    pUSB3DRD->DCTL = reg;

    do {
        HAL_DelayUs(1);
        reg = pUSB3DRD->DSTS;
        reg &= DRD_DSTS_DEVCTRLHLT;
    } while (--timeout && !(!isOn ^ !reg));

    if (!timeout) {
        return HAL_TIMEOUT;
    }

    return HAL_OK;
}

static HAL_Status DRD_GadgetStart(struct USB_DRD_HANDLE *pDRD)
{
    struct DRD_EP *dep;
    HAL_Status ret;

    DRD_GadgetSetSpeed(pDRD);

    dep = &pDRD->eps[0];
    ret = DRD_GadgetEpEnable(pDRD, dep, DRD_DEPCFG_ACTION_INIT);
    if (ret) {
        HAL_DBG_ERR("DRD failed to enable ep0out\n");

        goto err0;
    }

    dep = &pDRD->eps[1];
    ret = DRD_GadgetEpEnable(pDRD, dep, DRD_DEPCFG_ACTION_INIT);
    if (ret) {
        HAL_DBG_ERR("DRD failed to enable ep0in\n");

        goto err1;
    }

    /* begin to receive SETUP packets */
    pDRD->ep0State = EP0_SETUP_PHASE;
    pDRD->ep0Bounced = false;
    pDRD->linkState = DRD_LINK_STATE_SS_DIS;
    DRD_Ep0OutStart(pDRD);

    DRD_GadgetEnableIrq(pDRD);
    HAL_DRD_EnableSusphy(pDRD->pReg, &pDRD->cfg, true);

    return HAL_OK;

err1:
    DRD_GadgetEpDisable(pDRD, &pDRD->eps[0]);

err0:

    return ret;
}

static void DRD_GadgetStop(struct USB_DRD_HANDLE *pDRD)
{
    if (!pDRD->pullupsConnected) {
        return;
    }

    pDRD->connected = false;

    DRD_StopActiveTransfers(pDRD);

    if (pDRD->ep0State != EP0_SETUP_PHASE && pDRD->ep0State != EP0_UNCONNECTED) {
        HAL_DBG_WRN("Stop gadget while ep0 still in process\n");
        DRD_Ep0ResetState(pDRD);
    }

    DRD_GadgetRunStop(pDRD, false);

    DRD_GadgetDisableIrq(pDRD);
    DRD_GadgetEpDisable(pDRD, &pDRD->eps[0]);
    DRD_GadgetEpDisable(pDRD, &pDRD->eps[1]);
}

static HAL_Status DRD_GadgetInit(struct USB_DRD_HANDLE *pDRD)
{
    HAL_Status ret;

    pDRD->ep0Trb = HAL_DRD_MemMallocAlign(sizeof(struct DRD_TRB) * DRD_EP0_TRB_NUM, DRD_DMA_ALIGN_SIZE);
    if (pDRD->ep0Trb == NULL) {
        HAL_DBG_ERR("DRD failed to allocate ep0 trb\n");
        ret = HAL_ERROR;
        goto err0;
    }
    pDRD->ep0TrbAddr = HAL_CpuAddrToDmaAddr((uintptr_t)pDRD->ep0Trb);

    pDRD->setupBuf = HAL_DRD_MemMallocAlign(DRD_EP0_SETUP_SIZE, DRD_DMA_ALIGN_SIZE);
    if (pDRD->setupBuf == NULL) {
        HAL_DBG_ERR("DRD failed to allocate setup buffer\n");
        ret = HAL_ERROR;
        goto err1;
    }

    pDRD->bounce = HAL_DRD_MemMallocAlign(DRD_BOUNCE_SIZE, DRD_DMA_ALIGN_SIZE);
    if (pDRD->bounce == NULL) {
        HAL_DBG_ERR("DRD failed to allocate bounce buffer\n");
        ret = HAL_ERROR;
        goto err2;
    }
    pDRD->bounceAddr = HAL_CpuAddrToDmaAddr((uintptr_t)pDRD->bounce);

    pDRD->enumSpeed = DRD_USB_SPEED_UNKNOWN;

    memset(pDRD->eps, 0x00, sizeof(pDRD->eps));

    ret = DRD_GadgetInitEndpoints(pDRD);
    if (ret) {
        goto err3;
    }

    return HAL_OK;

err3:
    DRD_GadgetFreeEndpoints(pDRD);
    HAL_DRD_MemFreeAlign(pDRD->bounce);

err2:
    HAL_DRD_MemFreeAlign(pDRD->setupBuf);

err1:
    HAL_DRD_MemFreeAlign(pDRD->ep0Trb);

err0:

    return ret;
}

static HAL_Status DRD_GadgetExit(struct USB_DRD_HANDLE *pDRD)
{
    HAL_DRD_EnableSusphy(pDRD->pReg, &pDRD->cfg, false);

    DRD_GadgetFreeEndpoints(pDRD);

    HAL_DRD_MemFreeAlign(pDRD->bounce);

    HAL_DRD_MemFreeAlign(pDRD->setupBuf);

    HAL_DRD_MemFreeAlign(pDRD->ep0Trb);

    return HAL_OK;
}

static void DRD_ClearStallAllEp(struct USB_DRD_HANDLE *pDRD)
{
    uint32_t epNum;

    for (epNum = 1; epNum < DRD_ENDPOINTS_NUM; epNum++) {
        struct DRD_EP *dep;

        dep = &pDRD->eps[epNum];

        if (!dep->trbPool && (epNum > 1)) {
            continue;
        }

        if (!(dep->flags & DRD_EP_STALL)) {
            continue;
        }

        dep->flags &= ~DRD_EP_STALL;

        DRD_SendClearStallEpCmd(pDRD, dep);
    }
}

static uint32_t DRD_CalcTrbsLeft(struct DRD_EP *dep)
{
    uint8_t trbsLeft;

    /*
     * If the enqueue & dequeue are equal then the TRB ring is either full
     * or empty. It's considered full when there are DRD_TRB_NUM-1 of TRBs
     * pending to be processed by the driver.
     */
    if (dep->trbEnqueue == dep->trbDequeue) {
        /*
         * If there is any request remained at this point,
         * that means there is no TRB available.
         */

        /* TODO check TRB full */
        return DRD_TRB_NUM - 1;
    }

    trbsLeft = dep->trbDequeue - dep->trbEnqueue;
    trbsLeft &= (DRD_TRB_NUM - 1);

    if (dep->trbDequeue < dep->trbEnqueue) {
        trbsLeft--;
    }

    return trbsLeft;
}

static void DRD_PrepareOneTrb(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep,
                              uint32_t trbLength, uint32_t chain, uint32_t node,
                              bool useBounceBuffer, bool mustInterrupt)
{
    struct DRD_TRB *trb;
    uintptr_t dma;

    if (useBounceBuffer) {
        dma = pDRD->bounceAddr;
    } else {
        dma = dep->dmaAddr;
    }

    trb = &dep->trbPool[dep->trbEnqueue];
    dep->trbDma = DRD_TrbDmaOffset(dep, trb);
    dep->numTrbs++;

    trb->size = DRD_TRB_SIZE_LENGTH(trbLength);
    trb->bpl = HAL_LOWER_32_BITS(dma);
    trb->bph = HAL_UPPER_32_BITS(dma);

    switch (dep->type) {
    case DRD_USB_ENDPOINT_XFER_CONTROL:
        trb->ctrl = DRD_TRBCTL_CONTROL_SETUP;
        break;
    case DRD_USB_ENDPOINT_XFER_ISOC:
        if (!node) {
            trb->ctrl = DRD_TRBCTL_ISOCHRONOUS_FIRST;

            if (pDRD->enumSpeed == DRD_USB_SPEED_HIGH) {
                uint8_t mult = 2;
                uint32_t maxp = dep->maxPacket;

                if (dep->xferLen <= (2 * maxp)) {
                    mult--;
                }

                if (dep->xferLen <= maxp) {
                    mult--;
                }

                trb->size |= DRD_TRB_SIZE_PCM1(mult);
            }
        } else {
            trb->ctrl = DRD_TRBCTL_ISOCHRONOUS;
        }

        if (!chain) {
            trb->ctrl |= DRD_TRB_CTRL_ISP_IMI;
        }
        break;
    case DRD_USB_ENDPOINT_XFER_BULK:
    case DRD_USB_ENDPOINT_XFER_INT:
        trb->ctrl = DRD_TRBCTL_NORMAL;
        break;
    default:
        HAL_DBG_WRN("DRD unknown endpoint type %d\n", dep->type);
    }

    /*
     * Enable Continue on Short Packet when
     * endpoint out is not a stream capable.
     */
    if (!dep->direction) {
        if (!dep->streamCapable) {
            trb->ctrl |= DRD_TRB_CTRL_CSP;
        }
    }

    if (!chain || mustInterrupt) {
        trb->ctrl |= DRD_TRB_CTRL_IOC;
    }

    if (chain) {
        trb->ctrl |= DRD_TRB_CTRL_CHN;
    }

    trb->ctrl |= DRD_TRB_CTRL_HWO;

    HAL_DCACHE_CleanByRange((uintptr_t)trb, sizeof(struct DRD_TRB));
    if (dep->direction) {
        HAL_DCACHE_CleanByRange((uintptr_t)dma, trbLength);
    }

    DRD_EpIncEnq(dep);
}

static bool DRD_NeedsExtraTrb(struct DRD_EP *dep)
{
    uint32_t rem = dep->xferLen % dep->maxPacket;

    if ((dep->xferLen && dep->zero && !rem && (dep->type != DRD_USB_ENDPOINT_XFER_ISOC)) || (!dep->direction && rem)) {
        return true;
    }

    return false;
}

static int32_t DRD_PrepareTrbs(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    uint32_t maxp = dep->maxPacket;
    uint32_t rem = dep->xferLen % maxp;
    int32_t numTrbs = 1;

    if (DRD_NeedsExtraTrb(dep)) {
        numTrbs++;
    }

    if (DRD_CalcTrbsLeft(dep) < numTrbs) {
        return 0;
    }

    dep->needsExtraTrb = numTrbs > 1;

    /* Prepare a normal TRB */
    if (dep->direction || dep->xferLen) {
        DRD_PrepareOneTrb(pDRD, dep, dep->xferLen, dep->needsExtraTrb, 0, false, false);
    }

    /* Prepare extra TRBs for ZLP and MPS OUT transfer alignment */
    if ((!dep->direction && !dep->xferLen) || dep->needsExtraTrb) {
        DRD_PrepareOneTrb(pDRD, dep, dep->direction ? 0 : maxp - rem, false, 1, true, false);
    }

    return numTrbs;
}

static void DRD_GadgetEpSkipTrbs(struct DRD_EP *dep)
{
    struct DRD_TRB *trb, *trbTemp;
    uint32_t i;

    trbTemp = &dep->trbPool[dep->trbDequeue];

    for (i = 0; i < dep->numTrbs; i++) {
        trb = &dep->trbPool[dep->trbDequeue];
        trb->ctrl &= ~DRD_TRB_CTRL_HWO;

        HAL_DCACHE_CleanByRange((uintptr_t)trb, sizeof(struct DRD_TRB));
        DRD_EpIncDeq(dep);
    }

    if (dep->numTrbs) {
        HAL_DCACHE_CleanByRange((uintptr_t)trbTemp, sizeof(struct DRD_TRB) * dep->numTrbs);
        dep->numTrbs = 0;
    }
}

static HAL_Status DRD_GadgetKickTransfer(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct DRD_GADGET_EP_CMD_PARAMS params;
    uint32_t starting;
    uint32_t cmd;
    int32_t numTrbs;
    HAL_Status ret;

    numTrbs = DRD_PrepareTrbs(pDRD, dep);
    if (numTrbs < 0) {
        HAL_DBG_ERR("DRD failed to prepare trbs for ep%d%s\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");

        return HAL_ERROR;
    }

    /*
     * If there's no new TRB prepared and we don't need to restart
     * a transfer, there's no need to update the transfer.
     */
    starting = !(dep->flags & DRD_EP_TRANSFER_STARTED);
    if (!numTrbs && !starting) {
        return HAL_OK;
    }

    if (!dep->pxferBuff || !(dep->flags & DRD_EP_TRANSFER_QUEUED)) {
        dep->flags |= DRD_EP_PENDING_REQUEST;

        return HAL_OK;
    }

    if (dep->direction && dep->xferLen > 0) {
        HAL_DCACHE_CleanByRange((uintptr_t)dep->pxferBuff, dep->xferLen);
    }

    dep->flags &= ~DRD_EP_TRANSFER_QUEUED;
    memset(&params, 0, sizeof(params));

    if (starting) {
        params.param0 = HAL_UPPER_32_BITS(dep->trbDma);
        params.param1 = HAL_LOWER_32_BITS(dep->trbDma);
        cmd = DRD_DEPCMD_STARTTRANSFER;

        if (dep->type == DRD_USB_ENDPOINT_XFER_ISOC) {
            cmd |= DRD_DEPCMD_PARAM(dep->frameNumber);
        }
    } else {
        cmd = DRD_DEPCMD_UPDATETRANSFER | DRD_DEPCMD_PARAM(dep->resourceIndex);
    }

    ret = DRD_SendGadgetEpCmd(pDRD, dep, cmd, &params);
    if (ret) {
        if (ret == HAL_BUSY) {
            return ret;
        }

        DRD_StopActiveTransfer(pDRD, dep, true, true);

        return ret;
    }

    return ret;
}

static HAL_Status DRD_GadgetStartIsocQuirk(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    HAL_Status ret = HAL_OK;

    /* TODO */

    return ret;
}

static HAL_Status DRD_GadgetStartIsoc(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    uint32_t i;
    HAL_Status ret = HAL_OK;

    if (DRD_VER_IS_PRIOR(DRD1, 170A)) {
        if (pDRD->enumSpeed <= DRD_USB_SPEED_HIGH && dep->direction) {
            return DRD_GadgetStartIsocQuirk(pDRD, dep);
        }
    }

    if (dep->epInfo.bInterval <= 14 && pDRD->enumSpeed >= DRD_USB_SPEED_HIGH) {
        uint32_t frame = DRD_GadgetGetFrame(pDRD);
        bool rollover = frame < (dep->frameNumber & DRD_FRNUMBER_MASK);

        dep->frameNumber = (dep->frameNumber & ~DRD_FRNUMBER_MASK) | frame;
        if (rollover) {
            dep->frameNumber += HAL_BIT(14);
        }
    }

    for (i = 0; i < DRD_ISOC_MAX_RETRIES; i++) {
        uint32_t futureInterval = i + 1;

        /* Give the controller at least 500us to schedule transfers */
        if (dep->epInfo.bInterval < 3) {
            futureInterval += 3 - dep->epInfo.bInterval;
        }

        dep->frameNumber = DRD_ALIGN_FRAME(dep, futureInterval);

        ret = DRD_GadgetKickTransfer(pDRD, dep);
        if (ret != HAL_BUSY) {
            break;
        }
    }

    /*
     * After a number of unsuccessful start attempts due to
     * bus-expiry status, issue END_TRANSFER command and retry
     * on the next XferNotReady event.
     */
    if (ret == HAL_BUSY) {
        ret = DRD_StopActiveTransfer(pDRD, dep, false, true);
        if (ret) {
            dep->flags &= ~DRD_EP_END_TRANSFER_PENDING;
        }
    }

    return ret;
}

static HAL_Status DRD_GadgetEPStartXfer(struct USB_DRD_HANDLE *pDRD, struct DRD_EP *dep)
{
    if (!pDRD->pullupsConnected || !pDRD->connected) {
        HAL_DBG("DRD can't start xfer for disabled endpoint\n");

        return HAL_ERROR;
    }

    /*
     * Start the transfer only after the END_TRANSFER is
     * completed and endpoint STALL is cleared.
     */
    if ((dep->flags & DRD_EP_END_TRANSFER_PENDING) || (dep->flags & DRD_EP_DELAY_STOP) || (dep->flags & DRD_EP_STALL)) {
        dep->flags |= DRD_EP_DELAY_START;

        return HAL_OK;
    }

    /*
     * NOTICE: Isochronous endpoints should NEVER be prestarted. We must
     * wait for a XferNotReady event so we will know what's the current
     * (micro-)frame number.
     *
     * Without this trick, we are very, very likely gonna get Bus Expiry
     * errors which will force us issue EndTransfer command.
     */
    if (dep->type == DRD_USB_ENDPOINT_XFER_ISOC) {
        if (!(dep->flags & DRD_EP_TRANSFER_STARTED)) {
            if ((dep->flags & DRD_EP_PENDING_REQUEST)) {
                return DRD_GadgetStartIsoc(pDRD, dep);
            }

            return HAL_OK;
        }
    }

    DRD_GadgetKickTransfer(pDRD, dep);

    return HAL_OK;
}

static void DRD_GadgetEndpointFrameFromEvent(struct DRD_EP *dep,
                                             const struct DRD_EVENT_DEPEVT *event)
{
    dep->frameNumber = event->parameters;
}

static HAL_Status DRD_GadgetEpReclaimCompletedTrb(struct USB_DRD_HANDLE *pDRD,
                                                  struct DRD_EP *dep, struct DRD_TRB *trb,
                                                  const struct DRD_EVENT_DEPEVT *event,
                                                  HAL_Status status, uint32_t chain)
{
    uint32_t count;
    uint32_t transferred = 0;

    DRD_EpIncDeq(dep);
    dep->numTrbs--;

    if (chain && (trb->ctrl & DRD_TRB_CTRL_HWO)) {
        trb->ctrl &= ~DRD_TRB_CTRL_HWO;
    }

    if (dep->type == DRD_USB_ENDPOINT_XFER_ISOC && (trb->ctrl & DRD_TRBCTL_ISOCHRONOUS_FIRST)) {
        uint32_t frameNumber;

        frameNumber = DRD_TRB_CTRL_GET_SID_SOFN(trb->ctrl);
        frameNumber &= ~(dep->interval - 1);
        dep->frameNumber = frameNumber;
    }

    /*
     * We use bounce buffer for requests that needs extra TRB or OUT ZLP.
     * If this TRB points to the bounce buffer address, it's a MPS
     * alignment TRB. Don't add it to req->remaining calculation.
     */
    if (trb->bpl == HAL_LOWER_32_BITS(pDRD->bounceAddr) && trb->bph == HAL_UPPER_32_BITS(pDRD->bounceAddr)) {
        trb->ctrl &= ~DRD_TRB_CTRL_HWO;

        goto out;
    }

    count = trb->size & DRD_TRB_SIZE_MASK;
    transferred = dep->xferLen - count;
    dep->actualLen += transferred;

    if ((trb->ctrl & DRD_TRB_CTRL_HWO) && status == HAL_OK) {
        HAL_DBG_WRN("DRD ep%d%s TRB still owned by HW\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");
    }

out:
    HAL_DCACHE_CleanByRange((uintptr_t)trb, sizeof(struct DRD_TRB));

    return HAL_OK;
}

static HAL_Status DRD_GadgetEpReclaimTrbLinear(struct USB_DRD_HANDLE *pDRD,
                                               struct DRD_EP *dep,
                                               const struct DRD_EVENT_DEPEVT *event,
                                               HAL_Status status)
{
    struct DRD_TRB *trb = &dep->trbPool[dep->trbDequeue];

    HAL_DCACHE_InvalidateByRange((uintptr_t)trb, sizeof(struct DRD_TRB));

    return DRD_GadgetEpReclaimCompletedTrb(pDRD, dep, trb, event, status, false);
}

static HAL_Status DRD_GadgetEpCleanupCompletedRequest(struct USB_DRD_HANDLE *pDRD,
                                                      struct DRD_EP *dep,
                                                      const struct DRD_EVENT_DEPEVT *event,
                                                      HAL_Status status)
{
    struct DRD_TRB *trb;
    uint8_t epAddr;
    HAL_Status ret;

    ret = DRD_GadgetEpReclaimTrbLinear(pDRD, dep, event, status);

    if (dep->needsExtraTrb) {
        ret = DRD_GadgetEpReclaimTrbLinear(pDRD, dep, event, status);
        dep->needsExtraTrb = false;
    }

    /* Process for MISS ISOC */
    if (dep->type == DRD_USB_ENDPOINT_XFER_ISOC && status == HAL_BUSY) {
        dep->needsExtraTrb = false;
        trb = &dep->trbPool[dep->trbEnqueue];
        trb->ctrl &= ~DRD_TRB_CTRL_HWO;

        HAL_DCACHE_CleanByRange((uintptr_t)trb, sizeof(struct DRD_TRB));

        goto out;
    }

    epAddr = dep->epInfo.bEndpointAddress;
    if (dep->direction) {
        HAL_DRD_DataInStageCallback(pDRD, epAddr, dep->actualLen);
    } else {
        if (dep->actualLen) {
            HAL_DCACHE_InvalidateByRange((uintptr_t)dep->pxferBuff, dep->actualLen);
        }
        HAL_DRD_DataOutStageCallback(pDRD, epAddr, dep->actualLen);
    }

out:

    return ret;
}

static bool DRD_GadgetEndpointTrbsComplete(struct USB_DRD_HANDLE *pDRD,
                                           struct DRD_EP *dep,
                                           const struct DRD_EVENT_DEPEVT *event,
                                           HAL_Status status)
{
    bool noStartedTrb = true;

    DRD_GadgetEpCleanupCompletedRequest(pDRD, dep, event, status);

    if (dep->flags & DRD_EP_END_TRANSFER_PENDING) {
        goto out;
    }

    /* TODO process for MISS ISOC */
    if (dep->type == DRD_USB_ENDPOINT_XFER_ISOC && status == HAL_BUSY) {
        DRD_StopActiveTransfer(pDRD, dep, true, true);
    }

out:

    return noStartedTrb;
}

static void DRD_GadgetEndpointTransferInProgress(struct USB_DRD_HANDLE *pDRD,
                                                 struct DRD_EP *dep,
                                                 const struct DRD_EVENT_DEPEVT *event)
{
    HAL_Status status;

    if (!dep->type) {
        return;
    }

    if (dep->type == DRD_USB_ENDPOINT_XFER_ISOC) {
        DRD_GadgetEndpointFrameFromEvent(dep, event);
    }

    if (event->status & DEPEVT_STATUS_BUSERR) {
        status = HAL_ERROR;
    }

    if (event->status & DEPEVT_STATUS_MISSED_ISOC) {
        status = HAL_BUSY;
    }

    DRD_GadgetEndpointTrbsComplete(pDRD, dep, event, status);
}

static void DRD_GadgetEndpointTransferComplete(struct USB_DRD_HANDLE *pDRD,
                                               struct DRD_EP *dep,
                                               const struct DRD_EVENT_DEPEVT *event)
{
    HAL_Status status;

    dep->flags &= ~DRD_EP_TRANSFER_STARTED;

    if (event->status & DEPEVT_STATUS_BUSERR) {
        status = HAL_ERROR;
    }

    if (DRD_GadgetEndpointTrbsComplete(pDRD, dep, event, status)) {
        dep->flags &= ~DRD_EP_WAIT_TRANSFER_COMPLETE;
    }
}

static void DRD_GadgetEndpointTransferNotReady(struct USB_DRD_HANDLE *pDRD,
                                               struct DRD_EP *dep,
                                               const struct DRD_EVENT_DEPEVT *event)
{
    DRD_GadgetEndpointFrameFromEvent(dep, event);

    /*
     * The XferNotReady event is generated only once before the endpoint
     * starts. It will be generated again when END_TRANSFER command is
     * issued. For some controller versions, the XferNotReady event may be
     * generated while the END_TRANSFER command is still in process. Ignore
     * it and wait for the next XferNotReady event after the command is
     * completed.
     */
    if (dep->flags & DRD_EP_END_TRANSFER_PENDING) {
        return;
    }

    DRD_GadgetStartIsoc(pDRD, dep);
}

static void DRD_GadgetEndpointCommandComplete(struct USB_DRD_HANDLE *pDRD,
                                              struct DRD_EP *dep,
                                              const struct DRD_EVENT_DEPEVT *event)
{
    uint8_t cmd = DEPEVT_PARAMETER_CMD(event->parameters);

    if (cmd != DRD_DEPCMD_ENDTRANSFER) {
        return;
    }

    /*
     * The END_TRANSFER command will cause the controller to generate a
     * NoStream Event, and it's not due to the host DP NoStream rejection.
     * Ignore the next NoStream event.
     */
    if (dep->streamCapable) {
        dep->flags |= DRD_EP_IGNORE_NEXT_NOSTREAM;
    }

    dep->flags &= ~DRD_EP_END_TRANSFER_PENDING;
    dep->flags &= ~DRD_EP_TRANSFER_STARTED;

    if (dep->flags & DRD_EP_PENDING_CLEAR_STALL) {
        dep->flags &= ~DRD_EP_PENDING_CLEAR_STALL;
        if (DRD_SendClearStallEpCmd(pDRD, dep)) {
            HAL_DBG_ERR("DRD failed to clear STALL ep%d%s\n", dep->number >> 1, (dep->number & 1) ? "in" : "out");

            return;
        }

        dep->flags &= ~(DRD_EP_STALL | DRD_EP_WEDGE);
    }

    if ((dep->flags & DRD_EP_DELAY_START) && (dep->type != DRD_USB_ENDPOINT_XFER_ISOC)) {
        DRD_GadgetKickTransfer(pDRD, dep);
    }

    dep->flags &= ~DRD_EP_DELAY_START;
}

static void DRD_GadgetEndpointStreamEvent(struct USB_DRD_HANDLE *pDRD,
                                          struct DRD_EP *dep,
                                          const struct DRD_EVENT_DEPEVT *event)
{
    /* TODO */
    dep->flags &= ~DRD_EP_IGNORE_NEXT_NOSTREAM;
}

static void DRD_EndpointInterrupt(struct USB_DRD_HANDLE *pDRD,
                                  const struct DRD_EVENT_DEPEVT *event)
{
    struct DRD_EP *dep;
    uint8_t epNum = event->endpointNumber;

    dep = &pDRD->eps[epNum];

    if (!(dep->flags & DRD_EP_ENABLED)) {
        if ((epNum > 1) && !(dep->flags & DRD_EP_TRANSFER_STARTED)) {
            return;
        }

        /* Handle only EPCMDCMPLT when EP disabled */
        if ((event->endpointEvent != DRD_DEPEVT_EPCMDCMPLT) &&
            !(epNum <= 1 && event->endpointEvent == DRD_DEPEVT_XFERCOMPLETE)) {
            return;
        }
    }

    if (epNum == 0 || epNum == 1) {
        DRD_Ep0Interrupt(pDRD, event);

        return;
    }

    /* Handle for the other EPs */
    switch (event->endpointEvent) {
    case DRD_DEPEVT_XFERINPROGRESS:
        DRD_GadgetEndpointTransferInProgress(pDRD, dep, event);
        break;
    case DRD_DEPEVT_XFERNOTREADY:
        DRD_GadgetEndpointTransferNotReady(pDRD, dep, event);
        break;
    case DRD_DEPEVT_EPCMDCMPLT:
        DRD_GadgetEndpointCommandComplete(pDRD, dep, event);
        break;
    case DRD_DEPEVT_XFERCOMPLETE:
        DRD_GadgetEndpointTransferComplete(pDRD, dep, event);
        break;
    case DRD_DEPEVT_STREAMEVT:
        DRD_GadgetEndpointStreamEvent(pDRD, dep, event);
        break;
    case DRD_DEPEVT_RXTXFIFOEVT:
        break;
    }
}

static void DRD_GadgetDisconnectInterrupt(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;

    pDRD->suspended = false;

    DRD_GadgetSetLinkState(pDRD, DRD_LINK_STATE_RX_DET);

    reg = pUSB3DRD->DCTL;
    reg &= ~DRD_DCTL_INITU1ENA;
    reg &= ~DRD_DCTL_INITU2ENA;
    reg &= ~DRD_DCTL_ULSTCHNGREQ_MASK;
    pUSB3DRD->DCTL = reg;

    pDRD->connected = false;
    pDRD->enumSpeed = DRD_USB_SPEED_UNKNOWN;
    pDRD->setupPacketPending = false;

    DRD_Ep0ResetState(pDRD);
}

static void DRD_GadgetResetInterrupt(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    uint32_t reg;

    pDRD->suspended = false;
    pDRD->connected = false;

    DRD_Ep0ResetState(pDRD);

    DRD_StopActiveTransfers(pDRD);
    pDRD->connected = true;

    DRD_ClearStallAllEp(pDRD);

    /* Reset device address to zero */
    reg = pUSB3DRD->DCFG;
    reg &= ~(DRD_DCFG_DEVADDR_MASK);
    pUSB3DRD->DCFG = reg;
}

static void DRD_GadgetConndoneInterrupt(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    struct DRD_EP *dep;
    uint32_t reg;
    uint8_t speed;
    HAL_Status ret;

    reg = pUSB3DRD->DSTS;
    speed = reg & DRD_DSTS_CONNECTSPD;

    switch (speed) {
    case DRD_DSTS_SUPERSPEED:
        if (DRD_VER_IS_PRIOR(DRD, 190A)) {
            DRD_GadgetResetInterrupt(pDRD);
        }

        pDRD->eps[0].maxPacket = 512;
        pDRD->eps[1].maxPacket = 512;
        pDRD->enumSpeed = DRD_USB_SPEED_SUPER;
        break;
    case DRD_DSTS_HIGHSPEED:
        pDRD->eps[0].maxPacket = 64;
        pDRD->eps[1].maxPacket = 64;
        pDRD->enumSpeed = DRD_USB_SPEED_HIGH;
        break;
    case DRD_DSTS_FULLSPEED:
        pDRD->eps[0].maxPacket = 64;
        pDRD->eps[1].maxPacket = 64;
        pDRD->enumSpeed = DRD_USB_SPEED_FULL;
        break;
    }

    HAL_SYSLOG("usb device is %s\n", DRD_SpeedString(pDRD->enumSpeed));

    /* Enable USB2 LPM Capability */
    if (!DRD_VER_IS_WITHIN(DRD, ANY, 194A) && !pCFG->u2lpmDisable && (speed != DRD_DSTS_SUPERSPEED)) {
        reg = pUSB3DRD->DCFG;
        reg |= DRD_DCFG_LPM_CAP;
        pUSB3DRD->DCFG = reg;

        reg = pUSB3DRD->DCTL;
        reg &= ~(DRD_DCTL_HIRD_THRES_MASK | DRD_DCTL_L1_HIBER_EN);
        reg |= DRD_DCTL_HIRD_THRES(pCFG->hirdThreshold | (pCFG->isUtmiL1Suspend << 4));
        reg &= ~DRD_DCTL_ULSTCHNGREQ_MASK;
        pUSB3DRD->DCTL = reg;
    } else {
        if (pCFG->u2lpmDisable) {
            reg = pUSB3DRD->DCFG;
            reg &= ~DRD_DCFG_LPM_CAP;
            pUSB3DRD->DCFG = reg;
        }

        reg = pUSB3DRD->DCTL;
        reg &= ~DRD_DCTL_HIRD_THRES_MASK;
        reg &= ~DRD_DCTL_ULSTCHNGREQ_MASK;
        pUSB3DRD->DCTL = reg;
    }

    dep = &pDRD->eps[0];
    ret = DRD_GadgetEpEnable(pDRD, dep, DRD_DEPCFG_ACTION_MODIFY);
    if (ret) {
        HAL_DBG_ERR("DRD failed to enable ep0out\n");

        return;
    }

    dep = &pDRD->eps[1];
    ret = DRD_GadgetEpEnable(pDRD, dep, DRD_DEPCFG_ACTION_MODIFY);
    if (ret) {
        HAL_DBG_ERR("DRD failed to enable ep0in\n");

        return;
    }
}

static void DRD_GadgetWakeupInterrupt(struct USB_DRD_HANDLE *pDRD)
{
}

static void DRD_GadgetLinkstsChangeInterrupt(struct USB_DRD_HANDLE *pDRD, uint32_t evtInfo)
{
}

static void DRD_GadgetSuspendInterrupt(struct USB_DRD_HANDLE *pDRD, uint32_t evtInfo)
{
}

static void DRD_GadgetInterrupt(struct USB_DRD_HANDLE *pDRD, const struct DRD_EVENT_DEVT *event)
{
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;

    switch (event->type) {
    case DRD_DEVICE_EVENT_DISCONNECT:
        HAL_DBG("DRD device disconnect\n");
        DRD_GadgetDisconnectInterrupt(pDRD);
        HAL_DRD_DisconnectCallback(pDRD);
        break;
    case DRD_DEVICE_EVENT_RESET:
        HAL_DBG("DRD device reset\n");
        DRD_GadgetResetInterrupt(pDRD);
        HAL_DRD_ResetCallback(pDRD);
        break;
    case DRD_DEVICE_EVENT_CONNECT_DONE:
        DRD_GadgetConndoneInterrupt(pDRD);
        break;
    case DRD_DEVICE_EVENT_WAKEUP:
        DRD_GadgetWakeupInterrupt(pDRD);
        break;
    case DRD_DEVICE_EVENT_HIBER_REQ:
        HAL_DBG("DRD unexpected hibernation event\n");
        break;
    case DRD_DEVICE_EVENT_LINK_STATUS_CHANGE:
        DRD_GadgetLinkstsChangeInterrupt(pDRD, event->eventInfo);
        break;
    case DRD_DEVICE_EVENT_SUSPEND:
        if (!DRD_VER_IS_PRIOR(DRD, 230A)) {
            DRD_GadgetSuspendInterrupt(pDRD, event->eventInfo);
        }
        HAL_DRD_SuspendCallback(pDRD);
        break;
    case DRD_DEVICE_EVENT_SOF:
    case DRD_DEVICE_EVENT_ERRATIC_ERROR:
    case DRD_DEVICE_EVENT_CMD_CMPL:
    case DRD_DEVICE_EVENT_OVERFLOW:
        break;
    default:
        HAL_DBG("DRD UNKNOWN IRQ %d\n", event->type);
    }
}

static void DRD_ProcessEventEntry(struct USB_DRD_HANDLE *pDRD, const union DRD_EVENT *event)
{
    if (!event->type.isDevSpec) {
        DRD_EndpointInterrupt(pDRD, &event->depevt);
    } else if (event->type.type == DRD_EVENT_TYPE_DEV) {
        DRD_GadgetInterrupt(pDRD, &event->devt);
    } else {
        HAL_DBG_ERR("DRD UNKNOWN IRQ type 0x%x\n", event->raw);
    }
}

static void DRD_ProcessEventBuf(struct USB_DRD_HANDLE *pDRD, struct DRD_EVENT_BUFFER *evt)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    int32_t left;

    left = evt->count;

    if (!(evt->flags & DRD_EVENT_PENDING)) {
        return;
    }

    while (left > 0) {
        union DRD_EVENT event;

        event.raw = *(uint32_t *)(evt->buf + evt->lpos);

        DRD_ProcessEventEntry(pDRD, &event);

        evt->lpos = (evt->lpos + 4) % evt->length;
        left -= 4;
    }

    evt->count = 0;

    /* Clean + Invalidate the buffers after touching them */
    HAL_DCACHE_CleanInvalidateByRange((uintptr_t)evt->buf, evt->length);

    /* Unmask interrupt */
    pUSB3DRD->GEVNTSIZ0 = DRD_GEVNTSIZ_SIZE(evt->length);

    /* Keep the clearing of DRD_EVENT_PENDING at the end */
    evt->flags &= ~DRD_EVENT_PENDING;
}

static HAL_Status DRD_CheckEventBuf(struct USB_DRD_HANDLE *pDRD)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    struct DRD_EVENT_BUFFER *evt = &pDRD->evBuf;
    uint32_t count;

    HAL_DCACHE_InvalidateByRange((uintptr_t)evt->buf, evt->length);

    count = pUSB3DRD->GEVNTCOUNT0;
    count &= DRD_GEVNTCOUNT_MASK;
    if (!count) {
        return HAL_INVAL;
    }

    evt->count = count;
    evt->flags |= DRD_EVENT_PENDING;

    /* Mask interrupt */
    pUSB3DRD->GEVNTSIZ0 = (DRD_GEVNTSIZ_INTMASK | DRD_GEVNTSIZ_SIZE(evt->length));

    pUSB3DRD->GEVNTCOUNT0 = count;

    return HAL_OK;
}

/** @} */

/********************* Public Function Definition ****************************/
/** @defgroup USB_DRD_GADGET_Exported_Functions_Group1 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:
 *  @{
 */

/**
 * @brief Initializes the DRD Gadget according to the specified parameters
 *        in the struct USB_DRD_CFG and create the associated handle.
 * @param pDRD: DRD handle
 * @return HAL status
 */
HAL_Status HAL_DRD_GadgetInit(struct USB_DRD_HANDLE *pDRD)
{
    HAL_Status ret;

    /* Check the DRD handle allocation */
    HAL_ASSERT(pDRD != NULL);

    /* Check the parameters */
    HAL_ASSERT(IS_USB3DRD_INSTANCE(pDRD->pReg));

    pDRD->pReg = (struct USB3DRD_REG *)((uintptr_t)pDRD->pReg + DRD_GLOBALS_REGS_START);

    ret = DRD_AllocEventBuffer(pDRD, DRD_EVENT_BUFFERS_SIZE);
    if (ret) {
        return ret;
    }

    /*Init the Core (common init.) */
    ret = HAL_DRD_CoreInit(pDRD->pReg, &pDRD->cfg);
    if (ret) {
        HAL_DBG_ERR("DRD failed to initialize core\n");
        goto err0;
    }

    DRD_EventBufferSetup(pDRD);

    DRD_CoreNumEps(pDRD);

    HAL_DRD_SetMode(pDRD->pReg, DRD_GCTL_PRTCAP_DEVICE);

    ret = DRD_GadgetInit(pDRD);
    if (ret) {
        HAL_DBG_ERR("DRD failed to initialize gadget\n");
        goto err1;
    }

    return HAL_OK;
err1:
    DRD_EventBufferCleanup(pDRD);
    HAL_DRD_CoreDeInit(pDRD->pReg);

err0:
    DRD_FreeEventBuffer(pDRD);

    return ret;
}

/**
 * @brief  DeInitializes the DRD Gadget.
 * @param  pDRD: DRD handle
 * @return HAL status
 */
HAL_Status HAL_DRD_GadgetDeInit(struct USB_DRD_HANDLE *pDRD)
{
    /* Check the DRD handle allocation */
    HAL_ASSERT(pDRD != NULL);

    DRD_GadgetExit(pDRD);

    DRD_EventBufferCleanup(pDRD);

    DRD_FreeEventBuffer(pDRD);

    HAL_DRD_CoreDeInit(pDRD->pReg);

    return HAL_OK;
}

/** @} */

/** @defgroup USB_DRD_GADGET_Exported_Functions_Group2 IO Functions
 *  @brief    Data transfers functions
 *

 This section provides functions allowing to IO controlling:

 *  @{
 */

/**
 * @brief  Handle DRD Gadget interrupt request.
 * @param  pDRD: DRD handle
 */
HAL_Status HAL_DRD_IRQHandler(struct USB_DRD_HANDLE *pDRD)
{
    HAL_Status ret;

    ret = DRD_CheckEventBuf(pDRD);
    if (ret == HAL_OK) {
        struct DRD_EVENT_BUFFER *evt;

        evt = &pDRD->evBuf;
        DRD_ProcessEventBuf(pDRD, evt);
    }

    return HAL_OK;
}

/**
 * @brief  Start The DRD Gadget, begin to receive SETUP packets.
 * @param  pDRD: DRD handle
 * @return HAL status
 */
HAL_Status HAL_DRD_GadgetStart(struct USB_DRD_HANDLE *pDRD)
{
    HAL_Status ret;

    ret = DRD_GadgetStart(pDRD);
    if (ret) {
        HAL_DBG_ERR("DRD failed to start\n");

        return ret;
    }

    ret = DRD_GadgetRunStop(pDRD, true);
    if (ret) {
        HAL_DBG_ERR("DRD failed to run\n");

        return ret;
    }

    return HAL_OK;
}

/**
 * @brief  Stop The DRD Gadget.
 * @param  pDRD: DRD handle
 * @return HAL status
 */
HAL_Status HAL_DRD_GadgetStop(struct USB_DRD_HANDLE *pDRD)
{
    DRD_GadgetStop(pDRD);

    return HAL_OK;
}

/**
 * @brief  Open and configure an endpoint.
 * @param  pDRD: DRD handle
 * @param  pEpInfo: The endpoint info
 * @return HAL status
 */
HAL_Status HAL_DRD_EPOpen(struct USB_DRD_HANDLE *pDRD, struct DRD_EP_INFO *pEpInfo)
{
    struct DRD_EP *dep;
    uint8_t epNum;

    epNum = (pEpInfo->bEndpointAddress & DRD_USB_ENDPOINT_NUMBER_MASK) << 1;
    if ((pEpInfo->bEndpointAddress & DRD_USB_ENDPOINT_DIR_MASK) == DRD_USB_DIR_IN) {
        epNum |= 1;
    }

    if (epNum == 0 || epNum == 1) {
        return HAL_OK;
    }

    dep = &pDRD->eps[epNum];
    if (dep->flags & DRD_EP_ENABLED) {
        HAL_DBG_WRN("DRD epAddr 0x%02x already enabled\n", pEpInfo->bEndpointAddress);

        return HAL_OK;
    }

    dep->epInfo.bDescriptorType = pEpInfo->bDescriptorType;
    dep->epInfo.bEndpointAddress = pEpInfo->bEndpointAddress;
    dep->epInfo.bmAttributes = pEpInfo->bmAttributes;
    dep->epInfo.wMaxPacketSize = pEpInfo->wMaxPacketSize;
    dep->epInfo.bInterval = pEpInfo->bInterval;
    dep->epInfo.bMaxBurst = pEpInfo->bMaxBurst;

    dep->type = pEpInfo->bmAttributes & DRD_USB_ENDPOINT_XFERTYPE_MASK;
    dep->maxPacket = pEpInfo->wMaxPacketSize & DRD_USB_ENDPOINT_MAXP_MASK;

    if (pDRD->enumSpeed >= DRD_USB_SPEED_SUPER) {
        if (dep->epInfo.bMaxBurst > 15) {
            dep->maxBurst = 1;
        } else {
            dep->maxBurst = dep->epInfo.bMaxBurst + 1;
        }

        HAL_DBG("DRD epAddr 0x%02x maxBurst %d\n", pEpInfo->bEndpointAddress, dep->maxBurst);
    }

    HAL_DBG("DRD enable epAddr 0x%02x\n", pEpInfo->bEndpointAddress);

    DRD_GadgetEpEnable(pDRD, dep, DRD_DEPCFG_ACTION_INIT);

    return HAL_OK;
}

/**
 * @brief  Disable an endpoint.
 * @param  pDRD: DRD handle
 * @param  epAddr: The endpoint address
 * @return HAL status
 */
HAL_Status HAL_DRD_EPClose(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr)
{
    struct DRD_EP *dep;
    uint8_t epNum;

    epNum = (epAddr & DRD_USB_ENDPOINT_NUMBER_MASK) << 1;
    if ((epAddr & DRD_USB_ENDPOINT_DIR_MASK) == DRD_USB_DIR_IN) {
        epNum |= 1;
    }

    if (epNum == 0 || epNum == 1) {
        return HAL_OK;
    }

    dep = &pDRD->eps[epNum];
    if (!(dep->flags & DRD_EP_ENABLED)) {
        HAL_DBG_WRN("DRD epAddr 0x%02x already disabled\n", epAddr);

        return HAL_OK;
    }

    HAL_DBG("DRD disable epAddr 0x%02x\n", epAddr);

    DRD_GadgetEpSkipTrbs(dep);

    DRD_GadgetEpDisable(pDRD, dep);

    return HAL_OK;
}

/**
 * @brief  Receive an amount of data.
 * @param  pDRD: DRD handle
 * @param  epAddr: The endpoint address
 * @param  pBuf: The pointer to the reception buffer
 * @param  len: The amount of data to be received
 * @return HAL status
 */
HAL_Status HAL_DRD_EPReceive(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint8_t *pBuf, uint32_t len)
{
    struct DRD_EP *dep;
    uint8_t epNum;

    HAL_DBG("DRD epAddr 0x%02x Receive req len %d pBuf %p\n", epAddr, len, pBuf);

    epNum = (epAddr & DRD_USB_ENDPOINT_NUMBER_MASK) << 1;
    if ((epAddr & DRD_USB_ENDPOINT_DIR_MASK) == DRD_USB_DIR_IN) {
        epNum |= 1;
    }

    dep = &pDRD->eps[epNum];
    dep->pxferBuff = pBuf;
    dep->xferLen = len;
    dep->actualLen = 0;
    dep->numTrbs = 0;
    dep->dmaAddr = HAL_CpuAddrToDmaAddr((uintptr_t)pBuf);
    dep->flags |= DRD_EP_TRANSFER_QUEUED;

#ifdef DRD_DEBUG
    if (dep->dmaAddr > UINT32_MAX) {
        if (pDRD->cfg.hasAwidth64) {
            if ((uint8_t)HAL_UPPER_32_BITS(dep->dmaAddr) != pDRD->addrH) {
                HAL_DBG_ERR("DRD EPReceive dma buf high 32-bit address is not correct!\n");
            }
        } else {
            HAL_DBG_ERR("DRD EPReceive dma buf is not a 32-bit address!\n");
        }
    }
#endif

    if ((epAddr & DRD_USB_ENDPOINT_NUMBER_MASK) == 0) {
        DRD_EP0StartXfer(pDRD, dep);
    } else {
        DRD_GadgetEPStartXfer(pDRD, dep);
    }

    return HAL_OK;
}

/**
 * @brief  Send an amount of data.
 * @param  pDRD: DRD handle
 * @param  epAddr: The endpoint address
 * @param  pBuf: The pointer to the transmission buffer
 * @param  len: The amount of data to be sent
 * @return HAL status
 */
HAL_Status HAL_DRD_EPTransmit(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint8_t *pBuf, uint32_t len)
{
    struct DRD_EP *dep;
    uint8_t epNum;

    HAL_DBG("DRD epAddr 0x%02x Transmit req len %d pBuf %p\n", epAddr, len, pBuf);

    epNum = (epAddr & DRD_USB_ENDPOINT_NUMBER_MASK) << 1;
    if ((epAddr & DRD_USB_ENDPOINT_DIR_MASK) == DRD_USB_DIR_IN) {
        epNum |= 1;
    }

    dep = &pDRD->eps[epNum];
    dep->pxferBuff = pBuf;
    dep->xferLen = len;
    dep->actualLen = 0;
    dep->numTrbs = 0;
    dep->dmaAddr = HAL_CpuAddrToDmaAddr((uintptr_t)pBuf);
    dep->flags |= DRD_EP_TRANSFER_QUEUED;

#ifdef DRD_DEBUG
    if (dep->dmaAddr > UINT32_MAX) {
        if (pDRD->cfg.hasAwidth64) {
            if ((uint8_t)HAL_UPPER_32_BITS(dep->dmaAddr) != pDRD->addrH) {
                HAL_DBG_ERR("DRD EPTransmit dma buf high 32-bit address is not correct!\n");
            }
        } else {
            HAL_DBG_ERR("DRD EPTransmit dma buf is not a 32-bit address!\n");
        }
    }
#endif

    if ((epAddr & DRD_USB_ENDPOINT_NUMBER_MASK) == 0) {
        DRD_EP0StartXfer(pDRD, dep);
    } else {
        DRD_GadgetEPStartXfer(pDRD, dep);
    }

    return HAL_OK;
}

/**
 * @brief  Set a STALL condition over an endpoint.
 * @param  pDRD: DRD handle
 * @param  epAddr: The endpoint address
 * @return HAL status
 */
HAL_Status HAL_DRD_EPSetStall(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr)
{
    struct DRD_EP *dep;
    uint8_t epNum;

    HAL_DBG("DRD epAddr 0x%02x set stall\n", epAddr);

    epNum = (epAddr & DRD_USB_ENDPOINT_NUMBER_MASK) << 1;
    if ((epAddr & DRD_USB_ENDPOINT_DIR_MASK) == DRD_USB_DIR_IN) {
        epNum |= 1;
    }
    dep = &pDRD->eps[epNum];

    return DRD_GadgetEpSetHalt(pDRD, dep, 1, false);
}

/**
 * @brief  Clear a STALL condition over in an endpoint.
 * @param  pDRD: DRD handle
 * @param  epAddr: The endpoint address
 * @return HAL status
 */
HAL_Status HAL_DRD_EPClrStall(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr)
{
    struct DRD_EP *dep;
    uint8_t epNum;

    HAL_DBG("DRD epAddr 0x%02x clear stall\n", epAddr);

    epNum = (epAddr & DRD_USB_ENDPOINT_NUMBER_MASK) << 1;
    if ((epAddr & DRD_USB_ENDPOINT_DIR_MASK) == DRD_USB_DIR_IN) {
        epNum |= 1;
    }
    dep = &pDRD->eps[epNum];

    return DRD_GadgetEpSetHalt(pDRD, dep, 0, false);
}

/**
 * @brief  Set the USB Device address.
 * @param  pDRD: DRD handle
 * @param  addr: The new device address
 * @return HAL status
 */
HAL_Status HAL_DRD_SetAddress(struct USB_DRD_HANDLE *pDRD, uint8_t addr)
{
    struct USB3DRD_REG *pUSB3DRD = pDRD->pReg;
    struct USB_DRD_CFG *pCFG = &pDRD->cfg;
    uint32_t reg;

    reg = pUSB3DRD->DCFG;
    reg &= ~(DRD_DCFG_DEVADDR_MASK);
    reg |= DRD_DCFG_DEVADDR(addr);
    pUSB3DRD->DCFG = reg;

    /*
     * Enable transition to U1/U2 state when
     * nothing is pending from application.
     */
    reg = pUSB3DRD->DCTL;
    if (pCFG->disU1u2Quirk) {
        reg &= ~(DRD_DCTL_ACCEPTU1ENA | DRD_DCTL_ACCEPTU2ENA);
    } else {
        reg |= (DRD_DCTL_ACCEPTU1ENA | DRD_DCTL_ACCEPTU2ENA);
    }
    pUSB3DRD->DCTL = reg;

    return HAL_OK;
}

/**
 * @brief  USB Reset callback.
 * @param  pDRD: DRD handle
 */
__WEAK void HAL_DRD_ResetCallback(struct USB_DRD_HANDLE *pDRD)
{
}

/**
 * @brief  Setup stage callback.
 * @param  pDRD: DRD handle
 */
__WEAK void HAL_DRD_SetupStageCallback(struct USB_DRD_HANDLE *pDRD)
{
}

/**
 * @brief  USB Suspend callback.
 * @param  pDRD: DRD handle
 */
__WEAK void HAL_DRD_SuspendCallback(struct USB_DRD_HANDLE *pDRD)
{
}

/**
 * @brief  Data OUT stage callback.
 * @param  pDRD: DRD handle
 * @param  epAddr: The endpoint address
 * @param  nbytes: The actual length of out data
 */
__WEAK void HAL_DRD_DataOutStageCallback(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint32_t nbytes)
{
}

/**
 * @brief  Data IN stage callback.
 * @param  pDRD: DRD handle
 * @param  epAddr: The endpoint address
 * @param  nbytes: The actual length of in data
 */
__WEAK void HAL_DRD_DataInStageCallback(struct USB_DRD_HANDLE *pDRD, uint8_t epAddr, uint32_t nbytes)
{
}

/**
 * @brief  USB Disconnect callback.
 * @param  pDRD: DRD handle
 */
__WEAK void HAL_DRD_DisconnectCallback(struct USB_DRD_HANDLE *pDRD)
{
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_USB_DRD_MODULE_ENABLED */
