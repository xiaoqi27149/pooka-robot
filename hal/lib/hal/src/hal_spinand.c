/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_SPINAND_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SPINAND
 *  @{
 */

/** @defgroup SPINAND_How_To_Use How To Use
 *  @{

 HAL_SPINAND is the SPI Nand protocol layer.

 **HAL_SPINAND  controller host supported feature**

 - X1, X4 transfer
 - choose SPI mode

 **The SPINAND  driver can be used as follows**

 Register Host:

 - Init a SPI Nor abstract:
   - Malloc struct SPI_NAND  nor by user;
   - Designated host to SPI_NAND_HOST,  define a suitable spi->mode;
   - Init SPI Nor abstract by calling HAL_SPINAND _Init();

 SPI Nor operations for user:

 - Read, write and erase function groups  like HAL_SPINAND_PageReadRaw();

 **Add New SPI Nand flash**

  Figure out following points for adding a new SPI Nor flash to flash info table:

 - flash array
 - standard SPI (DI / DO), Quad SPI (D0 / D1/ D2 /D3) command
 - feature
   - [3, 2] quad r/w;
 - QE bit
 - META_AREA
 - ECC bits and the way to get ECC status

 @} */

/** @defgroup SPINAND_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
// #define HAL_SPINAND_DEBUG
#ifdef HAL_SPINAND_DEBUG
#define HAL_SPINAND_DBG(...) HAL_DBG(__VA_ARGS__)
#else
#define HAL_SPINAND_DBG(...) do { if (0) { HAL_DBG(__VA_ARGS__); } } while (0)
#endif

/* FLASH_INFO feature */
#define FEA_4BIT_READ                 HAL_BIT(2)
#define FEA_4BIT_PROG                 HAL_BIT(3)
#define FEA_QUAD_LINE_TRANSFER_PRESET HAL_BIT(6)

#define SPINAND_WR_EN_DIS_OP(enable)                              \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD((enable) ? 0x06 : 0x04, 1), \
            HAL_SPI_MEM_OP_NO_ADDR,                               \
            HAL_SPI_MEM_OP_NO_DUMMY,                              \
            HAL_SPI_MEM_OP_NO_DATA)

#define SPINAND_READID_OP(ndummy, buf, len)     \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD(0x9f, 1), \
            HAL_SPI_MEM_OP_ADDR(1, 0, 1),       \
            HAL_SPI_MEM_OP_NO_DUMMY,            \
            HAL_SPI_MEM_OP_DATA_IN(len, buf, 1))

#define SPINAND_SET_FEATURE_OP(reg, valptr)     \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD(0x1f, 1), \
            HAL_SPI_MEM_OP_ADDR(1, reg, 1),     \
            HAL_SPI_MEM_OP_NO_DUMMY,            \
            HAL_SPI_MEM_OP_DATA_OUT(1, valptr, 1))

#define SPINAND_GET_FEATURE_OP(reg, valptr)     \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD(0x0f, 1), \
            HAL_SPI_MEM_OP_ADDR(1, reg, 1),     \
            HAL_SPI_MEM_OP_NO_DUMMY,            \
            HAL_SPI_MEM_OP_DATA_IN(1, valptr, 1))

#define SPINAND_PAGE_READ_OP(addr)              \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD(0x13, 1), \
            HAL_SPI_MEM_OP_ADDR(3, addr, 1),    \
            HAL_SPI_MEM_OP_NO_DUMMY,            \
            HAL_SPI_MEM_OP_NO_DATA)

#define SPINAND_PAGE_READ_FROM_CACHE(dataLines, addr, buf, len)           \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD((dataLines == 4 ? 0x6b : 0x03), 1), \
            HAL_SPI_MEM_OP_ADDR(3, addr << 8, 1),                         \
            HAL_SPI_MEM_OP_NO_DUMMY,                                      \
            HAL_SPI_MEM_OP_DATA_IN(len, buf, dataLines))

#define SPINAND_PROG_EXEC_OP(addr)              \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD(0x10, 1), \
           HAL_SPI_MEM_OP_ADDR(3, addr, 1),     \
           HAL_SPI_MEM_OP_NO_DUMMY,             \
           HAL_SPI_MEM_OP_NO_DATA)

#define SPINAND_PROG_LOAD_TO_CACHE(dataLines, addr, buf, len)             \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD((dataLines == 4 ? 0x32 : 0x02), 1), \
           HAL_SPI_MEM_OP_ADDR(2, addr, 1),                               \
           HAL_SPI_MEM_OP_NO_DUMMY,                                       \
           HAL_SPI_MEM_OP_DATA_OUT(len, buf, dataLines))

#define SPINAND_BLK_ERASE_OP(addr)              \
    HAL_SPI_MEM_OP(HAL_SPI_MEM_OP_CMD(0xd8, 1), \
           HAL_SPI_MEM_OP_ADDR(3, addr, 1),     \
           HAL_SPI_MEM_OP_NO_DUMMY,             \
           HAL_SPI_MEM_OP_NO_DATA)

/* SPINAND_INFO feature */
#define SPINAND_FEA_4BIT_READ HAL_BIT(2)
#define SPINAND_FEA_4BIT_PROG HAL_BIT(3)

/***************************** Structure Definition **************************/

/* SPI_NAND devices info */
struct SPINAND_INFO {
    uint8_t id0;
    uint8_t id1;
    uint8_t id2;

    uint16_t secPerPage;
    uint16_t pagePerBlk;
    uint16_t planePerDie;
    uint16_t blkPerPlane;

    uint8_t feature;

    uint8_t density;  /* capacity in sector = (1 << density), 512B/sector */
    uint8_t maxECCBits;
    uint8_t hasQeBit;

    struct META_AREA meta;
    int32_t (*eccStatus)(struct SPI_NAND *spinand);
};

/********************* Private Variable Definition ***************************/

