#ifndef __LWIP_OPTS_H
#define __LWIP_OPTS_H

#include <string.h>
#include <stdlib.h>

#include "wmsdk_config.h"
#include "wm_task_config.h"

#include "sockets_ext.h"
#include "wm_utils.h"

#define TCPIP_THREAD_STACKSIZE WM_TASK_TCPIP_STACK //CONFIG_LWIP_TASK_STACK_SIZE
#define TCPIP_THREAD_PRIO      WM_TASK_TCPIP_PRIO  //CONFIG_LWIP_TASK_PRIO

#define MEM_LIBC_MALLOC        1
#define MEMP_MEM_MALLOC        1
#define MEM_USE_POOLS          0
#define MEMP_USE_CUSTOM_POOLS  0
#define MEMP_OVERFLOW_CHECK    0 ///2

#define mem_clib_free          wm_os_internal_free
#define mem_clib_malloc        wm_os_internal_malloc
#define mem_clib_calloc        wm_os_internal_calloc

#define LWIP_COMPAT_SOCKETS    2

#define NO_SYS                 0
#define SYS_LIGHTWEIGHT_PROT   1

#define LWIP_STATS             0

#define LWIP_SOCKET            1
#define LWIP_NETCONN           1
#define LWIP_TCP               1
#define LWIP_UDP               1
#define LWIP_ARP               1

#define RECV_BUFSIZE_DEFAULT   65536
#define LWIP_SOCKET_SELECT     1
#define LWIP_SOCKET_POLL       1
#define LWIP_SO_LINGER         1

#define TCP_MSS                CONFIG_LWIP_TCP_MSS //1460

#define TCP_SND_QUEUELEN       30

#define TCP_MSL                2000UL /* The maximum segment lifetime in milliseconds */

#define MEMP_NUM_TCP_SEG       32

#ifdef LWIP_UNITTESTS_LIB
#define LWIP_HAVE_LOOPIF 1
#define MIB2_STATS       1
#define TCPIP_THREAD_TEST
#define LWIP_WND_SCALE       1
#define TCP_RCV_SCALE        0
#define TCP_WND              (10 * TCP_MSS)
#define TCP_SND_BUF          (12 * TCP_MSS)
#define TCP_MAXRTX           12
#define TCP_SYNMAXRTX        6
#define LWIP_NETBUF_RECVINFO 1
#else                                                                            /* LWIP_UNITTESTS_LIB */
#define TCP_MAXRTX       CONFIG_LWIP_TCP_MAXRTX                                  ////3//12
#define TCP_SYNMAXRTX    CONFIG_LWIP_TCP_SYNMAXRTX                               ////4//6
#define TCP_WND          (CONFIG_LWIP_TCP_WND_DEFAULT * CONFIG_LWIP_TCP_MSS)     //(8 * TCP_MSS)
#define TCP_SND_BUF      (CONFIG_LWIP_TCP_SND_BUF_DEFAULT * CONFIG_LWIP_TCP_MSS) //(6 *TCP_MSS)
#define LWIP_HAVE_LOOPIF 0
#endif /* LWIP_UNITTESTS_LIB */

#ifdef CONFIG_LWIP_NETBUF_RECVINFO
#define LWIP_NETBUF_RECVINFO 1
#endif

#define TCPIP_MBOX_SIZE                        CONFIG_LWIP_TCPIP_MBOX_SIZE   //32
#define DEFAULT_TCP_RECVMBOX_SIZE              CONFIG_LWIP_TCP_RECVMBOX_SIZE //6
#define DEFAULT_UDP_RECVMBOX_SIZE              CONFIG_LWIP_UDP_RECVMBOX_SIZE //6
#define DEFAULT_RAW_RECVMBOX_SIZE              2
#define DEFAULT_ACCEPTMBOX_SIZE                5

#define LWIP_TCPIP_CORE_LOCKING                0

#define LWIP_TCPIP_CORE_LOCKING_INPUT          0

#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0

#define LWIP_NETIF_API                         1

#define MEMP_NUM_NETCONN                       8

#define LWIP_RAW                               1
#define LWIP_IGMP                              1
#define LWIP_RAND                              rand
#define LWIP_SO_RCVTIMEO                       1
#define LWIP_SO_RCVBUF                         1
#define LWIP_SO_SNDTIMEO                       1

#if CONFIG_LWIP_ENABLE_IPV4
#define LWIP_IPV4           1
#define LWIP_DHCP           1
#define DHCP_DOES_ARP_CHECK 1
#else
#define LWIP_IPV4 0
#define LWIP_DHCP 0
#endif

