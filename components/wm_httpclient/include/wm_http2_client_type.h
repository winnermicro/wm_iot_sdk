#ifndef WM_HTTP2_CLIENT_TYPE_H
#define WM_HTTP2_CLIENT_TYPE_H

#include "nghttp2_session.h"
#include "nghttp2/nghttp2.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WM_HTTP2_REQ_MAX_NUM
#define WM_HTTP2_REQ_MAX_NUM 5
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

typedef int32_t WM_HTTP2_ERR;
typedef uint32_t wm_http2_client_handle;

typedef void (*wm_http2_event_handle)(int event, void *data, void *priv);

typedef enum wm_http2_event {
    WM_HTTP2_EVENT_ERR,
    WM_HTTP2_EVENT_CONNECTED,
    WM_HTTP2_EVENT_DISCONNECTED,
    WM_HTTP2_EVENT_HEADER_SENTED,
    WM_HTTP2_EVENT_RECV_HEADER,
    WM_HTTP2_EVENT_RECV_DATA,
    WM_HTTP2_EVENT_FINISH,
} wm_http2_event_e;

typedef enum wm_http2_priority {
    WM_HTTP2_PRIORITY_LEVEL1,
    WM_HTTP2_PRIORITY_LEVEL2,
    WM_HTTP2_PRIORITY_LEVEL3,
    WM_HTTP2_PRIORITY_LEVEL4,
    WM_HTTP2_PRIORITY_LEVEL5,
    WM_HTTP2_PRIORITY_MAX
} wm_http2_priority_e;

typedef enum {
    WM_HTTP2_METHOD_GET = 1,
    WM_HTTP2_METHOD_HEAD,
    WM_HTTP2_METHOD_POST,
    WM_HTTP2_METHOD_PUT,
    WM_HTTP2_METHOD_DELETE,
    WM_HTTP2_METHOD_MAX
} wm_http2_client_method_e;

typedef enum wm_http_auth_achema {
    WM_HTTP2_AUTH_NONE = 0, /**< HTTP not use authentication. */
    WM_HTTP2_AUTH_BASIC,    /**< HTTP use basic authentication. */
    WM_HTTP2_AUTH_DIGEST,   /**< HTTP use digest authentication. */
    WM_HTTP2_AUTH_KERBEROS, /**< HTTP use kerberos authentication. */
    WM_HTTP2_AUTH_MAX       /**< HTTP not support authentication. */
} wm_http2_auth_schema_t;

#ifdef __cplusplus
}
#endif

#endif /* WM_HTTP2_CLIENT_TYPE_H */
