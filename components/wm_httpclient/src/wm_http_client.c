#include <stdarg.h>
#include "wm_types.h"

#define LOG_TAG "httpc"
#include "wm_log.h"

#include "wm_http_client.h"

typedef HTTP_CLIENT_SESSION_FLAGS wm_http_session_flags_t;

#if CONFIG_WM_HTTP_CLIENT_DEBUG
static void wm_http_client_debug_callback(const char *title, const char *data, uint32_t data_len, char *format, ...)
{
    va_list args;

    wm_log_dump(WM_LOG_LEVEL_DEBUG, title, 32, data, data_len);

    va_start(args, format);
    wm_log_vprintf(WM_LOG_LEVEL_DEBUG, LOG_TAR, format, args);
    va_end(args);
}
#endif

/***************************************************************************
* Function: wm_http_client_close_request
* Description: Closes any active connection and free any used memory.
*
* Input: pSession: HTTP client handle.
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_close_request(wm_http_client_t *session)
{
    if (!session) {
        wm_log_error("invalid param");
        return WM_ERR_INVALID_PARAM;
    }

    int ret = HTTPClientCloseRequest(session);
    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("close request failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }
    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_open_request
* Description: Allocate memory for a new HTTP Session.
*
* Input: Flags: HTTP Session internal API flags, 0 should be passed here.
*             : http_event_handle  event handle func
*             : priv               user private data
* Output: None
*
* Return: HTTP client handle
*
* Date : 2014-6-6
****************************************************************************/
wm_http_client_t wm_http_client_open_request(const wm_http_client_config_t *client_config)
{
    wm_http_session_flags_t http_flags = 0;
    wm_http_client_t seesion_handle;
    int nRetCode;

    if (!client_config) {
        wm_log_error("invalid client config");
        return WM_HTTP_CLIENT_INVALID;
    }

#if CONFIG_WM_HTTP_CLIENT_PROXY
    // Use Proxy server
    if ((true == client_config->use_proxy) && ((NULL == client_config->proxy_host) || (0 == client_config->proxy_port))) {
        wm_log_error("proxy param error");
        return WM_HTTP_CLIENT_INVALID;
    }
#endif //CONFIG_WM_HTTP_CLIENT_PROXY

#if CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
    if ((client_config->auth_type >= WM_HTTP_CLIENT_AUTH_MAX) ||
        ((client_config->auth_type >= WM_HTTP_CLIENT_AUTH_BASIC) && (!client_config->username || !client_config->password))) {
        wm_log_error("auth param error");
        return WM_HTTP_CLIENT_INVALID;
    }
#endif //CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST

    if ((client_config->method != WM_HTTP_CLIENT_REQUEST_TYPE_GET) &&
        (client_config->method != WM_HTTP_CLIENT_REQUEST_TYPE_HEAD) &&
        (client_config->method != WM_HTTP_CLIENT_REQUEST_TYPE_POST) &&
        (client_config->method != WM_HTTP_CLIENT_REQUEST_TYPE_PUT) &&
        (client_config->method != WM_HTTP_CLIENT_REQUEST_TYPE_DELETE)) {
        wm_log_error("method error");
        return WM_HTTP_CLIENT_INVALID;
    }

    if (client_config->content_type < WM_HTTP_CLIENT_CONTENT_NONE ||
        client_config->content_type > WM_HTTP_CLIENT_CONTENT_TEXT_XML) {
        wm_log_error("content type error");
        return WM_HTTP_CLIENT_INVALID;
    }

    if (client_config->keep_alive)
        http_flags |= HTTP_CLIENT_FLAG_KEEP_ALIVE;
    if (client_config->no_cache)
        http_flags |= HTTP_CLIENT_FLAG_NO_CACHE;
    if (client_config->chunked)
        http_flags |= HTTP_CLIENT_FLAG_SEND_CHUNKED;

    if (client_config->event_handler)
        seesion_handle = HTTPClientOpenRequestWithCb(http_flags, client_config->event_handler, client_config->priv);
    else
        seesion_handle = HTTPClientOpenRequest(http_flags);

    if (!seesion_handle) {
        wm_log_error("crate seesion fail");
        return WM_HTTP_CLIENT_INVALID;
    }

    // Set the Verb
    nRetCode = wm_http_client_set_verb(seesion_handle, client_config->method);
    if (nRetCode != HTTP_CLIENT_SUCCESS) {
        goto exit;
    }

#if CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
    // Set authentication
    if (WM_HTTP_CLIENT_AUTH_NONE != client_config->auth_type) {
        if ((nRetCode = wm_http_client_set_auth(seesion_handle, client_config->auth_type)) != HTTP_CLIENT_SUCCESS) {
            goto exit;
        }

        // Set authentication
        if ((nRetCode = wm_http_client_set_credentials(seesion_handle, client_config->username, client_config->password)) !=
            HTTP_CLIENT_SUCCESS) {
            goto exit;
        }
    }
#endif //CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST

#if CONFIG_WM_HTTP_CLIENT_PROXY
    // Use Proxy server
    if (true == client_config->use_proxy) {
        if ((nRetCode = wm_http_client_set_proxy(seesion_handle, client_config->proxy_host, client_config->proxy_port, NULL,
                                                 NULL)) != HTTP_CLIENT_SUCCESS) {
            goto exit;
        }
    }
#endif //CONFIG_WM_HTTP_CLIENT_PROXY

    if (client_config->content_type >= WM_HTTP_CLIENT_CONTENT_APPLICATION_URLENCODED &&
        client_config->content_type <= WM_HTTP_CLIENT_CONTENT_TEXT_XML) {
        if ((nRetCode = wm_http_client_add_request_headers(
                            seesion_handle, "Content-Type",
                            client_config->content_type == WM_HTTP_CLIENT_CONTENT_APPLICATION_URLENCODED ? "application/"
                                                                                                           "x-www-form-"
                                                                                                           "urlencoded" :
                            client_config->content_type == WM_HTTP_CLIENT_CONTENT_APPLICATION_JSON       ? "application/json" :
                            client_config->content_type == WM_HTTP_CLIENT_CONTENT_MULTIPART_FORM_DATA ? "multipart/form-data" :
                            client_config->content_type == WM_HTTP_CLIENT_CONTENT_TEXT_XML            ? "text/xml" :
                                                                                                        "application/"
                                                                                                        "x-www-form-urlencoded") !=
                        HTTP_CLIENT_SUCCESS)) {
            goto exit;
        }
    }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    HTTPClientSetDebugHook(seesion_handle, wm_http_client_debug_callback);
#endif

    return seesion_handle;

exit:
    wm_log_error("open request failed: %s", HTTP_error_code_2_string(nRetCode));
    wm_http_client_close_request(&seesion_handle);
    return WM_HTTP_CLIENT_INVALID;
}

