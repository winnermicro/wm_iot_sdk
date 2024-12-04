
#ifndef _HTTP_CLIENT
#define _HTTP_CLIENT
#include "lwip/sockets.h"
#include "HTTPClientWrapper.h"
#include "HTTPClientCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTP API global definitions
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////
#ifdef _HTTP_BUILD_AMT
#define CMSI_HTTPCLIENT_PROTOCOL_GUID                                              \
    {                                                                              \
        0x471b2c0e, 0x6137, 0x4d55, 0x92, 0x36, 0xdd, 0x0f, 0xdb, 0xc2, 0x52, 0xfb \
    }
#endif

#ifdef BOOL
#undef BOOL
#endif
#ifdef UCHAR
#undef UCHAR
#endif
#ifdef CHAR
#undef CHAR
#endif
#ifdef UINT16
#undef UINT16
#endif
#ifdef INT16
#undef INT16
#endif
#ifdef UINT32
#undef UINT32
#endif
#ifdef INT32
#undef INT32
#endif
#ifdef UINT64
#undef UINT64
#endif
#ifdef INT64
#undef INT64
#endif
#ifdef ULONG
#undef ULONG
#endif
#ifdef LONG
#undef LONG
#endif
#define VOID void
typedef int BOOL;
typedef unsigned char UCHAR;
//typedef signed char             CHAR;
typedef char CHAR;
typedef unsigned short UINT16;
typedef signed short INT16;
typedef uint32_t UINT32;
typedef signed int INT32;
typedef unsigned long long UINT64;
typedef long long INT64;
typedef unsigned long ULONG;
typedef signed long LONG;

typedef UINT32 u32;
typedef UINT16 u16;
typedef unsigned char u8;
typedef long long s64;
typedef INT32 s32;
typedef INT16 s16;
typedef char s8;

#define TRUE                                      true
#define FALSE                                     false

/* HTTP Status, API Return codes */
/** HTTP Success status */
#define HTTP_CLIENT_SUCCESS                       0
/** Unknown error */
#define HTTP_CLIENT_UNKNOWN_ERROR                 1
/** an Invalid handle or possible bad pointer was passed to a function */
#define HTTP_CLIENT_ERROR_INVALID_HANDLE          2
/** Buffer too small or a failure while in memory allocation */
#define HTTP_CLIENT_ERROR_NO_MEMORY               3
/** an attempt to use an invalid socket handle was made */
#define HTTP_CLIENT_ERROR_SOCKET_INVALID          4
/** Can't send socket parameters */
#define HTTP_CLIENT_ERROR_SOCKET_CANT_SET         5
/** Error while resolving host name */
#define HTTP_CLIENT_ERROR_SOCKET_RESOLVE          6
/** Error while connecting to the remote server */
#define HTTP_CLIENT_ERROR_SOCKET_CONNECT          7
/** socket time out error */
#define HTTP_CLIENT_ERROR_SOCKET_TIME_OUT         8
/** Error while receiving data */
#define HTTP_CLIENT_ERROR_SOCKET_RECV             9
/** Error while sending data */
#define HTTP_CLIENT_ERROR_SOCKET_SEND             10
/** Error while receiving the remote HTTP headers */
#define HTTP_CLIENT_ERROR_HEADER_RECV             11
/** Could not find element within header */
#define HTTP_CLIENT_ERROR_HEADER_NOT_FOUND        12
/** The headers search clue was too large for the internal API buffer */
#define HTTP_CLIENT_ERROR_HEADER_BIG_CLUE         13
/** No content length was specified for the outgoing data. the caller should
        specify chunking mode in the session creation */
