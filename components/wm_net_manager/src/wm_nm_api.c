#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "wm_utils.h"
#include "wm_osal.h"
#include "wm_component.h"
#include "wm_nm_api.h"
#include "wm_nm_netstack.h"
#include "wm_nm_core.h"
#include "wm_nm_wifi.h"
#include "lwip/ip_addr.h"

#if CONFIG_COMPONENT_WIFI_ENABLED
#include "wm_wifi.h"
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */

#define LOG_TAG "wm_nm_api"
#include "wm_log.h"

#define WM_NM_WIFI_SSID_MAX_LENGTH 32
#define WM_NM_WIFI_PWD_MIN_LENGTH  8
#define WM_NM_WIFI_PWD_MAX_LENGTH  64

#if WM_NM_IPV4
static int wm_nm_subnet_bits(uint32_t mask)
{
    int set_bits_count = 0;

    mask = htonl(mask);

    for (int i = 0; i < 32; i++) {
        if (mask & (1U << i)) {
            ++set_bits_count;
        } else if (set_bits_count) {
            return 0;
        }
    }

    return set_bits_count;
}

static int wm_nm_valid_subnet_mask(uint32_t mask)
{
    int set_bits_count = wm_nm_subnet_bits(mask);
    return set_bits_count > 0 && set_bits_count <= 32;
}

static bool wm_nm_validate_ip4(uint32_t ip, uint32_t gateway, uint32_t mask)
{
    if (ip == 0) {
        if (gateway == 0 && mask == 0) {
            return true;
        } else {
            return false;
        }
    }
    if (!wm_nm_valid_subnet_mask(mask)) {
        return false;
    }

    if (((ip & mask) | ~(mask)) == ip) {
        return false;
    }

    if (((gateway & mask) | ~(mask)) == gateway) {
        return false;
    }

    if (!(ip & ~(mask)) || !(gateway & ~(mask))) {
        return false;
    }

    return ((ip & mask) == (gateway & mask));
}
#endif /* WM_NM_IPV4 */

wm_nm_netif_t wm_nm_get_netif(void)
{
    for (wm_nm_netif_t i = WM_NETIF_TYPE_WIFI_STA; i < WM_NETIF_TYPE_MAX; i++) {
        if (wm_netif_get_netif(i) != NULL) {
            return i;
        }
    }

    return WM_NM_INVALID_NETIF;
}

int wm_nm_get_netif_number(void)
{
    int num = 0;

    for (wm_nm_netif_t i = WM_NETIF_TYPE_WIFI_STA; i < WM_NETIF_TYPE_MAX; i++) {
        if (wm_netif_get_netif(i) != NULL) {
            ++num;
        }
    }

    return num;
}

wm_nm_netif_t wm_nm_get_next_netif(wm_nm_netif_t netif)
{
    if (netif < WM_NETIF_TYPE_WIFI_STA || netif >= WM_NETIF_TYPE_MAX) {
        return WM_NM_INVALID_NETIF;
    }

    for (wm_nm_netif_t i = netif + 1; i < WM_NETIF_TYPE_MAX; i++) {
        if (wm_netif_get_netif(i) != NULL) {
            return i;
        }
    }

    return WM_NM_INVALID_NETIF;
}

const char *wm_nm_get_netif_name(wm_nm_netif_t netif)
{
    if (wm_netif_get_netif(netif) == NULL) {
        wm_log_error("invalid nm_netif\n");
        return NULL;
    }

    switch (netif) {
        case WM_NETIF_TYPE_WIFI_STA:
        {
            return WIFI_STATION_NETIF_NAME;
        }
        case WM_NETIF_TYPE_WIFI_AP:
        {
            return WIFI_SOFTAP_NETIF_NAME;
        }
        default:
        {
            return NULL;
        }
    }
}

wm_nm_netif_t wm_nm_get_netif_by_name(const char *name)
{
    if (name == NULL) {
        wm_log_error("invalid name\n");
        return WM_NM_INVALID_NETIF;
    }

    if (strcmp(name, WIFI_STATION_NETIF_NAME) == 0) {
        if (NULL != wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA)) {
            return WM_NETIF_TYPE_WIFI_STA;
        }
    } else if (strcmp(name, WIFI_SOFTAP_NETIF_NAME) == 0) {
        if (NULL != wm_netif_get_netif(WM_NETIF_TYPE_WIFI_AP)) {
            return WM_NETIF_TYPE_WIFI_AP;
        }
    }

    return WM_NM_INVALID_NETIF;
}

