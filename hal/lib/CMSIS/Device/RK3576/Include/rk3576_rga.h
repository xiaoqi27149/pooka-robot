/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#ifndef __RK3576_RGA_H
#define __RK3576_RGA_H
#ifdef __cplusplus
  extern "C" {
#endif
/****************************************************************************************/
/*                                                                                      */
/*                               Module Structure Section                               */
/*                                                                                      */
/****************************************************************************************/
#ifndef __ASSEMBLY__
/* RGA2 Register Structure Define */
struct RGA2_REG {
    __IO uint32_t SYS_CTRL;                           /* Address Offset: 0x0000 */
    __IO uint32_t CMD_CTRL;                           /* Address Offset: 0x0004 */
    __IO uint32_t CMD_BASE;                           /* Address Offset: 0x0008 */
    __I  uint32_t STATUS1;                            /* Address Offset: 0x000C */
    __IO uint32_t INT;                                /* Address Offset: 0x0010 */
    __IO uint32_t MMU_CTRL0;                          /* Address Offset: 0x0014 */
         uint32_t RESERVED0018;                       /* Address Offset: 0x0018 */
    __I  uint32_t STATUS2;                            /* Address Offset: 0x001C */
    __I  uint32_t WORK_CNT;                           /* Address Offset: 0x0020 */
         uint32_t RESERVED0024;                       /* Address Offset: 0x0024 */
    __IO uint32_t VERSION_INFO;                       /* Address Offset: 0x0028 */
         uint32_t RESERVED002C;                       /* Address Offset: 0x002C */
    __IO uint32_t READ_LINE_CNT_TH;                   /* Address Offset: 0x0030 */
    __IO uint32_t WRITE_LINE_CNT_TH;                  /* Address Offset: 0x0034 */
    __I  uint32_t INT_LINE_CNT;                       /* Address Offset: 0x0038 */
         uint32_t RESERVED003C[9];                    /* Address Offset: 0x003C */
    __IO uint32_t DST_CSC_00;                         /* Address Offset: 0x0060 */
    __IO uint32_t DST_CSC_01;                         /* Address Offset: 0x0064 */
    __IO uint32_t DST_CSC_02;                         /* Address Offset: 0x0068 */
    __IO uint32_t DST_CSC_OFF0;                       /* Address Offset: 0x006C */
    __IO uint32_t DST_CSC_10;                         /* Address Offset: 0x0070 */
    __IO uint32_t DST_CSC_11;                         /* Address Offset: 0x0074 */
    __IO uint32_t DST_CSC_12;                         /* Address Offset: 0x0078 */
    __IO uint32_t DST_CSC_OFF1;                       /* Address Offset: 0x007C */
    __IO uint32_t DST_CSC_20;                         /* Address Offset: 0x0080 */
    __IO uint32_t DST_CSC_21;                         /* Address Offset: 0x0084 */
    __IO uint32_t DST_CSC_22;                         /* Address Offset: 0x0088 */
    __IO uint32_t DST_CSC_OFF2;                       /* Address Offset: 0x008C */
         uint32_t RESERVED0090[28];                   /* Address Offset: 0x0090 */
    __IO uint32_t MODE_CTRL;                          /* Address Offset: 0x0100 */
    __IO uint32_t SRC_INFO;                           /* Address Offset: 0x0104 */
    union {
        __IO uint32_t SRC_BASE0;                          /* Address Offset: 0x0108 */
        __IO uint32_t FBCIN_HEAD_BASE;                    /* Address Offset: 0x0108 */
    };
    union {
        __IO uint32_t SRC_BASE1;                          /* Address Offset: 0x010C */
        __IO uint32_t FBCIN_PAYL_BASE;                    /* Address Offset: 0x010C */
    };
    union {
        __IO uint32_t SRC_BASE2;                          /* Address Offset: 0x0110 */
        __IO uint32_t FBCIN_ACT_OFF;                      /* Address Offset: 0x0110 */
    };
    __IO uint32_t SRC_BASE3;                          /* Address Offset: 0x0114 */
    __IO uint32_t SRC_VIR_INFO;                       /* Address Offset: 0x0118 */
    __IO uint32_t SRC_ACT_INFO;                       /* Address Offset: 0x011C */
    union {
        __IO uint32_t SRC_X_FACTOR;                       /* Address Offset: 0x0120 */
        __IO uint32_t OSD_CTRL0;                          /* Address Offset: 0x0120 */
    };
    union {
        __IO uint32_t SRC_Y_FACTOR;                       /* Address Offset: 0x0124 */
        __IO uint32_t OSD_CTRL1;                          /* Address Offset: 0x0124 */
    };
    union {
        __IO uint32_t SRC_BG_COLOR;                       /* Address Offset: 0x0128 */
        __IO uint32_t OSD_COLOR0;                         /* Address Offset: 0x0128 */
    };
    union {
        __IO uint32_t SRC_FG_COLOR;                       /* Address Offset: 0x012C */
        __IO uint32_t OSD_COLOR1;                         /* Address Offset: 0x012C */
    };
    union {
        __IO uint32_t MOSAIC_MODE;                        /* Address Offset: 0x0130 */
        __IO uint32_t SRC_TR_COLOR0;                      /* Address Offset: 0x0130 */
        __IO uint32_t CP_GR_A;                            /* Address Offset: 0x0130 */
        __IO uint32_t OSD_LAST_FLAGS0;                    /* Address Offset: 0x0130 */
    };
    union {
        __IO uint32_t SRC_TR_COLOR1;                      /* Address Offset: 0x0134 */
        __IO uint32_t CP_GR_B;                            /* Address Offset: 0x0134 */
        __IO uint32_t OSD_LAST_FLAGS1;                    /* Address Offset: 0x0134 */
    };
    __IO uint32_t DST_INFO;                           /* Address Offset: 0x0138 */
    __IO uint32_t DST_BASE0;                          /* Address Offset: 0x013C */
    __IO uint32_t DST_BASE1;                          /* Address Offset: 0x0140 */
    __IO uint32_t DST_BASE2;                          /* Address Offset: 0x0144 */
    __IO uint32_t DST_VIR_INFO;                       /* Address Offset: 0x0148 */
    __IO uint32_t DST_ACT_INFO;                       /* Address Offset: 0x014C */
    __IO uint32_t ALPHA_CTRL0;                        /* Address Offset: 0x0150 */
    __IO uint32_t ALPHA_CTRL1;                        /* Address Offset: 0x0154 */
    __IO uint32_t FADING_CTRL;                        /* Address Offset: 0x0158 */
    __IO uint32_t PAT_CON;                            /* Address Offset: 0x015C */
    union {
        __IO uint32_t NN_QUANTIZE_SCALE;                  /* Address Offset: 0x0160 */
        __IO uint32_t ROP_CON0;                           /* Address Offset: 0x0160 */
        __IO uint32_t CP_GR_G;                            /* Address Offset: 0x0160 */
        __IO uint32_t DST_Y4MAP_LUT0;                     /* Address Offset: 0x0160 */
        __IO uint32_t OSD_INVERTSION_CAL0;                /* Address Offset: 0x0160 */
    };
    union {
        __IO uint32_t NN_QUANTIZE_OFFSET;                 /* Address Offset: 0x0164 */
        __IO uint32_t CP_GR_R;                            /* Address Offset: 0x0164 */
        __IO uint32_t DST_Y4MAP_LUT1;                     /* Address Offset: 0x0164 */
        __IO uint32_t ROP_CON1;                           /* Address Offset: 0x0164 */
        __IO uint32_t OSD_INVERTSION_CAL1;                /* Address Offset: 0x0164 */
    };
    __IO uint32_t MASK_BASE;                          /* Address Offset: 0x0168 */
    __IO uint32_t SRC_SCL_OFFSET;                     /* Address Offset: 0x016C */
    __IO uint32_t PREFETCH_ADDR_TH;                   /* Address Offset: 0x0170 */
         uint32_t RESERVED0174[163];                  /* Address Offset: 0x0174 */
    __I  uint32_t OSD_ST_LUMA0;                       /* Address Offset: 0x0400 */
    __I  uint32_t OSD_ST_LUMA1;                       /* Address Offset: 0x0404 */
    __I  uint32_t OSD_ST_LUMA2;                       /* Address Offset: 0x0408 */
    __I  uint32_t OSD_ST_LUMA3;                       /* Address Offset: 0x040C */
    __I  uint32_t OSD_ST_LUMA4;                       /* Address Offset: 0x0410 */
    __I  uint32_t OSD_ST_LUMA5;                       /* Address Offset: 0x0414 */
    __I  uint32_t OSD_ST_LUMA6;                       /* Address Offset: 0x0418 */
    __I  uint32_t OSD_ST_LUMA7;                       /* Address Offset: 0x041C */
    __I  uint32_t OSD_ST_LUMA8;                       /* Address Offset: 0x0420 */
    __I  uint32_t OSD_ST_LUMA9;                       /* Address Offset: 0x0424 */
    __I  uint32_t OSD_ST_LUMA10;                      /* Address Offset: 0x0428 */
    __I  uint32_t OSD_ST_LUMA11;                      /* Address Offset: 0x042C */
    __I  uint32_t OSD_ST_LUMA12;                      /* Address Offset: 0x0430 */
    __I  uint32_t OSD_ST_LUMA13;                      /* Address Offset: 0x0434 */
    __I  uint32_t OSD_ST_LUMA14;                      /* Address Offset: 0x0438 */
    __I  uint32_t OSD_ST_LUMA15;                      /* Address Offset: 0x043C */
         uint32_t RESERVED0440[688];                  /* Address Offset: 0x0440 */
    __IO uint32_t DTE_ADDR;                           /* Address Offset: 0x0F00 */
    __I  uint32_t STATUS;                             /* Address Offset: 0x0F04 */
    __O  uint32_t COMMAND;                            /* Address Offset: 0x0F08 */
    __I  uint32_t PAGE_FAULT_ADDR;                    /* Address Offset: 0x0F0C */
    __O  uint32_t ZAP_ONE_LINE;                       /* Address Offset: 0x0F10 */
    __IO uint32_t INT_RAWSTAT;                        /* Address Offset: 0x0F14 */
    __O  uint32_t INT_CLEAR;                          /* Address Offset: 0x0F18 */
    __IO uint32_t INT_MASK;                           /* Address Offset: 0x0F1C */
    __I  uint32_t INT_STATUS;                         /* Address Offset: 0x0F20 */
    __IO uint32_t AUTO_GATING;                        /* Address Offset: 0x0F24 */
};
#endif /*  __ASSEMBLY__  */
/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Memory Base */
#define RGA2_BASE                      0x27920000UL /* RGA2 base address */
/****************************************************************************************/
/*                                                                                      */
/*                               Module Variable Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Module Variable Define */

#define RGA2                ((struct RGA2_REG *) RGA2_BASE)

#define IS_RGA2_INSTANCE(instance) ((instance) == RGA2)
/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************RGA2******************************************/
/* SYS_CTRL */
#define RGA2_SYS_CTRL_OFFSET                               (0x0U)
#define RGA2_SYS_CTRL_SYS_CMD_OP_ST_P_SHIFT                (0U)
#define RGA2_SYS_CTRL_SYS_CMD_OP_ST_P_MASK                 (0x1U << RGA2_SYS_CTRL_SYS_CMD_OP_ST_P_SHIFT)                /* 0x00000001 */
#define RGA2_SYS_CTRL_SW_CMD_MODE_SHIFT                    (1U)
#define RGA2_SYS_CTRL_SW_CMD_MODE_MASK                     (0x1U << RGA2_SYS_CTRL_SW_CMD_MODE_SHIFT)                    /* 0x00000002 */
#define RGA2_SYS_CTRL_SW_AUTO_CKG_SHIFT                    (2U)
#define RGA2_SYS_CTRL_SW_AUTO_CKG_MASK                     (0x1U << RGA2_SYS_CTRL_SW_AUTO_CKG_SHIFT)                    /* 0x00000004 */
#define RGA2_SYS_CTRL_SYS_ACLK_SRESET_P_SHIFT              (3U)
#define RGA2_SYS_CTRL_SYS_ACLK_SRESET_P_MASK               (0x1U << RGA2_SYS_CTRL_SYS_ACLK_SRESET_P_SHIFT)              /* 0x00000008 */
#define RGA2_SYS_CTRL_SYS_CCLK_SRESET_P_SHIFT              (4U)
#define RGA2_SYS_CTRL_SYS_CCLK_SRESET_P_MASK               (0x1U << RGA2_SYS_CTRL_SYS_CCLK_SRESET_P_SHIFT)              /* 0x00000010 */
#define RGA2_SYS_CTRL_SW_AUTO_RST_SHIFT                    (5U)
#define RGA2_SYS_CTRL_SW_AUTO_RST_MASK                     (0x1U << RGA2_SYS_CTRL_SW_AUTO_RST_SHIFT)                    /* 0x00000020 */
#define RGA2_SYS_CTRL_SW_RST_PROTECT_E_SHIFT               (6U)
#define RGA2_SYS_CTRL_SW_RST_PROTECT_E_MASK                (0x1U << RGA2_SYS_CTRL_SW_RST_PROTECT_E_SHIFT)               /* 0x00000040 */
#define RGA2_SYS_CTRL_SYS_RST_HANDSAVE_P_SHIFT             (7U)
#define RGA2_SYS_CTRL_SYS_RST_HANDSAVE_P_MASK              (0x1U << RGA2_SYS_CTRL_SYS_RST_HANDSAVE_P_SHIFT)             /* 0x00000080 */
#define RGA2_SYS_CTRL_SW_RD_HOLD_MODE_EN_SHIFT             (9U)
#define RGA2_SYS_CTRL_SW_RD_HOLD_MODE_EN_MASK              (0x1U << RGA2_SYS_CTRL_SW_RD_HOLD_MODE_EN_SHIFT)             /* 0x00000200 */
#define RGA2_SYS_CTRL_SYS_CMD_CONTINUE_P_SHIFT             (10U)
#define RGA2_SYS_CTRL_SYS_CMD_CONTINUE_P_MASK              (0x1U << RGA2_SYS_CTRL_SYS_CMD_CONTINUE_P_SHIFT)             /* 0x00000400 */
#define RGA2_SYS_CTRL_SW_DST_WR_OPT_DIS_SHIFT              (11U)
#define RGA2_SYS_CTRL_SW_DST_WR_OPT_DIS_MASK               (0x1U << RGA2_SYS_CTRL_SW_DST_WR_OPT_DIS_SHIFT)              /* 0x00000800 */
#define RGA2_SYS_CTRL_SW_SRC0YUV420SP_RD_OPT_DIS_SHIFT     (12U)
#define RGA2_SYS_CTRL_SW_SRC0YUV420SP_RD_OPT_DIS_MASK      (0x1U << RGA2_SYS_CTRL_SW_SRC0YUV420SP_RD_OPT_DIS_SHIFT)     /* 0x00001000 */
#define RGA2_SYS_CTRL_SW_SRC1_RGB888_FIX_DIS_SHIFT         (13U)
#define RGA2_SYS_CTRL_SW_SRC1_RGB888_FIX_DIS_MASK          (0x1U << RGA2_SYS_CTRL_SW_SRC1_RGB888_FIX_DIS_SHIFT)         /* 0x00002000 */
/* CMD_CTRL */
#define RGA2_CMD_CTRL_OFFSET                               (0x4U)
#define RGA2_CMD_CTRL_SYS_CMD_LINE_ST_P_SHIFT              (0U)
#define RGA2_CMD_CTRL_SYS_CMD_LINE_ST_P_MASK               (0x1U << RGA2_CMD_CTRL_SYS_CMD_LINE_ST_P_SHIFT)              /* 0x00000001 */
#define RGA2_CMD_CTRL_SYS_CMD_INCR_VALID_P_SHIFT           (1U)
#define RGA2_CMD_CTRL_SYS_CMD_INCR_VALID_P_MASK            (0x1U << RGA2_CMD_CTRL_SYS_CMD_INCR_VALID_P_SHIFT)           /* 0x00000002 */
#define RGA2_CMD_CTRL_SW_CMD_STOP_MODE_SHIFT               (2U)
#define RGA2_CMD_CTRL_SW_CMD_STOP_MODE_MASK                (0x1U << RGA2_CMD_CTRL_SW_CMD_STOP_MODE_SHIFT)               /* 0x00000004 */
#define RGA2_CMD_CTRL_SW_CMD_INCR_NUM_SHIFT                (3U)
#define RGA2_CMD_CTRL_SW_CMD_INCR_NUM_MASK                 (0x3FFU << RGA2_CMD_CTRL_SW_CMD_INCR_NUM_SHIFT)              /* 0x00001FF8 */
#define RGA2_CMD_CTRL_SW_OUTSTANDING_CFG_DIS_SHIFT         (13U)
#define RGA2_CMD_CTRL_SW_OUTSTANDING_CFG_DIS_MASK          (0x1U << RGA2_CMD_CTRL_SW_OUTSTANDING_CFG_DIS_SHIFT)         /* 0x00002000 */
#define RGA2_CMD_CTRL_SW_MAX_RD_OUTSTANDING_SHIFT          (14U)
#define RGA2_CMD_CTRL_SW_MAX_RD_OUTSTANDING_MASK           (0x3FU << RGA2_CMD_CTRL_SW_MAX_RD_OUTSTANDING_SHIFT)         /* 0x000FC000 */
/* CMD_BASE */
#define RGA2_CMD_BASE_OFFSET                               (0x8U)
#define RGA2_CMD_BASE_SW_CMD_BASE_SHIFT                    (0U)
#define RGA2_CMD_BASE_SW_CMD_BASE_MASK                     (0xFFFFFFFFU << RGA2_CMD_BASE_SW_CMD_BASE_SHIFT)             /* 0xFFFFFFFF */
/* STATUS1 */
#define RGA2_STATUS1_OFFSET                                (0xCU)
#define RGA2_STATUS1                                       (0x0U)
#define RGA2_STATUS1_SW_RGA_STA_SHIFT                      (0U)
#define RGA2_STATUS1_SW_RGA_STA_MASK                       (0x1U << RGA2_STATUS1_SW_RGA_STA_SHIFT)                      /* 0x00000001 */
#define RGA2_STATUS1_SW_CMD_CUR_NUM_SHIFT                  (8U)
#define RGA2_STATUS1_SW_CMD_CUR_NUM_MASK                   (0xFFFU << RGA2_STATUS1_SW_CMD_CUR_NUM_SHIFT)                /* 0x000FFF00 */
#define RGA2_STATUS1_SW_CMD_TOTAL_NUM_SHIFT                (20U)
#define RGA2_STATUS1_SW_CMD_TOTAL_NUM_MASK                 (0xFFFU << RGA2_STATUS1_SW_CMD_TOTAL_NUM_SHIFT)              /* 0xFFF00000 */
/* INT */
#define RGA2_INT_OFFSET                                    (0x10U)
#define RGA2_INT_RO_BUS_ERROR_INTR_SHIFT                   (0U)
#define RGA2_INT_RO_BUS_ERROR_INTR_MASK                    (0x1U << RGA2_INT_RO_BUS_ERROR_INTR_SHIFT)                   /* 0x00000001 */
#define RGA2_INT_RO_RPP_ERROR_INTR_SHIFT                   (1U)
#define RGA2_INT_RO_RPP_ERROR_INTR_MASK                    (0x1U << RGA2_INT_RO_RPP_ERROR_INTR_SHIFT)                   /* 0x00000002 */
#define RGA2_INT_RO_CMD_ALL_FINISH_INTR_SHIFT              (2U)
#define RGA2_INT_RO_CMD_ALL_FINISH_INTR_MASK               (0x1U << RGA2_INT_RO_CMD_ALL_FINISH_INTR_SHIFT)              /* 0x00000004 */
#define RGA2_INT_RO_CMD_CUR_FINISH_INTR_SHIFT              (3U)
#define RGA2_INT_RO_CMD_CUR_FINISH_INTR_MASK               (0x1U << RGA2_INT_RO_CMD_CUR_FINISH_INTR_SHIFT)              /* 0x00000008 */
#define RGA2_INT_SYS_INTR_BUS_ERR_CLR_SHIFT                (4U)
#define RGA2_INT_SYS_INTR_BUS_ERR_CLR_MASK                 (0x1U << RGA2_INT_SYS_INTR_BUS_ERR_CLR_SHIFT)                /* 0x00000010 */
#define RGA2_INT_SYS_INTR_RPP_ERR_CLR_SHIFT                (5U)
#define RGA2_INT_SYS_INTR_RPP_ERR_CLR_MASK                 (0x1U << RGA2_INT_SYS_INTR_RPP_ERR_CLR_SHIFT)                /* 0x00000020 */
#define RGA2_INT_SYS_INTR_AF_CLR_SHIFT                     (6U)
#define RGA2_INT_SYS_INTR_AF_CLR_MASK                      (0x1U << RGA2_INT_SYS_INTR_AF_CLR_SHIFT)                     /* 0x00000040 */
#define RGA2_INT_SYS_INTR_CF_CLR_SHIFT                     (7U)
#define RGA2_INT_SYS_INTR_CF_CLR_MASK                      (0x1U << RGA2_INT_SYS_INTR_CF_CLR_SHIFT)                     /* 0x00000080 */
#define RGA2_INT_SW_INTR_BUS_ERR_E_SHIFT                   (8U)
#define RGA2_INT_SW_INTR_BUS_ERR_E_MASK                    (0x1U << RGA2_INT_SW_INTR_BUS_ERR_E_SHIFT)                   /* 0x00000100 */
#define RGA2_INT_SW_INTR_RPP_ERR_E_SHIFT                   (9U)
#define RGA2_INT_SW_INTR_RPP_ERR_E_MASK                    (0x1U << RGA2_INT_SW_INTR_RPP_ERR_E_SHIFT)                   /* 0x00000200 */
#define RGA2_INT_SW_INTR_AF_E_SHIFT                        (10U)
#define RGA2_INT_SW_INTR_AF_E_MASK                         (0x1U << RGA2_INT_SW_INTR_AF_E_SHIFT)                        /* 0x00000400 */
#define RGA2_INT_RO_READ_CNT_INTR_SHIFT                    (11U)
#define RGA2_INT_RO_READ_CNT_INTR_MASK                     (0x1U << RGA2_INT_RO_READ_CNT_INTR_SHIFT)                    /* 0x00000800 */
#define RGA2_INT_RO_WRITE_CNT_INTR_SHIFT                   (12U)
#define RGA2_INT_RO_WRITE_CNT_INTR_MASK                    (0x1U << RGA2_INT_RO_WRITE_CNT_INTR_SHIFT)                   /* 0x00001000 */
#define RGA2_INT_SW_INTR_LINE_RD_EN_SHIFT                  (13U)
#define RGA2_INT_SW_INTR_LINE_RD_EN_MASK                   (0x1U << RGA2_INT_SW_INTR_LINE_RD_EN_SHIFT)                  /* 0x00002000 */
#define RGA2_INT_SW_INTR_LINE_WR_EN_SHIFT                  (14U)
#define RGA2_INT_SW_INTR_LINE_WR_EN_MASK                   (0x1U << RGA2_INT_SW_INTR_LINE_WR_EN_SHIFT)                  /* 0x00004000 */
#define RGA2_INT_SYS_INTR_LINE_RD_CLR_SHIFT                (15U)
#define RGA2_INT_SYS_INTR_LINE_RD_CLR_MASK                 (0x1U << RGA2_INT_SYS_INTR_LINE_RD_CLR_SHIFT)                /* 0x00008000 */
#define RGA2_INT_SYS_INTR_LINE_WR_CLR_SHIFT                (16U)
#define RGA2_INT_SYS_INTR_LINE_WR_CLR_MASK                 (0x1U << RGA2_INT_SYS_INTR_LINE_WR_CLR_SHIFT)                /* 0x00010000 */
#define RGA2_INT_RO_SCL_ERROR_INTR_SHIFT                   (17U)
#define RGA2_INT_RO_SCL_ERROR_INTR_MASK                    (0x1U << RGA2_INT_RO_SCL_ERROR_INTR_SHIFT)                   /* 0x00020000 */
#define RGA2_INT_SW_INTR_SCL_ERR_E_SHIFT                   (18U)
#define RGA2_INT_SW_INTR_SCL_ERR_E_MASK                    (0x1U << RGA2_INT_SW_INTR_SCL_ERR_E_SHIFT)                   /* 0x00040000 */
#define RGA2_INT_SYS_INTR_SCL_ERR_CLR_SHIFT                (19U)
#define RGA2_INT_SYS_INTR_SCL_ERR_CLR_MASK                 (0x1U << RGA2_INT_SYS_INTR_SCL_ERR_CLR_SHIFT)                /* 0x00080000 */
#define RGA2_INT_SYS_INTR_PRE_TH_CLR_SHIFT                 (20U)
#define RGA2_INT_SYS_INTR_PRE_TH_CLR_MASK                  (0x1U << RGA2_INT_SYS_INTR_PRE_TH_CLR_SHIFT)                 /* 0x00100000 */
#define RGA2_INT_RO_PREFETCH_TH_INTR_SHIFT                 (21U)
#define RGA2_INT_RO_PREFETCH_TH_INTR_MASK                  (0x1U << RGA2_INT_RO_PREFETCH_TH_INTR_SHIFT)                 /* 0x00200000 */
#define RGA2_INT_RO_INTR_FBCIN_DEC_ERR_SHIFT               (22U)
#define RGA2_INT_RO_INTR_FBCIN_DEC_ERR_MASK                (0x1U << RGA2_INT_RO_INTR_FBCIN_DEC_ERR_SHIFT)               /* 0x00400000 */
#define RGA2_INT_SW_INTR_FBCIN_DEC_ERR_E_SHIFT             (23U)
#define RGA2_INT_SW_INTR_FBCIN_DEC_ERR_E_MASK              (0x1U << RGA2_INT_SW_INTR_FBCIN_DEC_ERR_E_SHIFT)             /* 0x00800000 */
#define RGA2_INT_SYS_INTR_FBCIN_DEC_ERR_CLR_SHIFT          (24U)
#define RGA2_INT_SYS_INTR_FBCIN_DEC_ERR_CLR_MASK           (0x1U << RGA2_INT_SYS_INTR_FBCIN_DEC_ERR_CLR_SHIFT)          /* 0x01000000 */
/* MMU_CTRL0 */
#define RGA2_MMU_CTRL0_OFFSET                              (0x14U)
#define RGA2_MMU_CTRL0_SW_SRC_CH_PRIORITY_SHIFT            (3U)
#define RGA2_MMU_CTRL0_SW_SRC_CH_PRIORITY_MASK             (0x3U << RGA2_MMU_CTRL0_SW_SRC_CH_PRIORITY_SHIFT)            /* 0x00000018 */
#define RGA2_MMU_CTRL0_SW_SRC1_CH_PRIORITY_SHIFT           (5U)
#define RGA2_MMU_CTRL0_SW_SRC1_CH_PRIORITY_MASK            (0x3U << RGA2_MMU_CTRL0_SW_SRC1_CH_PRIORITY_SHIFT)           /* 0x00000060 */
#define RGA2_MMU_CTRL0_SW_DST_CH_PRIORITY_SHIFT            (7U)
#define RGA2_MMU_CTRL0_SW_DST_CH_PRIORITY_MASK             (0x3U << RGA2_MMU_CTRL0_SW_DST_CH_PRIORITY_SHIFT)            /* 0x00000180 */
#define RGA2_MMU_CTRL0_SW_ELS_CH_PRIORITY_SHIFT            (9U)
#define RGA2_MMU_CTRL0_SW_ELS_CH_PRIORITY_MASK             (0x3U << RGA2_MMU_CTRL0_SW_ELS_CH_PRIORITY_SHIFT)            /* 0x00000600 */
#define RGA2_MMU_CTRL0_SW_QOS_EN_SHIFT                     (11U)
#define RGA2_MMU_CTRL0_SW_QOS_EN_MASK                      (0x1U << RGA2_MMU_CTRL0_SW_QOS_EN_SHIFT)                     /* 0x00000800 */
#define RGA2_MMU_CTRL0_SW_MMU_QOS_NUM3_SHIFT               (12U)
#define RGA2_MMU_CTRL0_SW_MMU_QOS_NUM3_MASK                (0x7U << RGA2_MMU_CTRL0_SW_MMU_QOS_NUM3_SHIFT)               /* 0x00007000 */
#define RGA2_MMU_CTRL0_SW_MMU_QOS_NUM2_SHIFT               (15U)
#define RGA2_MMU_CTRL0_SW_MMU_QOS_NUM2_MASK                (0x7U << RGA2_MMU_CTRL0_SW_MMU_QOS_NUM2_SHIFT)               /* 0x00038000 */
/* STATUS2 */
#define RGA2_STATUS2_OFFSET                                (0x1CU)
#define RGA2_STATUS2                                       (0x0U)
#define RGA2_STATUS2_RPP_ERROR_SHIFT                       (0U)
#define RGA2_STATUS2_RPP_ERROR_MASK                        (0x1U << RGA2_STATUS2_RPP_ERROR_SHIFT)                       /* 0x00000001 */
#define RGA2_STATUS2_BUS_ERROR_SHIFT                       (1U)
#define RGA2_STATUS2_BUS_ERROR_MASK                        (0x1U << RGA2_STATUS2_BUS_ERROR_SHIFT)                       /* 0x00000002 */
#define RGA2_STATUS2_SRCRPP_OUTBUF_RREADY_SHIFT            (2U)
#define RGA2_STATUS2_SRCRPP_OUTBUF_RREADY_MASK             (0xFU << RGA2_STATUS2_SRCRPP_OUTBUF_RREADY_SHIFT)            /* 0x0000003C */
#define RGA2_STATUS2_DSTRPP_OUTBUF_RREADY_SHIFT            (6U)
#define RGA2_STATUS2_DSTRPP_OUTBUF_RREADY_MASK             (0x1FU << RGA2_STATUS2_DSTRPP_OUTBUF_RREADY_SHIFT)           /* 0x000007C0 */
#define RGA2_STATUS2_RPP_MKRAM_RREADY_SHIFT                (11U)
#define RGA2_STATUS2_RPP_MKRAM_RREADY_MASK                 (0x3U << RGA2_STATUS2_RPP_MKRAM_RREADY_SHIFT)                /* 0x00001800 */
/* WORK_CNT */
#define RGA2_WORK_CNT_OFFSET                               (0x20U)
#define RGA2_WORK_CNT                                      (0x0U)
#define RGA2_WORK_CNT_SW_WORK_CNT_SHIFT                    (0U)
#define RGA2_WORK_CNT_SW_WORK_CNT_MASK                     (0x7FFFFFFU << RGA2_WORK_CNT_SW_WORK_CNT_SHIFT)              /* 0x07FFFFFF */
/* VERSION_INFO */
#define RGA2_VERSION_INFO_OFFSET                           (0x28U)
#define RGA2_VERSION_INFO_SVNBUILD_SHIFT                   (0U)
#define RGA2_VERSION_INFO_SVNBUILD_MASK                    (0xFFFFFU << RGA2_VERSION_INFO_SVNBUILD_SHIFT)               /* 0x000FFFFF */
#define RGA2_VERSION_INFO_MINOR_SHIFT                      (20U)
#define RGA2_VERSION_INFO_MINOR_MASK                       (0xFU << RGA2_VERSION_INFO_MINOR_SHIFT)                      /* 0x00F00000 */
#define RGA2_VERSION_INFO_MAJOR_SHIFT                      (24U)
#define RGA2_VERSION_INFO_MAJOR_MASK                       (0xFFU << RGA2_VERSION_INFO_MAJOR_SHIFT)                     /* 0xFF000000 */
/* READ_LINE_CNT_TH */
#define RGA2_READ_LINE_CNT_TH_OFFSET                       (0x30U)
#define RGA2_READ_LINE_CNT_TH_SW_INTR_LINE_RD_TH_SHIFT     (0U)
#define RGA2_READ_LINE_CNT_TH_SW_INTR_LINE_RD_TH_MASK      (0x1FFFU << RGA2_READ_LINE_CNT_TH_SW_INTR_LINE_RD_TH_SHIFT)  /* 0x00001FFF */
/* WRITE_LINE_CNT_TH */
#define RGA2_WRITE_LINE_CNT_TH_OFFSET                      (0x34U)
#define RGA2_WRITE_LINE_CNT_TH_SW_INTR_LINE_WR_START_SHIFT (0U)
#define RGA2_WRITE_LINE_CNT_TH_SW_INTR_LINE_WR_START_MASK  (0x1FFFU << RGA2_WRITE_LINE_CNT_TH_SW_INTR_LINE_WR_START_SHIFT) /* 0x00001FFF */
#define RGA2_WRITE_LINE_CNT_TH_SW_INTR_LINE_WR_STEP_SHIFT  (16U)
#define RGA2_WRITE_LINE_CNT_TH_SW_INTR_LINE_WR_STEP_MASK   (0x1FFFU << RGA2_WRITE_LINE_CNT_TH_SW_INTR_LINE_WR_STEP_SHIFT) /* 0x1FFF0000 */
/* INT_LINE_CNT */
#define RGA2_INT_LINE_CNT_OFFSET                           (0x38U)
#define RGA2_INT_LINE_CNT                                  (0x0U)
#define RGA2_INT_LINE_CNT_RO_READ_LINE_CNT_SHIFT           (0U)
#define RGA2_INT_LINE_CNT_RO_READ_LINE_CNT_MASK            (0x1FFFU << RGA2_INT_LINE_CNT_RO_READ_LINE_CNT_SHIFT)        /* 0x00001FFF */
#define RGA2_INT_LINE_CNT_RO_DMA_CMD_HOLD_SHIFT            (13U)
#define RGA2_INT_LINE_CNT_RO_DMA_CMD_HOLD_MASK             (0x1U << RGA2_INT_LINE_CNT_RO_DMA_CMD_HOLD_SHIFT)            /* 0x00002000 */
#define RGA2_INT_LINE_CNT_RO_WRITE_LINE_CNT_SHIFT          (16U)
#define RGA2_INT_LINE_CNT_RO_WRITE_LINE_CNT_MASK           (0x1FFFU << RGA2_INT_LINE_CNT_RO_WRITE_LINE_CNT_SHIFT)       /* 0x1FFF0000 */
/* DST_CSC_00 */
#define RGA2_DST_CSC_00_OFFSET                             (0x60U)
#define RGA2_DST_CSC_00_SW_DST_COE_00_SHIFT                (0U)
#define RGA2_DST_CSC_00_SW_DST_COE_00_MASK                 (0x7FFU << RGA2_DST_CSC_00_SW_DST_COE_00_SHIFT)              /* 0x000007FF */
#define RGA2_DST_CSC_00_SW_DST_CSC_Y_MAX_SHIFT             (16U)
#define RGA2_DST_CSC_00_SW_DST_CSC_Y_MAX_MASK              (0xFFU << RGA2_DST_CSC_00_SW_DST_CSC_Y_MAX_SHIFT)            /* 0x00FF0000 */
#define RGA2_DST_CSC_00_SW_DST_CSC_Y_MIN_SHIFT             (24U)
#define RGA2_DST_CSC_00_SW_DST_CSC_Y_MIN_MASK              (0xFFU << RGA2_DST_CSC_00_SW_DST_CSC_Y_MIN_SHIFT)            /* 0xFF000000 */
/* DST_CSC_01 */
#define RGA2_DST_CSC_01_OFFSET                             (0x64U)
#define RGA2_DST_CSC_01_SW_DST_COE_01_SHIFT                (0U)
#define RGA2_DST_CSC_01_SW_DST_COE_01_MASK                 (0x7FFU << RGA2_DST_CSC_01_SW_DST_COE_01_SHIFT)              /* 0x000007FF */
#define RGA2_DST_CSC_01_SW_DST_CSC_UV_MAX_SHIFT            (16U)
#define RGA2_DST_CSC_01_SW_DST_CSC_UV_MAX_MASK             (0xFFU << RGA2_DST_CSC_01_SW_DST_CSC_UV_MAX_SHIFT)           /* 0x00FF0000 */
#define RGA2_DST_CSC_01_SW_DST_CSC_UV_MIN_SHIFT            (24U)
#define RGA2_DST_CSC_01_SW_DST_CSC_UV_MIN_MASK             (0xFFU << RGA2_DST_CSC_01_SW_DST_CSC_UV_MIN_SHIFT)           /* 0xFF000000 */
/* DST_CSC_02 */
#define RGA2_DST_CSC_02_OFFSET                             (0x68U)
#define RGA2_DST_CSC_02_SW_DST_COE_02_SHIFT                (0U)
#define RGA2_DST_CSC_02_SW_DST_COE_02_MASK                 (0x7FFU << RGA2_DST_CSC_02_SW_DST_COE_02_SHIFT)              /* 0x000007FF */
/* DST_CSC_OFF0 */
#define RGA2_DST_CSC_OFF0_OFFSET                           (0x6CU)
#define RGA2_DST_CSC_OFF0_SW_DST_COE_OFF0_SHIFT            (0U)
#define RGA2_DST_CSC_OFF0_SW_DST_COE_OFF0_MASK             (0x1FFFFFU << RGA2_DST_CSC_OFF0_SW_DST_COE_OFF0_SHIFT)       /* 0x001FFFFF */
/* DST_CSC_10 */
#define RGA2_DST_CSC_10_OFFSET                             (0x70U)
#define RGA2_DST_CSC_10_SW_DST_COE_10_SHIFT                (0U)
#define RGA2_DST_CSC_10_SW_DST_COE_10_MASK                 (0x7FFU << RGA2_DST_CSC_10_SW_DST_COE_10_SHIFT)              /* 0x000007FF */
/* DST_CSC_11 */
#define RGA2_DST_CSC_11_OFFSET                             (0x74U)
#define RGA2_DST_CSC_11_SW_DST_COE_11_SHIFT                (0U)
#define RGA2_DST_CSC_11_SW_DST_COE_11_MASK                 (0x7FFU << RGA2_DST_CSC_11_SW_DST_COE_11_SHIFT)              /* 0x000007FF */
/* DST_CSC_12 */
#define RGA2_DST_CSC_12_OFFSET                             (0x78U)
#define RGA2_DST_CSC_12_SW_DST_COE_12_SHIFT                (0U)
#define RGA2_DST_CSC_12_SW_DST_COE_12_MASK                 (0x7FFU << RGA2_DST_CSC_12_SW_DST_COE_12_SHIFT)              /* 0x000007FF */
/* DST_CSC_OFF1 */
#define RGA2_DST_CSC_OFF1_OFFSET                           (0x7CU)
#define RGA2_DST_CSC_OFF1_SW_DST_COE_OFF1_SHIFT            (0U)
#define RGA2_DST_CSC_OFF1_SW_DST_COE_OFF1_MASK             (0x1FFFFFU << RGA2_DST_CSC_OFF1_SW_DST_COE_OFF1_SHIFT)       /* 0x001FFFFF */
/* DST_CSC_20 */
#define RGA2_DST_CSC_20_OFFSET                             (0x80U)
#define RGA2_DST_CSC_20_SW_DST_COE_20_SHIFT                (0U)
#define RGA2_DST_CSC_20_SW_DST_COE_20_MASK                 (0x7FFU << RGA2_DST_CSC_20_SW_DST_COE_20_SHIFT)              /* 0x000007FF */
/* DST_CSC_21 */
#define RGA2_DST_CSC_21_OFFSET                             (0x84U)
#define RGA2_DST_CSC_21_SW_DST_COE_21_SHIFT                (0U)
#define RGA2_DST_CSC_21_SW_DST_COE_21_MASK                 (0x7FFU << RGA2_DST_CSC_21_SW_DST_COE_21_SHIFT)              /* 0x000007FF */
/* DST_CSC_22 */
#define RGA2_DST_CSC_22_OFFSET                             (0x88U)
#define RGA2_DST_CSC_22_SW_DST_COE_22_SHIFT                (0U)
#define RGA2_DST_CSC_22_SW_DST_COE_22_MASK                 (0x7FFU << RGA2_DST_CSC_22_SW_DST_COE_22_SHIFT)              /* 0x000007FF */
/* DST_CSC_OFF2 */
#define RGA2_DST_CSC_OFF2_OFFSET                           (0x8CU)
#define RGA2_DST_CSC_OFF2_SW_DST_COE_OFF2_SHIFT            (0U)
#define RGA2_DST_CSC_OFF2_SW_DST_COE_OFF2_MASK             (0x1FFFFFU << RGA2_DST_CSC_OFF2_SW_DST_COE_OFF2_SHIFT)       /* 0x001FFFFF */
/* MODE_CTRL */
#define RGA2_MODE_CTRL_OFFSET                              (0x100U)
#define RGA2_MODE_CTRL_SW_RENDER_MODE_SHIFT                (0U)
#define RGA2_MODE_CTRL_SW_RENDER_MODE_MASK                 (0x7U << RGA2_MODE_CTRL_SW_RENDER_MODE_SHIFT)                /* 0x00000007 */
#define RGA2_MODE_CTRL_SW_BB_MODE_SHIFT                    (3U)
#define RGA2_MODE_CTRL_SW_BB_MODE_MASK                     (0x1U << RGA2_MODE_CTRL_SW_BB_MODE_SHIFT)                    /* 0x00000008 */
#define RGA2_MODE_CTRL_SW_CF_ROP4_PAT_SHIFT                (4U)
#define RGA2_MODE_CTRL_SW_CF_ROP4_PAT_MASK                 (0x1U << RGA2_MODE_CTRL_SW_CF_ROP4_PAT_SHIFT)                /* 0x00000010 */
#define RGA2_MODE_CTRL_SW_ALPHA_ZERO_KEY_SHIFT             (5U)
#define RGA2_MODE_CTRL_SW_ALPHA_ZERO_KEY_MASK              (0x1U << RGA2_MODE_CTRL_SW_ALPHA_ZERO_KEY_SHIFT)             /* 0x00000020 */
#define RGA2_MODE_CTRL_SW_GRADIENT_SAT_SHIFT               (6U)
#define RGA2_MODE_CTRL_SW_GRADIENT_SAT_MASK                (0x1U << RGA2_MODE_CTRL_SW_GRADIENT_SAT_SHIFT)               /* 0x00000040 */
#define RGA2_MODE_CTRL_SW_INTR_CF_E_SHIFT                  (7U)
#define RGA2_MODE_CTRL_SW_INTR_CF_E_MASK                   (0x1U << RGA2_MODE_CTRL_SW_INTR_CF_E_SHIFT)                  /* 0x00000080 */
#define RGA2_MODE_CTRL_SW_OSD_EN_SHIFT                     (8U)
#define RGA2_MODE_CTRL_SW_OSD_EN_MASK                      (0x1U << RGA2_MODE_CTRL_SW_OSD_EN_SHIFT)                     /* 0x00000100 */
#define RGA2_MODE_CTRL_SW_MOSAIC_EN_SHIFT                  (9U)
#define RGA2_MODE_CTRL_SW_MOSAIC_EN_MASK                   (0x1U << RGA2_MODE_CTRL_SW_MOSAIC_EN_SHIFT)                  /* 0x00000200 */
#define RGA2_MODE_CTRL_SW_YIN_YOUT_MODE_SHIFT              (10U)
#define RGA2_MODE_CTRL_SW_YIN_YOUT_MODE_MASK               (0x1U << RGA2_MODE_CTRL_SW_YIN_YOUT_MODE_SHIFT)              /* 0x00000400 */
#define RGA2_MODE_CTRL_SW_TILE4X4_IN_EN_SHIFT              (12U)
#define RGA2_MODE_CTRL_SW_TILE4X4_IN_EN_MASK               (0x1U << RGA2_MODE_CTRL_SW_TILE4X4_IN_EN_SHIFT)              /* 0x00001000 */
#define RGA2_MODE_CTRL_SW_TILE4X4_OUT_EN_SHIFT             (13U)
#define RGA2_MODE_CTRL_SW_TILE4X4_OUT_EN_MASK              (0x1U << RGA2_MODE_CTRL_SW_TILE4X4_OUT_EN_SHIFT)             /* 0x00002000 */
#define RGA2_MODE_CTRL_SW_TABLE_PRE_FETCH_MODE_SHIFT       (14U)
#define RGA2_MODE_CTRL_SW_TABLE_PRE_FETCH_MODE_MASK        (0x3U << RGA2_MODE_CTRL_SW_TABLE_PRE_FETCH_MODE_SHIFT)       /* 0x0000C000 */
#define RGA2_MODE_CTRL_SW_FBCIN_EN_SHIFT                   (16U)
#define RGA2_MODE_CTRL_SW_FBCIN_EN_MASK                    (0x1U << RGA2_MODE_CTRL_SW_FBCIN_EN_SHIFT)                   /* 0x00010000 */
#define RGA2_MODE_CTRL_SW_FBCIN_BSP_DIS_SHIFT              (18U)
#define RGA2_MODE_CTRL_SW_FBCIN_BSP_DIS_MASK               (0x1U << RGA2_MODE_CTRL_SW_FBCIN_BSP_DIS_SHIFT)              /* 0x00040000 */
#define RGA2_MODE_CTRL_SW_TABLE_PRE_FETCH_DIS_SHIFT        (19U)
#define RGA2_MODE_CTRL_SW_TABLE_PRE_FETCH_DIS_MASK         (0x1U << RGA2_MODE_CTRL_SW_TABLE_PRE_FETCH_DIS_SHIFT)        /* 0x00080000 */
#define RGA2_MODE_CTRL_SW_AXI_WR128_DIS_SHIFT              (20U)
#define RGA2_MODE_CTRL_SW_AXI_WR128_DIS_MASK               (0x1U << RGA2_MODE_CTRL_SW_AXI_WR128_DIS_SHIFT)              /* 0x00100000 */
#define RGA2_MODE_CTRL_SW_HSP_LEFT_COPY_DIS_SHIFT          (21U)
#define RGA2_MODE_CTRL_SW_HSP_LEFT_COPY_DIS_MASK           (0x1U << RGA2_MODE_CTRL_SW_HSP_LEFT_COPY_DIS_SHIFT)          /* 0x00200000 */
/* SRC_INFO */
#define RGA2_SRC_INFO_OFFSET                               (0x104U)
#define RGA2_SRC_INFO_SW_SRC_FMT_SHIFT                     (0U)
#define RGA2_SRC_INFO_SW_SRC_FMT_MASK                      (0xFU << RGA2_SRC_INFO_SW_SRC_FMT_SHIFT)                     /* 0x0000000F */
#define RGA2_SRC_INFO_SW_SRC_RBSWAP_SHIFT                  (4U)
#define RGA2_SRC_INFO_SW_SRC_RBSWAP_MASK                   (0x1U << RGA2_SRC_INFO_SW_SRC_RBSWAP_SHIFT)                  /* 0x00000010 */
#define RGA2_SRC_INFO_SW_SRC_ALPHA_SWAP_SHIFT              (5U)
#define RGA2_SRC_INFO_SW_SRC_ALPHA_SWAP_MASK               (0x1U << RGA2_SRC_INFO_SW_SRC_ALPHA_SWAP_SHIFT)              /* 0x00000020 */
#define RGA2_SRC_INFO_SW_SRC_UVSWAP_SHIFT                  (6U)
#define RGA2_SRC_INFO_SW_SRC_UVSWAP_MASK                   (0x1U << RGA2_SRC_INFO_SW_SRC_UVSWAP_SHIFT)                  /* 0x00000040 */
#define RGA2_SRC_INFO_SW_CP_ENDIAN_SHIFT                   (7U)
#define RGA2_SRC_INFO_SW_CP_ENDIAN_MASK                    (0x1U << RGA2_SRC_INFO_SW_CP_ENDIAN_SHIFT)                   /* 0x00000080 */
#define RGA2_SRC_INFO_SW_SRC_CSC_MODE_SHIFT                (8U)
#define RGA2_SRC_INFO_SW_SRC_CSC_MODE_MASK                 (0x3U << RGA2_SRC_INFO_SW_SRC_CSC_MODE_SHIFT)                /* 0x00000300 */
#define RGA2_SRC_INFO_SW_SRC_ROT_MODE_SHIFT                (10U)
#define RGA2_SRC_INFO_SW_SRC_ROT_MODE_MASK                 (0x3U << RGA2_SRC_INFO_SW_SRC_ROT_MODE_SHIFT)                /* 0x00000C00 */
#define RGA2_SRC_INFO_SW_SRC_MIR_MODE_SHIFT                (12U)
#define RGA2_SRC_INFO_SW_SRC_MIR_MODE_MASK                 (0x3U << RGA2_SRC_INFO_SW_SRC_MIR_MODE_SHIFT)                /* 0x00003000 */
#define RGA2_SRC_INFO_SW_SRC_HSCL_MODE_SHIFT               (14U)
#define RGA2_SRC_INFO_SW_SRC_HSCL_MODE_MASK                (0x3U << RGA2_SRC_INFO_SW_SRC_HSCL_MODE_SHIFT)               /* 0x0000C000 */
#define RGA2_SRC_INFO_SW_SRC_VSCL_MODE_SHIFT               (16U)
#define RGA2_SRC_INFO_SW_SRC_VSCL_MODE_MASK                (0x3U << RGA2_SRC_INFO_SW_SRC_VSCL_MODE_SHIFT)               /* 0x00030000 */
#define RGA2_SRC_INFO_SW_SRC_TRANS_MODE_SHIFT              (18U)
#define RGA2_SRC_INFO_SW_SRC_TRANS_MODE_MASK               (0x1U << RGA2_SRC_INFO_SW_SRC_TRANS_MODE_SHIFT)              /* 0x00040000 */
#define RGA2_SRC_INFO_SW_SRC_TRANS_E_SHIFT                 (19U)
#define RGA2_SRC_INFO_SW_SRC_TRANS_E_MASK                  (0xFU << RGA2_SRC_INFO_SW_SRC_TRANS_E_SHIFT)                 /* 0x00780000 */
#define RGA2_SRC_INFO_SW_SRC_DITHER_UP_SHIFT               (23U)
#define RGA2_SRC_INFO_SW_SRC_DITHER_UP_MASK                (0x1U << RGA2_SRC_INFO_SW_SRC_DITHER_UP_SHIFT)               /* 0x00800000 */
#define RGA2_SRC_INFO_SW_BIC_COE_SEL_SHIFT                 (24U)
#define RGA2_SRC_INFO_SW_BIC_COE_SEL_MASK                  (0x3U << RGA2_SRC_INFO_SW_BIC_COE_SEL_SHIFT)                 /* 0x03000000 */
#define RGA2_SRC_INFO_SW_VSP_MODE_SHIFT                    (26U)
#define RGA2_SRC_INFO_SW_VSP_MODE_MASK                     (0x1U << RGA2_SRC_INFO_SW_VSP_MODE_SHIFT)                    /* 0x04000000 */
#define RGA2_SRC_INFO_SW_SRC_YUV10_E_SHIFT                 (27U)
#define RGA2_SRC_INFO_SW_SRC_YUV10_E_MASK                  (0x1U << RGA2_SRC_INFO_SW_SRC_YUV10_E_SHIFT)                 /* 0x08000000 */
#define RGA2_SRC_INFO_SW_SRC_YUV10_ROUND_DIS_SHIFT         (28U)
#define RGA2_SRC_INFO_SW_SRC_YUV10_ROUND_DIS_MASK          (0x1U << RGA2_SRC_INFO_SW_SRC_YUV10_ROUND_DIS_SHIFT)         /* 0x10000000 */
#define RGA2_SRC_INFO_SW_VSD_MODE_SHIFT                    (29U)
#define RGA2_SRC_INFO_SW_VSD_MODE_MASK                     (0x1U << RGA2_SRC_INFO_SW_VSD_MODE_SHIFT)                    /* 0x20000000 */
#define RGA2_SRC_INFO_SW_HSP_MODE_SHIFT                    (30U)
#define RGA2_SRC_INFO_SW_HSP_MODE_MASK                     (0x1U << RGA2_SRC_INFO_SW_HSP_MODE_SHIFT)                    /* 0x40000000 */
#define RGA2_SRC_INFO_SW_HSD_MODE_SHIFT                    (31U)
#define RGA2_SRC_INFO_SW_HSD_MODE_MASK                     (0x1U << RGA2_SRC_INFO_SW_HSD_MODE_SHIFT)                    /* 0x80000000 */
/* SRC_BASE0 */
#define RGA2_SRC_BASE0_OFFSET                              (0x108U)
#define RGA2_SRC_BASE0_SW_SRC_BASE0_SHIFT                  (0U)
#define RGA2_SRC_BASE0_SW_SRC_BASE0_MASK                   (0xFFFFFFFFU << RGA2_SRC_BASE0_SW_SRC_BASE0_SHIFT)           /* 0xFFFFFFFF */
/* FBCIN_HEAD_BASE */
#define RGA2_FBCIN_HEAD_BASE_OFFSET                        (0x108U)
#define RGA2_FBCIN_HEAD_BASE_SW_FBCIN_HEAD_BASE_SHIFT      (0U)
#define RGA2_FBCIN_HEAD_BASE_SW_FBCIN_HEAD_BASE_MASK       (0xFFFFFFFFU << RGA2_FBCIN_HEAD_BASE_SW_FBCIN_HEAD_BASE_SHIFT) /* 0xFFFFFFFF */
/* SRC_BASE1 */
#define RGA2_SRC_BASE1_OFFSET                              (0x10CU)
#define RGA2_SRC_BASE1_SW_SRC_BASE1_SHIFT                  (0U)
#define RGA2_SRC_BASE1_SW_SRC_BASE1_MASK                   (0xFFFFFFFFU << RGA2_SRC_BASE1_SW_SRC_BASE1_SHIFT)           /* 0xFFFFFFFF */
/* FBCIN_PAYL_BASE */
#define RGA2_FBCIN_PAYL_BASE_OFFSET                        (0x10CU)
#define RGA2_FBCIN_PAYL_BASE_SW_FBCIN_PAYL_BASE_SHIFT      (0U)
#define RGA2_FBCIN_PAYL_BASE_SW_FBCIN_PAYL_BASE_MASK       (0xFFFFFFFFU << RGA2_FBCIN_PAYL_BASE_SW_FBCIN_PAYL_BASE_SHIFT) /* 0xFFFFFFFF */
/* SRC_BASE2 */
#define RGA2_SRC_BASE2_OFFSET                              (0x110U)
#define RGA2_SRC_BASE2_SW_SRC_BASE2_SHIFT                  (0U)
#define RGA2_SRC_BASE2_SW_SRC_BASE2_MASK                   (0xFFFFFFFFU << RGA2_SRC_BASE2_SW_SRC_BASE2_SHIFT)           /* 0xFFFFFFFF */
/* FBCIN_ACT_OFF */
#define RGA2_FBCIN_ACT_OFF_OFFSET                          (0x110U)
#define RGA2_FBCIN_ACT_OFF_SW_FBCIN_ACT_XOFF_SHIFT         (0U)
#define RGA2_FBCIN_ACT_OFF_SW_FBCIN_ACT_XOFF_MASK          (0x1FFFU << RGA2_FBCIN_ACT_OFF_SW_FBCIN_ACT_XOFF_SHIFT)      /* 0x00001FFF */
#define RGA2_FBCIN_ACT_OFF_SW_FBCIN_ACT_YOFF_SHIFT         (16U)
#define RGA2_FBCIN_ACT_OFF_SW_FBCIN_ACT_YOFF_MASK          (0x1FFFU << RGA2_FBCIN_ACT_OFF_SW_FBCIN_ACT_YOFF_SHIFT)      /* 0x1FFF0000 */
/* SRC_BASE3 */
#define RGA2_SRC_BASE3_OFFSET                              (0x114U)
#define RGA2_SRC_BASE3_SW_SRC_BASE3_SHIFT                  (0U)
#define RGA2_SRC_BASE3_SW_SRC_BASE3_MASK                   (0xFFFFFFFFU << RGA2_SRC_BASE3_SW_SRC_BASE3_SHIFT)           /* 0xFFFFFFFF */
/* SRC_VIR_INFO */
#define RGA2_SRC_VIR_INFO_OFFSET                           (0x118U)
#define RGA2_SRC_VIR_INFO_SW_SRC_VIR_STRIDE_SHIFT          (0U)
#define RGA2_SRC_VIR_INFO_SW_SRC_VIR_STRIDE_MASK           (0xFFFFU << RGA2_SRC_VIR_INFO_SW_SRC_VIR_STRIDE_SHIFT)       /* 0x0000FFFF */
#define RGA2_SRC_VIR_INFO_SW_MASK_VIR_STRIDE_SHIFT         (16U)
#define RGA2_SRC_VIR_INFO_SW_MASK_VIR_STRIDE_MASK          (0x3FFU << RGA2_SRC_VIR_INFO_SW_MASK_VIR_STRIDE_SHIFT)       /* 0x03FF0000 */
/* SRC_ACT_INFO */
#define RGA2_SRC_ACT_INFO_OFFSET                           (0x11CU)
#define RGA2_SRC_ACT_INFO_SW_SRC_ACT_WIDTH_SHIFT           (0U)
#define RGA2_SRC_ACT_INFO_SW_SRC_ACT_WIDTH_MASK            (0x1FFFU << RGA2_SRC_ACT_INFO_SW_SRC_ACT_WIDTH_SHIFT)        /* 0x00001FFF */
#define RGA2_SRC_ACT_INFO_SW_TILE4X4_IN_XOFF_SHIFT         (14U)
#define RGA2_SRC_ACT_INFO_SW_TILE4X4_IN_XOFF_MASK          (0x3U << RGA2_SRC_ACT_INFO_SW_TILE4X4_IN_XOFF_SHIFT)         /* 0x0000C000 */
#define RGA2_SRC_ACT_INFO_SW_SRC_ACT_HEIGHT_SHIFT          (16U)
#define RGA2_SRC_ACT_INFO_SW_SRC_ACT_HEIGHT_MASK           (0x1FFFU << RGA2_SRC_ACT_INFO_SW_SRC_ACT_HEIGHT_SHIFT)       /* 0x1FFF0000 */
#define RGA2_SRC_ACT_INFO_SW_TILE4X4_IN_YOFF_SHIFT         (30U)
#define RGA2_SRC_ACT_INFO_SW_TILE4X4_IN_YOFF_MASK          (0x3U << RGA2_SRC_ACT_INFO_SW_TILE4X4_IN_YOFF_SHIFT)         /* 0xC0000000 */
/* SRC_X_FACTOR */
#define RGA2_SRC_X_FACTOR_OFFSET                           (0x120U)
#define RGA2_SRC_X_FACTOR_SW_SRC_HSD_FACTOR_SHIFT          (0U)
#define RGA2_SRC_X_FACTOR_SW_SRC_HSD_FACTOR_MASK           (0xFFFFU << RGA2_SRC_X_FACTOR_SW_SRC_HSD_FACTOR_SHIFT)       /* 0x0000FFFF */
#define RGA2_SRC_X_FACTOR_SW_SRC_HSP_FACTOR_SHIFT          (16U)
#define RGA2_SRC_X_FACTOR_SW_SRC_HSP_FACTOR_MASK           (0xFFFFU << RGA2_SRC_X_FACTOR_SW_SRC_HSP_FACTOR_SHIFT)       /* 0xFFFF0000 */
/* OSD_CTRL0 */
#define RGA2_OSD_CTRL0_OFFSET                              (0x120U)
#define RGA2_OSD_CTRL0_SW_OSD_MODE_SHIFT                   (0U)
#define RGA2_OSD_CTRL0_SW_OSD_MODE_MASK                    (0x3U << RGA2_OSD_CTRL0_SW_OSD_MODE_SHIFT)                   /* 0x00000003 */
#define RGA2_OSD_CTRL0_SW_OSD_VER_MODE_SHIFT               (2U)
#define RGA2_OSD_CTRL0_SW_OSD_VER_MODE_MASK                (0x1U << RGA2_OSD_CTRL0_SW_OSD_VER_MODE_SHIFT)               /* 0x00000004 */
#define RGA2_OSD_CTRL0_SW_OSD_WIDTH_MODE_SHIFT             (3U)
#define RGA2_OSD_CTRL0_SW_OSD_WIDTH_MODE_MASK              (0x1U << RGA2_OSD_CTRL0_SW_OSD_WIDTH_MODE_SHIFT)             /* 0x00000008 */
#define RGA2_OSD_CTRL0_SW_OSD_BLK_NUM_SHIFT                (4U)
#define RGA2_OSD_CTRL0_SW_OSD_BLK_NUM_MASK                 (0x3FU << RGA2_OSD_CTRL0_SW_OSD_BLK_NUM_SHIFT)               /* 0x000003F0 */
#define RGA2_OSD_CTRL0_SW_OSD_FLAGS_INDEX_SHIFT            (10U)
#define RGA2_OSD_CTRL0_SW_OSD_FLAGS_INDEX_MASK             (0x3FFU << RGA2_OSD_CTRL0_SW_OSD_FLAGS_INDEX_SHIFT)          /* 0x000FFC00 */
#define RGA2_OSD_CTRL0_SW_OSD_FIX_WIDTH_SHIFT              (20U)
#define RGA2_OSD_CTRL0_SW_OSD_FIX_WIDTH_MASK               (0x3FFU << RGA2_OSD_CTRL0_SW_OSD_FIX_WIDTH_SHIFT)            /* 0x3FF00000 */
#define RGA2_OSD_CTRL0_SW_OSD_2BPP_MODE_SHIFT              (30U)
#define RGA2_OSD_CTRL0_SW_OSD_2BPP_MODE_MASK               (0x1U << RGA2_OSD_CTRL0_SW_OSD_2BPP_MODE_SHIFT)              /* 0x40000000 */
/* SRC_Y_FACTOR */
#define RGA2_SRC_Y_FACTOR_OFFSET                           (0x124U)
#define RGA2_SRC_Y_FACTOR_SW_SRC_VSD_FACTOR_SHIFT          (0U)
#define RGA2_SRC_Y_FACTOR_SW_SRC_VSD_FACTOR_MASK           (0xFFFFU << RGA2_SRC_Y_FACTOR_SW_SRC_VSD_FACTOR_SHIFT)       /* 0x0000FFFF */
#define RGA2_SRC_Y_FACTOR_SW_SRC_VSP_FACTOR_SHIFT          (16U)
#define RGA2_SRC_Y_FACTOR_SW_SRC_VSP_FACTOR_MASK           (0xFFFFU << RGA2_SRC_Y_FACTOR_SW_SRC_VSP_FACTOR_SHIFT)       /* 0xFFFF0000 */
/* OSD_CTRL1 */
#define RGA2_OSD_CTRL1_OFFSET                              (0x124U)
#define RGA2_OSD_CTRL1_SW_OSD_COLOR_SEL_SHIFT              (0U)
#define RGA2_OSD_CTRL1_SW_OSD_COLOR_SEL_MASK               (0x1U << RGA2_OSD_CTRL1_SW_OSD_COLOR_SEL_SHIFT)              /* 0x00000001 */
#define RGA2_OSD_CTRL1_SW_OSD_FLAG_SEL_SHIFT               (1U)
#define RGA2_OSD_CTRL1_SW_OSD_FLAG_SEL_MASK                (0x1U << RGA2_OSD_CTRL1_SW_OSD_FLAG_SEL_SHIFT)               /* 0x00000002 */
#define RGA2_OSD_CTRL1_SW_OSD_DEFAULT_COLOR_SHIFT          (2U)
#define RGA2_OSD_CTRL1_SW_OSD_DEFAULT_COLOR_MASK           (0x1U << RGA2_OSD_CTRL1_SW_OSD_DEFAULT_COLOR_SHIFT)          /* 0x00000004 */
#define RGA2_OSD_CTRL1_SW_OSD_AUTO_INVERT_MODE_SHIFT       (3U)
#define RGA2_OSD_CTRL1_SW_OSD_AUTO_INVERT_MODE_MASK        (0x1U << RGA2_OSD_CTRL1_SW_OSD_AUTO_INVERT_MODE_SHIFT)       /* 0x00000008 */
#define RGA2_OSD_CTRL1_SW_OSD_THRESH_SHIFT                 (4U)
#define RGA2_OSD_CTRL1_SW_OSD_THRESH_MASK                  (0xFFU << RGA2_OSD_CTRL1_SW_OSD_THRESH_SHIFT)                /* 0x00000FF0 */
#define RGA2_OSD_CTRL1_SW_OSD_INVERT_A_EN_SHIFT            (12U)
#define RGA2_OSD_CTRL1_SW_OSD_INVERT_A_EN_MASK             (0x1U << RGA2_OSD_CTRL1_SW_OSD_INVERT_A_EN_SHIFT)            /* 0x00001000 */
#define RGA2_OSD_CTRL1_SW_OSD_INVERT_Y_DIS_SHIFT           (13U)
#define RGA2_OSD_CTRL1_SW_OSD_INVERT_Y_DIS_MASK            (0x1U << RGA2_OSD_CTRL1_SW_OSD_INVERT_Y_DIS_SHIFT)           /* 0x00002000 */
#define RGA2_OSD_CTRL1_SW_OSD_INVERT_C_DIS_SHIFT           (14U)
#define RGA2_OSD_CTRL1_SW_OSD_INVERT_C_DIS_MASK            (0x1U << RGA2_OSD_CTRL1_SW_OSD_INVERT_C_DIS_SHIFT)           /* 0x00004000 */
#define RGA2_OSD_CTRL1_SW_OSD_UNFIX_INDEX_SHIFT            (16U)
#define RGA2_OSD_CTRL1_SW_OSD_UNFIX_INDEX_MASK             (0xFU << RGA2_OSD_CTRL1_SW_OSD_UNFIX_INDEX_SHIFT)            /* 0x000F0000 */
/* SRC_BG_COLOR */
#define RGA2_SRC_BG_COLOR_OFFSET                           (0x128U)
#define RGA2_SRC_BG_COLOR_SW_SRC_BG_COLOR_SHIFT            (0U)
#define RGA2_SRC_BG_COLOR_SW_SRC_BG_COLOR_MASK             (0xFFFFFFFFU << RGA2_SRC_BG_COLOR_SW_SRC_BG_COLOR_SHIFT)     /* 0xFFFFFFFF */
/* OSD_COLOR0 */
#define RGA2_OSD_COLOR0_OFFSET                             (0x128U)
#define RGA2_OSD_COLOR0_SW_OSD_COLOR0_SHIFT                (0U)
#define RGA2_OSD_COLOR0_SW_OSD_COLOR0_MASK                 (0xFFFFFFU << RGA2_OSD_COLOR0_SW_OSD_COLOR0_SHIFT)           /* 0x00FFFFFF */
#define RGA2_OSD_COLOR0_SW_OSD2BPP_ALPHA0_SHIFT            (24U)
#define RGA2_OSD_COLOR0_SW_OSD2BPP_ALPHA0_MASK             (0xFFU << RGA2_OSD_COLOR0_SW_OSD2BPP_ALPHA0_SHIFT)           /* 0xFF000000 */
/* SRC_FG_COLOR */
#define RGA2_SRC_FG_COLOR_OFFSET                           (0x12CU)
#define RGA2_SRC_FG_COLOR_SW_SRC_FG_COLOR_SHIFT            (0U)
#define RGA2_SRC_FG_COLOR_SW_SRC_FG_COLOR_MASK             (0xFFFFFFFFU << RGA2_SRC_FG_COLOR_SW_SRC_FG_COLOR_SHIFT)     /* 0xFFFFFFFF */
/* OSD_COLOR1 */
#define RGA2_OSD_COLOR1_OFFSET                             (0x12CU)
#define RGA2_OSD_COLOR1_SW_OSD_COLOR1_SHIFT                (0U)
#define RGA2_OSD_COLOR1_SW_OSD_COLOR1_MASK                 (0xFFFFFFU << RGA2_OSD_COLOR1_SW_OSD_COLOR1_SHIFT)           /* 0x00FFFFFF */
#define RGA2_OSD_COLOR1_SW_OSD2BPP_ALPHA1_SHIFT            (24U)
#define RGA2_OSD_COLOR1_SW_OSD2BPP_ALPHA1_MASK             (0xFFU << RGA2_OSD_COLOR1_SW_OSD2BPP_ALPHA1_SHIFT)           /* 0xFF000000 */
/* MOSAIC_MODE */
#define RGA2_MOSAIC_MODE_OFFSET                            (0x130U)
#define RGA2_MOSAIC_MODE_SW_MOSAIC_MODE_SHIFT              (0U)
#define RGA2_MOSAIC_MODE_SW_MOSAIC_MODE_MASK               (0x7U << RGA2_MOSAIC_MODE_SW_MOSAIC_MODE_SHIFT)              /* 0x00000007 */
/* SRC_TR_COLOR0 */
#define RGA2_SRC_TR_COLOR0_OFFSET                          (0x130U)
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_RMIN_SHIFT         (0U)
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_RMIN_MASK          (0xFFU << RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_RMIN_SHIFT)        /* 0x000000FF */
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_GMIN_SHIFT         (8U)
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_GMIN_MASK          (0xFFU << RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_GMIN_SHIFT)        /* 0x0000FF00 */
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_BMIN_SHIFT         (16U)
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_BMIN_MASK          (0xFFU << RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_BMIN_SHIFT)        /* 0x00FF0000 */
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_AMIN_SHIFT         (24U)
#define RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_AMIN_MASK          (0xFFU << RGA2_SRC_TR_COLOR0_SW_SRC_TRANS_AMIN_SHIFT)        /* 0xFF000000 */
/* CP_GR_A */
#define RGA2_CP_GR_A_OFFSET                                (0x130U)
#define RGA2_CP_GR_A_SW_GRADIENT_X_A_SHIFT                 (0U)
#define RGA2_CP_GR_A_SW_GRADIENT_X_A_MASK                  (0xFFFFU << RGA2_CP_GR_A_SW_GRADIENT_X_A_SHIFT)              /* 0x0000FFFF */
#define RGA2_CP_GR_A_SW_GRADIENT_Y_A_SHIFT                 (16U)
#define RGA2_CP_GR_A_SW_GRADIENT_Y_A_MASK                  (0xFFFFU << RGA2_CP_GR_A_SW_GRADIENT_Y_A_SHIFT)              /* 0xFFFF0000 */
/* OSD_LAST_FLAGS0 */
#define RGA2_OSD_LAST_FLAGS0_OFFSET                        (0x130U)
#define RGA2_OSD_LAST_FLAGS0_SW_OSD_LAST_FLAGS0_SHIFT      (0U)
#define RGA2_OSD_LAST_FLAGS0_SW_OSD_LAST_FLAGS0_MASK       (0xFFFFFFFFU << RGA2_OSD_LAST_FLAGS0_SW_OSD_LAST_FLAGS0_SHIFT) /* 0xFFFFFFFF */
/* SRC_TR_COLOR1 */
#define RGA2_SRC_TR_COLOR1_OFFSET                          (0x134U)
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_RMAX_SHIFT         (0U)
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_RMAX_MASK          (0xFFU << RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_RMAX_SHIFT)        /* 0x000000FF */
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_GMAX_SHIFT         (8U)
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_GMAX_MASK          (0xFFU << RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_GMAX_SHIFT)        /* 0x0000FF00 */
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_BMAX_SHIFT         (16U)
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_BMAX_MASK          (0xFFU << RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_BMAX_SHIFT)        /* 0x00FF0000 */
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_AMAX_SHIFT         (24U)
#define RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_AMAX_MASK          (0xFFU << RGA2_SRC_TR_COLOR1_SW_SRC_TRANS_AMAX_SHIFT)        /* 0xFF000000 */
/* CP_GR_B */
#define RGA2_CP_GR_B_OFFSET                                (0x134U)
#define RGA2_CP_GR_B_SW_GRADIENT_X_B_SHIFT                 (0U)
#define RGA2_CP_GR_B_SW_GRADIENT_X_B_MASK                  (0xFFFFU << RGA2_CP_GR_B_SW_GRADIENT_X_B_SHIFT)              /* 0x0000FFFF */
#define RGA2_CP_GR_B_SW_GRADIENT_Y_B_SHIFT                 (16U)
#define RGA2_CP_GR_B_SW_GRADIENT_Y_B_MASK                  (0xFFFFU << RGA2_CP_GR_B_SW_GRADIENT_Y_B_SHIFT)              /* 0xFFFF0000 */
/* OSD_LAST_FLAGS1 */
#define RGA2_OSD_LAST_FLAGS1_OFFSET                        (0x134U)
#define RGA2_OSD_LAST_FLAGS1_SW_OSD_LAST_FLAGS1_SHIFT      (0U)
#define RGA2_OSD_LAST_FLAGS1_SW_OSD_LAST_FLAGS1_MASK       (0xFFFFFFFFU << RGA2_OSD_LAST_FLAGS1_SW_OSD_LAST_FLAGS1_SHIFT) /* 0xFFFFFFFF */
/* DST_INFO */
#define RGA2_DST_INFO_OFFSET                               (0x138U)
#define RGA2_DST_INFO_SW_DST_FMT_SHIFT                     (0U)
#define RGA2_DST_INFO_SW_DST_FMT_MASK                      (0xFU << RGA2_DST_INFO_SW_DST_FMT_SHIFT)                     /* 0x0000000F */
#define RGA2_DST_INFO_SW_DST_RBSWAP_SHIFT                  (4U)
#define RGA2_DST_INFO_SW_DST_RBSWAP_MASK                   (0x1U << RGA2_DST_INFO_SW_DST_RBSWAP_SHIFT)                  /* 0x00000010 */
#define RGA2_DST_INFO_SW_DST_ALPHA_SWAP_SHIFT              (5U)
#define RGA2_DST_INFO_SW_DST_ALPHA_SWAP_MASK               (0x1U << RGA2_DST_INFO_SW_DST_ALPHA_SWAP_SHIFT)              /* 0x00000020 */
#define RGA2_DST_INFO_SW_DST_UVSWAP_SHIFT                  (6U)
#define RGA2_DST_INFO_SW_DST_UVSWAP_MASK                   (0x1U << RGA2_DST_INFO_SW_DST_UVSWAP_SHIFT)                  /* 0x00000040 */
#define RGA2_DST_INFO_SW_SRC1_FMT_SHIFT                    (7U)
#define RGA2_DST_INFO_SW_SRC1_FMT_MASK                     (0x7U << RGA2_DST_INFO_SW_SRC1_FMT_SHIFT)                    /* 0x00000380 */
#define RGA2_DST_INFO_SW_SRC1_RBSWAP_SHIFT                 (10U)
#define RGA2_DST_INFO_SW_SRC1_RBSWAP_MASK                  (0x1U << RGA2_DST_INFO_SW_SRC1_RBSWAP_SHIFT)                 /* 0x00000400 */
#define RGA2_DST_INFO_SW_SRC1_ALPHA_SWAP_SHIFT             (11U)
#define RGA2_DST_INFO_SW_SRC1_ALPHA_SWAP_MASK              (0x1U << RGA2_DST_INFO_SW_SRC1_ALPHA_SWAP_SHIFT)             /* 0x00000800 */
#define RGA2_DST_INFO_SW_SRC1_DITHER_UP_SHIFT              (12U)
#define RGA2_DST_INFO_SW_SRC1_DITHER_UP_MASK               (0x1U << RGA2_DST_INFO_SW_SRC1_DITHER_UP_SHIFT)              /* 0x00001000 */
#define RGA2_DST_INFO_SW_DITHER_DOWN_SHIFT                 (13U)
#define RGA2_DST_INFO_SW_DITHER_DOWN_MASK                  (0x1U << RGA2_DST_INFO_SW_DITHER_DOWN_SHIFT)                 /* 0x00002000 */
#define RGA2_DST_INFO_SW_DITHER_MODE_SHIFT                 (14U)
#define RGA2_DST_INFO_SW_DITHER_MODE_MASK                  (0x3U << RGA2_DST_INFO_SW_DITHER_MODE_SHIFT)                 /* 0x0000C000 */
#define RGA2_DST_INFO_SW_DST_CSC_MODE_01_SHIFT             (16U)
#define RGA2_DST_INFO_SW_DST_CSC_MODE_01_MASK              (0x3U << RGA2_DST_INFO_SW_DST_CSC_MODE_01_SHIFT)             /* 0x00030000 */
#define RGA2_DST_INFO_SW_DST_CSC_CLIP_SHIFT                (18U)
#define RGA2_DST_INFO_SW_DST_CSC_CLIP_MASK                 (0x1U << RGA2_DST_INFO_SW_DST_CSC_CLIP_SHIFT)                /* 0x00040000 */
#define RGA2_DST_INFO_SW_DST_CSC_MODE_2_SHIFT              (19U)
#define RGA2_DST_INFO_SW_DST_CSC_MODE_2_MASK               (0x1U << RGA2_DST_INFO_SW_DST_CSC_MODE_2_SHIFT)              /* 0x00080000 */
#define RGA2_DST_INFO_SW_SRC1_CSC_MODE_SHIFT               (20U)
#define RGA2_DST_INFO_SW_SRC1_CSC_MODE_MASK                (0x3U << RGA2_DST_INFO_SW_SRC1_CSC_MODE_SHIFT)               /* 0x00300000 */
#define RGA2_DST_INFO_SW_SRC1_CSC_CLIP_SHIFT               (22U)
#define RGA2_DST_INFO_SW_SRC1_CSC_CLIP_MASK                (0x1U << RGA2_DST_INFO_SW_SRC1_CSC_CLIP_SHIFT)               /* 0x00400000 */
#define RGA2_DST_INFO_SW_DST_UVHDS_MODE_SHIFT              (23U)
#define RGA2_DST_INFO_SW_DST_UVHDS_MODE_MASK               (0x1U << RGA2_DST_INFO_SW_DST_UVHDS_MODE_SHIFT)              /* 0x00800000 */
#define RGA2_DST_INFO_SW_DST_FMT_YUV400_EN_SHIFT           (24U)
#define RGA2_DST_INFO_SW_DST_FMT_YUV400_EN_MASK            (0x1U << RGA2_DST_INFO_SW_DST_FMT_YUV400_EN_SHIFT)           /* 0x01000000 */
#define RGA2_DST_INFO_SW_DST_FMT_Y4_EN_SHIFT               (25U)
#define RGA2_DST_INFO_SW_DST_FMT_Y4_EN_MASK                (0x1U << RGA2_DST_INFO_SW_DST_FMT_Y4_EN_SHIFT)               /* 0x02000000 */
#define RGA2_DST_INFO_SW_DST_NN_QUANTIZE_EN_SHIFT          (26U)
#define RGA2_DST_INFO_SW_DST_NN_QUANTIZE_EN_MASK           (0x1U << RGA2_DST_INFO_SW_DST_NN_QUANTIZE_EN_SHIFT)          /* 0x04000000 */
#define RGA2_DST_INFO_SW_DST_UVVDS_MODE_SHIFT              (27U)
#define RGA2_DST_INFO_SW_DST_UVVDS_MODE_MASK               (0x1U << RGA2_DST_INFO_SW_DST_UVVDS_MODE_SHIFT)              /* 0x08000000 */
#define RGA2_DST_INFO_SW_SRC1_A1555_ACONFIG_EN_SHIFT       (28U)
#define RGA2_DST_INFO_SW_SRC1_A1555_ACONFIG_EN_MASK        (0x1U << RGA2_DST_INFO_SW_SRC1_A1555_ACONFIG_EN_SHIFT)       /* 0x10000000 */
/* DST_BASE0 */
#define RGA2_DST_BASE0_OFFSET                              (0x13CU)
#define RGA2_DST_BASE0_SW_DST_BASE0_SHIFT                  (0U)
#define RGA2_DST_BASE0_SW_DST_BASE0_MASK                   (0xFFFFFFFFU << RGA2_DST_BASE0_SW_DST_BASE0_SHIFT)           /* 0xFFFFFFFF */
/* DST_BASE1 */
#define RGA2_DST_BASE1_OFFSET                              (0x140U)
#define RGA2_DST_BASE1_SW_DST_BASE1_SHIFT                  (0U)
#define RGA2_DST_BASE1_SW_DST_BASE1_MASK                   (0xFFFFFFFFU << RGA2_DST_BASE1_SW_DST_BASE1_SHIFT)           /* 0xFFFFFFFF */
/* DST_BASE2 */
#define RGA2_DST_BASE2_OFFSET                              (0x144U)
#define RGA2_DST_BASE2_SW_DST_BASE2_SHIFT                  (0U)
#define RGA2_DST_BASE2_SW_DST_BASE2_MASK                   (0xFFFFFFFFU << RGA2_DST_BASE2_SW_DST_BASE2_SHIFT)           /* 0xFFFFFFFF */
/* DST_VIR_INFO */
#define RGA2_DST_VIR_INFO_OFFSET                           (0x148U)
#define RGA2_DST_VIR_INFO_SW_DST_VIR_STRIDE_SHIFT          (0U)
#define RGA2_DST_VIR_INFO_SW_DST_VIR_STRIDE_MASK           (0xFFFFU << RGA2_DST_VIR_INFO_SW_DST_VIR_STRIDE_SHIFT)       /* 0x0000FFFF */
#define RGA2_DST_VIR_INFO_SW_SRC1_VIR_STRIDE_SHIFT         (16U)
#define RGA2_DST_VIR_INFO_SW_SRC1_VIR_STRIDE_MASK          (0x7FFFU << RGA2_DST_VIR_INFO_SW_SRC1_VIR_STRIDE_SHIFT)      /* 0x7FFF0000 */
/* DST_ACT_INFO */
#define RGA2_DST_ACT_INFO_OFFSET                           (0x14CU)
#define RGA2_DST_ACT_INFO_SW_DST_ACT_WIDTH_SHIFT           (0U)
#define RGA2_DST_ACT_INFO_SW_DST_ACT_WIDTH_MASK            (0xFFFU << RGA2_DST_ACT_INFO_SW_DST_ACT_WIDTH_SHIFT)         /* 0x00000FFF */
#define RGA2_DST_ACT_INFO_SW_TILE4X4_OUT_XOFF_SHIFT        (14U)
#define RGA2_DST_ACT_INFO_SW_TILE4X4_OUT_XOFF_MASK         (0x3U << RGA2_DST_ACT_INFO_SW_TILE4X4_OUT_XOFF_SHIFT)        /* 0x0000C000 */
#define RGA2_DST_ACT_INFO_SW_DST_ACT_HEIGHT_SHIFT          (16U)
#define RGA2_DST_ACT_INFO_SW_DST_ACT_HEIGHT_MASK           (0xFFFU << RGA2_DST_ACT_INFO_SW_DST_ACT_HEIGHT_SHIFT)        /* 0x0FFF0000 */
#define RGA2_DST_ACT_INFO_SW_TILE4X4_OUT_YOFF_SHIFT        (30U)
#define RGA2_DST_ACT_INFO_SW_TILE4X4_OUT_YOFF_MASK         (0x3U << RGA2_DST_ACT_INFO_SW_TILE4X4_OUT_YOFF_SHIFT)        /* 0xC0000000 */
/* ALPHA_CTRL0 */
#define RGA2_ALPHA_CTRL0_OFFSET                            (0x150U)
#define RGA2_ALPHA_CTRL0_SW_ALPHA_ROP_E_SHIFT              (0U)
#define RGA2_ALPHA_CTRL0_SW_ALPHA_ROP_E_MASK               (0x1U << RGA2_ALPHA_CTRL0_SW_ALPHA_ROP_E_SHIFT)              /* 0x00000001 */
#define RGA2_ALPHA_CTRL0_SW_ALPHA_ROP_SEL_SHIFT            (1U)
#define RGA2_ALPHA_CTRL0_SW_ALPHA_ROP_SEL_MASK             (0x1U << RGA2_ALPHA_CTRL0_SW_ALPHA_ROP_SEL_SHIFT)            /* 0x00000002 */
#define RGA2_ALPHA_CTRL0_SW_ROP_MODE_SHIFT                 (2U)
#define RGA2_ALPHA_CTRL0_SW_ROP_MODE_MASK                  (0x3U << RGA2_ALPHA_CTRL0_SW_ROP_MODE_SHIFT)                 /* 0x0000000C */
#define RGA2_ALPHA_CTRL0_SW_SRC_GLOBAL_ALPHA_SHIFT         (4U)
#define RGA2_ALPHA_CTRL0_SW_SRC_GLOBAL_ALPHA_MASK          (0xFFU << RGA2_ALPHA_CTRL0_SW_SRC_GLOBAL_ALPHA_SHIFT)        /* 0x00000FF0 */
#define RGA2_ALPHA_CTRL0_SW_DST_GLOBAL_ALPHA_SHIFT         (12U)
#define RGA2_ALPHA_CTRL0_SW_DST_GLOBAL_ALPHA_MASK          (0xFFU << RGA2_ALPHA_CTRL0_SW_DST_GLOBAL_ALPHA_SHIFT)        /* 0x000FF000 */
#define RGA2_ALPHA_CTRL0_SW_MASK_ENDIAN_SHIFT              (20U)
#define RGA2_ALPHA_CTRL0_SW_MASK_ENDIAN_MASK               (0x1U << RGA2_ALPHA_CTRL0_SW_MASK_ENDIAN_SHIFT)              /* 0x00100000 */
/* ALPHA_CTRL1 */
#define RGA2_ALPHA_CTRL1_OFFSET                            (0x154U)
#define RGA2_ALPHA_CTRL1_SW_DST_COLOR_M0_SHIFT             (0U)
#define RGA2_ALPHA_CTRL1_SW_DST_COLOR_M0_MASK              (0x1U << RGA2_ALPHA_CTRL1_SW_DST_COLOR_M0_SHIFT)             /* 0x00000001 */
#define RGA2_ALPHA_CTRL1_SW_SRC_COLOR_M0_SHIFT             (1U)
#define RGA2_ALPHA_CTRL1_SW_SRC_COLOR_M0_MASK              (0x1U << RGA2_ALPHA_CTRL1_SW_SRC_COLOR_M0_SHIFT)             /* 0x00000002 */
#define RGA2_ALPHA_CTRL1_SW_DST_FACTOR_M0_SHIFT            (2U)
#define RGA2_ALPHA_CTRL1_SW_DST_FACTOR_M0_MASK             (0x7U << RGA2_ALPHA_CTRL1_SW_DST_FACTOR_M0_SHIFT)            /* 0x0000001C */
#define RGA2_ALPHA_CTRL1_SW_SRC_FACTOR_M0_SHIFT            (5U)
#define RGA2_ALPHA_CTRL1_SW_SRC_FACTOR_M0_MASK             (0x7U << RGA2_ALPHA_CTRL1_SW_SRC_FACTOR_M0_SHIFT)            /* 0x000000E0 */
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_CAL_M0_SHIFT         (8U)
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_CAL_M0_MASK          (0x1U << RGA2_ALPHA_CTRL1_SW_DST_ALPHA_CAL_M0_SHIFT)         /* 0x00000100 */
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_CAL_M0_SHIFT         (9U)
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_CAL_M0_MASK          (0x1U << RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_CAL_M0_SHIFT)         /* 0x00000200 */
#define RGA2_ALPHA_CTRL1_SW_DST_BLEND_M0_SHIFT             (10U)
#define RGA2_ALPHA_CTRL1_SW_DST_BLEND_M0_MASK              (0x3U << RGA2_ALPHA_CTRL1_SW_DST_BLEND_M0_SHIFT)             /* 0x00000C00 */
#define RGA2_ALPHA_CTRL1_SW_SRC_BLEND_M0_SHIFT             (12U)
#define RGA2_ALPHA_CTRL1_SW_SRC_BLEND_M0_MASK              (0x3U << RGA2_ALPHA_CTRL1_SW_SRC_BLEND_M0_SHIFT)             /* 0x00003000 */
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_M0_SHIFT             (14U)
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_M0_MASK              (0x1U << RGA2_ALPHA_CTRL1_SW_DST_ALPHA_M0_SHIFT)             /* 0x00004000 */
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_M0_SHIFT             (15U)
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_M0_MASK              (0x1U << RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_M0_SHIFT)             /* 0x00008000 */
#define RGA2_ALPHA_CTRL1_SW_DST_FACTOR_M1_SHIFT            (16U)
#define RGA2_ALPHA_CTRL1_SW_DST_FACTOR_M1_MASK             (0x7U << RGA2_ALPHA_CTRL1_SW_DST_FACTOR_M1_SHIFT)            /* 0x00070000 */
#define RGA2_ALPHA_CTRL1_W_SRC_FACTOR_M1_SHIFT             (19U)
#define RGA2_ALPHA_CTRL1_W_SRC_FACTOR_M1_MASK              (0x7U << RGA2_ALPHA_CTRL1_W_SRC_FACTOR_M1_SHIFT)             /* 0x00380000 */
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_CAL_M1_SHIFT         (22U)
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_CAL_M1_MASK          (0x1U << RGA2_ALPHA_CTRL1_SW_DST_ALPHA_CAL_M1_SHIFT)         /* 0x00400000 */
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_CAL_M1_SHIFT         (23U)
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_CAL_M1_MASK          (0x1U << RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_CAL_M1_SHIFT)         /* 0x00800000 */
#define RGA2_ALPHA_CTRL1_SW_DST_BLEND_M1_SHIFT             (24U)
#define RGA2_ALPHA_CTRL1_SW_DST_BLEND_M1_MASK              (0x3U << RGA2_ALPHA_CTRL1_SW_DST_BLEND_M1_SHIFT)             /* 0x03000000 */
#define RGA2_ALPHA_CTRL1_SW_SRC_BLEND_M1_SHIFT             (26U)
#define RGA2_ALPHA_CTRL1_SW_SRC_BLEND_M1_MASK              (0x3U << RGA2_ALPHA_CTRL1_SW_SRC_BLEND_M1_SHIFT)             /* 0x0C000000 */
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_M1_SHIFT             (28U)
#define RGA2_ALPHA_CTRL1_SW_DST_ALPHA_M1_MASK              (0x1U << RGA2_ALPHA_CTRL1_SW_DST_ALPHA_M1_SHIFT)             /* 0x10000000 */
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_M1_SHIFT             (29U)
#define RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_M1_MASK              (0x1U << RGA2_ALPHA_CTRL1_SW_SRC_ALPHA_M1_SHIFT)             /* 0x20000000 */
#define RGA2_ALPHA_CTRL1_RESERVED_SHIFT                    (30U)
#define RGA2_ALPHA_CTRL1_RESERVED_MASK                     (0x3U << RGA2_ALPHA_CTRL1_RESERVED_SHIFT)                    /* 0xC0000000 */
/* FADING_CTRL */
#define RGA2_FADING_CTRL_OFFSET                            (0x158U)
#define RGA2_FADING_CTRL_SW_FADING_OFFSET_R_SHIFT          (0U)
#define RGA2_FADING_CTRL_SW_FADING_OFFSET_R_MASK           (0xFFU << RGA2_FADING_CTRL_SW_FADING_OFFSET_R_SHIFT)         /* 0x000000FF */
#define RGA2_FADING_CTRL_SW_FADING_OFFSET_G_SHIFT          (8U)
#define RGA2_FADING_CTRL_SW_FADING_OFFSET_G_MASK           (0xFFU << RGA2_FADING_CTRL_SW_FADING_OFFSET_G_SHIFT)         /* 0x0000FF00 */
#define RGA2_FADING_CTRL_SW_FADING_OFFSET_B_SHIFT          (16U)
#define RGA2_FADING_CTRL_SW_FADING_OFFSET_B_MASK           (0xFFU << RGA2_FADING_CTRL_SW_FADING_OFFSET_B_SHIFT)         /* 0x00FF0000 */
#define RGA2_FADING_CTRL_SW_FADING_EN_SHIFT                (24U)
#define RGA2_FADING_CTRL_SW_FADING_EN_MASK                 (0x1U << RGA2_FADING_CTRL_SW_FADING_EN_SHIFT)                /* 0x01000000 */
/* PAT_CON */
#define RGA2_PAT_CON_OFFSET                                (0x15CU)
#define RGA2_PAT_CON_SW_PAT_WIDTH_SHIFT                    (0U)
#define RGA2_PAT_CON_SW_PAT_WIDTH_MASK                     (0xFFU << RGA2_PAT_CON_SW_PAT_WIDTH_SHIFT)                   /* 0x000000FF */
#define RGA2_PAT_CON_SW_PAT_HEIGHT_SHIFT                   (8U)
#define RGA2_PAT_CON_SW_PAT_HEIGHT_MASK                    (0xFFU << RGA2_PAT_CON_SW_PAT_HEIGHT_SHIFT)                  /* 0x0000FF00 */
#define RGA2_PAT_CON_SW_PAT_OFFSET_X_SHIFT                 (16U)
#define RGA2_PAT_CON_SW_PAT_OFFSET_X_MASK                  (0xFFU << RGA2_PAT_CON_SW_PAT_OFFSET_X_SHIFT)                /* 0x00FF0000 */
#define RGA2_PAT_CON_SW_PAT_OFFSET_Y_SHIFT                 (24U)
#define RGA2_PAT_CON_SW_PAT_OFFSET_Y_MASK                  (0xFFU << RGA2_PAT_CON_SW_PAT_OFFSET_Y_SHIFT)                /* 0xFF000000 */
/* NN_QUANTIZE_SCALE */
#define RGA2_NN_QUANTIZE_SCALE_OFFSET                      (0x160U)
#define RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_R_SHIFT (0U)
#define RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_R_MASK (0x3FFU << RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_R_SHIFT) /* 0x000003FF */
#define RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_G_SHIFT (10U)
#define RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_G_MASK (0x3FFU << RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_G_SHIFT) /* 0x000FFC00 */
#define RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_B_SHIFT (20U)
#define RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_B_MASK (0x3FFU << RGA2_NN_QUANTIZE_SCALE_SW_NN_QUANTIZE_SCALE_B_SHIFT) /* 0x3FF00000 */
/* ROP_CON0 */
#define RGA2_ROP_CON0_OFFSET                               (0x160U)
#define RGA2_ROP_CON0_SW_ROP3_CODE0_SHIFT                  (0U)
#define RGA2_ROP_CON0_SW_ROP3_CODE0_MASK                   (0x1FFFFFFU << RGA2_ROP_CON0_SW_ROP3_CODE0_SHIFT)            /* 0x01FFFFFF */
/* CP_GR_G */
#define RGA2_CP_GR_G_OFFSET                                (0x160U)
#define RGA2_CP_GR_G_SW_GRADIENT_X_G_SHIFT                 (0U)
#define RGA2_CP_GR_G_SW_GRADIENT_X_G_MASK                  (0xFFFFU << RGA2_CP_GR_G_SW_GRADIENT_X_G_SHIFT)              /* 0x0000FFFF */
#define RGA2_CP_GR_G_SW_GRADIENT_Y_G_SHIFT                 (16U)
#define RGA2_CP_GR_G_SW_GRADIENT_Y_G_MASK                  (0xFFFFU << RGA2_CP_GR_G_SW_GRADIENT_Y_G_SHIFT)              /* 0xFFFF0000 */
/* DST_Y4MAP_LUT0 */
#define RGA2_DST_Y4MAP_LUT0_OFFSET                         (0x160U)
#define RGA2_DST_Y4MAP_LUT0_SW_DST_Y4MAP_LUT0_SHIFT        (0U)
#define RGA2_DST_Y4MAP_LUT0_SW_DST_Y4MAP_LUT0_MASK         (0xFFFFFFFFU << RGA2_DST_Y4MAP_LUT0_SW_DST_Y4MAP_LUT0_SHIFT) /* 0xFFFFFFFF */
/* OSD_INVERTSION_CAL0 */
#define RGA2_OSD_INVERTSION_CAL0_OFFSET                    (0x160U)
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MINY_SHIFT         (0U)
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MINY_MASK          (0xFFU << RGA2_OSD_INVERTSION_CAL0_SW_OSD_MINY_SHIFT)        /* 0x000000FF */
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MAXY_SHIFT         (8U)
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MAXY_MASK          (0xFFU << RGA2_OSD_INVERTSION_CAL0_SW_OSD_MAXY_SHIFT)        /* 0x0000FF00 */
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MINC_SHIFT         (16U)
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MINC_MASK          (0xFFU << RGA2_OSD_INVERTSION_CAL0_SW_OSD_MINC_SHIFT)        /* 0x00FF0000 */
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MAXC_SHIFT         (24U)
#define RGA2_OSD_INVERTSION_CAL0_SW_OSD_MAXC_MASK          (0xFFU << RGA2_OSD_INVERTSION_CAL0_SW_OSD_MAXC_SHIFT)        /* 0xFF000000 */
/* NN_QUANTIZE_OFFSET */
#define RGA2_NN_QUANTIZE_OFFSET_OFFSET                     (0x164U)
#define RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_R_SHIFT (0U)
#define RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_R_MASK (0x1FFU << RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_R_SHIFT) /* 0x000001FF */
#define RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_G_SHIFT (10U)
#define RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_G_MASK (0x1FFU << RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_G_SHIFT) /* 0x0007FC00 */
#define RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_B_SHIFT (20U)
#define RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_B_MASK (0x1FFU << RGA2_NN_QUANTIZE_OFFSET_SW_NN_QUANTIZE_OFFSET_B_SHIFT) /* 0x1FF00000 */
/* CP_GR_R */
#define RGA2_CP_GR_R_OFFSET                                (0x164U)
#define RGA2_CP_GR_R_SW_GRADIENT_X_R_SHIFT                 (0U)
#define RGA2_CP_GR_R_SW_GRADIENT_X_R_MASK                  (0xFFFFU << RGA2_CP_GR_R_SW_GRADIENT_X_R_SHIFT)              /* 0x0000FFFF */
#define RGA2_CP_GR_R_SW_GRADIENT_Y_R_SHIFT                 (16U)
#define RGA2_CP_GR_R_SW_GRADIENT_Y_R_MASK                  (0xFFFFU << RGA2_CP_GR_R_SW_GRADIENT_Y_R_SHIFT)              /* 0xFFFF0000 */
/* DST_Y4MAP_LUT1 */
#define RGA2_DST_Y4MAP_LUT1_OFFSET                         (0x164U)
#define RGA2_DST_Y4MAP_LUT1_SW_DST_Y4MAP_LUT1_SHIFT        (0U)
#define RGA2_DST_Y4MAP_LUT1_SW_DST_Y4MAP_LUT1_MASK         (0xFFFFFFFFU << RGA2_DST_Y4MAP_LUT1_SW_DST_Y4MAP_LUT1_SHIFT) /* 0xFFFFFFFF */
/* ROP_CON1 */
#define RGA2_ROP_CON1_OFFSET                               (0x164U)
#define RGA2_ROP_CON1_SW_ROP3_CODE1_SHIFT                  (0U)
#define RGA2_ROP_CON1_SW_ROP3_CODE1_MASK                   (0x1FFFFFFU << RGA2_ROP_CON1_SW_ROP3_CODE1_SHIFT)            /* 0x01FFFFFF */
/* OSD_INVERTSION_CAL1 */
#define RGA2_OSD_INVERTSION_CAL1_OFFSET                    (0x164U)
#define RGA2_OSD_INVERTSION_CAL1_SW_OSD_MINA_SHIFT         (0U)
#define RGA2_OSD_INVERTSION_CAL1_SW_OSD_MINA_MASK          (0xFFU << RGA2_OSD_INVERTSION_CAL1_SW_OSD_MINA_SHIFT)        /* 0x000000FF */
#define RGA2_OSD_INVERTSION_CAL1_SW_OSD_MAXA_SHIFT         (8U)
#define RGA2_OSD_INVERTSION_CAL1_SW_OSD_MAXA_MASK          (0xFFU << RGA2_OSD_INVERTSION_CAL1_SW_OSD_MAXA_SHIFT)        /* 0x0000FF00 */
/* MASK_BASE */
#define RGA2_MASK_BASE_OFFSET                              (0x168U)
#define RGA2_MASK_BASE_SW_MASK_BASE_SHIFT                  (0U)
#define RGA2_MASK_BASE_SW_MASK_BASE_MASK                   (0xFFFFFFFFU << RGA2_MASK_BASE_SW_MASK_BASE_SHIFT)           /* 0xFFFFFFFF */
/* SRC_SCL_OFFSET */
#define RGA2_SRC_SCL_OFFSET_OFFSET                         (0x16CU)
#define RGA2_SRC_SCL_OFFSET_SW_SRC_H_FACTOR_OFFSET_SHIFT   (0U)
#define RGA2_SRC_SCL_OFFSET_SW_SRC_H_FACTOR_OFFSET_MASK    (0xFFFU << RGA2_SRC_SCL_OFFSET_SW_SRC_H_FACTOR_OFFSET_SHIFT) /* 0x00000FFF */
#define RGA2_SRC_SCL_OFFSET_SW_SRC_V_FACTOR_OFFSET_SHIFT   (16U)
#define RGA2_SRC_SCL_OFFSET_SW_SRC_V_FACTOR_OFFSET_MASK    (0xFFFU << RGA2_SRC_SCL_OFFSET_SW_SRC_V_FACTOR_OFFSET_SHIFT) /* 0x0FFF0000 */
/* PREFETCH_ADDR_TH */
#define RGA2_PREFETCH_ADDR_TH_OFFSET                       (0x170U)
#define RGA2_PREFETCH_ADDR_TH_SW_PREFETCH_ADDR_TH_Y_SHIFT  (0U)
#define RGA2_PREFETCH_ADDR_TH_SW_PREFETCH_ADDR_TH_Y_MASK   (0xFFFFU << RGA2_PREFETCH_ADDR_TH_SW_PREFETCH_ADDR_TH_Y_SHIFT) /* 0x0000FFFF */
#define RGA2_PREFETCH_ADDR_TH_SW_PREFETCH_ADDR_TH_UV_SHIFT (16U)
#define RGA2_PREFETCH_ADDR_TH_SW_PREFETCH_ADDR_TH_UV_MASK  (0xFFFFU << RGA2_PREFETCH_ADDR_TH_SW_PREFETCH_ADDR_TH_UV_SHIFT) /* 0xFFFF0000 */
/* OSD_ST_LUMA0 */
#define RGA2_OSD_ST_LUMA0_OFFSET                           (0x400U)
#define RGA2_OSD_ST_LUMA0                                  (0x0U)
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA0_SHIFT            (0U)
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA0_MASK             (0xFFU << RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA0_SHIFT)           /* 0x000000FF */
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA1_SHIFT            (8U)
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA1_MASK             (0xFFU << RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA1_SHIFT)           /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA2_SHIFT            (16U)
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA2_MASK             (0xFFU << RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA2_SHIFT)           /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA3_SHIFT            (24U)
#define RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA3_MASK             (0xFFU << RGA2_OSD_ST_LUMA0_RO_OSD_ST_LUMA3_SHIFT)           /* 0xFF000000 */
/* OSD_ST_LUMA1 */
#define RGA2_OSD_ST_LUMA1_OFFSET                           (0x404U)
#define RGA2_OSD_ST_LUMA1                                  (0x0U)
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA4_SHIFT            (0U)
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA4_MASK             (0xFFU << RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA4_SHIFT)           /* 0x000000FF */
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA5_SHIFT            (8U)
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA5_MASK             (0xFFU << RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA5_SHIFT)           /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA6_SHIFT            (16U)
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA6_MASK             (0xFFU << RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA6_SHIFT)           /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA7_SHIFT            (24U)
#define RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA7_MASK             (0xFFU << RGA2_OSD_ST_LUMA1_RO_OSD_ST_LUMA7_SHIFT)           /* 0xFF000000 */
/* OSD_ST_LUMA2 */
#define RGA2_OSD_ST_LUMA2_OFFSET                           (0x408U)
#define RGA2_OSD_ST_LUMA2                                  (0x0U)
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA8_SHIFT            (0U)
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA8_MASK             (0xFFU << RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA8_SHIFT)           /* 0x000000FF */
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA9_SHIFT            (8U)
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA9_MASK             (0xFFU << RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA9_SHIFT)           /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA10_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA10_MASK            (0xFFU << RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA10_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA11_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA11_MASK            (0xFFU << RGA2_OSD_ST_LUMA2_RO_OSD_ST_LUMA11_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA3 */
#define RGA2_OSD_ST_LUMA3_OFFSET                           (0x40CU)
#define RGA2_OSD_ST_LUMA3                                  (0x0U)
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA12_SHIFT           (0U)
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA12_MASK            (0xFFU << RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA12_SHIFT)          /* 0x000000FF */
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA13_SHIFT           (8U)
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA13_MASK            (0xFFU << RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA13_SHIFT)          /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA14_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA14_MASK            (0xFFU << RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA14_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA15_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA15_MASK            (0xFFU << RGA2_OSD_ST_LUMA3_RO_OSD_ST_LUMA15_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA4 */
#define RGA2_OSD_ST_LUMA4_OFFSET                           (0x410U)
#define RGA2_OSD_ST_LUMA4                                  (0x0U)
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA16_SHIFT           (0U)
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA16_MASK            (0xFFU << RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA16_SHIFT)          /* 0x000000FF */
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA17_SHIFT           (8U)
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA17_MASK            (0xFFU << RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA17_SHIFT)          /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA18_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA18_MASK            (0xFFU << RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA18_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA19_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA19_MASK            (0xFFU << RGA2_OSD_ST_LUMA4_RO_OSD_ST_LUMA19_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA5 */
#define RGA2_OSD_ST_LUMA5_OFFSET                           (0x414U)
#define RGA2_OSD_ST_LUMA5                                  (0x0U)
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA20_SHIFT           (0U)
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA20_MASK            (0xFFU << RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA20_SHIFT)          /* 0x000000FF */
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA21_SHIFT           (8U)
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA21_MASK            (0xFFU << RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA21_SHIFT)          /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA22_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA22_MASK            (0xFFU << RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA22_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA23_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA23_MASK            (0xFFU << RGA2_OSD_ST_LUMA5_RO_OSD_ST_LUMA23_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA6 */
#define RGA2_OSD_ST_LUMA6_OFFSET                           (0x418U)
#define RGA2_OSD_ST_LUMA6                                  (0x0U)
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA24_SHIFT           (0U)
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA24_MASK            (0xFFU << RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA24_SHIFT)          /* 0x000000FF */
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA25_SHIFT           (8U)
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA25_MASK            (0xFFU << RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA25_SHIFT)          /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA26_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA26_MASK            (0xFFU << RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA26_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA27_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA27_MASK            (0xFFU << RGA2_OSD_ST_LUMA6_RO_OSD_ST_LUMA27_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA7 */
#define RGA2_OSD_ST_LUMA7_OFFSET                           (0x41CU)
#define RGA2_OSD_ST_LUMA7                                  (0x0U)
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA28_SHIFT           (0U)
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA28_MASK            (0xFFU << RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA28_SHIFT)          /* 0x000000FF */
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA29_SHIFT           (8U)
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA29_MASK            (0xFFU << RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA29_SHIFT)          /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA30_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA30_MASK            (0xFFU << RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA30_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA31_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA31_MASK            (0xFFU << RGA2_OSD_ST_LUMA7_RO_OSD_ST_LUMA31_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA8 */
#define RGA2_OSD_ST_LUMA8_OFFSET                           (0x420U)
#define RGA2_OSD_ST_LUMA8                                  (0x0U)
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA32_SHIFT           (0U)
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA32_MASK            (0xFFU << RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA32_SHIFT)          /* 0x000000FF */
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA33_SHIFT           (8U)
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA33_MASK            (0xFFU << RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA33_SHIFT)          /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA34_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA34_MASK            (0xFFU << RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA34_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA35_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA35_MASK            (0xFFU << RGA2_OSD_ST_LUMA8_RO_OSD_ST_LUMA35_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA9 */
#define RGA2_OSD_ST_LUMA9_OFFSET                           (0x424U)
#define RGA2_OSD_ST_LUMA9                                  (0x0U)
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA36_SHIFT           (0U)
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA36_MASK            (0xFFU << RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA36_SHIFT)          /* 0x000000FF */
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA37_SHIFT           (8U)
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA37_MASK            (0xFFU << RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA37_SHIFT)          /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA38_SHIFT           (16U)
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA38_MASK            (0xFFU << RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA38_SHIFT)          /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA39_SHIFT           (24U)
#define RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA39_MASK            (0xFFU << RGA2_OSD_ST_LUMA9_RO_OSD_ST_LUMA39_SHIFT)          /* 0xFF000000 */
/* OSD_ST_LUMA10 */
#define RGA2_OSD_ST_LUMA10_OFFSET                          (0x428U)
#define RGA2_OSD_ST_LUMA10                                 (0x0U)
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA40_SHIFT          (0U)
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA40_MASK           (0xFFU << RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA40_SHIFT)         /* 0x000000FF */
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA41_SHIFT          (8U)
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA41_MASK           (0xFFU << RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA41_SHIFT)         /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA42_SHIFT          (16U)
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA42_MASK           (0xFFU << RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA42_SHIFT)         /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA43_SHIFT          (24U)
#define RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA43_MASK           (0xFFU << RGA2_OSD_ST_LUMA10_RO_OSD_ST_LUMA43_SHIFT)         /* 0xFF000000 */
/* OSD_ST_LUMA11 */
#define RGA2_OSD_ST_LUMA11_OFFSET                          (0x42CU)
#define RGA2_OSD_ST_LUMA11                                 (0x0U)
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA44_SHIFT          (0U)
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA44_MASK           (0xFFU << RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA44_SHIFT)         /* 0x000000FF */
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA45_SHIFT          (8U)
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA45_MASK           (0xFFU << RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA45_SHIFT)         /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA46_SHIFT          (16U)
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA46_MASK           (0xFFU << RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA46_SHIFT)         /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA47_SHIFT          (24U)
#define RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA47_MASK           (0xFFU << RGA2_OSD_ST_LUMA11_RO_OSD_ST_LUMA47_SHIFT)         /* 0xFF000000 */
/* OSD_ST_LUMA12 */
#define RGA2_OSD_ST_LUMA12_OFFSET                          (0x430U)
#define RGA2_OSD_ST_LUMA12                                 (0x0U)
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA48_SHIFT          (0U)
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA48_MASK           (0xFFU << RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA48_SHIFT)         /* 0x000000FF */
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA49_SHIFT          (8U)
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA49_MASK           (0xFFU << RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA49_SHIFT)         /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA50_SHIFT          (16U)
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA50_MASK           (0xFFU << RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA50_SHIFT)         /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA51_SHIFT          (24U)
#define RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA51_MASK           (0xFFU << RGA2_OSD_ST_LUMA12_RO_OSD_ST_LUMA51_SHIFT)         /* 0xFF000000 */
/* OSD_ST_LUMA13 */
#define RGA2_OSD_ST_LUMA13_OFFSET                          (0x434U)
#define RGA2_OSD_ST_LUMA13                                 (0x0U)
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA52_SHIFT          (0U)
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA52_MASK           (0xFFU << RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA52_SHIFT)         /* 0x000000FF */
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA53_SHIFT          (8U)
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA53_MASK           (0xFFU << RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA53_SHIFT)         /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA54_SHIFT          (16U)
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA54_MASK           (0xFFU << RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA54_SHIFT)         /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA55_SHIFT          (24U)
#define RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA55_MASK           (0xFFU << RGA2_OSD_ST_LUMA13_RO_OSD_ST_LUMA55_SHIFT)         /* 0xFF000000 */
/* OSD_ST_LUMA14 */
#define RGA2_OSD_ST_LUMA14_OFFSET                          (0x438U)
#define RGA2_OSD_ST_LUMA14                                 (0x0U)
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA56_SHIFT          (0U)
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA56_MASK           (0xFFU << RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA56_SHIFT)         /* 0x000000FF */
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA57_SHIFT          (8U)
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA57_MASK           (0xFFU << RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA57_SHIFT)         /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA58_SHIFT          (16U)
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA58_MASK           (0xFFU << RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA58_SHIFT)         /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA59_SHIFT          (24U)
#define RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA59_MASK           (0xFFU << RGA2_OSD_ST_LUMA14_RO_OSD_ST_LUMA59_SHIFT)         /* 0xFF000000 */
/* OSD_ST_LUMA15 */
#define RGA2_OSD_ST_LUMA15_OFFSET                          (0x43CU)
#define RGA2_OSD_ST_LUMA15                                 (0x0U)
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA60_SHIFT          (0U)
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA60_MASK           (0xFFU << RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA60_SHIFT)         /* 0x000000FF */
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA61_SHIFT          (8U)
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA61_MASK           (0xFFU << RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA61_SHIFT)         /* 0x0000FF00 */
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA62_SHIFT          (16U)
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA62_MASK           (0xFFU << RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA62_SHIFT)         /* 0x00FF0000 */
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA63_SHIFT          (24U)
#define RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA63_MASK           (0xFFU << RGA2_OSD_ST_LUMA15_RO_OSD_ST_LUMA63_SHIFT)         /* 0xFF000000 */
/* DTE_ADDR */
#define RGA2_DTE_ADDR_OFFSET                               (0xF00U)
#define RGA2_DTE_ADDR_MMU_DTE_ADDR_SHIFT                   (0U)
#define RGA2_DTE_ADDR_MMU_DTE_ADDR_MASK                    (0xFFFFFFFFU << RGA2_DTE_ADDR_MMU_DTE_ADDR_SHIFT)            /* 0xFFFFFFFF */
/* STATUS */
#define RGA2_STATUS_OFFSET                                 (0xF04U)
#define RGA2_STATUS                                        (0x0U)
#define RGA2_STATUS_PAGING_ENABLED_SHIFT                   (0U)
#define RGA2_STATUS_PAGING_ENABLED_MASK                    (0x1U << RGA2_STATUS_PAGING_ENABLED_SHIFT)                   /* 0x00000001 */
#define RGA2_STATUS_PAGE_FAULT_ACTIVE_SHIFT                (1U)
#define RGA2_STATUS_PAGE_FAULT_ACTIVE_MASK                 (0x1U << RGA2_STATUS_PAGE_FAULT_ACTIVE_SHIFT)                /* 0x00000002 */
#define RGA2_STATUS_STAIL_ACTIVE_SHIFT                     (2U)
#define RGA2_STATUS_STAIL_ACTIVE_MASK                      (0x1U << RGA2_STATUS_STAIL_ACTIVE_SHIFT)                     /* 0x00000004 */
#define RGA2_STATUS_MMU_IDLE_SHIFT                         (3U)
#define RGA2_STATUS_MMU_IDLE_MASK                          (0x1U << RGA2_STATUS_MMU_IDLE_SHIFT)                         /* 0x00000008 */
#define RGA2_STATUS_REPLAY_BUFFER_EMPTY_SHIFT              (4U)
#define RGA2_STATUS_REPLAY_BUFFER_EMPTY_MASK               (0x1U << RGA2_STATUS_REPLAY_BUFFER_EMPTY_SHIFT)              /* 0x00000010 */
#define RGA2_STATUS_PAGE_FAULT_IS_WRITE_SHIFT              (5U)
#define RGA2_STATUS_PAGE_FAULT_IS_WRITE_MASK               (0x1U << RGA2_STATUS_PAGE_FAULT_IS_WRITE_SHIFT)              /* 0x00000020 */
#define RGA2_STATUS_PAGE_FAULT_BUS_ID_SHIFT                (6U)
#define RGA2_STATUS_PAGE_FAULT_BUS_ID_MASK                 (0x1FU << RGA2_STATUS_PAGE_FAULT_BUS_ID_SHIFT)               /* 0x000007C0 */
/* COMMAND */
#define RGA2_COMMAND_OFFSET                                (0xF08U)
#define RGA2_COMMAND_MMU_CMD_SHIFT                         (0U)
#define RGA2_COMMAND_MMU_CMD_MASK                          (0x7U << RGA2_COMMAND_MMU_CMD_SHIFT)                         /* 0x00000007 */
/* PAGE_FAULT_ADDR */
#define RGA2_PAGE_FAULT_ADDR_OFFSET                        (0xF0CU)
#define RGA2_PAGE_FAULT_ADDR                               (0x0U)
#define RGA2_PAGE_FAULT_ADDR_PAGE_FAULT_ADDR_SHIFT         (0U)
#define RGA2_PAGE_FAULT_ADDR_PAGE_FAULT_ADDR_MASK          (0xFFFFFFFFU << RGA2_PAGE_FAULT_ADDR_PAGE_FAULT_ADDR_SHIFT)  /* 0xFFFFFFFF */
/* ZAP_ONE_LINE */
#define RGA2_ZAP_ONE_LINE_OFFSET                           (0xF10U)
#define RGA2_ZAP_ONE_LINE_MMU_ZAP_ONE_LINE_SHIFT           (0U)
#define RGA2_ZAP_ONE_LINE_MMU_ZAP_ONE_LINE_MASK            (0xFFFFFFFFU << RGA2_ZAP_ONE_LINE_MMU_ZAP_ONE_LINE_SHIFT)    /* 0xFFFFFFFF */
/* INT_RAWSTAT */
#define RGA2_INT_RAWSTAT_OFFSET                            (0xF14U)
#define RGA2_INT_RAWSTAT_PAGE_FAULT_SHIFT                  (0U)
#define RGA2_INT_RAWSTAT_PAGE_FAULT_MASK                   (0x1U << RGA2_INT_RAWSTAT_PAGE_FAULT_SHIFT)                  /* 0x00000001 */
#define RGA2_INT_RAWSTAT_READ_BUS_ERROR_SHIFT              (1U)
#define RGA2_INT_RAWSTAT_READ_BUS_ERROR_MASK               (0x1U << RGA2_INT_RAWSTAT_READ_BUS_ERROR_SHIFT)              /* 0x00000002 */
/* INT_CLEAR */
#define RGA2_INT_CLEAR_OFFSET                              (0xF18U)
#define RGA2_INT_CLEAR_PAGE_FAULT_SHIFT                    (0U)
#define RGA2_INT_CLEAR_PAGE_FAULT_MASK                     (0x1U << RGA2_INT_CLEAR_PAGE_FAULT_SHIFT)                    /* 0x00000001 */
#define RGA2_INT_CLEAR_READ_BUS_ERROR_SHIFT                (1U)
#define RGA2_INT_CLEAR_READ_BUS_ERROR_MASK                 (0x1U << RGA2_INT_CLEAR_READ_BUS_ERROR_SHIFT)                /* 0x00000002 */
/* INT_MASK */
#define RGA2_INT_MASK_OFFSET                               (0xF1CU)
#define RGA2_INT_MASK_PAGE_FAULT_SHIFT                     (0U)
#define RGA2_INT_MASK_PAGE_FAULT_MASK                      (0x1U << RGA2_INT_MASK_PAGE_FAULT_SHIFT)                     /* 0x00000001 */
#define RGA2_INT_MASK_READ_BUS_ERROR_SHIFT                 (1U)
#define RGA2_INT_MASK_READ_BUS_ERROR_MASK                  (0x1U << RGA2_INT_MASK_READ_BUS_ERROR_SHIFT)                 /* 0x00000002 */
/* INT_STATUS */
#define RGA2_INT_STATUS_OFFSET                             (0xF20U)
#define RGA2_INT_STATUS                                    (0x0U)
#define RGA2_INT_STATUS_PAGE_FAULT_SHIFT                   (0U)
#define RGA2_INT_STATUS_PAGE_FAULT_MASK                    (0x1U << RGA2_INT_STATUS_PAGE_FAULT_SHIFT)                   /* 0x00000001 */
#define RGA2_INT_STATUS_READ_BUS_ERROR_SHIFT               (1U)
#define RGA2_INT_STATUS_READ_BUS_ERROR_MASK                (0x1U << RGA2_INT_STATUS_READ_BUS_ERROR_SHIFT)               /* 0x00000002 */
/* AUTO_GATING */
#define RGA2_AUTO_GATING_OFFSET                            (0xF24U)
#define RGA2_AUTO_GATING_MMU_AUTO_GATING_SHIFT             (0U)
#define RGA2_AUTO_GATING_MMU_AUTO_GATING_MASK              (0x1U << RGA2_AUTO_GATING_MMU_AUTO_GATING_SHIFT)             /* 0x00000001 */
#define RGA2_AUTO_GATING_SW_MMU_OLD_EN_SHIFT               (31U)
#define RGA2_AUTO_GATING_SW_MMU_OLD_EN_MASK                (0x1U << RGA2_AUTO_GATING_SW_MMU_OLD_EN_SHIFT)               /* 0x80000000 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __RK3576_RGA_H */
