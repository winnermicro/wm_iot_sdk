#include <assert.h>
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/err.h"
#include "lwip/netifapi.h"
#include "lwip/apps/dhcp_server.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/apps/dns_server.h"
#include "wm_osal.h"
#include "wm_component.h"
#include "wm_netif.h"

#include "../wm_nm_netstack.h"

#define LOG_TAG "wm_nm_netstack"
#include "wm_log.h"

#if LWIP_IPV4 && LWIP_IPV6
static void wm_2_lwip_ip(const wm_ip_addr_t *src, ip_addr_t *dst)
{
    if (src->type == WM_IPADDR_TYPE_V4) {
        dst->type            = IPADDR_TYPE_V4;
        dst->u_addr.ip4.addr = src->u_addr.ip4.addr;
    } else {
        if (src->type == WM_IPADDR_TYPE_V6) {
            dst->type = IPADDR_TYPE_V6;
        } else {
            dst->type = IPADDR_TYPE_ANY;
        }
        dst->u_addr.ip6.addr[0] = src->u_addr.ip6.addr[0];
        dst->u_addr.ip6.addr[1] = src->u_addr.ip6.addr[1];
        dst->u_addr.ip6.addr[2] = src->u_addr.ip6.addr[2];
        dst->u_addr.ip6.addr[3] = src->u_addr.ip6.addr[3];
#if LWIP_IPV6_SCOPES
        dst->u_addr.ip6.zone = src->u_addr.ip6.zone;
#endif
    }
}
static void lwip_2_wm_ip(const ip_addr_t *src, wm_ip_addr_t *dst)
{
    if (src->type == WM_IPADDR_TYPE_V4) {
        dst->type            = IPADDR_TYPE_V4;
        dst->u_addr.ip4.addr = src->u_addr.ip4.addr;
    } else {
        if (src->type == WM_IPADDR_TYPE_V6) {
            dst->type = IPADDR_TYPE_V6;
        } else {
            dst->type = IPADDR_TYPE_ANY;
        }
        dst->u_addr.ip6.addr[0] = src->u_addr.ip6.addr[0];
        dst->u_addr.ip6.addr[1] = src->u_addr.ip6.addr[1];
        dst->u_addr.ip6.addr[2] = src->u_addr.ip6.addr[2];
        dst->u_addr.ip6.addr[3] = src->u_addr.ip6.addr[3];
#if LWIP_IPV6_SCOPES
        dst->u_addr.ip6.zone = src->u_addr.ip6.zone;
#endif
    }
}
static void wm_2_lwip_ip4(const wm_ip4_addr_t *src, ip4_addr_t *dst)
{
    dst->addr = src->addr;
}
static void lwip_2_wm_ip4(const ip4_addr_t *src, wm_ip4_addr_t *dst)
{
    dst->addr = src->addr;
}
__attribute__((unused)) static void wm_2_lwip_ip6(const wm_ip6_addr_t *src, ip6_addr_t *dst)
{
    dst->addr[0] = src->addr[0];
    dst->addr[1] = src->addr[1];
    dst->addr[2] = src->addr[2];
    dst->addr[3] = src->addr[3];
#if LWIP_IPV6_SCOPES
    dst->zone = src->zone;
#endif
}
static void lwip_2_wm_ip6(const ip6_addr_t *src, wm_ip6_addr_t *dst)
{
    dst->addr[0] = src->addr[0];
    dst->addr[1] = src->addr[1];
    dst->addr[2] = src->addr[2];
    dst->addr[3] = src->addr[3];
    dst->zone    = 0;
}
#elif LWIP_IPV4
static void wm_2_lwip_ip4(const wm_ip4_addr_t *src, ip4_addr_t *dst)
{
    dst->addr = src->addr;
}
static void lwip_2_wm_ip4(const ip4_addr_t *src, wm_ip4_addr_t *dst)
{
    dst->addr = src->addr;
}
static void wm_2_lwip_ip(const wm_ip_addr_t *src, ip_addr_t *dst)
{
    dst->addr = src->u_addr.ip4.addr;
}
static void lwip_2_wm_ip(const ip_addr_t *src, wm_ip_addr_t *dst)
{
    dst->u_addr.ip4.addr = src->addr;
}
#elif LWIP_IPV6

