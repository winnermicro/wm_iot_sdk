#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "lwip/ip_addr.h"
#include "wm_osal.h"
#include "wm_utils.h"
#include "wm_component.h"
#include "wm_netif.h"
#include "lwip/ip_addr.h"

#if CONFIG_COMPONENT_WIFI_ENABLED
#include "wm_wifi.h"
#include "wm_wifi_event.h"
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */

#include "wm_nm_api.h"
#include "wm_nm_wifi.h"
#include "wm_nm_netstack.h"
#include "wm_nm_core.h"

#define LOG_TAG "wm_nm_core"
#include "wm_log.h"

#define WM_TASK_NET_MGR_NAME "net_mgr"
WM_EVENT_DEFINE_GROUP(WM_NM_EV); /**< Define net manager event group */

static wm_nm_param_t s_nm_param = { 0 };
static void wm_nm_event_post(wm_nm_state_e event, void *data, size_t size);

#if CONFIG_COMPONENT_WIFI_ENABLED
static void wm_nm_wifi_station_connect_handle(void)
{
    wm_nm_netif_t netif;

    wm_netif_addif(WM_NETIF_TYPE_WIFI_STA);

    netif = wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME);
    assert(netif != WM_NM_INVALID_NETIF);

    if (!(s_nm_param.info[WM_NETIF_TYPE_WIFI_STA].ip6_disable)) {
        wm_nm_enable_netif_ip6(netif);
    }

    if (s_nm_param.info[WM_NETIF_TYPE_WIFI_STA].is_static_ip) {
        wm_nm_set_netif_ip_info(netif, &s_nm_param.info[WM_NETIF_TYPE_WIFI_STA].ip_info);
    } else {
        wm_nm_start_netif_dhcpc(netif);
    }

#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
    wm_nm_wifi_station_reconnect_stop();
#endif /* CONFIG_NM_WIFI_STA_AUTO_CONNECT */
}

static void wm_nm_wifi_station_disconnect_handle(void)
{
    wm_nm_event_post(WM_NM_WIFI_STA_LOST_IP, NULL, 0);
    wm_nm_stop_netif_dhcpc(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME));
    wm_netif_delif(WM_NETIF_TYPE_WIFI_STA);
#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
    wm_nm_wifi_station_reconnect_start(CONFIG_NM_STA_RECONNECT_INTERVAL);
#endif /* CONFIG_NM_WIFI_STA_AUTO_CONNECT */
}

#if CONFIG_WIFI_ENABLE_AP_FEATURE

static void wm_nm_wifi_ap_start_handle(void)
{
    int ret;
    wm_nm_internal_netif_t *netif;

    wm_netif_addif(WM_NETIF_TYPE_WIFI_AP);

    netif = wm_netif_get_netif(WM_NETIF_TYPE_WIFI_AP);
    if (netif == NULL) {
        wm_log_error("query AP netif fail");
        return;
    }

    if (!(s_nm_param.info[WM_NETIF_TYPE_WIFI_AP].ip6_disable)) {
        wm_net_stack_set_ip6_state(netif->netif, true);
    }

    ret = wm_net_stack_set_dhcps_option(netif->netif, &(s_nm_param.dhcps_option));
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("DHCP server set option failed");
    }

    if (!s_nm_param.dhcps_disabled) {
        ret = wm_net_stack_dhcps_start(netif->netif);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("DHCP server startup failed");
        }
    }

    if (!s_nm_param.dnss_disabled) {
        ret = wm_net_stack_dnss_start(netif->netif);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("DNS server startup failed");
        }
    }

    wm_nm_event_post(WM_NM_WIFI_AP_READY, NULL, 0);
}

static void wm_nm_wifi_ap_stop_handle(void)
{
    wm_nm_netif_t netif;
    netif = wm_nm_get_netif_by_name(WIFI_SOFTAP_NETIF_NAME);
    wm_nm_stop_netif_dnss(netif);
    wm_nm_stop_netif_dhcps(netif);
    wm_netif_delif(WM_NETIF_TYPE_WIFI_AP);
}
#endif /** CONFIG_WIFI_ENABLE_AP_FEATURE */

static void wm_wifi_event_callback(wm_event_group_t group, int event, void *data, void *priv)
{
    wm_wifi_event_data_t *wifi_data = (wm_wifi_event_data_t *)data;

    wm_log_debug("distribution %s event %d", group, event);

    switch (event) {
        case WM_EVENT_WIFI_STA_CONNECTED:
        {
            wm_log_info("%.*s is connected", wifi_data->sta_connected_info.ssid_len, wifi_data->sta_connected_info.ssid);
            wm_nm_wifi_station_connect_handle();
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
            wm_nm_set_wifi_station_state_internal(WM_NM_WIFI_STA_CONNECTED);
#endif
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED && CONFIG_NM_WIFI_SOFTAP_AUTO_CHANNEL
            wm_nm_msg_t msg = { .type = WM_NM_RF_CHANNEL_CHANGE };
            msg.channel     = wifi_data->sta_connected_info.channel;
            wm_nm_core_send_msg(&msg);
#endif
            break;
        }
        case WM_EVENT_WIFI_STA_DISCONNECTED:
        {
            wm_log_warn("%s is disconnect, reason is %d", wifi_data->sta_disconnected_info.ssid,
                        wifi_data->sta_disconnected_info.reason);
            wm_nm_wifi_station_disconnect_handle();
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
            wm_nm_set_wifi_station_state_internal(WM_NM_WIFI_STA_DISCONNECTED);
#endif
            break;
        }

#if CONFIG_WIFI_ENABLE_AP_FEATURE
        case WM_EVENT_WIFI_AP_START:
        {
            wm_log_debug("softap started");
            wm_nm_wifi_ap_start_handle();
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
            wm_nm_set_wifi_softap_state_internal(WM_NM_WIFI_AP_READY);
#endif
            break;
        }
        case WM_EVENT_WIFI_AP_STOP:
        {
            wm_log_info("softap stoped");
            wm_nm_wifi_ap_stop_handle();
            break;
        }
#endif /** CONFIG_WIFI_ENABLE_AP_FEATURE */

        default:
        {
            break;
        }
    }
}
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */

