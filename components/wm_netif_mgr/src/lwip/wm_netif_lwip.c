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

#include "wm_error.h"
#include <string.h>
#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/igmp.h"
#include "lwip/mld6.h"
#include "lwip/inet.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"
#include "lwip/lwip_napt.h"
#include "lwip/apps/dhcp_server.h"
#include "lwip/apps/dns_server.h"
#include "wm_netif.h"
#include "wm_osal.h"
#include "wm_event.h"
#include "wm_utils.h"
#include "wm_netif_lwip.h"

#if CONFIG_COMPONENT_WIFI_ENABLED
#include "wm_wifi_types.h"
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */

#define LOG_TAG "netif"
#include "wm_log.h"

/** Network link speed */
#define NET_LINK_SPEED 100000000

NETIF_DECLARE_EXT_CALLBACK(lwip_netif_ext_status);
extern err_t wm_netif_lwip_output(struct netif *netif, struct pbuf *p);
#if LWIP_IGMP
extern err_t wm_netif_lwip_igmp_mac_filter(struct netif *netif, const ip4_addr_t *group, enum netif_mac_filter_action action);
#if LWIP_IPV6
extern int8_t wm_netif_lwip_igmp6_mac_filter(struct netif *netif, const ip6_addr_t *group, enum netif_mac_filter_action action);
#endif /* LWIP_IPV6 */
#endif

static inline void wm_ipaddr_copy_to_lwip_ipaddr(const wm_ip_addr_t *wm_ipaddr, ip_addr_t *lwip_ipaddr)
{
    if (WM_IPADDR_TYPE_V4 == wm_ipaddr->type) {
#if LWIP_IPV4
        ip_addr_set_ip4_u32(lwip_ipaddr, wm_ipaddr->u_addr.ip4.addr);
#endif //LWIP_IPV4
    }
#if LWIP_IPV6
    else {
#if LWIP_IPV4
        memcpy(&(lwip_ipaddr->u_addr.ip6.addr[0]), &(wm_ipaddr->u_addr.ip6.addr[0]), 16);
#if LWIP_IPV6_SCOPES
        lwip_ipaddr->u_addr.ip6.zone = wm_ipaddr->u_addr.ip6.zone;
#endif //LWIP_IPV6_SCOPES
        lwip_ipaddr->type = IPADDR_TYPE_V6;
#else //LWIP_IPV4
        memcpy(&(lwip_ipaddr->addr[0]), &(wm_ipaddr->u_addr.ip6.addr[0]), 16);
#if LWIP_IPV6_SCOPES
        lwip_ipaddr->zone = wm_ipaddr->u_addr.ip6.zone;
#endif //LWIP_IPV6_SCOPES
#endif //LWIP_IPV4
    }
#endif //LWIP_IPV6
}

static inline void lwip_ipaddr_copy_to_wm_ipaddr(const ip_addr_t *lwip_ipaddr, wm_ip_addr_t *wm_ipaddr)
{
    if (IPADDR_TYPE_V4 == IP_GET_TYPE(lwip_ipaddr)) {
#if LWIP_IPV4
        wm_ipaddr->u_addr.ip4.addr = ip_addr_get_ip4_u32(lwip_ipaddr);
        memset(&(wm_ipaddr->u_addr.ip6.addr[1]), 0, 12);
#endif //LWIP_IPV4
#if LWIP_IPV6_SCOPES
        wm_ipaddr->u_addr.ip6.zone = 0;
#endif //LWIP_IPV6_SCOPES
        wm_ipaddr->type = WM_IPADDR_TYPE_V4;
    }
#if LWIP_IPV6
    else {
#if LWIP_IPV4
        memcpy(&(wm_ipaddr->u_addr.ip6.addr[0]), &(lwip_ipaddr->u_addr.ip6.addr[0]), 16);
#if LWIP_IPV6_SCOPES
        wm_ipaddr->u_addr.ip6.zone = lwip_ipaddr->u_addr.ip6.zone;
#endif //LWIP_IPV6_SCOPES
#else  //LWIP_IPV4
        memcpy(&(wm_ipaddr->u_addr.ip6.addr[0]), &(lwip_ipaddr->addr[0]), 16);
#if LWIP_IPV6_SCOPES
        wm_ipaddr->u_addr.ip6.zone = lwip_ipaddr->zone;
#endif //LWIP_IPV6_SCOPES
#endif //LWIP_IPV4
        wm_ipaddr->type = WM_IPADDR_TYPE_V6;
    }
#endif //LWIP_IPV6
}