int wm_nm_get_netif_flag(wm_nm_netif_t netif, uint32_t *flag)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);

    if (int_netif == NULL || flag == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    *flag = wm_net_stack_get_flag(int_netif->netif);
    return WM_ERR_SUCCESS;
}

int wm_nm_get_netif_mtu(wm_nm_netif_t netif, uint32_t *mtu)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);

    if (int_netif == NULL || mtu == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    *mtu = wm_net_stack_get_mtu(int_netif->netif);
    return WM_ERR_SUCCESS;
}

int wm_nm_set_netif_mac(wm_nm_netif_t netif, uint8_t *mac, size_t len)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);

    if (int_netif == NULL || mac == NULL || len != NETIF_MAX_HWADDR_LEN) {
        return WM_ERR_INVALID_PARAM;
    }

#if CONFIG_COMPONENT_WIFI_ENABLED
    int ret = WM_ERR_SUCCESS;
    uint8_t mac_addr[NETIF_MAX_HWADDR_LEN];

    memcpy(mac_addr, mac, sizeof(mac_addr));

    switch (netif) {
        case WM_NETIF_TYPE_WIFI_STA:
            ret = wm_wifi_set_mac(WM_WIFI_IF_STA, mac_addr);
            break;
        case WM_NETIF_TYPE_WIFI_AP:
            ret = wm_wifi_set_mac(WM_WIFI_IF_AP, mac_addr);
            break;
        default:
            break;
    }

    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */

    return wm_net_stack_set_mac(int_netif->netif, mac, len);
}

int wm_nm_get_netif_mac(wm_nm_netif_t netif, uint8_t *mac, size_t *max_len)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);

    if (int_netif == NULL || mac == NULL || max_len == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    return wm_net_stack_get_mac(int_netif->netif, mac, max_len);
}

