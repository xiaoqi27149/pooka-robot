# Rockchip HAL PCIe User Guide {#Rockchip_User_Guide_HAL_PCIe}

Release Version: V1.4.0

Release Date: 2025-04-09

Security Level: □Top-Secret   □Secret   □Internal   ■Public

**DISCLAIMER**

THIS DOCUMENT IS PROVIDED “AS IS”. ROCKCHIP ELECTRONICS CO., LTD.(“ROCKCHIP”)DOES NOT PROVIDE ANY WARRANTY OF ANY KIND, EXPRESSED, IMPLIED OR OTHERWISE, WITH RESPECT TO THE ACCURACY, RELIABILITY, COMPLETENESS,MERCHANTABILITY, FITNESS FOR ANY PARTICULAR PURPOSE OR NON-INFRINGEMENT OF ANY REPRESENTATION, INFORMATION AND CONTENT IN THIS DOCUMENT. THIS DOCUMENT IS FOR REFERENCE ONLY. THIS DOCUMENT MAY BE UPDATED OR CHANGED WITHOUT ANY NOTICE AT ANY TIME DUE TO THE UPGRADES OF THE PRODUCT OR ANY OTHER REASONS.

**Trademark Statement**

"Rockchip", "瑞芯微", "瑞芯" shall be Rockchip’s registered trademarks and owned by Rockchip. All the other trademarks or registered trademarks mentioned in this document shall be owned by their respective owners.

**All rights reserved. ©2025. Rockchip Electronics Co., Ltd.**

Beyond the scope of fair use, neither any entity nor individual shall extract, copy, or distribute this document in any form in whole or in part without the written approval of Rockchip.

Rockchip Electronics Co., Ltd.

No.18 Building, A District, No.89, software Boulevard Fuzhou, Fujian,PRC

