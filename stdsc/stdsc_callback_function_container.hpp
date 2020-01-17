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

#ifndef STDSC_CALLBACK_FUNCTION_CONTAINER_HPP
#define STDSC_CALLBACK_FUNCTION_CONTAINER_HPP

#include <memory>
#include <vector>

namespace stdsc
{
class CallbackFunction;
class Socket;
class StateContext;
class Packet;

/**
 * @brief Enumeration for common data kind
 */
enum CommonDataKind_t
{
    kCommonDataUnknownKind     = -1,
    kCommonDataOnAllConnection = 0,
    kCommonDataOnEachConnection,
    num_of_CommonDataKind,
};

/**
 * @ brief This class is used to hold the callback functions.
 */
class CallbackFunctionContainer
{
public:
    CallbackFunctionContainer(void);
    virtual ~CallbackFunctionContainer(void);
    void set(uint64_t code, std::shared_ptr<CallbackFunction>& func);
    void eval(const Socket& sock, const Packet& packet, StateContext& state);
    void set_commondata(const void* data, const size_t size,
                        const CommonDataKind_t kind=kCommonDataOnEachConnection);
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};
} /* namespace stdsc */

#endif /* STDSC_CALLBACK_FUNCTION_CONTAINER_HPP */
