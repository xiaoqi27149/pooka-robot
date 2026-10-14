#ifndef ROCKTTS_H
#define ROCKTTS_H

#include "rockx_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TTS回调函数
 * @param wav 音频数据
 * @param userdata 用户自定义数据指针
 */
typedef void (*RockTTSCallback)(RockXAudioWav* wav, void* userdata);

typedef struct {
    const char* model;          // 模型（基于该名字在data_path目录下找模型目录）
    const char* dict;           // 字典（基于该名字在data_path目录下找字典目录）
    int speakerId;              // 说话人ID 
    float alpha;                // 音量
    void* userdata;             // 用户自定义数据指针
    RockTTSCallback callback;   // 回调函数
} RockTTSInitParam;

#ifdef __cplusplus
}
#endif

#endif