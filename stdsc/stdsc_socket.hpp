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

#ifndef STDSC_SOCKET_HPP
#define STDSC_SOCKET_HPP

#include <sys/socket.h>

#include <memory>

#include <stdsc/stdsc_define.hpp>

#define STDSC_SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR))
#define STDSC_SOMAXCONN 0x7fffffff

namespace stdsc
{

struct Packet;
class Buffer;

/**
 * @ brief Provices socket communication
 */
class Socket
{
public:
    Socket(void);

    Socket(const Socket& rhs);
    Socket& operator=(const Socket& rhs);

    ~Socket();

    static Socket make_listen_socket(const char* port,
                                     int optname = STDSC_SO_EXCLUSIVEADDRUSE,
                                     int backlog = STDSC_SOMAXCONN);

    static Socket accept_connection(Socket& listen_sock,
                                    uint32_t timeout_sec = STDSC_TIME_INFINITE);

    static Socket establish_connection(const char* host,
                                       const char* port,
                                       uint32_t timeout_sec =
                                         STDSC_CONN_TIMEOUT_SEC);
    
    int connection_id(void) const;

    void shutdown(void);

    void close(void);

    void send_packet(const Packet& packet) const;

    void recv_packet(Packet& packet,
                     uint32_t timeout_sec = STDSC_TIME_INFINITE) const;

    void send_buffer(const Buffer& buffer) const;

    void recv_buffer(Buffer& buffer,
                     uint32_t timeout_sec = STDSC_TIME_INFINITE) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace stdsc */

#endif /* STDSC_SOCKET_HPP */
