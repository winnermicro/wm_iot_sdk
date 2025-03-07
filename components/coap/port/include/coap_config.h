#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WITH_POSIX
#define WITH_POSIX
#endif

#include "wmsdk_config.h"
#include <assert.h>
#include "coap_config_posix.h"
#include "mbedtls/mbedtls_config.h"
typedef unsigned char u_char;

#define HAVE_STDIO_H
#define HAVE_ASSERT_H
//#define HAVE_INTTYPES_H

#define PACKAGE_STRING PACKAGE_NAME PACKAGE_VERSION

/* it's just provided by libc. i hope we don't get too many of those, as
 * actually we'd need autotools again to find out what environment we're
 * building in */
#define HAVE_STRNLEN   1

#define HAVE_LIMITS_H
#define COAP_RESOURCES_NOHASH
#define COAP_DTLS_RETRANSMIT_MS       25000
#define COAP_DTLS_RETRANSMIT_TOTAL_MS 80000

/* Note: If neither of COAP_CLIENT_SUPPORT or COAP_SERVER_SUPPORT is set,
   then libcoap sets both for backward compatability */
#ifdef CONFIG_COAP_CLIENT_SUPPORT
#define COAP_CLIENT_SUPPORT 1
#endif /* CONFIG_COAP_CLIENT_SUPPORT */

#ifdef CONFIG_COAP_SERVER_SUPPORT
#define COAP_SERVER_SUPPORT 1
#endif /* CONFIG_COAP_SERVER_SUPPORT */

#ifdef CONFIG_LWIP_ENABLE_IPV4
#define COAP_IPV4_SUPPORT 1
#endif /* CONFIG_LWIP_IPV4 */

#ifdef CONFIG_LWIP_ENABLE_IPV6
#define COAP_IPV6_SUPPORT 1
#endif /* CONFIG_LWIP_IPV6 */

#ifdef CONFIG_COAP_TCP_SUPPORT
#define COAP_DISABLE_TCP 0
#else /* ! CONFIG_COAP_TCP_SUPPORT */
#define COAP_DISABLE_TCP 1
#endif /* ! CONFIG_COAP_TCP_SUPPORT */

#ifdef CONFIG_COAP_OSCORE_SUPPORT
#define COAP_OSCORE_SUPPORT 1
#else /* ! CONFIG_COAP_OSCORE_SUPPORT */
#define COAP_OSCORE_SUPPORT 0
#endif /* ! CONFIG_COAP_OSCORE_SUPPORT */

#ifdef CONFIG_COAP_WEBSOCKETS
#define COAP_WS_SUPPORT 1
#else /* ! CONFIG_COAP_WEBSOCKETS */
#define COAP_WS_SUPPORT 0
#endif /* ! CONFIG_COAP_WEBSOCKETS */

#ifdef CONFIG_COAP_OBSERVE_PERSIST
#define COAP_WITH_OBSERVE_PERSIST 1
#else /* ! CONFIG_COAP_OBSERVE_PERSIST */
#define COAP_WITH_OBSERVE_PERSIST 0
#endif /* ! CONFIG_COAP_OBSERVE_PERSIST */

#ifdef CONFIG_COAP_Q_BLOCK
#define COAP_Q_BLOCK_SUPPORT 1
#else /* ! CONFIG_COAP_Q_BLOCK */
#define COAP_Q_BLOCK_SUPPORT 0
#endif /* ! CONFIG_COAP_Q_BLOCK */

#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
