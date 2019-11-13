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

#include <unistd.h>
#include <sstream>
#include <mutex>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_define.hpp>

namespace stdsc
{

static inline uint32_t calc_retry_count(const uint32_t timeout_sec,
                                        const uint32_t retry_interval_usec)
{
    uint32_t retry_interval_sec = retry_interval_usec / 1000000;
    uint32_t retry_count =
      (retry_interval_sec) ? (timeout_sec / retry_interval_sec) : 1;
    return retry_count;
}

static inline std::string count2str(uint32_t count)
{
    std::ostringstream oss;
    if (STDSC_TIME_INFINITE == count)
    {
        oss << "inf";
    }
    else
    {
        oss << count;
    }
    return oss.str();
}

struct Client::Impl
{
    Impl(void)
    {
    }

    ~Impl(void)
    {
        close();
    }

    void connect(const char* host, const char* port,
                 const uint32_t retry_interval_usec, const uint32_t timeout_sec)
    {
        bool is_success = false;
        uint32_t retry_count = 0;

        uint32_t max_retry_count =
          calc_retry_count(timeout_sec, retry_interval_usec);

        std::lock_guard<std::mutex> lock(mutex_);
        
        while (!is_success && max_retry_count > retry_count)
        {
            try
            {
                sock_ = Socket::establish_connection(host, port);
                is_success = true;
            }
            catch (const SocketException& e)
            {
                retry_count++;
                STDSC_LOG_INFO("Retry to connect to %s @ %s. (%d / %s)", host,
                               port, retry_count,
                               count2str(max_retry_count).c_str());
                usleep(retry_interval_usec);
            }
        }

        STDSC_THROW_SOCKET_IF_CHECK(max_retry_count > retry_count,
                                    "Connection time out");

        STDSC_LOG_TRACE("Connected to server (%s)", host);
    }

    void close(void)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        sock_.close();
    }

    void send_request(const uint64_t code)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        STDSC_LOG_TRACE("Send request packet. (code:0x%08x)", code);
        auto control_code = code;
        auto packet = make_packet(control_code);
        sock_.send_packet(packet);

        Packet ack;
        sock_.recv_packet(ack);
        STDSC_LOG_TRACE("ack: 0x%x", ack.control_code);

