/**
 * Copyright     : Copyright(c) 2024 Rockchip. All right reserved.
 * FileName      : rkalg_idc_dma_buffer.h
 * Creacted By   : cjk
 * Creacted Date : 2024/05/29
 * Update Date   : 2025/02/25
 * Description   : DMA buffer header file for RK IDC
 */

#ifndef _RKALG_IDC_DMA_BUFFER_H_
#define _RKALG_IDC_DMA_BUFFER_H_

typedef struct DmaBufferInfo_t {
    int         height;
    int         width;
    int         pitch;
    size_t      size;
    void*       ptr;
    uint32_t    hnd;
    int         fd;
    int         index;
} DmaBufferInfo;

#ifndef WIN32
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include "drm/drm_fourcc.h"

#define drm_munmap(addr, length) \
              munmap(addr, length)


#define drm_dbg(flag, fmt, ...)
static int drmIoctl(int fd, int req, void* arg)
{
    int ret;

    do {
        ret = ioctl(fd, req, arg);
    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

    drm_dbg(DRM_FUNCTION, "drm_ioctl %x with code %d: %s", req,
        ret, strerror(errno));

    return ret;
}
#undef drm_dbg

static int drm_free(int fd, uint32_t handle)
{
    struct drm_mode_destroy_dumb destory_arg;
    memset(&destory_arg, 0, sizeof(destory_arg));
    destory_arg.handle = handle;
    int ret = drmIoctl(fd, (int)DRM_IOCTL_MODE_DESTROY_DUMB, (void*)&destory_arg);
    if (ret) {
        printf("failed to destory dumb %d, %s\n", ret, strerror(errno));
        return -1;
    }
    return 0;
}

/* bpp: bits per point. 
 *      NV12: 12(8 * 1.5), 
 *      merged float mesh: 8 * sizeof(float) * 2.
 * return: virtual address of buffer
 */
static void* alloc_drm_buf(DmaBufferInfo* pBufInfo, int in_w, int in_h, int in_bpp)
{
    static const char* card = "/dev/dri/card0"; 
    int flag = O_RDWR; 
    int drm_fd = -1; 
    int ret;
    void* vir_addr = NULL; 

    struct drm_mode_create_dumb alloc_arg;
    struct drm_prime_handle fd_args;
    struct drm_mode_map_dumb mmap_arg;

    /* open DRM device */
    drm_fd = open(card, flag);
    if (drm_fd < 0) {
        printf("failed to open %s\n", card);
        return NULL;
    }

    /* alloc dumb buffer */
    memset(&alloc_arg, 0, sizeof(alloc_arg));
    alloc_arg.bpp = in_bpp;
    alloc_arg.width = in_w;
    alloc_arg.height = in_h;
    ret = drmIoctl(drm_fd, (int)DRM_IOCTL_MODE_CREATE_DUMB, (void*)&alloc_arg); 
    if (ret) {
        printf("failed to create dumb buffer: %s\n", strerror(errno));
        return NULL;
    }

    /* get fd */
    memset(&fd_args, 0, sizeof(fd_args));
    fd_args.fd = -1;
    fd_args.handle = alloc_arg.handle;
    fd_args.flags = 0;
    ret = drmIoctl(drm_fd, (int)DRM_IOCTL_PRIME_HANDLE_TO_FD, (void*)&fd_args);
    if (ret) {
        printf("handle_to_fd failed ret=%d,err=%s, handle=%x \n", ret, strerror(errno), fd_args.handle);
        return NULL;
    }

    memset(&mmap_arg, 0, sizeof(mmap_arg));
    mmap_arg.handle = alloc_arg.handle;
    ret = drmIoctl(drm_fd, (int)DRM_IOCTL_MODE_MAP_DUMB, (void*)&mmap_arg);
    if (ret) {
        printf("failed to create map dumb: %s\n", strerror(errno));
        vir_addr = NULL;
    }

    /* map virtual address */
    vir_addr = mmap64(0, alloc_arg.size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd, mmap_arg.offset);
    if (vir_addr == MAP_FAILED) {
        printf("failed to mmap buffer: %s\n", strerror(errno));
        vir_addr = NULL;
    }

    // free handle
    drm_free(drm_fd, fd_args.handle);

    // close
    close(drm_fd);

    pBufInfo->height = alloc_arg.height;
    pBufInfo->width = alloc_arg.width;
    pBufInfo->pitch = alloc_arg.pitch;
    pBufInfo->size = alloc_arg.size;
    pBufInfo->ptr = vir_addr;
    pBufInfo->hnd = fd_args.handle;
    pBufInfo->fd = fd_args.fd;
    // pBufInfo->index = 0;

    return vir_addr;
}

#endif // !WIN32

#endif // _RKALG_IDC_DMA_BUFFER_H_