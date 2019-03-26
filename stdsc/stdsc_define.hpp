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

#ifndef STDSC_DEFINE_HPP
#define STDSC_DEFINE_HPP

#define STDSC_INFINITE 0xFFFFFFFF
#define STDSC_RETRY_INTERVAL_USEC (1000000)

#define STDSC_TCP_BUFFER_SIZE (1 * 1024 * 1024)
#define STDSC_CONN_TIMEOUT_SEC (30)

#define STDSC_CLOUD_PORT_QUERIER "10001"
#define STDSC_CLOUD_PORT_DATAOWA "10002"
#define STDSC_CLOUD_PORT_DATAOWB "10003"
#define STDSC_DATAA_PORT_QUERIER "20001"
#define STDSC_DATAB_PORT_QUERIER "20002"

#endif /* STDSC_DEFINE_HPP */
