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

#include <unistd.h>
#include <thread>

#include "rockit_routeline.hpp"

bool quit = false;
RK_S32 g_s32FrameCnt = -1;
rtsp_demo_handle g_rtsplive = NULL;
rtsp_session_handle g_rtsp_session;

RK_U64 TEST_COMM_GetNowUs() {
    struct timespec time = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (RK_U64)time.tv_sec * 1000000 + (RK_U64)time.tv_nsec / 1000; /* microseconds */
}

RK_S32 test_venc_init(int chnId, int width, int height, RK_CODEC_ID_E enType) {
    printf("================================%s==================================\n", __func__);
    RK_S32 s32Ret;
    VENC_RC_MODE_E enRcMode;
    VENC_RECV_PIC_PARAM_S stRecvParam;
    VENC_CHN_ATTR_S stAttr;
    memset(&stAttr, 0, sizeof(VENC_CHN_ATTR_S));

    if (enType == RK_VIDEO_ID_AVC) {
        enRcMode = VENC_RC_MODE_H264CBR;
    } else if (enType == RK_VIDEO_ID_HEVC) {
         enRcMode = VENC_RC_MODE_H265CBR;
    } else if (enType == RK_VIDEO_ID_MJPEG) {
        enRcMode = VENC_RC_MODE_MJPEGCBR;
    } else {
        printf("not support type\n");
        return -1;
    }

    stAttr.stVencAttr.enType = enType;
    stAttr.stVencAttr.enPixelFormat = RK_FMT_YUV420SP;
    stAttr.stRcAttr.enRcMode = enRcMode;
    stAttr.stRcAttr.stH264Cbr.u32BitRate = 10 * 1024;
    stAttr.stRcAttr.stH264Cbr.u32Gop = 60;
    stAttr.stVencAttr.u32PicWidth = width;
    stAttr.stVencAttr.u32PicHeight = height;
    stAttr.stVencAttr.u32VirWidth = width;
    stAttr.stVencAttr.u32VirHeight = height;
    stAttr.stVencAttr.u32StreamBufCnt = 2;
    stAttr.stVencAttr.u32BufSize = width * height * 3 / 2;

    s32Ret = RK_MPI_VENC_CreateChn(chnId, &stAttr);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VENC_CreateChn failed[%x]\n", s32Ret);
    }

    memset(&stRecvParam, 0, sizeof(VENC_RECV_PIC_PARAM_S));
    stRecvParam.s32RecvPicNum = -1;
    RK_MPI_VENC_StartRecvFrame(chnId, &stRecvParam);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VENC_StartRecvFrame failed[%x]\n", s32Ret);
    }

    return 0;
}

int vi_dev_init() {
    printf("%s\n", __func__);
    int ret = 0;
    int devId = 0;
    int pipeId = devId;

    VI_DEV_ATTR_S stDevAttr;
    VI_DEV_BIND_PIPE_S stBindPipe;
    memset(&stDevAttr, 0, sizeof(stDevAttr));
    memset(&stBindPipe, 0, sizeof(stBindPipe));
    // 0. get dev config status
    ret = RK_MPI_VI_GetDevAttr(devId, &stDevAttr);
    if (ret == RK_ERR_VI_NOT_CONFIG) {
        // 0-1.config dev
        ret = RK_MPI_VI_SetDevAttr(devId, &stDevAttr);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevAttr %x\n", ret);
            return -1;
        }
    } else {
        printf("RK_MPI_VI_SetDevAttr already\n");
    }
    // 1.get dev enable status
    ret = RK_MPI_VI_GetDevIsEnable(devId);
    if (ret != RK_SUCCESS) {
        // 1-2.enable dev
        ret = RK_MPI_VI_EnableDev(devId);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_EnableDev %x\n", ret);
            return -1;
        }
        // 1-3.bind dev/pipe
        stBindPipe.u32Num = 1;
        stBindPipe.PipeId[0] = pipeId;
        ret = RK_MPI_VI_SetDevBindPipe(devId, &stBindPipe);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevBindPipe %x\n", ret);
            return -1;
        }
    } else {
        printf("RK_MPI_VI_EnableDev already\n");
    }

    return 0;
}

