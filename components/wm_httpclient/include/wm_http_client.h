/**
 * @file wm_http_client.h
 *
 * @brief WM_HTTP_CLIENT Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __WM_HTTP_CLIENT_H__
#define __WM_HTTP_CLIENT_H__

#include "HTTPClient.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HTTP_CLIENT_Macros WM HTTP_CLIENT Macros
 * @brief WinnerMicro HTTP_CLIENT Macros
 */

/**
 * @addtogroup WM_HTTP_CLIENT_Macros
 * @{
 */

#define WM_HTTP_CLIENT_INVALID 0 /**< invalid http client handle. */

/**
 * @}
 */

/**
 * @defgroup WM_HTTP_CLIENT_Type_Definitions WM HTTP_CLIENT Type Definitions
 * @brief WinnerMicro HTTP_CLIENT Type Definitions
 */

/**
 * @addtogroup WM_HTTP_CLIENT_Type_Definitions
 * @{
 */

/**
 * @brief Type definition for an HTTP session handle.
 *
 * This type is used to represent a handle to an HTTP session.
 */
typedef HTTP_SESSION_HANDLE wm_http_client_t;

/**
 * @brief Type definition for HTTP client information.
 *
 * This type is used to store information related to the HTTP client.
 */
typedef HTTP_CLIENT wm_http_client_info_t;

/**
 * @}
 */

/**
 * @defgroup WM_HTTP_CLIENT_Enumerations WM HTTP_CLIENT Enumerations
 * @brief WinnerMicro HTTP_CLIENT Enumerations
 */

/**
 * @addtogroup WM_HTTP_CLIENT_Enumerations
 * @{
 */

/**
 * @brief Enumeration of HTTP authentication schemes.
 *
 * This enumeration lists the various authentication schemes that can be used with the HTTP client.
 */
typedef enum wm_http_auth_achema {
    WM_HTTP_CLIENT_AUTH_NONE     = AuthSchemaNone,     /**< HTTP not use authentication. */
    WM_HTTP_CLIENT_AUTH_BASIC    = AuthSchemaBasic,    /**< HTTP use basic authentication. */
    WM_HTTP_CLIENT_AUTH_DIGEST   = AuthSchemaDigest,   /**< HTTP use digest authentication. */
    WM_HTTP_CLIENT_AUTH_KERBEROS = AuthSchemaKerberos, /**< HTTP use kerberos authentication. */
    WM_HTTP_CLIENT_AUTH_MAX      = AuthNotSupported    /**< HTTP not support authentication. */
} wm_http_client_auth_schema_t;

/**
 * @brief Enumeration of HTTP client request types.
 *
 * This enumeration lists the different types of HTTP requests that can be made by the client.
 */
typedef enum {
    WM_HTTP_CLIENT_REQUEST_TYPE_GET    = VerbGet,  /**< HTTP Client GET request. */
    WM_HTTP_CLIENT_REQUEST_TYPE_HEAD   = VerbHead, /**< HTTP Client HEAD request. */
    WM_HTTP_CLIENT_REQUEST_TYPE_POST   = VerbPost, /**< HTTP Client POST request. */
    WM_HTTP_CLIENT_REQUEST_TYPE_PUT    = VerbPut,  /**< HTTP Client PUT request. */
    WM_HTTP_CLIENT_REQUEST_TYPE_DELETE = VerbDele, /**< HTTP Client DELETE request. */
} wm_http_client_request_type_t;

/**
 * @brief Enumeration of HTTP client content types.
 *
 * This enumeration defines the various content types that can be specified for HTTP requests.
 */
typedef enum {
    WM_HTTP_CLIENT_CONTENT_NONE                   = 0, /**< not set content type header. */
    WM_HTTP_CLIENT_CONTENT_APPLICATION_URLENCODED = 1, /**< application/x-www-form-urlencoded. */
    WM_HTTP_CLIENT_CONTENT_APPLICATION_JSON       = 2, /**< application/json. */
    WM_HTTP_CLIENT_CONTENT_MULTIPART_FORM_DATA    = 3, /**< multipart/form-data. */
    WM_HTTP_CLIENT_CONTENT_TEXT_XML               = 4, /**< text/xml. */
    WM_HTTP_CLIENT_CONTENT_TYPE_MAX, /**< max content types. If you want to add an undefined type, you can use max and then add the header yourself */
} wm_http_client_content_type_t;

/**
 * @brief Enumeration of HTTP client events.
 *
 * This enumeration lists the events that can occur within the HTTP client.
 */
