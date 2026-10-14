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
    ROCKASR_LANG_ENGLISH = 0,
    ROCKASR_LANG_CHINESE = 1
} RockAsrLanguage;

/**
 * @brief 识别结果类型
 */
typedef enum RockAsrResultType {
    ROCKASR_RESULT_TYPE_TRANSCRIPT = 0,
    ROCKASR_RESULT_TYPE_TRANSLATE = 1,
    ROCKASR_RESULT_TYPE_ALL = 2
} RockAsrResultType;

/**
 * @brief Asr结果状态
 */
typedef enum RockAsrState {
    ASR_STATE_NONE = 0,
    ASR_STATE_FIRST = 1,
    ASR_STATE_RUNNING = 2,
    ASR_STATE_FINISH = 3,
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
    RockAsrLanguage sourceLanguage;  //< 源语言
    RockAsrLanguage targetLanguage;  //< 目标语言
    int sampleRate;                  //< 输入音频采样率
    void* userdata;                  //< 用户自定义数据
    RockAsrResultCallback resultCallback; //< 结果回调
} RockXAsrInitParam;

#ifdef __cplusplus
}
#endif

#endif

