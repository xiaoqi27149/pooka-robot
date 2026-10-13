/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#ifndef __RV1126B_CRU_H
#define __RV1126B_CRU_H

// ======================= TOPCRU module definition bank=0 ========================
// TOPCRU_SOFTRST_CON00(Offset:0xA00)

// TOPCRU_SOFTRST_CON15(Offset:0xA3C)
#define SRST_PRESETN_CRU                         0x000000F1
#define SRST_PRESETN_CRU_BIU                     0x000000F2

// TOPCRU_GATE_CON00(Offset:0x800)
#define CLK_CPLL_DIV20_SRC_GATE                  0x00000000
#define CLK_CPLL_DIV10_SRC_GATE                  0x00000001
#define CLK_CPLL_DIV8_SRC_GATE                   0x00000002
#define CLK_GPLL_DIV8_SRC_GATE                   0x00000003
#define CLK_GPLL_DIV6_SRC_GATE                   0x00000004
#define CLK_GPLL_DIV4_SRC_GATE                   0x00000005
#define CLK_CPLL_DIV3_SRC_GATE                   0x00000006
#define CLK_GPLL_DIV3_SRC_GATE                   0x00000007
#define CLK_CPLL_DIV2_SRC_GATE                   0x00000008
#define CLK_GPLL_DIV2_SRC_GATE                   0x00000009
#define CLK_TESTOUT_TOP_GATE                     0x0000000E

// TOPCRU_GATE_CON01(Offset:0x804)
#define CLK_CM_FRAC_MATRIX0_GATE                 0x00000010
#define CLK_CM_FRAC_MATRIX1_GATE                 0x00000011
#define CLK_CM_FRAC_MATRIX2_GATE                 0x00000012
#define CLK_UART_FRAC_MATRIX0_GATE               0x00000013
#define CLK_UART_FRAC_MATRIX1_GATE               0x00000014
#define CLK_AUDIO_FRAC_MATRIX0_GATE              0x00000015
#define CLK_AUDIO_FRAC_MATRIX1_GATE              0x00000016
#define CLK_AUDIO_INT_MATRIX0_GATE               0x00000017
#define CLK_AUDIO_INT_MATRIX1_GATE               0x00000018
#define SCLK_UART0_SRC_GATE                      0x00000019
#define SCLK_UART1_SRC_GATE                      0x0000001A
#define SCLK_UART2_SRC_GATE                      0x0000001B
#define SCLK_UART3_SRC_GATE                      0x0000001C
#define SCLK_UART4_SRC_GATE                      0x0000001D
#define SCLK_UART5_SRC_GATE                      0x0000001E

// TOPCRU_GATE_CON02(Offset:0x808)
#define SCLK_UART6_SRC_GATE                      0x00000020
#define SCLK_UART7_SRC_GATE                      0x00000021
#define MCLK_SAI0_SRC_GATE                       0x00000022
#define MCLK_SAI1_SRC_GATE                       0x00000023
#define MCLK_SAI2_SRC_GATE                       0x00000024
#define MCLK_PDM_SRC_GATE                        0x00000025
#define CLKOUT_PDM_SRC_GATE                      0x00000026
#define MCLK_ASRC0_SRC_GATE                      0x00000027
#define MCLK_ASRC1_SRC_GATE                      0x00000028
#define MCLK_ASRC2_SRC_GATE                      0x00000029
#define MCLK_ASRC3_SRC_GATE                      0x0000002A
#define CLK_ASRC0_SRC_GATE                       0x0000002B
#define CLK_ASRC1_SRC_GATE                       0x0000002C

// TOPCRU_GATE_CON05(Offset:0x814)
#define CLK_CORE_PLL_SRC_GATE                    0x00000050
#define CLK_NPU_PLL_SRC_GATE                     0x00000051
#define CLK_VEPU_PLL_SRC_GATE                    0x00000052
#define CLK_ISP_PLL_SRC_GATE                     0x00000053
#define CLK_AISP_PLL_SRC_GATE                    0x00000054
#define CLK_SARADC0_SRC_GATE                     0x00000056
#define CLK_SARADC1_SRC_GATE                     0x00000057
#define CLK_SARADC2_SRC_GATE                     0x00000058
#define HCLK_CORE_SRC_GATE                       0x00000059
#define HCLK_NPU_SRC_GATE                        0x0000005A
#define PCLK_NPU_SRC_GATE                        0x0000005B
#define ACLK_VEPU_SRC_GATE                       0x0000005C
#define HCLK_VEPU_SRC_GATE                       0x0000005D
#define PCLK_VEPU_SRC_GATE                       0x0000005E

// TOPCRU_GATE_CON06(Offset:0x818)
#define CLK_CORE_RGA_SRC_GATE                    0x00000060
#define ACLK_GMAC_SRC_GATE                       0x00000061
#define ACLK_VI_SRC_GATE                         0x00000062
#define HCLK_VI_SRC_GATE                         0x00000063
#define PCLK_VI_SRC_GATE                         0x00000064
#define DCLK_VICAP_SRC_GATE                      0x00000065
#define CLK_SYS_DSMC_SRC_GATE                    0x00000066
#define ACLK_VDO_SRC_GATE                        0x00000067
#define ACLK_RKVDEC_SRC_GATE                     0x00000068
#define HCLK_VDO_SRC_GATE                        0x00000069
#define PCLK_VDO_SRC_GATE                        0x0000006A
#define DCLK_VOP_SRC_GATE                        0x0000006C
#define DCLK_OOC_SRC_GATE                        0x0000006D
#define DCLK_DECOM_SRC_GATE                      0x0000006E

// TOPCRU_GATE_CON07(Offset:0x81C)
#define PCLK_DDR_SRC_GATE                        0x00000070
#define ACLK_SYSMEM_SRC_GATE                     0x00000071
#define ACLK_TOP_SRC_GATE                        0x00000073
#define ACLK_BUS_SRC_GATE                        0x00000074
#define HCLK_BUS_SRC_GATE                        0x00000075
#define PCLK_BUS_SRC_GATE                        0x00000076
#define CCLK_SDMMC0_SRC_GATE                     0x00000077
#define CCLK_SDMMC1_SRC_GATE                     0x00000078
#define CCLK_EMMC_SRC_GATE                       0x00000079
#define SCLK_2X_FSPI0_SRC_GATE                   0x0000007A
#define CLK_GMAC_PTP_REF_SRC_GATE                0x0000007B
#define CLK_GMAC_125M_SRC_GATE                   0x0000007C
#define CLK_TIMER_SRC_GATE                       0x0000007D

// TOPCRU_GATE_CON08(Offset:0x820)
#define TCLK_WDT_NS_SRC_GATE                     0x00000080
#define TCLK_WDT_S_SRC_GATE                      0x00000081
#define TCLK_WDT_HPMCU_SRC_GATE                  0x00000082
#define CLK_CAN0_SRC_GATE                        0x00000084
#define CLK_CAN1_SRC_GATE                        0x00000085
#define PCLK_PERI_SRC_GATE                       0x00000086
#define ACLK_PERI_SRC_GATE                       0x00000087
#define CLK_I2C_BUS_SRC_GATE                     0x00000089
#define CLK_SPI0_SRC_GATE                        0x0000008A
#define CLK_SPI1_SRC_GATE                        0x0000008B
#define BUSCLK_PMU_SRC_GATE                      0x0000008C

// TOPCRU_GATE_CON09(Offset:0x824)
#define CLK_PWM0_SRC_GATE                        0x00000090
#define CLK_PWM2_SRC_GATE                        0x00000092
#define CLK_PWM3_SRC_GATE                        0x00000093
#define CLK_PKA_RKCE_SRC_GATE                    0x00000094
#define ACLK_RKCE_SRC_GATE                       0x00000095
#define ACLK_VCP_SRC_GATE                        0x00000096
#define HCLK_VCP_SRC_GATE                        0x00000097
#define PCLK_VCP_SRC_GATE                        0x00000098
#define CLK_CORE_FEC_SRC_GATE                    0x00000099
#define CLK_MATRIX_50M_VI_GATE                   0x0000009B

// TOPCRU_GATE_CON15(Offset:0x83C)
#define PCLK_TOP_ROOT_GATE                       0x000000F0
#define PCLK_CRU_GATE                            0x000000F1
#define PCLK_CRU_BIU_GATE                        0x000000F2
#define CLK_MIPI0_OUT2IO_GATE                    0x000000F3
#define CLK_MIPI1_OUT2IO_GATE                    0x000000F4
#define CLK_MIPI2_OUT2IO_GATE                    0x000000F5
#define CLK_MIPI3_OUT2IO_GATE                    0x000000F6
#define CLK_CIF_OUT2IO_GATE                      0x000000F7
#define CLK_MAC_OUT2IO_GATE                      0x000000F8
#define MCLK_SAI0_OUT2IO_GATE                    0x000000F9
#define MCLK_SAI1_OUT2IO_GATE                    0x000000FA
#define MCLK_SAI2_OUT2IO_GATE                    0x000000FB
#define CLK_AUPLLTEST_GATE                       0x000000FC
#define CLK_GPLLTEST_GATE                        0x000000FE
#define CLK_CPLLTEST_GATE                        0x000000FF

// TOPCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_TOP_DIV                      0x02000000
#define CLK_REF_AUPLL_SEL                        0x01030000
#define CLK_REF_AUPLL_SEL_XIN_OSC0_REF_AUPLL     0U
#define CLK_REF_AUPLL_SEL_XIN_IO_REF_AUPLL       1U
#define CLK_TESTOUT_TOP_SEL                      0x04040000
#define CLK_TESTOUT_TOP_SEL_PCLK_TOP_ROOT        0U
#define CLK_TESTOUT_TOP_SEL_CLK_MIPI0_OUT2IO     1U
#define CLK_TESTOUT_TOP_SEL_CLK_MIPI1_OUT2IO     2U
#define CLK_TESTOUT_TOP_SEL_CLK_MIPI2_OUT2IO     3U
#define CLK_TESTOUT_TOP_SEL_CLK_MIPI3_OUT2IO     4U
#define CLK_TESTOUT_TOP_SEL_CLK_CIF_OUT2IO       5U
#define CLK_TESTOUT_TOP_SEL_CLK_MAC_OUT2IO       6U
#define CLK_TESTOUT_TOP_SEL_MCLK_SAI0_OUT2IO     7U
#define CLK_TESTOUT_TOP_SEL_MCLK_SAI1_OUT2IO     8U
#define CLK_TESTOUT_TOP_SEL_MCLK_SAI2_OUT2IO     9U

// TOPCRU_CLKSEL_CON01(Offset:0x304)
#define CLK_CPLL_DIV20_SRC_DIV                   0x05000001
#define CLK_CPLL_DIV10_SRC_DIV                   0x05050001
#define CLK_CPLL_DIV8_SRC_DIV                    0x050A0001
#define CLK_CPLL_DIV10_SRC_SEL                   0x010F0001
#define CLK_CPLL_DIV10_SRC_SEL_CLK_GPLL_MUX      0U
#define CLK_CPLL_DIV10_SRC_SEL_CLK_CPLL_MUX      1U

// TOPCRU_CLKSEL_CON02(Offset:0x308)
#define CLK_GPLL_DIV8_SRC_DIV                    0x05000002
#define CLK_GPLL_DIV6_SRC_DIV                    0x05050002
#define CLK_GPLL_DIV4_SRC_DIV                    0x050A0002

// TOPCRU_CLKSEL_CON03(Offset:0x30C)
#define CLK_CPLL_DIV3_SRC_DIV                    0x05000003
#define CLK_GPLL_DIV3_SRC_DIV                    0x05050003
#define CLK_CPLL_DIV2_SRC_DIV                    0x050A0003

// TOPCRU_CLKSEL_CON04(Offset:0x310)
#define CLK_GPLL_DIV2_SRC_DIV                    0x05000004

// TOPCRU_CLKSEL_CON10(Offset:0x328)
#define CLK_CM_FRAC_MATRIX0_SEL                  0x0200000A
#define CLK_CM_FRAC_MATRIX0_SEL_XIN_OSC0_FUNC    0U
#define CLK_CM_FRAC_MATRIX0_SEL_CLK_GPLL_MUX     1U
#define CLK_CM_FRAC_MATRIX0_SEL_CLK_AUPLL_MUX    2U
#define CLK_CM_FRAC_MATRIX0_SEL_CLK_CPLL_MUX     3U
#define CLK_CM_FRAC_MATRIX1_SEL                  0x0202000A
#define CLK_CM_FRAC_MATRIX1_SEL_XIN_OSC0_FUNC    0U
#define CLK_CM_FRAC_MATRIX1_SEL_CLK_GPLL_MUX     1U
#define CLK_CM_FRAC_MATRIX1_SEL_CLK_AUPLL_MUX    2U
#define CLK_CM_FRAC_MATRIX1_SEL_CLK_CPLL_MUX     3U
#define CLK_CM_FRAC_MATRIX2_SEL                  0x0204000A
#define CLK_CM_FRAC_MATRIX2_SEL_XIN_OSC0_FUNC    0U
#define CLK_CM_FRAC_MATRIX2_SEL_CLK_GPLL_MUX     1U
#define CLK_CM_FRAC_MATRIX2_SEL_CLK_AUPLL_MUX    2U
#define CLK_CM_FRAC_MATRIX2_SEL_CLK_CPLL_MUX     3U
#define CLK_UART_FRAC_MATRIX0_SEL                0x0206000A
#define CLK_UART_FRAC_MATRIX0_SEL_XIN_OSC0_FUNC  0U
#define CLK_UART_FRAC_MATRIX0_SEL_CLK_GPLL_MUX   1U
#define CLK_UART_FRAC_MATRIX0_SEL_CLK_CPLL_MUX   2U
#define CLK_UART_FRAC_MATRIX1_SEL                0x0208000A
#define CLK_UART_FRAC_MATRIX1_SEL_XIN_OSC0_FUNC  0U
#define CLK_UART_FRAC_MATRIX1_SEL_CLK_GPLL_MUX   1U
#define CLK_UART_FRAC_MATRIX1_SEL_CLK_CPLL_MUX   2U
#define CLK_AUDIO_FRAC_MATRIX0_SEL               0x020A000A
#define CLK_AUDIO_FRAC_MATRIX0_SEL_XIN_OSC0_FUNC 0U
#define CLK_AUDIO_FRAC_MATRIX0_SEL_CLK_GPLL_MUX  1U
#define CLK_AUDIO_FRAC_MATRIX0_SEL_CLK_AUPLL_MUX 2U
#define CLK_AUDIO_FRAC_MATRIX1_SEL               0x020C000A
#define CLK_AUDIO_FRAC_MATRIX1_SEL_XIN_OSC0_FUNC 0U
#define CLK_AUDIO_FRAC_MATRIX1_SEL_CLK_GPLL_MUX  1U
#define CLK_AUDIO_FRAC_MATRIX1_SEL_CLK_AUPLL_MUX 2U

