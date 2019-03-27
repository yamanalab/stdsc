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
#include <stdsc/stdsc_exception.hpp>
#include <server/server.hpp>
#include <server/state.hpp>

int main()
{
    try
    {
        stdsc::StateContext state(std::make_shared<server::StateInit>());

        server::AddServer server(state, SERVER_PORT);
        server.start();
        server.join();
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
