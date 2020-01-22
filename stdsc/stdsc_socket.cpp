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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h> // for fcntl
#include <fcntl.h>  // for fcntl

#include <cstdint>
#include <climits>
#include <cstring>

#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_buffer.hpp>

static constexpr int INVALID_SOCKET = -1;
static constexpr int SOCKET_ERROR = -1;
static constexpr int SOCKET_CLOSED = 0;
static constexpr int KEEPALIVEDELAY_SEC = 60;
static constexpr int KEEPINTERVALTIME_SEC = 30;
static constexpr int KEEPALIVECOUNT = 10;

#if !defined(TCP_KEEPIDLE) && defined(TCP_KEEPALIVE)
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif

#define SOCKET_IF_CHECK(cond, msg)                               \
    do                                                           \
    {                                                            \
        if (!(cond))                                             \
        {                                                        \
            STDSC_LOG_ERR(msg " : Sock Error Code (%d)", errno); \
            STDSC_THROW_SOCKET(msg);                             \
        }                                                        \
    } while (0)

#define SOCKET_IF_CHECK_CLOSE(cond, msg, socket)                 \
    do                                                           \
    {                                                            \
        if (!(cond))                                             \
        {                                                        \
            STDSC_LOG_ERR(msg " : Sock Error Code (%d)", errno); \
            shutdown_socket(socket);                             \
            close_socket(socket);                                \
            STDSC_THROW_SOCKET(msg);                             \
        }                                                        \
    } while (0)

namespace stdsc
{

static void shutdown_socket(int socket)
{
    int ret = ::shutdown(socket, SHUT_RDWR);
    if (ret < 0)
    {
        // 9   : EBADF
        // 22  : EINVAL
        // 107 : ENOTCONN
        // 88  : ENOTSOCK
        STDSC_LOG_DEBUG("Failed to shutdown socket : %d", errno);
    }
    return;
}

static void close_socket(int& socket)
{
    if (socket != INVALID_SOCKET)
    {
        STDSC_LOG_DEBUG("close : 0x%x", socket);

        int ret = ::close(socket);
        if (ret < 0)
        {
            STDSC_LOG_WARN("peer was shutdowned : %d", errno);
        }

        socket = INVALID_SOCKET;
    }
}

static void make_blocking(int socket)
{
    STDSC_LOG_TRACE("make blocking : 0x%x", socket);

    int ret;
    ret = ::fcntl(socket, F_GETFL, 0);
    SOCKET_IF_CHECK(SOCKET_ERROR != ret,
                    "Failed to make socket blocking using ioctlsocket");

    ret = ::fcntl(socket, F_SETFL, ret & ~O_NONBLOCK);
    SOCKET_IF_CHECK(SOCKET_ERROR != ret,
                    "Failed to make socket blocking using ioctlsocket");
}

static bool wait_write(int socket, uint32_t timeout_sec)
{
    STDSC_LOG_TRACE("wait write : %u : 0x%x", timeout_sec, socket);

    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(socket, &wfds);

    int ret;

    if (STDSC_TIME_INFINITE != timeout_sec)
    {
        struct timeval tv;
        tv.tv_sec = static_cast<long>(timeout_sec);
        tv.tv_usec = 0;

        ret = ::select(socket + 1, NULL, &wfds, NULL, &tv);
        SOCKET_IF_CHECK(SOCKET_ERROR != ret, "Failed to select");
    }
    else
    {
        ret = ::select(socket + 1, NULL, &wfds, NULL, NULL);
        SOCKET_IF_CHECK(SOCKET_ERROR != ret, "Failed to select");
    }

    /* time out */
    if (0 == ret)
    {
        return false;
    }

    return true;
}

static bool wait_read(int socket, uint32_t timeout_sec)
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(socket, &rfds);

    int ret;

    if (STDSC_TIME_INFINITE != timeout_sec)
    {
        struct timeval tv;
        tv.tv_sec = static_cast<long>(timeout_sec);
        tv.tv_usec = 0;

        ret = ::select(socket + 1, &rfds, NULL, NULL, &tv);
        SOCKET_IF_CHECK(SOCKET_ERROR != ret, "Failed to select");
    }
    else
    {
        ret = ::select(socket + 1, &rfds, NULL, NULL, NULL);
        SOCKET_IF_CHECK(SOCKET_ERROR != ret, "Failed to select");
    }

    /* time out */
    if (0 == ret)
    {
        return false;
    }

    return true;
}

struct Socket::Impl
{
    Impl() : socket_(INVALID_SOCKET)
    {
    }
    ~Impl()
    {
    }

    void read(void* buffer, std::size_t bytes) const
    {
        STDSC_LOG_DEBUG("read: 0x%x", socket_);
        char* ptr = reinterpret_cast<char*>(buffer);
        std::size_t remain = bytes;

        while (0 < remain)
        {
            int ret = ::recv(socket_, ptr, remain, 0);
            SOCKET_IF_CHECK(SOCKET_ERROR != ret, "Failed to receive");
            SOCKET_IF_CHECK(SOCKET_CLOSED != ret, "Socket closed");
            ptr += ret;
            remain -= ret;
        }
    }