typedef enum {
    WM_HTTP_CLIENT_EVENT_CONNECTED,     /**< Event indicating a connection has been established. */
    WM_HTTP_CLIENT_EVENT_DISCONNECTED,  /**< Event indicating a disconnection. */
    WM_HTTP_CLIENT_EVENT_HEADER_SENTED, /**< Event indicating headers have been sent. */
    WM_HTTP_CLIENT_EVENT_RECV_HEADER,   /**< Event indicating headers have been received. */
    WM_HTTP_CLIENT_EVENT_RECV_DATA,     /**< Event indicating data has been received. */
    WM_HTTP_CLIENT_EVENT_FINISH,        /**< Event indicating the end of a transaction. */
} wm_http_client_event_t;

/**
 * @}
 */

/**
 * @defgroup WM_HTTP_CLIENT_Structures WM HTTP_CLIENT Structures
 * @brief WinnerMicro HTTP_CLIENT Structures
 */

/**
 * @addtogroup WM_HTTP_CLIENT_Structures
 * @{
 */

/**
 * @brief Structure for HTTP client event parameters.
 *
 * This structure is used to pass parameters related to HTTP client events.
 */
typedef struct wm_http_client_event_param {
    void *data;        /**< Pointer to the data buffer. */
    uint32_t data_len; /**< Length of the data in the buffer. */
} wm_http_client_event_param_t;

/**
 * @}
 */

/**
 * @defgroup WM_HTTP_CLIENT_Type_Definitions WM HTTP_CLIENT Type Definitions
 * @brief WinnerMicro HTTP_CLIENT Type Definitions
 */

/**
 * @addtogroup WM_HTTP_CLIENT_Type_Definitions
 * @{
 */

/**
 * @brief Type definition for an HTTP client event handler function.
 *
 * This type defines a function pointer that can be used as an event handler for the HTTP client.
 * It is called when various events occur within the HTTP client.
 */
typedef void (*wm_http_client_event_handle)(wm_http_client_t session, wm_http_client_event_t event,
                                            wm_http_client_event_param_t *data, void *priv);

/**
 * @}
 */

/**
 * @defgroup WM_HTTP_CLIENT_Structures WM HTTP_CLIENT Structures
 * @brief WinnerMicro HTTP_CLIENT Structures
 */

/**
 * @addtogroup WM_HTTP_CLIENT_Structures
 * @{
 */

/**
 * @brief Structure for HTTP client configuration.
 *
 * This structure is used to configure the settings for an HTTP client session.
 */
typedef struct wm_http_client_config {
    char *username;                         /**< User name of the HTTP session. */
    char *password;                         /**< Password of the HTTP session. */
    wm_http_client_auth_schema_t auth_type; /**< HTTP Supported authentication methods. */

    char *user_agent; /**< The User Agent string to send with HTTP requests. */

    wm_http_client_request_type_t method; /**< Request type of the HTTP client. */

    wm_http_client_content_type_t content_type; /**< Format of HTTP data. */

    bool keep_alive;     /**< Set the keep alive header. */
    bool no_cache;       /**< Set the no cache header. */
    bool chunked;        /**< Set the chunk header. */
    bool use_proxy;      /**< Whether to use the HTTP proxy. */
    uint16_t proxy_port; /**< HTTP proxy port name. */
    char *proxy_host;    /**< HTTP proxy host name. */

    wm_http_client_event_handle event_handler; /**< client event handler. */
    void *priv;                                /**< This is an optional temporary parameter. */
} wm_http_client_config_t;

/**
 * @}
 */

/**
 * @defgroup WM_HTTP_CLIENT_Functions WM HTTP_CLIENT Functions
 * @brief WinnerMicro HTTP_CLIENT Functions
 */

/**
 * @addtogroup WM_HTTP_CLIENT_Functions
 * @{
 */

/**
 * @brief Allocate memory for a new HTTP client.
 *
 * @param client_config HTTP client config.
 *
 * @return
 *    - WM_HTTP_CLIENT_INVALID: failed
 *    - others: succeed
 */
wm_http_client_t wm_http_client_open_request(const wm_http_client_config_t *client_config);