static int32_t SPINAND_GetEccStatus0(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus1(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus2(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus3(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus4(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus5(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus6(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus7(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus8(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus9(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus10(struct SPI_NAND *spinand);
static int32_t SPINAND_GetEccStatus11(struct SPI_NAND *spinand);

#define sfc_nand_get_ecc_status0  SPINAND_GetEccStatus0
#define sfc_nand_get_ecc_status1  SPINAND_GetEccStatus1
#define sfc_nand_get_ecc_status2  SPINAND_GetEccStatus2
#define sfc_nand_get_ecc_status3  SPINAND_GetEccStatus3
#define sfc_nand_get_ecc_status4  SPINAND_GetEccStatus4
#define sfc_nand_get_ecc_status5  SPINAND_GetEccStatus5
#define sfc_nand_get_ecc_status6  SPINAND_GetEccStatus6
#define sfc_nand_get_ecc_status7  SPINAND_GetEccStatus7
#define sfc_nand_get_ecc_status8  SPINAND_GetEccStatus8
#define sfc_nand_get_ecc_status9  SPINAND_GetEccStatus9
#define sfc_nand_get_ecc_status10 SPINAND_GetEccStatus10
#define sfc_nand_get_ecc_status11 SPINAND_GetEccStatus11

static const struct SPINAND_INFO s_spiNandTable[] = {
    /* TC58CVG0S0HxAIx */
    { 0x98, 0xC2, 0x00, 4, 0x40, 1, 1024, 0x00, 18, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* TC58CVG1S0HxAIx */
    { 0x98, 0xCB, 0x00, 4, 0x40, 2, 1024, 0x00, 19, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* TC58CVG2S0HRAIJ */
    { 0x98, 0xED, 0x00, 8, 0x40, 1, 2048, 0x0C, 20, 0x8, 0, { 0x04, 0x0C, 0x08, 0x10 }, &sfc_nand_get_ecc_status0 },
    /* TC58CVG1S3HRAIJ */
    { 0x98, 0xEB, 0x00, 4, 0x40, 1, 2048, 0x0C, 19, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* TC58CVG0S3HRAIJ */
    { 0x98, 0xE2, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },

    /* MX35LF1GE4AB */
    { 0xC2, 0x12, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* MX35LF2GE4AB */
    { 0xC2, 0x22, 0x00, 4, 0x40, 2, 1024, 0x0C, 19, 0x4, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* MX35LF2GE4AD */
    { 0xC2, 0x26, 0x00, 4, 0x40, 1, 2048, 0x0C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* MX35LF4GE4AD */
    { 0xC2, 0x37, 0x00, 8, 0x40, 1, 2048, 0x0C, 20, 0x8, 1, { 0x04, 0x08, 0x14, 0x18 }, &sfc_nand_get_ecc_status0 },
    /* MX35UF1GE4AC */
    { 0xC2, 0x92, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* MX35UF2GE4AC */
    { 0xC2, 0xA2, 0x00, 4, 0x40, 1, 2048, 0x0C, 19, 0x4, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* MX35UF1GE4AD */
    { 0xC2, 0x96, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* MX35UF2GE4AD */
    { 0xC2, 0xA6, 0x00, 4, 0x40, 1, 2048, 0x0C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* MX35UF4GE4AD */
    { 0xC2, 0xB7, 0x00, 8, 0x40, 1, 2048, 0x0C, 20, 0x8, 1, { 0x04, 0x08, 0x14, 0x18 }, &sfc_nand_get_ecc_status0 },

    /* GD5F1GQ4UAYIG */
    { 0xC8, 0xF1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* GD5F1GQ4RB9IGR */
    { 0xC8, 0xD1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F2GQ40BY2GR */
    { 0xC8, 0xD2, 0x00, 4, 0x40, 2, 1024, 0x0C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F1GQ5UEYIG */
    { 0xC8, 0x51, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status2 },
    /* GD5F2GQ5UEYIG */
    { 0xC8, 0x52, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status2 },
    /* GD5F1GQ4R */
    { 0xC8, 0xC1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F4GQ6RExxG 1*4096 */
    { 0xC8, 0x45, 0x00, 4, 0x40, 2, 2048, 0x4C, 20, 0x4, 1, { 0x04, 0x08, 0x14, 0x18 }, &sfc_nand_get_ecc_status2 },
    /* GD5F4GQ6UExxG 1*4096 */
    { 0xC8, 0x55, 0x00, 4, 0x40, 2, 2048, 0x4C, 20, 0x4, 1, { 0x04, 0x08, 0x14, 0x18 }, &sfc_nand_get_ecc_status2 },
    /* GD5F1GQ4UExxH */
    { 0xC8, 0xD9, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F1GQ5REYIG Add 3rd code to distingush with F50L2G41KA */
    { 0xC8, 0x41, 0xC8, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status2 },
    /* GD5F2GQ5REYIG */
    { 0xC8, 0x42, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status2 },
    /* GD5F2GM7RxG */
    { 0xC8, 0x82, 0x00, 4, 0x40, 1, 2048, 0x0C, 19, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F2GM7UxG */
    { 0xC8, 0x92, 0x00, 4, 0x40, 1, 2048, 0x0C, 19, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F1GM7UxG */
    { 0xC8, 0x91, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F4GQ4UAYIG 1*4096 */
    { 0xC8, 0xF4, 0x00, 4, 0x40, 2, 2048, 0x0C, 20, 0x8, 1, { 0x04, 0x08, 0x14, 0x18 }, &sfc_nand_get_ecc_status0 },
    /* GD5F1GM7REYIGR */
    { 0xC8, 0x81, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status3 },
    /* GD5F4GM8UEYIGR */
    { 0xC8, 0x95, 0x00, 4, 0x40, 1, 4096, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status3 },
    /* GD5F4GM8REY2GY */
    { 0xC8, 0x85, 0x00, 4, 0x40, 1, 4096, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status3 },

    /* W25N01GV */
    { 0xEF, 0xAA, 0x21, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 0, { 0x04, 0x14, 0x24, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* W25N02KVZEIR */
    { 0xEF, 0xAA, 0x22, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 0, { 0x04, 0x14, 0x24, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* W25N04KVZEIR */
    { 0xEF, 0xAA, 0x23, 4, 0x40, 1, 4096, 0x4C, 20, 0x8, 0, { 0x04, 0x14, 0x24, 0x34 }, &sfc_nand_get_ecc_status0 },
    /* W25N01GW */
    { 0xEF, 0xBA, 0x21, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 0, { 0x04, 0x14, 0x24, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* W25N02KW */
    { 0xEF, 0xBA, 0x22, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 0, { 0x04, 0x14, 0x24, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* W25N512GVEIG */
    { 0xEF, 0xAA, 0x20, 4, 0x40, 1, 512, 0x4C, 17, 0x1, 0, { 0x04, 0x14, 0x24, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* W25N01KV */
    { 0xEF, 0xAE, 0x21, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 0, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* W25N01JWZEIG */
    { 0xEF, 0xBC, 0x21, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* W25N01KWZPIG */
    { 0xEF, 0xBE, 0x21, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },

    /* HYF2GQ4UAACAE */
    { 0xC9, 0x52, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0xE, 1, { 0x04, 0x24, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* HYF1GQ4UDACAE */
    { 0xC9, 0x21, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* HYF1GQ4UPACAE */
    { 0xC9, 0xA1, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* HYF2GQ4UDACAE */
    { 0xC9, 0x22, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* HYF2GQ4UHCCAE */
    { 0xC9, 0x5A, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0xE, 1, { 0x04, 0x24, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* HYF4GQ4UAACBE */
    { 0xC9, 0xD4, 0x00, 8, 0x40, 1, 2048, 0x4C, 20, 0xE, 1, { 0x20, 0x40, 0x24, 0x44 }, &sfc_nand_get_ecc_status0 },
    /* HYF2GQ4IAACAE */
    { 0xC9, 0x82, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0xE, 1, { 0x04, 0x24, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* HYF1GQ4IDACAE */
    { 0xC9, 0x81, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },

    /* FS35ND01G-S1 */
    { 0xCD, 0xB1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x10, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status5 },
    /* FS35ND02G-S2 */
    { 0xCD, 0xA2, 0x00, 4, 0x40, 1, 2048, 0x00, 19, 0x4, 0, { 0x10, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status5 },
    /* FS35ND01G-S1Y2 */
    { 0xCD, 0xEA, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* FS35ND02G-S3Y2 */
    { 0xCD, 0xEB, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* FS35ND04G-S2Y2 1*4096 */
    { 0xCD, 0xEC, 0x00, 4, 0x40, 2, 2048, 0x4C, 20, 0x4, 0, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status1 },
    /* F35SQA001G */
    { 0xCD, 0x71, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F35SQA002G */
    { 0xCD, 0x72, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x1, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F35SQA512M */
    { 0xCD, 0x70, 0x00, 4, 0x40, 1, 512, 0x4C, 17, 0x1, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F35UQA512M */
    { 0xCD, 0x60, 0x00, 4, 0x40, 1, 512, 0x4C, 17, 0x1, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F35UQA002G-WWT */
    { 0xCD, 0x62, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x1, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F35SQB004G-WWT */
    { 0xCD, 0x61, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F35SQB004G */
    { 0xCD, 0x53, 0x00, 8, 0x40, 1, 2048, 0x0C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status10 },
    /* F35SQB002G */
    { 0xCD, 0x52, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status10 },

    /* DS35Q1GA-IB */
    { 0xE5, 0x71, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* DS35Q2GA-IB */
    { 0xE5, 0x72, 0x00, 4, 0x40, 2, 1024, 0x0C, 19, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* DS35M1GA-1B */
    { 0xE5, 0x21, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* DS35M2GA-IB */
    { 0xE5, 0x22, 0x00, 4, 0x40, 2, 1024, 0x0C, 19, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* DS35Q1GB-IB */
    { 0xE5, 0xF1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35Q2GB-IB */
    { 0xE5, 0xF2, 0x00, 4, 0x40, 2, 1024, 0x0C, 19, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35Q4GM */
    { 0xE5, 0xF4, 0x00, 4, 0x40, 2, 2048, 0x0C, 20, 0x8, 1, { 0x04, 0x14, 0x0C, 0x10 }, &sfc_nand_get_ecc_status6 },
    /* DS35M1GB-IB */
    { 0xE5, 0xA1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35Q12B-IB */
    { 0xE5, 0xF5, 0x00, 4, 0x40, 1, 512, 0x0C, 17, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35M12B-IB */
    { 0xE5, 0xA5, 0x00, 4, 0x40, 1, 512, 0x0C, 17, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35Q1GD-IB */
    { 0xE5, 0x51, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35M4GB-IB */
    { 0xE5, 0x64, 0x00, 4, 0x40, 1, 4096, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status6 },
    /* DS35Q4GB-IB */
    { 0xE5, 0xB4, 0x00, 4, 0x40, 1, 4096, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status6 },
    /* DS35M12C-IB */
    { 0xE5, 0x25, 0x00, 4, 0x40, 1, 512, 0x4C, 17, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35Q12C-IB */
    { 0xE5, 0x75, 0x00, 4, 0x40, 1, 512, 0x4C, 17, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* DS35Q2GBS */
    { 0xE5, 0xB2, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },

    /* EM73C044VCC-H */
    { 0xD5, 0x22, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* EM73D044VCE-H */
    { 0xD5, 0x20, 0x00, 4, 0x40, 1, 2048, 0x0C, 19, 0x8, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* EM73E044SNA-G */
    { 0xD5, 0x03, 0x00, 8, 0x40, 1, 2048, 0x4C, 20, 0x8, 1, { 0x04, 0x28, 0x08, 0x2C }, &sfc_nand_get_ecc_status0 },
    /* EM73C044VCF-H */
    { 0xD5, 0x25, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* EM73E044VCE-H */
    { 0xD5, 0x3B, 0x00, 4, 0x40, 1, 4096, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status0 },

    /* XT26G02A */
    { 0x0B, 0xE2, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status4 },
    /* XT26G01A */
    { 0x0B, 0xE1, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status4 },
    /* XT26G04A */
    { 0x0B, 0xE3, 0x00, 4, 0x80, 1, 2048, 0x4C, 20, 0x8, 1, { 0x08, 0x0C, 0x0C, 0x10 }, &sfc_nand_get_ecc_status4 },
    /* XT26G01B */
    { 0x0B, 0xF1, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status4 },
    /* XT26G02B */
    { 0x0B, 0xF2, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 1, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status5 },
    /* XT26G01C */
    { 0x0B, 0x11, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status7 },
    /* XT26G02C */
    { 0x0B, 0x12, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status7 },
    /* XT26G04C */
    { 0x0B, 0x13, 0x00, 8, 0x40, 1, 2048, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status7 },
    /* XT26G11C */
    { 0x0B, 0x15, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26Q02DWSIGA */
    { 0x0B, 0x52, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26Q01DWSIGA */
    { 0x0B, 0x51, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26Q04DWSIGA */
    { 0x0B, 0x53, 0x00, 8, 0x40, 1, 2048, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status0 },
    /* XT26G01DWSIGA */
    { 0x0B, 0x31, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26G02DWSIGA */
    { 0x0B, 0x32, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26G04DWSIGA */
    { 0x0B, 0x33, 0x00, 8, 0x40, 1, 2048, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status0 },
    /* XT26Q04DWSIGT-B */
    { 0x0B, 0x53, 0x00, 8, 0x40, 1, 2048, 0x4C, 20, 0x14, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status0 },
    /* XT26Q01DWSIGA */
    { 0x0B, 0x51, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26G12DWSIGA */
    { 0x0B, 0x35, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26Q12DWSIGA */
    { 0x0B, 0x55, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26G11DWSIGA */
    { 0x0B, 0x34, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XT26Q14DWSIGA */
    { 0x0B, 0x56, 0x00, 8, 0x40, 1, 2048, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status0 },

    /* MT29F2G01ABA, XT26G02E, F50L2G41XA */
    { 0x2C, 0x24, 0x00, 4, 0x40, 2, 1024, 0x4C, 19, 0x8, 0, { 0x20, 0x24, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* MT29F1G01ABA, F50L1G41XA */
    { 0x2C, 0x14, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 0, { 0x20, 0x24, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },

    /* FM25S01 */
    { 0xA1, 0xA1, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 0, { 0x00, 0x04, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* FM25S01A */
    { 0xA1, 0xE4, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* FM25S02A */
    { 0xA1, 0xE5, 0x00, 4, 0x40, 2, 1024, 0x4C, 19, 0x1, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* FM25LS01 */
    { 0xA1, 0xA5, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* FM25S01BI3 */
    { 0xA1, 0xD4, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* FM25G02BI3 */
    { 0xA1, 0xD2, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* FM25S02BI3-DND-A-G3 */
    { 0xA1, 0xD6, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* FM25G02D */
    { 0xA1, 0xF2, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status5 },

    /* IS37SML01G1 */
    { 0xC8, 0x21, 0x00, 4, 0x40, 1, 1024, 0x00, 18, 0x1, 0, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F50L1G41LB */
    { 0xC8, 0x01, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x1, 0, { 0x14, 0x24, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* F50L2G41KA */
    { 0xC8, 0x41, 0x7F, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },

    /* UM19A1HISW */
    { 0xB0, 0x24, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* UM19A0HCSW */
    { 0xB0, 0x14, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* UM19A0LCSW */
    { 0xB0, 0x15, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* UM19A1LISW */
    { 0xB0, 0x25, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status6 },
    /* UM19A9LISW */
    { 0xB0, 0x0D, 0x00, 4, 0x40, 1, 512, 0x4C, 17, 0x4, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* UM19A9HISW */
    { 0xB0, 0x0C, 0x00, 4, 0x40, 1, 512, 0x4C, 17, 0x4, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* UM19A0HISW */
    { 0xB0, 0x14, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status11 },
    /* UM19A0LISW */
    { 0xB0, 0x15, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status11 },

    /* ATO25D1GA */
    { 0x9B, 0x12, 0x00, 4, 0x40, 1, 1024, 0x40, 18, 0x1, 1, { 0x14, 0x24, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* BWJX08K-2Gb */
    { 0xBC, 0xB3, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x10, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* JS28U1GQSCAHG-83 */
    { 0xBF, 0x21, 0x00, 4, 0x40, 1, 1024, 0x40, 18, 0x4, 1, { 0x08, 0x0C, 0xFF, 0xFF }, &sfc_nand_get_ecc_status8 },
    /* SGM7000I-S24W1GH */
    { 0xEA, 0xC1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* TX25G01 */
    { 0xA1, 0xF1, 0x00, 4, 0x40, 1, 1024, 0x0C, 18, 0x4, 1, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status8 },

    /* S35ML01G3, ANV1GCP0CLG, HYF1GQ4UTXCAE, YX25G1E, GSS01GSAM0, EM73C044VCE-H */
    { 0x01, 0x15, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status9 },
    /* S35ML02G3, ANV2GCP0CLG, HYF2GQ4UTXCAE, YX25G2E, EM73D044VCE-H */
    { 0x01, 0x25, 0x00, 4, 0x40, 2, 1024, 0x4C, 19, 0x4, 0, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status9 },
    /* S35ML04G3, ANV4GCP0CLG-BI */
    { 0x01, 0x35, 0x00, 4, 0x40, 2, 2048, 0x4C, 20, 0x4, 0, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status9 },

    /* GSS01GSAK1 */
    { 0x52, 0xBA, 0x13, 4, 0x40, 1, 1024, 0x4C, 18, 0x4, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* GSS02GSAK1 */
    { 0x52, 0xBA, 0x23, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* GSS02GSAX1 */
    { 0x52, 0xCA, 0x23, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* GSS01GSAX1 */
    { 0x52, 0xCA, 0x13, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* GSS01GSBX1 */
    { 0x52, 0xCB, 0x13, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },

    /* XCSP2AAPK */
    { 0x8C, 0xA1, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* XCSP1AAPK */
    { 0x8C, 0x01, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 1, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status0 },
    /* F50L1G41LC */
    { 0x8C, 0x2C, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x1, 0, { 0x04, 0x14, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },

    /* ZB35Q01BYIG */
    { 0x5E, 0xA1, 0x00, 4, 0x40, 1, 1024, 0x4C, 18, 0x8, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
    /* ZB35Q04BYIG */
    { 0x5E, 0xA3, 0x00, 4, 0x80, 1, 2048, 0x4C, 20, 0x8, 1, { 0x04, 0x08, 0x0C, 0x10 }, &sfc_nand_get_ecc_status1 },

    /* HSESYHDSW2G */
    { 0x3C, 0xD2, 0x00, 4, 0x40, 1, 2048, 0x4C, 19, 0x4, 0, { 0x04, 0x08, 0xFF, 0xFF }, &sfc_nand_get_ecc_status1 },
};

/********************* Private Function Definition ***************************/
static HAL_Status SPINAND_SPIMemExecOp(struct SPI_NAND_HOST *spi, struct HAL_SPI_MEM_OP *op)
{
    if (spi->xfer) {
        return spi->xfer(spi, op);
    } else {
        return HAL_ERROR;
    }
}

static HAL_Status SPINAND_ReadRegOp(struct SPI_NAND *spinand, uint8_t reg, uint8_t *val)
{
    struct HAL_SPI_MEM_OP op = SPINAND_GET_FEATURE_OP(reg, val);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static HAL_Status SPINAND_WriteRegOp(struct SPI_NAND *spinand, uint8_t reg, uint8_t val)
{
    struct HAL_SPI_MEM_OP op = SPINAND_SET_FEATURE_OP(reg, &val);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static int SPINAND_ReadStatus(struct SPI_NAND *spinand, uint8_t reg, uint8_t *status)
{
    return SPINAND_ReadRegOp(spinand, reg, status);
}

static int SPINAND_WriteStatus(struct SPI_NAND *spinand, uint8_t reg, uint8_t status)
{
    return SPINAND_WriteRegOp(spinand, reg, status);
}

/*
 * ecc default:
 * ecc bits: 0xC0[4,5]
 * 0x00, No bit errors were detected
 * 0x01, Bit errors were detected and corrected.
 * 0x10, Multiple bit errors were detected and not corrected.
 * 0x11, Bits errors were detected and corrected, bit error count
 *	exceed the bit flip detection threshold
 */
static int32_t SPINAND_GetEccStatus0(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x03;

    if (ecc <= 1) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 2) {
        ret = SPINAND_ECC_ERROR;
    } else {
        ret = SPINAND_ECC_REFRESH;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type1:
 * ecc bits: 0xC0[4,5]
 * 0x00, No bit errors were detected;
 * 0x01, Bits errors were detected and corrected, bit error count
 *	may reach the bit flip detection threshold;
 * 0x10, Multiple bit errors were detected and not corrected;
 * 0x11, Reserved.
 */
static int32_t SPINAND_GetEccStatus1(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x03;

    if (ecc == 0) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 1) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type2:
 * ecc bits: 0xC0[4,5] 0xF0[4,5]
 * [0x0000, 0x0011], No bit errors were detected;
 * [0x0100, 0x0111], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x1000, 0x1011], Multiple bit errors were detected and
 *	not corrected.
 * [0x1100, 0x1111], reserved.
 */
static int32_t SPINAND_GetEccStatus2(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status, status1;

    if (SPINAND_ReadStatus(spinand, 0xC0, &status) != HAL_OK) {
        return SPINAND_ECC_ERROR;
    }
    if (SPINAND_ReadStatus(spinand, 0xF0, &status1) != HAL_OK) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x03;
    ecc = (ecc << 2) | ((status1 >> 4) & 0x03);

    if (ecc < 7) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 7) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    return ret;
}

/*
 * ecc spectial type3:
 * ecc bits: 0xC0[4,5] 0xF0[4,5]
 * [0x0000, 0x0011], No bit errors were detected;
 * [0x0100, 0x0111], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x1000, 0x1011], Multiple bit errors were detected and
 *	not corrected.
 * [0x1100, 0x1111], Bit error count equals the bit flip
 *	detectio nthreshold
 */
static int32_t SPINAND_GetEccStatus3(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status, status1;

    if (SPINAND_ReadStatus(spinand, 0xC0, &status) != HAL_OK) {
        return SPINAND_ECC_ERROR;
    }
    if (SPINAND_ReadStatus(spinand, 0xF0, &status1) != HAL_OK) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x03;
    ecc = (ecc << 2) | ((status1 >> 4) & 0x03);

    if (ecc < 7) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 7 || ecc >= 12) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x, F0 %x\n", __func__, status, status1);

    return ret;
}

/*
 * ecc spectial type4:
 * ecc bits: 0xC0[2,5]
 * [0x0000], No bit errors were detected;
 * [0x0001, 0x0111], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x1000], Multiple bit errors were detected and
 *	not corrected.
 * [0x1100], Bit error count equals the bit flip
 *	detectionthreshold
 * else, reserved
 */
static int32_t SPINAND_GetEccStatus4(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 2) & 0x0f;
    if (ecc < 7) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 7 || ecc == 12) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type5:
 * ecc bits: 0xC0[4,6]
 * [0x0], No bit errors were detected;
 * [0x001, 0x011], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x100], Bit error count equals the bit flip
 *	detectionthreshold
 * [0x101, 0x110], Reserved;
 * [0x111], Multiple bit errors were detected and
 *	not corrected.
 */
static int32_t SPINAND_GetEccStatus5(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x07;

    if (ecc < 4) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 4) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type6:
 * ecc bits: 0xC0[4,6]
 * [0b000], No bit errors were detected;
 * [0b001], 1-3 Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0b010], Multiple bit errors were detected and
 *	not corrected.
 * [0b011], 4-6 Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0b101], Bit error count equals the bit flip
 *	detectionthreshold
 * others, Reserved.
 */
static int32_t SPINAND_GetEccStatus6(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x07;

    if (ecc == 0 || ecc == 1 || ecc == 3) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 5) {
        ret = SPINAND_ECC_ERROR;
    } else {
        ret = SPINAND_ECC_REFRESH;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type7:
 * ecc bits: 0xC0[4,7]
 * [0b0000], No bit errors were detected;
 * [0b0001, 0b0111], 1-7 Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0b1000], 8 Bit errors were detected and corrected. Bit error count
 *	equals the bit flip detectionthreshold;
 * [0b1111], Bit errors greater than ECC capability(8 bits) and not corrected;
 * others, Reserved.
 */
static int32_t SPINAND_GetEccStatus7(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x0f;
    if (ecc < 7) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 7 || ecc == 8) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type8:
 * ecc bits: 0xC0[4,6]
 * [0b000], No bit errors were detected;
 * [0b001, 0b011], 1~3 Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0b100], Bit error count equals the bit flip
 *	detection threshold
 * others, Reserved.
 */
static int32_t SPINAND_GetEccStatus8(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x07;

    if (ecc < 4) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 4) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type9:
 * ecc bits: 0xC0[4,5]
 * 0b00, No bit errors were detected
 * 0b01, 1-2Bit errors were detected and corrected.
 * 0b10, 3-4Bit errors were detected and corrected.
 * 0b11, 11 can be used as uncorrectable
 */
static int32_t SPINAND_GetEccStatus9(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x03;

    if (ecc <= 1) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 2) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type10:
 * ecc bits: 0xC0[4,6]
 * [0b000], No bit errors were detected;
 * [0b001, 0b101], 3~7 Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0b110], Bit error count equals the bit flip detection threshold
 * [0b111], Bit errors greater than ECC capability(8 bits) and not corrected;
 */
static int32_t SPINAND_GetEccStatus10(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x07;

    if (ecc < 6) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 6) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

/*
 * ecc spectial type11:
 * ecc bits: 0xC0[4,6]
 * [0b000], No bit errors were detected;
 * [0b001, 0b101], 3~7 Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0b110], Bit error count equals the bit flip detection threshold
 * [0b111], Bit errors greater than ECC capability(8 bits) and not corrected;
 */
static int32_t SPINAND_GetEccStatus11(struct SPI_NAND *spinand)
{
    int32_t ret;
    uint8_t ecc, status;

    ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
    if (ret) {
        return SPINAND_ECC_ERROR;
    }

    ecc = (status >> 4) & 0x07;

    if (ecc <= 1) {
        ret = SPINAND_ECC_OK;
    } else if (ecc == 3 || ecc == 5) {
        ret = SPINAND_ECC_REFRESH;
    } else {
        ret = SPINAND_ECC_ERROR;
    }

    HAL_SPINAND_DBG("%s C0 %x\n", __func__, status);

    return ret;
}

static HAL_Status SPINAND_WaitBusy(struct SPI_NAND *spinand, uint8_t *data, uint64_t timeout)
{
    int32_t ret;
    uint32_t i;
    uint8_t status;

    /* HAL_SPINAND_DBG("%s %" PRIx32 "\n", __func__, timeout); */
    *data = 0;
    for (i = 0; i < timeout; i++) {
        ret = SPINAND_ReadStatus(spinand, 0xC0, &status);
        if (ret != HAL_OK) {
            return ret;
        }

        *data = status;
        if ((status & 0x01) == 0) {
            return HAL_OK;
        }

        HAL_DelayUs(1);
    }
    HAL_SPINAND_DBG("%s error %" PRId32 "\n", __func__, timeout);

    return HAL_BUSY;
}

static HAL_Status SPINAND_LoadPageOp(struct SPI_NAND *spinand,
                                     uint32_t row)
{
    struct HAL_SPI_MEM_OP op = SPINAND_PAGE_READ_OP(row);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static HAL_Status SPINAND_ReadFromCacheOp(struct SPI_NAND *spinand,
                                          uint32_t addrMix,
                                          uint32_t *buf,
                                          uint32_t len)
{
    struct HAL_SPI_MEM_OP op = SPINAND_PAGE_READ_FROM_CACHE(spinand->readLines,
                                                            addrMix, buf, len);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static HAL_Status SPINAND_ProgOp(struct SPI_NAND *spinand,
                                 uint32_t row)
{
    struct HAL_SPI_MEM_OP op = SPINAND_PROG_EXEC_OP(row);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static HAL_Status SPINAND_WriteToCacheOp(struct SPI_NAND *spinand,
                                         uint32_t addrMix,
                                         uint32_t *buf,
                                         uint32_t len)
{
    struct HAL_SPI_MEM_OP op = SPINAND_PROG_LOAD_TO_CACHE(spinand->progLines,
                                                          addrMix, buf, len);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static HAL_Status SPINAND_EraseOp(struct SPI_NAND *spinand, uint32_t row)
{
    struct HAL_SPI_MEM_OP op = SPINAND_BLK_ERASE_OP(row);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static HAL_Status SPINAND_QuadEnable(struct SPI_NAND *spinand)
{
    int ret = HAL_OK;
    uint8_t status;

    ret = SPINAND_ReadStatus(spinand, 0xB0, &status);
    if (ret != HAL_OK) {
        return ret;
    }

    status |= 1;

    return SPINAND_WriteStatus(spinand, 0xB0, status);
}

static HAL_Status SPINAND_WriteEnableOp(struct SPI_NAND *spinand)
{
    struct HAL_SPI_MEM_OP op = SPINAND_WR_EN_DIS_OP(true);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

static const struct SPINAND_INFO *SPINAND_GerFlashInfo(uint8_t *flashId)
{
    uint32_t i;

    for (i = 0; i < HAL_ARRAY_SIZE(s_spiNandTable); i++) {
        if (s_spiNandTable[i].id0 == flashId[0] &&
            s_spiNandTable[i].id1 == flashId[1]) {
            if (s_spiNandTable[i].id2 &&
                s_spiNandTable[i].id2 != flashId[2]) {
                continue;
            }

            return &s_spiNandTable[i];
        }
    }

    return NULL;
}

/********************* Public Function Definition ****************************/

/** @defgroup SPINAND_Exported_Functions_Group3 IO Functions
 *  @{
 */

/**
 * @brief  SPI Nand block erase.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @return HAL_Status.
 */
HAL_Status HAL_SPINAND_EraseBlock(struct SPI_NAND *spinand, uint32_t addr)
{
    uint8_t status;
    int ret;

    HAL_SPINAND_DBG("%s %x\n", __func__, addr);

    ret = SPINAND_WriteEnableOp(spinand);
    if (ret) {
        return ret;
    }

    ret = SPINAND_EraseOp(spinand, addr);
    if (ret) {
        return ret;
    }

    ret = SPINAND_WaitBusy(spinand, &status, 1000 * 1000);
    if (ret) {
        return ret;
    }

    if (status & SPINAND_ERASE_FAIL) {
        return SPINAND_ERASE_FAIL;
    }

    return ret;
}

/**
 * @brief  SPI Nand raw page read.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @param  pData: destination address.
 * @param  oob: whether pData including oob data.
 * @return return value < 0: transfer error, return HAL_Status;
 *   return value >= 0: transfer ok, return ecc status, check SPINAND_ECC_XXXX;
 */
int32_t HAL_SPINAND_ReadPageRaw(struct SPI_NAND *spinand, uint32_t addr, void *pData, bool oob)
{
    uint8_t status;
    int ret;
    uint32_t addrMix, size;

    HAL_SPINAND_DBG("%s 0x%x\n", __func__, addr);

    if (!pData) {
        return HAL_INVAL;
    }

    ret = SPINAND_LoadPageOp(spinand, addr);
    if (ret) {
        return ret;
    }

    ret = SPINAND_WaitBusy(spinand, &status, 1000 * 1000);
    if (ret < 0) {
        return ret;
    }

    addrMix = spinand->planePerDie == 1 ? 0 : ((addr >> 6) & 0x1) << 12;
    size = oob ? spinand->secPerPage * SPINAND_SECTOR_FULL_SIZE : spinand->secPerPage * SPINAND_SECTOR_SIZE;
    ret = SPINAND_ReadFromCacheOp(spinand, addrMix, pData, size);
    if (ret) {
        return ret;
    }

    return spinand->eccStatus(spinand);
}

/**
 * @brief  SPI Nand raw page program.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @param  pData: source address.
 * @param  oob: whether pData including oob data.
 * @return HAL_Status.
 */
HAL_Status HAL_SPINAND_ProgPageRaw(struct SPI_NAND *spinand, uint32_t addr, const void *pData, bool oob)
{
    uint8_t status;
    int ret;
    uint32_t addrMix, size;

    HAL_SPINAND_DBG("%s 0x%x\n", __func__, addr);

    if (!pData) {
        return HAL_INVAL;
    }

    ret = SPINAND_WriteEnableOp(spinand);
    if (ret) {
        return ret;
    }

    addrMix = spinand->planePerDie == 1 ? 0 : ((addr >> 6) & 0x1) << 12;
    size = oob ? spinand->secPerPage * SPINAND_SECTOR_FULL_SIZE : spinand->secPerPage * SPINAND_SECTOR_SIZE;
    ret = SPINAND_WriteToCacheOp(spinand, addrMix, (uint32_t *)pData, size);
    if (ret) {
        return ret;
    }

    ret = SPINAND_ProgOp(spinand, addr);
    if (ret) {
        return ret;
    }

    ret = SPINAND_WaitBusy(spinand, &status, 1000 * 1000);
    if (ret) {
        return ret;
    }

    if (status & SPINAND_PROG_FAIL) {
        return SPINAND_PROG_FAIL;
    }

    return ret;
}

/**
 * @brief  SPI Nand page read.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @param  pData: destination address.
 * @param  pSpare: destination spare address.
 * @return return value < 0: transfer error, return HAL_Status;
 *   return value >= 0: transfer ok, return ecc status, check SPINAND_ECC_XXXX;
 */
int32_t HAL_SPINAND_ReadPage(struct SPI_NAND *spinand, uint32_t addr, void *pData, uint32_t *pSpare)
{
    int32_t ret;
    uint32_t secs = spinand->secPerPage;
    uint32_t dataSize = secs * SPINAND_SECTOR_SIZE;
    struct META_AREA *meta = &spinand->meta;
    uint32_t *buffer = spinand->pageBuf;

    if (!pData || !pSpare) {
        return HAL_INVAL;
    }

    ret = HAL_SPINAND_ReadPageRaw(spinand, addr, buffer, true);
    memcpy(pData, buffer, dataSize);
    pSpare[0] = buffer[(dataSize + meta->off0) / 4];
    pSpare[1] = buffer[(dataSize + meta->off1) / 4];

    if (secs == 8) {
        pSpare[2] = buffer[(dataSize + meta->off2) / 4];
        pSpare[3] = buffer[(dataSize + meta->off3) / 4];
    }

    return ret;
}

/**
 * @brief  SPI Nand page program.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @param  pData: source address.
 * @param  pSpare: destination spare address.
 * @return HAL_Status.
 */
HAL_Status HAL_SPINAND_ProgPage(struct SPI_NAND *spinand, uint32_t addr, const void *pData, const uint32_t *pSpare)
{
    int32_t ret;
    uint32_t secs = spinand->secPerPage;
    uint32_t dataSize = secs * SPINAND_SECTOR_SIZE;
    struct META_AREA *meta = &spinand->meta;
    uint32_t *buffer = spinand->pageBuf;

    if (!pData || !pSpare) {
        return HAL_INVAL;
    }

    memcpy(buffer, pData, dataSize);
    memset(&buffer[dataSize / 4], 0xff, secs * 16);
    buffer[(dataSize + meta->off0) / 4] = pSpare[0];
    buffer[(dataSize + meta->off1) / 4] = pSpare[1];

    if (secs == 8) {
        buffer[(dataSize + meta->off2) / 4] = pSpare[2];
        buffer[(dataSize + meta->off3) / 4] = pSpare[3];
    }

    ret = HAL_SPINAND_ProgPageRaw(spinand, addr, buffer, true);

    return ret;
}

/**
 * @brief  SPI Nand page read meta data only.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @param  pSpare: destination spare address.
 * @return return value < 0: transfer error, return HAL_Status;
 *   return value >= 0: transfer ok, return ecc status, check SPINAND_ECC_XXXX;
 */
int32_t HAL_SPINAND_ReadPageMeta(struct SPI_NAND *spinand, uint32_t addr, uint32_t *pSpare)
{
    uint32_t secs = spinand->secPerPage;
    uint32_t oobSize = secs * SPINAND_SECTOR_OOB_SIZE;
    uint32_t *buffer = spinand->pageBuf;
    struct META_AREA *meta = &spinand->meta;
    uint8_t status;
    uint32_t addrMix;

    HAL_SPINAND_DBG("%s %x\n", __func__, addr);

    if (!pSpare) {
        return HAL_INVAL;
    }

    SPINAND_LoadPageOp(spinand, addr);

    SPINAND_WaitBusy(spinand, &status, 1000 * 1000);

    addrMix = spinand->secPerPage * SPINAND_SECTOR_SIZE;
    addrMix |= spinand->planePerDie == 1 ? 0 : ((addr >> 6) & 0x1) << 12;
    SPINAND_ReadFromCacheOp(spinand, addrMix, buffer, oobSize);

    pSpare[0] = buffer[(meta->off0) / 4];
    pSpare[1] = buffer[(meta->off1) / 4];

    if (secs == 8) {
        pSpare[2] = buffer[(meta->off2) / 4];
        pSpare[3] = buffer[(meta->off3) / 4];
    }

    return spinand->eccStatus(spinand);
}

/**
 * @brief  SPI Nand page read with bytes offset .
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @param  pData: destination address.
 * @param  length: read bytes.
 * @param  offset: bytes offset within one page.
 * @return return value < 0: transfer error, return HAL_Status;
 *   return value >= 0: transfer ok, return ecc status, check SPINAND_ECC_XXXX;
 */
int32_t HAL_SPINAND_ReadPageAnyWhere(struct SPI_NAND *spinand, uint32_t addr, void *pData, uint32_t offset, uint32_t length)
{
    uint8_t status;
    int ret;
    uint32_t addrMix, size;

    HAL_SPINAND_DBG("%s 0x%x\n", __func__, addr);

    if (!pData) {
        return HAL_INVAL;
    }

    ret = SPINAND_LoadPageOp(spinand, addr);
    if (ret) {
        return ret;
    }

    ret = SPINAND_WaitBusy(spinand, &status, 1000 * 1000);
    if (ret < 0) {
        return ret;
    }

    addrMix = offset;
    addrMix |= spinand->planePerDie == 1 ? 0 : ((addr >> 6) & 0x1) << 12;
    size = length;
    ret = SPINAND_ReadFromCacheOp(spinand, addrMix, pData, size);
    if (ret) {
        return ret;
    }

    return spinand->eccStatus(spinand);
}

/**
 * @brief  SPI Nand block bad block marker.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @return HAL_Check.
 */
HAL_Check HAL_SPINAND_IsBad(struct SPI_NAND *spinand, uint32_t addr)
{
    uint8_t status;
    uint32_t addrMix;
    uint8_t marker[2] = { };

    HAL_SPINAND_DBG("%s %x\n", __func__, addr);

    SPINAND_LoadPageOp(spinand, addr);

    SPINAND_WaitBusy(spinand, &status, 1000 * 1000);

    addrMix = spinand->secPerPage * SPINAND_SECTOR_SIZE;
    addrMix |= spinand->planePerDie == 1 ? 0 : ((addr >> 6) & 0x1) << 12;
    SPINAND_ReadFromCacheOp(spinand, addrMix, (uint32_t *)marker, sizeof(marker));

    if (marker[0] != 0xff || marker[1] != 0xff) {
        return HAL_TRUE;
    }

    return HAL_FALSE;
}

/**
 * @brief  SPI Nand mark block bad marker.
 * @param  spinand: SPI_NAND dev.
 * @param  addr: page address.
 * @return HAL_Status.
 */
HAL_Status HAL_SPINAND_MarkBad(struct SPI_NAND *spinand, uint32_t addr)
{
    uint8_t status;
    int ret;
    uint32_t addrMix;
    uint8_t marker[2] = { 0, 0 };

    HAL_SPINAND_DBG("%s %x\n", __func__, addr);

    ret = SPINAND_WriteEnableOp(spinand);
    if (ret) {
        return ret;
    }

    addrMix = spinand->secPerPage * SPINAND_SECTOR_SIZE;
    addrMix |= spinand->planePerDie == 1 ? 0 : ((addr >> 6) & 0x1) << 12;
    ret = SPINAND_WriteToCacheOp(spinand, addrMix, (uint32_t *)marker, sizeof(marker));
    if (ret) {
        return ret;
    }

    ret = SPINAND_ProgOp(spinand, addr);
    if (ret) {
        return ret;
    }

    ret = SPINAND_WaitBusy(spinand, &status, 1000 * 1000);
    if (ret) {
        return ret;
    }

    if (status & SPINAND_PROG_FAIL) {
        return SPINAND_PROG_FAIL;
    }

    return ret;
}
/** @} */

/** @defgroup SPINAND_Exported_Functions_Group4 Init and DeInit Functions
 *  @{
 */

/**
 * @brief  SPI_NAND flash module init.
 * @param  spinand: SPI_NAND dev.
 * @return HAL_Status.
 */
HAL_Status HAL_SPINAND_Init(struct SPI_NAND *spinand)
{
    const struct SPINAND_INFO *info;
    uint8_t idByte[5], status;

    if (!spinand->spi) {
        HAL_SPINAND_DBG("%s no host\n", __func__);

        return HAL_INVAL;
    }

    HAL_SPINAND_ReadID(spinand, idByte);
    HAL_SPINAND_DBG("SPI Nand ID: %x %x %x\n", idByte[0], idByte[1], idByte[2]);

    if ((idByte[0] == 0xFF) || (idByte[0] == 0x00)) {
        return HAL_ERROR;
    }

    info = SPINAND_GerFlashInfo(idByte);
    if (!info) {
        return HAL_INVAL;
    }

    spinand->size = (1 << info->density) << 9;
    spinand->secPerPage = info->secPerPage;
    spinand->pagePerBlk = info->pagePerBlk;
    spinand->blkPerPlane = info->blkPerPlane;
    spinand->planePerDie = info->planePerDie;
    spinand->readLines = 1;
    spinand->progLines = 1;
    memcpy(&spinand->meta, &info->meta, sizeof(struct META_AREA));
    spinand->eccStatus = info->eccStatus;

    /* disable block lock */
    SPINAND_WriteStatus(spinand, 0XA0, 0);
    if (spinand->spi->mode & HAL_SPI_TX_QUAD &&
        info->feature & SPINAND_FEA_4BIT_READ) {
        if ((info->hasQeBit && SPINAND_QuadEnable(spinand) == HAL_OK) ||
            !info->hasQeBit) {
            spinand->readLines = 4;
        }
    }

    if (spinand->spi->mode & HAL_SPI_TX_QUAD &&
        info->feature & SPINAND_FEA_4BIT_PROG &&
        spinand->readLines == 4) {
        spinand->progLines = 4;
    }

    SPINAND_ReadStatus(spinand, 0xA0, &status);
    HAL_SPINAND_DBG("SPI Nand A0 = 0x%x\n", status);
    SPINAND_ReadStatus(spinand, 0xB0, &status);
    HAL_SPINAND_DBG("SPI Nand B0 = 0x%x\n", status);
    HAL_SPINAND_DBG("readLines = %x\n", spinand->readLines);
    HAL_SPINAND_DBG("progLines = %x\n", spinand->progLines);

    return HAL_OK;
}

/**
 * @brief  SPI_NAND flash module deinit.
 * @param  spinand: SPI_NAND dev.
 * @return HAL_Status.
 */
HAL_Status HAL_SPINAND_DeInit(struct SPI_NAND *spinand)
{
    return HAL_OK;
}

/** @} */

/** @defgroup SPINAND_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Read flash ID.
 * @param  spinand: SPI_NAND dev.
 * @param  data: address to storage flash ID value.
 * @return HAL_Status.
 */
HAL_Status HAL_SPINAND_ReadID(struct SPI_NAND *spinand, uint8_t *data)
{
    struct HAL_SPI_MEM_OP op = SPINAND_READID_OP(1, data,
                                                 SPINAND_MAX_ID_LEN);

    return SPINAND_SPIMemExecOp(spinand->spi, &op);
}

/**
 * @brief  Get flash capacity.
 * @param  spinand: SPI_NAND dev.
 * @return uint32_t: flash capacity, n bytes.
 */
uint32_t HAL_SPINAND_GetCapacity(struct SPI_NAND *spinand)
{
    return spinand->size;
}

/**
 * @brief  Check if the flash support.
 * @param  flashId: flash id.
 * @return HAL_Check.
 */
HAL_Check HAL_SPINAND_IsFlashSupported(uint8_t *flashId)
{
    uint32_t i;

    if (!flashId) {
        return HAL_FALSE;
    }

    for (i = 0; i < HAL_ARRAY_SIZE(s_spiNandTable); i++) {
        if (s_spiNandTable[i].id0 == flashId[0] &&
            s_spiNandTable[i].id1 == flashId[1]) {
            if (s_spiNandTable[i].id2 &&
                s_spiNandTable[i].id2 != flashId[2]) {
                continue;
            }

            return HAL_TRUE;
        }
    }

    return HAL_FALSE;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_SPINAND_MODULE_ENABLED */
