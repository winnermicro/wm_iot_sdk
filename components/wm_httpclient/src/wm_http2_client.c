#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_list.h"
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_utils.h"

#include "lwipopts.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/netdb.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/ip.h"
#include "lwip/sockets.h"

#include "wm_http2_client.h"

#define LOG_TAG "http2"
#include "wm_log.h"

static const char *alpn_list[2];
extern const char mbedtls_demos_pem[];

#define WM_HTTP2_DEFAULT_PORT 443
#define HTTP2_ASSERT(expression)                                    \
    do {                                                            \
        if (!expression)                                            \
            wm_log_error("%s %d http2 assert", __func__, __LINE__); \
        while (1)                                                   \
            ;                                                       \
    } while (0)
#define WM_HTTP2_DEFAULT_SCHEME "https"

static wm_http2_client_req_t *wm_http2_client_add_header_from_reqlist(uint16_t req_num, wm_http2_request_list_t *req_list);

#if CONFIG_MBEDTLS_ENABLE_DEBUG
#define DEBUG_LEVEL 3

static void wm_ssl_client_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void)level);

    mbedtls_fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *)ctx);
}
#endif

WM_HTTP2_ERR wm_http2_parse_uri(wm_https_url_param_t *res, const char *uri)
{
    /* We only interested in https */
    size_t len, i, offset;
    int ipv6addr = 0;
    memset(res, 0x00, sizeof(wm_https_url_param_t));
    len = strlen(uri);
    if (len < 9 || memcmp("https://", uri, 8) != 0) {
        return WM_ERR_FAILED;
    }
    offset    = 8;
    res->host = res->hostport = &uri[offset];
    res->hostlen              = 0;
    if (uri[offset] == '[') {
        /* IPv6 literal address */
        ++offset;
        ++res->host;
        ipv6addr = 1;
        for (i = offset; i < len; ++i) {
            if (uri[i] == ']') {
                res->hostlen = i - offset;
                offset       = i + 1;
                break;
            }
        }
    } else {
        const char delims[] = ":/?#";
        for (i = offset; i < len; ++i) {
            if (strchr(delims, uri[i]) != NULL) {
                break;
            }
        }
        res->hostlen = i - offset;
        offset       = i;
    }

    if (res->hostlen == 0) {
        return WM_ERR_FAILED;
    }
    /* Assuming https */
    res->port = 443;
    if (offset < len) {
        if (uri[offset] == ':') {
            /* port */
            const char delims[] = "/?#";
            int port            = 0;
            ++offset;
            for (i = offset; i < len; ++i) {
                if (strchr(delims, uri[i]) != NULL) {
                    break;
                }
                if ('0' <= uri[i] && uri[i] <= '9') {
                    port *= 10;
                    port += uri[i] - '0';
                    if (port > 65535) {
                        return WM_ERR_FAILED;
                    }
                } else {
                    return WM_ERR_FAILED;
                }
            }
            if (port == 0) {
                return WM_ERR_FAILED;
            }
            offset    = i;
            res->port = (uint16_t)port;
        }
    }
    res->hostportlen = (size_t)(uri + offset + ipv6addr - res->host);
    for (i = offset; i < len; ++i) {
        if (uri[i] == '#') {
            break;
        }
    }
    if (i - offset == 0) {
        res->path    = "/";
        res->pathlen = 1;
    } else {
        res->path    = &uri[offset];
        res->pathlen = i - offset;
    }
    return WM_ERR_SUCCESS;
}

static int wm_http2_ssl_send(wm_http2_ssl_t *ssl, char *sndbuf, int len, int flags)
{
    int ret;
    while ((ret = mbedtls_ssl_write(&ssl->ssl, (unsigned char *)sndbuf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            break;
        }
    }
    return ret;
}

static int wm_http2_ssl_recv(wm_http2_ssl_t *ssl, char *buf, int len, int flags)
{
    int ret;

    do {
        ret = mbedtls_ssl_read(&ssl->ssl, (unsigned char *)buf, len);
        //mbedtls_printf("mbedtls_ssl_read ret %d\n", ret);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY || ret == MBEDTLS_ERR_SSL_TIMEOUT) {
            wm_log_info("ssl timeout or ready to close %d ", ret);
            ret = 0;
        }

        if (ret < 0) {
            break;
        }

        if (ret == 0) {
            mbedtls_printf("\n\nEOF\n\n");
            break;
        }

    } while (ret < 0);

    if (mbedtls_ssl_get_bytes_avail(&ssl->ssl) > 0)
        return WM_ERR_FAILED;

    return ret;
}

