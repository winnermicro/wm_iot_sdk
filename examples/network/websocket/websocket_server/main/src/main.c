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

#define LOG_TAG "example_lws"
#include "wm_log.h"

#define EXAMPLE_MAX_RECV_BUF_SIZE (8 * 1024)

#ifdef CONFIG_EXAMPLE_SECURITY
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

/* process websocket event */
static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            /* Connected from client */
            wm_log_info("Client connected\n");
            break;

        case LWS_CALLBACK_RECEIVE:
            /* received data from client */
            wm_log_info("Received msg from client: \n%.*s", (int)len, (char *)in);
            lws_write(wsi, in, len, LWS_WRITE_TEXT);
            wm_log_info("Send msg to client: \n%.*s", (int)len, (char *)in);
            break;

        case LWS_CALLBACK_CLOSED:
            /* closed by client */
            wm_log_info("Client disconnected\n");
            break;

        default:
            break;
    }

    return 0;
}

static struct lws_protocols protocols[] = {
    {
     "echo-protocol", callback_websocket,
     0, EXAMPLE_MAX_RECV_BUF_SIZE,
     },
    LWS_PROTOCOL_LIST_TERM
};

static int websocket_server(void)
{
    struct lws_context_creation_info info = { 0 };
    struct lws_context *context;
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

    /* Set libwebsockets log level*/
    lws_set_log_level(logs, NULL);

    memset(&info, 0, sizeof(info));
    info.port      = CONFIG_EXAMPLE_PORT;
    info.protocols = protocols;
    info.gid       = -1;
    info.uid       = -1;

#ifdef CONFIG_EXAMPLE_SECURITY
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    info.server_ssl_cert_mem            = (unsigned char *)g_cert_mem;
    info.server_ssl_cert_mem_len        = strlen(g_cert_mem);
    info.server_ssl_private_key_mem     = (unsigned char *)g_key_mem;
    info.server_ssl_private_key_mem_len = strlen(g_key_mem);
#endif

    /* Create the WebSocket context */
    context = lws_create_context(&info);
    if (!context) {
        wm_log_error("lws context creation failed");
        return WM_ERR_FAILED;
    }

    wm_log_info("The websocket server listened on port %d", info.port);

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

    /* Start connect to Wi-Fi station */
    wifi_init_sta();

    /* Start WebSocket server */
    websocket_server();

    return WM_ERR_SUCCESS;
}
