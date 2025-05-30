
///////////////////////////////////////////////////////////////////////////////
//
// Module Name:
//   HTTPClient.c
//
// Abstract: Partially Implements the client side of the HTTP 1.1 Protocol as
//           Defined in RFC 2616,2617
// Platform: Any that supports standard C calls and Berkeley sockets
//
// Author:	Eitan Michaelson
// Version:  1.0
// Date      7/1/2006
// Opens:    HTTP Proxy authentication support (not fully tested).
//           HTTP Keep Alive not impliamented
//           HTTPS not implemented
//           The HTTP counters are not 100% correct (will be fixed)
//
///////////////////////////////////////////////////////////////////////////////
#include "wmsdk_config.h"
#include "wm_osal.h"

//#define LOG_TAG "httpc"
//#include "wm_log.h"

#include "lwip/sockets.h"

#include "HTTPClient.h"
#include "HTTPClientAuth.h"   // Crypto support (Digest, MD5)
#include "HTTPClientString.h" // String utilities
#include "wm_http_client.h"

#if CONFIG_COMPONENT_HTTP_CLIENT_ENABLED
static uint32_t HTTPIntrnResizeBuffer(P_HTTP_SESSION pHTTPSession, uint32_t nNewSize);
static uint32_t HTTPIntrnSetURL(P_HTTP_SESSION pHTTPSession, char *pUrl, uint32_t nUrlLength);
static uint32_t HTTPIntrnConnectionOpen(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnGetRemoteHeaders(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnGetRemoteChunkLength(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnSend(P_HTTP_SESSION pHTTPSession, char *pData, uint32_t *nLength);
static uint32_t HTTPIntrnRecv(P_HTTP_SESSION pHTTPSession, char *pData, uint32_t *nLength, bool PeekOnly);
#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
static uint32_t HTTPIntrnParseAuthHeader(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnAuthHandler(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnAuthSendDigest(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnAuthSendBasic(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnAuthenticate(P_HTTP_SESSION pHTTPSession);
#endif
static uint32_t HTTPIntrnHeadersAdd(P_HTTP_SESSION pHTTPSession, char *pHeaderName, uint32_t nNameLength, char *pHeaderData,
                                    uint32_t nDataLength);
static uint32_t HTTPIntrnHeadersRemove(P_HTTP_SESSION pHTTPSession, char *pHeaderName);
static uint32_t HTTPIntrnHeadersReceive(P_HTTP_SESSION pHTTPSession, uint32_t nTimeout);
static uint32_t HTTPIntrnHeadersSend(P_HTTP_SESSION pHTTPSession, HTTP_VERB HttpVerb);
static uint32_t HTTPIntrnHeadersParse(P_HTTP_SESSION pHTTPSession);
static uint32_t HTTPIntrnHeadersFind(P_HTTP_SESSION pHTTPSession, char *pHeaderName, HTTP_PARAM *pParam, bool IncommingHeaders,
                                     uint32_t nOffset);
static uint32_t HTTPIntrnSessionReset(P_HTTP_SESSION pHTTPSession, bool EntireSession);
static uint32_t HTTPIntrnSessionGetUpTime(void);
static bool HTTPIntrnSessionEvalTimeout(P_HTTP_SESSION pHTTPSession);

static inline void call_http_event_handle(P_HTTP_SESSION pHTTPSession, wm_http_client_event_t event,
                                          wm_http_client_event_param_t *data)
{
    wm_http_client_event_handle event_handle;

    if (pHTTPSession->http_event_handle) {
        event_handle = pHTTPSession->http_event_handle;
        event_handle((HTTP_SESSION_HANDLE)pHTTPSession, event, data, pHTTPSession->event_priv);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientSetLocalConnection
// Purpose      : TBD
// Gets         :
// Returns      :
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////
uint32_t HTTPClientSetLocalConnection(HTTP_SESSION_HANDLE pSession, uint32_t nPort)
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientOpenRequestWithCb
// Purpose      : Allocate memory for a new HTTP Session, and register a func for http user event
// Gets         : FLAGS
//              : http_event_handle  event handle func
//              : priv               user private data
// Returns      : HTTP Session handle
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

HTTP_SESSION_HANDLE HTTPClientOpenRequestWithCb(HTTP_CLIENT_SESSION_FLAGS Flags, void *http_event_handle, void *priv)
{
    P_HTTP_SESSION pHTTPSession = NULL; // Handle to the session pointer
    uint32_t nAllocationSize;           // Size of the dynamically allocated buffer

    // Attempt to allocate the buffer
    pHTTPSession = (P_HTTP_SESSION)wm_os_internal_malloc(ALIGN(sizeof(HTTP_SESSION)));

    // Did we succeed?
    if (!pHTTPSession) {
        // Null pointer is returned upon error
        return 0;
    }
    // Reset the allocated space to zeros
    memset(pHTTPSession, 0x00, sizeof(HTTP_SESSION));

    // Allocate space for the incoming and outgoing headers
    // Check if the headers buffer is resizable
    if (HTTP_CLIENT_MEMORY_RESIZABLE) {
        // Memory is resizable, so use the init defined size or the maximum buffer size (which ever is smaller)
        nAllocationSize = MIN(HTTP_CLIENT_MAX_SEND_RECV_HEADERS, HTTP_CLIENT_INIT_SEND_RECV_HEADERS);

    } else {
        // Memory is not resizable so simply use the maximum defined size
        nAllocationSize = HTTP_CLIENT_MAX_SEND_RECV_HEADERS;
    }
    nAllocationSize = ALIGN(nAllocationSize);
    // Allocate the headers buffer
    pHTTPSession->HttpHeaders.HeadersBuffer.pParam = (char *)wm_os_internal_malloc(nAllocationSize);
    // Check the returned pointer
    if (!pHTTPSession->HttpHeaders.HeadersBuffer.pParam) {
        // wm_os_internal_malloc() error, free the containing structure and exit.
        wm_os_internal_free(pHTTPSession);
        return 0;
    }

    // Reset the headers allocated memory
    memset(pHTTPSession->HttpHeaders.HeadersBuffer.pParam, 0x00, nAllocationSize);
    // Set the buffer length
    pHTTPSession->HttpHeaders.HeadersBuffer.nLength = nAllocationSize;

    // Set default values in the session structure
    HTTPClientSetVerb((uint32_t)pHTTPSession, (HTTP_VERB)HTTP_CLIENT_DEFAULT_VERB); // Default HTTP verb
    pHTTPSession->HttpUrl.nPort             = HTTP_CLIENT_DEFAULT_PORT;             // Default TCP port
    pHTTPSession->HttpConnection.HttpSocket = HTTP_INVALID_SOCKET;                  // Invalidate the socket
    // Set the outgoing headers pointers
    pHTTPSession->HttpHeaders.HeadersOut.pParam = pHTTPSession->HttpHeaders.HeadersBuffer.pParam;
    // Set our state
    pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_INIT;

    // Save the flags
    pHTTPSession->HttpFlags = Flags;

    // Reset the status
    pHTTPSession->HttpHeadersInfo.nHTTPStatus = 0;
    pHTTPSession->http_event_handle           = http_event_handle;
    pHTTPSession->event_priv                  = priv;
    // Return an allocated session pointer (cast to uint32_t first)
    return (HTTP_SESSION_HANDLE)pHTTPSession;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientOpenRequest
// Purpose      : Allocate memory for a new HTTP Session
// Gets         : FLAGS
// Returns      : HTTP Session handle
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

HTTP_SESSION_HANDLE HTTPClientOpenRequest(HTTP_CLIENT_SESSION_FLAGS Flags)
{
    P_HTTP_SESSION pHTTPSession = NULL; // Handle to the session pointer
    uint32_t nAllocationSize;           // Size of the dynamically allocated buffer

    // Attempt to allocate the buffer
    pHTTPSession = (P_HTTP_SESSION)wm_os_internal_malloc(ALIGN(sizeof(HTTP_SESSION)));

    // Did we succeed?
    if (!pHTTPSession) {
        // Null pointer is returned upon error
        return 0;
    }
    // Reset the allocated space to zeros
    memset(pHTTPSession, 0x00, sizeof(HTTP_SESSION));

    // Allocate space for the incoming and outgoing headers
    // Check if the headers buffer is resizable
    if (HTTP_CLIENT_MEMORY_RESIZABLE) {
        // Memory is resizable, so use the init defined size or the maximum buffer size (which ever is smaller)
        nAllocationSize = MIN(HTTP_CLIENT_MAX_SEND_RECV_HEADERS, HTTP_CLIENT_INIT_SEND_RECV_HEADERS);

    } else {
        // Memory is not resizable so simply use the maximum defined size
        nAllocationSize = HTTP_CLIENT_MAX_SEND_RECV_HEADERS;
    }
    nAllocationSize = ALIGN(nAllocationSize);
    // Allocate the headers buffer
    pHTTPSession->HttpHeaders.HeadersBuffer.pParam = (char *)wm_os_internal_malloc(nAllocationSize);
    // Check the returned pointer
    if (!pHTTPSession->HttpHeaders.HeadersBuffer.pParam) {
        // wm_os_internal_malloc() error, free the containing structure and exit.
        wm_os_internal_free(pHTTPSession);
        return 0;
    }

    // Reset the headers allocated memory
    memset(pHTTPSession->HttpHeaders.HeadersBuffer.pParam, 0x00, nAllocationSize);
    // Set the buffer length
    pHTTPSession->HttpHeaders.HeadersBuffer.nLength = nAllocationSize;

    // Set default values in the session structure
    HTTPClientSetVerb((uint32_t)pHTTPSession, (HTTP_VERB)HTTP_CLIENT_DEFAULT_VERB); // Default HTTP verb
    pHTTPSession->HttpUrl.nPort             = HTTP_CLIENT_DEFAULT_PORT;             // Default TCP port
    pHTTPSession->HttpConnection.HttpSocket = HTTP_INVALID_SOCKET;                  // Invalidate the socket
    // Set the outgoing headers pointers
    pHTTPSession->HttpHeaders.HeadersOut.pParam = pHTTPSession->HttpHeaders.HeadersBuffer.pParam;
    // Set our state
    pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_INIT;

    // Save the flags
    pHTTPSession->HttpFlags = Flags;

    // Reset the status
    pHTTPSession->HttpHeadersInfo.nHTTPStatus = 0;

    // Return an allocated session pointer (cast to uint32_t first)
    return (HTTP_SESSION_HANDLE)pHTTPSession;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientReset
// Purpose      : Reset the HTTP session and make it ready for another call
// Gets         : FLAGS
// Returns      : HTTP Session handle
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientReset(HTTP_SESSION_HANDLE pSession)
{
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    return HTTPIntrnSessionReset(pHTTPSession, true);
}

///////////////////////////////////////////////////////////////////////////////
// Function     : HTTPClientCloseRequest
// Purpose      : Closes any active connection and free any used memory
// Gets         : a Session handle
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientCloseRequest(HTTP_SESSION_HANDLE *pSession)
{
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION) * (pSession);
    if (!pHTTPSession) {
        // User passed a bad pointer
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    call_http_event_handle(pHTTPSession, WM_HTTP_CLIENT_EVENT_DISCONNECTED, NULL);

    if (pHTTPSession->HttpUrl.Url != NULL) {
        wm_os_internal_free(pHTTPSession->HttpUrl.Url);
        pHTTPSession->HttpUrl.Url = NULL;
    }
    // Check for a valid pointer to the HTTP headers
    if (pHTTPSession->HttpHeaders.HeadersBuffer.pParam) {
        // Release the used memory
        wm_os_internal_free(pHTTPSession->HttpHeaders.HeadersBuffer.pParam);
        pHTTPSession->HttpHeaders.HeadersBuffer.pParam = NULL;
    }
    // Close any active socket connection
    HTTPIntrnConnectionClose(pHTTPSession);
    // free the session structure
    wm_os_internal_free(pHTTPSession);

    pHTTPSession = 0; // NULL the pointer
    *(pSession)  = 0;

    return HTTP_CLIENT_SUCCESS;
}

#if CONFIG_WM_HTTP_CLIENT_DEBUG
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientSetDebugHook
// Purpose      : Sets the HTTP debug function pointer
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientSetDebugHook(HTTP_SESSION_HANDLE pSession,
                                E_HTTPDebug pDebug) // [IN] Function pointer to the the caller debugging function)
{
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    pHTTPSession->pDebug = pDebug;
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPClientSetDebugHook", NULL, 0, "Debugging hook set, return code is %d", HTTP_CLIENT_SUCCESS);
    }

    return HTTP_CLIENT_SUCCESS;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientSetVerb
// Purpose      : Sets the HTTP verb for the outgoing request
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientSetVerb(HTTP_SESSION_HANDLE pSession,
                           HTTP_VERB HttpVerb) // [IN] Http verb (GET POST HEAD act')
{
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPClientSetVerb", NULL, 0, "Selected Verb is %d", (INT32)HttpVerb);
    }
#endif

    // Cache the verb (as integer) for later use
    pHTTPSession->HttpHeaders.HttpVerb = HttpVerb;

    // Convert the Verb parameter into its equivalent string representation
    switch (HttpVerb) {
        case VerbGet:
            strcpy(pHTTPSession->HttpHeaders.Verb, "GET");
            break;
        case VerbHead:
            if (!HTTP_CLIENT_ALLOW_HEAD_VERB) {
                return HTTP_CLIENT_ERROR_BAD_VERB;
            }
#if HTTP_CLIENT_ALLOW_HEAD_VERB
            strcpy(pHTTPSession->HttpHeaders.Verb, "HEAD");
            break;
#endif
        case VerbPost:
            strcpy(pHTTPSession->HttpHeaders.Verb, "POST");
            break;
        case VerbPut:
            strcpy(pHTTPSession->HttpHeaders.Verb, "PUT");
            break;
        case VerbDele:
            strcpy(pHTTPSession->HttpHeaders.Verb, "DELETE");
            break;
        default:
            // Unknown verb
            return HTTP_CLIENT_ERROR_BAD_VERB;
    };

    return HTTP_CLIENT_SUCCESS;
}
#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientSetVerb
// Purpose      : Sets the HTTP authentication schema
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientSetAuth(HTTP_SESSION_HANDLE pSession,
                           HTTP_AUTH_SCHEMA AuthSchema, // The type of the authentication to perform
                           void *pReserved)
{
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPClientSetAuth", NULL, 0, "Selected authentication is %d", (INT32)AuthSchema);
    }
#endif

    switch (AuthSchema) {
#if CONFIG_WM_HTTP_CLIENT_AUTH_BASIC
        case (AuthSchemaBasic):
            strcpy(pHTTPSession->HttpCredentials.AuthSchemaName, "basic");
            break;
#endif //CONFIG_WM_HTTP_CLIENT_AUTH_BASIC
#if CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
        case (AuthSchemaDigest):
            strcpy(pHTTPSession->HttpCredentials.AuthSchemaName, "digest");
            break;
#endif //CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
        case (AuthSchemaKerberos):
            strcpy(pHTTPSession->HttpCredentials.AuthSchemaName, "negotiate");
            break;
        default:
            break;
    };

    if (AuthSchema >= AuthNotSupported) {
        return HTTP_CLIENT_ERROR_BAD_AUTH;
    }

    pHTTPSession->HttpCredentials.CredAuthSchema = AuthSchema;
    return HTTP_CLIENT_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientSetCredentials
// Purpose      : Sets credentials for the target host
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientSetCredentials(HTTP_SESSION_HANDLE pSession,
                                  char *pUserName, // [IN] Null terminated string containing the sessions user name
                                  char *pPassword) // [IN] Null terminated string containing the sessions password
{
    uint32_t nLength;
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    // Get the length of the user name string and see if it's not too long
    nLength = strlen(pUserName);
    if (nLength > HTTP_CLIENT_MAX_USERNAME_LENGTH) {
        return HTTP_CLIENT_ERROR_LONG_INPUT;
    }

    // Get the length of the password string and see if it's not too long
    nLength = strlen(pPassword);
    if (nLength > HTTP_CLIENT_MAX_PASSWORD_LENGTH) {
        return HTTP_CLIENT_ERROR_LONG_INPUT;
    }

    // Copy them into our internal buffer
    pHTTPSession->HttpCredentials.CredUser     = pUserName;
    pHTTPSession->HttpCredentials.CredPassword = pPassword;

    // Set the authentication boolean flag
    pHTTPSession->HttpHeadersInfo.HaveCredentials = true;

    return HTTP_CLIENT_SUCCESS;
}

#endif //(CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)

#if CONFIG_WM_HTTP_CLIENT_PROXY
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientSetProxy
// Purpose      : Sets all the proxy related parameters
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientSetProxy(HTTP_SESSION_HANDLE pSession,
                            char *pProxyHost, // [IN] Null terminated string containing the host name
                            UINT16 nPort,     // [IN] The proxy port number
                            char *pUserName,  // [IN] User name for proxy authentication (can be null)
                            char *pPassword)  // [IN] User password for proxy authentication (can be null)
{
    P_HTTP_SESSION pHTTPSession = NULL;
    uint32_t nLength;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPClientSetProxy", NULL, 0, "Proxy host %s,Port %d, Username %s,Password %s", pProxyHost, nPort,
                             pUserName, pPassword);
    }
#endif

    nLength = strlen(pProxyHost);
    if (nLength > HTTP_CLIENT_MAX_PROXY_HOST_LENGTH) {
        return HTTP_CLIENT_ERROR_LONG_INPUT;
    }
    // Cache the user supplied information in the internal session structure
    pHTTPSession->HttpProxy.ProxyHost = pProxyHost;
    if (pUserName) {
        nLength = strlen(pUserName);
        if (nLength > HTTP_CLIENT_MAX_USERNAME_LENGTH) {
            return HTTP_CLIENT_ERROR_LONG_INPUT;
        }

        // Proxy user name (for Proxy server authentication)
        pHTTPSession->HttpProxy.ProxtUser = pUserName;
    }
    if (pPassword) {
        // Get the length of the password string and see if it's not too long
        nLength = strlen(pPassword);
        if (nLength > HTTP_CLIENT_MAX_PASSWORD_LENGTH) {
            return HTTP_CLIENT_ERROR_LONG_INPUT;
        }
        // Proxy password (for proxy server authentication)
        pHTTPSession->HttpProxy.ProxyPassword = pPassword;
    }
    // Proxy TCP port
    pHTTPSession->HttpProxy.nProxyPort = nPort;
    // Set the Proxy flag in the connection structure
    pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_USINGPROXY;
    // Set the proxy auyjentication schema
    if (pPassword && pUserName) {
        pHTTPSession->HttpProxy.ProxyAuthSchema = HTTP_CLIENT_DEFAULT_PROXY_AUTH;
    }
    return HTTP_CLIENT_SUCCESS;
}
#endif //CONFIG_WM_HTTP_CLIENT_PROXY

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientAddRequestHeaders
// Purpose      : Add headers to the outgoing request
// Gets         :
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

UINT32 HTTPClientAddRequestHeaders(HTTP_SESSION_HANDLE pSession,
                                   char *pHeaderName, // [IN] The Headers name
                                   char *pHeaderData, // [IN] The headers data
                                   BOOL nInsert)      // [IN] Reserved could be any
{
    uint32_t nRetCode;
    uint32_t nHeaderLength, nDataLength;
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPClientAddRequestHeaders", NULL, 0, "Adding Header %s: %s", pHeaderName, pHeaderData);
    }
#endif

    // Get the elements length
    nHeaderLength = strlen(pHeaderName);
    nDataLength   = strlen(pHeaderData);

    // Call the internal function to add the headers to our session buffer
    nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, pHeaderName, nHeaderLength, pHeaderData, nDataLength);

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientSendRequest
// Purpose      : This function builds the request headers, performs a DNS resolution ,
//                opens the connection (if it was not opened yet by a previous request or if it has closed)
//                and sends the request headers.
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

UINT32 HTTPClientSendRequest(HTTP_SESSION_HANDLE pSession,
                             CHAR *pUrl,         //  [IN] The requested URL
                             VOID *pData,        //  [IN] Data to post to the server
                             UINT32 nDataLength, //  [IN] Length of posted data
                             BOOL TotalLength,   //  [IN]
                             UINT32 nTimeout,    //  [IN] Operation timeout
                             UINT32 nClientPort) //  [IN] Client side port 0 for none
{
    uint32_t nRetCode;                  // Function call return code
    uint32_t nBytes;                    // Bytes counter (socket operations)
    uint32_t nUrlLength;                // Length of the given Url
    P_HTTP_SESSION pHTTPSession = NULL; // Session pointer
    char ContentLength[32];
    do {
        // Cast the handle to our internal structure and check the pointers validity
        pHTTPSession = (P_HTTP_SESSION)pSession;
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
        if (pHTTPSession->pDebug) {
            pHTTPSession->pDebug("HTTPClientSendRequest", NULL, 0, "Url: %s", pUrl);
        }
#endif

        // Set the operation timeout counters
        pHTTPSession->HttpCounters.nActionStartTime = HTTPIntrnSessionGetUpTime();
        // 0 makes us use the default defined value
        pHTTPSession->HttpCounters.nActionTimeout = HTTP_TIMEOUT(nTimeout);
        // Store the cliebt port for later usage
        pHTTPSession->HttpConnection.HttpClientPort = nClientPort;

        // Parse the URL
        nUrlLength = strlen(pUrl);
        nRetCode   = HTTPIntrnSetURL(pHTTPSession, pUrl, nUrlLength);
        if (nRetCode != HTTP_CLIENT_SUCCESS) {
            break;
        }

        // Create the default headers
        // Add the "Host" header. we should handle a special case of port incorporated within the host name.
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_URLANDPORT) != HTTP_CLIENT_FLAG_URLANDPORT) {
            // The case where we don't have the port
            if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Host", 4, pHTTPSession->HttpUrl.UrlHost.pParam,
                                                pHTTPSession->HttpUrl.UrlHost.nLength)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
        } else {
            // We have the port so use a deferent element
            if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Host", 4, pHTTPSession->HttpUrl.UrlHost.pParam,
                                                (pHTTPSession->HttpUrl.UrlPort.pParam - pHTTPSession->HttpUrl.UrlHost.pParam) -
                                                    1)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
        }
        // We are in a post request without knowing the total length in advance so return error or use chunking
        if (pHTTPSession->HttpHeaders.HttpVerb == VerbPost && TotalLength == false) {
            // If the user specified the chunked flag
            if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SEND_CHUNKED) == HTTP_CLIENT_FLAG_SEND_CHUNKED) {
                // Add the  Transfer-Encoding:  header
                if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Transfer-Encoding", 17, "chunked", 7)) !=
                    HTTP_CLIENT_SUCCESS) {
                    break;
                    ;
                }
            } else if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_MULTIPART_FORM) == HTTP_CLIENT_FLAG_MULTIPART_FORM) {
            } else {
                // Not a supported operation - unknown length
                nRetCode = HTTP_CLIENT_ERROR_HEADER_NO_LENGTH;
                break;
            }
        }

        // Add the "User-Agent" header
        if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "User-Agent", 10, HTTP_CLIENT_DEFAULT_AGENT,
                                            strlen(HTTP_CLIENT_DEFAULT_AGENT))) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Add the "Keep-Alive" header (if requested by the caller)
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_KEEP_ALIVE) == HTTP_CLIENT_FLAG_KEEP_ALIVE) {
            if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_USINGPROXY) != HTTP_CLIENT_FLAG_USINGPROXY) {
                // No proxy keep alive:
                if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Connection", 10, "Keep-Alive", 10)) != HTTP_CLIENT_SUCCESS) {
                    break;
                }
            }