// TOPCRU_CLKSEL_CON11(Offset:0x32C)
#define CLK_AUDIO_INT_MATRIX0_DIV                0x0500000B
#define CLK_AUDIO_INT_MATRIX1_DIV                0x0508000B
#define CLK_AUDIO_INT_MATRIX0_SEL                0x0206000B
#define CLK_AUDIO_INT_MATRIX0_SEL_XIN_OSC0_FUNC  0U
#define CLK_AUDIO_INT_MATRIX0_SEL_CLK_GPLL_MUX   1U
#define CLK_AUDIO_INT_MATRIX0_SEL_CLK_AUPLL_MUX  2U
#define CLK_AUDIO_INT_MATRIX1_SEL                0x020E000B
#define CLK_AUDIO_INT_MATRIX1_SEL_XIN_OSC0_FUNC  0U
#define CLK_AUDIO_INT_MATRIX1_SEL_CLK_GPLL_MUX   1U
#define CLK_AUDIO_INT_MATRIX1_SEL_CLK_AUPLL_MUX  2U

// TOPCRU_CLKSEL_CON12(Offset:0x330)
#define SCLK_UART0_SRC_DIV                       0x0500000C
#define SCLK_UART1_SRC_DIV                       0x0508000C
#define SCLK_UART0_SRC_SEL                       0x0305000C
#define SCLK_UART0_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART0_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART0_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART0_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART0_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART0_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U
#define SCLK_UART1_SRC_SEL                       0x030D000C
#define SCLK_UART1_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART1_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART1_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART1_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART1_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART1_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U

// TOPCRU_CLKSEL_CON13(Offset:0x334)
#define SCLK_UART2_SRC_DIV                       0x0500000D
#define SCLK_UART3_SRC_DIV                       0x0508000D
#define SCLK_UART2_SRC_SEL                       0x0305000D
#define SCLK_UART2_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART2_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART2_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART2_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART2_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART2_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U
#define SCLK_UART3_SRC_SEL                       0x030D000D
#define SCLK_UART3_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART3_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART3_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART3_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART3_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART3_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U

// TOPCRU_CLKSEL_CON14(Offset:0x338)
#define SCLK_UART4_SRC_DIV                       0x0500000E
#define SCLK_UART5_SRC_DIV                       0x0508000E
#define SCLK_UART4_SRC_SEL                       0x0305000E
#define SCLK_UART4_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART4_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART4_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART4_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART4_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART4_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U
#define SCLK_UART5_SRC_SEL                       0x030D000E
#define SCLK_UART5_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART5_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART5_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART5_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART5_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART5_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U

// TOPCRU_CLKSEL_CON15(Offset:0x33C)
#define SCLK_UART6_SRC_DIV                       0x0500000F
#define SCLK_UART7_SRC_DIV                       0x0508000F
#define SCLK_UART6_SRC_SEL                       0x0305000F
#define SCLK_UART6_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART6_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART6_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART6_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART6_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART6_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U
#define SCLK_UART7_SRC_SEL                       0x030D000F
#define SCLK_UART7_SRC_SEL_XIN_OSC0_FUNC         0U
#define SCLK_UART7_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define SCLK_UART7_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define SCLK_UART7_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define SCLK_UART7_SRC_SEL_CLK_UART_FRAC_MATRIX0 4U
#define SCLK_UART7_SRC_SEL_CLK_UART_FRAC_MATRIX1 5U

// TOPCRU_CLKSEL_CON16(Offset:0x340)
#define MCLK_SAI0_SRC_DIV                        0x08000010
#define MCLK_SAI0_SRC_SEL                        0x04080010
#define MCLK_SAI0_SRC_SEL_XIN_OSC0_FUNC          0U
#define MCLK_SAI0_SRC_SEL_CLK_CM_FRAC_MATRIX0    1U
#define MCLK_SAI0_SRC_SEL_CLK_CM_FRAC_MATRIX1    2U
#define MCLK_SAI0_SRC_SEL_CLK_CM_FRAC_MATRIX2    3U
#define MCLK_SAI0_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define MCLK_SAI0_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define MCLK_SAI0_SRC_SEL_CLK_AUDIO_INT_MATRIX0  6U
#define MCLK_SAI0_SRC_SEL_CLK_AUDIO_INT_MATRIX1  7U
#define MCLK_SAI0_SRC_SEL_MCLK_SAI0_FROM_IO      8U
#define MCLK_ASRC0_SRC_SEL                       0x040C0010
#define MCLK_ASRC0_SRC_SEL_XIN_OSC0_FUNC         0U
#define MCLK_ASRC0_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define MCLK_ASRC0_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define MCLK_ASRC0_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define MCLK_ASRC0_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define MCLK_ASRC0_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define MCLK_ASRC0_SRC_SEL_CLK_AUDIO_INT_MATRIX0 6U
#define MCLK_ASRC0_SRC_SEL_CLK_AUDIO_INT_MATRIX1 7U
#define MCLK_ASRC0_SRC_SEL_MCLK_SAI0_FROM_IO     8U
#define MCLK_ASRC0_SRC_SEL_MCLK_SAI1_FROM_IO     9U
#define MCLK_ASRC0_SRC_SEL_MCLK_SAI2_FROM_IO     10U

// TOPCRU_CLKSEL_CON17(Offset:0x344)
#define MCLK_SAI1_SRC_DIV                        0x08000011
#define MCLK_SAI1_SRC_SEL                        0x04080011
#define MCLK_SAI1_SRC_SEL_XIN_OSC0_FUNC          0U
#define MCLK_SAI1_SRC_SEL_CLK_CM_FRAC_MATRIX0    1U
#define MCLK_SAI1_SRC_SEL_CLK_CM_FRAC_MATRIX1    2U
#define MCLK_SAI1_SRC_SEL_CLK_CM_FRAC_MATRIX2    3U
#define MCLK_SAI1_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define MCLK_SAI1_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define MCLK_SAI1_SRC_SEL_CLK_AUDIO_INT_MATRIX0  6U
#define MCLK_SAI1_SRC_SEL_CLK_AUDIO_INT_MATRIX1  7U
#define MCLK_SAI1_SRC_SEL_MCLK_SAI1_FROM_IO      8U
#define MCLK_ASRC1_SRC_SEL                       0x040C0011
#define MCLK_ASRC1_SRC_SEL_XIN_OSC0_FUNC         0U
#define MCLK_ASRC1_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define MCLK_ASRC1_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define MCLK_ASRC1_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define MCLK_ASRC1_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define MCLK_ASRC1_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define MCLK_ASRC1_SRC_SEL_CLK_AUDIO_INT_MATRIX0 6U
#define MCLK_ASRC1_SRC_SEL_CLK_AUDIO_INT_MATRIX1 7U
#define MCLK_ASRC1_SRC_SEL_MCLK_SAI0_FROM_IO     8U
#define MCLK_ASRC1_SRC_SEL_MCLK_SAI1_FROM_IO     9U
#define MCLK_ASRC1_SRC_SEL_MCLK_SAI2_FROM_IO     10U

// TOPCRU_CLKSEL_CON18(Offset:0x348)
#define MCLK_SAI2_SRC_DIV                        0x08000012
#define MCLK_SAI2_SRC_SEL                        0x04080012
#define MCLK_SAI2_SRC_SEL_XIN_OSC0_FUNC          0U
#define MCLK_SAI2_SRC_SEL_CLK_CM_FRAC_MATRIX0    1U
#define MCLK_SAI2_SRC_SEL_CLK_CM_FRAC_MATRIX1    2U
#define MCLK_SAI2_SRC_SEL_CLK_CM_FRAC_MATRIX2    3U
#define MCLK_SAI2_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define MCLK_SAI2_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define MCLK_SAI2_SRC_SEL_CLK_AUDIO_INT_MATRIX0  6U
#define MCLK_SAI2_SRC_SEL_CLK_AUDIO_INT_MATRIX1  7U
#define MCLK_SAI2_SRC_SEL_MCLK_SAI2_FROM_IO      8U
#define MCLK_ASRC2_SRC_SEL                       0x040C0012
#define MCLK_ASRC2_SRC_SEL_XIN_OSC0_FUNC         0U
#define MCLK_ASRC2_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define MCLK_ASRC2_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define MCLK_ASRC2_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define MCLK_ASRC2_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define MCLK_ASRC2_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define MCLK_ASRC2_SRC_SEL_CLK_AUDIO_INT_MATRIX0 6U
#define MCLK_ASRC2_SRC_SEL_CLK_AUDIO_INT_MATRIX1 7U
#define MCLK_ASRC2_SRC_SEL_MCLK_SAI0_FROM_IO     8U
#define MCLK_ASRC2_SRC_SEL_MCLK_SAI1_FROM_IO     9U
#define MCLK_ASRC2_SRC_SEL_MCLK_SAI2_FROM_IO     10U

// TOPCRU_CLKSEL_CON19(Offset:0x34C)
#define MCLK_PDM_SRC_DIV                         0x05000013
#define MCLK_PDM_SRC_SEL                         0x04060013
#define MCLK_PDM_SRC_SEL_XIN_OSC0_FUNC           0U
#define MCLK_PDM_SRC_SEL_CLK_CM_FRAC_MATRIX0     1U
#define MCLK_PDM_SRC_SEL_CLK_CM_FRAC_MATRIX1     2U
#define MCLK_PDM_SRC_SEL_CLK_CM_FRAC_MATRIX2     3U
#define MCLK_PDM_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0  4U
#define MCLK_PDM_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1  5U
#define MCLK_PDM_SRC_SEL_CLK_AUDIO_INT_MATRIX0   6U
#define MCLK_PDM_SRC_SEL_CLK_AUDIO_INT_MATRIX1   7U
#define MCLK_PDM_SRC_SEL_MCLK_SAI0_FROM_IO       8U
#define MCLK_PDM_SRC_SEL_MCLK_SAI1_FROM_IO       9U
#define MCLK_PDM_SRC_SEL_MCLK_SAI2_FROM_IO       10U
#define MCLK_ASRC3_SRC_SEL                       0x040C0013
#define MCLK_ASRC3_SRC_SEL_XIN_OSC0_FUNC         0U
#define MCLK_ASRC3_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define MCLK_ASRC3_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define MCLK_ASRC3_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define MCLK_ASRC3_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define MCLK_ASRC3_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define MCLK_ASRC3_SRC_SEL_CLK_AUDIO_INT_MATRIX0 6U
#define MCLK_ASRC3_SRC_SEL_CLK_AUDIO_INT_MATRIX1 7U
#define MCLK_ASRC3_SRC_SEL_MCLK_SAI0_FROM_IO     8U
#define MCLK_ASRC3_SRC_SEL_MCLK_SAI1_FROM_IO     9U
#define MCLK_ASRC3_SRC_SEL_MCLK_SAI2_FROM_IO     10U

// TOPCRU_CLKSEL_CON20(Offset:0x350)
#define CLKOUT_PDM_SRC_DIV                       0x08000014
#define CLKOUT_PDM_SRC_SEL                       0x04080014
#define CLKOUT_PDM_SRC_SEL_XIN_OSC0_FUNC         0U
#define CLKOUT_PDM_SRC_SEL_CLK_CM_FRAC_MATRIX0   1U
#define CLKOUT_PDM_SRC_SEL_CLK_CM_FRAC_MATRIX1   2U
#define CLKOUT_PDM_SRC_SEL_CLK_CM_FRAC_MATRIX2   3U
#define CLKOUT_PDM_SRC_SEL_CLK_AUDIO_FRAC_MATRIX0 4U
#define CLKOUT_PDM_SRC_SEL_CLK_AUDIO_FRAC_MATRIX1 5U
#define CLKOUT_PDM_SRC_SEL_CLK_AUDIO_INT_MATRIX0 6U
#define CLKOUT_PDM_SRC_SEL_CLK_AUDIO_INT_MATRIX1 7U
#define CLKOUT_PDM_SRC_SEL_MCLK_SAI0_FROM_IO     8U
#define CLKOUT_PDM_SRC_SEL_MCLK_SAI1_FROM_IO     9U
#define CLKOUT_PDM_SRC_SEL_MCLK_SAI2_FROM_IO     10U

// TOPCRU_CLKSEL_CON21(Offset:0x354)
#define CLK_ASRC0_SRC_DIV                        0x05000015
#define CLK_ASRC1_SRC_DIV                        0x05080015
#define CLK_ASRC0_SRC_SEL                        0x01060015
#define CLK_ASRC0_SRC_SEL_CLK_GPLL_MUX           0U
#define CLK_ASRC0_SRC_SEL_CLK_AUPLL_MUX          1U
#define CLK_ASRC1_SRC_SEL                        0x010E0015
#define CLK_ASRC1_SRC_SEL_CLK_GPLL_MUX           0U
#define CLK_ASRC1_SRC_SEL_CLK_AUPLL_MUX          1U

// TOPCRU_CLKSEL_CON25(Offset:0x364)
#define CLK_CM_FRAC_MATRIX0_DIV                  0x20000019

// TOPCRU_CLKSEL_CON26(Offset:0x368)
#define CLK_CM_FRAC_MATRIX1_DIV                  0x2000001A

// TOPCRU_CLKSEL_CON27(Offset:0x36C)
#define CLK_CM_FRAC_MATRIX2_DIV                  0x2000001B

// TOPCRU_CLKSEL_CON28(Offset:0x370)
#define CLK_UART_FRAC_MATRIX0_DIV                0x2000001C

// TOPCRU_CLKSEL_CON29(Offset:0x374)
#define CLK_UART_FRAC_MATRIX1_DIV                0x2000001D

// TOPCRU_CLKSEL_CON30(Offset:0x378)
#define CLK_AUDIO_FRAC_MATRIX0_DIV               0x2000001E

// TOPCRU_CLKSEL_CON31(Offset:0x37C)
#define CLK_AUDIO_FRAC_MATRIX1_DIV               0x2000001F

// TOPCRU_CLKSEL_CON40(Offset:0x3A0)
#define CLK_CORE_RGA_SRC_DIV                     0x03020028
#define CLK_SYS_DSMC_SRC_DIV                     0x05090028
#define ACLK_VEPU_SRC_SEL                        0x02000028
#define ACLK_VEPU_SRC_SEL_CLK_CPLL_DIV2_SRC      0U
#define ACLK_VEPU_SRC_SEL_CLK_GPLL_DIV3_SRC      1U
#define ACLK_VEPU_SRC_SEL_CLK_GPLL_DIV6_SRC      2U
#define CLK_CORE_RGA_SRC_SEL                     0x01050028
#define CLK_CORE_RGA_SRC_SEL_CLK_GPLL_MUX        0U
#define CLK_CORE_RGA_SRC_SEL_CLK_CPLL_MUX        1U
#define ACLK_GMAC_SRC_SEL                        0x01060028
#define ACLK_GMAC_SRC_SEL_CLK_GPLL_DIV4_SRC      0U
#define ACLK_GMAC_SRC_SEL_CLK_GPLL_DIV6_SRC      1U
#define ACLK_VI_SRC_SEL                          0x02070028
#define ACLK_VI_SRC_SEL_CLK_CPLL_DIV2_SRC        0U
#define ACLK_VI_SRC_SEL_CLK_GPLL_DIV3_SRC        1U
#define ACLK_VI_SRC_SEL_CLK_GPLL_DIV4_SRC        2U
#define CLK_SYS_DSMC_SRC_SEL                     0x020E0028
#define CLK_SYS_DSMC_SRC_SEL_XIN_OSC0_FUNC       0U
#define CLK_SYS_DSMC_SRC_SEL_CLK_GPLL_MUX        1U
#define CLK_SYS_DSMC_SRC_SEL_CLK_CPLL_MUX        2U

