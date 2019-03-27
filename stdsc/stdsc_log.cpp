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

#include <stdarg.h>

#include <iomanip>
#include <sstream>
#include <iostream>
#include <mutex>

#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_utility.hpp>

#define STDSC_LOG_MAX_LENGTH (1024 * 5)
#define STDSC_LOG_LEVEL_ENV "STDSC_LOG_LEVEL"
#define STDSC_DEFAULT_LOG_LEVEL stdsc::kLogLevelInfo

namespace stdsc
{

static LogLevel_t read_env(void)
{
    LogLevel_t level = static_cast<LogLevel_t>(-1);
    auto env_str = utility::getenv(STDSC_LOG_LEVEL_ENV);
    if (utility::isdigit(env_str))
    {
        int x = stoi(env_str);
        if (x < kLogLevelNum)
        {
            level = static_cast<LogLevel_t>(x);
        }
    }
    return level;
}

static std::string make_debuginfo(const char* source_file_name,
                                  const char* func_name, int source_line_number)
{
    std::string striped_source_file_name(source_file_name);
    striped_source_file_name.erase(
      0, striped_source_file_name.find_last_of("/") + 1);

    std::stringstream ss;
    ss << std::setw(24) << std::left << striped_source_file_name << " ";
    ss << std::setw(5) << std::right << source_line_number << " ";
    ss << std::setw(32) << std::left << func_name << " ";

    return ss.str();
}

struct Logger::Impl
{
    Impl(void) : current_level_(STDSC_DEFAULT_LOG_LEVEL)
    {
    }
    ~Impl(void) = default;
    LogLevel_t current_level_;
    static std::mutex mutex_;
};

Logger::Logger(void) : pimpl_(new Impl())
{
}

Logger* Logger::get_instance(void)
{
    std::lock_guard<std::mutex> lock(Impl::mutex_);
    static Logger logger;
    LogLevel_t env_level = read_env();
    if (0 > static_cast<int>(env_level))
    {
        env_level = STDSC_DEFAULT_LOG_LEVEL;
    }
    logger.set_level(env_level);
    return &logger;
}

void Logger::emit(const LogLevel_t level, const char* source_file_name,
                  const char* func_name, int source_line_number,
                  const char* format, ...) const
{
    if (pimpl_->current_level_ < level)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(Impl::mutex_);

    char message_buffer[STDSC_LOG_MAX_LENGTH];

    va_list ap;
    va_start(ap, format);
    vsprintf(message_buffer, format, ap);
    va_end(ap);

    std::string message(message_buffer);

    std::stringstream ss;
    ss << std::setw(48) << std::left << message;
    if (pimpl_->current_level_ >= kLogLevelDebug) {
        ss << make_debuginfo(source_file_name, func_name, source_line_number);
    }
    std::cout << ss.str() << std::endl;
}

void Logger::set_level(const LogLevel_t level)
{
    pimpl_->current_level_ = level;
}

Logger* g_logger = nullptr;
std::mutex Logger::Impl::mutex_;

} /* namespace stdsc */