#if CONFIG_WM_HTTP_CLIENT_PROXY
            else // proxy keep alive
            {
                if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Proxy-Connection", 15, "Keep-Alive", 10)) !=
                    HTTP_CLIENT_SUCCESS) {
                    break;
                }
            }
#endif //CONFIG_WM_HTTP_CLIENT_PROXY
        }

        // If we know the total length in advance (only when posting)
        if ((pHTTPSession->HttpHeaders.HttpVerb == VerbPost || pHTTPSession->HttpHeaders.HttpVerb == VerbPut) &&
            TotalLength == true) {
            // set the total content length header
            pHTTPSession->HttpHeadersInfo.nHTTPPostContentLength = nDataLength; // Store for later usage
            memset(ContentLength, 0, 32);
            snprintf(ContentLength, sizeof(ContentLength), "%u", (unsigned int)nDataLength);
            //IToA(ContentLength,nDataLength); // Convert the buffer length to a string value
            if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Content-Length", 14, ContentLength, strlen(ContentLength))) !=
                HTTP_CLIENT_SUCCESS) {
                break;
            }
        }

        // Add the "Cache control" header (if requested by the caller)
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_NO_CACHE) == HTTP_CLIENT_FLAG_NO_CACHE) {
            if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Cache-Control", 13, "no-cache", 8)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
        }

        // Now we can connect to the remote server and send the leading request followed by the HTTP headers
        // Check for timeout
        if (HTTPIntrnSessionEvalTimeout(pHTTPSession) == true) {
            nRetCode = HTTP_CLIENT_ERROR_SOCKET_TIME_OUT;
            break;
        }
        //  Handle connection close message (reconnect)
        if (pHTTPSession->HttpHeadersInfo.Connection == false) {
            // Gracefully close the connection and set the socket as invalid
            if (pHTTPSession->HttpConnection.HttpSocket != HTTP_INVALID_SOCKET) {
                HTTPIntrnConnectionClose(pHTTPSession);
            }
            // Connect to the remote server (or proxy)
            nRetCode = HTTPIntrnConnectionOpen(pHTTPSession);
            if (nRetCode != HTTP_CLIENT_SUCCESS) {
                break;
            }

            //add http event
            call_http_event_handle(pHTTPSession, WM_HTTP_CLIENT_EVENT_CONNECTED, NULL);
        }

