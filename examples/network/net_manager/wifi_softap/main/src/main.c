#include "wmsdk_config.h"
#include "wm_types.h"

#include "freertos/FreeRTOS.h"
#include "wm_event.h"
#include "wm_nm_api.h"

#define LOG_TAG "example"
#include "wm_log.h"
#if !CONFIG_WIFI_API_ENABLED || !CONFIG_WIFI_ENABLE_AP_FEATURE
#error "Must enable WIFI-API & AP_FEATURE"
#endif

#ifndef CONFIG_EXAMPLE_SOFTAP_PASSWORD
#define CONFIG_EXAMPLE_SOFTAP_PASSWORD NULL
#endif

static wm_nm_state_e s_softap_state = WM_NM_EXIT;

static void wm_nm_event_handle(wm_event_group_t group, int event, void *data, void *priv)
{
    switch (event) {
        case WM_NM_WIFI_AP_READY:
        {
            s_softap_state = WM_NM_WIFI_AP_READY;
            wm_log_info("softAP is ready!");
            break;
        }
        case WM_NM_WIFI_AP_ASSIGNED_IP_ID:
        {
            wm_log_info("assigned IP to staion");
            break;
        }
        case WM_NM_EXIT:
        {
            s_softap_state = WM_NM_EXIT;
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
    wm_nm_wifi_softap_cfg_t cfg = { CONFIG_EXAMPLE_SOFTAP_SSID, CONFIG_EXAMPLE_SOFTAP_PASSWORD };

    wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);

    if (wm_nm_start_wifi_softap(&cfg) != WM_ERR_SUCCESS) {
        wm_event_remove_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);
        wm_log_error("NM Wi-Fi softap start failed");
        return WM_ERR_FAILED;
    }

    while (s_softap_state != WM_NM_WIFI_AP_READY) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    // softAP is ready.

    return WM_ERR_SUCCESS;
}
