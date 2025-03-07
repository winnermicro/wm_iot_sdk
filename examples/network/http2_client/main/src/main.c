#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wm_error.h"
#include "wm_event.h"
#include "wm_http2_client.h"

#include "wm_wifi.h"
#include "lwipopts.h"
#include "wm_wifi_types.h"
#include "wm_nm_api.h"

#define LOG_TAG "example"
#include "wm_log.h"

#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

void wm_http2_client_event_handle(int event, void *data, void *priv)
{
    wm_log_info("event %d \r\n", event);
    wm_http2_event_param_t *param = (wm_http2_event_param_t *)data;

    switch (event) {
        case WM_HTTP2_EVENT_CONNECTED:
            wm_log_info("HTTP2 CONNECT");
            break;
        case WM_HTTP2_EVENT_HEADER_SENTED:
            wm_log_info("HTTP2 HEADER SEDNTED:%d %s: %s", param->streamid, param->http2_data.header.name,
                        param->http2_data.header.value);
            break;
        case WM_HTTP2_EVENT_RECV_DATA:
            wm_log_info("%d %s", param->streamid, param->http2_data.data.data);
            break;
        case WM_HTTP2_EVENT_RECV_HEADER:
            wm_log_info("HTTP2 HEADER RECV:%d %s: %s", param->streamid, param->http2_data.header.name,
                        param->http2_data.header.value);
            break;
    }
}

static void http2_request_task(void *pvParameters)
{
    wm_http2_client_handle handle             = 0;
    wm_http2_client_config_t http2_client_cfg = { 0 };
    uint8_t content[]                         = "1234567890";

    http2_client_cfg.host    = CONFIG_EXAMPLE_DEFAULT_HOST;
    http2_client_cfg.port    = CONFIG_EXAMPLE_DEFAULT_PORT;
    http2_client_cfg.req_num = 5;
    http2_client_cfg.req     = (wm_http2_request_list_t *)malloc(http2_client_cfg.req_num * sizeof(wm_http2_request_list_t));
    if (http2_client_cfg.req) {
        memset(http2_client_cfg.req, 0x00, http2_client_cfg.req_num * sizeof(wm_http2_request_list_t));

        (http2_client_cfg.req + 0)->path        = "/";
        (http2_client_cfg.req + 0)->method      = WM_HTTP2_METHOD_GET;
        (http2_client_cfg.req + 1)->path        = "/";
        (http2_client_cfg.req + 1)->method      = WM_HTTP2_METHOD_GET;
        (http2_client_cfg.req + 1)->prio        = WM_HTTP2_PRIORITY_LEVEL5;
        (http2_client_cfg.req + 2)->path        = "/";
        (http2_client_cfg.req + 2)->method      = WM_HTTP2_METHOD_POST;
        (http2_client_cfg.req + 2)->content     = content;
        (http2_client_cfg.req + 2)->content_len = 10;
        (http2_client_cfg.req + 2)->prio        = WM_HTTP2_PRIORITY_LEVEL4;
        (http2_client_cfg.req + 3)->path        = "/";
        (http2_client_cfg.req + 3)->method      = WM_HTTP2_METHOD_GET;
        (http2_client_cfg.req + 4)->path        = "/";
        (http2_client_cfg.req + 4)->method      = WM_HTTP2_METHOD_GET;

        http2_client_cfg.event_handler = wm_http2_client_event_handle;
        handle                         = wm_http2_client_init(&http2_client_cfg);
        if (handle) {
            if (WM_ERR_SUCCESS == wm_http2_client_perform(handle)) {
                wm_log_info("HTTP status %d %d %d %d %d ", wm_http2_client_get_status_code(handle, 0),
                            wm_http2_client_get_status_code(handle, 1), wm_http2_client_get_status_code(handle, 2),
                            wm_http2_client_get_status_code(handle, 3), wm_http2_client_get_status_code(handle, 4));
            }

            wm_http2_client_cleanup(handle);
        }

        free(http2_client_cfg.req);
    }

    vTaskDelete(NULL);
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
    xTaskCreate(http2_request_task, "http2_request_task", 1024, NULL, 5, NULL);
    return 0;
}