#define HTTP_CLIENT_ERROR_HEADER_NO_LENGTH        14
/** The HTTP chunk token that was received from the server was too big and possibly wrong */
#define HTTP_CLIENT_ERROR_CHUNK_TOO_BIG           15
/** Could not authenticate with the remote host */
#define HTTP_CLIENT_ERROR_AUTH_HOST               16
/** Could not authenticate with the remote proxy */
#define HTTP_CLIENT_ERROR_AUTH_PROXY              17
/** Bad or not supported HTTP verb was passed to a function */
#define HTTP_CLIENT_ERROR_BAD_VERB                18
/** a function received a parameter that was too large */
#define HTTP_CLIENT_ERROR_LONG_INPUT              19
/** The session state prevents the current function from proceeding */
#define HTTP_CLIENT_ERROR_BAD_STATE               20
/** Could not parse the chunk length while in chunked transfer */
#define HTTP_CLIENT_ERROR_CHUNK                   21
/** Could not parse curtail elements from the URL (such as the host name, HTTP prefix act') */
#define HTTP_CLIENT_ERROR_BAD_URL                 22
/** Could not detect key elements in the received headers */
#define HTTP_CLIENT_ERROR_BAD_HEADER              23
/** Error while attempting to resize a buffer */
#define HTTP_CLIENT_ERROR_BUFFER_RSIZE            24
/** Authentication schema is not supported */
#define HTTP_CLIENT_ERROR_BAD_AUTH                25
/** The selected authentication schema does not match the server response */
#define HTTP_CLIENT_ERROR_AUTH_MISMATCH           26
/** an element was missing while parsing the digest authentication challenge */
#define HTTP_CLIENT_ERROR_NO_DIGEST_TOKEN         27
/** Digest algorithem could be MD5 or MD5-sess other types are not supported */
#define HTTP_CLIENT_ERROR_NO_DIGEST_ALG           28
/** Binding error */
#define HTTP_CLIENT_ERROR_SOCKET_BIND             29
/** Tls negotiation error */
#define HTTP_CLIENT_ERROR_TLS_NEGO                30
/** Feature is not (yet) implemented */
#define HTTP_CLIENT_ERROR_NOT_IMPLEMENTED         64
/** Connection closed.       */
#define HTTP_CLIENT_ERROR_CONNECTION_CLOSE        500
/** HTTP end of stream message */
#define HTTP_CLIENT_EOS                           1000

// API Version
#define HTTP_CLIENT_VERSION_MINOR                 0
#define HTTP_CLIENT_VERSION_MAJOR                 1

// Global default sizes
#define HTTP_CLIENT_MAX_SEND_RECV_HEADERS         2048 // Maximum Send and receive buffers size
#define HTTP_CLIENT_INIT_SEND_RECV_HEADERS        2048 // If we can resize the buffers this would be the initial size

#define HTTP_CLIENT_MAX_USERNAME_LENGTH           16 // Maximum length the user name (host and proxy authentication)
#define HTTP_CLIENT_MAX_PASSWORD_LENGTH           16 // Maximum length  for the password
// Maximum length for the base 64 encoded credentials (twice the size of the user name and password max parameters)
#define HTTP_CLIENT_MAX_64_ENCODED_CRED           ((HTTP_CLIENT_MAX_USERNAME_LENGTH + HTTP_CLIENT_MAX_PASSWORD_LENGTH) * 2) + 4
#define HTTP_CLIENT_MAX_CHUNK_HEADER              64   // Maximum length for the received chunk header (hex - string) size
#define HTTP_CLIENT_MAX_PROXY_HOST_LENGTH         64   // Maximum length for the proxy host name
#define HTTP_CLIENT_MAX_TOKEN_LENGTH              512  // Maximum length for an HTTP token data (authentication header elements)
#define HTTP_CLIENT_MAX_TOKEN_NAME_LENGTH         32   // Maximum length for an HTTP authorization token name ("qop")
#define HTTP_CLIENT_MAX_HEADER_SEARCH_CLUE        1024 // Maximum length for a search clue string (Headers searching)
#define HTTP_CLIENT_ALLOW_HEAD_VERB               1    // Can we use the HTTP HEAD verb in our outgoing requests?

#define HTTP_CLIENT_MEMORY_RESIZABLE              false // Permission to dynamically resize the headers buffer
#define HTTP_CLIENT_MEMORY_RESIZE_FACTOR          16    // Factor for memory resizing operation