static void lwip_low_level_init(struct netif *netif)
{
    netif->mtu = WM_NETIF_MTU;

#if CONFIG_LWIP_IPV6_AUTOCONFIG
    netif_set_ip6_autoconfig_enabled(netif, 1);
#endif
    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_LINK_UP
#if defined(DHCP_USED)
                    | NETIF_FLAG_DHCP
#endif
        ;

#if LWIP_IPV6 && LWIP_IPV6_MLD
    netif->flags |= NETIF_FLAG_MLD6;

    /*
   * For hardware/netifs that implement MAC filtering.
   * All-nodes link-local is handled by default, so we must let the hardware know
   * to allow multicast packets in.
   * Should set mld_mac_filter previously. */
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

    /* Do whatever else is needed to initialize interface. */
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
static err_t lwip_netif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, NET_LINK_SPEED);

    /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#if LWIP_IGMP
    netif->igmp_mac_filter = wm_netif_lwip_igmp_mac_filter;
#endif /* LWIP_IGMP */
#endif //LWIP_IPV4
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#if LWIP_IGMP
    netif_set_mld_mac_filter(netif, wm_netif_lwip_igmp6_mac_filter);
#endif /* LWIP_IGMP */
#endif /* LWIP_IPV6 */
    netif->linkoutput = wm_netif_lwip_output;

    /* initialize the hardware */
    lwip_low_level_init(netif);

    return ERR_OK;
}