int vi_chn_init(int channelId, int width, int height) {
    printf("================================%s==================================\n", __func__);
    int ret;
    int buf_cnt = 2;
    // VI init
    VI_CHN_ATTR_S vi_chn_attr;
    memset(&vi_chn_attr, 0, sizeof(vi_chn_attr));
    vi_chn_attr.stIspOpt.u32BufCount = buf_cnt;
    vi_chn_attr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
    // VI_V4L2_MEMORY_TYPE_MMAP;
    vi_chn_attr.stSize.u32Width = width;
    vi_chn_attr.stSize.u32Height = height;
    vi_chn_attr.enPixelFormat = RK_FMT_YUV420SP;
    vi_chn_attr.enCompressMode = COMPRESS_MODE_NONE; // COMPRESS_AFBC_16x16;
    vi_chn_attr.u32Depth = 0; //0, get fail, 1 - u32BufCount, can get, if bind to other device, must be < u32BufCount
    ret = RK_MPI_VI_SetChnAttr(0, channelId, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(0, channelId);
    if (ret) {
        printf("ERROR: create VI error! ret=%d\n", ret);
        return ret;
    }

    return ret;
}

int test_vpss_init(int VpssChn, int width, int height) {
    printf("================================%s==================================\n", __func__);
    int s32Ret;
    VPSS_CHN_ATTR_S stVpssChnAttr;
    VPSS_GRP_ATTR_S stGrpVpssAttr;
    int s32Grp = 0;

    stGrpVpssAttr.u32MaxW = 4096;
    stGrpVpssAttr.u32MaxH = 4096;
    stGrpVpssAttr.enPixelFormat = RK_FMT_YUV420SP;
    stGrpVpssAttr.stFrameRate.s32SrcFrameRate = -1;
    stGrpVpssAttr.stFrameRate.s32DstFrameRate = -1;
    stGrpVpssAttr.enCompressMode = COMPRESS_MODE_NONE;

    stVpssChnAttr.enChnMode = VPSS_CHN_MODE_USER;
    stVpssChnAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssChnAttr.enPixelFormat = RK_FMT_YUV420SP;
    stVpssChnAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssChnAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssChnAttr.u32Width = width;
    stVpssChnAttr.u32Height = height;
    stVpssChnAttr.enCompressMode = COMPRESS_MODE_NONE;

    s32Ret = RK_MPI_VPSS_CreateGrp(s32Grp, &stGrpVpssAttr);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    s32Ret = RK_MPI_VPSS_SetChnAttr(s32Grp, VpssChn, &stVpssChnAttr);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }
    s32Ret = RK_MPI_VPSS_EnableChn(s32Grp, VpssChn);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    s32Ret = RK_MPI_VPSS_StartGrp(s32Grp);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }
    return s32Ret;
}

void process_video(RK_U32 u32Width, RK_U32 u32Height, RK_U32 disp_width,
                   RK_U32 disp_height, RK_S32 s32chnlId, RK_CODEC_ID_E enCodecType,
                   rclcpp::Node* node, bool enable_publish_encoder) {

    RK_S32 s32Ret = RK_FAILURE;
    MPP_CHN_S stSrcChn, stvpssChn, stvencChn;

    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        RCLCPP_ERROR(node->get_logger(), "rk mpi sys init fail!");
        return;
    }

    // init rtsp
    g_rtsplive = create_rtsp_demo(554);
    g_rtsp_session = rtsp_new_session(g_rtsplive, "/live/0");
    if (enCodecType == RK_VIDEO_ID_AVC) {
        rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
    } else if (enCodecType == RK_VIDEO_ID_HEVC) {
        rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
    } else {
        printf("not support other type\n");
        return;
    }
    rtsp_sync_video_ts(g_rtsp_session, rtsp_get_reltime(), rtsp_get_ntptime());

    vi_dev_init();
    vi_chn_init(s32chnlId, u32Width, u32Height);
    test_vpss_init(0, disp_width, disp_height);
    // venc  init
    test_venc_init(0, disp_width, disp_height, enCodecType);

    // bind vi to venc
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = s32chnlId;

    stvpssChn.enModId = RK_ID_VPSS;
    stvpssChn.s32DevId = 0;
    stvpssChn.s32ChnId = 0;
    RCLCPP_INFO(node->get_logger(), "====RK_MPI_SYS_Bind vi0 to vpss0====");
    s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stvpssChn);
    if (s32Ret != RK_SUCCESS) {
        RCLCPP_ERROR(node->get_logger(), "bind 0 ch venc failed");
        // 解绑定并清理资源
        RK_MPI_SYS_UnBind(&stvpssChn, &stvencChn);
        RK_MPI_SYS_UnBind(&stSrcChn, &stvpssChn);
        RK_MPI_VI_DisableChn(0, s32chnlId);
        RK_MPI_VPSS_StopGrp(0);
        RK_MPI_VPSS_DestroyGrp(0);
        s32Ret = RK_MPI_VENC_StopRecvFrame(0);
        s32Ret = RK_MPI_VENC_DestroyChn(0);
        RK_MPI_VI_DisableDev(0);
        RCLCPP_INFO(node->get_logger(), "test running exit:%d", s32Ret);
        RK_MPI_SYS_Exit();
        return;
    }

    stvencChn.enModId = RK_ID_VENC;
    stvencChn.s32DevId = 0;
    stvencChn.s32ChnId = 0;
    RCLCPP_INFO(node->get_logger(), "====RK_MPI_SYS_Bind vpss0 to venc0====");
    s32Ret = RK_MPI_SYS_Bind(&stvpssChn, &stvencChn);
    if (s32Ret != RK_SUCCESS) {
        RCLCPP_ERROR(node->get_logger(), "bind 1 ch venc failed");
        // 解绑定并清理资源
        RK_MPI_SYS_UnBind(&stvpssChn, &stvencChn);
        RK_MPI_SYS_UnBind(&stSrcChn, &stvpssChn);
        RK_MPI_VI_DisableChn(0, s32chnlId);
        RK_MPI_VPSS_StopGrp(0);
        RK_MPI_VPSS_DestroyGrp(0);
        s32Ret = RK_MPI_VENC_StopRecvFrame(0);
        s32Ret = RK_MPI_VENC_DestroyChn(0);
        RK_MPI_VI_DisableDev(0);
        RCLCPP_INFO(node->get_logger(), "test running exit:%d", s32Ret);
        RK_MPI_SYS_Exit();
        return;
    }

    // 启动获取编码数据的线程
    std::thread get_frame_thread([node, enable_publish_encoder]() {
        get_encoded_frames(node, enable_publish_encoder);
    });

    while (!quit) {
        usleep(50000);
    }

    get_frame_thread.join();

    if (g_rtsplive)
        rtsp_del_demo(g_rtsplive);

    s32Ret = RK_MPI_SYS_UnBind(&stvpssChn, &stvencChn);
    if (s32Ret != RK_SUCCESS) {
        RCLCPP_ERROR(node->get_logger(), "RK_MPI_SYS_UnBind fail %x", s32Ret);
    }

    s32Ret = RK_MPI_SYS_UnBind(&stSrcChn, &stvpssChn);
    if (s32Ret != RK_SUCCESS) {
        RCLCPP_ERROR(node->get_logger(), "RK_MPI_SYS_UnBind fail %x", s32Ret);
    }

    RK_MPI_VI_DisableChn(0, s32chnlId);
    RK_MPI_VPSS_StopGrp(0);
    RK_MPI_VPSS_DestroyGrp(0);

    s32Ret = RK_MPI_VENC_StopRecvFrame(0);
    s32Ret = RK_MPI_VENC_DestroyChn(0);
    RK_MPI_VI_DisableDev(0);

    RCLCPP_INFO(node->get_logger(), "test running exit:%d", s32Ret);
    RK_MPI_SYS_Exit();
}

