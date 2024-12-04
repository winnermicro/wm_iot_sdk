#include "wm_ota_http.h"
#include "wm_http_client.h"
#include "wm_osal.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "wm_component.h"

#define LOG_TAG "ota_http"
#include "wm_log.h"

#define HTTPC_CLIENT_DEFAULT_TIMEOUT 30000

typedef struct {
    wm_ota_ctx_t *p_ota_ctx;           /**< Pointer to the OTA context*/
    wm_os_task_t ota_http_task_handle; /**< OTA HTTP task handle */
    wm_http_client_t http_session;     /**< HTTP session identifier */
    wm_ota_http_get_file_callback_t ota_get_file_cb;
    char *p_http_url;       /**< Pointer to the HTTP URL address */
    bool is_ota_running;    /**< Flag indicating if OTA is running */
    bool is_wifi_connected; /**< Flag indicating if WiFi is connected */
    bool is_got_ip;         /**< Flag indicating if IP address is obtained */
} ota_http_ctx_t;

static ota_http_ctx_t g_ota_http_ctx = { 0 };

static void wm_httpc_event_handle(wm_http_client_t session, wm_http_client_event_t event, wm_http_client_event_param_t *param,
                                  void *priv)
{
    switch (event) {
        case WM_HTTP_CLIENT_EVENT_CONNECTED:
            wm_log_debug("WM_HTTP_CLIENT_EVENT_CONNECTED");
            break;
        case WM_HTTP_CLIENT_EVENT_DISCONNECTED:
            wm_log_debug("WM_HTTP_CLIENT_EVENT_DISCONNECTED");
            break;
        case WM_HTTP_CLIENT_EVENT_HEADER_SENTED:
            wm_log_debug("WM_HTTP_CLIENT_EVENT_HEADER_SENTED");
            // wm_log_debug("%s", (char *)(param->data));
            break;
        case WM_HTTP_CLIENT_EVENT_RECV_HEADER:
            wm_log_debug("WM_HTTP_CLIENT_EVENT_RECV_HEADER");
            // wm_log_debug("%s", (char *)(param->data));
            break;
        case WM_HTTP_CLIENT_EVENT_RECV_DATA:
            // wm_log_debug("WM_HTTP_CLIENT_EVENT_RECV_DATA");
            // wm_log_debug("%s", (char *)(param->data));
            break;
        case WM_HTTP_CLIENT_EVENT_FINISH:
            wm_log_debug("WM_HTTP_CLIENT_EVENT_FINISH");
            break;
    }
    return;
}

static int wm_ota_http_connect(void **handle, uint32_t offset)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_http_client_config_t http_cfg = { 0 };

    if (handle == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    http_cfg.method        = WM_HTTP_CLIENT_REQUEST_TYPE_GET;
    http_cfg.event_handler = wm_httpc_event_handle;

    char range_header[64] = { 0 };
    snprintf(range_header, sizeof(range_header), "bytes=%d-", offset);

    do {
        // Open the HTTP request handle
        g_ota_http_ctx.http_session = wm_http_client_open_request(&http_cfg);
        if (!g_ota_http_ctx.http_session) {
            ret = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        ret = wm_http_client_add_request_headers(g_ota_http_ctx.http_session, "Range", range_header);
        if (ret != HTTP_CLIENT_SUCCESS) {
            break;
        }

        ret = wm_http_client_send_request(g_ota_http_ctx.http_session, g_ota_http_ctx.p_http_url, NULL, 0, true, 0, 0);
        if (ret != HTTP_CLIENT_SUCCESS) {
            break;
        }

        // Retrieve the the headers and analyze them
        ret = wm_http_client_recv_response(g_ota_http_ctx.http_session, HTTPC_CLIENT_DEFAULT_TIMEOUT);
        if (ret != HTTP_CLIENT_SUCCESS) {
            break;
        }
    } while (0);

    if (ret != WM_ERR_SUCCESS) {
        wm_http_client_close_request(&g_ota_http_ctx.http_session);
    } else {
        *handle = (void *)g_ota_http_ctx.http_session;
    }

    return ret;
}

static int wm_ota_http_get_firmware(void *handle, uint32_t offset, uint8_t *fw_buf, uint32_t buf_size, uint32_t *got)
{
    int ret             = WM_ERR_SUCCESS;
    uint32_t start_time = wm_os_internal_get_time_ms();

    if (handle == NULL || fw_buf == NULL || buf_size <= 0 || got == NULL ||
        (wm_http_client_t)handle != g_ota_http_ctx.http_session) {
        return WM_ERR_INVALID_PARAM;
    }

    do {
        ret = wm_http_client_read_data((wm_http_client_t)handle, fw_buf, buf_size + 1, HTTPC_CLIENT_DEFAULT_TIMEOUT, got);
        if (ret == HTTP_CLIENT_ERROR_SOCKET_TIME_OUT ||
            (wm_os_internal_get_time_ms() - start_time) >= HTTPC_CLIENT_DEFAULT_TIMEOUT) {
            return WM_ERR_OTA_TIMEOUT;
        }
        if (!g_ota_http_ctx.is_wifi_connected || !g_ota_http_ctx.is_got_ip) {
            return WM_ERR_OTA_NO_GOT_IP;
        }
    } while (0);

    if (ret == WM_ERR_EOS) {
        ret = WM_ERR_SUCCESS;
    }

    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("ota get firmware failed: %d (%d-%d)", ret, buf_size, *got);
    }

    if (g_ota_http_ctx.ota_get_file_cb) {
        g_ota_http_ctx.ota_get_file_cb(fw_buf, *got);
    }

    return ret;
}

