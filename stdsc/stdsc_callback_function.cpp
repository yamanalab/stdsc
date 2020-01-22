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

void CallbackFunction::eval(uint64_t code, StateContext& state,
                            void* cdata_on_each, void* cdata_on_all)
{
    request_function(code, state, cdata_on_each, cdata_on_all);
}

void CallbackFunction::eval(uint64_t code, const Buffer& buffer,
                            StateContext& state,
                            void* cdata_on_each, void* cdata_on_all)
{
    data_function(code, buffer, state, cdata_on_each, cdata_on_all);
}

void CallbackFunction::eval(uint64_t code, const Socket& sock,
                            StateContext& state,
                            void* cdata_on_each, void* cdata_on_all)
{
    download_function(code, sock, state, cdata_on_each, cdata_on_all);
}

void CallbackFunction::eval(uint64_t code, const Buffer& buffer,
                            const Socket& sock, StateContext& state,
                            void* cdata_on_each, void* cdata_on_all)
{
    updownload_function(code, buffer, sock, state, cdata_on_each, cdata_on_all);
}

void CallbackFunction::request_function(uint64_t code, StateContext& state,
                                        void* cdata_on_each, void* cdata_on_all)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

void CallbackFunction::data_function(uint64_t code, const Buffer& buffer,
                                     StateContext& state,
                                     void* cdata_on_each, void* cdata_on_all)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

void CallbackFunction::download_function(uint64_t code, const Socket& sock,
                                         StateContext& state,
                                         void* cdata_on_each, void* cdata_on_all)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

void CallbackFunction::updownload_function(uint64_t code, const Buffer& buffer,
                                           const Socket& sock, StateContext& state,
                                           void* cdata_on_each, void* cdata_on_all)
{
    STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
}

} /* stdsc */