    void write(const void* buffer, std::size_t bytes) const
    {
        STDSC_LOG_DEBUG("write : 0x%x", socket_);
        const char* ptr = reinterpret_cast<const char*>(buffer);
        std::size_t remain = bytes;

        while (0 < remain)
        {
            int ret = ::send(socket_, static_cast<const char*>(ptr), remain, 0);
            SOCKET_IF_CHECK(SOCKET_ERROR != ret, "Failed to send");

            ptr += ret;
            remain -= ret;
        }
    }

    int socket_;
};

Socket::Socket(void) : pimpl_(new Impl())
{
}

Socket::Socket(const Socket& rhs) : pimpl_(new Impl(*(rhs.pimpl_.get())))
{
}

Socket& Socket::operator=(const Socket& rhs)
{
    if (this != &rhs)
    {
        pimpl_ = std::shared_ptr<Impl>(new Impl(*(rhs.pimpl_.get())));
    }

    return *this;
}

Socket::~Socket(void)
{
}

Socket Socket::make_listen_socket(const char* port, int optname, int backlog)
{
    int ret;
    int onoff = 1;

    uint32_t uint32_port = atoi(port);
    STDSC_IF_CHECK(uint32_port <= USHRT_MAX, "invalid port number");
    uint16_t uint16_port = static_cast<uint16_t>(uint32_port);

    int listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    SOCKET_IF_CHECK(INVALID_SOCKET != listen_socket, "Failed to create socket");

    ret = setsockopt(listen_socket, SOL_SOCKET, optname,
                     reinterpret_cast<const char*>(&onoff), sizeof(onoff));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt",
                          listen_socket);

    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(uint16_port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = ::bind(listen_socket, reinterpret_cast<sockaddr*>(&sa), sizeof(sa));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to bind", listen_socket);

    ret = ::listen(listen_socket, backlog);
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to listen",
                          listen_socket);

    Socket socket;
    socket.pimpl_->socket_ = listen_socket;

    return socket;
}

Socket Socket::accept_connection(Socket& listen_sock, uint32_t timeout_sec)
{
    int ret;
    int onoff = 1;
    int keepalivedelay_sec = KEEPALIVEDELAY_SEC;
    int keepaliveinterval_sec = KEEPINTERVALTIME_SEC;
    int keepalivecount = KEEPALIVECOUNT;

    int listen_socket = listen_sock.pimpl_->socket_;
    STDSC_LOG_TRACE("accept connection : 0x%x", listen_socket);

    /* select */
    bool wait_result = wait_read(listen_socket, timeout_sec);
    SOCKET_IF_CHECK(true == wait_result, "Accept connection timed out");

    STDSC_LOG_DEBUG("wait_read.");

    /* accept */
    sockaddr_in client;
    socklen_t addr_len = sizeof(client);
    int socket =
      ::accept(listen_socket, reinterpret_cast<sockaddr*>(&client), &addr_len);
    SOCKET_IF_CHECK(INVALID_SOCKET != socket, "Failed to accept");

    STDSC_LOG_DEBUG("accepted.");

    /* set nodelay option */
    ret = setsockopt(socket, IPPROTO_TCP, TCP_NODELAY,
                     reinterpret_cast<const char*>(&onoff), sizeof(onoff));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    /* set packet buffer size */
    int buf_size = STDSC_TCP_BUFFER_SIZE;

    ret =
      setsockopt(socket, SOL_SOCKET, SO_RCVBUF,
                 reinterpret_cast<const char*>(&buf_size), sizeof(buf_size));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    ret =
      setsockopt(socket, SOL_SOCKET, SO_SNDBUF,
                 reinterpret_cast<const char*>(&buf_size), sizeof(buf_size));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    /* set keepalive values */
    ret = setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE,
                     reinterpret_cast<const char*>(&onoff), sizeof(onoff));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    ret = setsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE,
                     reinterpret_cast<const char*>(&keepalivedelay_sec),
                     sizeof(keepalivedelay_sec));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    ret = setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL,
                     reinterpret_cast<const char*>(&keepaliveinterval_sec),
                     sizeof(keepaliveinterval_sec));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);
    ret = setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT,
                     reinterpret_cast<const char*>(&keepalivecount),
                     sizeof(keepalivecount));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    STDSC_LOG_DEBUG("setsocketopt.");

    Socket accept_socket;
    accept_socket.pimpl_->socket_ = socket;
    return accept_socket;
}