static void wm_lwip_event_callback(wm_event_group_t group, int event, void *data, void *priv)
{
    wm_lwip_event_data_t *event_data = (wm_lwip_event_data_t *)data;
    wm_log_debug("distribution %s event %d", group, event);

    switch (event) {
        case WM_EVENT_WIFI_STA_GOT_IP:
        {
            char ip[16];
            ipaddr_ntoa_r((ip_addr_t *)&event_data->sta_got_ip_info.ip, ip, sizeof(ip));
            wm_log_info("sta got ip: %s", ip);
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
            wm_nm_set_wifi_station_state_internal(WM_NM_WIFI_STA_GOT_IP);
#endif
            wm_nm_event_post(WM_NM_WIFI_STA_GOT_IP, NULL, 0);
            break;
        }
        case WM_EVENT_WIFI_STA_LOST_IP:
        {
            wm_log_warn("sta lost ip");
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
            wm_nm_set_wifi_station_state_internal(WM_NM_WIFI_STA_LOST_IP);
#endif
            wm_nm_event_post(WM_NM_WIFI_STA_LOST_IP, NULL, 0);
            break;
        }
#if LWIP_IPV6
        case WM_EVENT_WIFI_STA_GOT_IP6:
        {
            char ip[64];

            wm_nm_event_post(WM_NM_WIFI_STA_GOT_IP6, NULL, 0);

            ip6addr_ntoa_r((ip6_addr_t *)&event_data->sta_got_ip6_info.ip, ip, sizeof(ip));
            wm_log_info("sta got ip6: %s", ip);
            break;
        }
        case WM_EVENT_WIFI_STA_LOST_IP6:
        {
            if (event_data->sta_lost_ip6_info.index > 0) {
                wm_log_warn("lost ip6");
                wm_nm_event_post(WM_NM_WIFI_STA_LOST_IP6, NULL, 0);
            }
            break;
        }
#endif
        case WM_EVENT_WIFI_AP_STAIPASSIGNED:
        {
            char ip[16];
            uint8_t *mac = event_data->ap_staipassigned_info.addr;

            ipaddr_ntoa_r((ip_addr_t *)&event_data->ap_staipassigned_info.ip, ip, sizeof(ip));

            wm_log_info("%02x:%02x:%02x:%02x:%02x:%02x assigned ip: %s", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ip);

            wm_nm_event_post(WM_NM_WIFI_AP_ASSIGNED_IP_ID, NULL, 0);
            break;
        }
        default:
        {
            break;
        }
    }
}

wm_nm_param_t *wm_nm_get_core_param(void)
{
    return &s_nm_param;
}

wm_nm_internal_netif_t *wm_nm_query_internal_netif(wm_nm_netif_t netif)
{
    return wm_netif_get_netif(netif);
}

void wm_nm_mutex_lock(void)
{
    wm_os_internal_mutex_acquire(s_nm_param.mutex, WM_OS_WAIT_TIME_MAX);
}

void wm_nm_mutex_unlock(void)
{
    wm_os_internal_mutex_release(s_nm_param.mutex);
}

static void wm_nm_event_post(wm_nm_state_e event, void *data, size_t size)
{
    static int last_event = -1;
    if (event == WM_NM_WIFI_STA_GOT_IP6 || last_event != event) {
        if (wm_event_send(WM_NM_EV, event, data, size) != WM_ERR_SUCCESS) {
            wm_log_error("WM_NM_EV post event %d fail", event);
        } else {
            last_event = event;
            wm_log_debug("WM_NM_EV post event %d success", event);
        }
    }
}

static int wm_nm_core_init(void)
{
    /** init wm_nm_param_t */
    memset(&s_nm_param, 0, sizeof(wm_nm_param_t));

#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
    s_nm_param.dhcps_option.server_ip.u_addr.ip4.addr = inet_addr(CONFIG_NM_DHCPS_IP);
    s_nm_param.dhcps_option.start_ip.u_addr.ip4.addr  = inet_addr(CONFIG_NM_DHCPS_START_IP);
    s_nm_param.dhcps_option.ip_mask.u_addr.ip4.addr   = inet_addr(CONFIG_NM_DHCPS_NETMASK);
    s_nm_param.dhcps_option.gateway.u_addr.ip4.addr   = inet_addr(CONFIG_NM_DHCPS_GATEWAY);
    s_nm_param.dhcps_option.dns1.u_addr.ip4.addr      = inet_addr(CONFIG_NM_DHCPS_IP);
    s_nm_param.dhcps_option.lease_time                = CONFIG_NM_DHCPS_LEASE_TIME;
#endif

    wm_os_internal_mutex_create(&s_nm_param.mutex);
    assert(s_nm_param.mutex != NULL);

    if (wm_net_stack_init() != WM_ERR_SUCCESS) {
        wm_log_error("net stack init failed");
        return WM_ERR_FAILED;
    }

#if CONFIG_COMPONENT_WIFI_ENABLED
    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, wm_wifi_event_callback, NULL);
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */
    wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, wm_lwip_event_callback, NULL);

#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED || CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
    if (wm_nm_wifi_init() != WM_ERR_SUCCESS) {
        wm_log_error("NM wifi module init failed");
        return WM_ERR_FAILED;
    }
#endif
    return WM_ERR_SUCCESS;
}
WM_COMPONEN_INIT_3(wm_nm_core_init);
