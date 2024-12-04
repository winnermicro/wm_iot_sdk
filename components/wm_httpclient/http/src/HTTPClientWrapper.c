#include "wmsdk_config.h"
#include "wm_osal.h"
#include "lwip/netdb.h"
#if CONFIG_WM_HTTP_CLIENT_SECURE
#include "HTTPClient.h"
#endif
#if CONFIG_COMPONENT_HTTP_CLIENT_ENABLED
#include "HTTPClientAuth.h"
#include "HTTPClientWrapper.h"

#define LOG_TAR "httpclient"
#include "wm_log.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: HTTPWrapperToUpper
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : Convert character to uppercase.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperToUpper(int c)
{
    // -32
    if (isascii(c) > 0) {
        if (c >= 97 && c <= 122) {
            return (c - 32);
        }
    }

    return c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: HTTPWrapperToLower
// Last updated : 13/06/2006
// Author Name	 : Eitan Michaelson
// Notes	       : Convert character to lowercase.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperToLower(int c)
{
    // +32
    if (isascii(c) > 0) {
        if (c >= 65 && c <= 90) {
            return (c + 32);
        }
    }

    return c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: isalpha
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : returns nonzero if c is a particular representation of an alphabetic character
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperIsAlpha(int c)
{
    if (isascii(c) > 0) {
        if ((c >= 97 && c <= 122) || (c >= 65 && c <= 90)) {
            return c;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : Stdc: isalnum
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : returns nonzero if c is a particular representation of an alphanumeric character
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperIsAlNum(int c)
{
    if (isascii(c) > 0) {
        if (HTTPWrapperIsAlpha(c) > 0) {
            return c;
        }

        if (c >= 48 && c <= 57) {
            return c;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_itoa
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : same as stdc itoa() // hmm.. allmost the same
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *HTTPWrapperItoa(char *s, int a)
{
    unsigned int b;
    if (a > 2147483647) {
        return 0; // overflow
    }

    if (a < 0)
        b = -a, *s++ = '-';
    else
        b = a;
    for (; a; a = a / 10)
        s++;
    for (*s = '\0'; b; b = b / 10)
        *--s = b % 10 + '0';
    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_ShutDown
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : Handles parameter changes in the socket shutdown() function in AMT
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperShutDown(int s, int how)
{
    return shutdown(s, how);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetSocketError
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : WSAGetLastError Wrapper (Win32 Specific)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperGetSocketError(int s)
{
    return errno;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetRandomeNumber
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : GetRandom number for Win32 & AMT
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HTTPWrapperInitRandomeNumber()
{
    srand((unsigned int)wm_os_internal_get_time_ms());
}

int HTTPWrapperGetRandomeNumber()
{
#if 1
    return 0x12f3123;
#else
    int num;
    num = (int)(((double)os_random() / ((double)RAND_MAX + 1)) * 16);
    return num;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetRTC
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : Get uptime under Win32 & AMT
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long HTTPWrapperGetUpTime()
{
    long lTime = 0;

    lTime = wm_os_internal_get_time_ms();
    return lTime;
}

#endif //CONFIG_COMPONENT_HTTP_CLIENT_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Section      : HTTPWrapper_GetHostByName
// Last updated : 15/05/2005
// Author Name	: Eitan Michaelson
// Notes	    : gethostbyname for Win32 (supports the AMT edition of the function)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long HTTPWrapperGetHostByName(char *name, unsigned long *address)
{
    int iPos = 0, iLen = 0, iNumPos = 0, iDots = 0;
    long iIPElement;
    char c = 0;
    char Num[4];
    int iHostType = 0; // 0 : numeric IP

    // Check if the name is an IP or host
    iLen = strlen(name);
    for (iPos = 0; iPos <= iLen; iPos++) {
        c = name[iPos];
        if ((c >= 48 && c <= 57) || (c == '.')) {
            // c is numeric or dot
            if (c != '.') {
                // c is numeric
                if (iNumPos > 3) {
                    iHostType++;
                    break;
                }
                Num[iNumPos]     = c;
                Num[iNumPos + 1] = 0;
                iNumPos++;
            } else {
                iNumPos = 0;
                iDots++;
                iIPElement = atol(Num);
                if (iIPElement > 256 || iDots > 3) {
                    return 0; // error invalid IP
                }
            }
        } else {
            break; // this is an alpha numeric address type
        }
    }

    if (c == 0 && iHostType == 0 && iDots == 3) {
        iIPElement = atol(Num);
        if (iIPElement > 256) {
            return 0; // error invalid IP
        }
    } else {
        iHostType++;
    }

    if (iHostType > 0) {
        int rc                  = -1;
        struct addrinfo *result = NULL;
        struct addrinfo hints   = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL };

        rc = getaddrinfo(name, NULL, &hints, &result);
        if (rc == 0) {
            struct addrinfo *res = result;

            while (res) {
                if (res->ai_family == AF_INET) {
                    *address = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
                    break;
                }
                res = res->ai_next;
            }

            freeaddrinfo(result);
            return 1;
        } else {
            return 0;
        }
    } else // numeric address - no need for DNS resolve
    {
        *(address) = inet_addr(name);
        return 1;
    }
}

#if CONFIG_WM_HTTP_CLIENT_SECURE
#define ALLOW_ANON_CONNECTIONS 1
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HTTPWrapperSSLNegotiate(HTTP_SESSION_HANDLE pSession, int s, const struct sockaddr *name, int namelen, char *hostname)
{
    return 0;
}

//static bool mbedtls_demo_inited = FALSE;

#if CONFIG_MBEDTLS_ENABLE_DEBUG

#define DEBUG_LEVEL WM_LOG_LEVEL_DEBUG

static void wm_ssl_client_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void)level);

    wm_log_printf(level, LOG_TAR, "%s:%04d: %s", file, line, str);
}
#else
#ifdef mbedtls_printf
#undef mbedtls_printf
#define mbedtls_printf(...)
#endif
#endif

int HTTPWrapperSSLConnect(tls_ssl_t **ssl_p, int fd, const struct sockaddr *name, int namelen, char *hostname)
{
    int ret          = MBEDTLS_EXIT_SUCCESS;
    const char *pers = "ssl_client";
    tls_ssl_t *ssl   = NULL;

    *ssl_p = NULL;

    ssl = wm_os_internal_malloc(sizeof(tls_ssl_t));
    if (!ssl) {
        return -1;
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
#if MBEDTLS_DEMO_USE_CERT
    mbedtls_x509_crt_init(&ssl->cacert);
#endif
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);

    mbedtls_printf("\n  . Seeding the random number generator...");
    fflush(stdout);

    mbedtls_entropy_init(&ssl->entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg, mbedtls_entropy_func, &ssl->entropy, (const unsigned char *)pers,
                                     strlen(pers))) != 0) {
        mbedtls_printf(" failed\n	! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

#if MBEDTLS_DEMO_USE_CERT
    /*
	 * 0. Initialize certificates
	 */
    mbedtls_printf("  . Loading the CA root certificate ...");
    fflush(stdout);

    ret = mbedtls_x509_crt_parse(&ssl->cacert, (const unsigned char *)mbedtls_demos_pem, strlen((char *)mbedtls_demos_pem) + 1);
    if (ret < 0) {
        mbedtls_printf(" failed\n	!  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        goto exit;
    }

    mbedtls_printf(" ok (%d skipped)\n", ret);
#endif

    /*
	 * 1. Start the connection
	 */
    mbedtls_printf("  . Connecting to tcp...");
    fflush(stdout);

    if (name) {
        ret = connect(fd,   // Socket
                      name, // Server address
                      sizeof(struct sockaddr));
        if (ret)
            ret = SocketGetErr(fd);
        //TLS_DBGPRT_INFO("SocketGetErr rc %d\n", rc);
        if (ret == 0 || ret == HTTP_EWOULDBLOCK || ret == HTTP_EINPROGRESS) {
        } else {
            mbedtls_printf("Connection Failed: %d.  Exiting\n", ret);
            goto exit;
        }
    }

    mbedtls_printf(" ok\n");
    ssl->server_fd.fd = fd;

    /*
	 * 2. Setup stuff
	 */
    mbedtls_printf("  . Setting up the SSL/TLS structure...");
    fflush(stdout);

    if ((ret = mbedtls_ssl_config_defaults(&ssl->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        mbedtls_printf(" failed\n	! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /* OPTIONAL is not optimal for security,
	 * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(&ssl->conf, MBEDTLS_SSL_VERIFY_NONE);
    //mbedtls_ssl_conf_authmode( &ssl->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#if MBEDTLS_DEMO_USE_CERT
    mbedtls_ssl_conf_ca_chain(&ssl->conf, &ssl->cacert, NULL);
#endif
    mbedtls_ssl_conf_rng(&ssl->conf, mbedtls_ctr_drbg_random, &ssl->ctr_drbg);

#if defined(CONFIG_MBEDTLS_ENABLE_DEBUG)
    mbedtls_ssl_conf_dbg(&ssl->conf, wm_ssl_client_debug, stdout);
#endif

    mbedtls_ssl_conf_read_timeout(&ssl->conf, 5000);

    if ((ret = mbedtls_ssl_setup(&ssl->ssl, &ssl->conf)) != 0) {
        mbedtls_printf(" failed\n	! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = mbedtls_ssl_set_hostname(&ssl->ssl, hostname)) != 0) {
        mbedtls_printf(" failed\n	! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto exit;
    }

    //mbedtls_ssl_set_bio( &ssl->ssl, &ssl->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );
    mbedtls_ssl_set_bio(&ssl->ssl, &ssl->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
	 * 4. Handshake
	 */
    mbedtls_printf("  . Performing the SSL/TLS handshake...");
    fflush(stdout);

    while ((ret = mbedtls_ssl_handshake(&ssl->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n	! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            goto exit;
        }
    }

    mbedtls_printf(" ok\n");

    *ssl_p = ssl;

    return 0;
exit:

#ifdef MBEDTLS_ERROR_C
    if (ret != MBEDTLS_EXIT_SUCCESS) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, 100);
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf);
    }
#endif

    mbedtls_net_free(&ssl->server_fd);
#if MBEDTLS_DEMO_USE_CERT
    mbedtls_x509_crt_free(&ssl->cacert);
#endif
    mbedtls_ssl_free(&ssl->ssl);
    mbedtls_ssl_config_free(&ssl->conf);
    mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
    mbedtls_entropy_free(&ssl->entropy);
    mbedtls_printf("start free ssl(%x)...\n", (unsigned int)ssl);
    wm_os_internal_free(ssl);
    mbedtls_printf("HTTPWrapperSSLConnect ret %d\n", ret);
    return (ret);
}
int HTTPWrapperSSLSend(tls_ssl_t *ssl, int s, char *sndbuf, int len, int flags)
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
int HTTPWrapperSSLRecv(tls_ssl_t *ssl, int s, char *buf, int len, int flags)
{
    int ret;

    do {
        ret = mbedtls_ssl_read(&ssl->ssl, (unsigned char *)buf, len);
        //mbedtls_printf("mbedtls_ssl_read ret %d\n", ret);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY || ret == MBEDTLS_ERR_NET_CONN_RESET) {
            ret = 0;
            break;
        }

        if (ret < 0) {
            //mbedtls_printf( "failed\n  ! mbedtls_ssl_read returned %d\n\n", ret );
            ret = SOCKET_ERROR;
            break;
        }

        if (ret == 0) {
            mbedtls_printf("\n\nEOF\n\n");
            break;
        }

    } while (ret < 0);

    if (mbedtls_ssl_get_bytes_avail(&ssl->ssl) > 0)
        return SOCKET_SSL_MORE_DATA;

    return ret;
}
int HTTPWrapperSSLRecvPending(tls_ssl_t *ssl)
{
    return 0;
}
int HTTPWrapperSSLClose(tls_ssl_t *ssl, int s)
{
    if (ssl) {
        mbedtls_ssl_close_notify(&ssl->ssl);

        mbedtls_net_free(&ssl->server_fd);
#if MBEDTLS_DEMO_USE_CERT
        mbedtls_x509_crt_free(&ssl->cacert);
#endif
        mbedtls_ssl_free(&ssl->ssl);
        mbedtls_ssl_config_free(&ssl->conf);
        mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
        mbedtls_entropy_free(&ssl->entropy);

        wm_os_internal_free(ssl);
    }
    return 0;
}
#endif //CONFIG_WM_HTTP_CLIENT_SECURE