#if CONFIG_LWIP_ENABLE_IPV6
#define LWIP_IPV6       1
#define LWIP_IPV6_DHCP6 1
#define LWIP_IPV6_MLD   1
#else
#define LWIP_IPV6       0
#define LWIP_IPV6_DHCP6 0
#endif

#if CONFIG_LWIP_IP_FORWARD
#define IP_FORWARD 1
#endif

#if CONFIG_LWIP_IP_FORWARD && CONFIG_LWIP_ENABLE_NAPT
#define IP_NAPT        1
#define IP_NAPT_MAX    512
#define IP_PORTMAP_MAX 32
#else
#define IP_NAPT 0
#endif

#define LWIP_IPV6_SCOPES               0
#define LWIP_DNS                       1
#define LWIP_NETIF_STATUS_CALLBACK     0
#define LWIP_NETIF_EXT_STATUS_CALLBACK 1
#define TCP_LISTEN_BACKLOG             1
#define TCP_DEFAULT_LISTEN_BACKLOG     8
#define SO_REUSE                       1
#define SO_REUSE_RXTOALL               1
#define LWIP_ND6_MAX_MULTICAST_SOLICIT 10

#define ETHARP_SUPPORT_STATIC_ENTRIES  1
#define ETHARP_TABLE_MATCH_NETIF       0
#define ARP_QUEUEING                   1

#define MEM_ALIGNMENT                  4
#define LWIP_TCPIP_TIMEOUT             1
#define LWIP_NETIF_HOSTNAME            1
#define LWIP_TCP_KEEPALIVE             1
#define LWIP_SOCKET_OFFSET             1

#define LWIP_NETCONN_SEM_PER_THREAD    1
#define LWIP_NETCONN_FULLDUPLEX        1

#define LWIP_NO_INTTYPES_H             1

#define LWIP_TIMEVAL_PRIVATE           0

#ifdef CONFIG_COMPONENT_NET_MANAGER_ENABLED
#define DNS_MAX_SERVERS 2
#endif
#include <time.h>

#if __GNUC__ >= 13
#define LWIP_PROVIDE_ERRNO
#endif

/* SNTP */
#define SNTP_SERVER_DNS      1
#define SNTP_COMP_ROUNDTRIP  1
#define LWIP_MAX_NTP_SERVERS CONFIG_LWIP_MAX_NTP_SERVERS

#if CONFIG_LWIP_DEBUG
#define LWIP_DEBUG
#if CONFIG_LWIP_NETIF_DEBUG
#define NETIF_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_SOCKETS_DEBUG
#define SOCKETS_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_TCP_DEBUG
#define TCP_DEBUG        LWIP_DBG_ON
#define TCP_INPUT_DEBUG  LWIP_DBG_ON
#define TCP_CWND_DEBUG   LWIP_DBG_ON
#define TCP_WND_DEBUG    LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG LWIP_DBG_ON
#define TCP_QLEN_DEBUG   LWIP_DBG_ON
#define TCP_RTO_DEBUG    LWIP_DBG_ON
#define TCPIP_DEBUG      LWIP_DBG_ON
#endif
#if CONFIG_LWIP_UDP_DEBUG
#define UDP_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_NAPT_DEBUG
#define NAPT_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_DHCP_DEBUG
#define DHCP_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_DHCP6_DEBUG
#define DHCP6_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_ETHARP_DEBUG
#define ETHARP_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_API_LIB_DEBUG
#define API_LIB_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_IP_DEBUG
#define IP_DEBUG LWIP_DBG_ON
#endif
#if CONFIG_LWIP_DNS_DEBUG
#define DNS_DEBUG LWIP_DBG_ON
#endif
#endif /* CONFIG_LWIP_DEBUG */

#if LWIP_NETCONN_SEM_PER_THREAD
#include "arch/sys_arch.h"
extern sys_sem_t *sys_lwip_netconn_thread_sem_get(void);
extern void sys_lwip_netconn_thread_sem_del(void *tcb);

#define LWIP_NETCONN_THREAD_SEM_ALLOC()
#define LWIP_NETCONN_THREAD_SEM_GET()  sys_lwip_netconn_thread_sem_get()
#define LWIP_NETCONN_THREAD_SEM_FREE() sys_lwip_netconn_thread_sem_del(NULL)
#endif