int wm_nm_set_netif_ip_info(wm_nm_netif_t netif, wm_netif_ip_info_t *ip_info)
{
#if WM_NM_IPV4
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL || ip_info == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_nm_param_t *nm = wm_nm_get_core_param();

    if (ip_info->ip.type != WM_IPADDR_TYPE_V4 || ip_info->gw.type != WM_IPADDR_TYPE_V4 ||
        ip_info->netmask.type != WM_IPADDR_TYPE_V4) {
        wm_log_error("not supported set IPv6 address");
        return WM_ERR_NO_SUPPORT;
    }
    if (!wm_nm_validate_ip4(ip_info->ip.u_addr.ip4.addr, ip_info->gw.u_addr.ip4.addr, ip_info->netmask.u_addr.ip4.addr)) {
        wm_log_error("ip netmask gateway verification failed");
        return WM_ERR_INVALID_PARAM;
    }

    if (netif == WM_NETIF_TYPE_WIFI_STA) {
        if (ip_info->ip.u_addr.ip4.addr == 0 && ip_info->gw.u_addr.ip4.addr == 0 && ip_info->netmask.u_addr.ip4.addr == 0) {
            // remove the static ip
            nm->info[netif].is_static_ip = false;
        } else {
            if (WM_ERR_SUCCESS != wm_net_stack_dhcpc_stop(int_netif->netif)) {
                wm_log_error("can't stop dhcp client");
                return WM_ERR_FAILED;
            }
            // set static ip
            nm->info[netif].is_static_ip = true;
        }
    } else if (netif == WM_NETIF_TYPE_WIFI_AP) {
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
        /** If the softAP IP address is modified, the softAP will be restarted.
         * Automatically calculate DHCPS options.
        */
        nm->dhcps_option.server_ip.u_addr.ip4.addr = ip_info->ip.u_addr.ip4.addr;
        nm->dhcps_option.ip_mask.u_addr.ip4.addr   = ip_info->netmask.u_addr.ip4.addr;
        nm->dhcps_option.gateway.u_addr.ip4.addr   = ip_info->gw.u_addr.ip4.addr;
        nm->dhcps_option.dns1.u_addr.ip4.addr      = ip_info->gw.u_addr.ip4.addr;
        nm->dhcps_option.dns2.u_addr.ip4.addr      = 0;
        nm->dhcps_option.lease_time                = CONFIG_NM_DHCPS_LEASE_TIME;

        uint32_t server_ip = ntohl(ip_addr_get_ip4_u32((ip_addr_t *)&nm->dhcps_option.server_ip));
        uint32_t ip_mask   = ntohl(ip_addr_get_ip4_u32((ip_addr_t *)&nm->dhcps_option.ip_mask));
        uint32_t gw_ip     = ntohl(ip_addr_get_ip4_u32((ip_addr_t *)&nm->dhcps_option.gateway));
        uint32_t start_ip  = 1;

        while (1) {
            if (!(start_ip & (~ip_mask))) {
                // No IP address pool available
                wm_log_error("The DHCPS address pool for softAP does not exist");
                return WM_ERR_FAILED;
            }

            if ((start_ip & (~ip_mask)) == (server_ip & (~ip_mask))) {
                ++start_ip;
                continue;
            }

            if ((start_ip & (~ip_mask)) == (gw_ip & (~ip_mask))) {
                ++start_ip;
                continue;
            }
            break;
        }
        nm->dhcps_option.start_ip.u_addr.ip4.addr = ntohl(start_ip | (server_ip & ip_mask));

        /** restart softAP */
        wm_nm_msg_t msg = { .type = WM_NM_AP_START };
        return wm_nm_core_send_msg(&msg);
#else
        return WM_ERR_NO_SUPPORT;
#endif
    }

    int ret =
        wm_net_stack_set_ip(int_netif->netif, &ip_info->ip.u_addr.ip4, &ip_info->netmask.u_addr.ip4, &ip_info->gw.u_addr.ip4);
    if (ret != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    wm_log_info("set ip:%s succes", ipaddr_ntoa((ip_addr_t *)&ip_info->ip));
    wm_log_info("set net mask:%s succes", ipaddr_ntoa((ip_addr_t *)&ip_info->netmask));
    wm_log_info("set gw:%s succes", ipaddr_ntoa((ip_addr_t *)&ip_info->gw));

    memcpy(&(nm->info[netif].ip_info), ip_info, sizeof(*ip_info));

    return WM_ERR_SUCCESS;
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV4 */
}

int wm_nm_get_netif_ip_info(wm_nm_netif_t netif, wm_netif_ip_info_t *ip_info)
{
#if WM_NM_IPV4
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL || ip_info == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    memset(&ip_info->ip, 0, sizeof(wm_ip_addr_t));
    memset(&ip_info->netmask, 0, sizeof(wm_ip_addr_t));
    memset(&ip_info->gw, 0, sizeof(wm_ip_addr_t));

    return wm_net_stack_get_ip(int_netif->netif, &ip_info->ip.u_addr.ip4, &ip_info->netmask.u_addr.ip4,
                               &ip_info->gw.u_addr.ip4);
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV4 */
}

int wm_nm_get_netif_ip6_linklocal(wm_nm_netif_t netif, wm_ip6_addr_t *ip6)
{
#if WM_NM_IPV6
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL || ip6 == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    return wm_net_stack_get_ip6_linklocal(int_netif->netif, ip6);
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV6 */
}

int wm_nm_enable_netif_ip6(wm_nm_netif_t netif)
{
#if WM_NM_IPV6
    int ret;
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }
    ret = wm_net_stack_set_ip6_state(int_netif->netif, true);

    if (ret == WM_ERR_SUCCESS) {
        wm_nm_param_t *nm = wm_nm_get_core_param();

        nm->info[netif].ip6_disable = false;
    }
    return ret;
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV6 */
}

int wm_nm_disable_netif_ip6(wm_nm_netif_t netif)
{
#if WM_NM_IPV6
    int ret;
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }
    ret = wm_net_stack_set_ip6_state(int_netif->netif, false);

    if (ret == WM_ERR_SUCCESS) {
        wm_nm_param_t *nm = wm_nm_get_core_param();

        nm->info[netif].ip6_disable = true;
    }
    return ret;
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV6 */
}

bool wm_nm_get_netif_ip6_state(wm_nm_netif_t netif)
{
#if WM_NM_IPV6
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return false;
    }

    return wm_net_stack_get_ip6_state(int_netif->netif);
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV6 */
}

int wm_nm_get_netif_ip6_global(wm_nm_netif_t netif, wm_ip6_addr_t *ip6)
{
#if WM_NM_IPV6
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL || ip6 == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    return wm_net_stack_get_ip6_global(int_netif->netif, ip6);
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV6 */
}

