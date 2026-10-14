/*
 * Copyright (C) 2025 Rockchip Electronics Co., Ltd.
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
 */

#include "rockit_capture.h"

void RockitCapture::configure(
    int devId,
    int pipeId,
    int channelId,
    int width,
    int height,
    int loop_count,
    int buffer_count,
    rkPIXEL_FORMAT_E pixel_format,
    rkVI_V4L2_MEMORY_TYPE memory_type,
    rkVI_V4L2_CAPTURE_TYPE capture_type,
    bool enable_region,
    int region_count
) {
    ctx_.devId = devId;
    ctx_.pipeId = pipeId;
    ctx_.channelId = channelId;
    ctx_.width = width;
    ctx_.height = height;
    ctx_.loopCountSet = loop_count;
    ctx_.enMode = TEST_VI_MODE_VI_FRAME_ONLY; // 可选：也可参数化
    ctx_.stChnAttr.stIspOpt.u32BufCount = buffer_count;
    ctx_.stChnAttr.stIspOpt.enMemoryType = memory_type;
    ctx_.stChnAttr.stIspOpt.enCaptureType = capture_type;
    ctx_.stChnAttr.u32Depth = 0; // 可选：也可参数化
    ctx_.aEntityName = nullptr;  // 可选：也可参数化
    ctx_.stChnAttr.enPixelFormat = pixel_format;
    ctx_.stChnAttr.stFrameRate.s32SrcFrameRate = -1; // 保持默认
    ctx_.stChnAttr.stFrameRate.s32DstFrameRate = -1; // 保持默认
    ctx_.bEnRgn = enable_region ? RK_TRUE : RK_FALSE;
    ctx_.s32RgnCnt = region_count;

    ctx_.stDebugFile.bCfg = RK_FALSE; // 不保存调试文件
    RK_LOGE("RockitCapture created for devId=%d, pipeId=%d, channelId=%d", devId, pipeId, channelId);


    if (ctx_.stDebugFile.bCfg) {
        memcpy(ctx_.stDebugFile.aFilePath, "/data", strlen("/data"));
        snprintf(ctx_.stDebugFile.aFileName, MAX_VI_FILE_PATH_LEN,
                 "test_%d_%d_%d.bin", ctx_.devId, ctx_.pipeId, ctx_.channelId);
        RK_LOGE("Debug file will be saved to: %s/%s", ctx_.stDebugFile.aFilePath, ctx_.stDebugFile.aFileName);
    }

    if (ctx_.aEntityName != nullptr) {
        memcpy(ctx_.stChnAttr.stIspOpt.aEntityName, ctx_.aEntityName, strlen(ctx_.aEntityName));
    }

    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        throw std::runtime_error("RK_MPI_SYS_Init failed");
    }

    if (!ctx_.stChnAttr.u32Depth) {
        RK_LOGE("depth need > 0 when vi not bind any other module!");
        ctx_.stChnAttr.u32Depth = ctx_.stChnAttr.stIspOpt.u32BufCount;
    }

}

RockitCapture::RockitCapture() {
    memset(&ctx_, 0, sizeof(ctx_));
}

RockitCapture::~RockitCapture() {
    stop();
    RK_MPI_VI_DisableChn(ctx_.pipeId, ctx_.channelId);
    RK_MPI_VI_DisableDev(ctx_.devId);

    RK_MPI_SYS_Exit();
    RK_LOGE("RockitCapture destroyed");
}

bool RockitCapture::init() {
    RK_S32 ret = rk_vi_init(&ctx_); // 你已有的初始化函数
    if (ret != RK_SUCCESS) {
        std::cerr << "VI init failed: " << std::hex << ret << std::endl;
        return false;
    }
    return true;
}

void RockitCapture::start(FrameCallback cb) {
    frameCallback_ = cb;
    running_ = true;
    captureThread_ = std::thread(&RockitCapture::captureLoop, this);
}

void RockitCapture::stop() {
    running_ = false;
    if (captureThread_.joinable()) {
        captureThread_.join();
    }
}

