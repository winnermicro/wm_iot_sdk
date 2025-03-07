#ifndef WM_HTTP2_CLIENT_H
#define WM_HTTP2_CLIENT_H

#include "wm_http2_client_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WM_HTTP2_HEADER_MAX_NUM
#define WM_HTTP2_HEADER_MAX_NUM 20
#endif

#define MAKE_NV(NAME, VALUE)                                                                         \
    {                                                                                                \
        (uint8_t *)NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1, NGHTTP2_NV_FLAG_NONE \
    }

#define MAKE_NV_1(NAME, VALUE)                                                                   \
    {                                                                                            \
        (uint8_t *)NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, strlen(VALUE), NGHTTP2_NV_FLAG_NONE \
    }

#define MAKE_NV_2(NAME, VALUE)                                                               \
    {                                                                                        \
        (uint8_t *)NAME, (uint8_t *)VALUE, strlen(NAME), strlen(VALUE), NGHTTP2_NV_FLAG_NONE \
    }

typedef struct wm_https_url_param {
    const char *host;
    /* In this program, path contains query component as well. */
    const char *path;
    size_t pathlen;
    const char *hostport;
    size_t hostlen;
    size_t hostportlen;
    uint16_t port;
} wm_https_url_param_t;

typedef struct wm_http2_event_param {
    uint32_t session;  // seesion handle
    uint16_t streamid; // stream id
    union {
        struct {
            char *name;
            char *value;
        } header;
        struct {
            uint8_t *data;     // http recv/send data
            uint32_t data_len; // recv data/send len
        } data;
    } http2_data;
    void *user_data; //priv data.reserve
    int errcode;
} wm_http2_event_param_t;

typedef struct wm_http2_request_list {
    char *path;                      /**< Request path.path. must set*/
    wm_http2_client_method_e method; /**< Request type of the HTTP client. */
    char *user_agent;                /**< The User Agent string to send with HTTP requests. */
    char keep_alive;                 /**< Set the keep alive header. */
    char no_cache;                   /**< Set the no cache header. */
    //upload
    uint8_t *content;         /**< for post / put method .*/
    uint32_t content_len;     /**< post data len .*/
    char *content_type;       /**< content type .*/
    wm_http2_priority_e prio; /**< http prio .*/
} wm_http2_request_list_t;

typedef struct wm_http2_client_config {
    char *host;
    uint16_t port;
    char *proxy_host;                    /**< HTTP proxy host name. */
    uint32_t use_proxy;                  /**< Whether to use the HTTP proxy. */
    uint32_t proxy_port;                 /**< HTTP proxy port name. */
    char *username;                      /**< User name of the HTTP session. */
    char *password;                      /**< Password of the HTTP session. */
    wm_http2_auth_schema_t auth_type;    /**< HTTP Supported authentication methods. */
    uint16_t req_num;                    /**<HTTP req list num*/
    wm_http2_request_list_t *req;        /**<HTTP req list*/
    wm_http2_event_handle event_handler; /**< client event handler. */
    void *priv;                          /**< This is for user private data. */
} wm_http2_client_config_t;

wm_http2_client_handle wm_http2_client_init(wm_http2_client_config_t *http2_client_cfg);
WM_HTTP2_ERR wm_http2_client_perform(wm_http2_client_handle client_handle);
WM_HTTP2_ERR wm_http2_client_cleanup(wm_http2_client_handle client_handle);
WM_HTTP2_ERR wm_http2_client_add_header(wm_http2_client_handle client_handle, uint8_t idx, char *name, char *value);
WM_HTTP2_ERR wm_http2_client_delete_header(wm_http2_client_handle client_handle, uint8_t idx, char *name);
WM_HTTP2_ERR wm_http2_client_get_status_code(wm_http2_client_handle client_handle, uint8_t idx);
WM_HTTP2_ERR wm_http2_parse_uri(wm_https_url_param_t *res, const char *uri);

#ifdef __cplusplus
}
#endif

#endif /* WM_HTTP2_CLIENT_H */
