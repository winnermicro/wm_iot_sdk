#include "wmsdk_config.h"
#include <string.h>
#include "wm_error.h"
#include "wm_key_config.h"
#include "wm_netif.h"
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
#include "wm_netif_lwip.h"
#endif

#if CONFIG_COMPONENT_WIFI_ENABLED
#include "wm_wifi_types.h"
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */

#define LOG_TAG "wm_netif"
#include "wm_log.h"

#define WM_NETIF_NAME_B0  'w'
#define WM_NETIF_HOSTNAME "wm_dev"

static wm_netif_t *g_wm_netif[WM_NETIF_TYPE_MAX] = { 0 };

wm_netif_t *wm_netif_get_netif(wm_netif_type_t interface)
{
    if (interface < WM_NETIF_TYPE_WIFI_STA || interface >= WM_NETIF_TYPE_MAX) {
        return NULL;
    }

    return g_wm_netif[interface];
}

void wm_netif_start_dnss(wm_netif_type_t interface)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && WM_NETIF_TYPE_WIFI_AP == interface) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_start_dnss_lwip(netif);
#endif
    }
}

void wm_netif_stop_dnss(wm_netif_type_t interface)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && WM_NETIF_TYPE_WIFI_AP == interface) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_stop_dnss_lwip(netif);
#endif
    }
}

void wm_netif_start_dhcps(wm_netif_type_t interface, wm_ip_addr_t *ipaddr, wm_ip_addr_t *netmask, wm_ip_addr_t *gw)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && WM_NETIF_TYPE_WIFI_AP == interface) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_start_dhcps_lwip(netif, ipaddr, netmask, gw);
#endif
    }
}

void wm_netif_stop_dhcps(wm_netif_type_t interface)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && WM_NETIF_TYPE_WIFI_AP == interface) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_stop_dhcps_lwip(netif);
#endif
    }
}

void wm_netif_set_dns(wm_netif_type_t interface, uint8_t index, wm_ip_addr_t *ip)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && ((WM_NETIF_TYPE_WIFI_STA == interface)
#if CONFIG_WM_NETIF_ENABLE_ETH
                  || (WM_NETIF_TYPE_ETH == interface)
#endif
                      )) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_set_dns_lwip(index, ip);
#endif
    }
}

void wm_netif_get_dns(wm_netif_type_t interface, uint8_t index, wm_ip_addr_t *ip)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && ((WM_NETIF_TYPE_WIFI_STA == interface)
#if CONFIG_WM_NETIF_ENABLE_ETH
                  || (WM_NETIF_TYPE_ETH == interface)
#endif
                      )) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_get_dns_lwip(index, ip);
#endif
    }
}

void wm_netif_set_ipaddr(wm_netif_type_t interface, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_set_ipaddr_lwip(netif, ip, netmask, gw);
#endif
    }
}

void wm_netif_get_ipaddr(wm_netif_type_t interface, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_get_ipaddr_lwip(netif, ip, netmask, gw);
#endif
    }
}

int wm_netif_get_ll_ip6(wm_netif_type_t interface, wm_ip_addr_t *ip6)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        return wm_netif_get_ll_ip6_lwip(netif, ip6);
#endif
    }

    return WM_ERR_FAILED;
}

void wm_netif_start_dhcpc(wm_netif_type_t interface)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && ((WM_NETIF_TYPE_WIFI_STA == interface)
#if CONFIG_WM_NETIF_ENABLE_ETH
                  || (WM_NETIF_TYPE_ETH == interface)
#endif
                      )) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_start_dhcpc_lwip(netif);
#endif
    }
}

void wm_netif_stop_dhcpc(wm_netif_type_t interface)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && ((WM_NETIF_TYPE_WIFI_STA == interface)
#if CONFIG_WM_NETIF_ENABLE_ETH
                  || (WM_NETIF_TYPE_ETH == interface)
#endif
                      )) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        wm_netif_stop_dhcpc_lwip(netif);
#endif
    }
}

int wm_netif_set_mtu(wm_netif_type_t interface, uint16_t mtu)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        return wm_netif_set_mtu_lwip(netif, mtu);
#endif
    }

    return WM_ERR_FAILED;
}

uint16_t wm_netif_get_mtu(wm_netif_type_t interface)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        return wm_netif_get_mtu_lwip(netif);
#endif
    }

    return 0;
}

int wm_netif_set_hostname(wm_netif_type_t interface, const char *hostname)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        return wm_netif_set_hostname_lwip(netif, hostname);
#endif
    }

    return 0;
}