#define LWIP_HOOK_SOCKETS_GETSOCKOPT(s, sock, level, optname, optval, optlen, err) \
    lwip_getsockopt_impl_ext(sock, level, optname, optval, optlen, err) ? true : false

#define LWIP_HOOK_SOCKETS_SETSOCKOPT(s, sock, level, optname, optval, optlen, err) \
    lwip_setsockopt_impl_ext(sock, level, optname, optval, optlen, err) ? true : false

#if LWIP_IPV4
#include "lwip/ip4_addr.h"
extern struct netif *wm_ip4_route_src(const ip4_addr_t *src, const ip4_addr_t *dest);
#define LWIP_HOOK_IP4_ROUTE_SRC(s, d) wm_ip4_route_src(s, d)
#endif

/* Number of seconds between 1970 and Feb 7, 2036 06:28:16 UTC (epoch 1) */
#define DIFF_SEC_1970_2036 ((u32_t)2085978496L)

#define SNTP_GET_SYSTEM_TIME_NTP(s, f) \
    do {                               \
        (s) = 0;                       \
        (f) = 0;                       \
    } while (0)

#include "wm_event.h"

WM_EVENT_DECLARE_GROUP(WM_SNTP_EV); /**< Declare SNTP event group           */

enum {
    WM_LWIP_EVENT_SNTP_GOT_TIME = 1, /**< sub event type for sntp got time   */
};

/** @brief sntp got time.
 *
 * Argument structure for WM_LWIP_EVENT_SNTP_GOT_TIME event
 *
 */
typedef struct {
    uint32_t sec;  /**< The integer part of the timestamp returned by ntp       */
    uint32_t frac; /**< The fractional portion of the timestamp returned by ntp */
} wm_sntp_patam_t;

#define SNTP_SET_SYSTEM_TIME_NTP(s, f)                                                         \
    do {                                                                                       \
        struct timeval tv = { .tv_sec = (s) + DIFF_SEC_1970_2036, .tv_usec = (f) * 1000 };     \
        wm_sntp_patam_t sntp_data;                                                             \
        settimeofday(&tv, NULL);                                                               \
        sntp_data.sec  = (s) + DIFF_SEC_1970_2036;                                             \
        sntp_data.frac = (f);                                                                  \
        wm_event_send(WM_SNTP_EV, WM_LWIP_EVENT_SNTP_GOT_TIME, &sntp_data, sizeof(sntp_data)); \
    } while (0)

/*lwip event */
WM_EVENT_DECLARE_GROUP(WM_LWIP_EV); /**< Declare LWIP event group           */

enum {
    WM_EVENT_WIFI_STA_GOT_IP,   /*!< station got IP from connected AP */
    WM_EVENT_WIFI_STA_GOT_IP6,  /*!< station lost IP and the IP is reset to 0 */
    WM_EVENT_WIFI_STA_LOST_IP,  /*!< station got IP6 from connected AP */
    WM_EVENT_WIFI_STA_LOST_IP6, /*!< station lost IP6 and the IP6 is reset to 0 */

#if CONFIG_WM_NETIF_ENABLE_ETH
    WM_EVENT_ETH_GOT_IP,   /*!< ethernet got IP from connected AP */
    WM_EVENT_ETH_GOT_IP6,  /*!< ethernet lost IP and the IP is reset to 0 */
    WM_EVENT_ETH_LOST_IP,  /*!< ethernet got IP6 from connected AP */
    WM_EVENT_ETH_LOST_IP6, /*!< ethernet lost IP6 and the IP6 is reset to 0 */
#endif

    WM_EVENT_WIFI_AP_STAIPASSIGNED, /*!< softap assign an IP to a connected station */
};
typedef union {
    struct {
        uint32_t ip;
        uint32_t gw;
        uint32_t netmask;
    } sta_got_ip_info;

    struct {
        uint8_t index;
        uint32_t ip[4];
    } sta_got_ip6_info;

    struct {
        uint8_t index;
    } sta_lost_ip6_info;

#if CONFIG_WM_NETIF_ENABLE_ETH
    struct {
        uint32_t ip;
        uint32_t gw;
        uint32_t netmask;
    } eth_got_ip_info;

    struct {
        uint8_t index;
        uint32_t ip[4];
    } eth_got_ip6_info;

    struct {
        uint8_t index;
    } eth_lost_ip6_info;
#endif

    struct {
        uint8_t addr[6];
        uint32_t ip;
    } ap_staipassigned_info;
} wm_lwip_event_data_t;

#endif /* end of __LWIP_OPTS_H */