// TOPCRU_CLKSEL_CON42(Offset:0x3A8)
#define ACLK_VDO_SRC_DIV                         0x0400002A
#define ACLK_RKVDEC_SRC_DIV                      0x0406002A
#define ACLK_VDO_SRC_SEL                         0x0104002A
#define ACLK_VDO_SRC_SEL_CLK_GPLL_MUX            0U
#define ACLK_VDO_SRC_SEL_CLK_CPLL_MUX            1U
#define DCLK_VICAP_SRC_SEL                       0x0105002A
#define DCLK_VICAP_SRC_SEL_CLK_CPLL_DIV3_SRC     0U
#define DCLK_VICAP_SRC_SEL_CLK_GPLL_DIV6_SRC     1U
#define ACLK_RKVDEC_SRC_SEL                      0x010A002A
#define ACLK_RKVDEC_SRC_SEL_CLK_GPLL_MUX         0U
#define ACLK_RKVDEC_SRC_SEL_CLK_CPLL_MUX         1U

// TOPCRU_CLKSEL_CON43(Offset:0x3AC)
#define DCLK_VOP_SRC_DIV                         0x0800002B
#define DCLK_VOP_SRC_SEL                         0x0108002B
#define DCLK_VOP_SRC_SEL_CLK_GPLL_MUX            0U
#define DCLK_VOP_SRC_SEL_CLK_CPLL_MUX            1U

// TOPCRU_CLKSEL_CON44(Offset:0x3B0)
#define ACLK_SYSMEM_SRC_DIV                      0x0300002C
#define ACLK_SYSMEM_SRC_SEL                      0x0103002C
#define ACLK_SYSMEM_SRC_SEL_CLK_GPLL_MUX         0U
#define ACLK_SYSMEM_SRC_SEL_CLK_CPLL_MUX         1U
#define ACLK_TOP_SRC_SEL                         0x0206002C
#define ACLK_TOP_SRC_SEL_CLK_GPLL_DIV2_SRC       0U
#define ACLK_TOP_SRC_SEL_CLK_GPLL_DIV3_SRC       1U
#define ACLK_TOP_SRC_SEL_CLK_GPLL_DIV6_SRC       2U
#define ACLK_BUS_SRC_SEL                         0x0208002C
#define ACLK_BUS_SRC_SEL_CLK_GPLL_DIV3_SRC       0U
#define ACLK_BUS_SRC_SEL_CLK_GPLL_DIV4_SRC       1U
#define ACLK_BUS_SRC_SEL_CLK_GPLL_DIV6_SRC       2U
#define HCLK_BUS_SRC_SEL                         0x010A002C
#define HCLK_BUS_SRC_SEL_CLK_GPLL_DIV6_SRC       0U
#define HCLK_BUS_SRC_SEL_CLK_CPLL_DIV10_SRC      1U

// TOPCRU_CLKSEL_CON45(Offset:0x3B4)
#define CCLK_SDMMC0_SRC_DIV                      0x0800002D
#define CLK_GMAC_PTP_REF_SRC_DIV                 0x050B002D
#define CCLK_SDMMC0_SRC_SEL                      0x0208002D
#define CCLK_SDMMC0_SRC_SEL_CLK_GPLL_MUX         0U
#define CCLK_SDMMC0_SRC_SEL_CLK_CPLL_MUX         1U
#define CCLK_SDMMC0_SRC_SEL_XIN_OSC0_FUNC        2U
#define CLK_GMAC_PTP_REF_SRC_SEL                 0x010A002D
#define CLK_GMAC_PTP_REF_SRC_SEL_CLK_CPLL_MUX    0U
#define CLK_GMAC_PTP_REF_SRC_SEL_XIN_OSC0_FUNC   1U

// TOPCRU_CLKSEL_CON46(Offset:0x3B8)
#define CCLK_SDMMC1_SRC_DIV                      0x0800002E
#define CCLK_SDMMC1_SRC_SEL                      0x0208002E
#define CCLK_SDMMC1_SRC_SEL_CLK_GPLL_MUX         0U
#define CCLK_SDMMC1_SRC_SEL_CLK_CPLL_MUX         1U
#define CCLK_SDMMC1_SRC_SEL_XIN_OSC0_FUNC        2U
#define CLK_TIMER_SRC_SEL                        0x010B002E
#define CLK_TIMER_SRC_SEL_CLK_CPLL_DIV10_SRC     0U
#define CLK_TIMER_SRC_SEL_XIN_OSC0_FUNC          1U
#define TCLK_WDT_NS_SRC_SEL                      0x010C002E
#define TCLK_WDT_NS_SRC_SEL_CLK_CPLL_DIV10_SRC   0U
#define TCLK_WDT_NS_SRC_SEL_XIN_OSC0_FUNC        1U
#define TCLK_WDT_S_SRC_SEL                       0x010D002E
#define TCLK_WDT_S_SRC_SEL_CLK_CPLL_DIV10_SRC    0U
#define TCLK_WDT_S_SRC_SEL_XIN_OSC0_FUNC         1U
#define TCLK_WDT_HPMCU_SRC_SEL                   0x010E002E
#define TCLK_WDT_HPMCU_SRC_SEL_CLK_CPLL_DIV10_SRC 0U
#define TCLK_WDT_HPMCU_SRC_SEL_XIN_OSC0_FUNC     1U

// TOPCRU_CLKSEL_CON47(Offset:0x3BC)
#define CCLK_EMMC_SRC_DIV                        0x0800002F
#define CCLK_EMMC_SRC_SEL                        0x0208002F
#define CCLK_EMMC_SRC_SEL_CLK_GPLL_MUX           0U
#define CCLK_EMMC_SRC_SEL_CLK_CPLL_MUX           1U
#define CCLK_EMMC_SRC_SEL_XIN_OSC0_FUNC          2U
#define PCLK_PERI_SRC_SEL                        0x010C002F
#define PCLK_PERI_SRC_SEL_CLK_CPLL_DIV10_SRC     0U
#define PCLK_PERI_SRC_SEL_XIN_OSC0_FUNC          1U
#define ACLK_PERI_SRC_SEL                        0x010D002F
#define ACLK_PERI_SRC_SEL_CLK_GPLL_DIV6_SRC      0U
#define ACLK_PERI_SRC_SEL_XIN_OSC0_FUNC          1U

// TOPCRU_CLKSEL_CON48(Offset:0x3C0)
#define SCLK_2X_FSPI0_SRC_DIV                    0x08000030
#define SCLK_2X_FSPI0_SRC_SEL                    0x02080030
#define SCLK_2X_FSPI0_SRC_SEL_CLK_GPLL_MUX       0U
#define SCLK_2X_FSPI0_SRC_SEL_CLK_CPLL_MUX       1U
#define SCLK_2X_FSPI0_SRC_SEL_XIN_OSC0_FUNC      2U
#define ACLK_VCP_SRC_SEL                         0x020C0030
#define ACLK_VCP_SRC_SEL_CLK_CPLL_DIV2_SRC       0U
#define ACLK_VCP_SRC_SEL_CLK_GPLL_DIV3_SRC       1U
#define ACLK_VCP_SRC_SEL_CLK_GPLL_DIV6_SRC       2U

// TOPCRU_CLKSEL_CON49(Offset:0x3C4)
#define CLK_CAN0_SRC_DIV                         0x05000031
#define CLK_CAN1_SRC_DIV                         0x05080031
#define CLK_CAN0_SRC_SEL                         0x02060031
#define CLK_CAN0_SRC_SEL_CLK_GPLL_MUX            0U
#define CLK_CAN0_SRC_SEL_CLK_CPLL_MUX            1U
#define CLK_CAN0_SRC_SEL_XIN_OSC0_FUNC           2U
#define CLK_CAN1_SRC_SEL                         0x020E0031
#define CLK_CAN1_SRC_SEL_CLK_GPLL_MUX            0U
#define CLK_CAN1_SRC_SEL_CLK_CPLL_MUX            1U
#define CLK_CAN1_SRC_SEL_XIN_OSC0_FUNC           2U

// TOPCRU_CLKSEL_CON50(Offset:0x3C8)
#define CLK_I2C_BUS_SRC_SEL                      0x01010032
#define CLK_I2C_BUS_SRC_SEL_CLK_GPLL_DIV6_SRC    0U
#define CLK_I2C_BUS_SRC_SEL_XIN_OSC0_FUNC        1U
#define CLK_SPI0_SRC_SEL                         0x02020032
#define CLK_SPI0_SRC_SEL_CLK_GPLL_DIV6_SRC       0U
#define CLK_SPI0_SRC_SEL_CLK_CPLL_DIV10_SRC      1U
#define CLK_SPI0_SRC_SEL_CLK_CPLL_DIV20_SRC      2U
#define CLK_SPI0_SRC_SEL_XIN_OSC0_FUNC           3U
#define CLK_SPI1_SRC_SEL                         0x02040032
#define CLK_SPI1_SRC_SEL_CLK_GPLL_DIV6_SRC       0U
#define CLK_SPI1_SRC_SEL_CLK_CPLL_DIV10_SRC      1U
#define CLK_SPI1_SRC_SEL_CLK_CPLL_DIV20_SRC      2U
#define CLK_SPI1_SRC_SEL_XIN_OSC0_FUNC           3U
#define CLK_PWM0_SRC_SEL                         0x01080032
#define CLK_PWM0_SRC_SEL_CLK_CPLL_DIV10_SRC      0U
#define CLK_PWM0_SRC_SEL_XIN_OSC0_FUNC           1U
#define CLK_PWM2_SRC_SEL                         0x010A0032
#define CLK_PWM2_SRC_SEL_CLK_CPLL_DIV10_SRC      0U
#define CLK_PWM2_SRC_SEL_XIN_OSC0_FUNC           1U
#define CLK_PWM3_SRC_SEL                         0x010B0032
#define CLK_PWM3_SRC_SEL_CLK_CPLL_DIV10_SRC      0U
#define CLK_PWM3_SRC_SEL_XIN_OSC0_FUNC           1U
#define CLK_PKA_RKCE_SRC_SEL                     0x010C0032
#define CLK_PKA_RKCE_SRC_SEL_CLK_GPLL_DIV4_SRC   0U
#define CLK_PKA_RKCE_SRC_SEL_CLK_GPLL_DIV6_SRC   1U
#define ACLK_RKCE_SRC_SEL                        0x010D0032
#define ACLK_RKCE_SRC_SEL_CLK_GPLL_DIV6_SRC      0U
#define ACLK_RKCE_SRC_SEL_XIN_OSC0_FUNC          1U

// TOPCRU_CLKSEL_CON51(Offset:0x3CC)
#define CLK_CORE_FEC_SRC_DIV                     0x03000033
#define CLK_CORE_FEC_SRC_SEL                     0x01030033
#define CLK_CORE_FEC_SRC_SEL_CLK_GPLL_MUX        0U
#define CLK_CORE_FEC_SRC_SEL_CLK_CPLL_MUX        1U

// TOPCRU_CLKSEL_CON60(Offset:0x3F0)
#define CLK_CORE_PLL_SRC_DIV                     0x0300003C
#define CLK_NPU_PLL_SRC_DIV                      0x0306003C

// TOPCRU_CLKSEL_CON61(Offset:0x3F4)
#define CLK_VEPU_PLL_SRC_DIV                     0x0300003D
#define CLK_ISP_PLL_SRC_DIV                      0x0406003D
#define CLK_VEPU_PLL_SRC_SEL                     0x0204003D
#define CLK_VEPU_PLL_SRC_SEL_CLK_GPLL_MUX        0U
#define CLK_VEPU_PLL_SRC_SEL_CLK_AUPLL_MUX       1U
#define CLK_VEPU_PLL_SRC_SEL_CLK_CPLL_MUX        2U
#define CLK_ISP_PLL_SRC_SEL                      0x020A003D
#define CLK_ISP_PLL_SRC_SEL_CLK_GPLL_MUX         0U
#define CLK_ISP_PLL_SRC_SEL_CLK_AUPLL_MUX        1U
#define CLK_ISP_PLL_SRC_SEL_CLK_CPLL_MUX         2U

// TOPCRU_CLKSEL_CON62(Offset:0x3F8)
#define CLK_AISP_PLL_SRC_DIV                     0x0300003E
#define DCLK_OOC_SRC_DIV                         0x0808003E
#define CLK_AISP_PLL_SRC_SEL                     0x0204003E
#define CLK_AISP_PLL_SRC_SEL_CLK_GPLL_MUX        0U
#define CLK_AISP_PLL_SRC_SEL_CLK_AUPLL_MUX       1U
#define CLK_AISP_PLL_SRC_SEL_CLK_CPLL_MUX        2U
#define DCLK_OOC_SRC_SEL                         0x0107003E
#define DCLK_OOC_SRC_SEL_CLK_GPLL_MUX            0U
#define DCLK_OOC_SRC_SEL_CLK_CPLL_MUX            1U

// TOPCRU_CLKSEL_CON63(Offset:0x3FC)
#define CLK_SARADC0_SRC_DIV                      0x0300003F
#define CLK_SARADC1_SRC_DIV                      0x0304003F
#define CLK_SARADC2_SRC_DIV                      0x0308003F
#define CLK_SARADC0_SRC_SEL                      0x010C003F
#define CLK_SARADC0_SRC_SEL_CLK_GPLL_DIV6_SRC    0U
#define CLK_SARADC0_SRC_SEL_XIN_OSC0_FUNC        1U
#define CLK_SARADC1_SRC_SEL                      0x010D003F
#define CLK_SARADC1_SRC_SEL_CLK_GPLL_DIV6_SRC    0U
#define CLK_SARADC1_SRC_SEL_XIN_OSC0_FUNC        1U
#define CLK_SARADC2_SRC_SEL                      0x010E003F
#define CLK_SARADC2_SRC_SEL_CLK_GPLL_DIV6_SRC    0U
#define CLK_SARADC2_SRC_SEL_XIN_OSC0_FUNC        1U

// TOPCRU_CLKSEL_CON67(Offset:0x40C)
#define CLK_MIPI0_OUT2IO_DIV                     0x05000043
#define CLK_MIPI1_OUT2IO_DIV                     0x05060043
#define CLK_MIPI0_OUT2IO_SEL                     0x010B0043
#define CLK_MIPI0_OUT2IO_SEL_CLK_GPLL_DIV2_SRC   0U
#define CLK_MIPI0_OUT2IO_SEL_XIN_OSC0_FUNC       1U
#define CLK_MIPI1_OUT2IO_SEL                     0x010C0043
#define CLK_MIPI1_OUT2IO_SEL_CLK_GPLL_DIV2_SRC   0U
#define CLK_MIPI1_OUT2IO_SEL_XIN_OSC0_FUNC       1U

// TOPCRU_CLKSEL_CON68(Offset:0x410)
#define CLK_MIPI2_OUT2IO_DIV                     0x05000044
#define CLK_MIPI3_OUT2IO_DIV                     0x05060044
#define CLK_MIPI2_OUT2IO_SEL                     0x010B0044
#define CLK_MIPI2_OUT2IO_SEL_CLK_GPLL_DIV2_SRC   0U
#define CLK_MIPI2_OUT2IO_SEL_XIN_OSC0_FUNC       1U
#define CLK_MIPI3_OUT2IO_SEL                     0x010C0044
#define CLK_MIPI3_OUT2IO_SEL_CLK_GPLL_DIV2_SRC   0U
#define CLK_MIPI3_OUT2IO_SEL_XIN_OSC0_FUNC       1U

