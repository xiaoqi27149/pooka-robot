#ifndef rk_tts_interface_H
#define rk_tts_interface_H

#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

typedef void* rkauth_handle_t;
class mix_frontend;
class audio_backend;

// typedef void (*Callback)(const int16_t *each_output, int wav_len, void *userdata);
typedef void (*Callback)(const int16_t *each_output, int wav_len, bool is_last_frame, void *userdata);

class rk_tts_interface
{
public:
    int init(const char *rkauth_json_config, const char *model_path, const char *frontend_extra_path, int speaker_id, float alpha, int sampleRate, Callback callback_func_in, void *_userdata);
    int release();
    int inference(const char *input_char, float input_alpha, bool is_last_text);

    void frontendThreadFuntion();
    void backendThreadFunction(int, float, int);

private:
    rkauth_handle_t rkauth_handle;
    mix_frontend *Mix_Frontend;
    audio_backend *Audio_Backend;
    Callback output_callback;
    void *userdata;

    std::thread frontendThread;
    bool stopfrontendThread;
    std::thread backendThread;
    bool stopbackendThread;

    std::mutex frontendMutex;
    std::condition_variable frontendCV;
    bool frontendThreadRun;

    std::mutex backendMutex;
    std::condition_variable backendCV;
    bool backendThreadRun;

    std::queue<std::pair<std::string, float>> textsQueue;
    std::vector<std::pair<std::vector<int>, float>> phone_ids;
    std::mutex phoneIdMutex;
    std::mutex hsMutex;

    bool need_check_last_frame;
};

#endif
