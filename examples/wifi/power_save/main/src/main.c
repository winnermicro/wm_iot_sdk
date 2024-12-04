#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"

#define LOG_TAG "power_save"
#include "wm_log.h"

/*set the ssid and password via "menuconfig"*/
#define DEFAULT_SSID            CONFIG_EXAMPLE_WIFI_SSID
#define DEFAULT_PWD             CONFIG_EXAMPLE_WIFI_PASSWORD

#define DEFAULT_LISTEN_INTERVAL CONFIG_EXAMPLE_WIFI_LISTEN_INTERVAL
#define DEFAULT_BEACON_TIMEOUT  CONFIG_EXAMPLE_WIFI_BEACON_TIMEOUT

#if CONFIG_EXAMPLE_POWER_SAVE_MIN_MODEM
#define DEFAULT_PS_MODE WM_WIFI_PS_MIN_MODEM
#elif CONFIG_EXAMPLE_POWER_SAVE_MAX_MODEM
#define DEFAULT_PS_MODE WM_WIFI_PS_MAX_MODEM
#elif CONFIG_EXAMPLE_POWER_SAVE_NONE
#define DEFAULT_PS_MODE WM_WIFI_PS_NONE
#else
#define DEFAULT_PS_MODE WM_WIFI_PS_NONE
#endif /*CONFIG_POWER_SAVE_MODEM*/

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

/*init wifi as sta and set power save mode*/
void wifi_power_save(void)
{
    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
    wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);

    int err = wm_wifi_init();

    wm_wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD,
            .listen_interval = DEFAULT_LISTEN_INTERVAL,
        },
    };
    if (!err)
        err = wm_wifi_set_config(WM_WIFI_IF_STA, &wifi_config);
    if (!err)
        err = wm_wifi_connect();
    if (!err)
        err = wm_wifi_set_inactive_time(WM_WIFI_IF_STA, DEFAULT_BEACON_TIMEOUT);

    if (err) {
        wm_log_error("wifi_power_save failed.");
        wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
        wm_event_remove_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);
        return;
    }

    wm_log_info("wm_wifi_set_ps().");
    wm_wifi_set_ps(DEFAULT_PS_MODE);
}

int main(void)
{
    wifi_power_save();

    return 0;
}
