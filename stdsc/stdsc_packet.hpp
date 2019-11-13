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

#ifndef STDSC_PACKET_HPP
#define STDSC_PACKET_HPP

#include <cstdint>
#include <string>

namespace stdsc
{

static const uint32_t STDSC_PACKET_BODY_SIZE = 1024;
static const uint32_t STDSC_FIXED_STRING_SIZE = 1024;

/**
 * @brief Enumeration for control code of packet.
 * 0x100 to 0x1FF are reservation numbers.
 */
enum ControlCode_t : uint64_t
{
    kControlCodeNil = 0x0,

    /* Reserved Code: 0x0100-0x01FF */
    kControlCodeGroupReserved   = 0x0100,
    kControlCodeAccept          = 0x0101,
    kControlCodeReject          = 0x0102,
    kControlCodeFailed          = 0x0103,
    kControlCodeConnected       = 0x0104,
    kControlCodeDisConnected    = 0x0105,

    /* Code for Request packet: 0x0200-0x02FF */
    kControlCodeGroupRequest    = 0x0200,

    /* Code for Data packet: 0x0400-0x04FF */
    kControlCodeGroupData       = 0x0400,

    /* Code for Download packet: 0x0800-0x08FF */
    kControlCodeGroupDownload   = 0x0800,

    /* Code for UpDownload packet: 0x1000-0x10FF */
    kControlCodeGroupUpDownload = 0x1000,
};

struct DataHeader
{
    uint64_t size;
};

struct FixedStringBody
{
    char str[STDSC_FIXED_STRING_SIZE];
};

struct EnumFieldBody
{
    uint64_t val;
};

union Body
{
    uint8_t padding[STDSC_PACKET_BODY_SIZE];
    DataHeader data;
    FixedStringBody fixed_string;
    EnumFieldBody enum_field;
};

/**
 * @brief This class is used to hold the packet data.
 */
struct Packet
{
    uint64_t control_code;
    Body u_body;

    Packet(void);
    Packet(uint64_t control_code_);
};

void initialize_packet(Packet& packet);
Packet make_data_packet(uint64_t control_code, uint64_t size);
Packet make_fixed_string_packet(const std::string string_);
Packet make_fixed_string_packet(int32_t val);
Packet make_packet(uint64_t control_code);

template <class T>
static Packet make_enum_field_packet(uint64_t control_code, T enum_val)
{
    Packet packet(control_code);
    packet.u_body.enum_field.val = static_cast<uint64_t>(enum_val);
    return packet;
}

} /* namespace stdsc */

#endif /* STDSC_PACKET_HPP */