/**
 * @brief Closes any active connection and free any used memory.
 *
 * @param session HTTP client handle.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_close_request(wm_http_client_t *session);

/**
 * @brief Set the HTTP client to tell the browser that it does not cache directly,
 *        but must request it from the server.
 *
 * @param session HTTP client handle.
 * @param no_cache TRUE: enable no_cache header.
 *                 FALSE: disable no_cache header.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_set_no_cache(wm_http_client_t session, bool no_cache);

/**
 * @brief Set the keepalive on the HTTP client.
 *
 * @param session HTTP client handle.
 * @param keepalive TRUE: enable keepalive header.
 *                  FALSE: disable keepalive header.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_set_keepalive(wm_http_client_t session, bool keepalive);

/**
 * @brief Sets the HTTP authentication schema.
 *
 * @param session HTTP client handle.
 * @param auth_schema HTTP Supported authentication methods.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_set_auth(wm_http_client_t session, wm_http_client_auth_schema_t auth_schema);

/**
 * @brief Sets credentials for the target host.
 *
 * @param session HTTP client handle.
 * @param user_name User name.
 * @param password Password.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_set_credentials(wm_http_client_t session, char *user_name, char *password);

/**
 * @brief Sets all the proxy related parameters.
 *
 * @param session HTTP client handle.
 * @param proxy_name The host name.
 * @param port The proxy port number.
 * @param user_name User name for proxy authentication (can be null).
 * @param password User password for proxy authentication (can be null).
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_set_proxy(wm_http_client_t session, char *proxy_name, uint16_t port, char *user_name, char *password);

/**
 * @brief Sets the HTTP verb for the outgoing request.
 *
 * @param session HTTP client handle.
 * @param http_verb HTTP supported verbs.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_set_verb(wm_http_client_t session, wm_http_client_request_type_t http_verb);

/**
 * @brief Add headers to the outgoing request.
 *
 * @param session HTTP client handle.
 * @param header_name The Headers name.
 * @param header_data The headers data. It can be NULL, if it is empty,
 *                    the header simply uses header_name without adding "\\r\\n" to the end.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_add_request_headers(wm_http_client_t session, char *header_name, char *header_data);

/**
 * @brief This function builds the request headers, performs a DNS resolution ,
 *        opens the connection (if it was not opened yet by a previous request or if it has closed)
 *        and sends the request headers.
 *
 * @param session HTTP client handle.
 * @param url The requested URL.
 * @param data Data to post to the server.
 * @param data_len Length of posted data.
 * @param data_len_valid  Valid only when HTTP method is post.
 *                   TRUE: data_len is valid when post data to HTTP server.
 *                   FALSE: data_len is invalid, in a post request without knowing the total length in advance so return error or use chunking.
 * @param time_out Operation timeout, the unit is milliseconds. A value of 0 defaults to 30s.
 * @param client_port Client side port 0 for none (bind local port).
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_send_request(wm_http_client_t session, char *url, void *data, uint32_t data_len, bool data_len_valid,
                                uint32_t time_out, uint32_t client_port);

/**
 * @brief Write data to the remote server.
 *
 * @param session HTTP client handle.
 * @param buffer Data to write to the server.
 * @param buffer_len Length of wtitten data.
 * @param time_out Timeout for the operation, the unit is milliseconds. A value of 0 defaults to 30s.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_write_data(wm_http_client_t session, void *buffer, uint32_t buffer_len, uint32_t time_out);

/**
 * @brief Receives the response header on the connection and parses it.
 *        Performs any required authentication.
 *
 * @param session HTTP client handle.
 * @param time_out Timeout for the operation, the unit is milliseconds.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_recv_response(wm_http_client_t session, uint32_t time_out);

/**
 * @brief Read data from the server. Parse out the chunks data.
 *
 * @param[in]  session HTTP client handle.
 * @param[out] buffer A pointer to a buffer that will be filled with the servers response.
 * @param[in]  bytes_to_read The size of the buffer (numbers of bytes to read).
 * @param[in]  time_out Timeout for the operation, the unit is milliseconds. A value of 0 defaults to 30s.
 * @param[out] bytes_recived Count of the bytes that were received in this operation.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_EOS: end of stream
 *    - others: failed
 */
int wm_http_client_read_data(wm_http_client_t session, void *buffer, uint32_t bytes_to_read, uint32_t time_out,
                             uint32_t *bytes_recived);

/**
 * @brief Fill the users structure with the session information.
 *
 * @param[in]  session HTTP client handle.
 * @param[out] http_client The session information.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_get_info(wm_http_client_t session, wm_http_client_info_t *http_client);

/**
 * @brief Initiate the headr searching functions and find the first header.
 *
 * @param[in]  session HTTP client handle.
 * @param[in]  search_clue Search clue.
 * @param[out] header_buffer A pointer to a buffer that will be filled with the header name and value.
 * @param[out] length Count of the bytes that were received in this operation.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_find_first_header(wm_http_client_t session, char *search_clue, char *header_buffer, uint32_t *length);

/**
 * @brief Find the next header.
 *
 * @param[in]  session HTTP client handle.
 * @param[out] header_buffer A pointer to a buffer that will be filled with the header name and value.
 * @param[out] length Count of the bytes that were received in this operation.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_get_next_header(wm_http_client_t session, char *header_buffer, uint32_t *length);

/**
 * @brief Terminate a headers search session.
 *
 * @param session HTTP client handle.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_http_client_find_close_header(wm_http_client_t session);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HTTP_CLIENT_H__ */
