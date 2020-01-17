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

#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_socket.hpp>

namespace stdsc
{

void CallbackFunction::eval(uint64_t code, StateContext& state, void* commondata)
{
    request_function(code, state, commondata);
}

void CallbackFunction::eval(uint64_t code, const Buffer& buffer,
                            StateContext& state, void* commondata)
{
    data_function(code, buffer, state, commondata);
}

void CallbackFunction::eval(uint64_t code, const Socket& sock,
                            StateContext& state, void* commondata)
{
    download_function(code, sock, state, commondata);
}

void CallbackFunction::eval(uint64_t code, const Buffer& buffer,
                            const Socket& sock, StateContext& state, void* commondata)
{
    updownload_function(code, buffer, sock, state, commondata);
}

void CallbackFunction::request_function(uint64_t code, StateContext& state, void* commondata)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

void CallbackFunction::data_function(uint64_t code, const Buffer& buffer,
                                     StateContext& state, void* commondata)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

void CallbackFunction::download_function(uint64_t code, const Socket& sock,
                                         StateContext& state, void* commondata)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

    void CallbackFunction::updownload_function(uint64_t code, const Buffer& buffer,
                                               const Socket& sock, StateContext& state, void* commondata)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

} /* stdsc */