__attribute__((unused)) static void wm_2_lwip_ip6(const wm_ip6_addr_t *src, ip6_addr_t *dst)
{
    dst->addr[0] = src->addr[0];
    dst->addr[1] = src->addr[1];
    dst->addr[2] = src->addr[2];
    dst->addr[3] = src->addr[3];
#if LWIP_IPV6_SCOPES
    dst->zone = src->zone;
#endif
}

static void lwip_2_wm_ip6(const ip6_addr_t *src, wm_ip6_addr_t *dst)
{
    dst->addr[0] = src->addr[0];
    dst->addr[1] = src->addr[1];
    dst->addr[2] = src->addr[2];
    dst->addr[3] = src->addr[3];
    dst->zone    = 0;
}

#endif

int wm_net_stack_init(void)
{
    wm_netif_init(NULL);
    return 0;
}

int wm_net_stack_get_netif_name(void *netif, char *name_buff, size_t max_len)
{
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    assert(max_len >= sizeof(lwip_netif->name));
    if (lwip_netif->name[0] == 'w' && lwip_netif->name[1] == '0') {
        snprintf(name_buff, max_len, "%s", WIFI_STATION_NETIF_NAME);
    } else if (lwip_netif->name[0] == 'w' && lwip_netif->name[1] == '1') {
        snprintf(name_buff, max_len, "%s", WIFI_SOFTAP_NETIF_NAME);
    } else {
        snprintf(name_buff, max_len, "%c%c%d", lwip_netif->name[0], lwip_netif->name[1], lwip_netif->num);
    }
    return WM_ERR_SUCCESS;
}

uint32_t wm_net_stack_get_flag(void *netif)
{
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    return lwip_netif->flags;
}

uint32_t wm_net_stack_get_mtu(void *netif)
{
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    return lwip_netif->mtu;
}

int wm_net_stack_set_mac(void *netif, uint8_t *mac, size_t len)
{
    if (NETIF_MAX_HWADDR_LEN != len) {
        return WM_ERR_INVALID_PARAM;
    }
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    memcpy(lwip_netif->hwaddr, mac, NETIF_MAX_HWADDR_LEN);
    return WM_ERR_SUCCESS;
}

int wm_net_stack_get_mac(void *netif, uint8_t *mac, size_t *max_len)
{
    if (NETIF_MAX_HWADDR_LEN > *max_len) {
        return WM_ERR_INVALID_PARAM;
    }
    *max_len                 = NETIF_MAX_HWADDR_LEN;
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    memcpy(mac, lwip_netif->hwaddr, NETIF_MAX_HWADDR_LEN);
    return WM_ERR_SUCCESS;
}

