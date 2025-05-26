/* lws_config.h  Generated from lws_config.h.in  */

#include "wmsdk_config.h"

#ifndef __LWS_CONFIG_H__
#define __LWS_CONFIG_H__

#define LWS_INSTALL_DATADIR ""
#define LWS_INSTALL_LIBDIR ""
#define LWS_LIBRARY_VERSION_MAJOR 4
#define LWS_LIBRARY_VERSION_MINOR 3
#define LWS_LIBRARY_VERSION_PATCH 3

/* LWS_LIBRARY_VERSION_NUMBER looks like 1005001 for e.g. version 1.5.1 */
#define LWS_LIBRARY_VERSION_NUMBER ((LWS_LIBRARY_VERSION_MAJOR * 1000000) + (LWS_LIBRARY_VERSION_MINOR * 1000) + LWS_LIBRARY_VERSION_PATCH)
#define LWS_MAX_SMP 1

#define LWS_BUILD_HASH "WM"

#define LWS_HAS_INTPTR_T
#define LWS_HAVE_ATOLL
#define LWS_HAVE_CLOCK_GETTIME
#define LWS_HAVE_LOCALTIME_R
#define LWS_HAVE_GMTIME_R
#define LWS_HAVE_CTIME_R
#define LWS_HAVE_GETGRGID_R
#define LWS_HAVE_GETGRNAM_R
#define LWS_HAVE_GETPWUID_R
#define LWS_HAVE_GETPWNAM_R
#define LWS_HAVE_STDINT_H
#define LWS_HAVE_SYS_TYPES_H
#define LWS_HAVE_SUSECONDS_T
#define LWS_HAVE_VFORK
#define LWS_LIBRARY_VERSION "4.3.3-WM"
#define LWS_LOGGING_BITFIELD_CLEAR 0
#define LWS_LOGGING_BITFIELD_SET 0
#define LWS_LOG_TAG_LIFECYCLE
#define LWS_NO_DAEMONIZE
#define LWS_PLAT_FREERTOS
#define LWS_AMAZON_RTOS
#define LWS_ROLE_H1
#define LWS_ROLE_RAW
#define LWS_ROLE_RAW_FILE
#define LWS_ROLE_WS
#define LWS_SUPPRESS_DEPRECATED_API_WARNINGS
#define LWS_WITH_CONMON
#define LWS_WITH_CUSTOM_HEADERS
#define LWS_WITH_HTTP_BASIC_AUTH
#define LWS_WITH_HTTP_UNCOMMON_HEADERS
#define LWS_WITH_LWSAC
#define LWS_LOGS_TIMESTAMP
#define LWS_WITH_NETWORK
#define LWS_WITH_CLIENT
#define LWS_WITHOUT_EXTENSIONS
#define LWS_WITH_SERVER
#define LWS_WITH_POLL
#define LWS_WITH_SYS_STATE
#define LWS_WITHOUT_TEST_SERVER
#define LWS_WITHOUT_TESTAPPS

#define LWS_HAVE_mbedtls_md_setup
#define LWS_HAVE_mbedtls_net_init
#define LWS_HAVE_mbedtls_rsa_complete
#define LWS_HAVE_mbedtls_internal_aes_encrypt
#define LWS_HAVE_mbedtls_ssl_conf_alpn_protocols
#define LWS_HAVE_mbedtls_ssl_get_alpn_protocol
#define LWS_HAVE_mbedtls_ssl_conf_sni
#define LWS_HAVE_mbedtls_ssl_set_hs_ca_chain
#define LWS_HAVE_mbedtls_ssl_set_hs_own_cert
#define LWS_HAVE_mbedtls_ssl_set_hs_authmode
#define LWS_HAVE_mbedtls_ssl_set_verify
#define LWS_HAVE_MBEDTLS_SSL_NEW_SESSION_TICKET

#ifdef CONFIG_WEBSOCKET_SECURITY
	#define LWS_WITH_TLS
	#define LWS_WITH_MBEDTLS
	#define LWS_HAVE_MBEDTLS_NET_SOCKETS
	#define LWS_WITH_GENCRYPTO
	#define LWS_WITH_TLS_SESSIONS
#endif

#if !CONFIG_LOG_DEFAULT_LEVEL
	#define LWS_WITH_NO_LOGS
#endif

#ifdef CONFIG_LWIP_ENABLE_IPV6
#define LWS_WITH_IPV6
#define LWS_IPV6
#define NI_MAXHOST 50
#endif

#define LWS_WITHOUT_LOG_COLOR
#define LWS_WITH_WINNERMICRO

#endif