static void lwip_netif_ext_callback(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
#if !defined(CONFIG_UNIT_TEST_LWIP_ETHARP) && !defined(CONFIG_UNIT_TEST_LWIP_UDP)

    if (reason & LWIP_NSC_STATUS_CHANGED) {
        if (netif_is_up((struct netif *)(wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif))) {
            netif_set_default(wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif);
        }
#if CONFIG_WIFI_ENABLE_AP_FEATURE
        else if (netif_is_up((struct netif *)(wm_netif_get_netif(WM_NETIF_TYPE_WIFI_AP)->netif))) {
            netif_set_default(wm_netif_get_netif(WM_NETIF_TYPE_WIFI_AP)->netif);
        }
#endif
#if CONFIG_WM_NETIF_ENABLE_ETH
        else if (netif_is_up((struct netif *)(wm_netif_get_netif(WM_NETIF_TYPE_ETH)->netif))) {
            netif_set_default(wm_netif_get_netif(WM_NETIF_TYPE_ETH)->netif);
        }
#endif
        else {
            netif_set_default(wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif);
        }
    }

#if LWIP_IPV4
    if (reason & LWIP_NSC_IPV4_SETTINGS_CHANGED) {
        if (!ip_addr_isany(&netif->ip_addr)) {
            if (netif == wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif) {
                wm_lwip_event_data_t data;
                data.sta_got_ip_info.ip      = ip_addr_get_ip4_u32(&netif->ip_addr);
                data.sta_got_ip_info.gw      = ip_addr_get_ip4_u32(&netif->gw);
                data.sta_got_ip_info.netmask = ip_addr_get_ip4_u32(&netif->netmask);
                wm_event_send(WM_LWIP_EV, WM_EVENT_WIFI_STA_GOT_IP, &data, sizeof(data));
            }
#if CONFIG_WM_NETIF_ENABLE_ETH
            else if (netif == wm_netif_get_netif(WM_NETIF_TYPE_ETH)->netif) {
                wm_lwip_event_data_t data;
                data.eth_got_ip_info.ip      = ip_addr_get_ip4_u32(&netif->ip_addr);
                data.eth_got_ip_info.gw      = ip_addr_get_ip4_u32(&netif->gw);
                data.eth_got_ip_info.netmask = ip_addr_get_ip4_u32(&netif->netmask);
                wm_event_send(WM_LWIP_EV, WM_EVENT_ETH_GOT_IP, &data, sizeof(data));
            }
#endif
            else {
#if IP_NAPT
                ip_napt_enable_no(netif->num, 1);
#endif
            }
        } else {
            if (netif == wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif) {
                wm_event_send(WM_LWIP_EV, WM_EVENT_WIFI_STA_LOST_IP, NULL, 0);
            }
#if CONFIG_WM_NETIF_ENABLE_ETH
            else if (netif == wm_netif_get_netif(WM_NETIF_TYPE_ETH)->netif) {
                wm_event_send(WM_LWIP_EV, WM_EVENT_ETH_LOST_IP, NULL, 0);
            }
#endif
            else {
#if IP_NAPT
                ip_napt_enable_no(netif->num, 0);
#endif
            }
        }
    }
#endif
#if LWIP_IPV4 && LWIP_IPV6
    else
#endif
#if LWIP_IPV6
        if (reason & LWIP_NSC_IPV6_ADDR_STATE_CHANGED) {
        if (netif_ip6_addr_state(netif, args->ipv6_addr_state_changed.addr_index) & IP6_ADDR_VALID) {
            if ((netif == wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif) &&
                (args->ipv6_addr_state_changed.old_state > IP6_ADDR_TENTATIVE)) {
                wm_lwip_event_data_t data;
                data.sta_got_ip6_info.index = args->ipv6_addr_state_changed.addr_index;
#if LWIP_IPV4
                memcpy(data.sta_got_ip6_info.ip, args->ipv6_addr_state_changed.address->u_addr.ip6.addr,
                       sizeof(data.sta_got_ip6_info.ip));
#else
                memcpy(data.sta_got_ip6_info.ip, args->ipv6_addr_state_changed.address->addr, sizeof(data.sta_got_ip6_info.ip));
#endif
                wm_event_send(WM_LWIP_EV, WM_EVENT_WIFI_STA_GOT_IP6, &data, sizeof(data));
            }
#if CONFIG_WM_NETIF_ENABLE_ETH
            else if ((netif == wm_netif_get_netif(WM_NETIF_TYPE_ETH)->netif) &&
                     (args->ipv6_addr_state_changed.old_state > IP6_ADDR_TENTATIVE)) {
                wm_lwip_event_data_t data;
                data.eth_got_ip6_info.index = args->ipv6_addr_state_changed.addr_index;
#if LWIP_IPV4
                memcpy(data.eth_got_ip6_info.ip, args->ipv6_addr_state_changed.address->u_addr.ip6.addr,
                       sizeof(data.eth_got_ip6_info.ip));
#else
                memcpy(data.eth_got_ip6_info.ip, args->ipv6_addr_state_changed.address->addr, sizeof(data.eth_got_ip6_info.ip));
#endif
                wm_event_send(WM_LWIP_EV, WM_EVENT_ETH_GOT_IP6, &data, sizeof(data));
            }
#endif
        } else if (args->ipv6_addr_state_changed.old_state & IP6_ADDR_VALID) {
            if (netif == wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)->netif) {
                wm_lwip_event_data_t data;
                data.sta_lost_ip6_info.index = args->ipv6_addr_state_changed.addr_index;
                wm_event_send(WM_LWIP_EV, WM_EVENT_WIFI_STA_LOST_IP6, &data, sizeof(data));
            }
#if CONFIG_WM_NETIF_ENABLE_ETH
            else if (netif == wm_netif_get_netif(WM_NETIF_TYPE_ETH)->netif) {
                wm_lwip_event_data_t data;
                data.eth_lost_ip6_info.index = args->ipv6_addr_state_changed.addr_index;
                wm_event_send(WM_LWIP_EV, WM_EVENT_ETH_LOST_IP6, &data, sizeof(data));
            }
#endif
        }
    }