void RockitCapture::captureLoop() {
    while (running_) {
        RK_LOGD("Attempting to get one frame...");
        RK_S32 ret = RK_MPI_VI_GetChnFrame(ctx_.pipeId, ctx_.channelId, &ctx_.stViFrame, 33);
        if (ret == RK_SUCCESS) {
            void* data = RK_MPI_MB_Handle2VirAddr(ctx_.stViFrame.stVFrame.pMbBlk);
            size_t len = RK_MPI_MB_GetLength(ctx_.stViFrame.stVFrame.pMbBlk);
            RK_LOGD("Got VI Frame: %dx%d, PTS: %lu, Size: %zu",
                     ctx_.stViFrame.stVFrame.u32Width,
                     ctx_.stViFrame.stVFrame.u32Height,
                     ctx_.stViFrame.stVFrame.u64PTS,
                     len);
            if (frameCallback_) {
                RK_LOGD("Calling frame callback with data %p, len %zu", data, len);
                frameCallback_(data, len,
                               ctx_.stViFrame.stVFrame.u32Width,
                               ctx_.stViFrame.stVFrame.u32Height,
                               ctx_.stViFrame.stVFrame.u64PTS);
            }

            ret = RK_MPI_VI_ReleaseChnFrame(ctx_.pipeId, ctx_.channelId, &ctx_.stViFrame);
            if (ret != RK_SUCCESS) {
                RK_LOGE("RK_MPI_VI_ReleaseChnFrame fail %x", ret);
            }
        } else {
            usleep(5 * 1000); // 避免空转
        }
    }

}

RK_S32 RockitCapture::rk_vi_init(TEST_VI_CTX_S *ctx) {
    RK_S32 s32Ret = RK_FAILURE;

    // 0. get dev config status
    s32Ret = RK_MPI_VI_GetDevAttr(ctx->devId, &ctx->stDevAttr);
    if (s32Ret == RK_ERR_VI_NOT_CONFIG) {
        // 0-1.config dev
        s32Ret = RK_MPI_VI_SetDevAttr(ctx->devId, &ctx->stDevAttr);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("RK_MPI_VI_SetDevAttr %x", s32Ret);
            goto __FAILED;
        }
    } else {
        RK_LOGE("RK_MPI_VI_SetDevAttr already");
    }
    // 1.get  dev enable status
    s32Ret = RK_MPI_VI_GetDevIsEnable(ctx->devId);
    if (s32Ret != RK_SUCCESS) {
        // 1-2.enable dev
        s32Ret = RK_MPI_VI_EnableDev(ctx->devId);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("RK_MPI_VI_EnableDev %x", s32Ret);
            goto __FAILED;
        }
        // 1-3.bind dev/pipe
        ctx->stBindPipe.u32Num = ctx->pipeId;
        ctx->stBindPipe.PipeId[0] = ctx->pipeId;
        //ctx->stBindPipe.u32Num = 0;
        //ctx->stBindPipe.PipeId[0] = 1;
        s32Ret = RK_MPI_VI_SetDevBindPipe(ctx->devId, &ctx->stBindPipe);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("RK_MPI_VI_SetDevBindPipe %x", s32Ret);
            goto __FAILED;
        }
    } else {
        RK_LOGE("RK_MPI_VI_EnableDev already");
    }
    // 2.config channel
    ctx->bNoUseLibv4l2 = RK_FALSE;
    ctx->enCompressMode = COMPRESS_MODE_NONE;
    ctx->stChnAttr.stSize.u32Width = ctx->width;
    ctx->stChnAttr.stSize.u32Height = ctx->height;
    ctx->stChnAttr.enCompressMode = ctx->enCompressMode;
    ctx->stChnAttr.stIspOpt.bNoUseLibV4L2 = ctx->bNoUseLibv4l2;
    s32Ret = RK_MPI_VI_SetChnAttr(ctx->pipeId, ctx->channelId, &ctx->stChnAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VI_SetChnAttr %x", s32Ret);
        goto __FAILED;
    }

    // 3.enable channel
    s32Ret = RK_MPI_VI_EnableChn(ctx->pipeId, ctx->channelId);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VI_EnableChn %x", s32Ret);
        goto __FAILED;
    }

    // 4.save debug file
    if (ctx->stDebugFile.bCfg) {
        s32Ret = RK_MPI_VI_ChnSaveFile(ctx->pipeId, ctx->channelId, &ctx->stDebugFile);
        RK_LOGD("RK_MPI_VI_ChnSaveFile %x", s32Ret);
    }

__FAILED:
    return s32Ret;
}
