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

#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_define.hpp>

namespace stdsc
{
struct Client::Impl
{
    Impl(void)
    {
    }

    ~Impl(void)
    {
    }

    void connect(const char* host, const char* port,
                 uint32_t retry_interval_usec = STDSC_RETRY_INTERVAL_USEC,
                 uint32_t timeout_sec = STDSC_INFINITE)
    {
        bool is_success = false;
        while (!is_success)
        {
            try
            {
                sock_ =
                  Socket::establish_connection(host, port, STDSC_INFINITE);
                is_success = true;
            }
            catch (const SocketException& e)
            {
                STDSC_LOG_TRACE(e.what());
                usleep(retry_interval_usec);
            }
        }
        STDSC_LOG_INFO("Connected to server (%s)", host);
    }

    void close(void)
    {
        sock_.send_packet(make_packet(kControlCodeExit));
    }

    void send_request(const uint64_t code)
    {
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
        STDSC_LOG_TRACE("Send data request packet. (code:0x%08x)", code);
        auto control_code = code;
        auto packet = make_packet(control_code);
        sock_.send_packet(packet);

        Packet recv_packet;
        sock_.recv_packet(recv_packet);
        auto size = static_cast<std::size_t>(recv_packet.u_body.data.size);
        STDSC_LOG_TRACE("Received packet. (code:0x%08x, sz:%lu)",
                        recv_packet.control_code, size);
        buffer.resize(size);
        sock_.recv_buffer(buffer);

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

    stdsc::Socket sock_;
};

Client::Client(void) : pimpl_(new Impl())
{
}

Client::~Client(void)
{
}

void Client::connect(const char* host, const char* port)
{
    try
    {
        pimpl_->connect(host, port);
    }
    catch (const stdsc::SocketException& e)
    {
        STDSC_LOG_TRACE("Can not connect to server (%s@%s)", host, port);
    }
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

void Client::send_request_blocking(const uint64_t code,
                                   uint32_t retry_interval_usec,
                                   uint32_t timeout_sec)
{
    bool is_success = false;
    while (!is_success)
    {
        try
        {
            send_request(code);
            is_success = true;
        }
        catch (const stdsc::RejectException& e)
        {
            STDSC_LOG_TRACE("%s", e.what());
            usleep(retry_interval_usec);
            continue;
        }
    }
}

void Client::send_data_blocking(const uint64_t code, const Buffer& buffer,
                                uint32_t retry_interval_usec,
                                uint32_t timeout_sec)
{
    bool is_success = false;
    while (!is_success)
    {
        try
        {
            send_data(code, buffer);
            is_success = true;
        }
        catch (const stdsc::RejectException& e)
        {
            STDSC_LOG_TRACE("%s", e.what());
            usleep(retry_interval_usec);
            continue;
        }
    }
}

void Client::recv_data_blocking(const uint64_t code, Buffer& buffer,
                                uint32_t retry_interval_usec,
                                uint32_t timeout_sec)
{
    bool is_success = false;
    while (!is_success)
    {
        try
        {
            recv_data(code, buffer);
            is_success = true;
        }
        catch (const stdsc::RejectException& e)
        {
            STDSC_LOG_TRACE("%s", e.what());
            usleep(retry_interval_usec);
            continue;
        }
    }
}

} /* namespace opsica_packet */
