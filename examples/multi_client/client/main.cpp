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
#include <random>
#include <chrono>
#include <cstdio>
#include <sstream>
#include <memory>
#include <array>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_thread.hpp>
#include <share/define.hpp>
#include <share/packet.hpp>

static constexpr uint32_t NTHREAD = 100;
static constexpr uint32_t VALUE_A = 10;
static constexpr uint32_t VALUE_B = 20;
static constexpr uint32_t SUM_AB  = 30; // expected value of sum(A, B)

#define LOG(fmt, ...) printf("[%s] " fmt "\n", thread_id.c_str(), ##__VA_ARGS__)

template <class T>
class ClientThread : public stdsc::Thread<T>
{
public:
    ClientThread(const char* host, const char* port)
        : host_(host), port_(port)
    {}
    ~ClientThread(void)
    {}

private:
    virtual void exec(T& args, std::shared_ptr<stdsc::ThreadException> te) const override
    {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        auto thread_id = oss.str();
        
        std::random_device seed_gen;
        std::mt19937 mt(seed_gen());
        std::uniform_int_distribution<> dist(1, 4);

        auto interval_sec_A = dist(mt);
        auto interval_sec_B = dist(mt);
        
        stdsc::Client client;
        client.connect(host_, port_);
        std::cout << "Connected to server." << std::endl;

        size_t sz = sizeof(uint32_t);
        uint32_t va = args.valA;
        uint32_t vb = args.valB;
        uint32_t vr = args.sumAB;

        LOG("sleep %u sec before sending valueA", interval_sec_A);
        std::this_thread::sleep_for(std::chrono::seconds(interval_sec_A));

        // Send VALUE_A
        stdsc::Buffer bufferA(sz);
        std::memcpy(bufferA.data(), static_cast<void*>(&va), sz);
        client.send_data_blocking(share::kControlCodeValueA, bufferA);
        LOG("Sent valueA.");

        LOG("sleep %u sec before sending valueB", interval_sec_B);
        std::this_thread::sleep_for(std::chrono::seconds(interval_sec_B));

        // Send VALUE_B
        stdsc::Buffer bufferB(sz);
        std::memcpy(bufferB.data(), static_cast<void*>(&vb), sz);
        client.send_data_blocking(share::kControlCodeValueB, bufferB);
        LOG("Sent valueB.");

        // Send Compute Request
        client.send_request_blocking(share::kControlCodeRequestCompute);
        LOG("Sent compute request.");

#if 1
        stdsc::Buffer bufferR(sz);
        std::memcpy(bufferR.data(), static_cast<void*>(&vr), sz);
        stdsc::Buffer result;
        client.send_recv_data_blocking(share::kControlCodeDownloadResult, bufferR, result);
        LOG("Sent expected value and received result.");
#else
        stdsc::Buffer result;
        client.recv_data_blocking(share::kControlCodeDownloadResult, result);
        LOG("Sent result request and received result.");
#endif
        
        client.close();

        args.thread_id = thread_id;
        args.result    = *static_cast<const uint32_t*>(result.data());
    }
private:
    const char* host_;
    const char* port_;
};

struct Param
{
    uint32_t valA;
    uint32_t valB;
    uint32_t sumAB;
    uint32_t result;
    std::string thread_id;
};

static void run(const uint32_t nthread)
{
    const char* host = SERVER_HOST;
    const char* port = SERVER_PORT;

    std::vector<std::shared_ptr<ClientThread<Param>>> cli(nthread);
    std::vector<Param> param(nthread);

    for (uint32_t i=0; i<nthread; ++i) {
        param[i].valA   = VALUE_A;
        param[i].valB   = VALUE_B;
        param[i].sumAB  = SUM_AB;
        param[i].result = 0;
        auto te = stdsc::ThreadException::create();
        cli[i] = std::make_shared<ClientThread<Param>>(host, port);
        cli[i]->start(param[i], te);
    }
    for (uint32_t i=0; i<NTHREAD; ++i) {
        cli[i]->join();
        std::cout << "Result of " << param[i].thread_id
                  << " : " << param[i].result << std::endl;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        STDSC_INIT_LOG();
        STDSC_LOG_INFO("Start client");
        run(NTHREAD);
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