#define HTTP_CLIENT_DEFAULT_PORT                  80         // Default HTTP port
#define HTTP_CLIENT_DEFAULT_SSL_PORT              443        // Default HTTPS port
#define HTTP_CLIENT_DEFAULT_VERB                  0          // GET
#define HTTP_CLIENT_DEFAULT_VER                   "HTTP/1.1" // We will send this in the outgoing header
#define HTTP_CLIENT_DEFAULT_PROXY_VER             "HTTP/1.0" // We will send this in the outgoing header (proxy)
#define HTTP_CLIENT_DEFAULT_AGENT                 "Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)"
#define HTTP_CLIENT_DEFAULT_TIMEOUT               30000           // Default timeout in mseconds
#define HTTP_CLIENT_DEFAULT_KEEP_ALIVE            30              // Default Keep-alive value in seconds
#define HTTP_CLIENT_DEFAULT_DIGEST_AUTH           "MD5"           // This is for bypassing a known bug in AMT05..
#define HTTP_CLIENT_DEFAULT_PROXY_AUTH            AuthSchemaBasic // Basic

#define HTTP_CLIENT_CRLF                          "\r\n"     // End of line macro
#define HTTP_CLIENT_CRLFX2                        "\r\n\r\n" // Double End of line macro

// HTTP Session internal API flags
// Note: Not intended to be set the by the API user
#define HTTP_CLIENT_FLAG_SECURE                   0x00000010 // The session is secured using TLS
#define HTTP_CLIENT_FLAG_URLANDPORT               0x00000020 // Url has a port within
#define HTTP_CLIENT_FLAG_URLHTTPS                 0x00000040 // Url has a https prefix
#define HTTP_CLIENT_FLAG_USINGPROXY               0x00000080 // Operation will be performed using a proxy server
#define HTTP_CLIENT_FLAG_CHUNKED                  0x00000100 // The incoming data is chunked

// HTTP Status codes
#define HTTP_STATUS_OK                            200 // The request has succeeded
#define HTTP_STATUS_UNAUTHORIZED                  401 // The request requires user authentic
#define HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED 407 // The client must first authenticate itself with the proxy

// Redirection (Note: there are more 30x codes, those are the most popular)
#define HTTP_STATUS_OBJECT_MOVED                  302 // Page redirection notification
#define HTTP_STATUS_OBJECT_MOVED_PERMANENTLY      301 // Page redirection notification
#define HTTP_STATUS_CONTINUE                      100 // Page continue message

/******************************************************************************
*
*  Section      : HTTP API structures
*
******************************************************************************/

/* HTTP Type Definitions */

/** HTTP Supported authentication methods */
typedef enum _HTTP_AUTH_SCHEMA {
    AuthSchemaNone = 0,
    AuthSchemaBasic,
    AuthSchemaDigest,
    AuthSchemaKerberos,
    AuthNotSupported

} HTTP_AUTH_SCHEMA;
/** HTTP supported verbs */
typedef enum _HTTP_VERB {
    VerbGet = 0,
    VerbHead,
    VerbPost,
    VerbPut,
    VerbFwup,
    VerbDele,
    VerbNotSupported
    // Note: others verb such as connect and put are currently not supported

} HTTP_VERB;
/** Data structure that the caller can request at any time that will include
    some information regarding the session */
typedef struct _HTTP_CLIENT {
    UINT32 HTTPStatusCode;             // HTTP Status code (200 OK)
    UINT32 RequestBodyLengthSent;      // Total bytes sent (body only)
    UINT32 ResponseBodyLengthReceived; // Total bytes received (body only)
    UINT32 TotalResponseBodyLength;    // as extracted from the “content-length" header
    UINT32 HttpState;
} HTTP_CLIENT;

/** HTTP parameters */
typedef struct _HTTPParameters {
    CHAR *Uri;
    CHAR *ProxyHost;
    UINT32 UseProxy;
    UINT32 ProxyPort;
    UINT32 Verbose;
    CHAR *UserName;
    CHAR *Password;
    HTTP_AUTH_SCHEMA AuthType;

} HTTPParameters;

// MIN AMX macro
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
// HTTP timeout macro for selecting the default value if the caller passed 0 (no timeout) to the function
//#define HTTP_TIMEOUT(nTimeout) (((nTimeout) > (0)) ? (nTimeout * HZ) : (HTTP_CLIENT_DEFAULT_TIMEOUT * HZ))
#define HTTP_TIMEOUT(nTimeout) (((nTimeout) > (0)) ? (nTimeout) : (HTTP_CLIENT_DEFAULT_TIMEOUT))

// 32 bit alignment macro
#define ALIGN(size)            ((size & 0xfffffffc) + ((size & 3) ? 4 : 0))