static ssize_t send_callback(nghttp2_session *session, const uint8_t *data, size_t length, int flags, void *user_data)
{
    wm_http2_client_t *client;
    int rv;
    client = (wm_http2_client_t *)user_data;
    rv     = wm_http2_ssl_send(client->ssl, (char *)data, (int)length, 0);
    if (rv <= 0) {
        wm_log_error("send fail  %d \r\n", rv);
    }
    return rv;
}

static ssize_t recv_callback(nghttp2_session *session, uint8_t *buf, size_t length, int flags, void *user_data)
{
    wm_http2_client_t *client;
    int rv;
    client = (wm_http2_client_t *)user_data;
    rv     = wm_http2_ssl_recv(client->ssl, (char *)buf, (int)length, 0);
    if (rv < 0) {
        wm_log_error("recv fail  %d \r\n", rv);
    } else if (rv == 0) {
        rv = NGHTTP2_ERR_EOF;
    }
    return rv;
}

static int32_t wm_http2_get_req_idx_by_stream(wm_http2_client_t *client, int32_t streamid)
{
    if (!client) {
        wm_log_error("wm_http2_get_req_idx param err");
        return WM_ERR_FAILED;
    }

    for (int i = 0; i < WM_HTTP2_REQ_MAX_NUM; i++) {
        if (client->http2_req[i].stream_id == streamid) {
            return i;
        }
    }

    return WM_ERR_FAILED;
}

static int on_frame_send_callback(nghttp2_session *session, const nghttp2_frame *frame, void *user_data)
{
    wm_http2_client_t *client  = (wm_http2_client_t *)user_data;
    wm_http2_client_req_t *req = NULL;

    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:
            if (NULL != (req = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id))) {
                wm_http2_event_param_t event = { 0 };
                const nghttp2_nv *nva        = frame->headers.nva;
                for (int i = 0; i < frame->headers.nvlen; ++i) {
                    event.session = (uint32_t)client;
                    if (WM_ERR_FAILED == (event.streamid = wm_http2_get_req_idx_by_stream(client, frame->hd.stream_id))) {
                        wm_log_error("get req idx fail ");
                        return 0;
                    }
                    event.http2_data.header.name  = (char *)nva[i].name;
                    event.http2_data.header.value = (char *)nva[i].value;
                    if (client && client->event_handler) {
                        client->event_handler(WM_HTTP2_EVENT_HEADER_SENTED, (void *)&event, client->priv);
                    }
                    wm_log_info("header %s value %s ", nva[i].name, nva[i].value);
                }
            }
            break;
        case NGHTTP2_RST_STREAM:
            wm_log_info("[INFO] C ----------------------------> S (RST_STREAM)\n");
            break;
        case NGHTTP2_GOAWAY:
            wm_log_info("[INFO] C ----------------------------> S (GOAWAY)\n");
            break;
    }
    return 0;
}

static int on_frame_recv_callback(nghttp2_session *session, const nghttp2_frame *frame, void *user_data)
{
    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:
            if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE)
                wm_log_info("[INFO] C <---------------------------- S (HEADERS)\n");
            break;
        case NGHTTP2_RST_STREAM:
            wm_log_info("[INFO] C <---------------------------- S (RST_STREAM)\n");
            break;
        case NGHTTP2_GOAWAY:
            wm_log_info("[INFO] C <---------------------------- S (GOAWAY)\n");
            break;
        case NGHTTP2_SETTINGS:
            wm_log_info("[INFO] C <---------------------------- S (SETTINGS)\n");
            break;
    }
    return 0;
}

static int on_stream_close_callback(nghttp2_session *session, int32_t stream_id, uint32_t error_code, void *user_data)
{
    wm_log_info("streamid %d close", stream_id);
    if (nghttp2_session_get_stream_user_data(session, stream_id)) {
        int rv;
        rv = nghttp2_session_terminate_session(session, NGHTTP2_NO_ERROR);
        if (rv != 0) {
            wm_log_error("nghttp2_session_terminate_session %d ", rv);
        }
    }
    return 0;
}

static int on_data_chunk_recv_callback(nghttp2_session *session, uint8_t flags, int32_t stream_id, const uint8_t *data,
                                       size_t len, void *user_data)
{
    wm_http2_event_param_t event = { 0 };
    wm_http2_client_t *client    = (wm_http2_client_t *)user_data;
    wm_http2_client_req_t *req   = NULL;
    nghttp2_stream *stream;

    if (NULL != (req = nghttp2_session_get_stream_user_data(session, stream_id))) {
        wm_log_info("[INFO] C <---------------------------- S (DATA chunk)\n"
                    "%lu bytes\n",
                    (unsigned long int)len);
        event.http2_data.data.data     = (uint8_t *)data;
        event.http2_data.data.data_len = len;
        event.session                  = (uint32_t)client;
        if ((WM_ERR_FAILED == (event.streamid = wm_http2_get_req_idx_by_stream(client, stream_id))) ||
            (NULL == (stream = nghttp2_session_get_stream(session, stream_id)))) {
            wm_log_error("WTF ? idx not found");
            return 0;
        }

        req->status = stream->status_code;
        if (client && client->event_handler) {
            client->event_handler(WM_HTTP2_EVENT_RECV_DATA, (void *)&event, client->priv);
        }
    }
    return 0;
}

