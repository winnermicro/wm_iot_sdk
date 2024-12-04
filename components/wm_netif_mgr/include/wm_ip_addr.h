/**
 * @file wm_ip_addr.h
 *
 * @brief WM ip address Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef __WM_IP_ADDR_H__
#define __WM_IP_ADDR_H__
#include "wm_types.h"
#include "wmsdk_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WM_IPADDR_TYPE_V4  = 0U, /** IPv4 */
    WM_IPADDR_TYPE_V6  = 6U, /** IPv6 */
    WM_IPADDR_TYPE_ANY = 46U /** IPv4+IPv6 ("dual-stack") */
} wm_ip_addr_type_t;

typedef struct {
    uint32_t addr[4];
    uint8_t zone;
} wm_ip6_addr_t;

typedef struct {
    uint32_t addr;
} wm_ip4_addr_t;

typedef struct {
    union {
        wm_ip6_addr_t ip6;
        wm_ip4_addr_t ip4;
    } u_addr;
    uint8_t type; /**@ref wm_ip_addr_type_t */
} wm_ip_addr_t;

#define wm_ip_addr_set_ip4_u32(ipaddr, val)            \
    do {                                               \
        (ipaddr)->u_addr.ip4.addr    = (val);          \
        (ipaddr)->type               = IPADDR_TYPE_V4; \
        (ipaddr)->u_addr.ip6.addr[1] = 0;              \
        (ipaddr)->u_addr.ip6.addr[2] = 0;              \
        (ipaddr)->u_addr.ip6.addr[3] = 0;              \
        (ipaddr)->u_addr.ip6.zone    = 0;              \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif //__WM_IP_ADDR_H__