Website:     [www.rock-chips.com](http://www.rock-chips.com)

Customer service Tel:  +86-4007-700-590

Customer service Fax:  +86-591-83951833

Customer service e-Mail:  [fae@rock-chips.com](mailto:fae@rock-chips.com)

---

**Preface**

**Overview**

This article introduces the instructions for using HAL PCIe. The HAL source code can be used in baremetal or RTOS system environments and only supports simple functional operations.

**Product Version**

| **Chipset** | **Kernel Version** |
| ----------- | ------------------ |
| ALL SOC     | HAL/RTOS           |

**Intended Audience**

This document (this guide) is mainly intended for:

Technical support engineers

Software development engineers

---

**Revision History**

| **Version** | **Author** | **Date**   | **Change Description**                 |
| ----------- | ---------- | ---------- | -------------------------------------- |
| V1.0.0      | Jon Lin    | 2023-04-04 | Initial version                        |
| V1.1.0      | Jon Lin    | 2024-08-08 | Add reference document                 |
| V1.2.0      | Jon Lin    | 2025-02-13 | Add software configuration instruction |
| V1.3.0      | Jon Lin    | 2025-03-18 | Optimization Application Description   |
| V1.4.0      | Jon Lin    | 2025-04-09 | Modify the EP legacy INTx enable api   |

---

**Contents**

[TOC]

---

## Introduction

Due to the lack of complete PCIe framework code under the bare metal or RTOS project, it is necessary to rely on the PCIe initialization and enumeration process under u-boot or Linux. Under bare metal or RTOS, only the following simple functions are supported:

- Access to controller registers
- CPU access to peripherals, mainly Bar and CFG spaces
- uDMA transfer
- INTx legacy interrupt

So in addition to the HAL interface introduced in this document, the following development work is also needed:

- On the RC side, develop the PCIe initialization and enumeration process under u-boot or Linux:
    - Refer to Rockchip_Developer_Guide_UBoot_Nextdev_EN.pdf
    - Refer to Rockchip_Developer_Guide_PCIe_CN.pdf
- On the EP side, for RK EP reference only, optional initialization of PCIe during the SPL phase or during the Linux phase:
    - Refer to Rockchip_Developer_Guide_PCIE_EP_Stardard_Card_CN.pdf, it is recommended to first refer to the RK EP demo version of the drawing for hardware design specifications, such as:
        - The PCIe interconnection has a master-slave relationship, and the enumeration is guided by the RC to reset the EP timing sequence. The RK EP solution suggests controlling the EP device NPOR through the PERST# signal.
        - The same source clock scheme.

## Specification

### RK RC Software Configuration And Booting Process

**PCIe Initialization Scheme with Kernel (Recommended)**

![amp_pcie-kernel.drawio](Rockchip_User_Guide_HAL_PCIe/amp_pcie-kernel.drawio.png)

Explanation:

- The "100 ms delay" in the process is a redundant time waiting for the PCIe peripheral driver to load and can be replaced by a handshake protocol in the business code.
- Refer to the "Rockchip_Developer_Guide_PCIe_CN.pdf" document for kernel PCIe development, and important information such as bdf and bar mapping can be obtained using the lspci command.

**PCIe Initialization Scheme with u-boot**

![amp_pcie-uboot.drawio](Rockchip_User_Guide_HAL_PCIe/amp_pcie-uboot.drawio.png)

- The "100 ms delay" in the process is a redundant time waiting for the PCIe peripheral driver to load. In fact, u-boot has completed this process, and the redundant delay can be manually deleted.
- Refer to the "Rockchip_Developer_Guide_UBoot_Nextdev_CN.pdf" document, PCIe chapter, for u-boot PCIe development, mainly involving:
    - Configuration instructions for "using PCIe before loading kernel dtb"
    - Pci cmd to obtain PCIe peripheral information, including important information such as bdf and bar mapping.

### RK EP Software Configuration

If RK EP is used as a PCIe endpoint and there is a need for bare-metal development, it is recommended to refer to the manual "Rockchip_Developer_Guide_PCIE_EP_Stardard_Card_CN.pdf" and use the "Flash Boot" boot scheme. Integrate the PCIe Bin driver to complete PCIe initialization and enumeration during the loader stage, and develop the RK EP device driver in the bare-metal environment.

### Resource Allocation

In addition to CPU accessing Bar resources, other resources involved in PCIe usually involve competition. Therefore, when multiple systems share the same PCIe controller, resources should be allocated in advance:

| Resource                    | Explanation                                                  |
| --------------------------- | ------------------------------------------------------------ |
| CPU access to Bar resources | No competition, multi-core access by bus arbitration         |
| CPU access to CFG space     | Read does not involve competition, write involves consistency issues |
| uDMA                        | Supports 2 channels, independent between channels, but multi-core access to the same DMA channel involves competition |
| INTx                        | Involves competition                                         |
| Controller Registers        | Avoid simultaneous access as much as possible, and refine analysis according to actual situation |
| MSI                         | Only supported in kernel                                     |

## RC Application Notes

The series supports reference to the `test_pcie.c` source code for the `TEST_GROUP_RUNNER(HAL_PCIE)` test.

### Setting Up the Peripheral

Refer to the section "RK RC Software Configuration and Boot Scheme" to confirm the PCIe device information. After that, modify the following information in `test_pcie.c`. For example, for device `01.00.00` with BAR0 mapped to address `0xF0200000`:

```c
#define TEST_DEVICE              PCI_BDF(0x01, 0x00, 0x00) /**< Valid for rc, reset PCIe bdf according to the actual device */
#define TEST_DEVICE_BAR_CPU_ADDR 0xF0200000 /**< Valid for rc, reset PCIe bus according to the actual device */
#define TEST_DEVICE_TEST_SIZE    0x100
```

### Enabling the Endpoint

```c
HAL_PCI_EnableDevice(&s_pcieHandle.dev->cfgBase);
```

### CPU Access to Endpoint CFG Space

```c
HAL_PCIE_OutboundConfigCFG0(&s_pcieHandle, TEST_DEVICE, 0x100000);
HAL_PCI_ReadConfigWord(cfg, PCI_VENDOR_ID, &vid);
HAL_PCI_ReadConfigWord(cfg, PCI_DEVICE_ID, &did);
```

Note:

- When accessing the CFG space multiple times, only one call to the `HAL_PCIE_OutboundConfigCFG0` interface is required.

### CPU Access to Endpoint Bar Space

Refer to the `PCIECPUSimpleTest` test:

```c
void *bar = (void *)TEST_DEVICE_BAR_CPU_ADDR;

memset_io(bar, 0xa5, len);
memcpy_fromio(buf, bar, len);
memcpy_toio(bar, buf, len);
```

### DMA Transfer

Refer to the Doxygen comments in `hal_pcie.c` under `PCIE_How_To_Use` for details. Also, refer to the TRM to confirm whether the target PCIe controller of the RK SOC supports uDMA implementation.

- EP with Integrated DMA Engine:
    - General peripherals such as FPGAs usually have their own integrated DMA. Use the peripheral's DMA engine to initiate DMA transfers.
    - The RK EP defaults to mapping the built-in DMA controller registers to BAR4 (wired register). The RC side supports configuring BAR4 to trigger the EP-side DMA engine to initiate DMA transfers. For details, refer to the `test_pcie.c` source code example `PCIeDMASimpleTest`:
        - Confirm that the MMU memory mapping settings include the `TEST_PCIE_DMA_LOCAL_ADDR` test address.
- EP without Integrated DMA Engine, but RK RC with Integrated DMA Engine:
    - Support for enabling its own DMA. For details, refer to the `test_pcie.c` source code example `RKRC_PCIeDMASimpleTest`.
    - Confirm that `TEST_PCIE_DMA_BUS_ADDR` is modified to the target BAR address, which is the BAR mapping address allocated to the EP.
    - Confirm that the MMU memory mapping settings include the `TEST_PCIE_DMA_LOCAL_ADDR` test address.

### Receiving INTx Interrupt

```c
HAL_IRQ_HANDLER_SetIRQHandler(g_pcieDev.legacyIrqNum, PCIE_INTxIsr, NULL);
HAL_GIC_Enable(g_pcieDev.legacyIrqNum);
HAL_PCIE_EnableLegacyINTx(&s_pcieHandle, 0);
HAL_PCI_EnableLegacyINTx(&s_pcieHandle.dev->cfgBase);
```

Note:

- In an AMP environment, the Linux system side only enables MSI/MSI-X interrupts. Therefore, legacy interrupts are available for the HAL environment. The allocation scheme is as follows:
    - Core A uses Linux and exclusively occupies MSI/MSI-X interrupts.
    - Core B uses HAL/RTOS and exclusively occupies legacy INTx interrupts.
    - Other cores do not have exclusive interrupt resources and can only be notified by Core A/B.
- Since the HAL framework does not allocate INTx numbers to peripherals (default values are used), users should allocate INTx interrupt numbers to peripherals according to their actual applications.

### Triggering ELBI Interrupts

```c
PCIE_RaiseELBI(&s_pcieHandle, 0);
```

## EP Application Notes

The series supports reference to the `test_pcie.c` source code for the `TEST_GROUP_RUNNER(HAL_PCIE_EP)` test.

### CPU Access to Host DRAM Space

```c
HAL_PCIE_OutboundConfig(&s_pcieHandle, 0, PCIE_ATU_TYPE_MEM, &s_pcieHandle.dev->cfgBase, 0x3c000000, 0x100000);
memset_io(bar, 0xa5, len);
memcpy_fromio(buf, bar, len);
memcpy_toio(bar, buf, len);
```

### DMA Transfer

Refer to the `PCIeDMASimpleTest` example.

### Receiving ELBI Interrupts

ELBI interrupts are a special feature of RK EP, supporting RC to trigger EP-side controller interrupts by configuring the ELBI registers in the config space:

```c
HAL_IRQ_HANDLER_SetIRQHandler(g_pcieDev.sysIrqNum, PCIE_SYSIsr, NULL);
HAL_GIC_Enable(g_pcieDev.sysIrqNum);
/* Enable client elbi interrupt */
HAL_PCIE_EnableELBIIndex0(&s_pcieHandle);
```

### Triggering MSI Interrupts

```c
HAL_PCIE_RaiseMSI(&s_pcieHandle, 0);
```

### Triggering Legacy Interrupts

```c
HAL_PCIE_RaiseLegacyINTx(&s_pcieHandle, 0);
```

## Frequently Asked Questions

### RC BAR Read/Write Not Taking Effect

- Confirm whether `TEST_DEVICE_BAR_CPU_ADDR` has been correctly modified to the BAR mapping address allocated to the peripheral.

### RC DMA Data Transfer Not as Expected

- Not all PCIe controllers support DMA transfer. First, confirm whether the PCIe controller supports DMA transfer.
- Please confirm whether the memory on both sides is of normal memory attribute, and whether memory consistency operations such as cache invalidate/flush have been performed when filling/obtaining memory.
- It is recommended to first use the default RK test code to verify the transfer is effective before proceeding with code migration and customization.

### RC Triggering RK EP ELBI Interrupt Not Working

Confirm that HAL_PCIE_OutboundConfigCFG0 is not called multiple times. It should only be called once.
