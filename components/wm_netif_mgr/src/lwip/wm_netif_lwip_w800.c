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
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/igmp.h"
#include "lwip/mld6.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"

#include "wm_wifi_drv.h"
#include "wm_wifi_types.h"
#include "wm_netif.h"

#if LWIP_IGMP
/***
	action:
	IGMP_DEL_MAC_FILTER		0
	IGMP_ADD_MAC_FILTER		1
***/

int8_t wm_netif_lwip_igmp_mac_filter(struct netif *netif, const ip4_addr_t *group, enum netif_mac_filter_action action)
{
    u8_t m_addr[6] = { 0x01, 0x00, 0x5E };
    int ret        = ERR_OK;
    if (netif != wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif) {
        return ERR_OK;
    }
    LWIP_DEBUGF(IGMP_DEBUG, ("IPaddr: %d.%d.%d.%d\n", ip4_addr1(group), ip4_addr2(group), ip4_addr3(group), ip4_addr4(group)));

    //create group mac address:
    memcpy(m_addr + 3, (u8_t *)&(group->addr) + 1, 3);
    m_addr[3] &= 0x7F; //clear bit24

    LWIP_DEBUGF(IGMP_DEBUG, ("MACaddr: %x:%x:%x:%x:%x:%x\n", m_addr[0], m_addr[1], m_addr[2], m_addr[3], m_addr[4], m_addr[5]));

    if (action == IGMP_ADD_MAC_FILTER) {
        ret = wm_wifi_drv_set_multicast_key(m_addr, 0);
    } else if (action == IGMP_DEL_MAC_FILTER) {
        ret = wm_wifi_drv_set_multicast_key(m_addr, 1);
    }
    if (ret < 0)
        return ret;
    else
        return ERR_OK;
}

#if LWIP_IPV6
int8_t wm_netif_lwip_igmp6_mac_filter(struct netif *netif, const ip6_addr_t *group, enum netif_mac_filter_action action)
{
    int ret        = ERR_OK;
    u8_t m_addr[6] = { 0x33, 0x33, 0x00, 0x00, 0x00, 0x00 };

    if (netif != wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif) {
        return ERR_OK;
    }
    memcpy(m_addr + 2, (u8_t *)&group->addr[3], 4);

    LWIP_DEBUGF(IGMP_DEBUG, ("MACaddr: %x:%x:%x:%x:%x:%x\n", m_addr[0], m_addr[1], m_addr[2], m_addr[3], m_addr[4], m_addr[5]));

    if (action == IGMP_ADD_MAC_FILTER) {
        ret = wm_wifi_drv_set_multicast_key(m_addr, 0);
    } else if (action == IGMP_DEL_MAC_FILTER) {
        ret = wm_wifi_drv_set_multicast_key(m_addr, 1);
    }

    if (ret < 0) {
        return ret;
    } else {
        return ERR_OK;
    }
}
#endif /* LWIP_IPV6 */
#endif /* LWIP_IGMP */

int8_t wm_netif_lwip_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    int datalen    = 0;
    u8_t *buf      = wm_wifi_drv_acquire_buffer(p->tot_len);
    if (buf == NULL)
        return ERR_MEM;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* Drop the padding word */
#endif

    for (q = p; q != NULL; q = q->next) {
        /* Send data from(q->payload, q->len); */
        MEMCPY(buf + datalen, q->payload, q->len);
        datalen += q->len;
    }

#if CONFIG_WIFI_ENABLE_AP_FEATURE
    if (netif == wm_netif_get_netif(WM_NETIF_TYPE_WIFI_AP)->netif)
        wm_wifi_drv_release_buffer(buf, true);
    else
#endif
        wm_wifi_drv_release_buffer(buf, false);

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* Reclaim the padding word */
#endif

    return ERR_OK;
}

static struct pbuf *low_level_input(struct netif *netif, u8_t *buf, u32_t buf_len)
{
    struct pbuf *p = NULL, *q = NULL;
    u16_t s_len;
    u8_t *bufptr;

    /* Obtain the size of the packet and put it into the "len"
     * variable. */

    s_len  = buf_len;
    bufptr = buf;

#if ETH_PAD_SIZE
    s_len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, s_len, PBUF_RAM);

    if (p != NULL) {
#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* Iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable. */
            /* read data into(q->payload, q->len); */
            MEMCPY(q->payload, bufptr, q->len);
            bufptr += q->len;
        }
        /* Acknowledge that packet has been read(); */

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* Reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        /* Drop packet(); */
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;
}

static int wm_lwip_input(void *priv, u8_t *buf, u32_t buf_len)
{
    struct pbuf *p;
    struct netif *netif = priv;

    if (!netif)
        return 0;

    /* move received packet into a new pbuf */
    p = low_level_input(netif, buf, buf_len);
    if (p) {
        if (ERR_OK != netif->input(p, netif)) {
            LWIP_DEBUGF(NETIF_DEBUG, ("wm_lwip_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
        return 0;
    } else {
        return -1;
    }
}

void wm_netif_set_input_lwip(wm_netif_t *netif, bool enable)
{
    if (WM_NETIF_TYPE_WIFI_STA == netif->type)
        wm_wifi_drv_set_sta_rx_data_callback(enable ? wm_lwip_input : NULL, netif->netif);
    else
        wm_wifi_drv_set_ap_rx_data_callback(enable ? wm_lwip_input : NULL, netif->netif);
}