int wm_http_client_set_no_cache(wm_http_client_t session, bool no_cache)
{
    P_HTTP_SESSION pHTTPSession = NULL; // Session pointer

    if (!session)
        return WM_ERR_INVALID_PARAM;

    pHTTPSession            = (P_HTTP_SESSION)session;
    pHTTPSession->HttpFlags = (true == no_cache ? pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_NO_CACHE :
                                                  pHTTPSession->HttpFlags & (!HTTP_CLIENT_FLAG_NO_CACHE));
    return WM_ERR_SUCCESS;
}

int wm_http_client_set_keepalive(wm_http_client_t session, bool keepalive)
{
    P_HTTP_SESSION pHTTPSession = NULL; // Session pointer

    if (!session)
        return WM_ERR_INVALID_PARAM;

    pHTTPSession            = (P_HTTP_SESSION)session;
    pHTTPSession->HttpFlags = (true == keepalive ? pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_KEEP_ALIVE :
                                                   pHTTPSession->HttpFlags & (!HTTP_CLIENT_FLAG_KEEP_ALIVE));
    return WM_ERR_SUCCESS;
}

#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
/***************************************************************************
* Function: wm_http_client_set_auth
* Description: Sets the HTTP authentication schema.
*
* Input: pSession:       HTTP client handle.
*           AuthSchema: HTTP Supported authentication methods.
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_set_auth(wm_http_client_t session, wm_http_client_auth_schema_t auth_schema)
{
    int ret = HTTPClientSetAuth(session, (HTTP_AUTH_SCHEMA)auth_schema, NULL);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("set auth failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_set_credentials
* Description: Sets credentials for the target host.
*
* Input: pSession: HTTP client handle.
*           pUserName:   User name.
*           pPassword:    Password.
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_set_credentials(wm_http_client_t session, char *user_name, char *password)
{
    int ret = HTTPClientSetCredentials(session, (CHAR *)user_name, (CHAR *)password);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("set credentials failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}
#endif

#if CONFIG_WM_HTTP_CLIENT_PROXY
/***************************************************************************
* Function: wm_http_client_set_proxy
* Description: Sets all the proxy related parameters.
*
* Input: pSession: HTTP client handle.
*           pProxyName: The host name.
*           nPort:            The proxy port number.
*           pUserName:   User name for proxy authentication (can be null).
*           pPassword:    User password for proxy authentication (can be null).
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_set_proxy(wm_http_client_t session, char *proxy_name, UINT16 port, char *user_name, char *password)
{
    int ret = HTTPClientSetProxy(session, (CHAR *)proxy_name, port, (CHAR *)user_name, (CHAR *)password);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("set credentials failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}
#endif

/***************************************************************************
* Function: wm_http_client_set_verb
* Description: Sets the HTTP verb for the outgoing request.
*
* Input: pSession: HTTP client handle.
*           HttpVerb: HTTP supported verbs
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_set_verb(wm_http_client_t session, wm_http_client_request_type_t http_verb)
{
    int ret = HTTPClientSetVerb(session, (HTTP_VERB)http_verb);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("set credentials failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}
/***************************************************************************
* Function: wm_http_client_add_request_headers
* Description: Add headers to the outgoing request.
*
* Input: pSession: HTTP client handle.
*           pHeaderName:    The Headers name
*           pHeaderData:      The headers data
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_add_request_headers(wm_http_client_t session, char *header_name, char *header_data)
{
    if (!header_name) {
        wm_log_error("header_name is NULL");
        return WM_ERR_INVALID_PARAM;
    }

    int ret = HTTPClientAddRequestHeaders(session, (CHAR *)header_name, (CHAR *)header_data, (BOOL)0);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("add request headers failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_send_request
* Description: This function builds the request headers, performs a DNS resolution ,
*                 opens the connection (if it was not opened yet by a previous request or if it has closed)
*                 and sends the request headers.
*
* Input: pSession: HTTP client handle.
*           pData:             Data to post to the server
*           nDataLength:   Length of posted data
*           TotalLength:     Valid only when http method is post.
*                     TRUE:   Post data to http server.
*                     FALSE: In a post request without knowing the total length in advance so return error or use chunking.
*           nTimeout:        Operation timeout
*           nClientPort:      Client side port 0 for none
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_send_request(wm_http_client_t session, char *url, void *data, uint32_t data_len, bool data_len_valid,
                                uint32_t time_out, uint32_t client_port)
{
    int ret = HTTPClientSendRequest(session, url, data, data_len, (BOOL)data_len_valid, time_out, client_port);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("send request failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_write_data
* Description: Write data to the remote server.
*
* Input: pSession: HTTP client handle.
*           pBuffer:             Data to write to the server.
*           nBufferLength:   Length of wtitten data.
*           nTimeout:          Timeout for the operation.
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_write_data(wm_http_client_t session, void *buffer, uint32_t buffer_len, uint32_t time_out)
{
    int ret = HTTPClientWriteData(session, buffer, buffer_len, time_out);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("write data failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_recv_response
* Description: Receives the response header on the connection and parses it.
*                Performs any required authentication.
*
* Input: pSession: HTTP client handle.
*           nTimeout:          Timeout for the operation.
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_recv_response(wm_http_client_t session, uint32_t time_out)
{
    int ret = HTTPClientRecvResponse(session, time_out);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("write data failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_read_data
* Description: Read data from the server. Parse out the chunks data.
*
* Input: pSession: HTTP client handle.
*           nBytesToRead:    The size of the buffer (numbers of bytes to read)
*           nTimeout:           Operation timeout in seconds
*
* Output: pBuffer:              A pointer to a buffer that will be filled with the servers response
*             nBytesRecived:   Count of the bytes that were received in this operation
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_read_data(wm_http_client_t session, void *buffer, uint32_t bytes_to_read, uint32_t time_out,
                             uint32_t *bytes_recived)
{
    uint32_t ret;

    ret = HTTPClientReadData(session, buffer, bytes_to_read, time_out, bytes_recived);

    if (ret == HTTP_CLIENT_SUCCESS) {
        return WM_ERR_SUCCESS;
    } else if (ret == HTTP_CLIENT_EOS) {
        return WM_ERR_EOS;
    } else {
        wm_log_error("read data failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }
}

/***************************************************************************
* Function: wm_http_client_get_info
* Description: Fill the users structure with the session information.
*
* Input: pSession: HTTP client handle.
*
* Output: HTTPClient:   The session information.
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_get_info(wm_http_client_t session, wm_http_client_info_t *http_client)
{
    if (!session || !http_client) {
        wm_log_error("input error");
        return WM_ERR_INVALID_PARAM;
    }

    int ret = HTTPClientGetInfo(session, http_client);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("get info failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_find_first_header
* Description: Initiate the headr searching functions and find the first header.
*
* Input: pSession: HTTP client handle.
*           pSearchClue:   Search clue.
*
* Output: pHeaderBuffer: A pointer to a buffer that will be filled with the header name and value.
*             nLength:          Count of the bytes that were received in this operation.
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_find_first_header(wm_http_client_t session, char *search_clue, char *header_buffer, uint32_t *length)
{
    if (!session || !search_clue || !header_buffer || !length) {
        wm_log_error("input error");
        return WM_ERR_INVALID_PARAM;
    }

    int ret = HTTPClientFindFirstHeader(session, (CHAR *)search_clue, (CHAR *)header_buffer, length);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("find first header failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_get_next_header
* Description: Find the next header.
*
* Input: pSession: HTTP client handle.
*
* Output: pHeaderBuffer: A pointer to a buffer that will be filled with the header name and value.
*             nLength:          Count of the bytes that were received in this operation.
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_get_next_header(wm_http_client_t session, char *header_buffer, uint32_t *length)
{
    if (!session || !header_buffer || !length) {
        wm_log_error("input error");
        return WM_ERR_INVALID_PARAM;
    }

    int ret = HTTPClientGetNextHeader(session, (CHAR *)header_buffer, length);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("get next header failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

/***************************************************************************
* Function: wm_http_client_find_close_header
* Description: Terminate a headers search session.
*
* Input: session: HTTP client handle.
*
* Output: None
*
* Return: HTTP Status
*
* Date : 2014-6-6
****************************************************************************/
int wm_http_client_find_close_header(wm_http_client_t session)
{
    if (!session) {
        wm_log_error("input error");
        return WM_ERR_INVALID_PARAM;
    }

    int ret = HTTPClientFindCloseHeader(session);

    if (ret != HTTP_CLIENT_SUCCESS) {
        wm_log_error("find close header failed: %s", HTTP_error_code_2_string(ret));
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}
