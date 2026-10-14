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

#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <string>

#include <stdio.h>
#include <sys/poll.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>

#include <opencv2/opencv.hpp> 
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

extern "C" {
#include "rkros_rockit/rk_defines.h"
#include "rkros_rockit/rk_debug.h"
#include "rkros_rockit/rk_mpi_vi.h"
#include "rkros_rockit/rk_mpi_mb.h"
#include "rkros_rockit/rk_mpi_sys.h"

#include "rkros_rockit/rk_common.h"
#include "rkros_rockit/rk_comm_vi.h"

}

typedef enum rkTestVIMODE_E {
    TEST_VI_MODE_VI_FRAME_ONLY = 0,
    TEST_VI_MODE_BIND_VENC = 1,
    TEST_VI_MODE_BIND_VENC_MULTI = 2,
    TEST_VI_MODE_BIND_VPSS_BIND_VENC = 3,
    TEST_VI_MODE_BIND_VO = 4,
    TEST_VI_MODE_MUTI_VI = 5,
    TEST_VI_MODE_VI_STREAM_ONLY = 6,
    TEST_VI_MODE_BIND_VDEC_BIND_VO = 7,
} TEST_VI_MODE_E;

typedef struct _rkMpiVICtx {
    RK_S32 width;
    RK_S32 height;
    RK_S32 devId;
    RK_S32 pipeId;
    RK_S32 channelId;
    RK_S32 loopCountSet;
    RK_S32 selectFd;
    RK_BOOL bFreeze;
    RK_BOOL bEnRgn;
    RK_S32 s32RgnCnt;
    RK_S32 rgnType;
    RK_BOOL bUserPicEnabled;
    RK_BOOL bGetConnecInfo;
    RK_BOOL bGetEdid;
    RK_BOOL bSetEdid;
    RK_BOOL bNoUseLibv4l2;
    RK_BOOL bEnSwcac;
    COMPRESS_MODE_E enCompressMode;
    VI_DEV_ATTR_S stDevAttr;
    VI_DEV_BIND_PIPE_S stBindPipe;
    VI_CHN_ATTR_S stChnAttr;
    VI_SAVE_FILE_INFO_S stDebugFile;
    VIDEO_FRAME_INFO_S stViFrame;
    VI_CHN_STATUS_S stChnStatus;
    VI_USERPIC_ATTR_S stUsrPic;
    TEST_VI_MODE_E enMode;
    const char *aEntityName;

    VO_LAYER s32VoLayer;
    VO_DEV s32VoDev;
    // for stream
    RK_CODEC_ID_E enCodecId;
} TEST_VI_CTX_S;

class RockitCapture {
public:
    using FrameCallback = std::function<void(const void* data, size_t len, int width, int height, int64_t pts)>;

    RockitCapture();
    ~RockitCapture();

    bool init();
    void start(FrameCallback cb);
    void stop();
    void captureLoop();
    void processLoop();

    void configure(
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
        int region_count);

private:
    RK_S32 rk_vi_init(TEST_VI_CTX_S *ctx);

    TEST_VI_CTX_S ctx_;

    std::thread captureThread_;
    std::atomic<bool> running_{false};
    FrameCallback frameCallback_;
};
