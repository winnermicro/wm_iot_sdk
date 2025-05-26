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
#include "mbedtls/ssl.h"

#include "wm_nm_api.h"
#include "libwebsockets.h"

#define LOG_TAG "example_lws"
#include "wm_log.h"

/* Allow the server to use self signed certificates.
Only for test, Please do not turn on this configuration for product project. */
#define EXAMPLE_ALLOW_SELF_SIGNED 1

#define EXAMPLE_MAX_RECV_BUF_SIZE (4 * 1024)
#define EXAMPLE_MAX_SEND_PKT_SIZE (4 * 1024)

#define EXAMPLE_TASK_STACK_SIZE   (2048)

/* CA root certificate*/
const static char *g_ca_root_cert = "-----BEGIN CERTIFICATE-----\n"
                                    "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
                                    "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
                                    "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
                                    "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
                                    "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
                                    "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
                                    "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
                                    "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
                                    "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
                                    "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
                                    "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
                                    "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
                                    "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
                                    "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
                                    "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
                                    "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
                                    "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
                                    "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
                                    "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
                                    "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
                                    "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
                                    "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
                                    "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
                                    "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
                                    "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
                                    "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
                                    "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
                                    "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
                                    "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
                                    "-----END CERTIFICATE-----";

/* Use for exit loop */
static int g_done = 0;

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

int parse_url(const char *url, char *protocol, char *host, int *port, char *path)
{
    if (url == NULL || protocol == NULL || host == NULL || port == NULL || path == NULL) {
        return -1; // invalid parameters
    }

    const char *ptr = url;

    /* get protocol */
    const char *colon = strstr(ptr, "://");
    if (colon) {
        strncpy(protocol, ptr, colon - ptr);
        protocol[colon - ptr] = '\0';
        ptr                   = colon + 3;
    } else {
        strcpy(protocol, "");
        return -1; // bad protocol
    }

    /* get host, port, path */
    const char *slash = strchr(ptr, '/');
    if (*ptr == '[') { // check IPv6 address
        const char *bracket_end = strchr(ptr, ']');
        if (!bracket_end) {
            return -1; // invalid IPv6 address
        }
        strncpy(host, ptr + 1, bracket_end - ptr - 1);
        host[bracket_end - ptr - 1] = '\0';
        ptr                         = bracket_end + 1;

        // check port
        if (*ptr == ':') {
            ptr++;
            if (slash) {
                if (sscanf(ptr, "%d%n", port, (int *)&ptr) != 1) {
                    return -1; // parse port error
                }
                strcpy(path, slash);
            } else {
                if (sscanf(ptr, "%d", port) != 1) {
                    return -1; // parse port error
                }
                strcpy(path, "/");
            }
        } else {
            *port = (strcmp(protocol, "ws") == 0) ? 80 : ((strcmp(protocol, "wss") == 0) ? 443 : 0);
            if (slash) {
                strcpy(path, slash);
            } else {
                strcpy(path, "/");
            }
        }
    } else {
        const char *colon_port = strchr(ptr, ':');
        if (colon_port && (!slash || colon_port < slash)) {
            strncpy(host, ptr, colon_port - ptr);
            host[colon_port - ptr] = '\0';
            ptr                    = colon_port + 1;
            if (slash) {
                if (sscanf(ptr, "%d%n", port, (int *)&ptr) != 1) {
                    return -1; // parse port error
                }
                strcpy(path, slash);
            } else {
                if (sscanf(ptr, "%d", port) != 1) {
                    return -1; // parse port error
                }
                strcpy(path, "/");
            }
        } else if (slash) {
            strncpy(host, ptr, slash - ptr);
            host[slash - ptr] = '\0';
            *port             = (strcmp(protocol, "ws") == 0) ? 80 : ((strcmp(protocol, "wss") == 0) ? 443 : 0);
            strcpy(path, slash);
        } else {
            strcpy(host, ptr);
            *port = (strcmp(protocol, "ws") == 0) ? 80 : ((strcmp(protocol, "wss") == 0) ? 443 : 0);
            strcpy(path, "/");
        }
    }

    return 0;
}

