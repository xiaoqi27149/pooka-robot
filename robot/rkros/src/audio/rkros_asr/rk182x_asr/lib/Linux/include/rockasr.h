#ifndef __ROCKASR_H__
#define __ROCKASR_H__

#include "rockx_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 工作模式
 */
typedef enum RockAsrWorkMode {
    ROCKASR_MODE_STREAM = 0,        //< 流式
    ROCKASR_MODE_TRANSCRIBE = 1     //< 非流式
} RockAsrWorkMode;

/**
 * @brief 语言
 */
typedef enum RockAsrLanguage {
    ROCKASR_LANG_ENGLISH = 0,  //< 英语
    ROCKASR_LANG_CHINESE = 1,  //< 中文
    ROCKASR_LANG_JAPANESE = 2, //< 日语
    ROCKASR_LANG_KOREAN = 3    //< 韩语
} RockAsrLanguage;

/**
 * @brief ASR语言配置
 */
typedef struct RockAsrLangMode {
    RockAsrLanguage source;  //< 源语言
    RockAsrLanguage target;  //< 目标语言
} RockAsrLangMode;

/**
 * @brief 识别结果类型
 */
typedef enum RockAsrResultType {
    ROCKASR_RESULT_TYPE_TRANSCRIPT = 0x1,  //< 转录结果
    ROCKASR_RESULT_TYPE_TRANSLATE = 0x2,   //< 翻译结果
    ROCKASR_RESULT_TYPE_AUDIO = 0x4        //< 音频结果
} RockAsrResultType;

/**
 * @brief Asr结果状态
 */
typedef enum RockAsrState {
    ASR_STATE_NONE = 0,     //< 未开始
    ASR_STATE_FIRST = 1,    //< 首次结果
    ASR_STATE_RUNNING = 2,  //< 识别中
    ASR_STATE_FINISH = 3,   //< 识别结束
} RockAsrState;

/**
 * @brief 识别结果
 */
typedef struct RockAsrResult {
    RockAsrResultType type;       //< 结果类型
    RockAsrState state;           //< 结果状态
    unsigned long tsStart;        //< 开始时间戳
    unsigned long tsEnd;          //< 结束时间戳
    char* result;                 //< 当前句子识别结果
    char* newResult;              //< 新的识别结果
    char* transResult;            //< 翻译结果
    RockXAudioWav* audioResult;   //< 音频结果
} RockAsrResult;

/**
 * @brief Asr结果回调
 */
typedef void (*RockAsrResultCallback)(const RockAsrResult* result, void* userdata);

/**
 * @brief Asr初始化参数
 */
typedef struct RockXAsrInitParam {
    char* modelName;                 //< 模型名称
    RockAsrWorkMode workMode;        //< 工作模式
    RockAsrLangMode langMode;        //< 语言模式
    int sampleRate;                  //< 输入音频采样率
    void* userdata;                  //< 用户自定义数据
    RockAsrResultCallback resultCallback; //< 结果回调
} RockXAsrInitParam;

#ifdef __cplusplus
}
#endif

#endif

