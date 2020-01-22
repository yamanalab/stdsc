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
#include <vector>
#include <cstring>
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
    Impl(void)
        : cdata_on_all_(),
          cdata_on_each_()
    {
    }
    ~Impl(void) = default;

    void set(uint64_t code, std::shared_ptr<CallbackFunction>& func)
    {
        STDSC_LOG_TRACE("set func for 0x%x.", code);
        funcmap_.emplace(code, func);
    }

    void eval(const Socket& sock, const Packet& packet, StateContext& state)
    {
        cdatamap_.emplace(sock.connection_id(), cdata_on_each_);
        void* cdata_on_each = nullptr;
        if (!cdata_on_each_.empty()) {
            cdata_on_each = static_cast<void*>(cdatamap_[sock.connection_id()].data());
        }
        void* cdata_on_all = (cdata_on_all_.empty()) ? nullptr : cdata_on_all_.data();
        
        auto code = static_cast<uint64_t>(packet.control_code);
        STDSC_LOG_TRACE("eval for 0x%x.", code);
        if (code & kControlCodeGroupRequest)
        {
            if (funcmap_.count(code))
            {
                funcmap_[code]->eval(code, state, cdata_on_each, cdata_on_all);
            }
        }
        else if (code & kControlCodeGroupData)
        {
            std::size_t buffer_size = packet.u_body.data.size;
            STDSC_LOG_TRACE("data size: %lu", buffer_size);
            Buffer buffer(buffer_size);
            sock.recv_buffer(buffer);
            if (funcmap_.count(code))
            {
                funcmap_[code]->eval(code, buffer, state, cdata_on_each, cdata_on_all);
            }
        }
        else if (code & kControlCodeGroupDownload)
        {
            if (funcmap_.count(code))
            {
                funcmap_[code]->eval(code, sock, state, cdata_on_each, cdata_on_all);
            }
        }
        else if (code & kControlCodeGroupUpDownload)
        {
            std::size_t buffer_size = packet.u_body.data.size;
            STDSC_LOG_TRACE("data size: %lu", buffer_size);
            Buffer buffer(buffer_size);
            sock.recv_buffer(buffer);
            if (funcmap_.count(code))
            {
                funcmap_[code]->eval(code, buffer, sock, state, cdata_on_each, cdata_on_all);
            }
        }
        
    }

    void set_commondata(const void* data, const size_t size, const CommonDataKind_t kind)
    {
        switch (kind) {
        case kCommonDataOnAllConnection:
            cdata_on_all_.clear();
            cdata_on_all_.resize(size);
            std::memcpy(cdata_on_all_.data(), data, size);
            break;
        case kCommonDataOnEachConnection:
        default:
            cdata_on_each_.clear();
            cdata_on_each_.resize(size);
            std::memcpy(cdata_on_each_.data(), data, size);
        }
    }

private:
    std::vector<uint8_t> cdata_on_all_; ///< common data on all connection
    std::vector<uint8_t> cdata_on_each_; ///< common data on each connection
    std::unordered_map<uint64_t, std::shared_ptr<CallbackFunction>> funcmap_; ///< func map for each control code
    std::unordered_map<int, std::vector<uint8_t>> cdatamap_; ///< common data map on each connection
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

void CallbackFunctionContainer::set_commondata(const void* data, const size_t size,
                                               const CommonDataKind_t kind)
{
    pimpl_->set_commondata(data, size, kind);
}

} /* namespace stdsc */