        if (ack.control_code == kControlCodeReject)
        {
            std::ostringstream ss;
            ss << "Rejected to send request. (0x" << std::hex
               << ack.control_code << ")";
            STDSC_THROW_REJECT(ss.str());
        }
        if (ack.control_code == kControlCodeFailed)
        {
            std::ostringstream ss;
            ss << "Failed to send request. (0x" << std::hex << ack.control_code
               << ")";
            STDSC_THROW_FAILURE(ss.str());
        }
    }

    void send_data(const uint64_t code, const Buffer& buffer)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        STDSC_LOG_TRACE("Send data packet. (code:0x%08x, sz:%lu)", code,
                        buffer.size());
        auto size = static_cast<uint64_t>(buffer.size());
        auto control_code = code;
        sock_.send_packet(make_data_packet(control_code, size));
        sock_.send_buffer(buffer);

        Packet ack;
        sock_.recv_packet(ack);
        STDSC_LOG_TRACE("ack: 0x%x", ack.control_code);

        if (ack.control_code == kControlCodeReject)
        {
            std::ostringstream ss;
            ss << "Rejected to send data. (0x" << std::hex << ack.control_code
               << ")";
            STDSC_THROW_REJECT(ss.str());
        }
        if (ack.control_code == kControlCodeFailed)
        {
            std::ostringstream ss;
            ss << "Failed to send data. (0x" << std::hex << ack.control_code
               << ")";
            STDSC_THROW_FAILURE(ss.str());
        }
    }

    void recv_data(const uint64_t code, Buffer& buffer)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        STDSC_LOG_TRACE("Send data request packet. (code:0x%08x)", code);
        auto control_code = code;
        auto packet = make_packet(control_code);
        sock_.send_packet(packet);

        Packet recv_packet;
        sock_.recv_packet(recv_packet);
        auto size = static_cast<std::size_t>(recv_packet.u_body.data.size);
        STDSC_LOG_TRACE("Received packet. (code:0x%08x, sz:%lu)",
                        recv_packet.control_code, size);
        if (recv_packet.control_code == kControlCodeReject)
        {
            std::ostringstream ss;
            ss << "Rejected to recv data. (0x" << std::hex
               << recv_packet.control_code << ")";
            STDSC_THROW_REJECT(ss.str());
        }

        if (size > 0)
        {
            buffer.resize(size);
            sock_.recv_buffer(buffer);
        }

        Packet ack;
        sock_.recv_packet(ack);
        STDSC_LOG_TRACE("ack: 0x%x", ack.control_code);

        if (ack.control_code == kControlCodeReject)
        {
            std::ostringstream ss;
            ss << "Rejected to recv data. (0x" << std::hex << ack.control_code
               << ")";
            STDSC_THROW_REJECT(ss.str());
        }
        if (ack.control_code == kControlCodeFailed)
        {
            std::ostringstream ss;
            ss << "Failed to recv data. (0x" << std::hex << ack.control_code
               << ")";
            STDSC_THROW_FAILURE(ss.str());
        }
    }

    void send_recv_data(const uint64_t code, const Buffer& sbuffer, Buffer& rbuffer)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        STDSC_LOG_TRACE("Send data packet. (code:0x%08x, sz:%lu)", code,
                        sbuffer.size());
        auto ssize = static_cast<uint64_t>(sbuffer.size());
        auto control_code = code;
        sock_.send_packet(make_data_packet(control_code, ssize));
        sock_.send_buffer(sbuffer);

        Packet recv_packet;
        sock_.recv_packet(recv_packet);
        auto rsize = static_cast<std::size_t>(recv_packet.u_body.data.size);
        STDSC_LOG_TRACE("Received packet. (code:0x%08x, sz:%lu)",
                        recv_packet.control_code, rsize);
        if (recv_packet.control_code == kControlCodeReject)
        {
            std::ostringstream ss;
            ss << "Rejected to recv data. (0x" << std::hex
               << recv_packet.control_code << ")";
            STDSC_THROW_REJECT(ss.str());
        }

        if (rsize > 0)
        {
            rbuffer.resize(rsize);
            sock_.recv_buffer(rbuffer);
        }

        Packet ack;
        sock_.recv_packet(ack);
        STDSC_LOG_TRACE("ack: 0x%x", ack.control_code);

        if (ack.control_code == kControlCodeReject)
        {
            std::ostringstream ss;
            ss << "Rejected to recv data. (0x" << std::hex << ack.control_code
               << ")";
            STDSC_THROW_REJECT(ss.str());
        }
        if (ack.control_code == kControlCodeFailed)
        {
            std::ostringstream ss;
            ss << "Failed to recv data. (0x" << std::hex << ack.control_code
               << ")";
            STDSC_THROW_FAILURE(ss.str());
        }
    }

private:
    stdsc::Socket sock_;
    std::mutex mutex_;
};

Client::Client(void) : pimpl_(new Impl())
{
}

Client::~Client(void)
{
}

void Client::connect(const char* host, const char* port,
                     const uint32_t retry_interval_usec,
                     const uint32_t timeout_sec)
{
    pimpl_->connect(host, port, retry_interval_usec, timeout_sec);
}

void Client::close(void)
{
    pimpl_->close();
}

void Client::send_request(const uint64_t code)
{
    try
    {
        pimpl_->send_request(code);
    }
    catch (const stdsc::SocketException& e)
    {
        STDSC_LOG_TRACE("Failed to send request.");
    }
}

void Client::send_data(const uint64_t code, const Buffer& buffer)
{
    try
    {
        pimpl_->send_data(code, buffer);
    }
    catch (const stdsc::SocketException& e)
    {
        STDSC_LOG_TRACE("Failed to send data.");
    }
}