// TOPCRU_CLKSEL_CON69(Offset:0x414)
#define CLK_CIF_OUT2IO_DIV                       0x05000045
#define CLK_MAC_OUT2IO_DIV                       0x07080045
#define CLK_CIF_OUT2IO_SEL                       0x01050045
#define CLK_CIF_OUT2IO_SEL_CLK_GPLL_DIV2_SRC     0U
#define CLK_CIF_OUT2IO_SEL_XIN_OSC0_FUNC         1U
#define CLK_MAC_OUT2IO_SEL                       0x02060045
#define CLK_MAC_OUT2IO_SEL_CLK_GPLL_MUX          0U
#define CLK_MAC_OUT2IO_SEL_CLK_CPLL_MUX          1U
#define CLK_MAC_OUT2IO_SEL_XIN_OSC0_FUNC         2U

// TOPCRU_CLKSEL_CON70(Offset:0x418)
#define MCLK_SAI0_OUT2IO_DIV                     0x04000046
#define MCLK_SAI1_OUT2IO_DIV                     0x04050046
#define MCLK_SAI2_OUT2IO_DIV                     0x040A0046

// ======================= BUSCRU module definition bank=1 ========================
// BUSCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_ARESETN_TOP_BIU                     0x00001000
#define SRST_ARESETN_RKCE_BIU                    0x00001001
#define SRST_ARESETN_BUS_BIU                     0x00001002
#define SRST_HRESETN_BUS_BIU                     0x00001003
#define SRST_PRESETN_BUS_BIU                     0x00001004
#define SRST_PRESETN_CRU_BUS                     0x00001005
#define SRST_PRESETN_SYS_GRF                     0x00001006
#define SRST_HRESETN_BOOTROM                     0x00001007
#define SRST_ARESETN_GIC400                      0x00001008
#define SRST_ARESETN_SPINLOCK                    0x00001009
#define SRST_PRESETN_WDT_NS                      0x0000100A
#define SRST_TRESETN_WDT_NS                      0x0000100B

// BUSCRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_WDT_HPMCU                   0x00001010
#define SRST_TRESETN_WDT_HPMCU                   0x00001011
#define SRST_HRESETN_CACHE                       0x00001012
#define SRST_PRESETN_HPMCU_MAILBOX               0x00001013
#define SRST_PRESETN_HPMCU_INTMUX                0x00001014
#define SRST_RESETN_HPMCU_FULL_CLUSTER           0x00001015
#define SRST_RESETN_HPMCU_PWUP                   0x00001016
#define SRST_RESETN_HPMCU_ONLY_CORE              0x00001017
#define SRST_TRESETN_HPMCU_JTAG                  0x00001018
#define SRST_PRESETN_RKDMA                       0x0000101B
#define SRST_ARESETN_RKDMA                       0x0000101C

// BUSCRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_PRESETN_DCF                         0x00001020
#define SRST_ARESETN_DCF                         0x00001021
#define SRST_HRESETN_RGA                         0x00001022
#define SRST_ARESETN_RGA                         0x00001023
#define SRST_RESETN_CORE_RGA                     0x00001024
#define SRST_PRESETN_TIMER                       0x00001025
#define SRST_RESETN_TIMER0                       0x00001026
#define SRST_RESETN_TIMER1                       0x00001027
#define SRST_RESETN_TIMER2                       0x00001028
#define SRST_RESETN_TIMER3                       0x00001029
#define SRST_RESETN_TIMER4                       0x0000102A
#define SRST_RESETN_TIMER5                       0x0000102B
#define SRST_ARESETN_RKCE                        0x0000102C
#define SRST_RESETN_PKA_RKCE                     0x0000102D
#define SRST_HRESETN_RKRNG_S                     0x0000102E
#define SRST_HRESETN_RKRNG_NS                    0x0000102F

// BUSCRU_SOFTRST_CON03(Offset:0xA0C)
#define SRST_PRESETN_I2C0                        0x00001030
#define SRST_RESETN_I2C0                         0x00001031
#define SRST_PRESETN_I2C1                        0x00001032
#define SRST_RESETN_I2C1                         0x00001033
#define SRST_PRESETN_I2C3                        0x00001034
#define SRST_RESETN_I2C3                         0x00001035
#define SRST_PRESETN_I2C4                        0x00001036
#define SRST_RESETN_I2C4                         0x00001037
#define SRST_PRESETN_I2C5                        0x00001038
#define SRST_RESETN_I2C5                         0x00001039
#define SRST_PRESETN_SPI0                        0x0000103A
#define SRST_RESETN_SPI0                         0x0000103B
#define SRST_PRESETN_SPI1                        0x0000103C
#define SRST_RESETN_SPI1                         0x0000103D

// BUSCRU_SOFTRST_CON04(Offset:0xA10)
#define SRST_PRESETN_PWM0                        0x00001040
#define SRST_RESETN_PWM0                         0x00001041
#define SRST_PRESETN_PWM2                        0x00001044
#define SRST_RESETN_PWM2                         0x00001045
#define SRST_PRESETN_PWM3                        0x00001048
#define SRST_RESETN_PWM3                         0x00001049

// BUSCRU_SOFTRST_CON05(Offset:0xA14)
#define SRST_PRESETN_UART1                       0x00001050
#define SRST_SRESETN_UART1                       0x00001051
#define SRST_PRESETN_UART2                       0x00001052
#define SRST_SRESETN_UART2                       0x00001053
#define SRST_PRESETN_UART3                       0x00001054
#define SRST_SRESETN_UART3                       0x00001055
#define SRST_PRESETN_UART4                       0x00001056
#define SRST_SRESETN_UART4                       0x00001057
#define SRST_PRESETN_UART5                       0x00001058
#define SRST_SRESETN_UART5                       0x00001059
#define SRST_PRESETN_UART6                       0x0000105A
#define SRST_SRESETN_UART6                       0x0000105B
#define SRST_PRESETN_UART7                       0x0000105C
#define SRST_SRESETN_UART7                       0x0000105D

// BUSCRU_SOFTRST_CON06(Offset:0xA18)
#define SRST_PRESETN_TSADC                       0x00001060
#define SRST_RESETN_TSADC                        0x00001061
#define SRST_HRESETN_SAI0                        0x00001062
#define SRST_MRESETN_SAI0                        0x00001063
#define SRST_HRESETN_SAI1                        0x00001064
#define SRST_MRESETN_SAI1                        0x00001065
#define SRST_HRESETN_SAI2                        0x00001066
#define SRST_MRESETN_SAI2                        0x00001067
#define SRST_HRESETN_RKDSM                       0x00001068
#define SRST_MRESETN_RKDSM                       0x00001069
#define SRST_HRESETN_PDM                         0x0000106A
#define SRST_MRESETN_PDM                         0x0000106B
#define SRST_RESETN_PDM                          0x0000106C

// BUSCRU_SOFTRST_CON07(Offset:0xA1C)
#define SRST_HRESETN_ASRC0                       0x00001070
#define SRST_RESETN_ASRC0                        0x00001071
#define SRST_HRESETN_ASRC1                       0x00001072
#define SRST_RESETN_ASRC1                        0x00001073
#define SRST_PRESETN_AUDIO_ADC_BUS               0x00001074
#define SRST_MRESETN_AUDIO_ADC_BUS               0x00001075
#define SRST_PRESETN_RKCE                        0x00001076
#define SRST_HRESETN_NS_RKCE                     0x00001077
#define SRST_PRESETN_OTPC_NS                     0x00001078
#define SRST_RESETN_SBPI_OTPC_NS                 0x00001079
#define SRST_RESETN_USER_OTPC_NS                 0x0000107A
#define SRST_RESETN_OTPC_ARB                     0x0000107B
#define SRST_PRESETN_OTP_MASK                    0x0000107C

// BUSCRU_GATE_CON00(Offset:0x800)
#define ACLK_TOP_BIU_GATE                        0x00001000
#define ACLK_RKCE_BIU_GATE                       0x00001001
#define ACLK_BUS_BIU_GATE                        0x00001002
#define HCLK_BUS_BIU_GATE                        0x00001003
#define PCLK_BUS_BIU_GATE                        0x00001004
#define PCLK_CRU_BUS_GATE                        0x00001005
#define PCLK_SYS_GRF_GATE                        0x00001006
#define HCLK_BOOTROM_GATE                        0x00001007
#define ACLK_GIC400_GATE                         0x00001008
#define ACLK_SPINLOCK_GATE                       0x00001009
#define PCLK_WDT_NS_GATE                         0x0000100A
#define TCLK_WDT_NS_GATE                         0x0000100B
#define CLK_TESTOUT_BUS_GATE                     0x0000100F

// BUSCRU_GATE_CON01(Offset:0x804)
#define PCLK_WDT_HPMCU_GATE                      0x00001010
#define HCLK_CACHE_GATE                          0x00001012
#define PCLK_HPMCU_MAILBOX_GATE                  0x00001013
#define PCLK_HPMCU_INTMUX_GATE                   0x00001014
#define CLK_HPMCU_GATE                           0x00001015
#define CLK_HPMCU_RTC_GATE                       0x0000101A
#define PCLK_RKDMA_GATE                          0x0000101B
#define ACLK_RKDMA_GATE                          0x0000101C

// BUSCRU_GATE_CON02(Offset:0x808)
#define PCLK_DCF_GATE                            0x00001020
#define ACLK_DCF_GATE                            0x00001021
#define HCLK_RGA_GATE                            0x00001022
#define ACLK_RGA_GATE                            0x00001023
#define CLK_CORE_RGA_GATE                        0x00001024
#define PCLK_TIMER_GATE                          0x00001025
#define CLK_TIMER0_GATE                          0x00001026
#define CLK_TIMER1_GATE                          0x00001027
#define CLK_TIMER2_GATE                          0x00001028
#define CLK_TIMER3_GATE                          0x00001029
#define CLK_TIMER4_GATE                          0x0000102A
#define CLK_TIMER5_GATE                          0x0000102B
#define ACLK_RKCE_GATE                           0x0000102C
#define CLK_PKA_RKCE_GATE                        0x0000102D
#define HCLK_RKRNG_S_GATE                        0x0000102E
#define HCLK_RKRNG_NS_GATE                       0x0000102F

// BUSCRU_GATE_CON03(Offset:0x80C)
#define PCLK_I2C0_GATE                           0x00001030
#define CLK_I2C0_GATE                            0x00001031
#define PCLK_I2C1_GATE                           0x00001032
#define CLK_I2C1_GATE                            0x00001033
#define PCLK_I2C3_GATE                           0x00001034
#define CLK_I2C3_GATE                            0x00001035
#define PCLK_I2C4_GATE                           0x00001036
#define CLK_I2C4_GATE                            0x00001037
#define PCLK_I2C5_GATE                           0x00001038
#define CLK_I2C5_GATE                            0x00001039
#define PCLK_SPI0_GATE                           0x0000103A
#define PCLK_SPI1_GATE                           0x0000103C

// BUSCRU_GATE_CON04(Offset:0x810)
#define PCLK_PWM0_GATE                           0x00001040
#define CLK_OSC_PWM0_GATE                        0x00001041
#define CLK_RC_PWM0_GATE                         0x00001042
#define PCLK_PWM2_GATE                           0x00001043
#define CLK_OSC_PWM2_GATE                        0x00001044
#define CLK_RC_PWM2_GATE                         0x00001045
#define PCLK_PWM3_GATE                           0x00001046
#define CLK_OSC_PWM3_GATE                        0x00001047
#define CLK_RC_PWM3_GATE                         0x00001048
#define PCLK_UART1_GATE                          0x00001049
#define PCLK_UART2_GATE                          0x0000104A
#define PCLK_UART3_GATE                          0x0000104B
#define PCLK_UART4_GATE                          0x0000104C
#define PCLK_UART5_GATE                          0x0000104D
#define PCLK_UART6_GATE                          0x0000104E
#define PCLK_UART7_GATE                          0x0000104F

// BUSCRU_GATE_CON05(Offset:0x814)
#define PCLK_TSADC_GATE                          0x00001050
#define CLK_TSADC_GATE                           0x00001051
#define HCLK_SAI0_GATE                           0x00001052
#define HCLK_SAI1_GATE                           0x00001054
#define HCLK_SAI2_GATE                           0x00001056
#define HCLK_RKDSM_GATE                          0x00001058
#define MCLK_RKDSM_GATE                          0x00001059
#define HCLK_PDM_GATE                            0x0000105A
#define HCLK_ASRC0_GATE                          0x0000105B
#define HCLK_ASRC1_GATE                          0x0000105C
#define PCLK_AUDIO_ADC_BUS_GATE                  0x0000105D
#define MCLK_AUDIO_ADC_BUS_GATE                  0x0000105E

// BUSCRU_GATE_CON06(Offset:0x818)
#define PCLK_RKCE_GATE                           0x00001060
#define HCLK_NS_RKCE_GATE                        0x00001061
#define PCLK_OTPC_NS_GATE                        0x00001062
#define CLK_SBPI_OTPC_NS_GATE                    0x00001063
#define CLK_USER_OTPC_NS_GATE                    0x00001064
#define CLK_OTPC_ARB_GATE                        0x00001065
#define PCLK_OTP_MASK_GATE                       0x00001066
#define CLK_TSADC_PHYCTRL_GATE                   0x00001068
#define CLK_HPMCU_JTAG_DFT_GATE                  0x0000106D
#define SCLKIN_SPI0_DFT_GATE                     0x0000106E
#define SCLKIN_SPI1_DFT_GATE                     0x0000106F

// BUSCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_BUS_DIV                      0x04000100
#define CLK_TESTOUT_BUS_MUX0_SEL                 0x05040100
#define CLK_TESTOUT_BUS_MUX1_SEL                 0x040A0100
#define CLK_TESTOUT_BUS_MUX1_SEL_CLK_HPMCU_JTAG_IO 0U
#define CLK_TESTOUT_BUS_MUX1_SEL_SCLKIN_SPI0_IO  1U
#define CLK_TESTOUT_BUS_MUX1_SEL_SCLKIN_SPI1_IO  2U
#define CLK_TESTOUT_BUS_MUX1_SEL_MCLK_SAI0_FROM_IO 3U
#define CLK_TESTOUT_BUS_MUX1_SEL_SCLK_SAI0_FROM_IO 4U
#define CLK_TESTOUT_BUS_MUX1_SEL_MCLK_SAI1_FROM_IO 5U
#define CLK_TESTOUT_BUS_MUX1_SEL_SCLK_SAI1_FROM_IO 6U
#define CLK_TESTOUT_BUS_MUX1_SEL_MCLK_SAI2_FROM_IO 7U
#define CLK_TESTOUT_BUS_MUX1_SEL_SCLK_SAI2_FROM_IO 8U
#define CLK_TESTOUT_BUS_MUX1_SEL_CLK_PWM0        9U
#define CLK_TESTOUT_BUS_MUX1_SEL_CLK_PWM2        10U
#define CLK_TESTOUT_BUS_MUX1_SEL_CLK_PWM3        11U
#define CLK_TESTOUT_BUS_MUX1_SEL_DBG2IO_CH_CLK_O_FROM_TSADC 12U
#define CLK_TESTOUT_BUS_SEL                      0x010E0100
#define CLK_TESTOUT_BUS_SEL_CLK_TESTOUT_BUS_MUX0 0U
#define CLK_TESTOUT_BUS_SEL_CLK_TESTOUT_BUS_MUX1 1U

