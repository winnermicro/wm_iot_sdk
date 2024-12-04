#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "wm_netif.h"
#include "lwip/ip_addr.h"

#define LOG_TAG "wifi softap"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID    CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASS    CONFIG_EXAMPLE_WIFI_PASSWORD
#define EXAMPLE_WIFI_CHANNEL CONFIG_EXAMPLE_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN CONFIG_EXAMPLE_MAX_STA_CONN

static void event_handler_ap(wm_event_group_t group, int event, wm_wifi_event_data_t *data, void *priv)
{
    uint8_t *mac;
    if (event == WM_EVENT_WIFI_AP_STACONNECTED) {
        mac = data->ap_staconnected_info.addr;
        wm_log_info("station %02X:%02X:%02X:%02X:%02X:%02X join, AID=%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                    data->ap_staconnected_info.aid);
    } else if (event == WM_EVENT_WIFI_AP_STADISCONNECTED) {
        mac = data->ap_stadisconnected_info.addr;
        wm_log_info("station %02X:%02X:%02X:%02X:%02X:%02X leave, AID=%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                    data->ap_stadisconnected_info.aid);
    }
}

void wifi_init_softap(void)
{
    int err;
    char ip[16];
    wm_ip_addr_t ipaddr;
    wm_wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_WIFI_SSID),
            .channel = EXAMPLE_WIFI_CHANNEL,
            .password = EXAMPLE_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WM_WIFI_AUTH_WPA2_PSK,
            .pairwise_cipher = WM_WIFI_CIPHER_TYPE_CCMP,
        },
    };

    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_ap, NULL);

    err = wm_wifi_init();

    if (strlen(EXAMPLE_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WM_WIFI_AUTH_OPEN;
    }

    if (!err)
        err = wm_wifi_set_config(WM_WIFI_IF_AP, &wifi_config);
    if (!err)
        err = wm_wifi_ap_start();

    if (!err) {
        wm_log_info("wifi_init_softap finished. SSID:%s password:%s channel:%d", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS,
                    EXAMPLE_WIFI_CHANNEL);

        wm_netif_get_ipaddr(WM_NETIF_TYPE_WIFI_AP, &ipaddr, NULL, NULL);
        ipaddr_ntoa_r((ip_addr_t *)&ipaddr, ip, sizeof(ip));
        wm_log_info("softap ip: %s", ip);
    } else {
        wm_log_error("wifi_init_softap failed. SSID:%s password:%s channel:%d", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS,
                     EXAMPLE_WIFI_CHANNEL);

        wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_ap, NULL);
    }
}

int main(void)
{
    wm_log_info("WM_WIFI_MODE_AP");
    wifi_init_softap();

    return 0;
}