#if CONFIG_WM_HTTP_CLIENT_DEBUG
typedef void (*E_HTTPDebug)(const char *, const char *, uint32_t, char *, ...); // HTTPDebug hook function
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTP API internals structures
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

// Generic parameter structure contains a pointer to the buffer and its length

typedef struct _HTTP_PARAM {
    char *pParam;
    uint32_t nLength;

} HTTP_PARAM;

// HTTP socket events
typedef struct _HTTP_CONNECTION {
    fd_set FDRead;           // socket read event
    fd_set FDWrite;          // socket write event
    fd_set FDError;          // socket error event
    int32_t HttpSocket;      // The underling socket
    uint32_t HttpStartTime;  // Time stamp for the session
    uint32_t HttpClientPort; // For client side binding
    bool TlsNego;            // TLS negotiation flag

} HTTP_CONNECTION;

// Request URL
typedef struct _HTTP_URL {
    HTTP_PARAM UrlBsee;    // a pointer and length to the "http" section of the URL
    HTTP_PARAM UrlHost;    // a pointer and length to the host section of the URL
    HTTP_PARAM UrlPort;    // a pointer and length to the PORT (if was specified section)
    HTTP_PARAM UrlRequest; // a pointer and length of the request section of the URL
    uint16_t nPort;        // the PORT that we should use (could be default or the one found within the URL)
    char *Url;             // a buffer for the URL

} HTTP_URL;
// HTTP headers (incoming and outgoing)
typedef struct _HTTP_HEADERS {
    HTTP_PARAM HeadersBuffer; // a pointer and length of the complete Headers (in\out) buffer
    HTTP_PARAM HeadersOut;    // a pointer and length of the outgoing HTTP headers
    HTTP_PARAM HeadersIn;     // a pointer and length of the incoming headers
    HTTP_PARAM HeaderSearch;  // Index and pointer for the header search functions
    HTTP_VERB HttpVerb;       // the HTTP verb that was used in the session
    HTTP_VERB HttpLastVerb;   // the HTTP verb that was last transmited to the server
    char *SearchClue;
    char Verb[16]; // the actual string buffer of the HTTP verb

} HTTP_HEADERS;

// HTTP headers (parsed headers information)
typedef struct _HTTP_HEADERS_INFO {
    HTTP_PARAM HttpRedirectURL;      // Stores the redirection URL if we got a 301 or 303 return code
    uint32_t nHTTPStatus;            // the HTTP status code (200 401 407 act')
    uint32_t nHTTPContentLength;     // the Content length if specified of the returned data
    uint32_t nHTTPPostContentLength; // the Content-Length of the POSTed data (if known)
    bool Connection;                 // True = Keep alive or undefined, False = Closed
    bool ValidHeaders;               // a flag that indicates if the incoming header ware parsed OK and found to be valid
    bool HaveCredentials;            // a flag that indicates if we have credentials for the session
    char HTTPVersion[16];            // HTTP version string buffer (for example: "HTTP 1.1")

} HTTP_HEADERS_INFO;

// Authentication parameters that ware extracted from the incoming headers
typedef struct _HTTP_AUTH_HEADER {
    HTTP_PARAM AuthHeader;     // the pointer and length of the authentication header
    uint32_t HTTP_AUTH_SCHEMA; // Its schema (could be any of the supported)

} HTTP_AUTH_HEADER;
// Proxy related data
typedef struct _HTTP_PROXY {
    char *ProxyHost;
    char *ProxtUser;
    char *ProxyPassword;
    uint16_t nProxyPort;
    char AuthSchemaName[16]; // The authentication schema name (for string comperission)
    HTTP_AUTH_SCHEMA ProxyAuthSchema;

} HTTP_PROXY;

// HTTP User credentials
typedef struct _HTTP_CREDENTIALS {
    char *CredUser;
    char *CredPassword;
    char AuthSchemaName[16];         // The authentication schema name (for string comperission)
    HTTP_AUTH_SCHEMA CredAuthSchema; // The schema that calle has selected for the session
    bool Authentication;             // a flag that indicates that this session has requested a user authentication

} HTTP_CREDENTIALS;
// HTTP Counters
typedef struct _HTTP_COUNTERS {
    uint32_t nRecivedHeaderLength; // Bytes count of the incoming header
    uint32_t nRecivedBodyLength;   // Bytes count of the incoming body length
    uint32_t nRecivedChunkLength;  // The next chunk length in bytes
    uint32_t nBytesToNextChunk;    // How many bytes we have to read until we can expect the next chunk
    uint32_t nActionStartTime;     // Operation start time
    uint32_t nActionTimeout;       // Timeout for the session
    uint32_t nSentChunks;          // Count of sent chunks
    uint32_t nSentBodyBytes;       // Count of body bytes that ware sent
    uint32_t nSentHeaderBytes;     // Count of header bytes thhat ware sent

} HTTP_COUNTERS;