/*
The sequence of events is as follows:

LWS_CALLBACK_PROTOCOL_INIT=27
LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL=85
LWS_CALLBACK_CONNECTING=105
LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED=19
LWS_CALLBACK_GET_THREAD_ID=31
LWS_CALLBACK_WSI_CREATE=29
LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER=24
LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP=44
LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH=2
LWS_CALLBACK_CLIENT_ESTABLISHED=3

LWS_CALLBACK_CLIENT_RECEIVE=8

LWS_CALLBACK_WS_PEER_INITIATED_CLOSE=38
LWS_CALLBACK_CLIENT_CLOSED=75
LWS_CALLBACK_WSI_DESTROY=30
LWS_CALLBACK_PROTOCOL_DESTROY=28
*/
static int client_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    wm_log_debug("reason=%d", reason);

    switch (reason) {
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        {
            wm_log_error("ev=CLIENT_CONNECTION_ERROR: %s", in ? (char *)in : "(null)");
            g_done = 1;
            break;
        }
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
        {
            /* attach to websocket server ok */
            const char *msg    = "Hello, server!";
            unsigned char *buf = NULL;
            unsigned char *p   = NULL;

            wm_log_info("ev=LWS_CALLBACK_CLIENT_ESTABLISHED");
            wm_log_info("send message: \n%s", (char *)msg);

            buf = malloc(LWS_PRE + 1024);
            if (!buf) {
                return -1;
            }

            p = &buf[LWS_PRE];

            memcpy(p, msg, strlen(msg));
            /* send msg to websocket server */
            if (lws_write(wsi, p, strlen(msg), LWS_WRITE_TEXT) < 0) {
                wm_log_error("Failed to send data to server");
            }
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE:
        {
            /* receive msg from websocket server */
            wm_log_info("ev=LWS_CALLBACK_RECEIVE");
            wm_log_info("Received message: \n%s", (char *)in);

            break;
        }
        case LWS_CALLBACK_CLIENT_CLOSED:
        {
            wm_log_info("ev=LWS_CALLBACK_CLIENT_CLOSED");
            g_done = 1;
            break;
        }
        default:
            break;
    }
    return 0;
}

static struct lws_protocols g_protocols[] = {
    {
     .name           = "echo-protocol",
     .callback       = client_callback,
     .rx_buffer_size = EXAMPLE_MAX_RECV_BUF_SIZE,
     .tx_packet_size = EXAMPLE_MAX_SEND_PKT_SIZE,
     },
    LWS_PROTOCOL_LIST_TERM
};

static int websocke_client(void)
{
    char url_protocol[8]  = "";
    char url_host[64]     = "";
    char url_path[64]     = "";
    int url_port          = 0;
    int is_ssl_connection = 0;

    struct lws_context_creation_info info;
    struct lws_client_connect_info ccinfo;
    struct lws_context *context = NULL;
    struct lws *lws             = NULL;

    /*parse url*/
    if (parse_url(CONFIG_EXAMPLE_SERVER_URL, url_protocol, url_host, &url_port, url_path)) {
        wm_log_error("parse url error: %s", CONFIG_EXAMPLE_SERVER_URL);
        return WM_ERR_FAILED;
    }

    wm_log_info("protocol=%s,host=%s,port=%d,path=%s", url_protocol, url_host, url_port, url_path);

    if (!strcmp(url_protocol, "ws")) {
        is_ssl_connection = 0;
    } else if (!strcmp(url_protocol, "wss")) {
        is_ssl_connection = 1;
    } else {
        wm_log_error("bad url protocol");
        return WM_ERR_INVALID_PARAM;
    }

    /* set config for context*/
    memset(&info, 0, sizeof info);
    info.port      = CONTEXT_PORT_NO_LISTEN;
    info.protocols = g_protocols;
    info.options   = (is_ssl_connection ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0);
    info.gid       = -1;
    info.uid       = -1;

    if (is_ssl_connection && !EXAMPLE_ALLOW_SELF_SIGNED) {
        /*set CA certificate*/
        info.client_ssl_ca_mem     = g_ca_root_cert;
        info.client_ssl_ca_mem_len = strlen(g_ca_root_cert) + 1;
    }

    /* create lws context*/
    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return WM_ERR_FAILED;
    }

    /* config connect info */
    memset(&ccinfo, 0, sizeof ccinfo);
    ccinfo.context        = context;
    ccinfo.address        = url_host;
    ccinfo.port           = url_port;
    ccinfo.path           = url_path;
    ccinfo.host           = ccinfo.address;
    ccinfo.origin         = ccinfo.address;
    ccinfo.protocol       = g_protocols[0].name;
    ccinfo.ssl_connection = (is_ssl_connection ? LCCSCF_USE_SSL : 0);

    if (is_ssl_connection && EXAMPLE_ALLOW_SELF_SIGNED) {
        ccinfo.ssl_connection |= (LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK);
    }

    /* connect to remote websocket server */
    lws = lws_client_connect_via_info(&ccinfo);
    if (lws) {
        /* poll */
        while (!g_done) {
            lws_service(context, 100);
        }
    } else {
        wm_log_error("connect fail");
    }

    /* destory lws */
    lws_context_destroy(context);

    wm_log_info("end!");

    return WM_ERR_SUCCESS;
}

static void ws_client_task(void *parameters)
{
    /*start connect WiFi station*/
    wifi_init_sta();

    /*start websocket client */
    websocke_client();

    vTaskDelete(NULL);
}

int main(void)
{
    wm_log_info("Start websocket client example");

    xTaskCreate(ws_client_task, "ws_client", EXAMPLE_TASK_STACK_SIZE, NULL, 5, NULL);

    return WM_ERR_SUCCESS;
}
