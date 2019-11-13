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

#ifndef CALLBACK_FUNCTION_HPP
#define CALLBACK_FUNCTION_HPP

#include <string>
#include <cstdint>
#include <stdsc/stdsc_callback_function.hpp>

namespace stdsc
{
class StateContext;
class Buffer;
class Socket;
}

namespace server
{

struct CallbackParam
{
    uint32_t valueA;
    uint32_t valueB;
    uint32_t sum;
};

DECLARE_DATA_CLASS(CallbackFunctionForValueA);
DECLARE_DATA_CLASS(CallbackFunctionForValueB);
DECLARE_REQUEST_CLASS(CallbackFunctionForComputeRequest);
#if 1
DECLARE_UPDOWNLOAD_CLASS(CallbackFunctionForResultRequest);
#else
DECLARE_DOWNLOAD_CLASS(CallbackFunctionForResultRequest);
#endif

} /* namespace server */

#endif /* CALLBACK_FUNCTION_HPP */
