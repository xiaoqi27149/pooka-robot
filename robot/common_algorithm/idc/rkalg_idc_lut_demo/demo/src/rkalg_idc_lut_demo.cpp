/*
 * @Copyright    : Copyright(c) 2024 Rockchip. All right reserved.
 * @Filename     : rkalg_idc_lut_demo.cpp
 * @Creacted By  : cjk
 * @Creacted Date: 2024/06/05
 * @Modify Date  : 2025/01/08
 * @Description  : Demo for LUT App
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rkalg_idc_lut_api.h"
#include "rkalg_idc_dma_buffer.h"

#define TMP_DATA_PATH "/data/"

int alignUp(int x, int y) {
    return ((x + y - 1) / y) * y;
}

int readBinaryFile(const char* filename, void* pData, size_t targetSize) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Failed to open file '%s'\n", filename);
        return -1;
    }
    if (targetSize == 0) {
        fseek(fp, 0, SEEK_END);     /* move fp to end of file */
        targetSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);     /* move fp back to beginning of file */
    }
    size_t readSize = fread(pData, sizeof(unsigned char), targetSize, fp);
    if (readSize != targetSize) {
        printf("read size (%zu) != target size (%zu). The file '%s' might be incomplete!\n", readSize, targetSize, filename);
    }
    fclose(fp);
    return 0;
}

