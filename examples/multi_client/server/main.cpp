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
#include <memory>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <share/packet.hpp>
#include <share/define.hpp>
#include <server/state.hpp>
#include <server/callback_function.hpp>

static void run()
{
    stdsc::StateContext state(std::make_shared<server::StateInit>());

    stdsc::CallbackFunctionContainer callback;
    server::CallbackParam param;
    {
        std::shared_ptr<stdsc::CallbackFunction> cb_valueA(
            new server::CallbackFunctionForValueA());
        callback.set(share::kControlCodeValueA, cb_valueA);

        std::shared_ptr<stdsc::CallbackFunction> cb_valueB(
            new server::CallbackFunctionForValueB());
        callback.set(share::kControlCodeValueB, cb_valueB);

        std::shared_ptr<stdsc::CallbackFunction> cb_compute(
            new server::CallbackFunctionForComputeRequest());
        callback.set(share::kControlCodeRequestCompute, cb_compute);

        std::shared_ptr<stdsc::CallbackFunction> cb_result(
            new server::CallbackFunctionForResultRequest());
        callback.set(share::kControlCodeDownloadResult, cb_result);
    }
    callback.set_commondata(static_cast<void*>(&param), sizeof(param));

    std::shared_ptr<stdsc::Server<>> server
        (new stdsc::Server<>(SERVER_PORT, state, callback));
    bool enable_async_mode = true;
    server->start(enable_async_mode);

    std::string key;
    std::cout << "hit any key to exit server: " << std::endl;
    std::cin >> key;

    server->stop();
    server->wait();
}    

int main()
{
    try
    {
        STDSC_INIT_LOG();
        STDSC_LOG_INFO("Start server");
        run();
    }
    catch (stdsc::AbstractException &e)
    {
        std::cerr << "catch exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "catch unknown exception" << std::endl;
    }
    return 0;
}