#endif
#endif
}

static void lwip_tcpip_init_done_callback(void *arg)
{
    wm_os_sem_t *sync_sem = arg;

    if (sync_sem) {
        if (wm_os_internal_sem_release(sync_sem)) {
            wm_log_error("tcpip sync failed");
        }
    }
}

void wm_netif_get_dns_lwip(uint8_t index, wm_ip_addr_t *ip)
{
    const ip_addr_t *dns = dns_getserver(index);

    if (IP_ADDR_ANY != dns) {
        lwip_ipaddr_copy_to_wm_ipaddr(dns, ip);
    }
}

void wm_netif_set_dns_lwip(uint8_t index, wm_ip_addr_t *ip)
{
    ip_addr_t addr;

    wm_ipaddr_copy_to_lwip_ipaddr(ip, &addr);

    dns_setserver(index, (const ip_addr_t *)&addr);
}

static void set_lwip_hostname_callback(struct netif *netif, void *ctx)
{
    if (netif && ctx) {
        if (netif->hostname)
            wm_os_internal_free((void *)netif->hostname);

        netif->hostname = strdup((char *)ctx);
    }
}

static void get_lwip_hostname_callback(struct netif *netif, void *ctx)
{
    if (netif && ctx) {
        const char *hostname = netif_get_hostname(netif);
        if (hostname)
            strcpy((char *)ctx, hostname);
        else
            ((char *)ctx)[0] = '\0';
    }
}

