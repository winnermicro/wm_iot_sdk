#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "wm_nm_api.h"
#include "libwebsockets.h"
#include "lwip/apps/wm_sntp.h"
#include "lwip/apps/dns_server.h"
#include "cJSON/cJSON.h"

#define LOG_TAG "example_lws"
#include "wm_log.h"

/* Max receive buffer of the libwebsockets */
#define EXAMPLE_MAX_RECV_BUF_SIZE (16 * 1024)

/* Max timeout of connecting to WiFi station */
#define EXAMPLE_MAX_PRO_TIMEOUT   10000

/* http server port */
#define CONFIG_EXAMPLE_PORT       80

/* WiFi ssid and password max size */
#define CONFIG_MAX_SSID           (32 + 1)
#define CONFIG_MAX_PASSWORD       (64 + 1)

/* index.html file */
extern const uint8_t g_index[] asm("__binary_main_src_index_html");
extern const uint32_t g_index_len asm("__binary_main_src_index_html_length");

/* soft AP status */
static wm_nm_state_e s_softap_state = WM_NM_EXIT;

/* status to control exit http server loop */
static int g_done = 0;

/* WiFi ssid and password*/
static char g_ssid[CONFIG_MAX_SSID];
static char g_password[CONFIG_MAX_PASSWORD];

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

int start_softap(void)
{
    char *psd = CONFIG_EXAMPLE_SOFTAP_PASSWORD;

    wm_nm_wifi_softap_cfg_t cfg = { CONFIG_EXAMPLE_SOFTAP_SSID, (psd[0] ? psd : NULL) };

    wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);

    if (wm_nm_start_wifi_softap(&cfg) != WM_ERR_SUCCESS) {
        wm_event_remove_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);
        wm_log_error("NM Wi-Fi softap start failed");
        return WM_ERR_FAILED;
    }

    /* wait soft ap created ok*/
    while (s_softap_state != WM_NM_WIFI_AP_READY) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    return WM_ERR_SUCCESS;
}

/* connect to wifi station */
static int wifi_init_sta(wm_nm_wifi_station_cfg_t *cfg)
{
    int ret;
    int i;

    wm_log_info("Start to connect %s,%s", cfg->ssid, cfg->password);

    /* start connect*/
    ret = wm_nm_start_wifi_station(cfg);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wifi station start fail: %d", ret);
        g_done = 1;
        return WM_ERR_FAILED;
    }

    /* wait to get ip address*/
    for (i = 0; i < 20; i++) {
        vTaskDelay(pdMS_TO_TICKS(EXAMPLE_MAX_PRO_TIMEOUT / 20));
        if (wm_nm_get_wifi_station_state() == WM_NM_WIFI_STA_GOT_IP) {
            wm_log_info("wifi station start success, network is ready!");

            g_done = 1;
            return WM_ERR_SUCCESS;
        }
    }

    g_done = 1;

    wm_log_info("Connect wifi station fail!");

    return WM_ERR_FAILED;
}