int wm_nm_set_netif_dhcps_option(wm_nm_netif_t netif, wm_nm_dhcps_option_t *dhcps_option)
{
#if WM_NM_IPV4
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL || dhcps_option == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dhcps_option->server_ip.type != WM_IPADDR_TYPE_V4 || dhcps_option->start_ip.type != WM_IPADDR_TYPE_V4 ||
        dhcps_option->ip_mask.type != WM_IPADDR_TYPE_V4 || dhcps_option->gateway.type != WM_IPADDR_TYPE_V4 ||
        dhcps_option->dns1.type != WM_IPADDR_TYPE_V4 || dhcps_option->dns2.type != WM_IPADDR_TYPE_V4) {
        wm_log_error("DHCP server not supported set IPv6 address");
        return WM_ERR_NO_SUPPORT;
    }

    if (!wm_nm_validate_ip4(dhcps_option->start_ip.u_addr.ip4.addr, dhcps_option->gateway.u_addr.ip4.addr,
                            dhcps_option->ip_mask.u_addr.ip4.addr)) {
        wm_log_error("start_ip netmask gateway verification failed");
        return WM_ERR_INVALID_PARAM;
    }

    if (!wm_nm_validate_ip4(dhcps_option->server_ip.u_addr.ip4.addr, dhcps_option->gateway.u_addr.ip4.addr,
                            dhcps_option->ip_mask.u_addr.ip4.addr)) {
        wm_log_error("server_ip netmask gateway verification failed");
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_nm_subnet_bits(dhcps_option->ip_mask.u_addr.ip4.addr) > 28) {
        wm_log_error("Host ID are less than 14, DHCP server IP address will be not enough");
        return WM_ERR_INVALID_PARAM;
    }

    if (dhcps_option->server_ip.u_addr.ip4.addr == dhcps_option->start_ip.u_addr.ip4.addr) {
        wm_log_error("server_ip and start_ip is the same address");
        return WM_ERR_INVALID_PARAM;
    }

    if (dhcps_option->lease_time < 60) {
        wm_log_error("lease_time must exceed 59 seconds");
        return WM_ERR_INVALID_PARAM;
    }
    if (dhcps_option->server_ip.u_addr.ip4.addr != dhcps_option->gateway.u_addr.ip4.addr) {
        wm_log_warn("server_ip and gateway not the same address");
        return WM_ERR_INVALID_PARAM;
    }

    int ret = wm_net_stack_set_dhcps_option(int_netif->netif, dhcps_option);

    if (ret == WM_ERR_SUCCESS) {
        wm_nm_param_t *nm = wm_nm_get_core_param();

        memcpy(&(nm->dhcps_option), dhcps_option, sizeof(nm->dhcps_option));
    }
    return ret;
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV4 */
}

int wm_nm_get_netif_dhcps_option(wm_nm_netif_t netif, wm_nm_dhcps_option_t *dhcps_option)
{
#if WM_NM_IPV4
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL || dhcps_option == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    return wm_net_stack_get_dhcps_option(int_netif->netif, dhcps_option);
#else
    return WM_ERR_NO_SUPPORT;
#endif /* WM_NM_IPV4 */
}

int wm_nm_start_netif_dhcps(wm_nm_netif_t netif)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    int ret = wm_net_stack_dhcps_start(int_netif->netif);

    if (ret == WM_ERR_SUCCESS) {
        wm_nm_param_t *nm = wm_nm_get_core_param();

        nm->dhcps_disabled = true;
    }
    return ret;
}

int wm_nm_stop_netif_dhcps(wm_nm_netif_t netif)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    int ret = wm_net_stack_dhcps_stop(int_netif->netif);

    if (ret == WM_ERR_SUCCESS) {
        wm_nm_param_t *nm = wm_nm_get_core_param();

        nm->dhcps_disabled = false;
    }
    return ret;
}

int wm_nm_start_netif_dhcpc(wm_nm_netif_t netif)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    return wm_net_stack_dhcpc_start(int_netif->netif);
}

int wm_nm_stop_netif_dhcpc(wm_nm_netif_t netif)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    return wm_net_stack_dhcpc_stop(int_netif->netif);
}

int wm_nm_set_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns)
{
    if (dns == NULL || type > WM_NM_DNS_BACKUP) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dns->type != WM_IPADDR_TYPE_V4) {
        return WM_ERR_INVALID_PARAM;
    }

    return wm_net_stack_set_dns(type, dns);
}

