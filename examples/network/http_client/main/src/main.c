#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "wm_http_client.h"
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

#define HTTPC_CLIENT_BUFFER_SIZE     1024
#define HTTPC_CLIENT_DEFAULT_TIMEOUT 30000

static void wm_httpc_event_handle(wm_http_client_t session, wm_http_client_event_t event, wm_http_client_event_param_t *param,
                                  void *priv)
{
    switch (event) {
        case WM_HTTP_CLIENT_EVENT_CONNECTED:
            wm_log_info("WM_HTTP_CLIENT_EVENT_CONNECTED");
            break;
        case WM_HTTP_CLIENT_EVENT_DISCONNECTED:
            wm_log_info("WM_HTTP_CLIENT_EVENT_DISCONNECTED");
            break;
        case WM_HTTP_CLIENT_EVENT_HEADER_SENTED:
            wm_log_info("WM_HTTP_CLIENT_EVENT_HEADER_SENTED");
            wm_log_info("%s", (char *)(param->data));
            break;
        case WM_HTTP_CLIENT_EVENT_RECV_HEADER:
            wm_log_info("WM_HTTP_CLIENT_EVENT_RECV_HEADER");
            wm_log_info("%s", (char *)(param->data));
            break;
        case WM_HTTP_CLIENT_EVENT_RECV_DATA:
            wm_log_info("WM_HTTP_CLIENT_EVENT_RECV_DATA");
            wm_log_info("%s", (char *)(param->data));
            break;
        case WM_HTTP_CLIENT_EVENT_FINISH:
            wm_log_info("WM_HTTP_CLIENT_EVENT_FINISH");
            break;
    }
    return;
}

static int wm_httpc_example_send_req(wm_http_client_config_t *cfg, char *url, char *data /*post data*/, uint32_t len)
{
    int ret;
    size_t size, total = 0;
    char *buffer = NULL;
    wm_http_client_t session;

    do {
        buffer = (char *)malloc(HTTPC_CLIENT_BUFFER_SIZE);
        if (buffer == NULL) {
            return WM_ERR_NO_MEM;
        }
        memset(buffer, 0, HTTPC_CLIENT_BUFFER_SIZE);

        // Open the HTTP request handle
        session = wm_http_client_open_request(cfg);
        if (!session) {
            ret = WM_ERR_FAILED;
            break;
        }

        if ((ret = wm_http_client_send_request(session, url, data, len, true, 0, 0)) != WM_ERR_SUCCESS) {
            break;
        }

        // Retrieve the the headers and analyze them
        if ((ret = wm_http_client_recv_response(session, HTTPC_CLIENT_DEFAULT_TIMEOUT)) != WM_ERR_SUCCESS) {
            break;
        }
        wm_log_info("Start to receive data from remote server...");

        // Get the data until we get an error or end of stream code
        while (ret == WM_ERR_SUCCESS || ret != WM_ERR_EOS) {
            // Set the size of our buffer
            size = HTTPC_CLIENT_BUFFER_SIZE;
            // Get the data
            ret = wm_http_client_read_data(session, buffer, size, HTTPC_CLIENT_DEFAULT_TIMEOUT, &size);
            if (ret != WM_ERR_SUCCESS && ret != WM_ERR_EOS)
                break;
            total += size;
        }
    } while (0);

    free(buffer);

    if (session)
        ret = wm_http_client_close_request(&session);

    return ret;
}

int wm_example_httpc_get(void)
{
    wm_http_client_config_t cfg = { 0 };

    cfg.method        = WM_HTTP_CLIENT_REQUEST_TYPE_GET;
    cfg.event_handler = wm_httpc_event_handle;

    return wm_httpc_example_send_req(&cfg, "http://www.baidu.com/img/bdlogo.gif", NULL, 0);
}

int wm_example_httpc_set_keepalive(void)
{
    wm_http_client_config_t cfg = { 0 };

    cfg.method        = WM_HTTP_CLIENT_REQUEST_TYPE_GET;
    cfg.keep_alive    = 1;
    cfg.event_handler = wm_httpc_event_handle;

    return wm_httpc_example_send_req(&cfg, "https://www.qq.com", NULL, 0);
}

int wm_example_httpc_set_content_type(void)
{
    wm_http_client_config_t cfg = { 0 };

    cfg.method        = WM_HTTP_CLIENT_REQUEST_TYPE_GET;
    cfg.keep_alive    = 1;
    cfg.content_type  = WM_HTTP_CLIENT_CONTENT_APPLICATION_URLENCODED;
    cfg.event_handler = wm_httpc_event_handle;

    return wm_httpc_example_send_req(&cfg, "https://www.qq.com", NULL, 0);
}

int wm_example_httpc_post(void)
{
    wm_http_client_config_t cfg = { 0 };
    char *data                  = "{\"form\":{\"purpose\":\"test\"}}";
    cfg.method                  = WM_HTTP_CLIENT_REQUEST_TYPE_POST;
    cfg.event_handler           = wm_httpc_event_handle;
    return wm_httpc_example_send_req(&cfg, "http://httpbin.org/post", data, strlen(data));
}

int wm_example_httpc_put(void)
{
    wm_http_client_config_t cfg = { 0 };

    cfg.method        = WM_HTTP_CLIENT_REQUEST_TYPE_PUT;
    cfg.event_handler = wm_httpc_event_handle;

    return wm_httpc_example_send_req(&cfg, "http://httpbin.org/put?user=foo", NULL, 0);
}

static void httpc_request_task(void *pvParameters)
{
    wm_example_httpc_get();
    wm_example_httpc_set_keepalive();
    wm_example_httpc_set_content_type();
    wm_example_httpc_put();
    wm_example_httpc_post();
    wm_example_httpc_put();

    vTaskDelete(NULL);
}

static void wifi_init_sta(void)
{
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };
    int ret;

    ret = wm_nm_start_wifi_station(&cfg);
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
    xTaskCreate(httpc_request_task, "http_client_task", 1024, NULL, 5, NULL);
    return 0;
}
