#ifndef WM_HTTP2_CLIENT_INNER_H
#define WM_HTTP2_CLIENT_INNER_H

#include "wm_http2_client_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"

typedef struct wm_http2_ssl {
    mbedtls_ssl_context ssl;
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
} wm_http2_ssl_t;

typedef struct wm_http2_connection_info {
    char *host;
    uint16_t port;
    char *hostport;
    char *proxy_host;                 /**< HTTP proxy host name. */
    uint32_t use_proxy;               /**< Whether to use the HTTP proxy. */
    uint32_t proxy_port;              /**< HTTP proxy port name. */
    char *username;                   /**< User name of the HTTP session. */
    char *password;                   /**< Password of the HTTP session. */
    wm_http2_auth_schema_t auth_type; /**< HTTP Supported authentication methods. */
} wm_http2_connection_info_t;

typedef struct wm_http2_client_req {
    char *path; /**< path.*/
    /* Stream ID for this request. */
    int32_t stream_id;
    wm_http2_client_method_e method;
    char method_name[16];
    uint16_t http_header_num;
    nghttp2_nv http_header[WM_HTTP2_HEADER_MAX_NUM];

    //data provider
    uint8_t *content;     /**< for post / put method .*/
    uint32_t content_len; /**< post data len .*/
    uint32_t content_offset;
    nghttp2_data_provider data_provider;
    nghttp2_priority_spec pri_spec; /**< http prio .*/
    int16_t status;                 /**< http2 response status .*/
} wm_http2_client_req_t;

typedef struct wm_http2_client {
    wm_http2_connection_info_t conn_info;
    wm_http2_ssl_t *ssl;
    nghttp2_session *session;
    uint16_t http2_reqnum;
    wm_http2_client_req_t *http2_req;
    wm_http2_event_handle event_handler; /**< client event handler. */
    void *priv;                          /**< This is an optional temporary parameter. */
} wm_http2_client_t;

#ifdef __cplusplus
}
#endif

#endif /* WM_HTTP2_CLIENT_INNER_H */