void test()
{
    int ret = 0;
    printf("test begin.\n");

    char version[128];
    RKALG_IDC_LUT_GetVersion(version);
    printf("%s\n", version);

    /* Step 1-1. Config initial params */
    RKALG_LUT_INIT_PARAMS_S stInitParams;
    stInitParams.u32SrcWidth  = 1920;
    stInitParams.u32SrcHeight = 1080;
    stInitParams.u32SrcStride = alignUp(stInitParams.u32SrcWidth, 64);
    stInitParams.u32SrcHgtStride = alignUp(stInitParams.u32SrcHeight, 2); 
    stInitParams.u32DstWidth  = 1920;
    stInitParams.u32DstHeight = 1080;
    stInitParams.u32DstStride = alignUp(stInitParams.u32DstWidth, 16);
    stInitParams.u32DstHgtStride = alignUp(stInitParams.u32DstHeight, 8);
    stInitParams.eMode        = RKALG_IDC_LUT_DEFAULT_MODE;

    char dst_img_path[256];
    char src_img_path[256];
    sprintf(src_img_path, TMP_DATA_PATH"input_%dx%d_nv12.yuv", stInitParams.u32SrcWidth, stInitParams.u32SrcHeight);
    sprintf(dst_img_path, TMP_DATA_PATH"output_%dx%d_nv12.yuv", stInitParams.u32DstWidth, stInitParams.u32DstHeight);
    char meshxy_path[] = TMP_DATA_PATH"/rkalg_idc_gpu_meshxy_step16x8.bin";

    /* Step 1-2. Create LUT context */
    RKALG_LUT_CTX_S stLutCtx;
    RKALG_IDC_LUT_Init(&stLutCtx, &stInitParams);
    
    /* Step 2-1. malloc DMA Buffer for srcImg and read data */
    RKALG_IDC_IMAGE_S srcImage;
    memset(&srcImage, 0, sizeof(RKALG_IDC_IMAGE_S));
    srcImage.eImgFmt         = RKALG_IDC_IMG_FMT_NV12; // only NV12 supported now
    srcImage.u32Width        = stInitParams.u32SrcWidth;
    srcImage.u32Height       = stInitParams.u32SrcHeight;
    srcImage.u32Stride[0]    = stInitParams.u32SrcStride;
    srcImage.u32HgtStride[0] = stInitParams.u32SrcHgtStride;
    
    DmaBufferInfo dmaBufSrc;
    memset(&dmaBufSrc, 0, sizeof(DmaBufferInfo));
    int bpp_nv12 = 12 * sizeof(unsigned char);
    alloc_drm_buf(&dmaBufSrc, srcImage.u32Stride[0], srcImage.u32HgtStride[0], bpp_nv12);
    srcImage.fd[0]      = dmaBufSrc.fd;
    srcImage.virAddr[0] = dmaBufSrc.ptr;

    size_t srcImgSize = srcImage.u32Width * srcImage.u32Height * 3 / 2 * sizeof(unsigned char);
    ret |= readBinaryFile(src_img_path, srcImage.virAddr[0], srcImgSize);
    if (ret != 0) {
        printf("read src image failed.\n");
        return;
    }

    /* Step 2-2. malloc DMA Buffer for dstImg and config */
    RKALG_IDC_IMAGE_S dstImage;
    memset(&dstImage, 0, sizeof(RKALG_IDC_IMAGE_S));
    dstImage.eImgFmt         = RKALG_IDC_IMG_FMT_NV12; // Only support NV12 now
    dstImage.u32Width        = stInitParams.u32DstWidth;
    dstImage.u32Height       = stInitParams.u32DstHeight;
    dstImage.u32Stride[0]    = stInitParams.u32DstStride;
    dstImage.u32HgtStride[0] = stInitParams.u32DstHgtStride;

    DmaBufferInfo dmaBufDst;
    memset(&dmaBufDst, 0, sizeof(DmaBufferInfo));
    alloc_drm_buf(&dmaBufDst, dstImage.u32Stride[0], dstImage.u32HgtStride[0], bpp_nv12);
    dstImage.fd[0]      = dmaBufDst.fd;
    dstImage.virAddr[0] = dmaBufDst.ptr;

    /* Step 2-3. malloc DMA Buffer for Xy mesh and read data */
    RKALG_IDC_MESH_S mesh;
    memset(&mesh, 0, sizeof(RKALG_IDC_MESH_S));
    mesh.u32StepX     = 16;
    mesh.u32StepY     = 8;
    mesh.u32Width     = (dstImage.u32Stride[0] - 1) / mesh.u32StepX + 2;
    mesh.u32Height    = (dstImage.u32Height - 1) / mesh.u32StepY + 2;
    mesh.u32Stride    = mesh.u32Width;
    mesh.u32HgtStride = mesh.u32Height;
    mesh.eMeshType    = RKALG_IDC_MESH_TYPE_MERGED; // only merged mesh supported now

    DmaBufferInfo dmaBufMesh;
    memset(&dmaBufMesh, 0, sizeof(DmaBufferInfo));
    size_t bpp_mesh_merged = 8 * sizeof(float) * 2; 
    alloc_drm_buf(&dmaBufMesh, mesh.u32Width, mesh.u32Height, bpp_mesh_merged);
    mesh.fd[0]      = dmaBufMesh.fd;
    mesh.virAddr[0] = dmaBufMesh.ptr;
    size_t meshSize = mesh.u32Width * mesh.u32Height * sizeof(float) * 2;
	ret |= readBinaryFile(meshxy_path, mesh.virAddr[0], meshSize);
    if (ret != 0) {
        printf("read mesh data failed.\n");
        return ;
    }

    /* Step 2-4. Config Op Attr. Skip this step if mode is RKALG_IDC_LUT_DEFAULT_MODE */
    RKALG_LUT_OP_ATTR_S stOpAttr;
    memset(&stOpAttr, 0, sizeof(RKALG_LUT_OP_ATTR_S));

    /* Step 2-5. Call doLut API */
    RKALG_LUT_TASK_S stTask;
    stTask.pSrcImage = &srcImage;
    stTask.pDstImage = &dstImage;
    stTask.pMesh     = &mesh;
    stTask.pOpAttr   = &stOpAttr; // only take effect when mode is RKALG_IDC_LUT_MANUAL_MODE.
    RKALG_IDC_LUT_DoLut(&stLutCtx, &stTask);

    /* Step 2-6. Write dstImg to file */
    FILE *fp_out = fopen(dst_img_path, "wb");
    if (fp_out == NULL) {
        printf("Can not open %s\n", dst_img_path);
        return;
    }
    unsigned char* pt     = NULL;
    unsigned char* pDstY  = (unsigned char*)dmaBufDst.ptr;
    unsigned char* pDstUV = pDstY + dstImage.u32Stride[0] * dstImage.u32HgtStride[0];

    for (size_t row_idx = 0; row_idx < dstImage.u32Height; row_idx++) {
        pt = pDstY + row_idx * dstImage.u32Stride[0];
		fwrite(pt, sizeof(unsigned char), dstImage.u32Width, fp_out);
	}
    for (size_t row_idx = 0; row_idx < dstImage.u32Height / 2; row_idx++) {
        pt = pDstUV + row_idx * dstImage.u32Stride[0];
        fwrite(pt, sizeof(unsigned char), dstImage.u32Width, fp_out);
    }
    fclose(fp_out);
    printf("write %s success.\n", dst_img_path);

    /* Step 3. Call deInit API */
    RKALG_IDC_LUT_Deinit(&stLutCtx);
    if (dmaBufMesh.ptr != nullptr) {
        drm_munmap(dmaBufMesh.ptr, dmaBufMesh.size);
        dmaBufMesh.ptr = nullptr;
    }
    close(dmaBufMesh.fd);
    if (dmaBufSrc.ptr != nullptr) {
		drm_munmap(dmaBufSrc.ptr, dmaBufSrc.size);
		dmaBufSrc.ptr = nullptr;
	}
	close(dmaBufSrc.fd);
	if (dmaBufDst.ptr != nullptr) {
		drm_munmap(dmaBufDst.ptr, dmaBufDst.size);
		dmaBufDst.ptr = nullptr;
	}
	close(dmaBufDst.fd);

    printf("test end.\n");
}

int main()
{
    test();
    return 0;
}