static int check_is_html(char *uri, int len)
{
    char *dot = NULL;
    int slash = -1;
    int i;

    for (i = len - 1; i >= 0; i--) {
        if (uri[i] == '/') {
            slash = i;
            break;
        }
    }

    if (slash >= 0) {
        for (i = len - 1; i > slash; i--) {
            if (uri[i] == '.') {
                dot = &uri[i + 1];
                break;
            }
        }
    }

    if (!dot || !strncmp(dot, "html", 4)) {
        return 1;
    } else {
        return 0;
    }
}

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    int header_item_len;

    wm_log_debug("reason=%d", reason);

    switch (reason) {
        case LWS_CALLBACK_HTTP:
        {
            int n;
            unsigned char *p   = NULL;
            unsigned char *buf = NULL;
            int is_browser     = 0;
            int is_html        = 0;

            wm_log_debug("path=%.*s", len, (char *)in);

            /* check request method*/
            if (lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI)) {
                int size = 128 + g_index_len;

                wm_log_debug("GET");

                buf = malloc(LWS_PRE + size);
                if (!buf) {
                    return -1;
                }

                p = &buf[LWS_PRE];

                if ((header_item_len = lws_hdr_copy(wsi, (char *)p, size, WSI_TOKEN_HTTP_USER_AGENT)) > 0) {
                    wm_log_debug("User-Agent:%.*s", header_item_len, p);
                    if (strstr((char *)p, "Mozilla") || strstr((char *)p, "Mozilla") || strstr((char *)p, "Mozilla")) {
                        is_browser = 1;
                    }
                }

                wm_log_debug("is_browser=%d", is_browser);

                /* Is GET */
                lws_callback_on_writable(wsi);

                if (is_browser) {
                    is_html = check_is_html((char *)in, len);
                    wm_log_debug("is_html=%d", is_html);
                    if (is_html) {
                        /*html request, go to index.html*/
                        n = snprintf((char *)p, size,
                                     "HTTP/1.1 200 OK\r\n"
                                     "Content-Length: %u\r\n"
                                     "Content-Type: text/html; charset=UTF-8\r\n"
                                     "\r\n"
                                     "%s",
                                     g_index_len, g_index);
                        wm_log_debug("resp index");
                    } else {
                        char tmp[100];

                        int body_len = snprintf(tmp, sizeof(tmp),
                                                "File not found\n"
                                                "\n"
                                                "URI: %.*s\n"
                                                "Method: GET\n"
                                                "Arguments: 0\n",
                                                len, (char *)in);

                        /*other file, not found*/
                        n = snprintf((char *)p, size,
                                     "HTTP/1.1 404 Not Found\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Cache-Control: no-cache, no-store, must-revalidate\r\n"
                                     "Pragma: no-cache\r\n"
                                     "Expires: -1\r\n"
                                     "Content-Length: %u\r\n"
                                     "Connection: close\r\n"
                                     "\r\n"
                                     "%s",
                                     body_len, tmp);

                        wm_log_debug("resp 404");
                    }
                } else {
                    /*not bowser, go to index.html*/
                    n = snprintf((char *)p, size,
                                 "HTTP/1.1 302 Found\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Location: http://192.168.10.1\r\n"
                                 "Content-Length: 0\r\n"
                                 "Connection: close\r\n\r\n");
                    wm_log_debug("resp 302");
                }

                lws_write(wsi, (unsigned char *)p, n, LWS_WRITE_HTTP);

                free(buf);

            } else if (lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)) {
                /* Is POST */
                wm_log_debug("POST");
                lws_callback_on_writable(wsi);
            } else {
                wm_log_debug("OTHER");
                /* Neither GET nor POST*/
                lws_return_http_status(wsi, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
            }
            return 0;
        }

        case LWS_CALLBACK_HTTP_BODY:
        {
            /* Receive POST body*/
            cJSON *body = NULL;
            char buf[32];

            wm_log_debug("event LWS_CALLBACK_HTTP_BODY");

            if ((header_item_len = lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_HEAD_URI)) > 0) {
                wm_log_debug("uri:%.*s", header_item_len, buf);
            }

            if (!strncmp(buf, "/provsion", header_item_len)) {
                /*Pase request */
                if (len > 0 && in && (body = cJSON_ParseWithLength(in, len))) {
                    cJSON *ssid = cJSON_GetObjectItem(body, "wifiName");
                    cJSON *pswd = cJSON_GetObjectItem(body, "wifiPassword");
                    cJSON *cmd  = cJSON_GetObjectItem(body, "cmd");

                    wm_log_info("Received POST data: %.*s\n", (int)len, (char *)in);

                    if (cmd && cmd->type == cJSON_String) {
                        /* Save ssid and password*/
                        wm_log_debug("cmd=%s", cmd->valuestring);
                        if (!strcmp(cmd->valuestring, "prov") && ssid && ssid->type == cJSON_String && pswd &&
                            pswd->type == cJSON_String) {
                            snprintf(g_ssid, sizeof(g_ssid), "%s", ssid->valuestring);
                            snprintf(g_password, sizeof(g_password), "%s", pswd->valuestring);
                        }
                    }

                    cJSON_Delete(body);
                }
            } else {
                wm_log_error("post error");
            }
            return 0;
        }

        case LWS_CALLBACK_HTTP_BODY_COMPLETION:
        {
            /*receive http body end*/
            wm_nm_wifi_station_cfg_t cfg = { g_ssid, g_password };
            unsigned char buf[LWS_PRE + 256];
            unsigned char *p = &buf[LWS_PRE];
            int n;
            char body[64];
            int body_len;

            wm_log_debug("event LWS_CALLBACK_HTTP_BODY_COMPLETION");

            if ((header_item_len = lws_hdr_copy(wsi, body, sizeof(body), WSI_TOKEN_HEAD_URI)) > 0) {
                wm_log_debug("uri:%.*s", header_item_len, buf);
            }

            if (strncmp(body, "/provsion", header_item_len)) {
                /* Not WiFi provisioning Page Post*/
                return lws_callback_http_dummy(wsi, reason, user, in, len);
            }

            if (g_ssid[0]) {
                body_len = snprintf(body, sizeof(body), "{\"status\": \"%s\"}", "done");

                n = sprintf((char *)p,
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: application/json\r\n"
                            "Content-Length: %u\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "%s",
                            body_len, body);
                /* write response to http client*/
                lws_write(wsi, p, n, LWS_WRITE_HTTP);
                wm_log_debug("response=\n%s", body);

                /* start connect to WiFi station, the network will disconnect, Wait for a moment
                for the response data to be sent to the browser.*/
                vTaskDelay(pdMS_TO_TICKS(300));

                wifi_init_sta(&cfg);
            }

            return 0;
        }

        default:
            break;
    }

    return 0;
}

static struct lws_protocols protocols[] = {

    { "http-only", callback_http, 0, EXAMPLE_MAX_RECV_BUF_SIZE },
    LWS_PROTOCOL_LIST_TERM
};

static int http_server(void)
{
    struct lws_context_creation_info info = { 0 };
    struct lws_context *context;

    memset(&info, 0, sizeof(info));
    info.port      = CONFIG_EXAMPLE_PORT;
    info.protocols = protocols;

    wm_log_info("start httpd on port %d", info.port);

    /*  Create the WebSocket context */
    context = lws_create_context(&info);
    if (!context) {
        wm_log_error("lws context creation failed");
        return WM_ERR_FAILED;
    }

    /* Main loop to service the server */
    while (!g_done) {
        lws_service(context, 100);
    }

    /* Clean up */
    lws_context_destroy(context);

    wm_log_info("http server end");

    return WM_ERR_SUCCESS;
}

int main(void)
{
    wm_log_info("Start http server example");

    /* Start WiFi soft AP*/
    if (start_softap() != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    /* All domain names are resolved to local addresses */
    DNSS_Force_local(1);

    /* Start http server */
    http_server();

    /* Stop softAP */
    wm_nm_stop_wifi_softap();

    /* Restore domain name resolution */
    DNSS_Force_local(0);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    return WM_ERR_SUCCESS;
}
