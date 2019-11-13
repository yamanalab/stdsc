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

#include <cstring>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_log.hpp>
#include <share/define.hpp>
#include <share/packet.hpp>

static constexpr uint32_t VALUE_A = 10;
static constexpr uint32_t VALUE_B = 20;

static uint32_t run(void)
{
    const char* host = SERVER_HOST;
    const char* port = SERVER_PORT;

    stdsc::Client client;
    client.connect(host, port);
    std::cout << "Connected to server." << std::endl;

    size_t sz = sizeof(uint32_t);
    uint32_t va = VALUE_A;
    uint32_t vb = VALUE_B;

    stdsc::Buffer bufferA(sz);
    std::memcpy(bufferA.data(), static_cast<void*>(&va), sz);
    client.send_data_blocking(share::kControlCodeValueA, bufferA);
    std::cout << "Sent valueA." << std::endl;

    stdsc::Buffer bufferB(sz);
    std::memcpy(bufferB.data(), static_cast<void*>(&vb), sz);
    client.send_data_blocking(share::kControlCodeValueB, bufferB);
    std::cout << "Sent valueB." << std::endl;

    client.send_request_blocking(share::kControlCodeRequestCompute);
    std::cout << "Sent compute request." << std::endl;

    stdsc::Buffer result;
    client.recv_data_blocking(share::kControlCodeDownloadResult, result);
    std::cout << "Sent result request and received result." << std::endl;

    client.close();

    return *static_cast<const uint32_t*>(result.data());
}

int main(int argc, char* argv[])
{
    try
    {
        STDSC_INIT_LOG();
        STDSC_LOG_INFO("Start client");
        std::cout << "Result: " << run() << std::endl;
    }
    catch (stdsc::AbstractException& e)
    {
        std::cerr << "Err: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Catch unknown exception" << std::endl;
    }

    return 0;
}