static int wm_ota_http_close(void *handle)
{
    int ret = WM_ERR_SUCCESS;

    if (handle == NULL || (wm_http_client_t)handle != g_ota_http_ctx.http_session) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_http_client_close_request((wm_http_client_t *)&handle);

    return ret;
}

static void ota_http_task(void *arg)
{
    int ret     = 0;
    bool reboot = (bool)arg;

    ret = wm_ota_start(g_ota_http_ctx.p_ota_ctx, reboot);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("ota start failed: %d", ret);
    }
    wm_ota_deinit(g_ota_http_ctx.p_ota_ctx);

    wm_os_internal_free(g_ota_http_ctx.p_http_url);
    g_ota_http_ctx.p_http_url = NULL;

    wm_os_internal_free(g_ota_http_ctx.p_ota_ctx);
    g_ota_http_ctx.p_ota_ctx = NULL;

    g_ota_http_ctx.is_ota_running = false;

    wm_log_debug("ota_http task exit");
    if (g_ota_http_ctx.ota_http_task_handle) {
        wm_os_internal_task_del(g_ota_http_ctx.ota_http_task_handle);
    }
}
int wm_ota_http_update(wm_ota_http_cfg_t *cfg)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_os_status_t err           = WM_OS_STATUS_SUCCESS;
    uint32_t url_len             = 0;
    wm_ota_session_t ota_session = {
        .ota_connect_cb      = wm_ota_http_connect,
        .ota_get_firmware_cb = wm_ota_http_get_firmware,
        .ota_close_cb        = wm_ota_http_close,
    };

    if (cfg == NULL || cfg->fw_url == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (g_ota_http_ctx.is_ota_running || g_ota_http_ctx.p_ota_ctx != NULL || g_ota_http_ctx.p_http_url != NULL) {
        return WM_ERR_OTA_ALREADY_RUNNING;
    }

    if (!g_ota_http_ctx.is_wifi_connected || !g_ota_http_ctx.is_got_ip) {
        return WM_ERR_OTA_NO_GOT_IP;
    }

    g_ota_http_ctx.p_ota_ctx = wm_os_internal_malloc(sizeof(wm_ota_ctx_t));
    if (g_ota_http_ctx.p_ota_ctx != NULL) {
        memset(g_ota_http_ctx.p_ota_ctx, 0, sizeof(wm_ota_ctx_t));
        g_ota_http_ctx.ota_get_file_cb         = cfg->ota_get_file_cb;
        g_ota_http_ctx.p_ota_ctx->ota_session  = ota_session;
        g_ota_http_ctx.p_ota_ctx->ota_state_cb = cfg->ota_state_cb;
    } else {
        ret = WM_ERR_NO_MEM;
        goto exit;
    }

    url_len                   = strlen(cfg->fw_url);
    g_ota_http_ctx.p_http_url = wm_os_internal_malloc(url_len + 1);
    if (g_ota_http_ctx.p_http_url != NULL) {
        memset(g_ota_http_ctx.p_http_url, 0, url_len + 1);
        strncpy(g_ota_http_ctx.p_http_url, cfg->fw_url, url_len);
    } else {
        ret = WM_ERR_NO_MEM;
        goto exit;
    }

    ret = wm_ota_init(g_ota_http_ctx.p_ota_ctx);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("ota init failed: %d", ret);
        goto exit;
    }

    err = wm_os_internal_task_create(&g_ota_http_ctx.ota_http_task_handle, "ota_http", ota_http_task, (void *)cfg->reboot,
                                     WM_TASK_OTA_HTTP_STACK, WM_TASK_OTA_HTTP_PRIO, 0);
    if (err != WM_OS_STATUS_SUCCESS || g_ota_http_ctx.ota_http_task_handle == NULL) {
        ret = (int)err;
        goto exit;
    }
    g_ota_http_ctx.is_ota_running = true;

exit:
    if (ret != WM_ERR_SUCCESS) {
        wm_ota_deinit(g_ota_http_ctx.p_ota_ctx);

        wm_os_internal_free(g_ota_http_ctx.p_http_url);
        g_ota_http_ctx.p_http_url = NULL;

        wm_os_internal_free(g_ota_http_ctx.p_ota_ctx);
        g_ota_http_ctx.p_ota_ctx = NULL;
    }

    return ret;
}

static void event_handler_sta_disconnected(wm_event_group_t group, int event, void *data, void *priv)
{
    if (WM_EVENT_WIFI_STA_CONNECTED == event) {
        g_ota_http_ctx.is_wifi_connected = true;
    } else if (WM_EVENT_WIFI_STA_DISCONNECTED == event) {
        g_ota_http_ctx.is_wifi_connected = false;
    }
}

static void event_handler_sta_got_ip(wm_event_group_t group, int event, wm_lwip_event_data_t *data, void *priv)
{
    if (WM_EVENT_WIFI_STA_GOT_IP == event) {
        g_ota_http_ctx.is_got_ip = true;
    } else if (WM_EVENT_WIFI_STA_LOST_IP == event) {
        g_ota_http_ctx.is_got_ip = false;
    }
}

int wm_ota_http_init(void)
{
    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
    wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);

    return WM_ERR_SUCCESS;
}
WM_COMPONEN_INIT_3(wm_ota_http_init);
