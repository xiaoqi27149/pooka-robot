/*
 *  Copyright (c) 2024 Rockchip Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllexport))
#else
	// Note: actually gcc seems to also supports this syntax.
#define DLL_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllimport))
#else
	// Note: actually gcc seems to also supports this syntax.
#define DLL_PUBLIC
#endif
#define DLL_LOCAL
#endif
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif
 /**
 * @brief DMA buffer descriptor for direct memory access operations
 *   Manages low-level memory buffers compatible with DRM subsystem
 *   Supports zero-copy data transfer between hardware modules
 */
typedef struct rkALG_DMA_BUFFER_S
{
    uint32_t height;
    uint32_t width;
    uint32_t pitch;
    uint32_t handle;
    int32_t dmabuf_fd;
    uint64_t size;
    void* virAddr;
}ALG_DMA_BUFFER_S;

/**
* @brief Allocate DRM-backed DMA buffer with specific parameters
* @param in_w    Requested buffer width (pixels)
* @param in_h    Requested buffer height (pixels)
* @param in_bpp  Bits-per-pixel
* @param buf     Pre-allocated buffer descriptor to initialize
* @return 0 success, non-zero indicates error:
*/
DLL_PUBLIC int32_t rkALG_allocDrmBuffer(int32_t in_w, int32_t in_h, int32_t in_bpp, ALG_DMA_BUFFER_S* buf);

/**
* @brief Release DRM buffer resources‌
* @param pDrmBuf Buffer descriptor to deallocate
* @return 0 success, non-zero indicates:
*/
DLL_PUBLIC int32_t rkALG_freeDrmBuffer(ALG_DMA_BUFFER_S* pDrmBuf);

#ifdef __cplusplus
} /* extern "C" { */
#endif