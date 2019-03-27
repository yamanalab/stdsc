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
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <share/packet.hpp>
#include <server/server.hpp>
#include <server/state.hpp>

namespace server
{

AddServer::AddServer(stdsc::StateContext &state, const char *port)
  : server_(new stdsc::Server<>(port, state))
{

    stdsc::CallbackFunctionContainer callback;

    std::shared_ptr<stdsc::CallbackFunction> cb_valueA(
      new CallbackFunctionForValueA(param_));
    callback.set(share::kControlCodeValueA, cb_valueA);

    std::shared_ptr<stdsc::CallbackFunction> cb_valueB(
      new CallbackFunctionForValueB(param_));
    callback.set(share::kControlCodeValueB, cb_valueB);

    std::shared_ptr<stdsc::CallbackFunction> cb_compute(
      new CallbackFunctionForComputeRequest(param_));
    callback.set(share::kControlCodeRequestCompute, cb_compute);

    std::shared_ptr<stdsc::CallbackFunction> cb_result(
      new CallbackFunctionForResultRequest(param_));
    callback.set(share::kControlCodeDownloadResult, cb_result);

    server_->set_callback(callback);
    state.set(kEventConnectSocket);
}

void AddServer::start(void)
{
    server_->start();
}

void AddServer::join(void)
{
    server_->wait_for_finish();
}

} /* namespace server */