// BUSCRU_CLKSEL_CON02(Offset:0x308)
#define CLK_USER_OTPC_NS_DIV                     0x030C0102
#define CLK_TIMER0_SEL                           0x02000102
#define CLK_TIMER0_SEL_CLK_TIMER_ROOT            0U
#define CLK_TIMER0_SEL_MCLK_SAI0_FROM_IO         1U
#define CLK_TIMER0_SEL_SCLK_SAI0_FROM_IO         2U
#define CLK_TIMER1_SEL                           0x02020102
#define CLK_TIMER1_SEL_CLK_TIMER_ROOT            0U
#define CLK_TIMER1_SEL_MCLK_SAI1_FROM_IO         1U
#define CLK_TIMER1_SEL_SCLK_SAI1_FROM_IO         2U
#define CLK_TIMER2_SEL                           0x02040102
#define CLK_TIMER2_SEL_CLK_TIMER_ROOT            0U
#define CLK_TIMER2_SEL_MCLK_SAI2_FROM_IO         1U
#define CLK_TIMER2_SEL_SCLK_SAI2_FROM_IO         2U
#define CLK_TIMER3_SEL                           0x02060102
#define CLK_TIMER3_SEL_CLK_TIMER_ROOT            0U
#define CLK_TIMER3_SEL_MCLK_ASRC0_SRC            1U
#define CLK_TIMER3_SEL_MCLK_ASRC1_SRC            2U
#define CLK_TIMER4_SEL                           0x02080102
#define CLK_TIMER4_SEL_CLK_TIMER_ROOT            0U
#define CLK_TIMER4_SEL_MCLK_ASRC2_SRC            1U
#define CLK_TIMER4_SEL_MCLK_ASRC3_SRC            2U

// BUSCRU_CLKSEL_CON03(Offset:0x30C)
#define LRCK_SRC_ASRC0_SEL                       0x03000103
#define LRCK_SRC_ASRC0_SEL_MCLK_ASRC0_SRC        0U
#define LRCK_SRC_ASRC0_SEL_MCLK_ASRC1_SRC        1U
#define LRCK_SRC_ASRC0_SEL_MCLK_ASRC2_SRC        2U
#define LRCK_SRC_ASRC0_SEL_MCLK_ASRC3_SRC        3U
#define LRCK_SRC_ASRC0_SEL_FS_INTER_FROM_SAI0    4U
#define LRCK_SRC_ASRC0_SEL_FS_INTER_FROM_SAI1    5U
#define LRCK_SRC_ASRC0_SEL_FS_INTER_FROM_SAI2    6U
#define LRCK_SRC_ASRC0_SEL_CLKOUT_PDM            7U
#define LRCK_DST_ASRC0_SEL                       0x03040103
#define LRCK_DST_ASRC0_SEL_MCLK_ASRC0_SRC        0U
#define LRCK_DST_ASRC0_SEL_MCLK_ASRC1_SRC        1U
#define LRCK_DST_ASRC0_SEL_MCLK_ASRC2_SRC        2U
#define LRCK_DST_ASRC0_SEL_MCLK_ASRC3_SRC        3U
#define LRCK_DST_ASRC0_SEL_FS_INTER_FROM_SAI0    4U
#define LRCK_DST_ASRC0_SEL_FS_INTER_FROM_SAI1    5U
#define LRCK_DST_ASRC0_SEL_FS_INTER_FROM_SAI2    6U
#define LRCK_DST_ASRC0_SEL_CLKOUT_PDM            7U
#define LRCK_SRC_ASRC1_SEL                       0x03080103
#define LRCK_SRC_ASRC1_SEL_MCLK_ASRC0_SRC        0U
#define LRCK_SRC_ASRC1_SEL_MCLK_ASRC1_SRC        1U
#define LRCK_SRC_ASRC1_SEL_MCLK_ASRC2_SRC        2U
#define LRCK_SRC_ASRC1_SEL_MCLK_ASRC3_SRC        3U
#define LRCK_SRC_ASRC1_SEL_FS_INTER_FROM_SAI0    4U
#define LRCK_SRC_ASRC1_SEL_FS_INTER_FROM_SAI1    5U
#define LRCK_SRC_ASRC1_SEL_FS_INTER_FROM_SAI2    6U
#define LRCK_SRC_ASRC1_SEL_CLKOUT_PDM            7U
#define LRCK_DST_ASRC1_SEL                       0x030C0103
#define LRCK_DST_ASRC1_SEL_MCLK_ASRC0_SRC        0U
#define LRCK_DST_ASRC1_SEL_MCLK_ASRC1_SRC        1U
#define LRCK_DST_ASRC1_SEL_MCLK_ASRC2_SRC        2U
#define LRCK_DST_ASRC1_SEL_MCLK_ASRC3_SRC        3U
#define LRCK_DST_ASRC1_SEL_FS_INTER_FROM_SAI0    4U
#define LRCK_DST_ASRC1_SEL_FS_INTER_FROM_SAI1    5U
#define LRCK_DST_ASRC1_SEL_FS_INTER_FROM_SAI2    6U
#define LRCK_DST_ASRC1_SEL_CLKOUT_PDM            7U

// ======================= PERICRU module definition bank=2 =======================
// PERICRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_ARESETN_PERI_BIU                    0x00002000
#define SRST_PRESETN_PERI_BIU                    0x00002001
#define SRST_PRESETN_RTC_BIU                     0x00002002
#define SRST_PRESETN_CRU_PERI                    0x00002003
#define SRST_PRESETN_PERI_GRF                    0x00002004
#define SRST_PRESETN_GPIO1                       0x00002005
#define SRST_DBRESETN_GPIO1                      0x00002006
#define SRST_PRESETN_IOC_VCCIO1                  0x00002007
#define SRST_ARESETN_USB3OTG                     0x00002008
#define SRST_HRESETN_USB2HOST                    0x0000200B
#define SRST_HRESETN_ARB_USB2HOST                0x0000200C
#define SRST_PRESETN_RTC_TEST                    0x0000200D

// PERICRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_HRESETN_EMMC                        0x00002010
#define SRST_HRESETN_FSPI0                       0x00002011
#define SRST_HRESETN_XIP_FSPI0                   0x00002012
#define SRST_SRESETN_2X_FSPI0                    0x00002013
#define SRST_RESETN_UTMI_USB2HOST                0x00002015
#define SRST_RESETN_REF_PIPEPHY                  0x00002017
#define SRST_PRESETN_PIPEPHY                     0x00002018
#define SRST_PRESETN_PIPEPHY_GRF                 0x00002019
#define SRST_PRESETN_USB2PHY                     0x0000201A
#define SRST_RESETN_POR_USB2PHY                  0x0000201B
#define SRST_RESETN_OTG_USB2PHY                  0x0000201C
#define SRST_RESETN_HOST_USB2PHY                 0x0000201D

// PERICRU_GATE_CON00(Offset:0x800)
#define ACLK_PERI_BIU_GATE                       0x00002000
#define PCLK_PERI_BIU_GATE                       0x00002001
#define PCLK_RTC_BIU_GATE                        0x00002002
#define PCLK_CRU_PERI_GATE                       0x00002003
#define PCLK_PERI_GRF_GATE                       0x00002004
#define PCLK_GPIO1_GATE                          0x00002005
#define DBCLK_GPIO1_GATE                         0x00002006
#define PCLK_IOC_VCCIO1_GATE                     0x00002007
#define ACLK_USB3OTG_GATE                        0x00002008
#define CLK_REF_USB3OTG_GATE                     0x00002009
#define CLK_SUSPEND_USB3OTG_GATE                 0x0000200A
#define HCLK_USB2HOST_GATE                       0x0000200B
#define HCLK_ARB_USB2HOST_GATE                   0x0000200C
#define PCLK_RTC_TEST_GATE                       0x0000200D
#define CLK_TESTOUT_PERI_GATE                    0x0000200F

// PERICRU_GATE_CON01(Offset:0x804)
#define HCLK_EMMC_GATE                           0x00002010
#define HCLK_FSPI0_GATE                          0x00002011
#define HCLK_XIP_FSPI0_GATE                      0x00002012
#define CLK_UTMI_USB3OTG_DFT_GATE                0x00002013
#define CLK_PIPE_USB3OTG_DFT_GATE                0x00002014
#define CLK_UTMI_USB2HOST_DFT_GATE               0x00002015
#define CLK48_UTMI_USB2HOST_DFT_GATE             0x00002016
#define PCLK_PIPEPHY_GATE                        0x00002018
#define PCLK_PIPEPHY_GRF_GATE                    0x00002019
#define PCLK_USB2PHY_GATE                        0x0000201A
#define CLK_EMMC_DELAY_LINE_TEST_GATE            0x0000201B
#define CLK_CPLL_PMU_SRC_GATE                    0x0000201D
#define CLK_REF_PIPEPHY_CPLL_SRC_GATE            0x0000201E

// PERICRU_CLKSEL_CON00(Offset:0x300)
#define PCLK_RTC_ROOT_DIV                        0x02000200
#define CLK_TESTOUT_PERI_DIV                     0x04040200
#define CLK_TESTOUT_PERI_SEL                     0x040A0200
#define CLK_TESTOUT_PERI_SEL_ACLK_PERI_ROOT      0U
#define CLK_TESTOUT_PERI_SEL_PCLK_PERI_ROOT      1U
#define CLK_TESTOUT_PERI_SEL_PCLK_RTC_ROOT       2U
#define CLK_TESTOUT_PERI_SEL_CCLK_EMMC           3U
#define CLK_TESTOUT_PERI_SEL_SCLK_2X_FSPI0       4U
#define CLK_TESTOUT_PERI_SEL_CLK_UTMI_USB2PHY_OTG_IO 5U
#define CLK_TESTOUT_PERI_SEL_CLK_UTMI_USB2PHY_HOST_IO 6U
#define CLK_TESTOUT_PERI_SEL_CLK48_UTMI_USB2PHY_IO 7U
#define CLK_TESTOUT_PERI_SEL_CLK_PIPE_PIPEPHY_IO 8U

// PERICRU_CLKSEL_CON01(Offset:0x304)
#define CLK_REF_PIPEPHY_CPLL_SRC_DIV             0x06000201
#define CLK_REF_PIPEPHY_SEL                      0x010C0201
#define CLK_REF_PIPEPHY_SEL_CLK_REF_PIPEPHY_CPLL_SRC 0U
#define CLK_REF_PIPEPHY_SEL_XIN_OSC0_REF_PIPEPHY 1U

// ======================= CORECRU module definition bank=3 =======================
// CORECRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_REF_PVTPLL_CORE              0x00003000
#define SRST_NCOREPORESET0                       0x00003001
#define SRST_NCORESET0                           0x00003002
#define SRST_NCOREPORESET1                       0x00003003
#define SRST_NCORESET1                           0x00003004
#define SRST_NCOREPORESET2                       0x00003005
#define SRST_NCORESET2                           0x00003006
#define SRST_NCOREPORESET3                       0x00003007
#define SRST_NCORESET3                           0x00003008
#define SRST_NDBGRESET                           0x00003009
#define SRST_NL2RESET                            0x0000300A

// CORECRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_ARESETN_CORE_BIU                    0x00003010
#define SRST_PRESETN_CORE_BIU                    0x00003011
#define SRST_HRESETN_CORE_BIU                    0x00003012
#define SRST_PRESETN_DBG                         0x00003013
#define SRST_POTRESETN_DBG                       0x00003014
#define SRST_NTRESETN_DBG                        0x00003015
#define SRST_PRESETN_CORE_PVTPLL                 0x00003016
#define SRST_PRESETN_CRU_CORE                    0x00003017
#define SRST_PRESETN_CORE_GRF                    0x00003018
#define SRST_PRESETN_DFT2APB                     0x0000301A

// CORECRU_GATE_CON00(Offset:0x800)
#define CLK_REF_PVTPLL_CORE_GATE                 0x00003000
#define CLK_CORE_ROOT_GATE                       0x00003003
#define CLK_CORE_GATE                            0x00003004
#define ACLK_CORE_ROOT_GATE                      0x00003005
#define PCLK_CORE_ROOT_GATE                      0x00003006
#define ACLK_CORE_BIU_GATE                       0x00003007
#define PCLK_CORE_BIU_GATE                       0x00003008
#define PCLK_DBG_GATE                            0x0000300A
#define CLK_TESTOUT_CORE_GATE                    0x0000300F

// CORECRU_GATE_CON01(Offset:0x804)
#define PCLK_CORE_PVTPLL_GATE                    0x00003010
#define PCLK_CRU_CORE_GATE                       0x00003011
#define PCLK_CORE_GRF_GATE                       0x00003012
#define PCLK_DFT2APB_GATE                        0x00003014
#define SWCLKTCK_DFT_GATE                        0x0000301C
#define CLK_SCANHS_CLK_CORE_GATE                 0x0000301F

// CORECRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_CORE_DIV                     0x05040300
#define CLK_CORE_PVTPLL_SRC_SEL                  0x01000300
#define CLK_CORE_PVTPLL_SRC_SEL_CLK_DEEPSLOW     0U
#define CLK_CORE_PVTPLL_SRC_SEL_CLK_CORE_PVTPLL  1U
#define CLK_CORE_SRC_SEL                         0x01010300
#define CLK_CORE_SRC_SEL_CLK_CORE_PLL_SRC        0U
#define CLK_CORE_SRC_SEL_CLK_CORE_PVTPLL_SRC     1U
#define CLK_TESTOUT_CORE_SEL                     0x030A0300
#define CLK_TESTOUT_CORE_SEL_CLK_CORE_ROOT       0U
#define CLK_TESTOUT_CORE_SEL_ACLK_CORE_ROOT      1U
#define CLK_TESTOUT_CORE_SEL_PCLK_CORE_ROOT      2U
#define CLK_TESTOUT_CORE_SEL_HCLK_CORE_BIU       3U
#define CLK_TESTOUT_CORE_SEL_TEST_CLKOUT_CORE_PVTPLL 4U
#define CLK_TESTOUT_CORE_SEL_SWCLKTCK_IO         5U

// CORECRU_CLKSEL_CON01(Offset:0x304)
#define CLK_SCANHS_CLK_CORE_DIV2_DIV             0x05000301
#define CLK_SCANHS_CLK_CORE_DIV8_DIV             0x05080301
#define MBIST_CLK_A53_NDFT_SEL                   0x010F0301
#define MBIST_CLK_A53_NDFT_SEL_MBIST_CLK_CORE    0U
#define MBIST_CLK_A53_NDFT_SEL_CLK_CORE_PVTPLL_SRC 1U

// CORECRU_CLKSEL_CON02(Offset:0x308)
#define ACLK_CORE_ROOT_DIV                       0x05000302
#define PCLK_CORE_ROOT_DIV                       0x05080302

// ======================= PMUCRU module definition bank=4 ========================
// PMUCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_HRESETN_PMU_BIU                     0x00004000
#define SRST_PRESETN_PMU_GPIO0                   0x00004007
#define SRST_DBRESETN_PMU_GPIO0                  0x00004008
#define SRST_PRESETN_PMU_HP_TIMER                0x0000400A
#define SRST_RESETN_PMU_HP_TIMER                 0x0000400B
#define SRST_RESETN_PMU_32K_HP_TIMER             0x0000400C

// PMUCRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_PWM1                        0x00004010
#define SRST_RESETN_PWM1                         0x00004011
#define SRST_PRESETN_I2C2                        0x00004012
#define SRST_RESETN_I2C2                         0x00004013
#define SRST_PRESETN_UART0                       0x00004014
#define SRST_SRESETN_UART0                       0x00004015

// PMUCRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_PRESETN_RCOSC_CTRL                  0x00004020
#define SRST_RESETN_REF_RCOSC_CTRL               0x00004022
#define SRST_PRESETN_IOC_PMUIO0                  0x00004023
#define SRST_PRESETN_CRU_PMU                     0x00004024
#define SRST_PRESETN_PMU_GRF                     0x00004025
#define SRST_RESETN_PREROLL                      0x00004027
#define SRST_RESETN_PREROLL_32K                  0x00004028
#define SRST_HRESETN_PMU_SRAM                    0x00004029

// PMUCRU_SOFTRST_CON03(Offset:0xA0C)
#define SRST_PRESETN_WDT_LPMCU                   0x00004030
#define SRST_TRESETN_WDT_LPMCU                   0x00004031
#define SRST_RESETN_LPMCU_FULL_CLUSTER           0x00004032
#define SRST_RESETN_LPMCU_PWUP                   0x00004033
#define SRST_RESETN_LPMCU_ONLY_CORE              0x00004034
#define SRST_TRESETN_LPMCU_JTAG                  0x00004035
#define SRST_PRESETN_LPMCU_MAILBOX               0x00004036

// PMUCRU_GATE_CON00(Offset:0x800)
#define CLK_RCOSC_SRC_GATE                       0x00004000
#define BUSCLK_PMU_ROOT_GATE                     0x00004001
#define HCLK_PMU_BIU_GATE                        0x00004003
#define CLK_PMU_GATE                             0x00004005
#define PCLK_PMU_GATE                            0x00004006
#define PCLK_PMU_GPIO0_GATE                      0x00004007
#define DBCLK_PMU_GPIO0_GATE                     0x00004008
#define PCLK_PMU_HP_TIMER_GATE                   0x0000400A
#define CLK_PMU_HP_TIMER_XIN_PLL_GATE            0x0000400B
#define CLK_PMU_HP_TIMER_GATE                    0x0000400C
#define CLK_PMU_32K_HP_TIMER_GATE                0x0000400D
#define CLK_TESTOUT_OUT_GATE                     0x0000400E
#define CLK_TESTOUT_PMU_GATE                     0x0000400F

// PMUCRU_GATE_CON01(Offset:0x804)
#define PCLK_PWM1_GATE                           0x00004010
#define CLK_PWM1_GATE                            0x00004011
#define CLK_OSC_PWM1_GATE                        0x00004012
#define CLK_RC_PWM1_GATE                         0x00004013
#define CLK_FREQ_PWM1_GATE                       0x00004014
#define CLK_COUNTER_PWM1_GATE                    0x00004015
#define PCLK_I2C2_GATE                           0x00004016
#define CLK_I2C2_GATE                            0x00004017
#define PCLK_UART0_GATE                          0x00004018
#define SCLK_UART0_GATE                          0x0000401B

// PMUCRU_GATE_CON02(Offset:0x808)
#define PCLK_RCOSC_CTRL_GATE                     0x00004020
#define CLK_OSC_RCOSC_CTRL_GATE                  0x00004021
#define CLK_REF_RCOSC_CTRL_GATE                  0x00004022
#define PCLK_IOC_PMUIO0_GATE                     0x00004023
#define PCLK_CRU_PMU_GATE                        0x00004024
#define PCLK_PMU_GRF_GATE                        0x00004025
#define CLK_REFOUT_GATE                          0x00004026
#define CLK_PREROLL_GATE                         0x00004027
#define CLK_PREROLL_32K_GATE                     0x00004028
#define HCLK_PMU_SRAM_GATE                       0x00004029

// PMUCRU_GATE_CON03(Offset:0x80C)
#define PCLK_WDT_LPMCU_GATE                      0x00004030
#define TCLK_WDT_LPMCU_GATE                      0x00004031
#define CLK_LPMCU_GATE                           0x00004032
#define CLK_LPMCU_RTC_GATE                       0x00004033
#define PCLK_LPMCU_MAILBOX_GATE                  0x00004034
#define BUSCLK_PMU1_ROOT_GATE                    0x0000403B
#define CLK_LPMCU_JTAG_DFT_GATE                  0x0000403E

// PMUCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_PMU_DIV                      0x03040400
#define CLK_TESTOUT_OUT_SEL                      0x04000400
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_PMU      0U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_TOP      1U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_CORE     2U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_NPU      3U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_VI       4U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_VEPU     5U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_VDO      6U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_DDR      7U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_BUS      8U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_PERI     9U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_VCP      10U
#define CLK_TESTOUT_PMU_SEL                      0x050A0400

// PMUCRU_CLKSEL_CON01(Offset:0x304)
#define BUSCLK_PMU_MUX_DIV                       0x02040401
#define CLK_PMU_HP_TIMER_XIN_PLL_DIV             0x05080401
#define CLK_RCOSC_SRC_SEL                        0x03000401
#define CLK_RCOSC_SRC_SEL_XIN_OSC0_FUNC          0U
#define CLK_RCOSC_SRC_SEL_CLK_RCOSC_IO           1U
#define CLK_RCOSC_SRC_SEL_CLK_RCOSC_DIV2_IO      2U
#define CLK_RCOSC_SRC_SEL_CLK_RCOSC_DIV3_IO      3U
#define CLK_RCOSC_SRC_SEL_CLK_RCOSC_DIV4_IO      4U
#define BUSCLK_PMU_MUX_SEL                       0x01030401
#define BUSCLK_PMU_MUX_SEL_BUSCLK_PMU_32K        0U
#define BUSCLK_PMU_MUX_SEL_CLK_RCOSC_SRC         1U
#define CLK_PMU_HP_TIMER_XIN_PLL_SEL             0x010D0401
#define CLK_PMU_HP_TIMER_XIN_PLL_SEL_CLK_CPLL_PMU_SRC 0U
#define CLK_PMU_HP_TIMER_XIN_PLL_SEL_XIN_OSC0_FUNC 1U

// PMUCRU_CLKSEL_CON02(Offset:0x308)
#define CLK_PWM1_DIV                             0x02060402
#define CLK_I2C2_DIV                             0x020C0402
#define CLK_XIN_RC_DIV_SEL                       0x01000402
#define CLK_XIN_RC_DIV_SEL_XIN_OSC0_FUNC         0U
#define CLK_XIN_RC_DIV_SEL_CLK_RCOSC_SRC         1U
#define CLK_DEEPSLOW_SEL                         0x02010402
#define CLK_DEEPSLOW_SEL_CLK_XIN_RC_DIV          0U
#define CLK_DEEPSLOW_SEL_CLK_32K_RTC             1U
#define CLK_DEEPSLOW_SEL_CLK_32K_IO              2U
#define DBCLK_PMU_GPIO0_SEL                      0x01040402
#define DBCLK_PMU_GPIO0_SEL_XIN_OSC0_FUNC        0U
#define DBCLK_PMU_GPIO0_SEL_CLK_DEEPSLOW         1U
#define CLK_PWM1_SEL                             0x02080402
#define CLK_PWM1_SEL_XIN_OSC0_FUNC               0U
#define CLK_PWM1_SEL_CLK_RCOSC_SRC               1U
#define CLK_PWM1_SEL_BUSCLK_PMU_SRC              2U
#define CLK_I2C2_SEL                             0x020E0402
#define CLK_I2C2_SEL_XIN_OSC0_FUNC               0U
#define CLK_I2C2_SEL_CLK_RCOSC_SRC               1U
#define CLK_I2C2_SEL_BUSCLK_PMU_SRC              2U

// PMUCRU_CLKSEL_CON03(Offset:0x30C)
#define SCLK_UART0_SEL                           0x02000403
#define SCLK_UART0_SEL_SCLK_UART0_SRC            0U
#define SCLK_UART0_SEL_XIN_OSC0_FUNC             1U
#define SCLK_UART0_SEL_CLK_RCOSC_SRC             2U
#define CLK_OSC_RCOSC_CTRL_SEL                   0x01020403
#define CLK_OSC_RCOSC_CTRL_SEL_CLK_RCOSC_SRC     0U
#define CLK_OSC_RCOSC_CTRL_SEL_CLK_TESTOUT_OUT   1U
#define TCLK_WDT_LPMCU_SEL                       0x02060403
#define TCLK_WDT_LPMCU_SEL_XIN_OSC0_FUNC         0U
#define TCLK_WDT_LPMCU_SEL_CLK_RCOSC_SRC         1U
#define TCLK_WDT_LPMCU_SEL_BUSCLK_PMU_SRC        2U
#define TCLK_WDT_LPMCU_SEL_CLK_DEEPSLOW          3U

// PMUCRU_CLKSEL_CON08(Offset:0x320)
#define CLK_XIN_RC_DIV_DIV                       0x20000408

// ======================= PMU1CRU module definition bank=5 =======================
// PMU1CRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_SPI2AHB                     0x00005000
#define SRST_HRESETN_SPI2AHB                     0x00005001
#define SRST_HRESETN_FSPI1                       0x00005002
#define SRST_HRESETN_XIP_FSPI1                   0x00005003
#define SRST_SRESETN_1X_FSPI1                    0x00005004
#define SRST_PRESETN_IOC_PMUIO1                  0x00005005
#define SRST_PRESETN_CRU_PMU1                    0x00005006
#define SRST_PRESETN_AUDIO_ADC_PMU               0x00005007
#define SRST_MRESETN_AUDIO_ADC_PMU               0x00005008
#define SRST_HRESETN_PMU1_BIU                    0x00005009

// PMU1CRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_LPDMA                       0x00005010
#define SRST_ARESETN_LPDMA                       0x00005011
#define SRST_HRESETN_LPSAI                       0x00005012
#define SRST_MRESETN_LPSAI                       0x00005013
#define SRST_PRESETN_AOA_TDD                     0x00005014
#define SRST_PRESETN_AOA_FE                      0x00005015
#define SRST_PRESETN_AOA_AAD                     0x00005016
#define SRST_PRESETN_AOA_APB                     0x00005017
#define SRST_PRESETN_AOA_SRAM                    0x00005018

// PMU1CRU_GATE_CON00(Offset:0x800)
#define PCLK_SPI2AHB_GATE                        0x00005000
#define HCLK_SPI2AHB_GATE                        0x00005001
#define HCLK_FSPI1_GATE                          0x00005002
#define HCLK_XIP_FSPI1_GATE                      0x00005003
#define SCLK_1X_FSPI1_GATE                       0x00005004
#define PCLK_IOC_PMUIO1_GATE                     0x00005005
#define PCLK_CRU_PMU1_GATE                       0x00005006
#define PCLK_AUDIO_ADC_PMU_GATE                  0x00005008
#define MCLK_AUDIO_ADC_PMU_GATE                  0x00005009
#define HCLK_PMU1_BIU_GATE                       0x0000500A

// PMU1CRU_GATE_CON01(Offset:0x804)
#define PCLK_LPDMA_GATE                          0x00005010
#define ACLK_LPDMA_GATE                          0x00005011
#define MCLK_LPSAI_GATE                          0x00005013
#define PCLK_AOA_GATE                            0x00005014
#define HCLK_AOA_SRAM_GATE                       0x00005017

// PMU1CRU_CLKSEL_CON00(Offset:0x300)
#define SCLK_1X_FSPI1_DIV                        0x03020500
#define MCLK_LPSAI_DIV                           0x05080500
#define SCLK_1X_FSPI1_SEL                        0x02000500
#define SCLK_1X_FSPI1_SEL_XIN_OSC0_FUNC          0U
#define SCLK_1X_FSPI1_SEL_CLK_RCOSC_SRC          1U
#define SCLK_1X_FSPI1_SEL_BUSCLK_PMU_SRC         2U
#define MCLK_LPSAI_SEL                           0x02060500
#define MCLK_LPSAI_SEL_XIN_OSC0_FUNC             0U
#define MCLK_LPSAI_SEL_CLK_RCOSC_SRC             1U
#define MCLK_LPSAI_SEL_BUSCLK_PMU_SRC            2U

// ======================= DDRCRU module definition bank=6 ========================
// DDRCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_DDR_BIU                     0x00006001
#define SRST_PRESETN_DDRC                        0x00006002
#define SRST_PRESETN_DDRMON                      0x00006003
#define SRST_RESETN_TIMER_DDRMON                 0x00006004
#define SRST_PRESETN_DFICTRL                     0x00006005
#define SRST_PRESETN_DDR_GRF                     0x00006006
#define SRST_PRESETN_CRU_DDR                     0x00006007
#define SRST_PRESETN_DDRPHY                      0x00006008
#define SRST_PRESETN_DMA2DDR                     0x00006009

// DDRCRU_GATE_CON00(Offset:0x800)
#define PCLK_DDR_BIU_GATE                        0x00006001
#define PCLK_DDRC_GATE                           0x00006002
#define PCLK_DDRMON_GATE                         0x00006003
#define CLK_TIMER_DDRMON_GATE                    0x00006004
#define PCLK_DFICTRL_GATE                        0x00006005
#define PCLK_DDR_GRF_GATE                        0x00006006
#define PCLK_CRU_DDR_GATE                        0x00006007
#define PCLK_DDRPHY_GATE                         0x00006008
#define PCLK_DMA2DDR_GATE                        0x00006009
#define CLK_TESTOUT_DDR_GATE                     0x0000600F

// DDRCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_DDR_DIV                      0x04040600
#define CLK_TESTOUT_DDR_SEL                      0x020A0600
#define CLK_TESTOUT_DDR_SEL_CLK1X_DDR_ROOT       0U
#define CLK_TESTOUT_DDR_SEL_PCLK_DDR_ROOT        1U
#define CLK_TESTOUT_DDR_SEL_ACLK_SYSMEM_ROOT     2U

// ====================== SUBDDRCRU module definition bank=7 ======================
// SUBDDRCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_ARESETN_SYSMEM_BIU                  0x00007000
#define SRST_ARESETN_SYSMEM                      0x00007001
#define SRST_ARESETN_DDR_BIU                     0x00007002
#define SRST_ARESETN_DDRSCH0_CPU                 0x00007003
#define SRST_ARESETN_DDRSCH1_NPU                 0x00007004
#define SRST_ARESETN_DDRSCH2_POE                 0x00007005
#define SRST_ARESETN_DDRSCH3_VI                  0x00007006
#define SRST_RESETN_CORE_DDRC                    0x00007007
#define SRST_RESETN_DDRMON                       0x00007008
#define SRST_RESETN_DFICTRL                      0x00007009
#define SRST_RESETN_RS                           0x0000700B
#define SRST_ARESETN_DMA2DDR                     0x0000700C
#define SRST_RESETN_DDRPHY                       0x0000700D

// SUBDDRCRU_GATE_CON00(Offset:0x800)
#define ACLK_SYSMEM_BIU_GATE                     0x00007000
#define ACLK_SYSMEM_GATE                         0x00007001
#define ACLK_DDR_BIU_GATE                        0x00007002
#define ACLK_DDRSCH0_CPU_GATE                    0x00007003
#define ACLK_DDRSCH1_NPU_GATE                    0x00007004
#define ACLK_DDRSCH2_POE_GATE                    0x00007005
#define ACLK_DDRSCH3_VI_GATE                     0x00007006
#define CLK_CORE_DDRC_GATE                       0x00007007
#define CLK_DDRMON_GATE                          0x00007008
#define CLK_DFICTRL_GATE                         0x00007009
#define CLK1X_RS_GATE                            0x0000700B
#define ACLK_DMA2DDR_GATE                        0x0000700C
#define CLK1X_DDRPHY_GATE                        0x0000700D
#define CLK1X_DDR_SRC_GATE                       0x0000700E
#define CLK_DPLLTEST_GATE                        0x0000700F

