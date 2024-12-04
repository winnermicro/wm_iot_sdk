#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_event.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#ifdef CONFIG_WIFI_API_ENABLED
#include "wm_wifi.h"
#include "wm_netif.h"
#endif
#include "wm_wifi_drv.h"

#define LOG_TAG "wifi"
#include "wm_log.h"

WM_EVENT_DEFINE_GROUP(WM_WIFI_EV); /**< define wifi event group */

#define WIFI_INIT_SYNC_WAIT_TIME WM_OS_WAIT_TIME_MAX //(5 * HZ)

#define WIFI_DHCPS_PARSE_IP(conf, ipaddr)                                \
    ip = (uint8_t *)&ipaddr.u_addr.ip4.addr;                             \
    sscanf(conf, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]); \
    ipaddr.type = WM_IPADDR_TYPE_V4

extern int wm_wifi_load_cfg(void);
extern void wm_wpa_supplicant_deinit(void *sync_sem);
extern void wm_wpa_supplicant_init(void *sync_sem);

static uint8_t wifi_is_inited = 0;

#ifdef CONFIG_WIFI_API_ENABLED
static void wifi_event_default_callback(wm_event_group_t group, int event, wm_wifi_event_data_t *data, void *priv)
{
    //wm_wifi_ps_type_t type = WM_WIFI_PS_NONE;

    wm_log_debug("distribution event %d", event);

    switch (event) {
        case WM_EVENT_WIFI_SCAN_DONE:
        {
            wm_log_debug("scan finish number %d", data->scan_done_info.number);
            break;
        }
        case WM_EVENT_WIFI_STA_CONNECTED:
        {
            uint8_t *bssid = data->sta_connected_info.bssid;
            wm_log_debug("sta connected %02X:%02X:%02X:%02X:%02X:%02X channel %d authmode %d", bssid[0], bssid[1], bssid[2],
                         bssid[3], bssid[4], bssid[5], data->sta_connected_info.channel, data->sta_connected_info.authmode);
#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
            wm_netif_addif(WM_NETIF_TYPE_WIFI_STA);
            wm_netif_ip6_enable(WM_NETIF_TYPE_WIFI_STA, 1);
            wm_netif_start_dhcpc(WM_NETIF_TYPE_WIFI_STA);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */
            break;
        }
        case WM_EVENT_WIFI_STA_DISCONNECTED:
        {
            wm_log_debug("sta disconnected reason %d", data->sta_disconnected_info.reason);
#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
            wm_netif_stop_dhcpc(WM_NETIF_TYPE_WIFI_STA);
            wm_netif_delif(WM_NETIF_TYPE_WIFI_STA);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */
            break;
        }
        case WM_EVENT_WIFI_AP_START:
        {
            wm_log_debug("softap started");
#ifdef CONFIG_WIFI_ENABLE_AP_FEATURE
#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
            uint8_t *ip;
            wm_ip_addr_t ipaddr, netmask, gw;
            WIFI_DHCPS_PARSE_IP(CONFIG_WIFI_DHCPS_IPV4_ADDR, ipaddr);
            WIFI_DHCPS_PARSE_IP(CONFIG_WIFI_DHCPS_IPV4_NETMASK, netmask);
            WIFI_DHCPS_PARSE_IP(CONFIG_WIFI_DHCPS_IPV4_GW, gw);
            wm_netif_addif(WM_NETIF_TYPE_WIFI_AP);
            wm_netif_ip6_enable(WM_NETIF_TYPE_WIFI_AP, 1);
            wm_netif_start_dhcps(WM_NETIF_TYPE_WIFI_AP, &ipaddr, &netmask, &gw);
            wm_netif_start_dnss(WM_NETIF_TYPE_WIFI_AP);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */
#endif
            break;
        }
        case WM_EVENT_WIFI_AP_STOP:
        {
            wm_log_debug("softap stoped");
#ifdef CONFIG_WIFI_ENABLE_AP_FEATURE
#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
            wm_netif_stop_dnss(WM_NETIF_TYPE_WIFI_AP);
            wm_netif_stop_dhcps(WM_NETIF_TYPE_WIFI_AP);
            wm_netif_delif(WM_NETIF_TYPE_WIFI_AP);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */
#endif
            break;
        }
        case WM_EVENT_WIFI_AP_STACONNECTED:
        {
            uint8_t *mac = data->ap_staconnected_info.addr;
            wm_log_debug("sta %02X:%02X:%02X:%02X:%02X:%02X aid %hhu joined softap", mac[0], mac[1], mac[2], mac[3], mac[4],
                         mac[5], data->ap_staconnected_info.aid);
            break;
        }
        case WM_EVENT_WIFI_AP_STADISCONNECTED:
        {
            uint8_t *mac = data->ap_staconnected_info.addr;
            wm_log_debug("sta %02X:%02X:%02X:%02X:%02X:%02X aid %hhu left softap", mac[0], mac[1], mac[2], mac[3], mac[4],
                         mac[5], data->ap_stadisconnected_info.aid);
            break;
        }

        default:
        {
            break;
        }
    }
}
#endif /* CONFIG_WIFI_API_ENABLED */

