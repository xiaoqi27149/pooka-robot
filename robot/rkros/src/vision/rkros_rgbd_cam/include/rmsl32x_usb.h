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
#include "rmsl32x_calib.h"

#define RMSL_DATA_SIZE_CMD 0x04
#define RMSL_DATA_SIZE_QUERY 0x3c

enum uvc_frame_type {
    UVC_FRAME_TYPE_DEFAULT = 0,
    UVC_FRAME_TYPE_RGB,
    UVC_FRAME_TYPE_DEPTH,
    UVC_FRAME_TYPE_COMBINE,
    UVC_FRAME_TYPE_LEFT,
    UVC_FRAME_TYPE_RIGHT,
    UVC_FRAME_TYPE_NULL
};

typedef struct uvc_head_v1_s {
    unsigned short magic;
    unsigned short version;
    unsigned short size;
    unsigned short type; //1: rgb, 2:dp, 3:combine
    unsigned short width;
    unsigned short height;
    int            sequence;
    int64_t        timestamp;
    int            temperature;
} uvc_head_v1_t;

int rmsl_reboot_to_loader(int fd);
int rmsl_get_version(int fd, char *ver, int size);
int rmsl_get_devices(char *dev_depth, char *dev_rgb, char *dev_ir, int silent = 0);
int rmsl_get_calib_data(int fd, UVC_StereoCameraParams *param, int silent = 0);
int rmsl_sync_time_to_module(int fd);
int rmsl_get_extra_data_from_jpeg(uint8_t* jpeg_data, size_t jpeg_size,
    uint8_t** extra_data, size_t* extra_size);
int rmsl_set_model_mode(int fd, int mode);
int rmsl_module_reboot(int fd);