int wm_net_stack_set_ip(void *netif, wm_ip4_addr_t *ip, wm_ip4_addr_t *mask, wm_ip4_addr_t *gw)
{
#if LWIP_IPV4
    int ret                  = WM_ERR_SUCCESS;
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    ip4_addr_t lwip_addr;
    ip4_addr_t lwip_mask;
    ip4_addr_t lwip_gw;
    if (ip) {
        wm_2_lwip_ip4(ip, &lwip_addr);
    }
    if (mask) {
        wm_2_lwip_ip4(mask, &lwip_mask);
    }
    if (gw) {
        wm_2_lwip_ip4(gw, &lwip_gw);
    }
    ret = netifapi_netif_set_addr(lwip_netif, ip ? &lwip_addr : NULL, mask ? &lwip_mask : NULL, gw ? &lwip_gw : NULL);
    if (ret != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_get_ip(void *netif, wm_ip4_addr_t *ip, wm_ip4_addr_t *mask, wm_ip4_addr_t *gw)
{
#if LWIP_IPV4
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    ip4_addr_t lwip_addr;
    ip4_addr_t lwip_mask;
    ip4_addr_t lwip_gw;

    if (!netif_is_up(lwip_netif)) {
        return WM_ERR_NO_INITED;
    }
    ip4_addr_set(&lwip_addr, ip_2_ip4(&lwip_netif->ip_addr));
    ip4_addr_set(&lwip_mask, ip_2_ip4(&lwip_netif->netmask));
    ip4_addr_set(&lwip_gw, ip_2_ip4(&lwip_netif->gw));
    if (ip) {
        lwip_2_wm_ip4(&lwip_addr, ip);
    }
    if (mask) {
        lwip_2_wm_ip4(&lwip_mask, mask);
    }
    if (gw) {
        lwip_2_wm_ip4(&lwip_gw, gw);
    }
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_get_ip6_linklocal(void *netif, wm_ip6_addr_t *ip6)
{
#if LWIP_IPV6
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    if (netif_is_up(lwip_netif) && ip6_addr_ispreferred(netif_ip6_addr_state(lwip_netif, 0))) {
        lwip_2_wm_ip6(ip_2_ip6(&lwip_netif->ip6_addr[0]), ip6);
    } else {
        return WM_ERR_NO_INITED;
    }
    return WM_ERR_SUCCESS;
#else
    (void)netif;
    (void)ip6;
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_get_ip6_global(void *netif, wm_ip6_addr_t *ip6)
{
#if LWIP_IPV6
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    if (netif_is_up(lwip_netif)) {
        for (int i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (ip6_addr_ispreferred(netif_ip6_addr_state(lwip_netif, i)) && ip6_addr_isglobal(netif_ip6_addr(lwip_netif, i))) {
                lwip_2_wm_ip6(ip_2_ip6(&lwip_netif->ip6_addr[i]), ip6);
                return WM_ERR_SUCCESS;
            }
        }
    }
    return WM_ERR_FAILED;
#else
    (void)netif;
    (void)ip6;
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_set_dhcps_option(void *netif, wm_nm_dhcps_option_t *dhcps_option)
{
#if LWIP_IPV4
    int ret                  = WM_ERR_SUCCESS;
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL && dhcps_option != NULL);

    ret = wm_net_stack_set_ip(netif, &dhcps_option->server_ip.u_addr.ip4, &dhcps_option->ip_mask.u_addr.ip4,
                              &dhcps_option->gateway.u_addr.ip4);
    if (ret != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    /** clean DHCPS option */
    DHCPS_Clean_Option();

    wm_dhcps_options_t dhcps_opt = { 0 };
    wm_2_lwip_ip(&dhcps_option->start_ip, &dhcps_opt.start_ip);
    wm_2_lwip_ip(&dhcps_option->gateway, &dhcps_opt.gate_way);
    wm_2_lwip_ip(&dhcps_option->dns1, &dhcps_opt.dns1);
    wm_2_lwip_ip(&dhcps_option->dns2, &dhcps_opt.dns2);
    dhcps_opt.lease_time = dhcps_option->lease_time;

    /** set DHCPS option */
    ret = DHCPS_Set_Option(&dhcps_opt);
    if (ret != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    if (DHCPS_Is_Start(lwip_netif)) {
        (void)wm_net_stack_dhcps_stop(netif);
        (void)wm_net_stack_dhcps_start(netif);
    }
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_get_dhcps_option(void *netif, wm_nm_dhcps_option_t *dhcps_option)
{
#if LWIP_IPV4
    wm_dhcps_options_t dhcps_opt = { 0 };
    int ret                      = WM_ERR_SUCCESS;

    assert(dhcps_option != NULL);

    dhcps_option->server_ip.type = WM_IPADDR_TYPE_V4;
    dhcps_option->start_ip.type  = WM_IPADDR_TYPE_V4;
    dhcps_option->ip_mask.type   = WM_IPADDR_TYPE_V4;
    dhcps_option->gateway.type   = WM_IPADDR_TYPE_V4;
    dhcps_option->dns1.type      = WM_IPADDR_TYPE_V4;
    dhcps_option->dns2.type      = WM_IPADDR_TYPE_V4;
    ret = wm_net_stack_get_ip(netif, &dhcps_option->server_ip.u_addr.ip4, &dhcps_option->ip_mask.u_addr.ip4,
                              &dhcps_option->gateway.u_addr.ip4);
    if (ret != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    /** get DHCPS option */
    ret = DHCPS_Get_Option(&dhcps_opt);
    if (ret != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    lwip_2_wm_ip(&dhcps_opt.start_ip, &dhcps_option->start_ip);
    lwip_2_wm_ip(&dhcps_opt.gate_way, &dhcps_option->gateway);
    lwip_2_wm_ip(&dhcps_opt.dns1, &dhcps_option->dns1);
    lwip_2_wm_ip(&dhcps_opt.dns2, &dhcps_option->dns2);
    dhcps_option->lease_time = dhcps_opt.lease_time;

    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

bool wm_net_stack_dhcps_is_start(void *netif)
{
#if LWIP_IPV4
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    return DHCPS_Is_Start(lwip_netif);
#else
    return false;
#endif
}

int wm_net_stack_dhcps_start(void *netif)
{
#if LWIP_IPV4
    int ret                  = WM_ERR_SUCCESS;
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    ret = DHCPS_Start(lwip_netif);
    if (ret != DHCPS_ERR_SUCCESS) {
        return ret == DHCPS_ERR_LINKDOWN ? WM_ERR_NO_INITED : WM_ERR_NO_MEM;
    }
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_dhcps_stop(void *netif)
{
#if LWIP_IPV4
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    if (DHCPS_Is_Start(lwip_netif)) {
        DHCPS_Stop();
    }
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_dhcpc_start(void *netif)
{
    int ret                  = WM_ERR_SUCCESS;
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
#if LWIP_IPV4
    ret = netifapi_dhcp_start(lwip_netif);
    if (ret) {
        return WM_ERR_FAILED;
    }
#endif
#if LWIP_IPV6 && CONFIG_LWIP_IPV6_DHCP6
    ret = netifapi_dhcp6_start(lwip_netif);
    if (ret) {
        return WM_ERR_FAILED;
    }
#endif
    return WM_ERR_SUCCESS;
}

int wm_net_stack_dhcpc_stop(void *netif)
{
    int ret                  = WM_ERR_SUCCESS;
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
#if LWIP_IPV6 && CONFIG_LWIP_IPV6_DHCP6
    ret = netifapi_dhcp6_stop(lwip_netif);
    if (ret) {
        return WM_ERR_FAILED;
    }
#endif
#if LWIP_IPV4
    ret = netifapi_dhcp_stop(lwip_netif);
    if (ret) {
        return WM_ERR_FAILED;
    }
#endif
    return WM_ERR_SUCCESS;
}

int wm_net_stack_set_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns)
{
#if LWIP_IPV4
    assert(dns != NULL);
    ip_addr_t lwip_dns;
    wm_2_lwip_ip(dns, &lwip_dns);
    dns_set_lock(false);
    dns_setserver(type, &lwip_dns);
    const ip_addr_t *lwip_dns1 = dns_getserver(WM_NM_DNS_MAIN);
    const ip_addr_t *lwip_dns2 = dns_getserver(WM_NM_DNS_BACKUP);
    if (!ip_addr_isany(lwip_dns1) || !ip_addr_isany(lwip_dns2)) {
        dns_set_lock(true); //User sets DNS, ignoring DHCP
    }
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_get_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns)
{
#if LWIP_IPV4
    const ip_addr_t *lwip_dns = dns_getserver(type);
    if (lwip_dns == NULL) {
        return WM_ERR_FAILED;
    }
    lwip_2_wm_ip(lwip_dns, dns);
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
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
            for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
            }
        }
    }
}
#endif

int wm_net_stack_set_ip6_state(void *netif, bool enable)
{
#if LWIP_IPV6
    int ret                  = WM_ERR_SUCCESS;
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    if (netif_is_up(lwip_netif)) {
        ret = netifapi_netif_msg_call(lwip_netif, netif_ip6_enable_callback, (void *)&enable);
    }
    return ERR_OK == ret ? WM_ERR_SUCCESS : WM_ERR_FAILED;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

bool wm_net_stack_get_ip6_state(void *netif)
{
#if CONFIG_LWIP_ENABLE_IPV6
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (!ip6_addr_isinvalid(netif_ip6_addr_state(lwip_netif, i))) {
            return true;
        }
    }
#endif
    return false;
}

int wm_net_stack_dnss_start(void *netif)
{
#if CONFIG_LWIP_ENABLE_DNS_SERVER
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    if (DNSS_Start(lwip_netif, CONFIG_LWIP_DNS_SERVER_NAME) == ERR_OK) {
        return WM_ERR_SUCCESS;
    }
    return WM_ERR_FAILED;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_net_stack_dnss_stop(void *netif)
{
#if CONFIG_LWIP_ENABLE_DNS_SERVER
    struct netif *lwip_netif = (struct netif *)netif;
    assert(lwip_netif != NULL);
    DNSS_Stop();
    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif
}