#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
        // Send the request along with the rest of the headers
        if (pHTTPSession->HttpCredentials.CredAuthSchema != AuthSchemaNone
#if CONFIG_WM_HTTP_CLIENT_PROXY
            || pHTTPSession->HttpProxy.ProxyAuthSchema != AuthSchemaNone
#endif        //CONFIG_WM_HTTP_CLIENT_PROXY
            ) // If we have to authenticate we should use the HEAD verb
        {
            if (HTTP_CLIENT_ALLOW_HEAD_VERB) // HEAD should not be ussed if not defined
            {
                if ((nRetCode = HTTPIntrnHeadersSend(pHTTPSession, VerbHead)) != HTTP_CLIENT_SUCCESS) {
                    break;
                }
                // Set the state flag
                pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_HEAD_SENT;
            } else {
                // Simply use the verb that was set by the caller without changing to HEAD
                if ((nRetCode = HTTPIntrnHeadersSend(pHTTPSession, pHTTPSession->HttpHeaders.HttpVerb)) !=
                    HTTP_CLIENT_SUCCESS) {
                    break;
                }
                // This the case where the caller know the total length to receive in advance
                // and he wishes to send the data right away
                if (pHTTPSession->HttpHeaders.HttpVerb == VerbPost && TotalLength == true) {
                    // Send the data
                    nBytes = nDataLength;
                    if ((nRetCode = HTTPIntrnSend(pHTTPSession, pData, &nBytes)) != HTTP_CLIENT_SUCCESS) {
                        break;
                    }
                    // Set the session state
                    pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_POST_SENT;
                }
            }

            // Retrive and analyze the Headers
            if ((nRetCode = HTTPIntrnHeadersReceive(pHTTPSession, nTimeout)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
            if (pHTTPSession->HttpHeadersInfo.nHTTPStatus != 401 && pHTTPSession->HttpHeadersInfo.nHTTPStatus != 407) {
                nRetCode = HTTP_CLIENT_ERROR_AUTH_MISMATCH;
                break;
            }
            // Authenticate
            if ((nRetCode = HTTPIntrnAuthenticate(pHTTPSession)) != HTTP_CLIENT_SUCCESS) {
                break;
            }

        } else
#endif //(CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
        {
            // No authentication use the verb that was requested by the caller
            if ((nRetCode = HTTPIntrnHeadersSend(pHTTPSession, pHTTPSession->HttpHeaders.HttpVerb)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
        }

        // This the case where the caller know the total length to receive in advance
        // and he wishes to send the data right away
        if ((pHTTPSession->HttpHeaders.HttpVerb == VerbPost || pHTTPSession->HttpHeaders.HttpVerb == VerbPut) &&
            (TotalLength == true ||
             (pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_MULTIPART_FORM) == HTTP_CLIENT_FLAG_MULTIPART_FORM)) {
            // Send the data
            nBytes = nDataLength;
            if ((nRetCode = HTTPIntrnSend(pHTTPSession, pData, &nBytes)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
            // Set the session state
            pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_POST_SENT;
        }

    } while (0);

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientRecvResponse
// Purpose      : Receives the response header on the connection and parses it.
//                Performs any required authentication.
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientRecvResponse(HTTP_SESSION_HANDLE pSession,
                                uint32_t nTimeout) // [IN] Timeout for the operation
{
    uint32_t nRetCode;                  // Function return code
    P_HTTP_SESSION pHTTPSession = NULL; // Session pointer

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    do {
        if ((nRetCode = HTTPIntrnHeadersReceive(pHTTPSession, nTimeout)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
    } while (0);

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientWriteData
// Purpose      : Write data to the remote server
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientWriteData(HTTP_SESSION_HANDLE pSession, void *pBuffer, uint32_t nBufferLength, uint32_t nTimeout)
{
    uint32_t nRetCode = HTTP_CLIENT_SUCCESS;
    uint32_t nBytes;
    char Chunk[HTTP_CLIENT_MAX_CHUNK_HEADER];

    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointer validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    do {
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        // Set the operation timeout counters
        pHTTPSession->HttpCounters.nActionStartTime = HTTPIntrnSessionGetUpTime();
        pHTTPSession->HttpCounters.nActionTimeout   = HTTP_TIMEOUT(nTimeout);

        // Did the caller specified chunked sending?
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SEND_CHUNKED) == HTTP_CLIENT_FLAG_SEND_CHUNKED) {
            // Prep the chunk Header and send it
            memset(Chunk, 0x00, HTTP_CLIENT_MAX_CHUNK_HEADER);
            HTTPStrLToH(Chunk, nBufferLength);
            strcat(Chunk, HTTP_CLIENT_CRLF);

            // Send the leading CrLf (only after the first chunk)
            if (pHTTPSession->HttpCounters.nSentChunks >= 1) {
                nBytes = 2;
                ;
                nRetCode = HTTPIntrnSend(pHTTPSession, HTTP_CLIENT_CRLF, &nBytes);
                if (nRetCode != HTTP_CLIENT_SUCCESS) {
                    break;
                }
            }

            // Send the chunk header
            nBytes   = strlen(Chunk);
            nRetCode = HTTPIntrnSend(pHTTPSession, Chunk, &nBytes);
            if (nRetCode != HTTP_CLIENT_SUCCESS) {
                break;
            }
        }

        // Send the data
        nBytes   = nBufferLength;
        nRetCode = HTTPIntrnSend(pHTTPSession, pBuffer, &nBytes);
        if (nRetCode != HTTP_CLIENT_SUCCESS) {
            break;
        }

        // If we are using chunks then..
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SEND_CHUNKED) == HTTP_CLIENT_FLAG_SEND_CHUNKED) {
            // Set the chunks count
            pHTTPSession->HttpCounters.nSentChunks++;

            // If it was the last chunk (0) we should re-get the headers from the server reply
            if (nBufferLength == 0) {
                // Send the trailing CrLf
                nBytes = 2;
                ;
                nRetCode = HTTPIntrnSend(pHTTPSession, HTTP_CLIENT_CRLF, &nBytes);
                if (nRetCode != HTTP_CLIENT_SUCCESS) {
                    break;
                }
                // Get the remote headers (since the last chunk was transmitted we can expect the server to start the reply)
                if ((nRetCode = HTTPIntrnHeadersReceive(pHTTPSession, nTimeout)) != HTTP_CLIENT_SUCCESS) {
                    break;
                }
            }
        }

    } while (0);

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientReadData
// Purpose      : Read data from the server. Parse out the chunks data.
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientReadData(HTTP_SESSION_HANDLE pSession,
                            void *pBuffer, // [IN OUT] a pointer to a buffer that will be filled with the servers response
                            uint32_t nBytesToRead,   // [IN]     The size of the buffer (numbers of bytes to read)
                            uint32_t nTimeout,       // [IN]     operation timeout in mseconds
                            uint32_t *nBytesRecived) // [OUT]    Count of the bytes that ware received in this operation
{
    uint32_t nBytes       = 0;
    uint32_t nRetCode     = 0;
    INT32 nProjectedBytes = 0; // Should support negative numbers
    char *pNullPtr;
    bool EndOfStream = false;

    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    // If the last verb that was used was HEAD there is no point to get this data (chanses are that we will endup with timeout)
    if (pHTTPSession->HttpHeaders.HttpVerb == VerbHead) {
        return HTTP_CLIENT_EOS;
    }

    // Set the operation timeout counters
    pHTTPSession->HttpCounters.nActionStartTime = HTTPIntrnSessionGetUpTime();
    pHTTPSession->HttpCounters.nActionTimeout   = HTTP_TIMEOUT(nTimeout);

    nBytes             = nBytesToRead - 1; // We will spare 1 byte for the trailing null termination
    *((char *)pBuffer) = 0;                // Null terminate the user supplied buffer
    *(nBytesRecived)   = 0;                // Set the return bytes count to 0

    // We can read the data only if we got valid headers (and not authentication requests for example)
    if ((pHTTPSession->HttpState & HTTP_CLIENT_STATE_HEADERS_PARSED) != HTTP_CLIENT_STATE_HEADERS_PARSED) {
        return HTTP_CLIENT_ERROR_BAD_STATE;
    }

    // Is it a chunked mode transfer?
    if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_CHUNKED) == HTTP_CLIENT_FLAG_CHUNKED) {
        // How many bytes left until the next chunk?
        if (pHTTPSession->HttpCounters.nBytesToNextChunk == 0) {
            // Read the chunk header and get its length
            if (HTTPIntrnGetRemoteChunkLength(pHTTPSession) != HTTP_CLIENT_SUCCESS) {
                // Could not parse the chunk parameter
                return HTTP_CLIENT_ERROR_CHUNK;
            }

            // 0 Bytes chunk, we should return end of stream
            if (pHTTPSession->HttpCounters.nRecivedChunkLength == 0) {
                return HTTP_CLIENT_EOS;
            }
        }
        // If we are about to read pass the next chunk, reduce the read bytes so we will read
        // non HTML data
        nProjectedBytes = pHTTPSession->HttpCounters.nBytesToNextChunk - nBytes;
        if (nProjectedBytes <= 0) {
            // Set the correct bytes count we should read
            nBytes = pHTTPSession->HttpCounters.nBytesToNextChunk;
        }
    }

    // Do we have the content length?
    if (pHTTPSession->HttpHeadersInfo.nHTTPContentLength > 0) {
        // Length of the projected buffer
        nProjectedBytes = pHTTPSession->HttpCounters.nRecivedBodyLength + nBytes;
        // If we are going to read more then the known content length then..
        if (nProjectedBytes >= (INT32)pHTTPSession->HttpHeadersInfo.nHTTPContentLength) {
            // Reduce the received bytes count to the correct size
            nBytes = pHTTPSession->HttpHeadersInfo.nHTTPContentLength - pHTTPSession->HttpCounters.nRecivedBodyLength;
        }
    }
    // Receive the data from the socket
    nRetCode = HTTPIntrnRecv(pHTTPSession, (char *)pBuffer, &nBytes, false);
    // Set the return bytes count
    *(nBytesRecived) = nBytes; // + 1; Fixed 11/9/2005

    // Pointer to the end of the buffer
    pNullPtr = (char *)pBuffer + nBytes;
    // And null terminate
    *pNullPtr = 0;
    // Socket read went OK
    if (nRetCode == HTTP_CLIENT_SUCCESS) {
        wm_http_client_event_param_t event = { 0 };
        event.data                         = pBuffer;
        event.data_len                     = nBytes;

        call_http_event_handle(pHTTPSession, WM_HTTP_CLIENT_EVENT_RECV_DATA, &event);
#if CONFIG_WM_HTTP_CLIENT_DEBUG
        if (pHTTPSession->pDebug) {
            pHTTPSession->pDebug("HTTPClientReadData", NULL, 0, "Reading %d bytes", nBytes);
        }
#endif
        // Set the HTTP counters
        pHTTPSession->HttpCounters.nRecivedBodyLength += nBytes;
        // If we know the total content length and..
        if (pHTTPSession->HttpHeadersInfo.nHTTPContentLength > 0) {
            // If total received body is equal or greater then the known content length then..
            if (pHTTPSession->HttpCounters.nRecivedBodyLength >= pHTTPSession->HttpHeadersInfo.nHTTPContentLength) {
                // Raise a flag to signal end of stream
                EndOfStream = true;
            }
        }
        // Is it a chunked mode transfer?
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_CHUNKED) == HTTP_CLIENT_FLAG_CHUNKED) {
            // We are a little closer to the next chunk now
            pHTTPSession->HttpCounters.nBytesToNextChunk -= nBytes;
        }
        // Is it End of stream?
        if (EndOfStream == true) {
            // So exit
            return HTTP_CLIENT_EOS;
        }
    }

    return nRetCode;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientGetInfo
// Purpose      : Fill the users structure with the session information
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientGetInfo(HTTP_SESSION_HANDLE pSession, HTTP_CLIENT *HTTPClient)
{
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
    // Reset the users info structure
    memset(HTTPClient, 0x00, sizeof(HTTP_CLIENT));

    HTTPClient->HTTPStatusCode             = pHTTPSession->HttpHeadersInfo.nHTTPStatus;
    HTTPClient->RequestBodyLengthSent      = pHTTPSession->HttpCounters.nSentBodyBytes;
    HTTPClient->ResponseBodyLengthReceived = pHTTPSession->HttpCounters.nRecivedBodyLength;
    HTTPClient->TotalResponseBodyLength    = pHTTPSession->HttpHeadersInfo.nHTTPContentLength;
    HTTPClient->HttpState                  = pHTTPSession->HttpState;

    return HTTP_CLIENT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientFindFirstHeader
// Purpose      : Initiate the headr searching functions
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientFindFirstHeader(HTTP_SESSION_HANDLE pSession, char *pSearchClue, char *pHeaderBuffer, uint32_t *nLength)
{
    P_HTTP_SESSION pHTTPSession = NULL;
    uint32_t nClueLength;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    nClueLength = strlen(pSearchClue); // See if we are not to long
    if (nClueLength >= HTTP_CLIENT_MAX_HEADER_SEARCH_CLUE) {
        return HTTP_CLIENT_ERROR_HEADER_BIG_CLUE;
    } else {
        pHTTPSession->HttpHeaders.SearchClue           = pSearchClue;
        pHTTPSession->HttpHeaders.HeaderSearch.nLength = 0;
        pHTTPSession->HttpHeaders.HeaderSearch.pParam  = NULL;
    }

    return HTTPClientGetNextHeader(pSession, pHeaderBuffer, nLength);
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientFindCloseHeader
// Purpose      : Terminate a headers search session
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPClientFindCloseHeader(HTTP_SESSION_HANDLE pSession)
{
    P_HTTP_SESSION pHTTPSession = NULL;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    pHTTPSession->HttpHeaders.HeaderSearch.nLength = 0;
    pHTTPSession->HttpHeaders.HeaderSearch.pParam  = NULL;

    return HTTP_CLIENT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPClientGetNextHeader
// Purpose      : Terminate a headers search session
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////
uint32_t HTTPClientGetNextHeader(HTTP_SESSION_HANDLE pSession, char *pHeaderBuffer, uint32_t *nLength)
{
    P_HTTP_SESSION pHTTPSession = NULL;
    uint32_t nOffset            = 0;
    uint32_t nRetCode;
    HTTP_PARAM HttpHeader;
    char *pPtr;

    // Cast the handle to our internal structure and check the pointers validity
    pHTTPSession = (P_HTTP_SESSION)pSession;
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
    if (pHTTPSession->HttpHeaders.HeaderSearch.nLength >
        0) // We must adjust the search offset since it is not the fierst iteration
    {
        nOffset = pHTTPSession->HttpHeaders.HeaderSearch.pParam - pHTTPSession->HttpHeaders.HeadersIn.pParam;
    }
    // Search for the next header
    nRetCode = HTTPIntrnHeadersFind(pHTTPSession, pHTTPSession->HttpHeaders.SearchClue, &HttpHeader, true, nOffset);

    if (nRetCode == HTTP_CLIENT_SUCCESS) {
        if (HttpHeader.nLength > *(nLength)) // Check for sufficiant length
        {
            *(nLength)       = HttpHeader.nLength;
            pHeaderBuffer[0] = 0; // Reset the users buffer
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        }

        pPtr    = HttpHeader.pParam;
        nOffset = 0;
        if (*pPtr == 0x0d) {
            nOffset++;
            pPtr++;
        }
        if (*pPtr == 0x0a) {
            nOffset++;
            pPtr++;
        }

        strncpy(pHeaderBuffer, pPtr, HttpHeader.nLength - nOffset);
        pHeaderBuffer[HttpHeader.nLength - nOffset]   = 0;
        *(nLength)                                    = HttpHeader.nLength - nOffset;
        pHTTPSession->HttpHeaders.HeaderSearch.pParam = HttpHeader.pParam + HttpHeader.nLength;
        pHTTPSession->HttpHeaders.HeaderSearch.nLength++;

        return HTTP_CLIENT_SUCCESS;
    }

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnSetURL
// Purpose      : Parse the user's URL
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static UINT32 HTTPIntrnSetURL(P_HTTP_SESSION pHTTPSession,
                              char *pUrl,          // [IN] a null terminated string containing the Url we should retrieve
                              uint32_t nUrlLength) // [IN] The length the Url string
{
    uint32_t nUrlOffset; // Offset in bytes within the Url string
    HTTP_URL *pUrlPtr;   // a Pointer to the Url structure (within the global session structure)
    char *pPtr;          // a Pointer for the Url port (Used in the parsing process)
    char UrlPort[16];    // a temporary byte array for the Url port conversion operation (string to number)

    // Check for the session pointer validity
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    // Get the length of the Url
    nUrlLength = strlen(pUrl);
    // Check if it is not longer than the permitted length
    if ((nUrlLength + 1) > HTTP_CLIENT_MAX_URL_LENGTH) {
        return HTTP_CLIENT_ERROR_LONG_INPUT;
    }

    if (pHTTPSession->HttpUrl.Url != NULL) {
        //wm_log_info("free pHTTPSession->HttpUrl.Url = %x\n", (UINT32)pHTTPSession->HttpUrl.Url);
        wm_os_internal_free(pHTTPSession->HttpUrl.Url);
        pHTTPSession->HttpUrl.Url = NULL;
    }
    pHTTPSession->HttpUrl.Url = wm_os_internal_malloc(nUrlLength + 2);
    if (pHTTPSession->HttpUrl.Url == NULL)
        return HTTP_CLIENT_ERROR_NO_MEMORY;
    memset(pHTTPSession->HttpUrl.Url, 0, nUrlLength + 2);
    //wm_log_info("HttpUrl.Url=%x, size=%d\n", pHTTPSession->HttpUrl.Url, nUrlLength+2);
    // Point the local pointer on the global structure
    pUrlPtr = &pHTTPSession->HttpUrl;
    // Copy to the internal buffer
    MEMCPY(pUrlPtr->Url, pUrl, nUrlLength);
    nUrlOffset = 0;

    // Get the Url base ("http" or "https")
    if (HTTPStrSearch(pUrlPtr->Url, ":", nUrlOffset, nUrlLength, &pUrlPtr->UrlBsee) == false) {
        return HTTP_CLIENT_ERROR_BAD_URL;
    }

    // Increase the offset parameter
    nUrlOffset += pUrlPtr->UrlBsee.nLength;
#if CONFIG_WM_HTTP_CLIENT_SECURE
    // If we can parse the string "HTTPS" we can assume a secured session
    if (HTTPStrInsensitiveCompare(pUrlPtr->UrlBsee.pParam, "https", pUrlPtr->UrlBsee.nLength) == true) {
        // Set the secured flags on the session
        pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_URLHTTPS;
        pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_SECURE;
        // ToDo: Init TLS (GetProtocol)
#ifdef _HTTP_BUILD_AMT
        // OS_GET_CLIENT_SUBSET_PROTOCOL(true,&pHTTPSession->pSecProtocol);
#endif

#if CONFIG_WM_HTTP_CLIENT_DEBUG
        if (pHTTPSession->pDebug) {
            pHTTPSession->pDebug("HTTPIntrnSetURL", NULL, 0, "SSL Protected Url %s", pUrl);
        }
#endif
    } else // it should be "http"
#endif     //CONFIG_WM_HTTP_CLIENT_SECURE
    {
        if (HTTPStrInsensitiveCompare(pUrlPtr->UrlBsee.pParam, "http", pUrlPtr->UrlBsee.nLength) == false) {
            return HTTP_CLIENT_ERROR_BAD_URL; // cOULD NOT DETECT http or https prefix
        }
    }

    // Look for standard Url elements
    if (HTTPStrSearch(pUrlPtr->Url, "://", nUrlOffset, 3, 0) == false) {
        return HTTP_CLIENT_ERROR_BAD_URL; // Could not detect "://"
    }
    // Increase the offset parameter
    nUrlOffset += 3;

    // Get the host name
    if (HTTPStrSearch(pUrlPtr->Url, "/", nUrlOffset, (nUrlLength - nUrlOffset), &pUrlPtr->UrlHost) == false) {
        pUrlPtr->Url[nUrlLength] = '/';
        nUrlLength++;
        if (HTTPStrSearch(pUrlPtr->Url, "/", nUrlOffset, (nUrlLength - nUrlOffset), &pUrlPtr->UrlHost) == false) {
            return HTTP_CLIENT_ERROR_BAD_URL;
        }
    }

    nUrlOffset += pUrlPtr->UrlHost.nLength;

    // Do we have the port within the hostname?
    if (HTTPStrSearch(pUrlPtr->Url, ":", (nUrlOffset - pUrlPtr->UrlHost.nLength), pUrlPtr->UrlHost.nLength,
                      &pUrlPtr->UrlPort) == true) {
        if ((pUrlPtr->UrlHost.nLength - pUrlPtr->UrlPort.nLength) < 10) {
            // To-Do: check the actual port length before the memcpy
            pUrlPtr->UrlPort.pParam += pUrlPtr->UrlPort.nLength + 1;
            MEMCPY(UrlPort, pUrlPtr->UrlPort.pParam, 15);
            pUrlPtr->UrlPort.nLength = 0;
            pPtr                     = UrlPort;
            while (*pPtr && pPtr++) {
                pUrlPtr->UrlPort.nLength++;
                if (*pPtr == '/') {
                    *pPtr                   = 0;
                    pUrlPtr->nPort          = (UINT16)atol(UrlPort);
                    pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_URLANDPORT;
                    break;
                }
            }
        } else {
            // Port too big
            return HTTP_CLIENT_ERROR_BAD_URL;
        }
    }

    // Get the request body
    pUrlPtr->UrlRequest.pParam  = pUrlPtr->Url + nUrlOffset;
    pUrlPtr->UrlRequest.nLength = nUrlLength - nUrlOffset;
#if CONFIG_WM_HTTP_CLIENT_SECURE
    // If we got SSL url with no port we should set the default ssl port
    if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_URLHTTPS) == HTTP_CLIENT_FLAG_URLHTTPS) {
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_URLANDPORT) != HTTP_CLIENT_FLAG_URLANDPORT) {
            pHTTPSession->HttpUrl.nPort = HTTP_CLIENT_DEFAULT_SSL_PORT;
        }
    }
#endif //CONFIG_WM_HTTP_CLIENT_SECURE
    // Set the state flag
    pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_URL_PARSED;

    return HTTP_CLIENT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnResizeBuffer
// Purpose      : Resize the HTTP headers buffer and reset the pointers
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnResizeBuffer(P_HTTP_SESSION pHTTPSession,
                                      uint32_t nNewBufferSize) // [IN] The new (and larger) buffer size
{
    char *pPtr = NULL;
    uint32_t nCurrentBufferSize;

    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
    // If the new buffer size is less or equal to the current buffer size then..
    if (pHTTPSession->HttpHeaders.HeadersBuffer.nLength >= nNewBufferSize) {
        // Return an error (bad buffer)
        return HTTP_CLIENT_ERROR_BUFFER_RSIZE;
    }

    // If the new buffer size is bigger then the defined maximum buffer size then..
    if (nNewBufferSize > HTTP_CLIENT_MAX_SEND_RECV_HEADERS) {
        // Return an error (bad buffer)
        return HTTP_CLIENT_ERROR_BUFFER_RSIZE;
    }
    // Current buffer size is the sum of the incoming and outgoing headers strings lengths
    nCurrentBufferSize = pHTTPSession->HttpHeaders.HeadersOut.nLength + pHTTPSession->HttpHeaders.HeadersIn.nLength;
    // Allocate a new buffer with the requested buffer size
    nNewBufferSize = ALIGN(nNewBufferSize);
    //wm_log_info("nNewBufferSize=%d\n", nNewBufferSize);
    pPtr = (char *)wm_os_internal_malloc(nNewBufferSize);
    if (!pPtr) {
        // malloc() error
        return HTTP_CLIENT_ERROR_NO_MEMORY;
    }

    // Copy the memory only if there is data to copy
    if (nCurrentBufferSize > 0) {
        MEMCPY(pPtr, pHTTPSession->HttpHeaders.HeadersBuffer.pParam, nCurrentBufferSize);
        // Reset the rest of the buffer
        memset(pPtr + nCurrentBufferSize, 0x00, (nNewBufferSize - nCurrentBufferSize));
    } else {
        // Reset the entire buffer (no previous buffer was copied)
        memset(pPtr, 0x00, nNewBufferSize);
    }

    wm_os_internal_free(pHTTPSession->HttpHeaders.HeadersBuffer.pParam);

    pHTTPSession->HttpHeaders.HeadersBuffer.pParam  = pPtr;
    pHTTPSession->HttpHeaders.HeadersBuffer.nLength = nNewBufferSize;

    // Refresh the pointers
    pHTTPSession->HttpHeaders.HeadersOut.pParam = pHTTPSession->HttpHeaders.HeadersBuffer.pParam;
    if (pHTTPSession->HttpHeaders.HeadersIn.pParam) {
        pHTTPSession->HttpHeaders.HeadersIn.pParam =
            pHTTPSession->HttpHeaders.HeadersBuffer.pParam + pHTTPSession->HttpHeaders.HeadersOut.nLength;
    }
    return HTTP_CLIENT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnRemoveHeader
// Purpose      : Removes an HTTP headers by its name
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnHeadersRemove(P_HTTP_SESSION pHTTPSession,
                                       char *pHeaderName) // [IN] The header's name

{
    HTTP_PARAM HttpParam;
    uint32_t nRetCode = HTTP_CLIENT_SUCCESS;
    uint32_t nBytes;

    if (!pHTTPSession) // Pointer validation check
    {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    // First see if we have that header in our outgoing headers buffer
    do {
        if ((nRetCode = HTTPIntrnHeadersFind(pHTTPSession, pHeaderName, &HttpParam, false, 0)) != HTTP_CLIENT_SUCCESS) {
            // Could not find this header
            break;
        }
        // Calculate the new headers length
        nBytes = (HttpParam.pParam - pHTTPSession->HttpHeaders.HeadersOut.pParam);
        nBytes -= HttpParam.nLength;

        // Copy the memory
        MEMCPY(HttpParam.pParam, HttpParam.pParam + HttpParam.nLength, nBytes);

        // Set the new length
        pHTTPSession->HttpHeaders.HeadersOut.nLength -= HttpParam.nLength;

        // Reset the buffer from it's modified end to it's previous end
        memset(pHTTPSession->HttpHeaders.HeadersOut.pParam + pHTTPSession->HttpHeaders.HeadersOut.nLength, 0x00,
               HttpParam.nLength);

    } while (0);

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnHeadersRemove", NULL, 0, "Removing Header %", pHeaderName);
    }
#endif

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnHeadersAdd
// Purpose      : Add HTTP headers to the outgoing headers buffers
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnHeadersAdd(P_HTTP_SESSION pHTTPSession,
                                    char *pHeaderName,    // [IN] The header's name
                                    uint32_t nNameLength, // [IN] Name length
                                    char *pHeaderData,    // [IN] The Header's data
                                    uint32_t nDataLength) // [IN] Data length
{
    char *pPtr;
    uint32_t nProjectedHeaderLength;
    uint32_t nProjectedBufferLength;
    INT32 nCurrentfreeSpace;
    INT32 nProjectedfreeSpace;
    uint32_t nRetCode;

    if (!pHTTPSession) // pointer validation check
    {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    if (nDataLength)
        nProjectedHeaderLength = nNameLength + nDataLength + 4;
    else
        nProjectedHeaderLength = nNameLength; //don't need: ': ' + '\r\n'
    nProjectedBufferLength =
        nProjectedHeaderLength + pHTTPSession->HttpHeaders.HeadersOut.nLength + pHTTPSession->HttpHeaders.HeadersIn.nLength;
    nCurrentfreeSpace = pHTTPSession->HttpHeaders.HeadersBuffer.nLength -
                        (pHTTPSession->HttpHeaders.HeadersOut.nLength + pHTTPSession->HttpHeaders.HeadersIn.nLength);
    nProjectedfreeSpace = nCurrentfreeSpace - nProjectedHeaderLength;

    // Check total size limit
    if (nProjectedBufferLength > HTTP_CLIENT_MAX_SEND_RECV_HEADERS) {
        return HTTP_CLIENT_ERROR_NO_MEMORY;
    }

    if ((INT32)nProjectedfreeSpace < 0) {
        if (HTTP_CLIENT_MEMORY_RESIZABLE == false) {
            // Need more space but we can't grow beyond the current size
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        } else {
            // We can resizes so..
            nRetCode = HTTPIntrnResizeBuffer(pHTTPSession, nProjectedBufferLength + HTTP_CLIENT_MEMORY_RESIZE_FACTOR);
            if (nRetCode != HTTP_CLIENT_SUCCESS) {
                return nRetCode;
            }
        }
    }

    // Move the incoming headers data within the buffer so we will have space for the added headers
    if (pHTTPSession->HttpHeaders.HeadersIn.pParam) {
        // Move the data and reset the data in the offset.
        MEMCPY(pHTTPSession->HttpHeaders.HeadersIn.pParam + nProjectedHeaderLength, pHTTPSession->HttpHeaders.HeadersIn.pParam,
               pHTTPSession->HttpHeaders.HeadersIn.nLength);
        // Reset the space created
        memset(pHTTPSession->HttpHeaders.HeadersOut.pParam + pHTTPSession->HttpHeaders.HeadersOut.nLength, 0x00,
               nProjectedHeaderLength);
    }

    pPtr = pHTTPSession->HttpHeaders.HeadersOut.pParam + pHTTPSession->HttpHeaders.HeadersOut.nLength;
    // Create the new header
    MEMCPY(pPtr, pHeaderName, nNameLength);
    pPtr += nNameLength;
    if (nDataLength) {
        MEMCPY(pPtr, ": ", 2);
        pPtr += 2;
        MEMCPY(pPtr, pHeaderData, nDataLength);
        pPtr += nDataLength;
        MEMCPY(pPtr, HTTP_CLIENT_CRLF, 2);
        pPtr += 2;
    }

    // Set the new length
    pHTTPSession->HttpHeaders.HeadersOut.nLength += nProjectedHeaderLength;

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnHeadersAdd", NULL, 0, "Adding Header %s: %s", pHeaderName, pHeaderData);
    }
#endif

    return HTTP_CLIENT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnConnectionClose
// Purpose      : Closes an active socket connection and invalidate the socket handle
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

uint32_t HTTPIntrnConnectionClose(P_HTTP_SESSION pHTTPSession)
{
    INT32 nRetCode = HTTP_CLIENT_SUCCESS;

    do {
        if (!pHTTPSession) // Validate the session pointer
        {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        // If we have a valid socket then..
        if (pHTTPSession->HttpConnection.HttpSocket != HTTP_INVALID_SOCKET) // INVALID_SOCKET
        {
#if CONFIG_WM_HTTP_CLIENT_SECURE
            if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
                // TLS Close
                nRetCode = HTTPWrapperSSLClose(pHTTPSession->ssl, pHTTPSession->HttpConnection.HttpSocket);
            }
#endif      //CONFIG_WM_HTTP_CLIENT_SECURE
            // Gracefully close it
            shutdown(pHTTPSession->HttpConnection.HttpSocket, 0x02);
            closesocket(pHTTPSession->HttpConnection.HttpSocket);
            // And invalidate the socket
            pHTTPSession->HttpConnection.HttpSocket = HTTP_INVALID_SOCKET;

            break;
            ;
        } else {
            // Not a valid socket error
            nRetCode = HTTP_CLIENT_ERROR_SOCKET_INVALID;
            break;
        }

    } while (0);

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnConnectionClose", NULL, 0, "");
    }
#endif
    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnConnectionOpen
// Purpose      : Opens a socket connection to the remote host or proxy server
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnConnectionOpen(P_HTTP_SESSION pHTTPSession)
{
    INT32 nRetCode = HTTP_CLIENT_SUCCESS; // a function return code value
    uint32_t nNullOffset;                 // a helper value to null terminate a given string
    //int              nNonBlocking    = 1;                // non blocking mode parameter
    char Backup; // a container for a char value (helps in temporary null termination)
    // HTTP_HOSTNET     *HostEntry;                          // Socket host entry pointer
    ULONG Address = 0;
    HTTP_SOCKADDR_IN ServerAddress; // Socket address structure
    HTTP_SOCKADDR_IN LoaclAddress;  // Socket address structure (for client binding)
    do {
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }
        // Use an existing connection if valid
        if (pHTTPSession->HttpConnection.HttpSocket != HTTP_INVALID_SOCKET) {
            // Set the state flag
            pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_HOST_CONNECTED;
            return HTTP_CLIENT_SUCCESS;
        }
        // Zero the socket events
        FD_ZERO(&pHTTPSession->HttpConnection.FDRead);
        FD_ZERO(&pHTTPSession->HttpConnection.FDWrite);
        FD_ZERO(&pHTTPSession->HttpConnection.FDError);

        if (pHTTPSession->HttpConnection.HttpSocket == HTTP_INVALID_SOCKET) {
            // Create a TCP/IP stream socket
            pHTTPSession->HttpConnection.HttpSocket = socket(AF_INET,      // Address family
                                                             SOCK_STREAM,  // Socket type
                                                             IPPROTO_TCP); // Protocol
            //wm_log_debug("pHTTPSession->HttpConnection.HttpSocket=%d", pHTTPSession->HttpConnection.HttpSocket);
        }

        // Exit if we don't have a valid socket
        if (pHTTPSession->HttpConnection.HttpSocket == HTTP_INVALID_SOCKET) {
            nRetCode = HTTP_CLIENT_ERROR_SOCKET_INVALID;
            break;
        }
        // Set non blocking socket
        /*
        nRetCode = ioctlsocket(pHTTPSession->HttpConnection.HttpSocket, FIONBIO, &nNonBlocking);
        if(nRetCode != 0)
        {
            nRetCode = HTTP_CLIENT_ERROR_SOCKET_CANT_SET;
            break;
        }
        */
        // Resolve the target host name
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_USINGPROXY) != HTTP_CLIENT_FLAG_USINGPROXY) {
            // No proxy, directly resolving the host name
            // Prep the parameter
            if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_URLANDPORT) == HTTP_CLIENT_FLAG_URLANDPORT) {
                nNullOffset = pHTTPSession->HttpUrl.UrlHost.nLength - pHTTPSession->HttpUrl.UrlPort.nLength - 1;
            } else {
                nNullOffset = pHTTPSession->HttpUrl.UrlHost.nLength;
            }

            Backup = HTTPStrExtract(pHTTPSession->HttpUrl.UrlHost.pParam, nNullOffset, 0);
            // Resolve the host name
            nRetCode = HostByName(pHTTPSession->HttpUrl.UrlHost.pParam, &Address);
            if (!nRetCode) {
                nRetCode = HTTP_CLIENT_ERROR_BAD_URL;
                break;
            }
            // Restore from backup (fix the buffer)
            HTTPStrExtract(pHTTPSession->HttpUrl.UrlHost.pParam, nNullOffset, Backup);

        }
#if CONFIG_WM_HTTP_CLIENT_PROXY
        else {
            // Using a Proxy server so resolve the proxy host name
            nRetCode = HostByName(pHTTPSession->HttpProxy.ProxyHost, &Address);
            if (!nRetCode) {
                nRetCode = HTTP_CLIENT_ERROR_BAD_URL;
                break;
            }
        }
#endif  //CONFIG_WM_HTTP_CLIENT_PROXY
        // See if we have a valid response from the net resolve operation
        /*
        if(nRetCode)
        {
        nRetCode = HTTP_CLIENT_ERROR_SOCKET_RESOLVE;
        break;
        }
        */
        // Reset the address structures
        memset(&ServerAddress, 0, sizeof(HTTP_SOCKADDR_IN));
        memset(&LoaclAddress, 0, sizeof(HTTP_SOCKADDR_IN));

        // Fill in the address structure
        ServerAddress.sin_family = AF_INET;
#ifdef _HTTP_BUILD_AMT
        ServerAddress.sin_len         = sizeof(HTTP_SOCKADDR_IN);
        ServerAddress.sin_addr.s_addr = htonl(Address); // Server's address
#endif
#ifdef _HTTP_BUILD_WIN32
        ServerAddress.sin_addr.s_addr = Address; // Server's address
#endif
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_USINGPROXY) != HTTP_CLIENT_FLAG_USINGPROXY) {
            // Use the remote web server port
            ServerAddress.sin_port = htons(pHTTPSession->HttpUrl.nPort); // Host Port number
        }
#if CONFIG_WM_HTTP_CLIENT_PROXY
        else {
            // Use the proxy port
            ServerAddress.sin_port = htons(pHTTPSession->HttpProxy.nProxyPort); // Proxy Port number
        }
#endif  //CONFIG_WM_HTTP_CLIENT_PROXY
        // Client-side Binding
        if (pHTTPSession->HttpConnection.HttpClientPort != 0) {
            LoaclAddress.sin_family = AF_INET;
#ifdef _HTTP_BUILD_AMT
            LoaclAddress.sin_len = sizeof(HTTP_SOCKADDR_IN);
#endif
            LoaclAddress.sin_port = htons((unsigned short)pHTTPSession->HttpConnection.HttpClientPort);

            nRetCode = bind(pHTTPSession->HttpConnection.HttpSocket, (HTTP_SOCKADDR *)&LoaclAddress, sizeof(HTTP_SOCKADDR_IN));

            if (nRetCode != 0) {
                nRetCode = HTTP_CLIENT_ERROR_SOCKET_BIND;
                break;
            }
        }
#if CONFIG_WM_HTTP_CLIENT_SECURE
        // Connect using TLS or otherwise clear connection
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) { // Is it a TLS connection?
            char *hostname = calloc(1, pHTTPSession->HttpUrl.UrlHost.nLength + 1);
            if (hostname == NULL) {
                nRetCode = HTTP_CLIENT_ERROR_NO_MEMORY;
                break;
            }

            memcpy(hostname, pHTTPSession->HttpUrl.UrlHost.pParam, pHTTPSession->HttpUrl.UrlHost.nLength);
            nRetCode = HTTPWrapperSSLConnect(&pHTTPSession->ssl, pHTTPSession->HttpConnection.HttpSocket, // Socket
                                             (HTTP_SOCKADDR *)&ServerAddress,                             // Server address
                                             sizeof(HTTP_SOCKADDR), // Length of server address structure
                                             hostname);
            free(hostname);

        } else                                                      // Non TLS so..
