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

#include <sstream>

#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_exception.hpp>

namespace stdsc
{

Packet::Packet(void) : control_code(kControlCodeNil)
{
    std::fill(&u_body.padding[0], &u_body.padding[STDSC_PACKET_BODY_SIZE], 0);
}

Packet::Packet(uint64_t control_code_) : control_code(control_code_)
{
    std::fill(&u_body.padding[0], &u_body.padding[STDSC_PACKET_BODY_SIZE], 0);
}

void initialize_packet(Packet& packet)
{
    packet.control_code = kControlCodeNil;
    std::fill(&packet.u_body.padding[0],
              &packet.u_body.padding[STDSC_PACKET_BODY_SIZE], 0);
}

Packet make_data_packet(uint64_t control_code, uint64_t size)
{
    STDSC_IF_CHECK((static_cast<uint64_t>(control_code) & kControlCodeGroupData ||
                    static_cast<uint64_t>(control_code) & kControlCodeGroupUpDownload),
                    "invalid control code");
    Packet packet(control_code);
    packet.u_body.data.size = size;
    return packet;
}

Packet make_packet(uint64_t control_code)
{
    return make_enum_field_packet(control_code, 0);
}

} /* namespace stdsc_packet */