static int on_header_recv_callback(nghttp2_session *session, const nghttp2_frame *frame, const uint8_t *name, size_t namelen,
                                   const uint8_t *value, size_t valuelen, uint8_t flags, void *user_data)
{
    wm_http2_event_param_t event = { 0 };
    wm_http2_client_t *client    = (wm_http2_client_t *)user_data;
    wm_http2_client_req_t *req   = NULL;

    if (NULL != (req = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id))) {
        wm_log_info("[INFO] C <---------------------------- S (HEADERS)\n");
        event.session = (uint32_t)client;
        if (WM_ERR_FAILED == (event.streamid = wm_http2_get_req_idx_by_stream(client, frame->hd.stream_id))) {
            wm_log_error("get req idx fail ");
            return 0;
        }
        event.http2_data.header.name  = (char *)name;
        event.http2_data.header.value = (char *)value;
        if (client && client->event_handler) {
            client->event_handler(WM_HTTP2_EVENT_RECV_HEADER, (void *)&event, client->priv);
        }
        wm_log_info("header %s: value %s ", name, value);
    }
    return 0;
}

//ret fd
static int wm_http2_connto(const char *host, uint16_t port)
{
    int rc                  = -1;
    int fd                  = -1;
    struct addrinfo *result = NULL;
    struct addrinfo hints   = { 0, AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL };
    struct sockaddr_in addr = { 0 };
    addr.sin_port           = htons(WM_HTTP2_DEFAULT_PORT);
    addr.sin_family         = AF_INET;

    rc = getaddrinfo(host, NULL, &hints, &result);
    if (rc == 0) {
        struct addrinfo *res = result;

        while (res) {
            if (res->ai_family == AF_INET) {
                addr.sin_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;
                wm_log_info("+IPV4:\"%s\"", inet_ntoa(addr.sin_addr));

                //create socket
                fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
                if (fd < 0) {
                    wm_log_error("socket error");
                    freeaddrinfo(result);
                    return WM_ERR_FAILED;
                }

                //conn
                rc = connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
                if (rc == 0 || rc == EWOULDBLOCK || rc == EINPROGRESS) {
                    ;
                } else {
                    wm_log_info("Connection Failed: %d.  Exiting\n", rc);
                    freeaddrinfo(result);
                    close(fd);
                    return WM_ERR_FAILED;
                }
            }
            res = res->ai_next;
        }
        freeaddrinfo(result);
    } else {
        wm_log_error("getaddrinfo error");
        return WM_ERR_FAILED;
    }

    return fd;
}

static void wm_http2_ssl_release(wm_http2_ssl_t *ssl)
{
    if (ssl) {
        mbedtls_net_free(&ssl->server_fd);
        mbedtls_x509_crt_free(&ssl->cacert);
        mbedtls_ssl_free(&ssl->ssl);
        mbedtls_ssl_config_free(&ssl->conf);
        mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
        mbedtls_entropy_free(&ssl->entropy);
        wm_os_internal_free(ssl);
    }
}