// HTTP Client Session data
typedef struct _HTTP_REQUEST {
    HTTP_URL HttpUrl;
    HTTP_HEADERS HttpHeaders;
    HTTP_HEADERS_INFO HttpHeadersInfo;
    HTTP_AUTH_HEADER HttpAuthHeader;
    HTTP_PROXY HttpProxy;
    HTTP_CREDENTIALS HttpCredentials;
    HTTP_CONNECTION HttpConnection;
    HTTP_COUNTERS HttpCounters;
    uint32_t HttpState;
    uint32_t HttpFlags;
#if CONFIG_WM_HTTP_CLIENT_DEBUG
    E_HTTPDebug pDebug;
#endif
#if CONFIG_WM_HTTP_CLIENT_SECURE
    tls_ssl_t *ssl;
    int ssl_more_data;
#endif
    void *http_event_handle;
    void *event_priv;
} HTTP_SESSION, *P_HTTP_SESSION;

/***********move to wm_http_client.h**************
    // HTTP Type Definitions
    typedef uint32_t          HTTP_SESSION_HANDLE;
    typedef uint32_t          HTTP_CLIENT_SESSION_FLAGS;
    */
typedef UINT32 HTTP_SESSION_HANDLE;
typedef UINT32 HTTP_CLIENT_SESSION_FLAGS;

///////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTP API public interface
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

/***********move to wm_http_client.h**************
    HTTP_SESSION_HANDLE     HTTPClientOpenRequest         (HTTP_CLIENT_SESSION_FLAGS Flags);
    uint32_t                  HTTPClientCloseRequest        (HTTP_SESSION_HANDLE *pSession);
    uint32_t                  HTTPClientSetAuth             (HTTP_SESSION_HANDLE pSession, HTTP_AUTH_SCHEMA AuthSchema, void *pReserved);
    uint32_t                  HTTPClientSetCredentials      (HTTP_SESSION_HANDLE pSession, char *pUserName, char *pPassword);
    uint32_t                  HTTPClientSetProxy            (HTTP_SESSION_HANDLE pSession, char *pProxyName, uint16_t nPort, char *pUserName, char *pPassword);
    uint32_t                  HTTPClientSetVerb             (HTTP_SESSION_HANDLE pSession, HTTP_VERB HttpVerb);
    uint32_t                  HTTPClientAddRequestHeaders   (HTTP_SESSION_HANDLE pSession, char *pHeaderName, char *pHeaderData, bool nInsert);
    uint32_t                  HTTPClientSendRequest         (HTTP_SESSION_HANDLE pSession, char *pUrl, VOID *pData, uint32_t nDataLength, bool TotalLength, uint32_t nTimeout,uint32_t nClientPort);
    uint32_t                  HTTPClientWriteData           (HTTP_SESSION_HANDLE pSession, VOID *pBuffer, uint32_t nBufferLength, uint32_t nTimeout);
    uint32_t                  HTTPClientRecvResponse        (HTTP_SESSION_HANDLE pSession, uint32_t nTimeout);
    uint32_t                  HTTPClientReadData            (HTTP_SESSION_HANDLE pSession, VOID *pBuffer, uint32_t nBytesToRead, uint32_t nTimeout, uint32_t *nBytesRecived);
    uint32_t                  HTTPClientGetInfo             (HTTP_SESSION_HANDLE pSession, HTTP_CLIENT *HTTPClient);

    uint32_t                  HTTPClientFindFirstHeader     (HTTP_SESSION_HANDLE pSession, char *pSearchClue,char *pHeaderBuffer, uint32_t *nLength);
    uint32_t                  HTTPClientGetNextHeader       (HTTP_SESSION_HANDLE pSession, char *pHeaderBuffer, uint32_t *nLength);
    uint32_t                  HTTPClientFindCloseHeader     (HTTP_SESSION_HANDLE pSession);
    */
