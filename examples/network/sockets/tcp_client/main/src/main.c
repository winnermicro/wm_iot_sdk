#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "wm_nm_api.h"

#define LOG_TAG "tcp-client"
#include "wm_log.h"
/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

extern void tcp_client(void);

static void wifi_init_sta(void)
{
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };
    int ret                      = wm_nm_start_wifi_station(&cfg);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wifi station start fail: %d", ret);
        return;
    }
    while (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    wm_log_info("wifi station start success, network is ready!");
}

int main(void)
{
    wifi_init_sta();

#if defined(CONFIG_EXAMPLE_IPV6)
    vTaskDelay(2000); // Waiting to create IPv6 address
#endif

    tcp_client();

    return 0;
}
