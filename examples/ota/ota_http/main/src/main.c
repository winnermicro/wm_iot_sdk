#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "wm_ota_http.h"
#include "wm_ota.h"
#include "wm_nm_api.h"
#define LOG_TAG "example"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif
#define OTA_HTTP_URL   CONFIG_OTA_IMAGE_URI

#define OTA_TASK_STACK (4096)
#define OTA_TASK_PRIO  (configMAX_PRIORITIES - 10)

static TaskHandle_t ota_task_handle = NULL;

void wm_ota_state_callback(wm_ota_state_t *ota_state)
{
    wm_log_debug("OTA progress:%d-%d%%.", ota_state->status, ota_state->progress);
    if (ota_state->status == WM_OTA_STATUS_CONNECTION_FAILED || ota_state->status == WM_OTA_STATUS_DOWNLOAD_FAILED ||
        ota_state->status == WM_OTA_STATUS_UPGRADE_FAILED || ota_state->status == WM_OTA_STATUS_ABORT) {
        wm_log_info("OTA failed.");
    }
    if (ota_state->status == WM_OTA_STATUS_UPGRADE_START) {
        wm_log_info("OTA finished.");
        wm_log_info("This example succeeds in running.");
    }
}

static void ota_task(void *arg)
{
    int ret = WM_ERR_SUCCESS;

    wm_ota_http_cfg_t cfg = {
        .fw_url = OTA_HTTP_URL, .ota_get_file_cb = NULL, .ota_state_cb = wm_ota_state_callback, .reboot = true
    };
    ret = wm_ota_http_update(&cfg);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("OTA http update error: %d.", ret);
        ret = WM_ERR_FAILED;
    }

    if (ota_task_handle) {
        vTaskDelete(ota_task_handle);
    }
}

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
    int err = xTaskCreate(ota_task, "ota_task", OTA_TASK_STACK, NULL, OTA_TASK_PRIO, &ota_task_handle);
    if (err != pdTRUE || ota_task_handle == NULL) {
        wm_log_error("ota_task creation failed.");
    }
    return 0;
}