uint32_t HTTPClientSetLocalConnection(HTTP_SESSION_HANDLE pSession, uint32_t nPort);
#if CONFIG_WM_HTTP_CLIENT_DEBUG
uint32_t HTTPClientSetDebugHook(HTTP_SESSION_HANDLE pSession, E_HTTPDebug pDebug);
#endif

/**
     * @defgroup APP_APIs APP APIs
     * @brief APP APIs
     */

/**
     * @addtogroup APP_APIs
     * @{
     */

/**
     * @defgroup HTTPC_APIs HTTPC APIs
     * @brief HTTP client APIs
     */

/**
     * @addtogroup HTTPC_APIs
     * @{
     */

/******************************************************************************
    *
    *  Section      : HTTP API public interface
    *
    ******************************************************************************/

/**
     * @brief          Allocate memory for a new HTTP Session
     *
     * @param[in]      Flags    HTTP Session internal API flags, 0 should be passed here
     *
     * @retval         0        failed
     * @retval         other    HTTP Session handle
     *
     * @note           None
     */
HTTP_SESSION_HANDLE HTTPClientOpenRequest(HTTP_CLIENT_SESSION_FLAGS Flags);

/**
     * @brief          Allocate memory for a new HTTP Session
     *
     * @param[in]      Flags    HTTP Session internal API flags, 0 should be passed here
     * @param[in]      event_cb User register handle
     * @param[in]      priv     User private data
     *
     * @retval         0        failed
     * @retval         other    HTTP Session handle
     *
     * @note           None
     */
HTTP_SESSION_HANDLE HTTPClientOpenRequestWithCb(HTTP_CLIENT_SESSION_FLAGS Flags, void *http_event_handle, void *priv);

