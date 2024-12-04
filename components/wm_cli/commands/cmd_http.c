#include "wmsdk_config.h"

#ifdef CONFIG_CLI_COMMANDS_HTTP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <assert.h>
#include "wm_cli.h"
#include "wm_netif.h"
#include "wm_http_client.h"

#define LOG_TAG "HTTPC_CLI"
#include "wm_log.h"

#define WM_HTTP_CLI_DEFAULT_RXSZIE  2048
#define WM_HTTP_CLI_DEFAULT_TIMEOUT 30000

#define HTTP_USAGE     \
    "httpc usage:\r\n" \
    "  Retrieve HTTP resources: httpc get [url]\r\n"

static void wm_httpc_event_handle(wm_http_client_t session, wm_http_client_event_t event, wm_http_client_event_param_t *param,
                                  void *priv)
{
    switch (event) {
        case WM_HTTP_CLIENT_EVENT_CONNECTED:
        {
            wm_log_debug("WM_HTTP_CLIENT_EVENT_CONNECTED");
            break;
        }
        case WM_HTTP_CLIENT_EVENT_DISCONNECTED:
        {
            wm_log_debug("WM_HTTP_CLIENT_EVENT_DISCONNECTED");
            break;
        }
        case WM_HTTP_CLIENT_EVENT_HEADER_SENTED:
        {
            wm_log_debug("WM_HTTP_CLIENT_EVENT_HEADER_SENTED");
            //wm_cli_printf("%s\r\n", (char *)(param->data));
            break;
        }
        case WM_HTTP_CLIENT_EVENT_RECV_HEADER:
        {
            wm_log_debug("WM_HTTP_CLIENT_EVENT_RECV_HEADER");
            //wm_cli_printf("%s\r\n", (char *)(param->data));
            break;
        }
        case WM_HTTP_CLIENT_EVENT_RECV_DATA:
        {
            wm_log_debug("WM_HTTP_CLIENT_EVENT_RECV_DATA");
            wm_cli_printf("%s\r\n", (char *)(param->data));
            break;
        }
        case WM_HTTP_CLIENT_EVENT_FINISH:
        {
            wm_log_debug("WM_HTTP_CLIENT_EVENT_FINISH");
            break;
        }
        default:
        {
            break;
        }
    }
    return;
}

static int wm_httpc_send_req(wm_http_client_config_t *cfg, char *url, char *data, uint32_t len)
{
    int ret;
    size_t size, total = 0;
    char *buffer = NULL;
    wm_http_client_t session;

    do {
        buffer = (char *)malloc(WM_HTTP_CLI_DEFAULT_RXSZIE);
        if (buffer == NULL) {
            wm_log_error("Out of memory");
            return WM_ERR_NO_MEM;
        }
        memset(buffer, 0, WM_HTTP_CLI_DEFAULT_RXSZIE);

        // Open the HTTP request handle
        session = wm_http_client_open_request(cfg);
        if (!session) {
            ret = WM_ERR_FAILED;
            wm_log_error("Create session failed");
            break;
        }

        if ((ret = wm_http_client_send_request(session, url, data, len, true, 0, 0)) != WM_ERR_SUCCESS) {
            wm_log_error("Send request failed");
            break;
        }

        // Retrieve the the headers and analyze them
        if ((ret = wm_http_client_recv_response(session, WM_HTTP_CLI_DEFAULT_TIMEOUT)) != WM_ERR_SUCCESS) {
            wm_log_error("Recv response failed");
            break;
        }
        wm_log_debug("Start to receive data from remote server...");

        // Get the data until we get an error or end of stream code
        while (ret == WM_ERR_SUCCESS || ret != WM_ERR_EOS) {
            // Set the size of our buffer
            size = WM_HTTP_CLI_DEFAULT_RXSZIE;

            // Get the data
            ret = wm_http_client_read_data(session, buffer, size, WM_HTTP_CLI_DEFAULT_TIMEOUT, &size);
            if (ret != WM_ERR_SUCCESS && ret != WM_ERR_EOS) {
                break;
            }
            total += size;
        }
    } while (0);

    if (buffer) {
        free(buffer);
    }

    if (session) {
        ret = wm_http_client_close_request(&session);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Close session failed");
        }
    }

    return ret;
}

/**
 * @brief HTTP client command fuction
*/
static int http_client_process(int argc, char *argv[])
{
    if (argc < 1) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!strcmp(argv[0], "get") && argc == 2) {
        wm_http_client_config_t cfg = {
            .method        = WM_HTTP_CLIENT_REQUEST_TYPE_GET,
            .keep_alive    = 1,
            .event_handler = wm_httpc_event_handle,
        };

        return wm_httpc_send_req(&cfg, argv[1], NULL, 0);
    }

    return WM_ERR_INVALID_PARAM;
}

/**
 * @brief HTTP command fuction
*/
static void cmd_http(int argc, char *argv[])
{
    int ret = WM_ERR_INVALID_PARAM;

    if (argc < 2) {
        goto usage;
    }

    ret = http_client_process(argc - 1, &argv[1]);

    if (ret != WM_ERR_SUCCESS) {
        goto usage;
    }

    wm_log_debug("HTTP execution succeeded");
    return;

usage:
    if (ret == WM_ERR_INVALID_PARAM) {
        wm_cli_printf(HTTP_USAGE);
    } else {
        wm_cli_printf("HTTP command error: %d\r\n", ret);
    }
    return;
}

WM_CLI_CMD_DEFINE(httpc, cmd_http, httpc cmd, httpc<args...> --operate HTTP);
#endif //CLI_COMMANDS_HTTP