int wm_netif_get_hostname(wm_netif_type_t interface, const char *hostname)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        return wm_netif_get_hostname_lwip(netif, hostname);
#endif
    }

    return 0;
}

int wm_netif_get_netif_impl_index(wm_netif_t *netif)
{
    if (!netif) {
        return 0;
    }

#ifdef CONFIG_WM_NETIF_SELECT_LWIP
    return wm_netif_get_netif_impl_index_lwip(netif);
#else
    return 0;
#endif
}

int wm_netif_ip6_enable(wm_netif_type_t interface, uint8_t enable)
{
    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif && ((WM_NETIF_TYPE_WIFI_STA == interface) || (WM_NETIF_TYPE_WIFI_AP == interface)
#if CONFIG_WM_NETIF_ENABLE_ETH
                  || (WM_NETIF_TYPE_ETH == interface)
#endif
                      )) {
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
        return wm_netif_ip6_enable_lwip(netif, enable);
#endif
    }

    return WM_ERR_FAILED;
}

int wm_netif_addif(wm_netif_type_t interface)
{
#if CONFIG_COMPONENT_WIFI_ENABLED
    int ret = WM_ERR_SUCCESS;
    uint8_t mac_addr[WM_MAC_ADDR_LEN];

    uint8_t if_name[2];
    char hostname[32];

    if (interface < WM_NETIF_TYPE_WIFI_STA || interface >= WM_NETIF_TYPE_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif) {
        wm_log_warn("netif %d have inited", interface);
        return WM_ERR_SUCCESS;
    }

    netif = wm_os_internal_malloc(sizeof(wm_netif_t));
    if (!netif) {
        return WM_ERR_NO_MEM;
    }

    g_wm_netif[interface] = netif;
    memset(netif, 0, sizeof(wm_netif_t));

    netif->type = interface;
#if CONFIG_WIFI_ENABLE_AP_FEATURE
    if (WM_NETIF_TYPE_WIFI_AP == interface) {
        ret = wm_sys_get_mac_addr(WM_MAC_TYPE_SAP, mac_addr, sizeof(mac_addr));
    } else
#endif
    {
        ret = wm_sys_get_mac_addr(WM_MAC_TYPE_STA, mac_addr, sizeof(mac_addr));
    }

    if (ret != WM_ERR_SUCCESS) {
        wm_os_internal_free(netif);
        g_wm_netif[interface] = NULL;
        wm_log_error("netif %d init failed as get mac abnormal", interface);
        return ret;
    }

    if_name[0] = WM_NETIF_NAME_B0;
    if_name[1] = interface + '0';
    snprintf(hostname, sizeof(hostname), "%s-%02x%02x%02x%02x", WM_NETIF_HOSTNAME, mac_addr[0], mac_addr[3], mac_addr[4],
             mac_addr[5]);

#ifdef CONFIG_WM_NETIF_SELECT_LWIP
    ret = wm_netif_addif_lwip(netif, mac_addr, if_name, hostname);
#endif
    if (ret) {
        wm_os_internal_free(netif);
        g_wm_netif[interface] = NULL;
        wm_log_error("netif %d init failed", interface);
    }

    return ret;

#else
    return WM_ERR_FAILED;
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */
}

int wm_netif_delif(wm_netif_type_t interface)
{
#if CONFIG_COMPONENT_WIFI_ENABLED
    int ret = WM_ERR_SUCCESS;

    wm_netif_t *netif = wm_netif_get_netif(interface);

    if (netif == NULL) {
        return WM_ERR_SUCCESS;
    }

#ifdef CONFIG_WM_NETIF_SELECT_LWIP
    ret = wm_netif_delif_lwip(netif);
#endif
    if (!ret) {
        wm_os_internal_free(netif);
        g_wm_netif[interface] = NULL;
    } else {
        wm_log_error("netif %d deinit failed", interface);
    }

    return ret;

#else
    return WM_ERR_FAILED;
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */
}

void wm_netif_cleanup_tcb(void *tcb)
{
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
    wm_netif_cleanup_tcb_lwip(tcb);
#endif
}

int wm_netif_init(void *sync_sem)
{
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
    wm_netif_init_lwip(sync_sem);
#endif

    return WM_ERR_SUCCESS;
}

int wm_netif_deinit(void *sync_sem)
{
#ifdef CONFIG_WM_NETIF_SELECT_LWIP
    wm_netif_deinit_lwip(sync_sem);
#endif

    return WM_ERR_SUCCESS;
}
