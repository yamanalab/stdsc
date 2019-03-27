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

void CallbackFunctionForValueA::data_function(uint64_t code,
                                              const stdsc::Buffer& buffer,
                                              stdsc::StateContext& state)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected <= state.current_state(),
      "Warn: must be connected state to receive valueA.");
    std::cout << "Received valueA." << std::endl;
    param_.valueA = *static_cast<const uint32_t*>(buffer.data());
    state.set(kEventReceivedValueA);
}
DEFINE_REQUEST_FUNC(CallbackFunctionForValueA);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionForValueA);

void CallbackFunctionForValueB::data_function(uint64_t code,
                                              const stdsc::Buffer& buffer,
                                              stdsc::StateContext& state)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected <= state.current_state(),
      "Warn: must be connected state to receive valueB.");
    std::cout << "Received valueB." << std::endl;
    std::cout << "Called " << __FUNCTION__ << std::endl;
    param_.valueB = *static_cast<const uint32_t*>(buffer.data());
    state.set(kEventReceivedValueB);
}
DEFINE_REQUEST_FUNC(CallbackFunctionForValueB);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionForValueB);

void CallbackFunctionForComputeRequest::request_function(
  uint64_t code, stdsc::StateContext& state)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady <= state.current_state(),
      "Warn: must be connected state to receive valueB.");
    std::cout << "Received compute request." << std::endl;
    std::cout << "Called " << __FUNCTION__ << std::endl;
    param_.sum = param_.valueA + param_.valueB;
    state.set(kEventReceivedComputeRequest);
}
DEFINE_DATA_FUNC(CallbackFunctionForComputeRequest);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionForComputeRequest);

void CallbackFunctionForResultRequest::download_function(
  uint64_t code, const stdsc::Socket& sock, stdsc::StateContext& state)
{
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateComputed <= state.current_state(),
      "Warn: must be connected state to receive valueB.");
    std::cout << "Received result request." << std::endl;
    size_t size = sizeof(param_.sum);
    stdsc::Buffer buffer(size);
    *static_cast<uint32_t*>(buffer.data()) = param_.sum;
    sock.send_packet(
      stdsc::make_data_packet(share::kControlCodeDataResult, size));
    sock.send_buffer(buffer);
    state.set(kEventReceivedResultRequest);
}
DEFINE_REQUEST_FUNC(CallbackFunctionForResultRequest);
DEFINE_DATA_FUNC(CallbackFunctionForResultRequest);

} /* namespace server */
