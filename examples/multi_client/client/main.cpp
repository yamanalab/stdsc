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
#include <unistd.h>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_thread.hpp>
#include <share/define.hpp>
#include <share/packet.hpp>

static constexpr uint32_t DEF_NTHREAD = 4;

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
        
        stdsc::Client client;
        client.connect(host_, port_);
        std::cout << "Connected to server." << std::endl;

        size_t sz = sizeof(uint32_t);
        uint32_t va = args.valA;
        uint32_t vb = args.valB;
        uint32_t vr = args.sumAB;

        LOG("compute sum of %u and %u. expected result is %u", va, vb, vr);

        auto interval_sec_1 = va;
        auto interval_sec_2 = vb;

        LOG("sleep %u sec before sending valueA", interval_sec_1);
        std::this_thread::sleep_for(std::chrono::seconds(interval_sec_1));

        // Send VALUE_A
        stdsc::Buffer bufferA(sz);
        std::memcpy(bufferA.data(), static_cast<void*>(&va), sz);
        client.send_data_blocking(share::kControlCodeValueA, bufferA);
        LOG("Sent valueA. (val:%u)", va);

        // Send VALUE_B
        stdsc::Buffer bufferB(sz);
        std::memcpy(bufferB.data(), static_cast<void*>(&vb), sz);
        client.send_data_blocking(share::kControlCodeValueB, bufferB);
        LOG("Sent valueB. (val:%u)", vb);

        LOG("sleep %u sec before sending compute request", interval_sec_2);
        std::this_thread::sleep_for(std::chrono::seconds(interval_sec_2));
        
        // Send Compute Request
        client.send_request_blocking(share::kControlCodeRequestCompute);
        LOG("Sent compute request.");

#if 1
        stdsc::Buffer bufferR(sz);
        std::memcpy(bufferR.data(), static_cast<void*>(&vr), sz);
        stdsc::Buffer result;
        client.send_recv_data_blocking(share::kControlCodeDownloadResult, bufferR, result);
        LOG("Sent expected value, then compare result on server. (expected:%u)", vr);
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

struct Option
{
    uint32_t nthread = DEF_NTHREAD;
};

struct Param
{
    uint32_t valA;
    uint32_t valB;
    uint32_t sumAB;
    uint32_t result;
    std::string thread_id;
};

void init(Option& options, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "n:h")) != -1)
    {
        switch (opt)
        {
            case 'n':
                options.nthread = std::stol(optarg);
                break;
            case 'h':
            default:
                printf("Usage: %s [-n nthread]\n", argv[0]);
                exit(1);
        }
    }
}

static void run(const uint32_t nthread)
{
    const char* host = SERVER_HOST;
    const char* port = SERVER_PORT;

    std::vector<std::shared_ptr<ClientThread<Param>>> cli(nthread);
    std::vector<Param> param(nthread);

    std::cout << "Launch " << nthread << " clients" << std::endl;

    for (uint32_t i=0; i<nthread; ++i) {
        std::random_device seed_gen;
        std::mt19937 mt(seed_gen());
        std::uniform_int_distribution<> dist(1, 4);

        auto valA = dist(mt);
        auto valB = dist(mt);
        
        param[i].valA   = valA;
        param[i].valB   = valB;
        param[i].sumAB  = valA + valB;
        param[i].result = 0;
        auto te = stdsc::ThreadException::create();
        cli[i] = std::make_shared<ClientThread<Param>>(host, port);
        cli[i]->start(param[i], te);
    }

    uint32_t verify = 0;
    for (uint32_t i=0; i<nthread; ++i) {
        cli[i]->join();
        verify += param[i].result;
        std::string res = (param[i].result) ? "True" : "False";
        std::cout << "[" << param[i].thread_id << "] "
                  << "Result of comparision: " << res << std::endl;
    }

    std::cout << "Result of all comparision: "
              << ((verify == nthread) ? "True" : "False") << std::endl;
    
}

int main(int argc, char* argv[])
{
    try
    {
        STDSC_INIT_LOG();
        STDSC_LOG_INFO("Start client");
        Option opt;
        init(opt, argc, argv);
        run(opt.nthread);
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
