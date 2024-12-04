/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "wmsdk_config.h"

#include <string.h>
#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/netifapi.h"

#define LOG_TAG "lwip"
#include "wm_log.h"

WM_EVENT_DEFINE_GROUP(WM_LWIP_EV);

void wm_lwip_print(const char *format, ...)
{
    va_list list;
    va_start(list, format);
    if (format) {
        wm_log_vprintf(WM_LOG_LEVEL_DEBUG, LOG_TAG, format, list);
    }
    va_end(list);
}

#if LWIP_IPV4
#ifdef LWIP_HOOK_IP4_ROUTE_SRC
struct netif *wm_ip4_route_src(const ip4_addr_t *src, const ip4_addr_t *dest)
{
    struct netif *netif = NULL;

    /* select netif whose ip is equal to src*/
    if ((src != NULL) && !ip4_addr_isany(src)) {
        for (netif = netif_list; netif != NULL; netif = netif->next) {
            if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
                if (ip4_addr_cmp(src, netif_ip4_addr(netif))) {
                    return netif;
                }
            }
        }
    }
    return netif;
}
#endif //LWIP_HOOK_IP4_ROUTE_SRC
#endif //LWIP_IPV4