Socket Socket::establish_connection(const char* host, const char* port,
                                    uint32_t timeout_sec)
{
    STDSC_LOG_TRACE("establish connection");

    int ret;
    int onoff = 1;
    int keepalivedelay_sec = KEEPALIVEDELAY_SEC;
    int keepaliveinterval_sec = KEEPINTERVALTIME_SEC;
    int keepalivecount = KEEPALIVECOUNT;

    uint32_t uint32_port = atoi(port);
    STDSC_IF_CHECK(uint32_port <= USHRT_MAX, "invalid port number");

    /* make socket */
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    SOCKET_IF_CHECK(SOCKET_ERROR != socket, "Failed to create socket");

    /* set nodelay option */
    ret = setsockopt(socket, IPPROTO_TCP, TCP_NODELAY,
                     reinterpret_cast<const char*>(&onoff), sizeof(onoff));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    /* set packet buffer size */
    int buf_size = STDSC_TCP_BUFFER_SIZE;

    ret =
      setsockopt(socket, SOL_SOCKET, SO_RCVBUF,
                 reinterpret_cast<const char*>(&buf_size), sizeof(buf_size));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    ret =
      setsockopt(socket, SOL_SOCKET, SO_SNDBUF,
                 reinterpret_cast<const char*>(&buf_size), sizeof(buf_size));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    /* set keepalive values */
    ret = setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE,
                     reinterpret_cast<const char*>(&onoff), sizeof(onoff));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    ret = setsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE,
                     reinterpret_cast<const char*>(&keepalivedelay_sec),
                     sizeof(keepalivedelay_sec));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    ret = setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL,
                     reinterpret_cast<const char*>(&keepaliveinterval_sec),
                     sizeof(keepaliveinterval_sec));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);
    ret = setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT,
                     reinterpret_cast<const char*>(&keepalivecount),
                     sizeof(keepalivecount));
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret, "Failed to setsockopt", socket);

    /* convert hostname to addr */
    addrinfo* info;
    addrinfo hint;
    std::memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    ret = ::getaddrinfo(host, port, &hint, &info);
    SOCKET_IF_CHECK_CLOSE(0 == ret, "Failed to get addr info", socket);

    /* make sockaddr */
    sockaddr_in server = *(reinterpret_cast<sockaddr_in*>(info->ai_addr));
    ::freeaddrinfo(info);

    /* connect */
    ret =
      ::connect(socket, reinterpret_cast<sockaddr*>(&server), sizeof(server));
    int err = errno;
    STDSC_LOG_DEBUG("connect return: ret:%d, errno:%d", ret, err);
    SOCKET_IF_CHECK_CLOSE(SOCKET_ERROR != ret || EINPROGRESS == err,
                          "Failed to connect", socket);

    /* select */
    if (EINPROGRESS == err)
    {
        STDSC_LOG_TRACE("wait until connected");
        bool wait_result = wait_write(socket, timeout_sec);
        if (false == wait_result)
        {
            STDSC_LOG_WARN("false from wait write");
            shutdown_socket(socket);
            close_socket(socket);
            STDSC_LOG_WARN(
              "Connection timed out"
              " : Sock Error Code (%d)",
              EWOULDBLOCK);
            STDSC_THROW_SOCKET("Connection timed out");
        }
    }

    STDSC_LOG_DEBUG("Connected");

    /* make blocking */
    try
    {
        make_blocking(socket);
    }
    catch (const SocketException& e)
    {
        shutdown_socket(socket);
        close_socket(socket);
        throw e;
    }

    Socket connected_socket;
    connected_socket.pimpl_->socket_ = socket;
    return connected_socket;
}

int Socket::connection_id(void) const
{
    return pimpl_->socket_;
}

void Socket::shutdown(void)
{
    shutdown_socket(pimpl_->socket_);
}

void Socket::close(void)
{
    close_socket(pimpl_->socket_);
}

void Socket::send_packet(const Packet& packet) const
{
    pimpl_->write(reinterpret_cast<const void*>(&packet), sizeof(Packet));
}

void Socket::recv_packet(Packet& packet, uint32_t timeout_sec) const
{
    initialize_packet(packet);

    bool wait_result = wait_read(pimpl_->socket_, timeout_sec);

    SOCKET_IF_CHECK(true == wait_result, "Receive timed out");

    pimpl_->read(reinterpret_cast<void*>(&packet), sizeof(Packet));
}

void Socket::send_buffer(const Buffer& buffer) const
{
    if (0 < buffer.size())
    {
        pimpl_->write(reinterpret_cast<const void*>(buffer.data()),
                      buffer.size());
    }
}

void Socket::recv_buffer(Buffer& buffer, uint32_t timeout_sec) const
{
    if (0 < buffer.size())
    {
        bool wait_result = wait_read(pimpl_->socket_, timeout_sec);

        SOCKET_IF_CHECK(true == wait_result, "Receive timed out");

        pimpl_->read(reinterpret_cast<void*>(buffer.data()), buffer.size());
    }
}

} /* stdsc */
