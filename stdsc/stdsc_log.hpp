/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STDSC_LOG_HPP
#define STDSC_LOG_HPP

#include <memory>
#include <stdsc/stdsc_define.hpp>

#define STDSC_INIT_LOG() stdsc::g_logger = stdsc::Logger::get_instance();

#define STDSC_LOG_ERR(format, ...) \
    STDSC_LOG(stdsc::kLogLevelErr, format, ##__VA_ARGS__)
#define STDSC_LOG_WARN(format, ...) \
    STDSC_LOG(stdsc::kLogLevelWarn, format, ##__VA_ARGS__)
#define STDSC_LOG_INFO(format, ...) \
    STDSC_LOG(stdsc::kLogLevelInfo, format, ##__VA_ARGS__)
#define STDSC_LOG_TRACE(format, ...) \
    STDSC_LOG(stdsc::kLogLevelTrace, format, ##__VA_ARGS__)
#define STDSC_LOG_DEBUG(format, ...) \
    STDSC_LOG(stdsc::kLogLevelDebug, format, ##__VA_ARGS__)

#define STDSC_LOG(level, format, ...)                                        \
    do                                                                       \
    {                                                                        \
        if (nullptr != stdsc::g_logger)                                      \
        {                                                                    \
            stdsc::g_logger->emit((level), __FILE__, __FUNCTION__, __LINE__, \
                                  format, ##__VA_ARGS__);                    \
        }                                                                    \
    } while (0)

#define STDSC_SET_LOG_LEVEL(level)               \
    do                                           \
    {                                            \
        if (nullptr != stdsc::g_logger)          \
        {                                        \
            stdsc::g_logger->set_level((level)); \
        }                                        \
    } while (0)

namespace stdsc
{

/**
 * @brief Enumeration for log level.
 */
enum LogLevel_t : int
{
    kLogLevelErr = 0,
    kLogLevelWarn = 1,
    kLogLevelInfo = 2,
    kLogLevelTrace = 3,
    kLogLevelDebug = 4,
    kLogLevelNum,
};

/**
 * @brief Provides logging function.
 */
class Logger
{
public:
    Logger(void);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(const Logger&&) = delete;
    Logger& operator=(const Logger&&) = delete;

    ~Logger(void) = default;

    static Logger* get_instance(void);

    void emit(const LogLevel_t level, const char* source_file_name,
              const char* func_name, int source_line_number, const char* format,
              ...) const;

    void set_level(const LogLevel_t level);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

extern Logger* g_logger;

} /* namespace stdsc */

#endif /* STDSC_LOG_HPP */