int wm_netif_set_hostname_lwip(wm_netif_t *netif, const char *hostname)
{
    err_t err;

    err = netifapi_netif_msg_call(netif->netif, set_lwip_hostname_callback, (void *)hostname);

    return ERR_OK == err ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

int wm_netif_get_hostname_lwip(wm_netif_t *netif, const char *hostname)
{
    err_t err;

    err = netifapi_netif_msg_call(netif->netif, get_lwip_hostname_callback, (void *)hostname);

    return ERR_OK == err ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

void wm_netif_set_ipaddr_lwip(wm_netif_t *netif, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw)
{
#if LWIP_IPV4
    ip_addr_t ipaddr;
    ip_addr_t mask;
    ip_addr_t gatew;

    if (netif->netif) {
        wm_ipaddr_copy_to_lwip_ipaddr(ip, &ipaddr);
        wm_ipaddr_copy_to_lwip_ipaddr(netmask, &mask);
        wm_ipaddr_copy_to_lwip_ipaddr(gw, &gatew);

        netifapi_netif_set_addr(netif->netif, (const ip4_addr_t *)&ipaddr, (const ip4_addr_t *)&mask,
                                (const ip4_addr_t *)&gatew);
    }
#endif
}

void wm_netif_get_ipaddr_lwip(wm_netif_t *netif, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw)
{
#if LWIP_IPV4
    struct netif *nif = netif->netif;

    if (ip)
        lwip_ipaddr_copy_to_wm_ipaddr(&nif->ip_addr, ip);
    if (netmask)
        lwip_ipaddr_copy_to_wm_ipaddr(&nif->netmask, netmask);
    if (gw)
        lwip_ipaddr_copy_to_wm_ipaddr(&nif->gw, gw);
#endif
}

#if LWIP_IPV6
static void netif_ip6_enable_callback(struct netif *netif, void *ctx)
{
    uint8_t enable;

    if (netif && ctx) {
        enable = *(uint8_t *)ctx;

        if (enable) {
            netif_create_ip6_linklocal_address(netif, 1);
        } else {
            for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++)
                netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
        }
    }
}
#endif

int16_t wm_netif_ip6_enable_lwip(wm_netif_t *netif, uint8_t enable)
{
    err_t err             = ERR_ARG;
    struct netif *p_netif = (struct netif *)netif->netif;

    if (p_netif != NULL && netif_is_up(p_netif)) {
#if LWIP_IPV6
        err = netifapi_netif_msg_call(netif->netif, netif_ip6_enable_callback, (void *)&enable);
#endif
    } else {
        return WM_ERR_FAILED;
    }

    return ERR_OK == err ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

int wm_netif_get_ll_ip6_lwip(wm_netif_t *netif, wm_ip_addr_t *ip6)
{
#if LWIP_IPV6
    if (netif == NULL || ip6 == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    struct netif *p_netif = (struct netif *)netif->netif;
    if (p_netif != NULL && netif_is_up(p_netif)) {
        if (ip6_addr_ispreferred(netif_ip6_addr_state(p_netif, 0))) {
            lwip_ipaddr_copy_to_wm_ipaddr(&p_netif->ip6_addr[0], ip6);
        } else {
            return WM_ERR_FAILED;
        }
    } else {
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

static void set_netif_mtu_callback(struct netif *netif, void *ctx)
{
    if (netif && ctx) {
        netif->mtu = *(uint16_t *)ctx;
    }
}

int16_t wm_netif_set_mtu_lwip(wm_netif_t *netif, uint16_t mtu)
{
    err_t err;

    err = netifapi_netif_msg_call(netif->netif, set_netif_mtu_callback, (void *)&mtu);

    return ERR_OK == err ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

uint16_t wm_netif_get_mtu_lwip(wm_netif_t *netif)
{
    if (netif->netif)
        return ((struct netif *)(netif->netif))->mtu;

    return 0;
}

void wm_netif_set_mac_addr_lwip(wm_netif_t *netif, uint8_t mac_addr[6])
{
    if (netif->netif)
        memcpy(((struct netif *)(netif->netif))->hwaddr, mac_addr, 6);
}

void wm_netif_cleanup_tcb_lwip(void *tcb)
{
#if LWIP_NETCONN_SEM_PER_THREAD
    sys_lwip_netconn_thread_sem_del(tcb);
#endif
}

void wm_netif_init_lwip(void *sync_sem)
{
    tcpip_init(lwip_tcpip_init_done_callback, sync_sem);

    netif_add_ext_callback(&lwip_netif_ext_status, lwip_netif_ext_callback);
}

#ifdef CONFIG_LWIP_ENABLE_DEINIT
void wm_netif_deinit_lwip(void *sync_sem)
{
    netif_remove_ext_callback(&lwip_netif_ext_status);

    tcpip_deinit(lwip_tcpip_init_done_callback, sync_sem);
}
#endif

int wm_netif_addif_lwip(wm_netif_t *netif, const uint8_t *mac_addr, const uint8_t if_name[2], const char *hostname)
{
    err_t err;
    struct netif *nif;

    nif = wm_os_internal_malloc(sizeof(struct netif));
    if (!nif)
        return WM_ERR_NO_MEM;

    netif->netif = nif;
    memset(nif, 0, sizeof(struct netif));

    nif->hwaddr_len = NETIF_MAX_HWADDR_LEN;
    memcpy(&nif->hwaddr[0], mac_addr, NETIF_MAX_HWADDR_LEN);
    memcpy(&nif->name[0], if_name, 2);
#if LWIP_NETIF_HOSTNAME
    if (hostname) {
        nif->hostname = strdup(hostname);
        if (!nif->hostname) {
            wm_os_internal_free(nif);
            return ERR_MEM;
        }
    }
#endif /* LWIP_NETIF_HOSTNAME */

    err = netifapi_netif_add(nif,
#if LWIP_IPV4
                             IPADDR_ANY, IPADDR_ANY, IPADDR_ANY,
#endif
                             NULL, lwip_netif_init, tcpip_input);

    if (ERR_OK == err) {
        netifapi_netif_set_up(nif);

        wm_netif_set_input_lwip(netif, true);
    } else {
#if LWIP_NETIF_HOSTNAME
        if (nif->hostname)
            wm_os_internal_free((void *)nif->hostname);
#endif
        wm_os_internal_free(nif);
        netif->netif = NULL;
    }

    return ERR_OK == err ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

int wm_netif_delif_lwip(wm_netif_t *netif)
{
    err_t err = ERR_OK;

    if (netif->netif) {
        wm_netif_set_input_lwip(netif, false);

        err = netifapi_netif_set_down(netif->netif);
        if (ERR_OK == err) {
            err = netifapi_netif_remove(netif->netif);
            if (((struct netif *)(netif->netif))->hostname)
                wm_os_internal_free((void *)(((struct netif *)(netif->netif))->hostname));
            wm_os_internal_free(netif->netif);
            netif->netif = NULL;

            LWIP_NETCONN_THREAD_SEM_FREE();
        }
    }

    return ERR_OK == err ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

int wm_netif_get_netif_impl_index_lwip(wm_netif_t *netif)
{
    if (netif->netif)
        return ((struct netif *)(netif->netif))->num + 1;

    return 0;
}

void wm_netif_start_dhcpc_lwip(wm_netif_t *netif)
{
    if (netif->netif) {
#if LWIP_IPV4
        netifapi_dhcp_start((struct netif *)netif->netif);
#endif

#if LWIP_IPV6 && CONFIG_LWIP_IPV6_DHCP6
        netifapi_dhcp6_start((struct netif *)netif->netif);
#endif
    }
}

void wm_netif_stop_dhcpc_lwip(wm_netif_t *netif)
{
    if (netif->netif) {
#if LWIP_IPV6 && CONFIG_LWIP_IPV6_DHCP6
        netifapi_dhcp6_stop((struct netif *)netif->netif);
#endif

#if LWIP_IPV4
        netifapi_dhcp_stop((struct netif *)netif->netif);
#endif
    }
}

void wm_netif_start_dhcps_lwip(wm_netif_t *netif, wm_ip_addr_t *ipaddr, wm_ip_addr_t *netmask, wm_ip_addr_t *gw)
{
#if CONFIG_WIFI_ENABLE_AP_FEATURE
    ip_addr_t ip;
    ip_addr_t mask;
    ip_addr_t gatew;

    if (!netif || !ipaddr || !netmask || !gw)
        return;

    wm_ipaddr_copy_to_lwip_ipaddr(ipaddr, &ip);
    wm_ipaddr_copy_to_lwip_ipaddr(netmask, &mask);
    wm_ipaddr_copy_to_lwip_ipaddr(gw, &gatew);

    netifapi_netif_set_up(netif->netif);
    netifapi_netif_set_addr(netif->netif, (const ip4_addr_t *)&ip, (const ip4_addr_t *)&mask, (const ip4_addr_t *)&gatew);
    DHCPS_Start(netif->netif);
#endif
}

void wm_netif_stop_dhcps_lwip(wm_netif_t *netif)
{
#if CONFIG_WIFI_ENABLE_AP_FEATURE
    if (netif->netif) {
        DHCPS_Stop();
        netifapi_netif_set_down(netif->netif);
        netifapi_netif_set_addr(netif->netif, NULL, NULL, NULL);
    }
#endif
}

void wm_netif_start_dnss_lwip(wm_netif_t *netif)
{
#if CONFIG_WIFI_ENABLE_AP_FEATURE && CONFIG_LWIP_ENABLE_DNS_SERVER
    if (netif->netif) {
        DNSS_Start(netif->netif, CONFIG_LWIP_DNS_SERVER_NAME);
    }
#endif
}

void wm_netif_stop_dnss_lwip(wm_netif_t *netif)
{
#if CONFIG_WIFI_ENABLE_AP_FEATURE && CONFIG_LWIP_ENABLE_DNS_SERVER
    if (netif->netif) {
        DNSS_Stop();
    }
#endif
}
