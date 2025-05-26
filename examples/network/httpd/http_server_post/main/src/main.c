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

#define LOG_TAG "example_lws"
#include "wm_log.h"

#define EXAMPLE_MAX_RECV_BUF_SIZE (16 * 1024)

#if CONFIG_EXAMPLE_HTTPS

/*
generate a cert which signed by itself:
    1 Generate ECDSA private key:
        openssl ecparam -genkey -name prime256v1 -out server.key
    2 Generate Certificate Signing Request (CSR):
        openssl req -new -key server.key -out server.csr
    3 Generate self signed certificate (99 Years):
        openssl x509 -req -in server.csr -signkey server.key -out server.crt -days 36135
*/

const static char *g_cert_mem = "-----BEGIN CERTIFICATE-----\n"
                                "MIICCDCCAa+gAwIBAgIUZkIs9JvPaktilBlCCuasY913FPYwCgYIKoZIzj0EAwIw\n"
                                "cjELMAkGA1UEBhMCY24xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0JlaWpp\n"
                                "bmcxFDASBgNVBAoMC1dpbm5lck1hY3JvMSkwJwYJKoZIhvcNAQkBFhptaWtlLmNo\n"
                                "YW5nQHdpbm5lcm1pY3JvLmNvbTAgFw0yNTAyMDUwNjQyMTZaGA8yMTI0MDExMzA2\n"
                                "NDIxNlowcjELMAkGA1UEBhMCY24xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcM\n"
                                "B0JlaWppbmcxFDASBgNVBAoMC1dpbm5lck1hY3JvMSkwJwYJKoZIhvcNAQkBFhpt\n"
                                "aWtlLmNoYW5nQHdpbm5lcm1pY3JvLmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEH\n"
                                "A0IABPiCILpswD96G29fz3znhwPTXIuk3G071VrcMmNS1FHvMyaJVzftasCCvMxm\n"
                                "rV3sJkSFx38XOmt3nQf6fHgLBzyjITAfMB0GA1UdDgQWBBSF/AHUdNfcxVg+SEK5\n"
                                "IvlPHAi+5TAKBggqhkjOPQQDAgNHADBEAiBnXICtL2ulwcm7nZtlqROSEE0F0fFv\n"
                                "3dt6HLkTwolfIQIgI5U7ZOtquZBqOiR0ZG9D+/WveJNao0XKKWMjv5iKtMM=\n"
                                "-----END CERTIFICATE-----";

const static char *g_key_mem = "-----BEGIN EC PRIVATE KEY-----\n"
                               "MHcCAQEEIEAUw//nDgf3hCSZWNefq4wWl1wvB1mvTJUBVKE9cthxoAoGCCqGSM49\n"
                               "AwEHoUQDQgAE+IIgumzAP3obb1/PfOeHA9Nci6TcbTvVWtwyY1LUUe8zJolXN+1q\n"
                               "wIK8zGatXewmRIXHfxc6a3edB/p8eAsHPA==\n"
                               "-----END EC PRIVATE KEY-----";

#endif

/* connect to wifi station */
static void wifi_init_sta(void)
{
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };
    int ret;

    /* start connect*/
    ret = wm_nm_start_wifi_station(&cfg);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wifi station start fail: %d", ret);
        return;
    }

    /* wait get ip address*/
    while (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    wm_log_info("wifi station start success, network is ready!");
}

/* process http event */
static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason) {
        case LWS_CALLBACK_HTTP:
        {
            /*receive http header*/
            wm_log_info("LWS_CALLBACK_HTTP");
            wm_log_info("path:%s", (char *)in);
            return 0;
        }

        case LWS_CALLBACK_HTTP_BODY:
        {
            /*receive http body*/
            wm_log_info("LWS_CALLBACK_HTTP_BODY");
            if (in) {
                wm_log_info("recv body=\n%.*s", len, (char *)in);
                return 0;
            }
            break;
        }

        case LWS_CALLBACK_HTTP_BODY_COMPLETION:
        {
            /*receive http body end*/
            const char *response_json = "{\"status\": \"success\", \"message\": \"Received your request.\"}";
            unsigned char buf[LWS_PRE + 256];
            unsigned char *p = &buf[LWS_PRE];
            int n;

            wm_log_info("LWS_CALLBACK_HTTP_BODY_COMPLETION");

            n = sprintf((char *)p,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: %u\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "%s",
                        strlen(response_json), response_json);
            /*write response to http client*/
            lws_write(wsi, p, n, LWS_WRITE_HTTP);
            wm_log_info("response=\n%s", response_json);

            break;
        }

        default:
            break;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static struct lws_protocols protocols[] = {

    { "test", callback_http, 0, EXAMPLE_MAX_RECV_BUF_SIZE },
    LWS_PROTOCOL_LIST_TERM
};

static int http_server(void)
{
    struct lws_context_creation_info info = { 0 };
    struct lws_context *context;
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

    /* set libwebsockets log level*/
    lws_set_log_level(logs, NULL);

    memset(&info, 0, sizeof(info));
    info.port      = CONFIG_EXAMPLE_PORT;
    info.protocols = protocols;
    info.gid       = -1;
    info.uid       = -1;

#if CONFIG_EXAMPLE_HTTPS
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    info.server_ssl_cert_mem            = (unsigned char *)g_cert_mem;
    info.server_ssl_cert_mem_len        = strlen(g_cert_mem);
    info.server_ssl_private_key_mem     = (unsigned char *)g_key_mem;
    info.server_ssl_private_key_mem_len = strlen(g_key_mem);
#endif

    wm_log_info("start httpd on port %d", info.port);

    /*  Create the WebSocket context */
    context = lws_create_context(&info);
    if (!context) {
        wm_log_error("lws context creation failed");
        return WM_ERR_FAILED;
    }

    /* Main loop to service the server */
    while (1) {
        lws_service(context, 100);
    }

    /* Clean up */
    lws_context_destroy(context);

    return WM_ERR_SUCCESS;
}

int main(void)
{
    wm_log_info("Start http server example");

    /* Start connect WiFi */
    wifi_init_sta();

    /* Set time zone and start NTP time synchronization */
    setenv("TZ", "UTC+8", 1);
    wm_sntp_start();

    /* wait time Synchronized */
    vTaskDelay(pdMS_TO_TICKS(500));

    /* Start http server */
    http_server();

    return WM_ERR_SUCCESS;
}
