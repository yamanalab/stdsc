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

#include <iostream>
#include <cstring>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <share/packet.hpp>
#include <server/callback_function.hpp>
#include <server/state.hpp>

namespace server
{

DEFUN_DATA(CallbackFunctionForValueA)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateInit <= state.current_state(),
      "Warn: must be connected state to receive valueA.");
    std::cout << "Received valueA." << std::endl;
    DEF_CDATA(server::CallbackParam);
    cdata->valueA = *static_cast<const uint32_t*>(buffer.data());
    state.set(kEventReceivedValueA);
}

DEFUN_DATA(CallbackFunctionForValueB)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateInit <= state.current_state(),
      "Warn: must be connected state to receive valueB.");
    std::cout << "Received valueB." << std::endl;
    std::cout << "Called " << __FUNCTION__ << std::endl;
    DEF_CDATA(server::CallbackParam);
    cdata->valueB = *static_cast<const uint32_t*>(buffer.data());
    state.set(kEventReceivedValueB);
}

DEFUN_REQUEST(CallbackFunctionForComputeRequest)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady <= state.current_state(),
      "Warn: must be connected state to receive valueB.");
    std::cout << "Received compute request." << std::endl;
    std::cout << "Called " << __FUNCTION__ << std::endl;
    DEF_CDATA(server::CallbackParam);
    cdata->sum = cdata->valueA + cdata->valueB;
    state.set(kEventReceivedComputeRequest);
}

#if 1
DEFUN_UPDOWNLOAD(CallbackFunctionForResultRequest)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateComputed <= state.current_state(),
      "Warn: must be connected state to receive valueB.");
    std::cout << "Received result request." << std::endl;

    auto sumAB = *static_cast<const uint32_t*>(buffer.data());
    
    DEF_CDATA(server::CallbackParam);
    size_t size = sizeof(cdata->sum);
    
    stdsc::Buffer sbuffer(size);
    *static_cast<uint32_t*>(sbuffer.data()) = cdata->sum == sumAB;
    sock.send_packet(
      stdsc::make_data_packet(share::kControlCodeDataResult, size));
    sock.send_buffer(sbuffer);
    state.set(kEventReceivedResultRequest);
}
#else
DEFUN_DOWNLOAD(CallbackFunctionForResultRequest)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateComputed <= state.current_state(),
      "Warn: must be connected state to receive valueB.");
    std::cout << "Received result request." << std::endl;
    DEF_CDATA(server::CallbackParam);
    size_t size = sizeof(cdata->sum);
    
    stdsc::Buffer buffer(size);
    *static_cast<uint32_t*>(buffer.data()) = cdata->sum;
    sock.send_packet(
      stdsc::make_data_packet(share::kControlCodeDataResult, size));
    sock.send_buffer(buffer);
    state.set(kEventReceivedResultRequest);
}
#endif
    
} /* namespace server */
