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

#include <memory>
#include <unordered_map>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_state.hpp>

namespace stdsc
{

struct CallbackFunctionContainer::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;

    void set(uint64_t code, std::shared_ptr<CallbackFunction>& func)
    {
        STDSC_LOG_TRACE("set func for 0x%x.", code);
        map_.emplace(code, func);
    }

    void eval(const Socket& sock, const Packet& packet, StateContext& state)
    {
        auto code = static_cast<uint64_t>(packet.control_code);
        STDSC_LOG_TRACE("eval for 0x%x.", code);
        if (code & kControlCodeGroupRequest)
        {
            if (map_.count(code))
            {
                map_[code]->eval(code, state);
            }
        }
        else if (code & kControlCodeGroupData)
        {
            std::size_t buffer_size = packet.u_body.data.size;
            STDSC_LOG_TRACE("data size: %lu", buffer_size);
            Buffer buffer(buffer_size);
            sock.recv_buffer(buffer);
            if (map_.count(code))
            {
                map_[code]->eval(code, buffer, state);
            }
        }
        else if (code & kControlCodeGroupDownload)
        {
            if (map_.count(code))
            {
                map_[code]->eval(code, sock, state);
            }
        }
        else if (code & kControlCodeGroupUpDownload)
        {
            std::size_t buffer_size = packet.u_body.data.size;
            STDSC_LOG_TRACE("data size: %lu", buffer_size);
            Buffer buffer(buffer_size);
            sock.recv_buffer(buffer);
            if (map_.count(code))
            {
                map_[code]->eval(code, buffer, sock, state);
            }
        }
        
    }

private:
    std::unordered_map<uint64_t, std::shared_ptr<CallbackFunction>> map_;
};

CallbackFunctionContainer::CallbackFunctionContainer(void) : pimpl_(new Impl())
{
}

CallbackFunctionContainer::~CallbackFunctionContainer(void)
{
}

void CallbackFunctionContainer::set(uint64_t code,
                                    std::shared_ptr<CallbackFunction>& func)
{
    pimpl_->set(code, func);
}

void CallbackFunctionContainer::eval(const Socket& sock, const Packet& packet,
                                     StateContext& state)
{
    pimpl_->eval(sock, packet, state);
}

} /* namespace stdsc */