void get_encoded_frames(rclcpp::Node* node, bool enable_publish_encoder) {
    printf("========%s========\n", __func__);
    void *pData = RK_NULL;
    int loopCount = 0;
    int s32Ret;

    VENC_STREAM_S stFrame;
    stFrame.pstPack = static_cast<VENC_PACK_S*>(malloc(sizeof(VENC_PACK_S)));
    extern rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;

    while (!quit) {
        s32Ret = RK_MPI_VENC_GetStream(0, &stFrame, 200000);
        if (s32Ret == RK_SUCCESS) {
            if (g_rtsplive && g_rtsp_session) {
                pData = RK_MPI_MB_Handle2VirAddr(stFrame.pstPack->pMbBlk);
                rtsp_tx_video(g_rtsp_session, static_cast<const uint8_t*>(pData), stFrame.pstPack->u32Len,
                            stFrame.pstPack->u64PTS);
                rtsp_do_event(g_rtsplive);
            }

            // 创建ROS2消息并发布
            if (publisher_ && enable_publish_encoder) {
                pData = RK_MPI_MB_Handle2VirAddr(stFrame.pstPack->pMbBlk);
                auto msg = std::make_unique<sensor_msgs::msg::Image>();
                msg->header.stamp = node->now();
                msg->header.frame_id = "camera_frame";
                msg->height = 0;  // 编码数据没有固定的高度
                msg->width = stFrame.pstPack->u32Len;
                msg->encoding = "h264";  // 根据实际编码类型调整
                msg->is_bigendian = false;
                msg->step = stFrame.pstPack->u32Len;

                msg->data.resize(stFrame.pstPack->u32Len);
                memcpy(msg->data.data(), pData, stFrame.pstPack->u32Len);

                // 发布消息
                publisher_->publish(std::move(msg));
            }

            RK_U64 nowUs = TEST_COMM_GetNowUs();
            RCLCPP_DEBUG(node->get_logger(),
                "chn:0, loopCount:%d enc->seq:%d wd:%d pts=%lu delay=%luus\n",
                loopCount, stFrame.u32Seq, stFrame.pstPack->u32Len,
                stFrame.pstPack->u64PTS, nowUs - stFrame.pstPack->u64PTS);

            s32Ret = RK_MPI_VENC_ReleaseStream(0, &stFrame);
            if (s32Ret != RK_SUCCESS) {
                RCLCPP_ERROR(node->get_logger(), "RK_MPI_VENC_ReleaseStream fail %x", s32Ret);
            }
            loopCount++;
        } else {
            RCLCPP_ERROR(node->get_logger(), "RK_MPI_VENC_GetChnFrame fail %x", s32Ret);
        }

        if ((g_s32FrameCnt >= 0) && (loopCount > g_s32FrameCnt))
            quit = true;
    }

    free(stFrame.pstPack);
}