int wm_nm_get_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns)
{
    if (dns == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    memset(dns, 0, sizeof(wm_ip_addr_t));
    return wm_net_stack_get_dns(type, dns);
}

int wm_nm_start_netif_dnss(wm_nm_netif_t netif)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    int ret = wm_net_stack_dnss_start(int_netif->netif);

    if (ret == WM_ERR_SUCCESS) {
        wm_nm_param_t *nm = wm_nm_get_core_param();

        nm->dnss_disabled = true;
    }
    return ret;
}

int wm_nm_stop_netif_dnss(wm_nm_netif_t netif)
{
    wm_nm_internal_netif_t *int_netif = wm_nm_query_internal_netif(netif);
    if (int_netif == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    int ret = wm_net_stack_dnss_stop(int_netif->netif);

    if (ret == WM_ERR_SUCCESS) {
        wm_nm_param_t *nm = wm_nm_get_core_param();

        nm->dnss_disabled = false;
    }
    return ret;
}

#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED || CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
static bool wm_nm_wifi_param_is_valid(const char *ssid, const char *password)
{
    /** return 0: the ssid or password is invalid; return 1: ssid and password is valid */
    if (ssid == NULL) {
        return false;
    }

    if (strnlen(ssid, WM_NM_WIFI_SSID_MAX_LENGTH + 1) > WM_NM_WIFI_SSID_MAX_LENGTH) {
        return false;
    }

    if (password != NULL) {
        int len = strnlen(password, WM_NM_WIFI_PWD_MAX_LENGTH + 1);

        if (len > WM_NM_WIFI_PWD_MAX_LENGTH || len < WM_NM_WIFI_PWD_MIN_LENGTH) {
            return false;
        }
    }

    return true;
}
#endif

int wm_nm_start_wifi_station(wm_nm_wifi_station_cfg_t *cfg)
{
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
    if (cfg == NULL || !wm_nm_wifi_param_is_valid(cfg->ssid, cfg->password)) {
        wm_log_error("wifi station config error");
        return WM_ERR_INVALID_PARAM;
    }

    wm_nm_msg_t msg = { .type = WM_NM_STA_START_CONNECT };

    memcpy(msg.sta.ssid, cfg->ssid, strlen(cfg->ssid));

    if (cfg->password != NULL) {
        memcpy(msg.sta.pwd, cfg->password, strlen(cfg->password));
    }

    return wm_nm_core_send_msg(&msg);
#else
    wm_log_error("NM wifi station is not enabled");
    return WM_ERR_NO_SUPPORT;
#endif
}

wm_nm_state_e wm_nm_get_wifi_station_state(void)
{
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
    return wm_nm_get_wifi_station_state_internal();
#else
    wm_log_error("NM wifi station is not enabled");
    return WM_NM_EXIT;
#endif
}

int wm_nm_stop_wifi_station(void)
{
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
    wm_nm_msg_t msg = { .type = WM_NM_STA_DISCONNECT };
    return wm_nm_core_send_msg(&msg);
#else
    wm_log_error("NM wifi station is not enabled");
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_nm_start_wifi_softap(wm_nm_wifi_softap_cfg_t *cfg)
{
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
    if (cfg == NULL || !wm_nm_wifi_param_is_valid(cfg->ssid, cfg->password)) {
        wm_log_error("wifi softap config error");
        return WM_ERR_INVALID_PARAM;
    }

    wm_nm_msg_t msg = { .type = WM_NM_AP_START };

    memcpy(msg.ap.ssid, cfg->ssid, strlen(cfg->ssid));

    if (cfg->password != NULL) {
        memcpy(msg.ap.pwd, cfg->password, strlen(cfg->password));
    }

    return wm_nm_core_send_msg(&msg);
#else
    wm_log_error("NM wifi softAP is not enabled");
    return WM_ERR_NO_SUPPORT;
#endif
}

int wm_nm_stop_wifi_softap(void)
{
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
    wm_nm_msg_t msg = { .type = WM_NM_AP_STOP };
    return wm_nm_core_send_msg(&msg);
#else
    wm_log_error("NM wifi softAP is not enabled");
    return WM_ERR_NO_SUPPORT;
#endif
}

wm_nm_state_e wm_nm_get_wifi_softap_state(void)
{
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
    return wm_nm_get_wifi_softap_state_internal();
#else
    wm_log_error("NM wifi softAP is not enabled");
    return WM_NM_EXIT;
#endif
}