// SUBDDRCRU_CLKSEL_CON00(Offset:0x300)
#define CLK1X_PLL_ROOT_SEL                       0x01010700
#define CLK1X_PLL_ROOT_SEL_CLK_DPLL              0U
#define CLK1X_PLL_ROOT_SEL_ACLK_SYSMEM_SRC       1U

// ======================== VICRU module definition bank=8 ========================
// VICRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_REF_PVTPLL_ISP               0x00008000
#define SRST_ARESETN_GMAC_BIU                    0x00008001
#define SRST_ARESETN_VI_BIU                      0x00008002
#define SRST_HRESETN_VI_BIU                      0x00008003
#define SRST_PRESETN_VI_BIU                      0x00008004
#define SRST_PRESETN_CRU_VI                      0x00008005
#define SRST_PRESETN_VI_GRF                      0x00008006
#define SRST_PRESETN_VI_PVTPLL                   0x00008007
#define SRST_PRESETN_DSMC                        0x00008008
#define SRST_ARESETN_DSMC                        0x00008009
#define SRST_HRESETN_CAN0                        0x0000800A
#define SRST_RESETN_CAN0                         0x0000800B
#define SRST_HRESETN_CAN1                        0x0000800C
#define SRST_RESETN_CAN1                         0x0000800D

// VICRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_GPIO2                       0x00008010
#define SRST_DBRESETN_GPIO2                      0x00008011
#define SRST_PRESETN_GPIO4                       0x00008012
#define SRST_DBRESETN_GPIO4                      0x00008013
#define SRST_PRESETN_GPIO5                       0x00008014
#define SRST_DBRESETN_GPIO5                      0x00008015
#define SRST_PRESETN_GPIO6                       0x00008016
#define SRST_DBRESETN_GPIO6                      0x00008017
#define SRST_PRESETN_GPIO7                       0x00008018
#define SRST_DBRESETN_GPIO7                      0x00008019
#define SRST_PRESETN_IOC_VCCIO2                  0x0000801A
#define SRST_PRESETN_IOC_VCCIO4                  0x0000801B
#define SRST_PRESETN_IOC_VCCIO5                  0x0000801C
#define SRST_PRESETN_IOC_VCCIO6                  0x0000801D
#define SRST_PRESETN_IOC_VCCIO7                  0x0000801E

// VICRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_RESETN_CORE_ISP                     0x00008020
#define SRST_HRESETN_VICAP                       0x00008021
#define SRST_ARESETN_VICAP                       0x00008022
#define SRST_DRESETN_VICAP                       0x00008023
#define SRST_ISP0RESETN_VICAP                    0x00008024
#define SRST_RESETN_CORE_VPSS                    0x00008025
#define SRST_RESETN_CORE_VPSL                    0x00008026
#define SRST_PRESETN_CSI2HOST0                   0x00008027
#define SRST_PRESETN_CSI2HOST1                   0x00008028
#define SRST_PRESETN_CSI2HOST2                   0x00008029
#define SRST_PRESETN_CSI2HOST3                   0x0000802A
#define SRST_HRESETN_SDMMC0                      0x0000802B
#define SRST_ARESETN_GMAC                        0x0000802C
#define SRST_PRESETN_CSIPHY0                     0x0000802D
#define SRST_PRESETN_CSIPHY1                     0x0000802E

// VICRU_SOFTRST_CON03(Offset:0xA0C)
#define SRST_PRESETN_MACPHY                      0x00008030
#define SRST_RESETN_MACPHY                       0x00008031
#define SRST_PRESETN_SARADC1                     0x00008032
#define SRST_RESETN_SARADC1                      0x00008033
#define SRST_PRESETN_SARADC2                     0x00008035
#define SRST_RESETN_SARADC2                      0x00008036

// VICRU_GATE_CON00(Offset:0x800)
#define CLK_REF_PVTPLL_ISP_GATE                  0x00008000
#define ACLK_GMAC_BIU_GATE                       0x00008001
#define ACLK_VI_BIU_GATE                         0x00008002
#define HCLK_VI_BIU_GATE                         0x00008003
#define PCLK_VI_BIU_GATE                         0x00008004
#define PCLK_CRU_VI_GATE                         0x00008005
#define PCLK_VI_GRF_GATE                         0x00008006
#define PCLK_VI_PVTPLL_GATE                      0x00008007
#define PCLK_DSMC_GATE                           0x00008008
#define ACLK_DSMC_GATE                           0x00008009
#define HCLK_CAN0_GATE                           0x0000800A
#define HCLK_CAN1_GATE                           0x0000800B
#define CLK_IN_50M_MACPHY_GATE                   0x0000800C
#define CLK_RMII_MACPHY_IO_GATE                  0x0000800D
#define CLK_RMII_MACPHY_CRU_GATE                 0x0000800E
#define CLK_TESTOUT_VI_GATE                      0x0000800F

// VICRU_GATE_CON01(Offset:0x804)
#define PCLK_GPIO2_GATE                          0x00008010
#define DBCLK_GPIO2_GATE                         0x00008011
#define PCLK_GPIO4_GATE                          0x00008012
#define DBCLK_GPIO4_GATE                         0x00008013
#define PCLK_GPIO5_GATE                          0x00008014
#define DBCLK_GPIO5_GATE                         0x00008015
#define PCLK_GPIO6_GATE                          0x00008016
#define DBCLK_GPIO6_GATE                         0x00008017
#define PCLK_GPIO7_GATE                          0x00008018
#define DBCLK_GPIO7_GATE                         0x00008019
#define PCLK_IOC_VCCIO2_GATE                     0x0000801A
#define PCLK_IOC_VCCIO4_GATE                     0x0000801B
#define PCLK_IOC_VCCIO5_GATE                     0x0000801C
#define PCLK_IOC_VCCIO6_GATE                     0x0000801D
#define PCLK_IOC_VCCIO7_GATE                     0x0000801E

// VICRU_GATE_CON02(Offset:0x808)
#define HCLK_ISP_GATE                            0x00008020
#define ACLK_ISP_GATE                            0x00008021
#define CLK_CORE_ISP_GATE                        0x00008022
#define HCLK_VICAP_GATE                          0x00008023
#define ACLK_VICAP_GATE                          0x00008024
#define DCLK_VICAP_GATE                          0x00008025
#define ISP0CLK_VICAP_GATE                       0x00008026
#define HCLK_VPSS_GATE                           0x00008027
#define ACLK_VPSS_GATE                           0x00008028
#define CLK_CORE_VPSS_GATE                       0x00008029
#define HCLK_VPSL_GATE                           0x0000802A
#define ACLK_VPSL_GATE                           0x0000802B
#define CLK_CORE_VPSL_GATE                       0x0000802C

// VICRU_GATE_CON03(Offset:0x80C)
#define PCLK_CSI2HOST0_GATE                      0x00008030
#define DCLK_CSI2HOST0_GATE                      0x00008031
#define PCLK_CSI2HOST1_GATE                      0x00008032
#define DCLK_CSI2HOST1_GATE                      0x00008033
#define PCLK_CSI2HOST2_GATE                      0x00008034
#define DCLK_CSI2HOST2_GATE                      0x00008035
#define PCLK_CSI2HOST3_GATE                      0x00008036
#define DCLK_CSI2HOST3_GATE                      0x00008037
#define HCLK_SDMMC0_GATE                         0x00008038
#define ACLK_GMAC_GATE                           0x00008039
#define PCLK_GMAC_GATE                           0x0000803A
#define PCLK_CSIPHY0_GATE                        0x0000803B
#define PCLK_CSIPHY1_GATE                        0x0000803C
#define PCLK_MACPHY_GATE                         0x0000803D
#define CLK_50M_GMAC_IOBUF_GATE                  0x0000803E

// VICRU_GATE_CON04(Offset:0x810)
#define PCLK_SARADC1_GATE                        0x00008040
#define RX0PCLK_VICAP_LVDS_DFT_GATE              0x00008041
#define PCLK_SARADC2_GATE                        0x00008042
#define RX1PCLK_VICAP_LVDS_DFT_GATE              0x00008043
#define CLK_CIF_DELAY_LINE_TEST_M0_GATE          0x00008044
#define CLK_CIF_DELAY_LINE_TEST_M1_GATE          0x00008045
#define CLK_SDMMC0_DELAY_LINE_TEST_GATE          0x00008046
#define CLK_DSMC_DELAY_LINE_TEST_GATE            0x00008047
#define CLK_VOP_DELAY_LINE_TEST_GATE             0x00008048
#define RX2PCLK_VICAP_LVDS_DFT_GATE              0x00008049
#define RX3PCLK_VICAP_LVDS_DFT_GATE              0x0000804A
#define CLK_RXBYTECLKHS_0_DFT_GATE               0x0000804B
#define CLK_RXBYTECLKHS_1_DFT_GATE               0x0000804C
#define CLK_RXBYTECLKHS_2_DFT_GATE               0x0000804D
#define CLK_RXBYTECLKHS_3_DFT_GATE               0x0000804E
#define PCLKIN_VICAP_CIF_DFT_GATE                0x0000804F

// VICRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_VI_DIV                       0x04040800
#define CLK_ISP_PVTPLL_SRC_SEL                   0x01000800
#define CLK_ISP_PVTPLL_SRC_SEL_CLK_DEEPSLOW      0U
#define CLK_ISP_PVTPLL_SRC_SEL_CLKSP_PVTPLL      1U
#define CLK_CORE_ISP_ROOT_SEL                    0x01010800
#define CLK_CORE_ISP_ROOT_SEL_CLK_ISP_PLL_SRC    0U
#define CLK_CORE_ISP_ROOT_SEL_CLK_ISP_PVTPLL_SRC 1U
#define CLK_SARADC1_SEL                          0x01020800
#define CLK_SARADC1_SEL_CLK_SARADC1_SRC          0U
#define CLK_SARADC1_SEL_CLK_SARADC1_RCOSC_IO     1U
#define CLK_SARADC2_SEL                          0x01030800
#define CLK_SARADC2_SEL_CLK_SARADC2_SRC          0U
#define CLK_SARADC2_SEL_CLK_SARADC2_RCOSC_IO     1U
#define CLK_TESTOUT_VI_SEL                       0x05080800
#define CLK_GMAC_PTP_REF_SEL                     0x010E0800
#define CLK_GMAC_PTP_REF_SEL_CLK_GMAC_PTP_REF_SRC 0U
#define CLK_GMAC_PTP_REF_SEL_CLK_GMAC_PTP_FROM_IO 1U
#define MBIST_CLK_PVTPLL_VI_SEL                  0x010F0800
#define MBIST_CLK_PVTPLL_VI_SEL_MBIST_CLK_ISP    0U
#define MBIST_CLK_PVTPLL_VI_SEL_CLK_ISP_PVTPLL_SRC 1U

// VICRU_CLKSEL_CON01(Offset:0x304)
#define CLK_IN_50M_MACPHY_SEL                    0x01000801
#define CLK_IN_50M_MACPHY_SEL_CLK_MATRIX_50M_VI  0U
#define CLK_IN_50M_MACPHY_SEL_XIN_OSC0_FUNC      1U
#define CLK_IN_MACPHY_SEL                        0x01010801
#define CLK_IN_MACPHY_SEL_XIN_OSC0_REF_MACPHY    0U
#define CLK_IN_MACPHY_SEL_CLK_IN_50M_MACPHY      1U

// ======================= VEPUCRU module definition bank=9 =======================
// VEPUCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_REF_PVTPLL_VEPU              0x00009000
#define SRST_ARESETN_VEPU_BIU                    0x00009001
#define SRST_HRESETN_VEPU_BIU                    0x00009002
#define SRST_PRESETN_VEPU_BIU                    0x00009003
#define SRST_PRESETN_CRU_VEPU                    0x00009004
#define SRST_PRESETN_VEPU_GRF                    0x00009005
#define SRST_PRESETN_GPIO3                       0x00009007
#define SRST_DBRESETN_GPIO3                      0x00009008
#define SRST_PRESETN_IOC_VCCIO3                  0x00009009
#define SRST_PRESETN_SARADC0                     0x0000900A
#define SRST_RESETN_SARADC0                      0x0000900B
#define SRST_HRESETN_SDMMC1                      0x0000900D

// VEPUCRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_VEPU_PVTPLL                 0x00009010
#define SRST_HRESETN_VEPU                        0x00009011
#define SRST_ARESETN_VEPU                        0x00009012
#define SRST_RESETN_CORE_VEPU                    0x00009013

// VEPUCRU_GATE_CON00(Offset:0x800)
#define CLK_REF_PVTPLL_VEPU_GATE                 0x00009000
#define ACLK_VEPU_BIU_GATE                       0x00009001
#define HCLK_VEPU_BIU_GATE                       0x00009002
#define PCLK_VEPU_BIU_GATE                       0x00009003
#define PCLK_CRU_VEPU_GATE                       0x00009004
#define PCLK_VEPU_GRF_GATE                       0x00009005
#define PCLK_GPIO3_GATE                          0x00009007
#define DBCLK_GPIO3_GATE                         0x00009008
#define PCLK_IOC_VCCIO3_GATE                     0x00009009
#define PCLK_SARADC0_GATE                        0x0000900A
#define HCLK_SDMMC1_GATE                         0x0000900C
#define CLK_TESTOUT_VEPU_GATE                    0x0000900F

// VEPUCRU_GATE_CON01(Offset:0x804)
#define PCLK_VEPU_PVTPLL_GATE                    0x00009010
#define HCLK_VEPU_GATE                           0x00009011
#define ACLK_VEPU_GATE                           0x00009012
#define CLK_CORE_VEPU_GATE                       0x00009013
#define CLK_SDMMC1_DELAY_LINE_TEST_GATE          0x00009014

// VEPUCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_VEPU_DIV                     0x05040900
#define CLK_VEPU_PVTPLL_SRC_SEL                  0x01000900
#define CLK_VEPU_PVTPLL_SRC_SEL_CLK_DEEPSLOW     0U
#define CLK_VEPU_PVTPLL_SRC_SEL_CLK_VEPU_PVTPLL  1U
#define CLK_CORE_VEPU_OCCMUX_SEL                 0x01010900
#define CLK_CORE_VEPU_OCCMUX_SEL_CLK_VEPU_PLL_SRC 0U
#define CLK_CORE_VEPU_OCCMUX_SEL_CLK_VEPU_PVTPLL_SRC 1U
#define CLK_SARADC0_SEL                          0x01020900
#define CLK_SARADC0_SEL_CLK_SARADC0_SRC          0U
#define CLK_SARADC0_SEL_CLK_SARADC0_RCOSC_IO     1U
#define CLK_TESTOUT_VEPU_SEL                     0x030A0900
#define CLK_TESTOUT_VEPU_SEL_ACLK_VEPU_ROOT      0U
#define CLK_TESTOUT_VEPU_SEL_HCLK_VEPU_ROOT      1U
#define CLK_TESTOUT_VEPU_SEL_PCLK_VEPU_ROOT      2U
#define CLK_TESTOUT_VEPU_SEL_CLK_CORE_VEPU       3U
#define CLK_TESTOUT_VEPU_SEL_TEST_CLKOUT_VEPU_PVTPLL 4U
#define CLK_TESTOUT_VEPU_SEL_CCLK_SDMMC1         5U
#define CLK_TESTOUT_VEPU_SEL_CLK_SARADC0_SRC     6U
#define CLK_TESTOUT_VEPU_SEL_CLK_SARADC0_RCOSC_IO 7U
#define MBIST_CLK_PVTPLL_VEPU_SEL                0x010F0900
#define MBIST_CLK_PVTPLL_VEPU_SEL_MBIST_CLK_VEPU 0U
#define MBIST_CLK_PVTPLL_VEPU_SEL_CLK_VEPU_PVTPLL_SRC 1U

