#include "wmsdk_config.h"
#include "wm_types.h"
#include "freertos/FreeRTOS.h"
#include "wm_event.h"
#include "wm_nm_api.h"

#define LOG_TAG "example"
#include "wm_log.h"
#if !CONFIG_WIFI_API_ENABLED
#error "Must enable WIFI-API"
#endif

#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

static wm_nm_state_e s_station_state = WM_NM_EXIT;

static void wm_nm_event_handle(wm_event_group_t group, int event, void *data, void *priv)
{
    switch (event) {
        case WM_NM_WIFI_STA_GOT_IP:
        {
            s_station_state = WM_NM_WIFI_STA_GOT_IP;
            wm_log_info("got ip, network is ready!");
            break;
        }
        case WM_NM_WIFI_STA_LOST_IP:
        {
            s_station_state = WM_NM_WIFI_STA_LOST_IP;
            wm_log_info("lost ip, waiting for get IP");
            break;
        }
        default:
        {
            break;
        }
    }
}

int main(void)
{
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };

    wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);

    if (wm_nm_start_wifi_station(&cfg) != WM_ERR_SUCCESS) {
        wm_log_error("NM Wi-Fi station start failed");
        wm_event_remove_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);
        return WM_ERR_FAILED;
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // wait for GOT IP event
        while (s_station_state == WM_NM_WIFI_STA_GOT_IP) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            // you can use the network to send & receive data.
        }
    }

    return WM_ERR_SUCCESS;
}
