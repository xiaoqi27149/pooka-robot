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

#ifndef ROCKIT_ROUTELINE_HPP_
#define ROCKIT_ROUTELINE_HPP_

#include <mutex>
#include <condition_variable>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

#include "rtsp_demo.h"
#include "rkros_rockit/rk_debug.h"
#include "rkros_rockit/rk_defines.h"
#include "rkros_rockit/rk_mpi_adec.h"
#include "rkros_rockit/rk_mpi_aenc.h"
#include "rkros_rockit/rk_mpi_ai.h"
#include "rkros_rockit/rk_mpi_ao.h"
#include "rkros_rockit/rk_mpi_avs.h"
#include "rkros_rockit/rk_mpi_cal.h"
#include "rkros_rockit/rk_mpi_ivs.h"
#include "rkros_rockit/rk_mpi_mb.h"
#include "rkros_rockit/rk_mpi_rgn.h"
#include "rkros_rockit/rk_mpi_sys.h"
#include "rkros_rockit/rk_mpi_tde.h"
#include "rkros_rockit/rk_mpi_vdec.h"
#include "rkros_rockit/rk_mpi_venc.h"
#include "rkros_rockit/rk_mpi_vi.h"
#include "rkros_rockit/rk_mpi_vo.h"
#include "rkros_rockit/rk_mpi_vpss.h"

extern bool quit;
extern RK_S32 g_s32FrameCnt;
extern VENC_STREAM_S latest_frame;
extern rtsp_demo_handle g_rtsplive;
extern rtsp_session_handle g_rtsp_session;
extern rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;

RK_U64 TEST_COMM_GetNowUs();

// Rockit相关初始化和处理函数
int vi_dev_init();
int vi_chn_init(int channelId, int width, int height);
int test_vpss_init(int VpssChn, int width, int height);
RK_S32 test_venc_init(int chnId, int width, int height, RK_CODEC_ID_E enType);
void process_video(RK_U32 u32Width, RK_U32 u32Height, RK_U32 disp_width,
                   RK_U32 disp_height, RK_S32 s32chnlId, RK_CODEC_ID_E enCodecType,
                   rclcpp::Node* node, bool enable_publish_encoder);
void get_encoded_frames(rclcpp::Node* node, bool enable_publish_encoder);

#endif // ROCKIT_ROUTELINE_HPP_