#endif                                                              //CONFIG_WM_HTTP_CLIENT_SECURE
        {
            nRetCode = connect(pHTTPSession->HttpConnection.HttpSocket, // Socket
                               (HTTP_SOCKADDR *)&ServerAddress,         // Server address
                               sizeof(HTTP_SOCKADDR));                  // Length of server address structure
            // The socket was set to be asyn so we should check the error being returned from connect()
            if (nRetCode) {
                nRetCode = SocketGetErr(pHTTPSession->HttpConnection.HttpSocket);
            }
        }

        if (nRetCode == 0 || nRetCode == HTTP_EWOULDBLOCK || nRetCode == HTTP_EINPROGRESS) {
            // Set TLS Nego flag to flase
            pHTTPSession->HttpConnection.TlsNego = false;
            // Set the Write fd_sets for a socket connection event
            FD_SET(pHTTPSession->HttpConnection.HttpSocket, &pHTTPSession->HttpConnection.FDWrite);
            // Set the state flag
            pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_HOST_CONNECTED;
            // We have connected so set the return value to success
            nRetCode = HTTP_CLIENT_SUCCESS;
            break;
        } else {
            //printf("%s: nRetCode %d\n", __func__, nRetCode);
            // Socket connection problem
            nRetCode = HTTP_CLIENT_ERROR_SOCKET_CONNECT;
            break;
        }
    } while (0);
#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnConnectionOpen", NULL, 0, "");
    }
