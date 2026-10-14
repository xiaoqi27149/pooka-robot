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
#include <sstream>
#include <string>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cstdarg>
#include <cstdio>
#include <cstdlib> // getenv

#define ENABLE_FPS_DEBUG false

class Logger {
public:
    enum class Level {
        ERROR = 0,
        WARN,
        INFO,
        DEBUG
    };

    static Logger& instance() {
        static Logger instance;
        return instance;
    }

    void setLevel(Level level) {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }

    void updateLevelFromEnv() {
        const char* env_level = std::getenv("RMSL_LOG_LEVEL");
        if (env_level != nullptr) {
            std::string level_str(env_level);
            std::lock_guard<std::mutex> lock(mutex_);

            if (level_str == "DEBUG") {
                level_ = Level::DEBUG;
            } else if (level_str == "INFO") {
                level_ = Level::INFO;
            } else if (level_str == "WARN") {
                level_ = Level::WARN;
            } else if (level_str == "ERROR") {
                level_ = Level::ERROR;
            } else {
                std::cerr << "Invalid RMSL_LOG_LEVEL value: " << level_str << ". Level remains unchanged." << std::endl;
            }
        }

    }

    Level getLevel() const {
        return level_;
    }

    void log(Level msgLevel, const char* format, ...) {
        if (msgLevel > level_) return;

        std::lock_guard<std::mutex> lock(mutex_);

        // 获取时间戳
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm_time;
#if defined(_WIN32)
        localtime_s(&tm_time, &t);
#else
        localtime_r(&t, &tm_time);
#endif

        char buffer[512];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        std::ostringstream oss;
        oss << "[" << std::put_time(&tm_time, "%H:%M:%S") << "]";
        oss << " [" << levelToString(msgLevel) << "]";
        oss << " " << buffer;

        std::cout << oss.str() << std::endl;
    }

private:
    Logger() : level_(Level::WARN) {
        updateLevelFromEnv();
    }
    ~Logger() {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string levelToString(Level level) const {
        switch (level) {
            case Level::ERROR: return "ERROR";
            case Level::WARN:  return "WARN";
            case Level::INFO:  return "INFO";
            case Level::DEBUG: return "DEBUG";
            default:           return "UNKNOWN";
        }
    }

    Level level_;
    mutable std::mutex mutex_;
};

#define RMSL_ERROR(format, ...) Logger::instance().log(Logger::Level::ERROR, format, ##__VA_ARGS__)
#define RMSL_WARN(format, ...)  Logger::instance().log(Logger::Level::WARN,  format, ##__VA_ARGS__)
#define RMSL_INFO(format, ...)  Logger::instance().log(Logger::Level::INFO,  format, ##__VA_ARGS__)
#define RMSL_DEBUG(format, ...) Logger::instance().log(Logger::Level::DEBUG, format, ##__VA_ARGS__)
