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

#ifndef STDSC_CALLBACK_FUNCTION_HPP
#define STDSC_CALLBACK_FUNCTION_HPP

#include <cstdint>
#include <memory>

#define DEFINE_REQUEST_FUNC(cls)                                          \
    void cls::request_function(uint64_t code, stdsc::StateContext& state) \
    {                                                                     \
    }

#define DEFINE_DATA_FUNC(cls)                                           \
    void cls::data_function(uint64_t code, const stdsc::Buffer& buffer, \
                            stdsc::StateContext& state)                 \
    {                                                                   \
    }

#define DEFINE_DOWNLOAD_FUNC(cls)                                         \
    void cls::download_function(uint64_t code, const stdsc::Socket& sock, \
                                stdsc::StateContext& state)               \
    {                                                                     \
    }

namespace stdsc
{
class Buffer;
class Socket;
class StateContext;

/**
 * @brief Defines a callback function interface.
 * That any implementation of this interface can be used by inheritor's to handle a callback function.
 */
class CallbackFunction
{
public:
    CallbackFunction(void) = default;
    ~CallbackFunction(void) = default;

    void eval(uint64_t code, StateContext& state);
    void eval(uint64_t code, const Buffer& buffer, StateContext& state);
    void eval(uint64_t code, const Socket& sock, StateContext& state);

protected:
    virtual void request_function(uint64_t code, StateContext& state);
    virtual void data_function(uint64_t code, const Buffer& buffer,
                               StateContext& state);
    virtual void download_function(uint64_t code, const Socket& sock,
                                   StateContext& state);
};
} /* namespace stdsc */

#endif /* STDSC_CALLBACK_FUNCTION_HPP */