#endif

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnSend
// Purpose      : Send data to the remote server (Asynchronous sockets)
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnSend(P_HTTP_SESSION pHTTPSession,
                              char *pData,       // [IN] a pointer to the data to be sent
                              uint32_t *nLength) // [IN OUT] Length of data to send and the transmitted bytes count
{
    INT32 nSocketEvents;                                // Socket events center
    INT32 nRetCode               = HTTP_CLIENT_SUCCESS; // a function return code value
    HTTP_TIMEVAL Timeval         = { 1, 0 };            // Timeout value for the socket() method
    HTTP_CONNECTION *pConnection = NULL;                // Pointer for the connection structure

    do {
        // Validate the session pointer
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        // Have a pointer on the internal connection structure for simplifying code reading
        pConnection = &pHTTPSession->HttpConnection;

        while (1) {
            // Check for timeout
            if (HTTPIntrnSessionEvalTimeout(pHTTPSession) == true) {
                nRetCode = HTTP_CLIENT_ERROR_SOCKET_TIME_OUT;
                break;
            }
            if (pConnection->HttpSocket == HTTP_INVALID_SOCKET) {
                nRetCode = HTTP_CLIENT_ERROR_SOCKET_INVALID;
                break;
            }
            // Reset socket events , only Error, since we don't want to get
            // a repeated Write events (socket is connected)

            FD_SET(pConnection->HttpSocket, &pConnection->FDError);
            FD_SET(pConnection->HttpSocket, &pConnection->FDWrite);

            // See if we got any events on the socket
            nSocketEvents = select((pConnection->HttpSocket + 1), 0, &pConnection->FDWrite, &pConnection->FDError, &Timeval);

            if (nSocketEvents < 0) // No events on the socket
            {
                *(nLength) = 0;
                break; // To-Do: This might be an error
            }

            if (nSocketEvents == 0) // No new events so
            {
                continue; // restart this loop
            }

            // Socket is writable (we are connected) so send the data
            if (FD_ISSET(pConnection->HttpSocket, &pConnection->FDWrite)) {
#if CONFIG_WM_HTTP_CLIENT_SECURE
                // Send the data
                if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
                    // TLS Protected connection
                    if (pConnection->TlsNego == false) {
                        char *hostname = calloc(1, pHTTPSession->HttpUrl.UrlHost.nLength + 1);
                        if (hostname == NULL) {
                            nRetCode = HTTP_CLIENT_ERROR_NO_MEMORY;
                            break;
                        }

                        memcpy(hostname, pHTTPSession->HttpUrl.UrlHost.pParam, pHTTPSession->HttpUrl.UrlHost.nLength);
                        nRetCode = HTTPWrapperSSLNegotiate((HTTP_SESSION_HANDLE)pHTTPSession, pConnection->HttpSocket, 0, 0,
                                                           hostname);
                        free(hostname);
                        if (nRetCode != 0) {
                            // TLS Error
                            nRetCode = HTTP_CLIENT_ERROR_TLS_NEGO;
                            break;
                        }
                        pConnection->TlsNego = true;
                    }
                    nRetCode = HTTPWrapperSSLSend(pHTTPSession->ssl, pConnection->HttpSocket, pData, *(nLength), 0);
                } else
#endif //CONFIG_WM_HTTP_CLIENT_SECURE
                {
                    nRetCode = send(pConnection->HttpSocket, pData, *(nLength), 0);
                }
                if (nRetCode == SOCKET_ERROR) {
                    nRetCode = SocketGetErr(pHTTPSession->HttpConnection.HttpSocket);
                    nRetCode = HTTP_CLIENT_ERROR_SOCKET_SEND;
                    break;
                }
                //wm_log_info("%s", pData);
                // The data was sent to the remote server
                *(nLength) = nRetCode;
                nRetCode   = HTTP_CLIENT_SUCCESS;
                break;
            }

            // We had a socket related error
            if (FD_ISSET(pConnection->HttpSocket, &pConnection->FDError)) {
                FD_CLR((uint32_t)pConnection->HttpSocket, &pConnection->FDError);
                *(nLength) = 0;
                // To-Do: Handle this case
                nRetCode = HTTP_CLIENT_ERROR_SOCKET_SEND;
                break;
            }
        }
    } while (0);

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnSend", pData, *(nLength), "");
    }
#endif

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnRecv
// Purpose      : Receive data from the connected socket using asynchronous sockets
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnRecv(P_HTTP_SESSION pHTTPSession,
                              char *pData,       // [IN] a pointer for a buffer that receives the data
                              uint32_t *nLength, // [IN OUT] Length of the buffer and the count of the received bytes
                              bool PeekOnly)     // [IN] State if we should only peek the socket (default is no)
{
    INT32 nSocketEvents;
#if CONFIG_WM_HTTP_CLIENT_SECURE
    INT32 nRetCode = pHTTPSession->ssl_more_data;
#else
    INT32 nRetCode = 0;
#endif
    HTTP_TIMEVAL Timeval         = { 0, 50000 };
    HTTP_CONNECTION *pConnection = NULL;

    do {
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        // Set a pointer on the session internal connection structure (simplify code reading)
        pConnection = &pHTTPSession->HttpConnection;
        while (1) {
            // Check for timeout
            if (HTTPIntrnSessionEvalTimeout(pHTTPSession) == true) {
                nRetCode = HTTP_CLIENT_ERROR_SOCKET_TIME_OUT;
                break;
            }
            if (pConnection->HttpSocket == HTTP_INVALID_SOCKET) {
                nRetCode = HTTP_CLIENT_ERROR_SOCKET_INVALID;
                break;
            }
            if (nRetCode != SOCKET_SSL_MORE_DATA) {
                // Reset socket events
                FD_SET(pConnection->HttpSocket, &pConnection->FDRead);
                FD_SET(pConnection->HttpSocket, &pConnection->FDError);

                // See if we got any events on the socket
                nSocketEvents = select(pConnection->HttpSocket + 1, &pConnection->FDRead, 0, &pConnection->FDError, &Timeval);
            } else {
                nSocketEvents = 1;
            }
            if (nSocketEvents < 0) // Error or no new socket events
            {
                *(nLength) = 0;
                break;
            }

            if (nSocketEvents == 0) {
                //////////////////////////////////////////////////////////////////////////////////////////////////////////
                // This is a simple bypass for the TSL session (for some reason the socket read event is not set so
                // The pending bytes on the socket are being checked manualy.
                // TLS hack:
                /*
                if((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE)
                {
                    nRetCode = 	HTTPWrapperSSLRecvPending(pConnection->HttpSocket);
                    if(nRetCode > 0)
                    {
                        // Recive without being notified by the socket event
                        if((nRetCode = HTTPWrapperSSLRecv(pConnection->HttpSocket,pData,*(nLength),0)) == SOCKET_ERROR)
                        {
                            // Socket error
                            nRetCode =  HTTP_CLIENT_ERROR_SOCKET_RECV;
                            break;
                        }
                        *(nLength) = nRetCode;
                        // Break on no data or server connection reset
                        if ( nRetCode == 0 || nRetCode == HTTP_ECONNRESET)
                        {
                            // Connection closed, simply break - this is not an error
                            nRetCode =  HTTP_CLIENT_EOS;  // Signal end of stream
                            break;
                        }
                        // We have successfully got the data from the server
                        nRetCode = HTTP_CLIENT_SUCCESS;
                        break;
                    }
                }
								*/
                // End Of the TLS bypass section
                //
                ////////////////////////////////////////////////////////////////////////////////////////////////
                continue; // select() timed out - restart this loop
            }
            if (nRetCode == SOCKET_SSL_MORE_DATA ||
                FD_ISSET(pConnection->HttpSocket, &pConnection->FDRead)) // Are there any read events on the socket ?
            {
                if (nRetCode != SOCKET_SSL_MORE_DATA) {
                    // Clear the event
                    FD_CLR((uint32_t)pConnection->HttpSocket, &pConnection->FDRead);
                }
                // Socket is readable so so read the data
                if (PeekOnly == false) {
#if CONFIG_WM_HTTP_CLIENT_SECURE
                    // Get the data (secuure)
                    if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
                        if ((nRetCode = HTTPWrapperSSLRecv(pHTTPSession->ssl, pConnection->HttpSocket, pData, *(nLength), 0)) ==
                            SOCKET_ERROR) {
                            // Socket error
                            nRetCode = HTTP_CLIENT_ERROR_SOCKET_RECV;
                            break;
                        }
                        pHTTPSession->ssl_more_data = (nRetCode == SOCKET_SSL_MORE_DATA ? SOCKET_SSL_MORE_DATA : 0);
                    } else // Get the data (non secuure)
#endif                     //CONFIG_WM_HTTP_CLIENT_SECURE
                    {
                        if ((nRetCode = recv(pConnection->HttpSocket, pData, *(nLength), 0)) == SOCKET_ERROR) {
                            // Socket error
                            nRetCode = HTTP_CLIENT_ERROR_SOCKET_RECV;
                            break;
                        }
                    }

                } else {
                    // Only peek te socket
                    if ((nRetCode = recv(pConnection->HttpSocket, pData, *(nLength), MSG_PEEK)) == SOCKET_ERROR) {
                        // Socket error
                        nRetCode = HTTP_CLIENT_ERROR_SOCKET_RECV;
                        break;
                    }
                }
                if (nRetCode != SOCKET_SSL_MORE_DATA)
                    *(nLength) = nRetCode;
                // Break on server connection reset
                if (/* nRetCode == 0 || */ HTTPWrapperGetSocketError(pConnection->HttpSocket) == HTTP_ECONNRESET) {
                    nRetCode = HTTP_CLIENT_EOS; // Signal end of stream
                    break;
                }
                // Break on no data
                // MSDN: If the connection has been gracefully closed, the return value is zero.
                if (nRetCode == 0) {
                    nRetCode = HTTP_CLIENT_ERROR_CONNECTION_CLOSE;
                    break;
                }

                // We have successfully got the data from the server
                nRetCode = HTTP_CLIENT_SUCCESS;
                break;
            }

            // We had a socket related error
            if (nRetCode != SOCKET_SSL_MORE_DATA && FD_ISSET(pConnection->HttpSocket, &pConnection->FDError)) {
                FD_CLR((uint32_t)pConnection->HttpSocket, &pConnection->FDError);
                *(nLength) = 0;

                // To-Do: Handle this case
                nRetCode = HTTP_CLIENT_ERROR_SOCKET_RECV;
                break;
            }
        }
    } while (0);

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnGetRemoteChunkLength
// Purpose      : Receive (byte by byte) the chunk parameter (while in chunk mode receive) and
//                Convert the HEX string into an integer
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnGetRemoteChunkLength(P_HTTP_SESSION pHTTPSession)
{
    uint32_t nBytesRead  = 1;
    uint32_t nRetCode    = HTTP_CLIENT_SUCCESS;
    uint32_t nBytesCount = 0;
    char ChunkHeader[HTTP_CLIENT_MAX_CHUNK_HEADER];
    char *pPtr;

    do {
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        // Read byte by byte until we get a CrLf
        pPtr  = ChunkHeader; // Get a pointer to the received buffer
        *pPtr = 0;           // Terminate with null

        while (nBytesRead > 0) {
            // Receive a single byte
            nRetCode = HTTPIntrnRecv(pHTTPSession, pPtr, &nBytesRead, false);
            // Did we succeed?
            if (nRetCode == HTTP_CLIENT_SUCCESS && nBytesRead > 0) {
                // Increment the bytes count
                nBytesCount += nBytesRead;
                if (nBytesRead > HTTP_CLIENT_MAX_CHUNK_HEADER) {
                    // Error chunk buffer is full
                    nRetCode = HTTP_CLIENT_ERROR_CHUNK_TOO_BIG;
                    break;
                }
                // Don't Process if the fist 2 bytes are CrLf.
                if (!((nBytesCount == 1 && *pPtr == 0x0d) || (nBytesCount == 2 && *pPtr == 0x0a))) {
                    // Advance the pointer by the received data length
                    pPtr += nBytesRead;
                    // Look for CrLf in the last 2 bytes
                    if (memcmp(pPtr - 2, HTTP_CLIENT_CRLF, 2) == 0) {
                        // Chunk Header was received
                        *pPtr                                          = 0; // null terminate the chunk parameter
                        pHTTPSession->HttpCounters.nRecivedChunkLength = HTTPStrHToL(ChunkHeader); // Convert to a number
                        // Set the HTTP counters
                        pHTTPSession->HttpCounters.nBytesToNextChunk = pHTTPSession->HttpCounters.nRecivedChunkLength;
                        break;
                    }
                }
            } else // Socket Error
            {
                nRetCode = 0;
                break;
            }
        }
    } while (0);

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnGetRemoteChunkLength", NULL, 0, "Next chunk is %d bytes",
                             pHTTPSession->HttpCounters.nRecivedChunkLength);
    }
#endif

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnGetRemoteHeaders
// Purpose      : Perform a socket receive (byte by byte) until all the HTTP headers are received
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnGetRemoteHeaders(P_HTTP_SESSION pHTTPSession)
{
    uint32_t nBytesRead = 1;
    uint32_t nRetCode   = HTTP_CLIENT_SUCCESS;
    uint32_t nProjectedHeaderLength;
    uint32_t nProjectedBufferLength;
    INT32 nCurrentfreeSpace;
    INT32 nProjectedfreeSpace;
    char *pPtr;

    do {
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        // Read byte by byte until we get CrLf followed by CrLf
        // Set the incoming headers pointer

        if (!pHTTPSession->HttpHeaders.HeadersIn.pParam) {
            /// The incoming headers starts where the outgoing headers ends
            pHTTPSession->HttpHeaders.HeadersIn.pParam =
                pHTTPSession->HttpHeaders.HeadersOut.pParam + pHTTPSession->HttpHeaders.HeadersOut.nLength;
        }

        // Receive until we get all the headers or any other error event
        while (nBytesRead > 0) {
            // Size of the projected buffer we are going to receive
            nProjectedHeaderLength = nBytesRead;
            // Size of the projected total incoming buffer
            nProjectedBufferLength = nProjectedHeaderLength + pHTTPSession->HttpHeaders.HeadersOut.nLength +
                                     pHTTPSession->HttpHeaders.HeadersIn.nLength;
            // Current free space on the incoming headers buffer
            nCurrentfreeSpace = pHTTPSession->HttpHeaders.HeadersBuffer.nLength -
                                (pHTTPSession->HttpHeaders.HeadersOut.nLength + pHTTPSession->HttpHeaders.HeadersIn.nLength);
            // Projected free space after the completion of the receive
            nProjectedfreeSpace = nCurrentfreeSpace - nProjectedHeaderLength;

            // Check total size limit
            if (nProjectedBufferLength > HTTP_CLIENT_MAX_SEND_RECV_HEADERS) {
                return HTTP_CLIENT_ERROR_NO_MEMORY;
            }

            if ((INT32)nProjectedfreeSpace < 0) {
                if (HTTP_CLIENT_MEMORY_RESIZABLE == false) {
                    // Need more space but we can't grow beyond the current size
                    nRetCode = HTTP_CLIENT_ERROR_NO_MEMORY;
                    break;
                } else {
                    // We can resizes so..
                    nRetCode = HTTPIntrnResizeBuffer(pHTTPSession, nProjectedBufferLength + HTTP_CLIENT_MEMORY_RESIZE_FACTOR);
                    if (nRetCode != HTTP_CLIENT_SUCCESS) {
                        break;
                    }
                }
            }
            // Jump to the beginning of the incoming headers (just after the end of the outgoing headers)
            pPtr = pHTTPSession->HttpHeaders.HeadersIn.pParam + pHTTPSession->HttpHeaders.HeadersIn.nLength;
            // Read a single byte
            nRetCode = HTTPIntrnRecv(pHTTPSession, pPtr, &nBytesRead, false);

            // ToDo: Break if not getting HTTP on the first 4 bytes
            if (nRetCode == HTTP_CLIENT_SUCCESS && nBytesRead > 0) {
                // Advance the pointer by 1 byte
                pPtr += nBytesRead;
                // Increase the total receive length
                pHTTPSession->HttpHeaders.HeadersIn.nLength++;

                // Set the HTTP counters
                pHTTPSession->HttpCounters.nRecivedHeaderLength++;

                if (memcmp(pPtr - 4, HTTP_CLIENT_CRLFX2, 4) == 0) {
                    // Headers were received
                    break;
                }
            } else {
                //wm_log_debug("HTTPIntrnRecv nRetCode = %d\n", nRetCode);
                if (nRetCode != HTTP_CLIENT_ERROR_SOCKET_TIME_OUT)
                    nRetCode = HTTP_CLIENT_ERROR_HEADER_RECV; // This was marked out for some reason
                break;
            }
        }
    } while (0);

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnGetRemoteHeaders", NULL, 0, "Got %d bytes", pHTTPSession->HttpHeaders.HeadersIn.nLength);
    }
