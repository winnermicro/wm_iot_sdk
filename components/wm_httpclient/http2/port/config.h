#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "wmsdk_config.h"

/* Hint to the compiler that a function never returns */
#define NGHTTP2_NORETURN

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

/* Define to 1 if you have the `std::map::emplace`. */
/* #undef HAVE_STD_MAP_EMPLACE */

/* Define to 1 if you have `libjansson` library. */
/* #undef HAVE_JANSSON */

/* Define to 1 if you have `libxml2` library. */
/* #undef HAVE_LIBXML2 */

/* Define to 1 if you have `mruby` library. */
/* #undef HAVE_MRUBY */

/* Define to 1 if you have `neverbleed` library. */
/* #undef HAVE_NEVERBLEED */

/* sizeof(int *) */
/* #undef SIZEOF_INT_P */

/* sizeof(time_t) */
/* #undef SIZEOF_TIME_T */

/* Define to 1 if you have the `_Exit` function. */
/* #undef HAVE__EXIT */

/* Define to 1 if you have the `accept4` function. */
/* #undef HAVE_ACCEPT4 */

/* Define to 1 if you have the `clock_gettime` function. */
/* #undef HAVE_CLOCK_GETTIME */

/* Define to 1 if you have the `mkostemp` function. */
/* #undef HAVE_MKOSTEMP */

/* Define to 1 if you have the `GetTickCount64` function. */
/* #undef HAVE_GETTICKCOUNT64 */

/* Define to 1 if you have the `initgroups` function. */
#define HAVE_DECL_INITGROUPS      0

/* Define to 1 if you have the `CLOCK_MONOTONIC` defined. */
#define HAVE_DECL_CLOCK_MONOTONIC 0

/* Define to 1 to enable debug output. */
/* #undef DEBUGBUILD */
#if CONFIG_WM_HTTP2_CLIENT_DEBUG_ENABLED
#define DEBUGBUILD
#endif
/* Define to 1 if you want to disable threads. */
/* #undef NOTHREADS */

/* Define to 1 if you have the <arpa/inet.h> header file. */
/* #undef HAVE_ARPA_INET_H */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
/* #undef HAVE_FCNTL_H */

/* Define to 1 if you have the <inttypes.h> header file. */
/* #undef HAVE_INTTYPES_H */

/* Define to 1 if you have the <limits.h> header file. */
/* #undef HAVE_LIMITS_H */

/* Define to 1 if you have the <netdb.h> header file. */
/* #undef HAVE_NETDB_H */

/* Define to 1 if you have the <netinet/in.h> header file. */
/* #undef HAVE_NETINET_IN_H */

/* Define to 1 if you have the <netinet/ip.h> header file. */
/* #undef HAVE_NETINET_IP_H */

/* Define to 1 if you have the <pwd.h> header file. */
/* #undef HAVE_PWD_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
/* #undef HAVE_SYS_SOCKET_H */

/* Define to 1 if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */

/* Define to 1 if you have the <sysinfoapi.h> header file. */
/* #undef HAVE_SYSINFOAPI_H */

/* Define to 1 if you have the <syslog.h> header file. */
/* #undef HAVE_SYSLOG_H */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H      1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define to 1 if HTTP/3 is enabled. */
/* #undef ENABLE_HTTP3 */

/* Define to 1 if you have `libbpf` library. */
/* #undef HAVE_LIBBPF */

/* Define to 1 if you have enum bpf_stats_type in linux/bpf.h. */
/* #undef HAVE_BPF_STATS_TYPE */

/* Define to 1 if you have `libngtcp2_crypto_quictls` library. */
/* #undef HAVE_LIBNGTCP2_CRYPTO_QUICTLS */

/* Define to 1 if you have `libev` library. */
/* #undef HAVE_LIBEV */

#endif /* __CONFIG_H__ */
