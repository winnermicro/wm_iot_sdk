#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "wm_pm.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/ip_addr.h"

#define LOG_TAG "demo"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID       CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASS       CONFIG_EXAMPLE_WIFI_PASSWORD
#define EXAMPLE_MAXIMUM_RETRY   CONFIG_EXAMPLE_MAXIMUM_RETRY
#define EXAMPLE_LISTEN_INTERVAL CONFIG_EXAMPLE_WIFI_LISTEN_INTERVAL

static int s_retry_num = 0;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT 0x00000001
#define WIFI_FAIL_BIT      0x00000002

static void event_handler_sta_disconnected(wm_event_group_t group, int event, void *data, void *priv)
{
    if (event == WM_EVENT_WIFI_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_MAXIMUM_RETRY) {
            wm_wifi_connect();
            s_retry_num++;
            wm_log_info("retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        wm_log_info("connect to the AP fail");
    }
}

static void event_handler_sta_got_ip(wm_event_group_t group, int event, wm_lwip_event_data_t *data, void *priv)
{
    if (event == WM_EVENT_WIFI_STA_GOT_IP) {
        char ip[16];
        ipaddr_ntoa_r((ip_addr_t *)&data->sta_got_ip_info.ip, ip, sizeof(ip));
        wm_log_info("got ip: %s", ip);
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
    wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);

    int err = wm_wifi_init();

    wm_wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
            .listen_interval = EXAMPLE_LISTEN_INTERVAL,
        },
    };

    if (!err)
        err = wm_wifi_set_config(WM_WIFI_IF_STA, &wifi_config);

    if (!err)
        err = wm_wifi_connect();

    if (!err) {
        wm_log_info("wifi_init_sta finished.");
        wm_wifi_set_ps(WM_WIFI_PS_MAX_MODEM);
    } else {
        wm_log_error("wifi_init_sta failed.");
        wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
        wm_event_remove_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);
        vEventGroupDelete(s_wifi_event_group);
        return;
    }

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits =
        xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        wm_log_info("connected to ap SSID:%s password:%s", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        wm_log_info("Failed to connect to SSID:%s, password:%s", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    } else {
        wm_log_error("UNEXPECTED EVENT");
    }
}

int main(void)
{
    int ret               = WM_ERR_SUCCESS;
    wm_pm_config_t config = { WM_PM_MODE_DEEP_SLEEP, 0, 0 };

    ret = wm_pm_set_config(&config);

    if (WM_ERR_SUCCESS == ret)
        wifi_init_sta();

    return 0;
}