#endif

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnHeadersFind
// Purpose      : Look for a header (insensitive search) by its name
// Gets         : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t
HTTPIntrnHeadersFind(P_HTTP_SESSION pHTTPSession, char *pHeaderName,
                     HTTP_PARAM *pParam,    //  [OUT] HTTP parameter structure that holds the search results
                     bool IncommingHeaders, //  [IN]  Indicate if we are to search in the incoming or outgoing headers
                     uint32_t nOffset)      //  [IN]  Optionaly privide an offset to start looking from
{
    char *pHeaderEnd;
    char *Header; // To-Do: Use pointers insted of fixed length
    uint32_t nLength;
    uint32_t nRetCode = HTTP_CLIENT_ERROR_HEADER_NOT_FOUND;

    do {
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        // Reset the input parameter structure
        pParam->pParam  = NULL;
        pParam->nLength = 0;
        // Get the requested header length
        nLength = strlen(pHeaderName);
        if (nLength > (HTTP_CLIENT_MAX_HEADER_SEARCH_CLUE - 3)) {
            // Error : header search clue too big
            nRetCode = HTTP_CLIENT_ERROR_HEADER_BIG_CLUE;
            break;
        }
        // Build the searched header name , add a leading CrLf before the header name and trailing ":"
        Header = wm_os_internal_malloc(HTTP_CLIENT_MAX_HEADER_SEARCH_CLUE);
        if (Header == NULL)
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        memset(Header, 0x00, HTTP_CLIENT_MAX_HEADER_SEARCH_CLUE);
        strcpy(Header, HTTP_CLIENT_CRLF);
        strcat(Header, pHeaderName);
        strcat(Header, ":");
        // Case insensitive search for the header name (search the incoming headers)
        if (IncommingHeaders == true) {
            pParam->pParam = HTTPStrCaseStr(pHTTPSession->HttpHeaders.HeadersIn.pParam + nOffset,
                                            pHTTPSession->HttpHeaders.HeadersIn.nLength, Header);
        } else {
            // Optionally search the outgoing headers
            pParam->pParam = HTTPStrCaseStr(pHTTPSession->HttpHeaders.HeadersOut.pParam + nOffset,
                                            pHTTPSession->HttpHeaders.HeadersOut.nLength, Header);
        }
        wm_os_internal_free(Header);
        if (pParam->pParam) // Did we find it?
        {
            // Search for the token end (trailing CrLf)
            pHeaderEnd = strstr(pParam->pParam + 2, HTTP_CLIENT_CRLF);
            if (pHeaderEnd) {
                // Get the length (up to the CrLf)
                pParam->nLength = pHeaderEnd - pParam->pParam;
                nRetCode        = HTTP_CLIENT_SUCCESS;
                break;
            }
        }
    } while (0);

    // Could not find the header
    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnHeadersSend
// Purpose      : Build and send the HTTP request. this includes the HTTP headers
//                and any required authentication data
// Gets         :
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnHeadersSend(P_HTTP_SESSION pHTTPSession,
                                     HTTP_VERB HttpVerb) // [IN] Argument that can bypass the requested verb
                                                         // Can be used for evaluating a HEAD request
{
    char *send_buff = NULL;
    uint32_t total_len = 0;
    uint32_t nBytes;
    uint32_t nRetCode = HTTP_CLIENT_SUCCESS;
    char RequestCmd[16];
    char ContentLength[32];
    bool RestoreHeadersFlag = false;
    HTTP_VERB HttpCachedVerb;

    if (!pHTTPSession) {
        // Bad session pointer error
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnHeadersSend", NULL, 0, "Using Verb: %d", (INT32)HttpVerb);
    }
#endif
    // Cache the original VERB
    HttpCachedVerb = pHTTPSession->HttpHeaders.HttpVerb;

    send_buff =
        calloc(1, 16 + pHTTPSession->HttpUrl.UrlRequest.nLength + 16 + pHTTPSession->HttpHeaders.HeadersOut.nLength + 2);

    if (send_buff == NULL) {
        return HTTP_CLIENT_ERROR_NO_MEMORY;
    }

    do {
        // Set the verb (temporarily)
        if (pHTTPSession->HttpHeaders.HttpVerb != HttpVerb) {
            if ((nRetCode = HTTPClientSetVerb((HTTP_SESSION_HANDLE)pHTTPSession, HttpVerb)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
        }

        // Remeber this state for later usage
        pHTTPSession->HttpHeaders.HttpLastVerb = pHTTPSession->HttpHeaders.HttpVerb;

        // If this is a head request we should temporary remove the chunking header and the content length header
        if (pHTTPSession->HttpHeaders.HttpVerb == VerbHead) {
            // If send in chunks flag was set
            if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SEND_CHUNKED) == HTTP_CLIENT_FLAG_SEND_CHUNKED) {
                // Chunking
                if ((nRetCode = HTTPIntrnHeadersRemove(pHTTPSession, "Transfer-Encoding")) != HTTP_CLIENT_SUCCESS) {
                    break;
                }
            }
            // Content-Length
            if (pHTTPSession->HttpHeadersInfo.nHTTPPostContentLength > 0) // Attempt to remove only if it was previusly set
            {
                if ((nRetCode = HTTPIntrnHeadersRemove(pHTTPSession, "Content-Length")) != HTTP_CLIENT_SUCCESS) {
                    break;
                }
            }

            RestoreHeadersFlag = true; // So it would be restored later
        }
        // Request Verb
        nBytes = strlen(pHTTPSession->HttpHeaders.Verb) + 1;
        memset(RequestCmd, 0x00, 16);
        strcpy(RequestCmd, pHTTPSession->HttpHeaders.Verb);
        strcat(RequestCmd, " ");

        memcpy(&send_buff[total_len], RequestCmd, nBytes);
        total_len += nBytes;

        // Request URI
        if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_USINGPROXY) != HTTP_CLIENT_FLAG_USINGPROXY) {
            nBytes = pHTTPSession->HttpUrl.UrlRequest.nLength;

            memcpy(&send_buff[total_len], pHTTPSession->HttpUrl.UrlRequest.pParam, nBytes);
            total_len += nBytes;
        }
#if CONFIG_WM_HTTP_CLIENT_PROXY
        else {
            nBytes = strlen(pHTTPSession->HttpUrl.Url);

            memcpy(&send_buff[total_len], pHTTPSession->HttpUrl.Url, nBytes);
            total_len += nBytes;
        }
#endif  //CONFIG_WM_HTTP_CLIENT_PROXY
        // Request HTTP Version
        memset(RequestCmd, 0x00, 16);
        strcpy(RequestCmd, " ");
        strcat(RequestCmd, HTTP_CLIENT_DEFAULT_VER);
        strcat(RequestCmd, HTTP_CLIENT_CRLF);
        nBytes = strlen(RequestCmd);

        memcpy(&send_buff[total_len], RequestCmd, nBytes);
        total_len += nBytes;

        // Request headers
        nBytes = pHTTPSession->HttpHeaders.HeadersOut.nLength;

        memcpy(&send_buff[total_len], pHTTPSession->HttpHeaders.HeadersOut.pParam, nBytes);
        total_len += nBytes;

        wm_http_client_event_param_t event = { 0 };
        event.data                         = pHTTPSession->HttpHeaders.HeadersOut.pParam;
        event.data_len                     = nBytes;

        call_http_event_handle(pHTTPSession, WM_HTTP_CLIENT_EVENT_HEADER_SENTED, &event);

        // Set the counters

#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
        // Optionally add authentication headers and send them (for host or proxy authentication)
        if (pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_UNAUTHORIZED
#if CONFIG_WM_HTTP_CLIENT_PROXY
            || pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED
#endif //CONFIG_WM_HTTP_CLIENT_PROXY
        ) {

            if ((nRetCode = HTTPIntrnAuthHandler(pHTTPSession)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
        }
#endif //(CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)

        // Request terminating CrLf
        nBytes = strlen(HTTP_CLIENT_CRLF);

        memcpy(&send_buff[total_len], HTTP_CLIENT_CRLF, nBytes);
        total_len += nBytes;

        if ((nRetCode = HTTPIntrnSend(pHTTPSession, send_buff, &total_len)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += total_len;

        // Restore the verb
        if (pHTTPSession->HttpHeaders.HttpVerb != HttpCachedVerb) {
            if ((nRetCode = HTTPClientSetVerb((HTTP_SESSION_HANDLE)pHTTPSession, HttpCachedVerb)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
        }

        if (RestoreHeadersFlag == true) {
            // Restore chunking header (since it was temporarily removed for the head request
            // Add the  Transfer-Encoding:  header
            if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SEND_CHUNKED) == HTTP_CLIENT_FLAG_SEND_CHUNKED) {
                if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Transfer-Encoding", 17, "chunked", 7)) !=
                    HTTP_CLIENT_SUCCESS) {
                    break;
                }
            }
            // Restore the content length
            if (pHTTPSession->HttpHeadersInfo.nHTTPPostContentLength > 0) // Attempt to remove only if it was previusly set
            {
                IToA(ContentLength,
                     pHTTPSession->HttpHeadersInfo.nHTTPPostContentLength); // Convert the buffer length to a string value
                if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Content-Length", 14, ContentLength,
                                                    strlen(ContentLength))) != HTTP_CLIENT_SUCCESS) {
                    if (send_buff) {
                        free(send_buff);
                    }
                    return nRetCode;
                }
            }
        }
        // Set the session stage
        pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_REQUEST_SENT;

    } while (0);

    if (send_buff) {
        free(send_buff);
    }

    return nRetCode; // end of HTTPIntrnSendHeaders()
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnHeadersParse
// Purpose      : Parse the HTTP incoming headers.
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnHeadersParse(P_HTTP_SESSION pHTTPSession)
{
    char *pPtr;                                  // a pointer that points on the incoming headers
    uint32_t nTokenLength = 0;                   // Length of the parsed token
    uint32_t nRetCode     = HTTP_CLIENT_SUCCESS; // a function return code value
#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
    uint32_t nOffset = 0;     // Bytes offset (strings comperision)
    bool AuthHeaders = false; // While we are searching the authentication methods
#endif
    char *HTTPToken;      // Buffer for the parsed HTTP token
    HTTP_PARAM HTTPParam; // A generic pointer\length parameter for parsing

    do {
        // Validate the session pointer
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
        if (pHTTPSession->pDebug) {
            pHTTPSession->pDebug("HTTPIntrnHeadersParse", pHTTPSession->HttpHeaders.HeadersIn.pParam,
                                 pHTTPSession->HttpHeaders.HeadersIn.nLength, "[Incomming Headers]");
        }
#endif

        // Set a pointer on the incoming headers
        pPtr = pHTTPSession->HttpHeaders.HeadersIn.pParam;
        // Detect the leading HTTP string
        if (HTTPStrInsensitiveCompare(pPtr, "http", 4) != true) {
            nRetCode = HTTP_CLIENT_ERROR_BAD_HEADER;
            break;
        }

        // Get the HTTP Version
        while ((*pPtr) && (*pPtr != 0x20)) {
            nTokenLength++;
            pPtr++; // Move to the first space
        }
        strncpy(pHTTPSession->HttpHeadersInfo.HTTPVersion, pPtr - nTokenLength, MIN(15, nTokenLength));
        pPtr++;

        // Get the HTTP status code
        HTTPToken = wm_os_internal_malloc(HTTP_CLIENT_MAX_TOKEN_LENGTH);
        if (HTTPToken == NULL)
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        memset(HTTPToken, 0x00, HTTP_CLIENT_MAX_TOKEN_LENGTH);
        nTokenLength = 0;
        while ((*pPtr) && (*pPtr != 0x20)) {
            nTokenLength++;
            pPtr++; // Move to the next space
        }
        strncpy(HTTPToken, (pPtr - nTokenLength), MIN(HTTP_CLIENT_MAX_TOKEN_LENGTH, nTokenLength));

        pHTTPSession->HttpHeadersInfo.nHTTPStatus = atol(HTTPToken);

        // Search for content length
        pHTTPSession->HttpHeadersInfo.nHTTPContentLength = 0; // Default no unknown length
        // Look for the token
        if (HTTPIntrnHeadersFind(pHTTPSession, "content-length", &HTTPParam, true, 0) == HTTP_CLIENT_SUCCESS) {
            memset(HTTPToken, 0x00, HTTP_CLIENT_MAX_TOKEN_LENGTH); // Reset the token buffer
            nTokenLength = HTTP_CLIENT_MAX_TOKEN_LENGTH;           // Set the buffer length
            // Attempt to extract the token
            if (HTTPStrGetToken(HTTPParam.pParam, HTTPParam.nLength, HTTPToken, &nTokenLength)) {
                // Convert the content-length into an integer.
                pHTTPSession->HttpHeadersInfo.nHTTPContentLength = atol(HTTPToken);
            }
        }

        // Search for connection status
        pHTTPSession->HttpHeadersInfo.Connection = true; // Default status where no server connection header was detected
        // Look for token (can be standard connection or a proxy connection)
        if ((HTTPIntrnHeadersFind(pHTTPSession, "connection", &HTTPParam, true, 0) == HTTP_CLIENT_SUCCESS) ||
            (HTTPIntrnHeadersFind(pHTTPSession, "proxy-connection", &HTTPParam, true, 0) == HTTP_CLIENT_SUCCESS)) {
            memset(HTTPToken, 0x00, HTTP_CLIENT_MAX_TOKEN_LENGTH);
            nTokenLength = HTTP_CLIENT_MAX_TOKEN_LENGTH;
            // Attempt to extract the token
            if (HTTPStrGetToken(HTTPParam.pParam, HTTPParam.nLength, HTTPToken, &nTokenLength)) {
                // Is this a keep alive session?
                pHTTPSession->HttpHeadersInfo.Connection = HTTPStrInsensitiveCompare(HTTPToken, "keep-alive", 0);
                // Is it a closed session
                if (HTTPStrInsensitiveCompare(HTTPToken, "close", 0) == true) {
                    pHTTPSession->HttpHeadersInfo.Connection = false;
                }
            }
        }

        // Search for chunking mode transfer
        pHTTPSession->HttpFlags = pHTTPSession->HttpFlags & ~HTTP_CLIENT_FLAG_CHUNKED; // Remove the flag
        if (HTTPIntrnHeadersFind(pHTTPSession, "transfer-encoding", &HTTPParam, true, 0) == HTTP_CLIENT_SUCCESS) {
            memset(HTTPToken, 0x00, HTTP_CLIENT_MAX_TOKEN_LENGTH);
            nTokenLength = HTTP_CLIENT_MAX_TOKEN_LENGTH;
            if (HTTPStrGetToken(HTTPParam.pParam, HTTPParam.nLength, HTTPToken, &nTokenLength)) {
                // If the chunks token was find then set the session flag accordingly
                if (HTTPStrInsensitiveCompare(HTTPToken, "chunked", 0) == true) {
                    pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_CHUNKED;
                }
            }
        }
        wm_os_internal_free(HTTPToken);
#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
        // Look for the authentication header
        while (AuthHeaders == false) // address multiple authentication methods presented by the server
        {
            if (pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_UNAUTHORIZED) {
                // Double check for the "www-authenticate" header token
                if (HTTPIntrnHeadersFind(pHTTPSession, "www-authenticate", &pHTTPSession->HttpAuthHeader.AuthHeader, true,
                                         nOffset) != HTTP_CLIENT_SUCCESS) {
                    if (nOffset > 0) // an authentication header was found but not the right one so adjust the error
                    {
                        nRetCode = HTTP_CLIENT_ERROR_AUTH_MISMATCH;
                    } else {
                        nRetCode = HTTP_CLIENT_ERROR_BAD_HEADER;
                    }

                    break;
                }

                // Make sure that we get an authentication header that maches the caller requested schema
                pPtr = HTTPStrCaseStr(pHTTPSession->HttpAuthHeader.AuthHeader.pParam,
                                      pHTTPSession->HttpAuthHeader.AuthHeader.nLength,
                                      pHTTPSession->HttpCredentials.AuthSchemaName);
                if (pPtr) {
                    AuthHeaders = true;
                } else {
                    // Simply pass the point where the last "www" was found
                    nOffset = (pHTTPSession->HttpAuthHeader.AuthHeader.pParam - pHTTPSession->HttpHeaders.HeadersIn.pParam) + 3;
                }
            } else {
                AuthHeaders = true;
            }
        }
#if CONFIG_WM_HTTP_CLIENT_PROXY
        // Is this a proxy authentication header?
        if (pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED) {
            // Double check for the "Proxy-Authentication" header token
            if (HTTPIntrnHeadersFind(pHTTPSession, "proxy-authenticate", &pHTTPSession->HttpAuthHeader.AuthHeader, true, 0) !=
                HTTP_CLIENT_SUCCESS) {
                nRetCode = HTTP_CLIENT_ERROR_BAD_HEADER;
                break;
            }
        }
#endif  //CONFIG_WM_HTTP_CLIENT_PROXY
#endif  //(CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
        // Do we have a redirection response?
        if ((pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_OBJECT_MOVED) ||
            (pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_OBJECT_MOVED_PERMANENTLY)) {
            // Check for the "Location" header token
            if (HTTPIntrnHeadersFind(pHTTPSession, "location", &pHTTPSession->HttpHeadersInfo.HttpRedirectURL, true, 0) !=
                HTTP_CLIENT_SUCCESS) {
                // Protocol violation, we got a redirect code without the host name to redirect to
                nRetCode = HTTP_CLIENT_ERROR_BAD_HEADER;
                break;
            }
            // Fix the pointers location (address the "Location: " prefix)
            pHTTPSession->HttpHeadersInfo.HttpRedirectURL.pParam += 12;
            pHTTPSession->HttpHeadersInfo.HttpRedirectURL.nLength -= 12;
        }

    } while (0);

    return nRetCode;
}
#if (CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnAuthenticate
// Purpose      :
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnAuthenticate(P_HTTP_SESSION pHTTPSession)
{
    uint32_t nRetCode    = HTTP_CLIENT_SUCCESS; // Function call return code
    uint32_t nBytes      = 32;
    uint32_t nTotalBytes = 0;
    char ErrorPage[32];
    bool NewConnection = false;

    do {
        // Validate the session pointer
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

        //  Handle connection close message (reconnect)
        if (pHTTPSession->HttpHeadersInfo.Connection == false) {
            // Gracefully close the connection and set the socket as invalid
            if (pHTTPSession->HttpConnection.HttpSocket != HTTP_INVALID_SOCKET) {
                HTTPIntrnConnectionClose(pHTTPSession);
            }
            // Connect to the remote server (or proxy)
            nRetCode = HTTPIntrnConnectionOpen(pHTTPSession);
            if (nRetCode != HTTP_CLIENT_SUCCESS) {
                break;
            }

            NewConnection = true;
        }

        // Analyze the security headers and optionally build the authentication reply header
        if ((nRetCode = HTTPIntrnParseAuthHeader(pHTTPSession)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // We have to recive any HTML data here inorder to "Clear" the socket buffer for later usage
        // Note: We should skip this when the HEAD verb was used
        while (NewConnection == false && pHTTPSession->HttpHeaders.HttpLastVerb != VerbHead &&
               pHTTPSession->HttpHeadersInfo.nHTTPContentLength > 0 && nBytes > 0) {
            ErrorPage[0] = 0;
            if ((nRetCode = HTTPIntrnRecv(pHTTPSession, ErrorPage, &nBytes, false)) != HTTP_CLIENT_SUCCESS) {
                break;
            }

            nTotalBytes += nBytes;
            if (nTotalBytes >= pHTTPSession->HttpHeadersInfo.nHTTPContentLength) {
                break;
            }
        }

        // Re-Send the headers after having analyzed the authorizaton headers
        if ((nRetCode = HTTPIntrnHeadersSend(pHTTPSession, pHTTPSession->HttpHeaders.HttpVerb)) != HTTP_CLIENT_SUCCESS) {
            break;
        }

    } while (0);

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnAuthenticate", NULL, 0, "");
    }
#endif

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnParseAuthHeader
// Purpose      : Parse the HTTP headers for the required authentication method
// Gets         :
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnParseAuthHeader(P_HTTP_SESSION pHTTPSession)
{
    char *pPtrStart, *pPtrEnd;

    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
#if CONFIG_WM_HTTP_CLIENT_PROXY
    if (pHTTPSession->HttpProxy.ProxyAuthSchema != AuthSchemaNone) {
        // for proxy authentication simply assume basic and exit
        return HTTP_CLIENT_SUCCESS;
    }
#endif //CONFIG_WM_HTTP_CLIENT_PROXY
    // Advance the pointer in the string and break on the first space
    pPtrEnd   = pHTTPSession->HttpAuthHeader.AuthHeader.pParam + pHTTPSession->HttpAuthHeader.AuthHeader.nLength;
    pPtrStart = pHTTPSession->HttpAuthHeader.AuthHeader.pParam;
    // Jump to the first space
    while ((pPtrEnd - pPtrStart) > 0 && *pPtrStart != 0x20)
        pPtrStart++;

    do {
#if CONFIG_WM_HTTP_CLIENT_AUTH_BASIC
        if (HTTPStrCaseStr(pPtrStart, 8, "basic")) {
            pHTTPSession->HttpAuthHeader.HTTP_AUTH_SCHEMA = AuthSchemaBasic;
            break;
        }
#endif //CONFIG_WM_HTTP_CLIENT_AUTH_BASIC
#if CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
        if (HTTPStrCaseStr(pPtrStart, 8, "digest")) {
            pHTTPSession->HttpAuthHeader.HTTP_AUTH_SCHEMA = AuthSchemaDigest;
            break;
        }
#endif
        if (HTTPStrCaseStr(pPtrStart, 8,
                           "negotiate")) // Note that this could be NLM negotiation as well (which is not supported)
        {
            pHTTPSession->HttpAuthHeader.HTTP_AUTH_SCHEMA = AuthSchemaKerberos;
            break;
        }
        // To-Do: Add any other supported authentication method
    } while (0);

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnParseAuthHeader", pHTTPSession->HttpAuthHeader.AuthHeader.pParam,
                             pHTTPSession->HttpAuthHeader.AuthHeader.nLength, "[Incomming Auth Headers: %d]",
                             pHTTPSession->HttpAuthHeader.HTTP_AUTH_SCHEMA);
    }
#endif

    // If we could not detect the authentication schema return an error
    if (pHTTPSession->HttpAuthHeader.HTTP_AUTH_SCHEMA == AuthSchemaNone) {
        return HTTP_CLIENT_ERROR_BAD_AUTH;
    }

    //Make sure we are going to authenticate with the method specified by the caller
    if (pHTTPSession->HttpAuthHeader.HTTP_AUTH_SCHEMA != (uint32_t)pHTTPSession->HttpCredentials.CredAuthSchema) {
        return HTTP_CLIENT_ERROR_AUTH_MISMATCH;
    }

    return HTTP_CLIENT_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnAuthHandler
// Purpose      : Differentiate between the authenticate method that we have to implement and perform
//                the required operation.
// Gets         :
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnAuthHandler(P_HTTP_SESSION pHTTPSession)
{
    uint32_t nRetCode = HTTP_CLIENT_SUCCESS;

    if (!pHTTPSession) {
        // Bad session pointer error
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
#if CONFIG_WM_HTTP_CLIENT_PROXY
    if (pHTTPSession->HttpProxy.ProxyAuthSchema != AuthSchemaNone) {
        // For proxy authentication simply assume basic and exit
        // Basic authentication
        nRetCode = HTTPIntrnAuthSendBasic(pHTTPSession);
        return nRetCode;
    }
#endif //CONFIG_WM_HTTP_CLIENT_PROXY

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnAuthHandler", NULL, 0, "");
    }
#endif

    // Use the correct authentication method as requested by the server
    switch (pHTTPSession->HttpAuthHeader.HTTP_AUTH_SCHEMA) {
#if CONFIG_WM_HTTP_CLIENT_AUTH_BASIC
        case AuthSchemaBasic:
        {
            // Basic authentication
            nRetCode = HTTPIntrnAuthSendBasic(pHTTPSession);
            break;
        }
#endif
#if CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
        case AuthSchemaDigest:
        {
            // Digest authentication
            nRetCode = HTTPIntrnAuthSendDigest(pHTTPSession);
            break;
        }
#endif
        case AuthSchemaKerberos:
        {
            // ToDo: impliament the Kerberos nego authentication here
            nRetCode = HTTP_CLIENT_ERROR_NOT_IMPLEMENTED;
            break;
        }
        default:
        {
            // Not supported method
            return HTTP_CLIENT_ERROR_BAD_AUTH; // Not implemented error
        }
    };

    // This session requested an authentication so..
    pHTTPSession->HttpCredentials.Authentication = true;
    return nRetCode;
}

#if CONFIG_WM_HTTP_CLIENT_AUTH_BASIC
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnAuthSendBasic
// Purpose      : Handle basic authentication for direst host connection and proxy authentication
// Gets         :
// Returns      :
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnAuthSendBasic(P_HTTP_SESSION pHTTPSession)
{
    uint32_t nSegmentLength;
    uint32_t nRetCode;
    char *Cred   = NULL; // Credentials (Clear)
    char *Cred64 = NULL; // Credentials (64 bit encoded)
    uint32_t nSrcLength, nDestLength;
    char *pPtr;
    char *INITIAL_HDR       = "Authorization: Basic ";
    char *INITIAL_PROXY_HDR = "Proxy-Authorization: Basic ";

    do {
        if (!pHTTPSession) {
            nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
        if (pHTTPSession->pDebug) {
            pHTTPSession->pDebug("HTTPIntrnAuthSendBasic", NULL, 0, "");
        }
#endif
        Cred = wm_os_internal_malloc(HTTP_CLIENT_MAX_64_ENCODED_CRED / 2);
        if (Cred == NULL)
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        Cred64 = wm_os_internal_malloc(HTTP_CLIENT_MAX_64_ENCODED_CRED);
        if (Cred64 == NULL) {
            wm_os_internal_free(Cred);
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        }
        memset(Cred, 0x00, HTTP_CLIENT_MAX_64_ENCODED_CRED / 2);
        memset(Cred64, 0x00, HTTP_CLIENT_MAX_64_ENCODED_CRED);

        switch (pHTTPSession->HttpHeadersInfo.nHTTPStatus) {
            case (HTTP_STATUS_UNAUTHORIZED): // For host authentication
            {
                // Copy the clear text credentials to a format of user:password
                strcpy(Cred, pHTTPSession->HttpCredentials.CredUser);
                strcat(Cred, ":");
                strcat(Cred, pHTTPSession->HttpCredentials.CredPassword);
                nSrcLength     = strlen(Cred);
                nDestLength    = HTTP_CLIENT_MAX_64_ENCODED_CRED;
                nSegmentLength = strlen(INITIAL_HDR);
                // Build and send the data first the hard-coded static portion
                if ((nRetCode = HTTPIntrnSend(pHTTPSession, INITIAL_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
                    break;
                }
                // Set the counters
                pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

                // Convert to base 64
                HTTPBase64Encoder((unsigned char *)Cred64, (const unsigned char *)Cred, nSrcLength);
                nDestLength = strlen(Cred64);

            }; break;
#if CONFIG_WM_HTTP_CLIENT_PROXY
            case (HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED): // For Proxy authentication
            {
                // Copy the clear text credentials to a format of user:password
                strcpy(Cred, pHTTPSession->HttpProxy.ProxtUser);
                strcat(Cred, ":");
                strcat(Cred, pHTTPSession->HttpProxy.ProxyPassword);
                nSrcLength  = strlen(Cred);
                nDestLength = HTTP_CLIENT_MAX_64_ENCODED_CRED;

                // Convert to base 64
                HTTPBase64Encoder((unsigned char *)Cred64, (unsigned char *)Cred, nSrcLength);
                nDestLength    = strlen(Cred64);
                nSegmentLength = strlen(INITIAL_PROXY_HDR);
                // Build and send the data first the hard-coded static portion
                if ((nRetCode = HTTPIntrnSend(pHTTPSession, INITIAL_PROXY_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
                    break;
                }
                // Set the counters
                pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

            }; break;
#endif //CONFIG_WM_HTTP_CLIENT_PROXY
            default:
            {
                wm_os_internal_free(Cred);
                wm_os_internal_free(Cred64);
                return HTTP_CLIENT_ERROR_BAD_AUTH; // Wrong status for this function
            };
        };

        // Send the base 64 encoded data
        pPtr = Cred64;
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, pPtr, &nDestLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nDestLength;

        // Terminating CRLF
        nSegmentLength = strlen(HTTP_CLIENT_CRLF);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, HTTP_CLIENT_CRLF, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

    } while (0);
    if (Cred != NULL) {
        wm_os_internal_free(Cred);
        wm_os_internal_free(Cred64);
    }
    return nRetCode;
}
#endif //CONFIG_WM_HTTP_CLIENT_AUTH_BASIC

#if CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnAuthSendDigest
// Purpose      : Handle digest authentication for direct host connection and proxy authentication
// Gets         :
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnAuthSendDigest(P_HTTP_SESSION pHTTPSession)
{
    char Cnonce[33];
    uint32_t nSegmentLength;
    uint32_t nRetCode;
    uint32_t nAlgType = 0; // a flag for the algorithem type (default to MD5)
    HTTP_PARAM HttpParamOpq, HttpParamRealm, HttpParamNonce, HttpParamQop,
        HttpParamAlg; // Pointers and lengths of the dynamic sections
    // of the Digest response.

    // Fragments of the Digest client response (The hard coded text portion of the response)
    char *INITIAL_HDR       = "Authorization: Digest username=\"";
    char *INITIAL_PROXY_HDR = "Proxy-Authorization: Digest username=\"";
    char *REALEM_HDR        = "\", realm=\"";
    char *QOP_HDR           = "\", qop=\"";
    char *ALGO_HDR          = "\", algorithm=\"";
    char *URI_HDR           = "\", uri=\"";
    char *NONCE_HDR         = "\", nonce=\"";
    char *NC_HDR            = "\", nc=00000001, cnonce=\""; // To-Do: This should be tested!!
    char *RSP_HDR           = "\", response=\"";
    char *OPQ_HDR           = "\", opaque=\"";
    // Digest Calculation related
    HASHHEX HA1;
    HASHHEX HA2 = "";
    HASHHEX Response;

    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnAuthSendDigest", NULL, 0, "");
    }
#endif

    // Generate random Cnonce number
    HTTPDigestGenerateCNonce(Cnonce);

    switch (pHTTPSession->HttpHeadersInfo.nHTTPStatus) {
        case (HTTP_STATUS_UNAUTHORIZED): // For host authentication
        {
            // "Authorization: Digest username="
            nSegmentLength = strlen(INITIAL_HDR);
            if ((nRetCode = HTTPIntrnSend(pHTTPSession, INITIAL_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
                return nRetCode;
            }
            // Set the counters
            pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;
        }; break;
#if CONFIG_WM_HTTP_CLIENT_PROXY
        case (HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED): // For Proxy authentication
        {
            // "Proxy-Authorization: Digest username="
            nSegmentLength = strlen(INITIAL_PROXY_HDR);
            if ((nRetCode = HTTPIntrnSend(pHTTPSession, INITIAL_PROXY_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
                return nRetCode;
            }
            // Set the counters
            pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;
        }; break;
#endif //CONFIG_WM_HTTP_CLIENT_PROXY
        default:
        {
            return HTTP_CLIENT_ERROR_BAD_AUTH; // Wrong status for this function
        };
    };

    do {
        // "Authorization: Digest username="username
        nSegmentLength = strlen(pHTTPSession->HttpCredentials.CredUser);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, pHTTPSession->HttpCredentials.CredUser, &nSegmentLength)) !=
            HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // "Authorization: Digest username="username", realm="
        nSegmentLength = strlen(REALEM_HDR);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, REALEM_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // "Authorization: Digest username="username", realm="realm
        if ((nRetCode = HTTPStrGetDigestToken(pHTTPSession->HttpAuthHeader.AuthHeader, "realm", &HttpParamRealm)) !=
            HTTP_CLIENT_SUCCESS) {
            break;
        }
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, HttpParamRealm.pParam, &HttpParamRealm.nLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += HttpParamRealm.nLength;

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        nSegmentLength = strlen(QOP_HDR);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, QOP_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        if ((nRetCode = HTTPStrGetDigestToken(pHTTPSession->HttpAuthHeader.AuthHeader, "qop", &HttpParamQop)) !=
            HTTP_CLIENT_SUCCESS) {
            break;
        }
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, HttpParamQop.pParam, &HttpParamQop.nLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += HttpParamQop.nLength;

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5",

        nSegmentLength = strlen(ALGO_HDR);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, ALGO_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        if ((nRetCode = HTTPStrGetDigestToken(pHTTPSession->HttpAuthHeader.AuthHeader, "algorithm", &HttpParamAlg)) !=
            HTTP_CLIENT_SUCCESS) {
            // The server did not state its required algorithm so use the default
            HttpParamAlg.pParam  = HTTP_CLIENT_DEFAULT_DIGEST_AUTH;
            HttpParamAlg.nLength = strlen(HTTP_CLIENT_DEFAULT_DIGEST_AUTH);
        }
        // Get the algorithem type
        if (HTTPStrInsensitiveCompare(HttpParamAlg.pParam, "md5", 3) == true) {
            if (HTTPStrInsensitiveCompare(HttpParamAlg.pParam, "md5-sess", HttpParamAlg.nLength) == true) {
                nAlgType = 1;
            }

        } else {
            // Error algorithem not supported
            nRetCode = HTTP_CLIENT_ERROR_NO_DIGEST_ALG;
            break;
        }

        // Send the algorithem
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, HttpParamAlg.pParam, &HttpParamAlg.nLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += HttpParamAlg.nLength;

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="
        nSegmentLength = strlen(URI_HDR);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, URI_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="/....Service
        nSegmentLength = strlen(pHTTPSession->HttpUrl.UrlRequest.pParam);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, pHTTPSession->HttpUrl.UrlRequest.pParam, &nSegmentLength)) !=
            HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="/....Service", nonce="
        nSegmentLength = strlen(NONCE_HDR);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, NONCE_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="/....Service", nonce="7a5c...
        if ((nRetCode = HTTPStrGetDigestToken(pHTTPSession->HttpAuthHeader.AuthHeader, "nonce", &HttpParamNonce)) !=
            HTTP_CLIENT_SUCCESS) {
            break;
        }
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, HttpParamNonce.pParam, &HttpParamNonce.nLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += HttpParamNonce.nLength;

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="/....Service", nonce="7a5c...", nc=00000001, cnonce="
        nSegmentLength = strlen(NC_HDR);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, NC_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="/....Service", nonce="7a5c...", nc=00000001, cnonce="ab341...
        nSegmentLength = strlen(Cnonce);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, Cnonce, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // Send the opaque data if we got it from the server
        if ((nRetCode = HTTPStrGetDigestToken(pHTTPSession->HttpAuthHeader.AuthHeader, "opaque", &HttpParamOpq)) ==
            HTTP_CLIENT_SUCCESS) {
            nSegmentLength = strlen(OPQ_HDR);
            if ((nRetCode = HTTPIntrnSend(pHTTPSession, OPQ_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
            // Set the counters
            pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;
            if ((nRetCode = HTTPIntrnSend(pHTTPSession, HttpParamOpq.pParam, &HttpParamOpq.nLength)) != HTTP_CLIENT_SUCCESS) {
                break;
            }
            // Set the counters
            pHTTPSession->HttpCounters.nSentHeaderBytes += HttpParamOpq.nLength;
        }

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="/....Service", nonce="7a5c...", nc=00000001, cnonce="ab341...", response="
        nSegmentLength = strlen(RSP_HDR);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, RSP_HDR, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // Calculate response
        HTTPDigestCalcHA1(nAlgType, pHTTPSession->HttpCredentials.CredUser, HttpParamRealm.pParam, HttpParamRealm.nLength,
                          pHTTPSession->HttpCredentials.CredPassword, HttpParamNonce.pParam, HttpParamNonce.nLength, Cnonce,
                          HA1);

        HTTPDigestCalcResponse(HA1, HttpParamNonce.pParam, HttpParamNonce.nLength, "00000001", Cnonce, HttpParamQop.pParam,
                               HttpParamQop.nLength, pHTTPSession->HttpHeaders.Verb, pHTTPSession->HttpUrl.UrlRequest.pParam,
                               pHTTPSession->HttpUrl.UrlRequest.nLength, HA2, Response);

        // "Authorization: Digest username="username", realm="myRealm", qop="auth",
        // algorithm="MD5", uri="/....Service", nonce="7a5c...", nc=00000001, cnonce="ab341...", response="8bbf2...
        nSegmentLength = strlen(Response);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, Response, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // Terminate 0x24 (")
        nSegmentLength = 1;
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, "\"", &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

        // Terminating CRLF
        nSegmentLength = strlen(HTTP_CLIENT_CRLF);
        if ((nRetCode = HTTPIntrnSend(pHTTPSession, HTTP_CLIENT_CRLF, &nSegmentLength)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the counters
        pHTTPSession->HttpCounters.nSentHeaderBytes += nSegmentLength;

    } while (0);

    return nRetCode; // End of digest respobse sending
}
#endif //CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST
#endif //(CONFIG_WM_HTTP_CLIENT_AUTH_BASIC || CONFIG_WM_HTTP_CLIENT_AUTH_DIGEST)
///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnSessionReset
// Purpose      : Reset the session data for the next operation
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnSessionReset(P_HTTP_SESSION pHTTPSession, bool EntireSession)
{
    uint32_t nActionTimeout; // For restoring a parameter after this reset
    uint32_t nAllocationSize;

    // Validate the pointer
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

#if CONFIG_WM_HTTP_CLIENT_DEBUG
    if (pHTTPSession->pDebug) {
        pHTTPSession->pDebug("HTTPIntrnSessionReset", NULL, 0, "");
    }
#endif

    memset(pHTTPSession->HttpHeaders.HeadersIn.pParam, 0x00, pHTTPSession->HttpHeaders.HeadersIn.nLength);
    pHTTPSession->HttpHeaders.HeadersIn.nLength = 0;

    // Reset the HTTP counters
    nActionTimeout = pHTTPSession->HttpCounters.nActionTimeout;
    memset(&pHTTPSession->HttpCounters, 0x00, sizeof(HTTP_COUNTERS));
    pHTTPSession->HttpCounters.nActionStartTime = HTTPIntrnSessionGetUpTime();
    // Restore the parameter
    pHTTPSession->HttpCounters.nActionTimeout = nActionTimeout;
    // Reset the authentication flag
    pHTTPSession->HttpCredentials.Authentication = false;

    if (EntireSession == true) // Partial reset, clear only the incoming headers
    {
        if (pHTTPSession->HttpUrl.Url != NULL) {
            wm_os_internal_free(pHTTPSession->HttpUrl.Url);
            pHTTPSession->HttpUrl.Url = NULL;
        }
        memset(&pHTTPSession->HttpUrl, 0, sizeof(HTTP_URL));
        nAllocationSize = pHTTPSession->HttpHeaders.HeadersBuffer.nLength;
        // Reset the headers allocated memory
        memset(pHTTPSession->HttpHeaders.HeadersBuffer.pParam, 0x00, nAllocationSize);

        // Set default values in the session structure
        HTTPClientSetVerb((uint32_t)pHTTPSession, (HTTP_VERB)HTTP_CLIENT_DEFAULT_VERB); // Default HTTP verb
        pHTTPSession->HttpUrl.nPort = HTTP_CLIENT_DEFAULT_PORT;                         // Default TCP port
        // Set the outgoing headers pointers
        memset(&pHTTPSession->HttpHeaders.HeadersIn, 0, sizeof(HTTP_PARAM));
        memset(&pHTTPSession->HttpHeaders.HeadersOut, 0, sizeof(HTTP_PARAM));

        pHTTPSession->HttpHeaders.HeadersOut.pParam = pHTTPSession->HttpHeaders.HeadersBuffer.pParam;
        // Set our state
        pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_INIT;

        memset(&pHTTPSession->HttpHeadersInfo, 0, sizeof(HTTP_HEADERS_INFO));
        if (pHTTPSession->HttpConnection.HttpSocket != HTTP_INVALID_SOCKET) {
            pHTTPSession->HttpHeadersInfo.Connection = true;
        }
        memset(&pHTTPSession->HttpAuthHeader, 0, sizeof(HTTP_AUTH_HEADER));
#if CONFIG_WM_HTTP_CLIENT_PROXY
        memset(&pHTTPSession->HttpProxy, 0, sizeof(HTTP_PROXY));
#endif //CONFIG_WM_HTTP_CLIENT_PROXY
    }

    return HTTP_CLIENT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnHeadersReceive
// Purpose      : Receives the response header on the connection and parses it.
//                Performs any required authentication.
// Returns      : HTTP Status
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnHeadersReceive(P_HTTP_SESSION pHTTPSession,
                                        uint32_t nTimeout) // [IN] Timeout for the operation

{
    uint32_t nRetCode; // Function call return code
    uint32_t nCount = 0;
    if (!pHTTPSession) {
        // Bad session pointer error
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }

    do {
        // Set the operation time out if was set by the caller
        if (nTimeout > 0) {
            // 0 makes us use the default defined value
            pHTTPSession->HttpCounters.nActionTimeout = HTTP_TIMEOUT(nTimeout);
        }
        // Reset the incoming headers
        if ((nRetCode = HTTPIntrnSessionReset(pHTTPSession, false)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Get the server response
        if ((nRetCode = HTTPIntrnGetRemoteHeaders(pHTTPSession)) != HTTP_CLIENT_SUCCESS) {
            break;
        }
        // Set the session state
        pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_HEADERS_RECIVED;

        // Parse the response headers
        if ((nRetCode = HTTPIntrnHeadersParse(pHTTPSession)) != HTTP_CLIENT_SUCCESS) {
            break;
        }

        wm_http_client_event_param_t event = { 0 };
        event.data                         = pHTTPSession->HttpHeaders.HeadersIn.pParam;
        event.data_len                     = pHTTPSession->HttpHeaders.HeadersIn.nLength;

        call_http_event_handle(pHTTPSession, WM_HTTP_CLIENT_EVENT_RECV_HEADER, &event);

        // Set the session state
        pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_HEADERS_PARSED;

        // Set the session stage upon seccess
        if (pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_OK) {
            pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_HEADERS_OK;
        }
        // Handle 100 continue message
        if (pHTTPSession->HttpHeadersInfo.nHTTPStatus != HTTP_STATUS_CONTINUE) {
            nCount++;
        }
#if CONFIG_WM_HTTP_CLIENT_DEBUG
        if (pHTTPSession->pDebug) {
            if (pHTTPSession->HttpHeadersInfo.nHTTPStatus == HTTP_STATUS_CONTINUE) {
                pHTTPSession->pDebug("HTTPIntrnHeadersReceive", NULL, 0, "100 Continue Header");
            }
        }
#endif
    } while (nCount < 1);

    return nRetCode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnGetTicks
// Purpose      : Like GetTickCount() (implemented with time.h)
// Gets         : void
// Returns      : System ticks
// Last updated : 01/09/200515/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : Assuming 1000 ticks per sec, should be implemented by the OS
//
///////////////////////////////////////////////////////////////////////////////

static uint32_t HTTPIntrnSessionGetUpTime(void)
{
    return GetUpTime();
}

///////////////////////////////////////////////////////////////////////////////
//
// Function     : HTTPIntrnSessionEvalTimeout
// Purpose      : Check if we have to break the operation and return a time out error
// Gets         : a pointer to the session structure
// Returns      : bool, true if we have to break
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

static bool HTTPIntrnSessionEvalTimeout(P_HTTP_SESSION pHTTPSession)
{
    uint32_t nElapsedTime; // Integer for calculating the elapsed time

    // Validate the session pointer
    if (!pHTTPSession) {
        return HTTP_CLIENT_ERROR_INVALID_HANDLE;
    }
    // Calculate the elapsed time since the last call
    nElapsedTime = HTTPIntrnSessionGetUpTime() - pHTTPSession->HttpCounters.nActionStartTime;
    // If the elapsed time is greater then the time out value we should return true
    if (nElapsedTime >= pHTTPSession->HttpCounters.nActionTimeout) {
        return true;
    }

    return false;
}

struct http_error_code {
    int error_code;
    const char *error_msg;
};

const char *HTTP_error_code_2_string(int error_code)
{
    static struct http_error_code http_err[] = {
        { HTTP_CLIENT_SUCCESS,                "HTTP Success status"                                                                        },
        { HTTP_CLIENT_UNKNOWN_ERROR,          "Unknown error"                                                                              },
        { HTTP_CLIENT_ERROR_INVALID_HANDLE,   "an Invalid handle or possible bad pointer was passed to a function"                         },
        { HTTP_CLIENT_ERROR_NO_MEMORY,        "Buffer too small or a failure while in memory allocation"                                   },
        { HTTP_CLIENT_ERROR_SOCKET_INVALID,   "an attempt to use an invalid socket handle was made"                                        },
        { HTTP_CLIENT_ERROR_SOCKET_CANT_SET,  "Can't send socket parameters"                                                               },
        { HTTP_CLIENT_ERROR_SOCKET_RESOLVE,   "Error while resolving host name"                                                            },
        { HTTP_CLIENT_ERROR_SOCKET_CONNECT,   "Error while connecting to the remote server"                                                },
        { HTTP_CLIENT_ERROR_SOCKET_TIME_OUT,  "Socket time out error"                                                                      },
        { HTTP_CLIENT_ERROR_SOCKET_RECV,      "Error while receiving data"                                                                 },
        { HTTP_CLIENT_ERROR_SOCKET_SEND,      "Error while sending data"                                                                   },
        { HTTP_CLIENT_ERROR_HEADER_RECV,      "Error while receiving the remote HTTP headers"                                              },
        { HTTP_CLIENT_ERROR_HEADER_NOT_FOUND, "Could not find element within header"                                                       },
        { HTTP_CLIENT_ERROR_HEADER_BIG_CLUE,  "The headers search clue was too large for the internal API buffer"                          },
        { HTTP_CLIENT_ERROR_HEADER_NO_LENGTH, "No content length was specified for the outgoing data. the caller should "
                                              "specify chunking mode in the session creation" },
        { HTTP_CLIENT_ERROR_CHUNK_TOO_BIG,    "The HTTP chunk token that was received from the server was too big and possibly "
                                           "wrong"                                               },
        { HTTP_CLIENT_ERROR_AUTH_HOST,        "Could not authenticate with the remote host"                                                },
        { HTTP_CLIENT_ERROR_AUTH_PROXY,       "Could not authenticate with the remote proxy"                                               },
        { HTTP_CLIENT_ERROR_BAD_VERB,         "Bad or not supported HTTP verb was passed to a function"                                    },
        { HTTP_CLIENT_ERROR_LONG_INPUT,       "A function received a parameter that was too large"                                         },
        { HTTP_CLIENT_ERROR_BAD_STATE,        "The session state prevents the current function from proceeding"                            },
        { HTTP_CLIENT_ERROR_CHUNK,            "Could not parse the chunk length while in chunked transfer"                                 },
        { HTTP_CLIENT_ERROR_BAD_URL,          "Could not parse curtail elements from the URL (such as the host name, HTTP prefix "
                                     "act, or not support HTTPS)"                                      },
        { HTTP_CLIENT_ERROR_BAD_HEADER,       "Could not detect key elements in the received headers"                                      },
        { HTTP_CLIENT_ERROR_BUFFER_RSIZE,     "Error while attempting to resize a buffer"                                                  },
        { HTTP_CLIENT_ERROR_BAD_AUTH,         "Authentication schema is not supported"                                                     },
        { HTTP_CLIENT_ERROR_AUTH_MISMATCH,    "The selected authentication schema does not match the server response"                      },
        { HTTP_CLIENT_ERROR_NO_DIGEST_TOKEN,  "an element was missing while parsing the digest authentication challenge"                   },
        { HTTP_CLIENT_ERROR_NO_DIGEST_ALG,    "Digest algorithem could be MD5 or MD5-sess other types are not supported"                   },
        { HTTP_CLIENT_ERROR_SOCKET_BIND,      "Binding error"                                                                              },
        { HTTP_CLIENT_ERROR_TLS_NEGO,         "Tls negotiation error"                                                                      },
        { HTTP_CLIENT_ERROR_NOT_IMPLEMENTED,  "Feature is not (yet) implemented"                                                           },
        { HTTP_CLIENT_ERROR_CONNECTION_CLOSE, "Connection closed."                                                                         },
        { HTTP_CLIENT_EOS,                    "HTTP end of stream message"                                                                 },
    };

    for (int i = 0; i < sizeof(http_err) / sizeof(http_err[0]); i++) {
        if (http_err[i].error_code == error_code) {
            return http_err[i].error_msg;
        }
    }

    return "Unknown error";
}

#endif //CONFIG_COMPONENT_HTTP_CLIENT_ENABLED