static wm_http2_ssl_t *wm_http2_do_sslconnect(const char *host, uint16_t port)
{
    int ret             = MBEDTLS_EXIT_SUCCESS;
    const char *pers    = "ssl_client";
    wm_http2_ssl_t *ssl = NULL;
    int fd              = -1;

    ssl = wm_os_internal_malloc(sizeof(wm_http2_ssl_t));
    if (!ssl) {
        return NULL;
    }

#if defined(CONFIG_MBEDTLS_ENABLE_DEBUG)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    /*
    * 0. Initialize the RNG and the session data
    */
    mbedtls_net_init(&ssl->server_fd);
    mbedtls_ssl_init(&ssl->ssl);
    mbedtls_ssl_config_init(&ssl->conf);
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);

    mbedtls_entropy_init(&ssl->entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg, mbedtls_entropy_func, &ssl->entropy, (const unsigned char *)pers,
                                     strlen(pers))) != 0) {
        goto exit;
    }

    /*
    * 0. Initialize certificates
    */
    ret = mbedtls_x509_crt_parse(&ssl->cacert, (const unsigned char *)mbedtls_demos_pem, strlen((char *)mbedtls_demos_pem) + 1);
    if (ret < 0) {
        wm_log_info(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        goto exit;
    }

    /*
    * 1. Start the connection
    */
    fflush(stdout);

    if (WM_ERR_FAILED == (fd = wm_http2_connto(host, port))) {
        wm_log_error("http2 conn fail");
        goto exit;
    }
    ssl->server_fd.fd = fd;

    /*
    * 2. Setup stuff
    */
    fflush(stdout);

    if ((ret = mbedtls_ssl_config_defaults(&ssl->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        wm_log_info(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    // only support h2 protocal
    alpn_list[0] = "h2";
    alpn_list[1] = NULL;
    mbedtls_ssl_conf_alpn_protocols(&ssl->conf, alpn_list);
    wm_log_info(" ok\n");

    /* OPTIONAL is not optimal for security,
    * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(&ssl->conf, MBEDTLS_SSL_VERIFY_NONE);
    //mbedtls_ssl_conf_authmode( &ssl->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&ssl->conf, &ssl->cacert, NULL);
    mbedtls_ssl_conf_rng(&ssl->conf, mbedtls_ctr_drbg_random, &ssl->ctr_drbg);
#if defined(CONFIG_MBEDTLS_ENABLE_DEBUG)
    mbedtls_ssl_conf_dbg(&ssl->conf, wm_ssl_client_debug, stdout);
#endif
    mbedtls_ssl_conf_read_timeout(&ssl->conf, 5000);

    if ((ret = mbedtls_ssl_setup(&ssl->ssl, &ssl->conf)) != 0) {
        wm_log_info(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    //mbedtls_ssl_set_bio( &ssl->ssl, &ssl->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );
    mbedtls_ssl_set_bio(&ssl->ssl, &ssl->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
    * 4. Handshake
    */
    wm_log_info("  . Performing the SSL/TLS handshake...");
    fflush(stdout);

    while ((ret = mbedtls_ssl_handshake(&ssl->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            wm_log_info(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            goto exit;
        }
    }

    wm_log_info(" ok\n");

    return ssl;
exit:

#ifdef MBEDTLS_ERROR_C
    if (ret != MBEDTLS_EXIT_SUCCESS) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, 100);
        wm_log_info("Last error was: %d - %s\n\n", ret, error_buf);
    }
#endif

    mbedtls_net_free(&ssl->server_fd);
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_ssl_free(&ssl->ssl);
    mbedtls_ssl_config_free(&ssl->conf);
    mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
    mbedtls_entropy_free(&ssl->entropy);
    wm_log_info("start free ssl(%x)...\n", (unsigned int)ssl);
    wm_os_internal_free(ssl);
    return NULL;
}

/*
 * Submits the request |req| to the connection |connection|.  This
 * function does not send packets; just append the request to the
 * internal queue in |connection->session|.
 */
static int32_t wm_http2_submit_request(wm_http2_client_req_t *req, wm_http2_client_t *client)
{
    int32_t stream_id;
    wm_log_debug("submit weight %d exclusive %d", req->pri_spec.weight, req->pri_spec.exclusive);
    stream_id =
        nghttp2_submit_request(client->session, &req->pri_spec, req->http_header, req->http_header_num,
                               (((req->method == WM_HTTP2_METHOD_POST || req->method == WM_HTTP2_METHOD_PUT) && req->content) ?
                                    (nghttp2_data_provider *)&req->data_provider :
                                    NULL),
                               req);

    if (stream_id < 0) {
        return WM_ERR_FAILED;
    }
    wm_log_info("ID %d submit weight %d exclusive %d", stream_id, req->pri_spec.weight, req->pri_spec.exclusive);

    req->stream_id = stream_id;
    nghttp2_submit_priority(client->session, 0, req->stream_id, &req->pri_spec);
    return WM_ERR_SUCCESS;
}

static ssize_t wm_http2_data_source_read_callback(nghttp2_session *session, int32_t stream_id, uint8_t *buf, size_t length,
                                                  uint32_t *data_flags, nghttp2_data_source *source, void *user_data)
{
    size_t size                = 0;
    wm_http2_client_req_t *req = (wm_http2_client_req_t *)source->ptr;
    if (!req || req->content_offset >= req->content_len) {
        wm_log_error("req null or source read already finish %d %d %d", stream_id, req->content_offset, req->content_len);
        *data_flags = NGHTTP2_DATA_FLAG_EOF;
        return 0;
    }
    size = (req->content_len - req->content_offset) > length ? length : (req->content_len - req->content_offset);
    memcpy(buf, req->content + req->content_offset, size);
    req->content_offset += size;
    if (req->content_offset >= req->content_len)
        *data_flags = NGHTTP2_DATA_FLAG_EOF;
    return size;
}

static WM_HTTP2_ERR wm_http2_get_prio(nghttp2_priority_spec *pri_spec, wm_http2_priority_e pri)
{
#define WM_HTTP2_PRIORITY_WEIGHT1 16
#define WM_HTTP2_PRIORITY_WEIGHT2 48
#define WM_HTTP2_PRIORITY_WEIGHT3 96
#define WM_HTTP2_PRIORITY_WEIGHT4 192

    pri_spec->stream_id = 0;
    pri_spec->weight    = 16;
    pri_spec->exclusive = 0;
    switch (pri) {
        case WM_HTTP2_PRIORITY_LEVEL1:
            pri_spec->weight = WM_HTTP2_PRIORITY_WEIGHT1;
            break;
        case WM_HTTP2_PRIORITY_LEVEL2:
            pri_spec->weight = WM_HTTP2_PRIORITY_WEIGHT2;
            break;
        case WM_HTTP2_PRIORITY_LEVEL3:
            pri_spec->weight = WM_HTTP2_PRIORITY_WEIGHT3;
            break;
        case WM_HTTP2_PRIORITY_LEVEL4:
            pri_spec->weight = WM_HTTP2_PRIORITY_WEIGHT4;
            break;
        case WM_HTTP2_PRIORITY_LEVEL5:
            pri_spec->exclusive = 1;
            break;
        default:
            return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static WM_HTTP2_ERR wm_http2_request_init(wm_http2_client_t *client, const wm_http2_client_config_t *http2_client_cfg)
{
    char hostport[256] = { 0 }; // max host:port len

    client->conn_info.host = strdup(http2_client_cfg->host);
    client->conn_info.port = http2_client_cfg->port;
    snprintf(hostport, sizeof(hostport), "%s:%d", http2_client_cfg->host, http2_client_cfg->port);
    client->conn_info.hostport = strdup(hostport);

    if (NULL ==
        (client->http2_req = wm_http2_client_add_header_from_reqlist(http2_client_cfg->req_num, http2_client_cfg->req))) {
        wm_log_error("http req init add header err");
        return WM_ERR_FAILED;
    }
    client->http2_reqnum = http2_client_cfg->req_num;

    return WM_ERR_SUCCESS;
}

/*
 * Update |pollfd| based on the state of |connection|.
 */
static void ctl_poll(struct pollfd *pollfd, wm_http2_client_t *client)
{
    pollfd->events = 0;
    if (nghttp2_session_want_read(client->session)) {
        pollfd->events |= POLLIN;
    }

    if (nghttp2_session_want_write(client->session)) {
        pollfd->events |= POLLOUT;
    }
}

static int32_t set_tcp_nodelay(int fd)
{
    int val = 1;
    int rv;
    rv = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, (socklen_t)sizeof(val));
    if (rv == -1) {
        wm_log_error("set tcp nodelay err %d", rv);
        return WM_ERR_FAILED;
    }
    return WM_ERR_SUCCESS;
}

WM_HTTP2_ERR wm_http2_client_get_status_code(wm_http2_client_handle client_handle, uint8_t idx)
{
    wm_http2_client_t *client  = NULL;
    wm_http2_client_req_t *req = NULL;

    client = (wm_http2_client_t *)client_handle;
    if (!client || idx >= WM_HTTP2_REQ_MAX_NUM) {
        wm_log_error("client handle is null or idx err");
        return WM_ERR_FAILED;
    }

    req = &client->http2_req[idx];

    return 0 == req->status ? WM_ERR_FAILED : req->status;
}

static void wm_http2_client_clean_req(wm_http2_client_t *client)
{
    int i                      = 0;
    int j                      = 0;
    wm_http2_client_req_t *req = NULL;

    for (i = 0; i < client->http2_reqnum; i++) {
        req = (client->http2_req + i);
        if (req->path)
            wm_os_internal_free(req->path);
        req->path = NULL;
        if (req->content)
            wm_os_internal_free(req->content);
        req->content = NULL;
        for (j = 0; j < WM_HTTP2_HEADER_MAX_NUM; j++) {
            if (req->http_header[j].name)
                wm_os_internal_free(req->http_header[j].name);
            req->http_header[j].name = NULL;
            if (req->http_header[j].value)
                wm_os_internal_free(req->http_header[j].value);
            req->http_header[j].value    = NULL;
            req->http_header[j].valuelen = 0;
            req->http_header[j].namelen  = 0;
        }
    }

    wm_os_internal_free(client->http2_req);
    client->http2_req    = NULL;
    client->http2_reqnum = 0;
}

static wm_http2_client_req_t *wm_http2_client_add_header_from_reqlist(uint16_t req_num, wm_http2_request_list_t *req_list)
{
    int i                           = 0;
    wm_http2_client_req_t *req      = NULL;
    wm_http2_client_req_t *req_head = NULL;

    req = (wm_http2_client_req_t *)wm_os_internal_malloc(sizeof(wm_http2_client_req_t) * req_num);
    if (!req) {
        wm_log_error("http2 req alloc fail");
        return NULL;
    }
    memset(req, 0x00, sizeof(wm_http2_client_req_t) * req_num);
    req_head = req;

    for (i = 0; i < req_num; i++, req_list++, req++) {
        if (WM_HTTP2_METHOD_GET > req_list->method || WM_HTTP2_METHOD_MAX < req_list->method ||
            WM_ERR_SUCCESS != wm_http2_get_prio(&req->pri_spec, req_list->prio)) {
            wm_log_error("update req method err %d prio %d", req_list->method, req_list->prio);
            goto exit;
        }
        req->path      = strdup(req_list->path);
        req->stream_id = -1;
        req->method    = req_list->method;
        strcpy(req->method_name, req->method == WM_HTTP2_METHOD_GET    ? "GET" :
                                 req->method == WM_HTTP2_METHOD_HEAD   ? "HEAD" :
                                 req->method == WM_HTTP2_METHOD_POST   ? "POST" :
                                 req->method == WM_HTTP2_METHOD_PUT    ? "PUT" :
                                 req->method == WM_HTTP2_METHOD_DELETE ? "DELETE" :
                                                                         "GET");
        if ((WM_HTTP2_METHOD_POST == req->method || WM_HTTP2_METHOD_PUT == req->method) && req_list->content &&
            req_list->content_len) {
            req->content                     = (uint8_t *)strdup((char *)req_list->content);
            req->content_len                 = strlen((char *)req->content);
            req->content_offset              = 0;
            req->data_provider.source.ptr    = (void *)req;
            req->data_provider.read_callback = wm_http2_data_source_read_callback;
        }
    }

    return req_head;

exit:
    req = req_head;
    for (i = 0; i < req_num; i++, req++) {
        if (req->path)
            wm_os_internal_free(req->path);
        if (req->content)
            wm_os_internal_free(req->content);
    }
    wm_os_internal_free(req_head);
    return NULL;
}

WM_HTTP2_ERR wm_http2_client_update_req(wm_http2_client_handle client_handle, uint16_t req_num,
                                        wm_http2_request_list_t *req_list)
{
    wm_http2_client_t *client  = NULL;
    wm_http2_client_req_t *req = NULL;

    client = (wm_http2_client_t *)client_handle;
    if (!client) {
        wm_log_error("client handle is null or idx err");
        return WM_ERR_FAILED;
    }

    if (NULL == (req = wm_http2_client_add_header_from_reqlist(req_num, req_list))) {
        wm_log_error("add header from reqlist fail");
        return WM_ERR_FAILED;
    }
    wm_http2_client_clean_req(client);

    client->http2_reqnum = req_num;
    client->http2_req    = req;
    return WM_ERR_SUCCESS;
}

WM_HTTP2_ERR wm_http2_client_add_header(wm_http2_client_handle client_handle, uint8_t idx, char *name, char *value)
{
    wm_http2_client_t *client  = NULL;
    wm_http2_client_req_t *req = NULL;
    int i                      = 0;

    client = (wm_http2_client_t *)client_handle;
    if (!client || idx >= client->http2_reqnum) {
        wm_log_error("client handle is null or idx err");
        return WM_ERR_FAILED;
    }

    req = &client->http2_req[idx];

    for (i = 0; i < WM_HTTP2_HEADER_MAX_NUM; i++) {
        if (req->http_header[i].name && !strcmp((char *)req->http_header[i].name, name)) {
            wm_log_info("header confilt");
            return WM_ERR_FAILED;
        }
    }

    for (i = 0; i < WM_HTTP2_HEADER_MAX_NUM; i++) {
        if (!req->http_header[i].name) {
            req->http_header[i].name     = (uint8_t *)strdup((const char *)name);
            req->http_header[i].namelen  = strlen(name);
            req->http_header[i].value    = (uint8_t *)strdup(value);
            req->http_header[i].valuelen = strlen(value);
            req->http_header[i].flags    = NGHTTP2_NV_FLAG_NONE;
            req->http_header_num++;
            return WM_ERR_SUCCESS;
        }
    }

    return WM_ERR_FAILED;
}

WM_HTTP2_ERR wm_http2_client_delete_header(wm_http2_client_handle client_handle, uint8_t idx, char *name)
{
    wm_http2_client_t *client  = NULL;
    wm_http2_client_req_t *req = NULL;

    client = (wm_http2_client_t *)client_handle;
    if (!client || idx >= client->http2_reqnum) {
        wm_log_error("client handle is null or idx err");
        return WM_ERR_FAILED;
    }

    req = &client->http2_req[idx];

    for (int i = 0; i < WM_HTTP2_HEADER_MAX_NUM; i++) {
        if (req->http_header[i].name && !strcmp((char *)req->http_header[i].name, name)) {
            if (req->http_header[i].name)
                wm_os_internal_free(req->http_header[i].name);
            if (req->http_header[i].value)
                wm_os_internal_free(req->http_header[i].value);
            req->http_header[i].name     = NULL;
            req->http_header[i].namelen  = 0;
            req->http_header[i].flags    = 0;
            req->http_header[i].value    = NULL;
            req->http_header[i].valuelen = 0;
            req->http_header_num--;
            return WM_ERR_SUCCESS;
        }
    }
    return WM_ERR_FAILED;
}

static WM_HTTP2_ERR wm_http2_client_release(wm_http2_client_t *client)
{
    if (client) {
        if (client->conn_info.host)
            wm_os_internal_free(client->conn_info.host);
        client->conn_info.host = NULL;
        if (client->conn_info.hostport)
            wm_os_internal_free(client->conn_info.hostport);
        client->conn_info.hostport = NULL;
        if (client->conn_info.proxy_host)
            wm_os_internal_free(client->conn_info.proxy_host);
        client->conn_info.proxy_host = NULL;
        if (client->conn_info.username)
            wm_os_internal_free(client->conn_info.username);
        client->conn_info.username = NULL;
        if (client->conn_info.password)
            wm_os_internal_free(client->conn_info.password);
        client->conn_info.password = NULL;

        wm_http2_client_clean_req(client);
        wm_os_internal_free(client);
    }
    return WM_ERR_SUCCESS;
}

WM_HTTP2_ERR wm_http2_client_cleanup(wm_http2_client_handle client_handle)
{
    wm_http2_client_t *client = NULL;
    client                    = (wm_http2_client_t *)client_handle;
    if (!client) {
        wm_log_error("close handle is null");
        return WM_ERR_FAILED;
    }

    wm_http2_ssl_release(client->ssl);
    nghttp2_session_del(client->session);
    wm_http2_client_release(client);
    return WM_ERR_SUCCESS;
}

wm_http2_client_handle wm_http2_client_init(wm_http2_client_config_t *http2_client_cfg)
{
    if (!http2_client_cfg) {
        wm_log_error("http cfg null");
        return (wm_http2_client_handle)WM_INVALID_HANDLE;
    }

    int i = 0;
    int ret;
    wm_http2_client_t *client               = NULL;
    nghttp2_session_callbacks *callbacks    = NULL;
    wm_http2_request_list_t *client_cfg_req = NULL;
    wm_http2_client_req_t *client_req       = NULL;

    client = (wm_http2_client_t *)wm_os_internal_malloc(sizeof(wm_http2_client_t));
    if (!client) {
        wm_log_error("client alloc err");
        return (wm_http2_client_handle)WM_INVALID_HANDLE;
    }
    memset(client, 0x00, sizeof(wm_http2_client_t));

    // set event handle
    client->event_handler = http2_client_cfg->event_handler;
    client->priv          = http2_client_cfg->priv;

    if (WM_ERR_SUCCESS != wm_http2_request_init(client, http2_client_cfg)) {
        wm_http2_client_cleanup((wm_http2_client_handle)client);
        return (wm_http2_client_handle)WM_INVALID_HANDLE;
    }

    ret = nghttp2_session_callbacks_new(&callbacks);
    if (ret != 0) {
        wm_log_error("nghttp2_session_callbacks_new fail %d\r\n", ret);
        wm_http2_client_cleanup((wm_http2_client_handle)client);
        return (wm_http2_client_handle)WM_INVALID_HANDLE;
    }

    nghttp2_session_callbacks_set_send_callback(callbacks, send_callback);
    nghttp2_session_callbacks_set_recv_callback(callbacks, recv_callback);
    nghttp2_session_callbacks_set_on_frame_send_callback(callbacks, on_frame_send_callback);
    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, on_frame_recv_callback);
    nghttp2_session_callbacks_set_on_stream_close_callback(callbacks, on_stream_close_callback);
    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, on_data_chunk_recv_callback);
    nghttp2_session_callbacks_set_on_header_callback(callbacks, on_header_recv_callback);

    ret = nghttp2_session_client_new(&client->session, callbacks, client);
    nghttp2_session_callbacks_del(callbacks);
    if (ret != 0) {
        wm_log_error("nghttp2_session_callbacks_del err %d", ret);
        wm_http2_client_cleanup((wm_http2_client_handle)client);
        return (wm_http2_client_handle)WM_INVALID_HANDLE;
    }

    ret = nghttp2_submit_settings(client->session, NGHTTP2_FLAG_NONE, NULL, 0);
    if (ret != 0) {
        wm_log_error("nghttp2_submit_settings err %d", ret);
        wm_http2_client_cleanup((wm_http2_client_handle)client);
        return (wm_http2_client_handle)WM_INVALID_HANDLE;
    }

    client_cfg_req = http2_client_cfg->req;
    client_req     = client->http2_req;

    for (i = 0; i < client->http2_reqnum; i++, client_cfg_req++, client_req++) {
        //add req list header
        if (WM_ERR_SUCCESS !=
                wm_http2_client_add_header((wm_http2_client_handle)client, i, ":method", client_req->method_name) ||
            WM_ERR_SUCCESS != wm_http2_client_add_header((wm_http2_client_handle)client, i, ":path", client_req->path) ||
            WM_ERR_SUCCESS !=
                wm_http2_client_add_header((wm_http2_client_handle)client, i, ":scheme", WM_HTTP2_DEFAULT_SCHEME) ||
            WM_ERR_SUCCESS !=
                wm_http2_client_add_header((wm_http2_client_handle)client, i, ":authority", client->conn_info.hostport)) {
            wm_log_error("add req header fail");
            wm_http2_client_cleanup((wm_http2_client_handle)client);
            return (wm_http2_client_handle)WM_INVALID_HANDLE;
        }

        if (client_cfg_req->content_type) {
            if (WM_ERR_SUCCESS !=
                wm_http2_client_add_header((wm_http2_client_handle)client, i, ":content-type", client_cfg_req->content_type)) {
                wm_log_error("add req header fail");
                wm_http2_client_cleanup((wm_http2_client_handle)client);
                return (wm_http2_client_handle)WM_INVALID_HANDLE;
            }
        }

        // add header in here
    }

    return (wm_http2_client_handle)client;
}

WM_HTTP2_ERR wm_http2_client_perform(wm_http2_client_handle client_handle)
{
    wm_http2_client_t *client = NULL;
    int ret;
    struct pollfd pollfds[1];
    nfds_t npollfds = 1;

    client = (wm_http2_client_t *)client_handle;
    if (!client) {
        wm_log_error("perform handle is null");
        return WM_ERR_FAILED;
    }

    if (!client->ssl) {
        //do ssl conn
        if (NULL == (client->ssl = wm_http2_do_sslconnect(client->conn_info.host, client->conn_info.port))) {
            wm_log_error("ssl conn err");
            return WM_ERR_FAILED;
        }

        if (client->event_handler) {
            client->event_handler(WM_HTTP2_EVENT_CONNECTED, NULL, client->priv);
        }
        // set TCP_NODELAY for lower latency on control messages
        if (WM_ERR_SUCCESS != set_tcp_nodelay(client->ssl->server_fd.fd)) {
            wm_log_error("set_tcp_nodelay fail");
            return WM_ERR_FAILED;
        }
    }

    for (int i = 0; i < client->http2_reqnum; i++) {
        if (WM_ERR_SUCCESS != (ret = wm_http2_submit_request(&client->http2_req[i], client))) {
            wm_log_error("http2 submit err %d %d ", i, ret);
            return WM_ERR_FAILED;
        }
    }

    // -------------------------------------------------
    // Poll for response
    // -------------------------------------------------
    pollfds[0].fd = client->ssl->server_fd.fd;
    ctl_poll(pollfds, client);

    while (nghttp2_session_want_read(client->session) || nghttp2_session_want_write(client->session)) {
        int nfds = poll(pollfds, npollfds, -1);
        if (nfds == -1) {
            wm_log_error("poll err");
            return WM_ERR_FAILED;
        }

        if (pollfds[0].revents & (POLLOUT)) {
            ret = nghttp2_session_send(client->session);
            if (ret != 0) {
                wm_log_error("session send fail %d \r\n", ret);
                return WM_ERR_FAILED;
            }
        }

        if (pollfds[0].revents & (POLLIN)) {
            ret = nghttp2_session_recv(client->session);
            if (ret != 0 && ret != NGHTTP2_ERR_EOF) {
                wm_log_error("session recv over %d \r\n", ret);
                break;
            }
        }

        if ((pollfds[0].revents & POLLHUP) || (pollfds[0].revents & POLLERR)) {
            wm_log_error("Connection error");
            return WM_ERR_FAILED;
        }
        ctl_poll(pollfds, client);
    }

    return WM_ERR_SUCCESS;
}