/**
     * @brief          Closes the active connection and free the corresponding memory
     *
     * @param[in]      *pSession                HTTP Session handle
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientCloseRequest(HTTP_SESSION_HANDLE *pSession);
/**
     * @brief          Sets the HTTP authentication schema
     *
     * @param[in]      pSession                 HTTP Session handle
     * @param[in]      AuthSchema             HTTP Supported authentication methods
     * @param[in]      *pReserved               Reserved parameter
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientSetAuth(HTTP_SESSION_HANDLE pSession, HTTP_AUTH_SCHEMA AuthSchema, void *pReserved);
/**
     * @brief          Sets credentials for the target host
     *
     * @param[in]      pSession                 HTTP Session handle
     * @param[in]      *pUserName              User name
     * @param[in]      *pPassword               Password
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientSetCredentials(HTTP_SESSION_HANDLE pSession, CHAR *pUserName, CHAR *pPassword);
/**
     * @brief          Sets all the proxy related parameters
     *
     * @param[in]      pSession        HTTP Session handle
     * @param[in]      *pProxyName    The host name
     * @param[in]      nPort           The proxy port number
     * @param[in]      *pUserName     User name for proxy authentication (can be null)
     * @param[in]      *pPassword      User password for proxy authentication (can be null)
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientSetProxy(HTTP_SESSION_HANDLE pSession, CHAR *pProxyName, UINT16 nPort, CHAR *pUserName, CHAR *pPassword);
/**
     * @brief          Sets the HTTP verb for the outgoing request
     *
     * @param[in]      pSession                 HTTP Session handle
     * @param[in]      HttpVerb               HTTP supported verbs
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientSetVerb(HTTP_SESSION_HANDLE pSession, HTTP_VERB HttpVerb);
/**
     * @brief          Add headers into the outgoing request
     *
     * @param[in]      pSession                 HTTP Session
     * @param[in]      *pHeaderName            The Header name
     * @param[in]      *pHeaderData            The header data
     * @param[in]      nInsert                  Reserved, could be any
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientAddRequestHeaders(HTTP_SESSION_HANDLE pSession, CHAR *pHeaderName, CHAR *pHeaderData, BOOL nInsert);
/**
     * @brief          This function builds the request headers, performs a DNS resolution,
     *                 opens the connection (if it was not opened yet by a previous request
     *                 or if it has closed) and sends the request headers
     *
     * @param[in]      pSession      HTTP Session handle
     * @param[in]      *pUrl         The requested URL
     * @param[in]      *pData        Data to post to the server
     * @param[in]      nDataLength     Length of posted data
     * @param[in]      TotalLength    Valid only when http method is post
     *                              TRUE:  Post data to http server.
     *                              FALSE: In a post request without knowing the total
     *                                     length in advance so return error or use chunking.
     * @param[in]      nTimeout     Operation timeout
     * @param[in]      nClientPort  Client side port 0 for none
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientSendRequest(HTTP_SESSION_HANDLE pSession, CHAR *pUrl, VOID *pData, UINT32 nDataLength, BOOL TotalLength,
                             UINT32 nTimeout, UINT32 nClientPort);
/**
     * @brief          Write data to the remote server
     *
     * @param[in]      pSession                 HTTP Session handle
     * @param[in]      *pBuffer                 Data to write to the server
     * @param[in]      nBufferLength              Length of wtitten data
     * @param[in]      nTimeout                Timeout for the operation
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientWriteData(HTTP_SESSION_HANDLE pSession, VOID *pBuffer, UINT32 nBufferLength, UINT32 nTimeout);
/**
     * @brief          Receives the response header on the connection and parses it.
     *                 Performs any required authentication.
     *
     * @param[in]      pSession                 HTTP Session handle
     * @param[in]      nTimeout                Timeout for the operation
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientRecvResponse(HTTP_SESSION_HANDLE pSession, UINT32 nTimeout);
/**
     * @brief          Read data from the server. Parse out the chunks data
     *
     * @param[in]      pSession           HTTP Session handle
     * @param[out]     *pBuffer           A pointer to a buffer that will be filled with the servers response
     * @param[in]      nBytesToRead     The size of the buffer (numbers of bytes to read)
     * @param[in]      nTimeout          Operation timeout in mseconds
     * @param[out]     *nBytesRecived    Count of the bytes that were received in this operation
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientReadData(HTTP_SESSION_HANDLE pSession, VOID *pBuffer, UINT32 nBytesToRead, UINT32 nTimeout,
                          UINT32 *nBytesRecived);
/**
     * @brief          Fill the users structure with the session information
     *
     * @param[in]      pSession                 HTTP Session handle
     * @param[out]     *HTTPClient            The session information
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientGetInfo(HTTP_SESSION_HANDLE pSession, HTTP_CLIENT *HTTPClient);
/**
     * @brief          Initiate the headr searching functions and find the first header
     *
     * @param[in]      pSession           HTTP Session handle
     * @param[in]      *pSearchClue      Search clue
     * @param[out]     *pHeaderBuffer    A pointer to a buffer that will be filled with the header name and value
     * @param[out]     *nLength           Count of the bytes that were received in this operation
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientFindFirstHeader(HTTP_SESSION_HANDLE pSession, CHAR *pSearchClue, CHAR *pHeaderBuffer, UINT32 *nLength);
/**
     * @brief          Find the next header.
     *
     * @param[in]      pSession           HTTP Session handle
     * @param[out]     *pHeaderBuffer    A pointer to a buffer that will be filled with the header name and value
     * @param[out]     *nLength           Count of the bytes that were received in this operation
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientGetNextHeader(HTTP_SESSION_HANDLE pSession, CHAR *pHeaderBuffer, UINT32 *nLength);
/**
     * @brief          Terminate a headers search session
     *
     * @param[in]      pSession                 HTTP Session handle
     *
     * @retval         HTTP_CLIENT_SUCCESS     success
     * @retval         other                   failed
     *
     * @note           None
     */
UINT32 HTTPClientFindCloseHeader(HTTP_SESSION_HANDLE pSession);

// Debug hook
// #define  _HTTP_DEBUGGING_ CONFIG_WM_HTTP_CLIENT_DEBUG                   // Simply dumps more debugging data to the console

///////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTP API private function
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPIntrnConnectionClose(P_HTTP_SESSION pHTTPSession);

/**
     * @brief          Error code to string conversion
     *
     * @param[in]      error_code                 HTTP error code
     *
     * @retval         Reason for error
     *
     * @note           None
     */
const char *HTTP_error_code_2_string(int error_code);

#ifdef __cplusplus
}
#endif

#endif //_HTTP_CLIENT