#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
static void wifi_lwip_event_default_callback(wm_event_group_t group, int event, wm_lwip_event_data_t *data, void *priv)
{
#ifdef CONFIG_WIFI_API_ENABLED
    //wm_wifi_ps_type_t type = WM_WIFI_PS_NONE;
#endif

    wm_log_debug("distribution event %d", event);

    switch (event) {
#ifdef CONFIG_WIFI_API_ENABLED

        case WM_EVENT_WIFI_STA_GOT_IP:
        {
            char ip[16];
            ipaddr_ntoa_r((ip_addr_t *)&data->sta_got_ip_info.ip, ip, sizeof(ip));
            wm_log_debug("sta got ip %s", ip);
            //wm_wifi_get_ps(&type);
            //wm_wifi_set_ps(type);
            break;
        }
        case WM_EVENT_WIFI_STA_LOST_IP:
        {
            wm_log_debug("sta lost ip");
            break;
        }
#if LWIP_IPV6
        case WM_EVENT_WIFI_STA_GOT_IP6:
        {
            char ip[64];
            ip6addr_ntoa_r((ip6_addr_t *)&data->sta_got_ip6_info.ip, ip, sizeof(ip));
            wm_log_debug("sta got ip6 %s (index %hhu)", ip, data->sta_got_ip6_info.index);
            //wm_wifi_get_ps(&type);
            //wm_wifi_set_ps(type);
            break;
        }
        case WM_EVENT_WIFI_STA_LOST_IP6:
        {
            wm_log_debug("sta lost ip6 (index %hhu)", data->sta_lost_ip6_info.index);
            break;
        }
#endif
#endif

#ifdef CONFIG_WIFI_API_ENABLED
        case WM_EVENT_WIFI_AP_STAIPASSIGNED:
        {
            uint8_t *mac = data->ap_staipassigned_info.addr;
            char ip[16];
            ipaddr_ntoa_r((ip_addr_t *)&data->ap_staipassigned_info.ip, ip, sizeof(ip));

            wm_log_debug("sta %02X:%02X:%02X:%02X:%02X:%02X assigned ip %s", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                         ip);
            break;
        }
#endif
        default:
        {
            break;
        }
    }
}

#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */

int wm_wifi_deinit(void)
{
    wm_os_sem_t *sync_sem = NULL;
    if (wifi_is_inited) {
        wm_os_internal_sem_create(&sync_sem, 0);

#ifdef CONFIG_WIFI_API_ENABLED
        wm_wpa_supplicant_deinit(sync_sem);
        wm_os_internal_sem_acquire(sync_sem, WIFI_INIT_SYNC_WAIT_TIME);

#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
        wm_netif_deinit(sync_sem);
        wm_os_internal_sem_acquire(sync_sem, WIFI_INIT_SYNC_WAIT_TIME);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */
#endif

        wm_os_internal_sem_delete(sync_sem);

#ifdef CONFIG_WIFI_API_ENABLED
        wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)wifi_event_default_callback, NULL);
#endif /* CONFIG_WIFI_API_ENABLED */
#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
        wm_event_remove_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)wifi_lwip_event_default_callback, NULL);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */

        wifi_is_inited = 0;
    }

    return WM_ERR_SUCCESS;
}

int wm_wifi_init(void)
{
    int ret = WM_ERR_SUCCESS;
#ifdef CONFIG_WIFI_API_ENABLED
    wm_os_sem_t *init_sync_sem = NULL;
#endif

    if (wifi_is_inited)
        return WM_ERR_SUCCESS;

#ifdef CONFIG_WIFI_API_ENABLED
    ret = wm_os_internal_sem_create(&init_sync_sem, 0);

#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
    if (!ret)
        wm_netif_init(init_sync_sem);

    if (!ret)
        ret = wm_os_internal_sem_acquire(init_sync_sem, WIFI_INIT_SYNC_WAIT_TIME);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */

    if (!ret)
        wm_wpa_supplicant_init(init_sync_sem);

    if (!ret)
        ret = wm_os_internal_sem_acquire(init_sync_sem, WIFI_INIT_SYNC_WAIT_TIME);
#endif

    if (!ret) {
#ifdef CONFIG_WIFI_API_ENABLED
        wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)wifi_event_default_callback, NULL);
#endif /* CONFIG_WIFI_API_ENABLED */
#ifndef CONFIG_COMPONENT_NET_MANAGER_ENABLED
        ret = wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)wifi_lwip_event_default_callback, NULL);
#endif /* CONFIG_COMPONENT_NET_MANAGER_ENABLED */
    }

#ifdef CONFIG_WIFI_API_ENABLED
    wm_wifi_load_cfg();
#endif

    if (!ret) {
#ifdef CONFIG_WIFI_API_ENABLED
        wm_wifi_config_80211_tx_rate(WM_WIFI_IF_MAX, WM_WIFI_PHY_RATE_1M_L);
        wm_wifi_set_ps(WM_WIFI_PS_MIN_MODEM);
        wm_wifi_set_country_code("CN");
#endif

        wifi_is_inited = 1;
    } else {
        wm_log_error("wifi init failed");
    }

#ifdef CONFIG_WIFI_API_ENABLED
    if (init_sync_sem) {
        wm_os_internal_sem_delete(init_sync_sem);
    }
#endif

    return ret ? WM_ERR_FAILED : WM_ERR_SUCCESS;
}
