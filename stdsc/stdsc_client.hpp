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

#ifndef STDSC_CLIENT_HPP
#define STDSC_CLIENT_HPP

#include <memory>
#include <stdsc/stdsc_define.hpp>

namespace stdsc
{

class Buffer;

/**
 * @ brief Provides client functions.
 */
class Client
{
public:
    Client(void);
    virtual ~Client(void);

    void connect(const char* host, const char* port,
                 const uint32_t retry_interval_usec = STDSC_RETRY_INTERVAL_USEC,
                 const uint32_t timeout_sec = STDSC_TIME_INFINITE);

    void close(void);

    void send_request(const uint64_t code);
    void send_data(const uint64_t code, const Buffer& buffer);
    void recv_data(const uint64_t code, Buffer& buffer);
    void send_recv_data(const uint64_t code, const Buffer& sbuffer, Buffer& rbuffer);

    void send_request_blocking(const uint64_t code,
                               const uint32_t retry_interval_usec =
                                 STDSC_RETRY_INTERVAL_USEC,
                               const uint32_t timeout_sec =
                                 STDSC_TIME_INFINITE);
    void send_data_blocking(const uint64_t code, const Buffer& buffer,
                            const uint32_t retry_interval_usec =
                              STDSC_RETRY_INTERVAL_USEC,
                            const uint32_t timeout_sec = STDSC_TIME_INFINITE);
    void recv_data_blocking(const uint64_t code, Buffer& buffer,
                            const uint32_t retry_interval_usec =
                              STDSC_RETRY_INTERVAL_USEC,
                            const uint32_t timeout_sec = STDSC_TIME_INFINITE);
    void send_recv_data_blocking(const uint64_t code,
                                 const Buffer& sbuffer, Buffer& rbuffer,
                                 const uint32_t retry_interval_usec =
                                 STDSC_RETRY_INTERVAL_USEC,
                                 const uint32_t timeout_sec = STDSC_TIME_INFINITE);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace stdsc_client */

#endif /* STDSC_CLIENT_HPP */