// ======================= NPUCRU module definition bank=10 =======================
// NPUCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_REF_PVTPLL_NPU               0x0000A000
#define SRST_ARESETN_NPU_BIU                     0x0000A002
#define SRST_HRESETN_NPU_BIU                     0x0000A003
#define SRST_PRESETN_NPU_BIU                     0x0000A004
#define SRST_PRESETN_CRU_NPU                     0x0000A005
#define SRST_PRESETN_NPU_GRF                     0x0000A006
#define SRST_PRESETN_NPU_PVTPLL                  0x0000A008
#define SRST_HRESETN_RKNN                        0x0000A009
#define SRST_ARESETN_RKNN                        0x0000A00A

// NPUCRU_GATE_CON00(Offset:0x800)
#define CLK_REF_PVTPLL_NPU_GATE                  0x0000A000
#define ACLK_NPU_BIU_GATE                        0x0000A002
#define HCLK_NPU_BIU_GATE                        0x0000A003
#define PCLK_NPU_BIU_GATE                        0x0000A004
#define PCLK_CRU_NPU_GATE                        0x0000A005
#define PCLK_NPU_GRF_GATE                        0x0000A006
#define PCLK_NPU_PVTPLL_GATE                     0x0000A008
#define HCLK_RKNN_GATE                           0x0000A009
#define ACLK_RKNN_GATE                           0x0000A00A
#define CLK_TESTOUT_NPU_GATE                     0x0000A00F

// NPUCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_NPU_DIV                      0x05040A00
#define CLK_NPU_PVTPLL_SRC_SEL                   0x01000A00
#define CLK_NPU_PVTPLL_SRC_SEL_CLK_DEEPSLOW      0U
#define CLK_NPU_PVTPLL_SRC_SEL_CLK_NPU_PVTPLL    1U
#define ACLK_NPU_ROOT_SEL                        0x01010A00
#define ACLK_NPU_ROOT_SEL_CLK_NPU_PLL_SRC        0U
#define ACLK_NPU_ROOT_SEL_CLK_NPU_PVTPLL_SRC     1U
#define CLK_TESTOUT_NPU_SEL                      0x020A0A00
#define CLK_TESTOUT_NPU_SEL_ACLK_NPU_ROOT        0U
#define CLK_TESTOUT_NPU_SEL_HCLK_NPU_ROOT        1U
#define CLK_TESTOUT_NPU_SEL_PCLK_NPU_ROOT        2U
#define CLK_TESTOUT_NPU_SEL_TEST_CLKOUT_NPU_PVTPLL 3U
#define MBIST_CLK_PVTPLL_NPU_SEL                 0x010F0A00
#define MBIST_CLK_PVTPLL_NPU_SEL_MBIST_CLK_NPU   0U
#define MBIST_CLK_PVTPLL_NPU_SEL_CLK_NPU_PVTPLL_SRC 1U

// ======================= VDOCRU module definition bank=11 =======================
// VDOCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_ARESETN_RKVDEC_BIU                  0x0000B000
#define SRST_ARESETN_VDO_BIU                     0x0000B001
#define SRST_HRESETN_VDO_BIU                     0x0000B003
#define SRST_PRESETN_VDO_BIU                     0x0000B004
#define SRST_PRESETN_CRU_VDO                     0x0000B005
#define SRST_PRESETN_VDO_GRF                     0x0000B006
#define SRST_ARESETN_RKVDEC                      0x0000B007
#define SRST_HRESETN_RKVDEC                      0x0000B008
#define SRST_RESETN_HEVC_CA_RKVDEC               0x0000B009
#define SRST_ARESETN_VOP                         0x0000B00A
#define SRST_HRESETN_VOP                         0x0000B00B
#define SRST_DRESETN_VOP                         0x0000B00C
#define SRST_ARESETN_OOC                         0x0000B00D
#define SRST_HRESETN_OOC                         0x0000B00E
#define SRST_DRESETN_OOC                         0x0000B00F

// VDOCRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_HRESETN_RKJPEG                      0x0000B013
#define SRST_ARESETN_RKJPEG                      0x0000B014
#define SRST_ARESETN_RKMMU_DECOM                 0x0000B015
#define SRST_HRESETN_RKMMU_DECOM                 0x0000B016
#define SRST_DRESETN_DECOM                       0x0000B018
#define SRST_ARESETN_DECOM                       0x0000B019
#define SRST_PRESETN_DECOM                       0x0000B01A
#define SRST_PRESETN_MIPI_DSI                    0x0000B01C
#define SRST_PRESETN_DSIPHY                      0x0000B01D

// VDOCRU_GATE_CON00(Offset:0x800)
#define ACLK_RKVDEC_BIU_GATE                     0x0000B000
#define ACLK_VDO_BIU_GATE                        0x0000B001
#define HCLK_VDO_BIU_GATE                        0x0000B003
#define PCLK_VDO_BIU_GATE                        0x0000B004
#define PCLK_CRU_VDO_GATE                        0x0000B005
#define PCLK_VDO_GRF_GATE                        0x0000B006
#define ACLK_RKVDEC_GATE                         0x0000B007
#define HCLK_RKVDEC_GATE                         0x0000B008
#define CLK_HEVC_CA_RKVDEC_GATE                  0x0000B009
#define ACLK_VOP_GATE                            0x0000B00A
#define HCLK_VOP_GATE                            0x0000B00B
#define ACLK_OOC_GATE                            0x0000B00D
#define HCLK_OOC_GATE                            0x0000B00E
#define CLK_TESTOUT_VDO_GATE                     0x0000B00F

// VDOCRU_GATE_CON01(Offset:0x804)
#define HCLK_RKJPEG_GATE                         0x0000B013
#define ACLK_RKJPEG_GATE                         0x0000B014
#define ACLK_RKMMU_DECOM_GATE                    0x0000B015
#define HCLK_RKMMU_DECOM_GATE                    0x0000B016
#define DCLK_DECOM_GATE                          0x0000B018
#define ACLK_DECOM_GATE                          0x0000B019
#define PCLK_DECOM_GATE                          0x0000B01A
#define PCLK_MIPI_DSI_GATE                       0x0000B01C
#define PCLK_DSIPHY_GATE                         0x0000B01D

// VDOCRU_GATE_CON02(Offset:0x808)
#define CLK_TXBYTECLKHS_DFT_GATE                 0x0000B020
#define CLK_RXCLKESC_DFT_GATE                    0x0000B021

// VDOCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_VDO_DIV                      0x04040B00
#define CLK_TESTOUT_VDO_SEL                      0x040A0B00
#define CLK_TESTOUT_VDO_SEL_ACLK_VDO_ROOT        0U
#define CLK_TESTOUT_VDO_SEL_ACLK_RKVDEC_ROOT     1U
#define CLK_TESTOUT_VDO_SEL_HCLK_VDO_ROOT        2U
#define CLK_TESTOUT_VDO_SEL_PCLK_VDO_ROOT        3U
#define CLK_TESTOUT_VDO_SEL_CLK_HEVC_CA_RKVDEC   4U
#define CLK_TESTOUT_VDO_SEL_DCLK_VOP             5U
#define CLK_TESTOUT_VDO_SEL_DCLK_OOC             6U
#define CLK_TESTOUT_VDO_SEL_DCLK_DECOM           7U
#define CLK_TESTOUT_VDO_SEL_CLK_TXBYTECLKHS_IO   8U
#define CLK_TESTOUT_VDO_SEL_CLK_RXCLKESC_IO      9U

// ======================= VCPCRU module definition bank=12 =======================
// VCPCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_REF_PVTPLL_VCP               0x0000C000
#define SRST_ARESETN_VCP_BIU                     0x0000C001
#define SRST_HRESETN_VCP_BIU                     0x0000C002
#define SRST_PRESETN_VCP_BIU                     0x0000C003
#define SRST_PRESETN_CRU_VCP                     0x0000C004
#define SRST_PRESETN_VCP_GRF                     0x0000C005
#define SRST_PRESETN_VCP_PVTPLL                  0x0000C007
#define SRST_ARESETN_AISP_BIU                    0x0000C008
#define SRST_HRESETN_AISP_BIU                    0x0000C009
#define SRST_RESETN_CORE_AISP                    0x0000C00D

// VCPCRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_HRESETN_FEC                         0x0000C010
#define SRST_ARESETN_FEC                         0x0000C011
#define SRST_RESETN_CORE_FEC                     0x0000C012
#define SRST_HRESETN_AVSP                        0x0000C013
#define SRST_ARESETN_AVSP                        0x0000C014

// VCPCRU_GATE_CON00(Offset:0x800)
#define CLK_REF_PVTPLL_VCP_GATE                  0x0000C000
#define ACLK_VCP_BIU_GATE                        0x0000C001
#define HCLK_VCP_BIU_GATE                        0x0000C002
#define PCLK_VCP_BIU_GATE                        0x0000C003
#define PCLK_CRU_VCP_GATE                        0x0000C004
#define PCLK_VCP_GRF_GATE                        0x0000C005
#define PCLK_VCP_PVTPLL_GATE                     0x0000C007
#define ACLK_AISP_BIU_GATE                       0x0000C008
#define HCLK_AISP_BIU_GATE                       0x0000C009
#define HCLK_AISP_GATE                           0x0000C00B
#define ACLK_AISP_GATE                           0x0000C00C
#define CLK_CORE_AISP_GATE                       0x0000C00D
#define CLK_TESTOUT_VCP_GATE                     0x0000C00F

// VCPCRU_GATE_CON01(Offset:0x804)
#define HCLK_FEC_GATE                            0x0000C010
#define ACLK_FEC_GATE                            0x0000C011
#define CLK_CORE_FEC_GATE                        0x0000C012
#define HCLK_AVSP_GATE                           0x0000C013
#define ACLK_AVSP_GATE                           0x0000C014

// VCPCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_VCP_DIV                      0x05040C00
#define CLK_VCP_PVTPLL_SRC_SEL                   0x01000C00
#define CLK_VCP_PVTPLL_SRC_SEL_CLK_DEEPSLOW      0U
#define CLK_VCP_PVTPLL_SRC_SEL_CLK_VCP_PVTPLL    1U
#define MBIST_CLK_PVTPLL_VCP_SEL                 0x01010C00
#define MBIST_CLK_PVTPLL_VCP_SEL_MBIST_CLK_500M  0U
#define MBIST_CLK_PVTPLL_VCP_SEL_CLK_VCP_PVTPLL_SRC 1U
#define MBIST_CLK_PVTPLL_AISP_SEL                0x01020C00
#define MBIST_CLK_PVTPLL_AISP_SEL_MBIST_CLK_AISP 0U
#define MBIST_CLK_PVTPLL_AISP_SEL_CLK_VCP_PVTPLL_SRC 1U
#define CLK_TESTOUT_VCP_SEL                      0x030A0C00
#define CLK_TESTOUT_VCP_SEL_ACLK_VCP_ROOT        0U
#define CLK_TESTOUT_VCP_SEL_HCLK_VCP_ROOT        1U
#define CLK_TESTOUT_VCP_SEL_PCLK_VCP_ROOT        2U
#define CLK_TESTOUT_VCP_SEL_CLK_CORE_FEC         3U
#define CLK_TESTOUT_VCP_SEL_CLK_CORE_AISP        5U
#define CLK_TESTOUT_VCP_SEL_TEST_CLKOUT_VCP_PVTPLL 7U
#define CLK_CORE_FEC_SEL                         0x010D0C00
#define CLK_CORE_FEC_SEL_CLK_CORE_FEC_SRC        0U
#define CLK_CORE_FEC_SEL_CLK_VCP_PVTPLL_SRC      1U
#define CLK_CORE_AISP_SEL                        0x010F0C00
#define CLK_CORE_AISP_SEL_CLK_AISP_PLL_SRC       0U
#define CLK_CORE_AISP_SEL_CLK_VCP_PVTPLL_SRC     1U

// ====================== SBUSCRU module definition bank=13 =======================
// SBUSCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_CRU_SBUS                    0x0000D001
#define SRST_PRESETN_SYS_SGRF                    0x0000D002
#define SRST_PRESETN_STIMER                      0x0000D003
#define SRST_RESETN_STIMER0                      0x0000D004
#define SRST_RESETN_STIMER1                      0x0000D005
#define SRST_PRESETN_WDT_S                       0x0000D006
#define SRST_TRESETN_WDT_S                       0x0000D007
#define SRST_HRESETN_S_RKCE                      0x0000D008
#define SRST_HRESETN_KL_RKCE                     0x0000D009
#define SRST_PRESETN_OTPC_S                      0x0000D00A
#define SRST_RESETN_SBPI_OTPC_S                  0x0000D00B
#define SRST_RESETN_USER_OTPC_S                  0x0000D00C
#define SRST_PRESETN_KEY_READER                  0x0000D00D

// SBUSCRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_ARESETN_RKCE_S                        0x0000D02C
#define SRST_RESETN_PKA_RKCE_S                     0x0000D02D

// SBUSCRU_GATE_CON00(Offset:0x800)
#define PCLK_CRU_SBUS_GATE                       0x0000D001
#define PCLK_SYS_SGRF_GATE                       0x0000D002
#define PCLK_STIMER_GATE                         0x0000D003
#define CLK_STIMER0_GATE                         0x0000D004
#define CLK_STIMER1_GATE                         0x0000D005
#define PCLK_WDT_S_GATE                          0x0000D006
#define TCLK_WDT_S_GATE                          0x0000D007
#define HCLK_S_RKCE_GATE                         0x0000D008
#define HCLK_KL_RKCE_GATE                        0x0000D009
#define PCLK_OTPC_S_GATE                         0x0000D00A
#define CLK_SBPI_OTPC_S_GATE                     0x0000D00B
#define CLK_USER_OTPC_S_GATE                     0x0000D00C
#define PCLK_KEY_READER_GATE                     0x0000D00D

// SBUSCRU_GATE_CON02(Offset:0x808)
#define ACLK_RKCE_S_GATE                           0x0000D02C
#define CLK_PKA_RKCE_S_GATE                        0x0000D02D

// SBUSCRU_CLKSEL_CON00(Offset:0x300)

// SBUSCRU_CLKSEL_CON02(Offset:0x308)
#define CLK_USER_OTPC_S_DIV                      0x030C0D02

// ====================== SPMUCRU module definition bank=14 =======================
// SPMUCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_CRU_SPMU                    0x0000E000
#define SRST_PRESETN_PMU_SGRF                    0x0000E001
#define SRST_PRESETN_PMU_SGRF_REMAP              0x0000E002

// SPMUCRU_GATE_CON00(Offset:0x800)
#define PCLK_CRU_SPMU_GATE                       0x0000E000
#define PCLK_PMU_SGRF_GATE                       0x0000E001

#endif /* __RV1103B_CRU_H */