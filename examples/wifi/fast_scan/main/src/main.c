#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define LOG_TAG "fast scan"
#include "wm_log.h"

/* Set the SSID and Password via project configuration, or can set directly here */
#define DEFAULT_SSID CONFIG_EXAMPLE_WIFI_SSID
#define DEFAULT_PWD  CONFIG_EXAMPLE_WIFI_PASSWORD

#if CONFIG_EXAMPLE_WIFI_ALL_CHANNEL_SCAN
#define DEFAULT_SCAN_METHOD WM_WIFI_ALL_CHANNEL_SCAN
#elif CONFIG_EXAMPLE_WIFI_FAST_SCAN
#define DEFAULT_SCAN_METHOD WM_WIFI_FAST_SCAN
#else
#define DEFAULT_SCAN_METHOD WM_WIFI_FAST_SCAN
#endif /*CONFIG_EXAMPLE_SCAN_METHOD*/

#if CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_SORT_METHOD WM_WIFI_CONNECT_AP_BY_SIGNAL
#elif CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SECURITY
#define DEFAULT_SORT_METHOD WM_WIFI_CONNECT_AP_BY_SECURITY
#else
#define DEFAULT_SORT_METHOD WM_WIFI_CONNECT_AP_BY_SIGNAL
#endif /*CONFIG_EXAMPLE_SORT_METHOD*/

#if CONFIG_EXAMPLE_FAST_SCAN_THRESHOLD
#define DEFAULT_RSSI CONFIG_EXAMPLE_FAST_SCAN_MINIMUM_SIGNAL
#if CONFIG_EXAMPLE_FAST_SCAN_WEAKEST_AUTHMODE_OPEN
#define DEFAULT_AUTHMODE WM_WIFI_AUTH_OPEN
#elif CONFIG_EXAMPLE_FAST_SCAN_WEAKEST_AUTHMODE_WEP
#define DEFAULT_AUTHMODE WM_WIFI_AUTH_WEP
#elif CONFIG_EXAMPLE_FAST_SCAN_WEAKEST_AUTHMODE_WPA
#define DEFAULT_AUTHMODE WM_WIFI_AUTH_WPA_PSK
#elif CONFIG_EXAMPLE_FAST_SCAN_WEAKEST_AUTHMODE_WPA2
#define DEFAULT_AUTHMODE WM_WIFI_AUTH_WPA2_PSK
#else
#define DEFAULT_AUTHMODE WM_WIFI_AUTH_OPEN
#endif
#else
#define DEFAULT_RSSI     -127
#define DEFAULT_AUTHMODE WM_WIFI_AUTH_OPEN
#endif /*CONFIG_EXAMPLE_FAST_SCAN_THRESHOLD*/

static void event_handler_sta_disconnected(wm_event_group_t group, int event, void *data, void *priv)
{
    if (WM_EVENT_WIFI_STA_DISCONNECTED == event) {
        wm_log_info("retry to connect to the AP");

        wm_wifi_connect();
    }
}

static void event_handler_sta_got_ip(wm_event_group_t group, int event, wm_lwip_event_data_t *data, void *priv)
{
    if (WM_EVENT_WIFI_STA_GOT_IP == event) {
        char ip[16];
        ipaddr_ntoa_r((ip_addr_t *)&data->sta_got_ip_info.ip, ip, sizeof(ip));
        wm_log_info("got ip: %s", ip);
    }
}

static void fast_scan(void)
{
    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
    wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);

    int err = wm_wifi_init();

    wm_wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD,
            .scan_method = DEFAULT_SCAN_METHOD,
            .sort_method = DEFAULT_SORT_METHOD,
            .threshold.rssi = DEFAULT_RSSI,
            .threshold.authmode = DEFAULT_AUTHMODE,
        },
    };

    if (!err)
        err = wm_wifi_set_config(WM_WIFI_IF_STA, &wifi_config);

    if (!err)
        err = wm_wifi_connect();

    if (!err) {
        wm_log_info("fast_scan finished.");
    } else {
        wm_log_error("fast_scan failed.");
        wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
        wm_event_remove_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);
    }
}

int main(void)
{
    fast_scan();

    return 0;
}
