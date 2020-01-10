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

#ifndef PACKET_HPP
#define PACKET_HPP

#include <cstdint>

namespace share
{

/**
 * @brief Enumeration for control code of packet.
 */
enum ControlCode_t : uint64_t
{
    /* Code for Request packet: 0x0201-0x02FF */
    kControlCodeRequestCompute = 0x0201,

    /* Code for Data packet: 0x0401-0x04FF */
    kControlCodeValueA         = 0x0401,
    kControlCodeValueB         = 0x0402,
    kControlCodeDataResult     = 0x0403,

#if 1
    /* Code for UpDownload packet: 0x1000-0x10FF */
    kControlCodeDownloadResult  = 0x1001,
#else
    /* Code for Download packet: 0x0801-0x08FF */
    kControlCodeDownloadResult  = 0x0801,
#endif
};

} /* namespace opsica */

#endif /* PACKET_HPP */