void Client::recv_data(const uint64_t code, Buffer& buffer)
{
    try
    {
        pimpl_->recv_data(code, buffer);
    }
    catch (const stdsc::SocketException& e)
    {
        STDSC_LOG_TRACE("Failed to recv data.");
    }
}

void Client::send_recv_data(const uint64_t code, const Buffer& sbuffer, Buffer& rbuffer)
{
    try
    {
        pimpl_->send_recv_data(code, sbuffer, rbuffer);
    }
    catch (const stdsc::SocketException& e)
    {
        STDSC_LOG_TRACE("Failed to recv data.");
    }
}

void Client::send_request_blocking(const uint64_t code,
                                   const uint32_t retry_interval_usec,
                                   const uint32_t timeout_sec)
{
    bool is_success = false;
    uint32_t retry_count = 0;

    uint32_t max_retry_count =
      calc_retry_count(timeout_sec, retry_interval_usec);

    while (!is_success && max_retry_count > retry_count)
    {
        try
        {
            send_request(code);
            is_success = true;
        }
        catch (const stdsc::RejectException& e)
        {
            retry_count++;
            STDSC_LOG_TRACE("Retry to send request. (%d / %d)", retry_count,
                            max_retry_count);
            usleep(retry_interval_usec);
            continue;
        }
    }

    STDSC_THROW_SOCKET_IF_CHECK(max_retry_count > retry_count,
                                "Sending request time out");
}

void Client::send_data_blocking(const uint64_t code, const Buffer& buffer,
                                const uint32_t retry_interval_usec,
                                const uint32_t timeout_sec)
{
    bool is_success = false;
    uint32_t retry_count = 0;

    uint32_t max_retry_count =
      calc_retry_count(timeout_sec, retry_interval_usec);

    while (!is_success && max_retry_count > retry_count)
    {
        try
        {
            send_data(code, buffer);
            is_success = true;
        }
        catch (const stdsc::RejectException& e)
        {
            retry_count++;
            STDSC_LOG_TRACE("Retry to send data. (%d / %d)", retry_count,
                            max_retry_count);
            usleep(retry_interval_usec);
            continue;
        }
    }

    STDSC_THROW_SOCKET_IF_CHECK(max_retry_count > retry_count,
                                "Sending data time out");
}

void Client::recv_data_blocking(const uint64_t code, Buffer& buffer,
                                const uint32_t retry_interval_usec,
                                const uint32_t timeout_sec)
{
    bool is_success = false;
    uint32_t retry_count = 0;

    uint32_t max_retry_count =
      calc_retry_count(timeout_sec, retry_interval_usec);

    while (!is_success && max_retry_count > retry_count)
    {
        try
        {
            recv_data(code, buffer);
            is_success = true;
        }
        catch (const stdsc::RejectException& e)
        {
            retry_count++;
            STDSC_LOG_TRACE("Retry to recv data. (%d / %d)", retry_count,
                            max_retry_count);
            usleep(retry_interval_usec);
            continue;
        }
    }

    STDSC_THROW_SOCKET_IF_CHECK(max_retry_count > retry_count,
                                "Receiving data time out");
}

void Client::send_recv_data_blocking(const uint64_t code,
                                     const Buffer& sbuffer, Buffer& rbuffer,
                                     const uint32_t retry_interval_usec,
                                     const uint32_t timeout_sec)
{
    bool is_success = false;
    uint32_t retry_count = 0;

    uint32_t max_retry_count =
      calc_retry_count(timeout_sec, retry_interval_usec);

    while (!is_success && max_retry_count > retry_count)
    {
        try
        {
            send_recv_data(code, sbuffer, rbuffer);
            is_success = true;
        }
        catch (const stdsc::RejectException& e)
        {
            retry_count++;
            STDSC_LOG_TRACE("Retry to recv data. (%d / %d)", retry_count,
                            max_retry_count);
            usleep(retry_interval_usec);
            continue;
        }
    }

    STDSC_THROW_SOCKET_IF_CHECK(max_retry_count > retry_count,
                                "Receiving data time out");
}

} /* namespace opsica_packet */
