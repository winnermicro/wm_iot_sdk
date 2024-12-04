#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/mem.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/apps/wm_ping.h"

#define LOG_TAG "ping"
#include "wm_log.h"

/** ping receive timeout - in milliseconds */
#define PING_RCV_TIMEO (2 * HZ)
/** ping delay - in milliseconds */
#define PING_DELAY     (1 * HZ)

/** ping identifier - must fit on a uint16_t */
#ifndef PING_ID
#define PING_ID 0xBEDF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

//max ping instance
#ifndef PING_MAX_INSTANCE
#define PING_MAX_INSTANCE 4
#endif

//ping task
#define WM_DEFAULT_PING_TASK_STACK_SIZE 1024
#define WM_DEFAULT_PING_TASK_PRIO       5
#define WM_DEFAULT_PING_TASK_NAME       "ping"

typedef struct wm_ping_ctx {
    int ping_running; // 1:start 0:stop
    wm_os_task_t ping_task;
    ip_addr_t ping_target;
    uint32_t ping_max_send_times;
    uint32_t ping_data_size;
    uint32_t ping_timeout;
    uint32_t ping_max_time;

    wm_ping_stat_t stat;
    wm_ping_stat_cb_func_t cb;
    void *cb_param;
} wm_ping_ctx_t;

static wm_ping_ctx_t ping_ctx_list[PING_MAX_INSTANCE];

/* ping variables */
static uint16_t ping_seq_num;
struct _ip_addr {
    uint8_t addr0, addr1, addr2, addr3;
};

#if LWIP_IPV4
/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}
#endif

#if LWIP_IPV6
static void ping6_prepare_echo(struct icmp6_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp6_echo_hdr);

    iecho->type   = ICMP6_TYPE_EREQ;
    iecho->code   = 0;
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char *)iecho)[sizeof(struct icmp6_echo_hdr) + i] = (char)i;
    }
}
#endif

/* Ping using the socket ip */
static int lwip_ping_send(int s, ip_addr_t *addr, int size)
{
    int err;
    struct icmp_echo_hdr *iecho;
    //struct icmp6_echo_hdr *iecho6;
#if LWIP_IPV4
    struct sockaddr_in to4 = { 0 };
#endif
#if LWIP_IPV6
    struct sockaddr_in6 to6 = { 0 };
    int cksum_offset        = 2;
#endif
    int ping_size = 0;

    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

#if LWIP_IPV4 && LWIP_IPV6
    if (IP_IS_V6_VAL(*addr))
        ping_size = sizeof(struct icmp6_echo_hdr) + size;
    else
#elif LWIP_IPV6
    ping_size = sizeof(struct icmp6_echo_hdr) + size;
#endif
#if LWIP_IPV4
        ping_size = sizeof(struct icmp_echo_hdr) + size;
#endif

    iecho = wm_os_internal_malloc(ping_size);
    if (iecho == NULL) {
        return ERR_MEM;
    }

#if LWIP_IPV4 && LWIP_IPV6
    if (IP_IS_V6_VAL(*addr)) {
        ping_size = sizeof(struct icmp6_echo_hdr) + size;
        ping6_prepare_echo((struct icmp6_echo_hdr *)iecho, (uint16_t)ping_size);
    } else
#elif LWIP_IPV6
    ping_size = sizeof(struct icmp6_echo_hdr) + size;
    ping6_prepare_echo((struct icmp6_echo_hdr *)iecho, (uint16_t)ping_size);
#endif
    {
#if LWIP_IPV4
        ping_size = sizeof(struct icmp_echo_hdr) + size;
        ping_prepare_echo(iecho, (uint16_t)ping_size);
#endif
    }

#if LWIP_IPV4 && LWIP_IPV6
    if (IP_IS_V6_VAL(*addr)) {
        lwip_setsockopt(s, IPPROTO_RAW, IPV6_CHECKSUM, &cksum_offset, sizeof(int));
        to6.sin6_len    = sizeof(to6);
        to6.sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to6.sin6_addr, (ip6_addr_t *)addr);
    } else
#elif LWIP_IPV6
    {
        lwip_setsockopt(s, IPPROTO_RAW, IPV6_CHECKSUM, &cksum_offset, sizeof(int));
        to6.sin6_len    = sizeof(to6);
        to6.sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to6.sin6_addr, (ip6_addr_t *)addr);
    }
#endif //LWIP_IPV6
    {
#if LWIP_IPV4
        to4.sin_len    = sizeof(to4);
        to4.sin_family = AF_INET;
#if LWIP_IPV4 && LWIP_IPV6
        to4.sin_addr.s_addr = addr->u_addr.ip4.addr;
#else  //LWIP_IPV4 && LWIP_IPV6
        to4.sin_addr.s_addr = addr->addr;
#endif //LWIP_IPV4 && LWIP_IPV6
#endif //LWIP_IPV4
    }

#if LWIP_IPV4 && LWIP_IPV6
    if (IP_IS_V6_VAL(*addr)) {
        err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr *)&to6, sizeof(to6));
    } else
#elif LWIP_IPV6
    {
        err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr *)&to6, sizeof(to6));
    }
#endif //LWIP_IPV6
    {
#if LWIP_IPV4
        err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr *)&to4, sizeof(to4));
#endif
    }

    wm_os_internal_free(iecho);

    return (err == ping_size ? ERR_OK : ERR_VAL);
}

#if LWIP_IPV6
static int lwip_ping6_recv(int s, int *ttl)
{
    char buf[64];
    int fromlen, len;
    struct sockaddr_in6 from;
    struct icmp6_echo_hdr *iecho;

    fromlen = sizeof(struct sockaddr_in6);
    while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen)) > 0) {
        if (len >= (int)(sizeof(struct ip6_hdr) + sizeof(struct icmp6_echo_hdr))) {
            //ip6hdr = (struct ip6_hdr *)buf;
            iecho = (struct icmp6_echo_hdr *)(buf + IP6_HLEN);
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
                *ttl = 0; //  not used
                return len;
            }
        }
    }

    return len;
}
#endif

#if LWIP_IPV4
static int lwip_ping_recv(int s, int *ttl)
{
    char buf[64];
    int len     = 0;
    int fromlen = sizeof(struct sockaddr_in);
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen)) > 0) {
        if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
            iphdr = (struct ip_hdr *)buf;
            iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
                *ttl = iphdr->_ttl;
                return len;
            }
        }
    }

    return len;
}
#endif

static void wm_ping_task(void *data)
{
    wm_ping_ctx_t *para = (wm_ping_ctx_t *)data;

#if LWIP_VERSION_MAJOR >= 2U
    struct timeval timeout;
    timeout.tv_sec  = para->ping_timeout / 1000;
    timeout.tv_usec = (para->ping_timeout % 1000) * 1000;
#else
    int timeout = para->ping_timeout * 1000UL / HZ;
#endif

    int s, ttl, recv_len;
    uint32_t send_times = 0;
    uint32_t recv_start_tick;
    uint32_t elapsed_time = 0;
    uint32_t ping_start   = 0;
    uint32_t diff         = 0;
    int ret;

#if LWIP_IPV6 && LWIP_IPV4
    if (IP_IS_V6(&para->ping_target)) {
        s = lwip_socket(AF_INET6, SOCK_RAW, IP6_NEXTH_ICMP6);
    } else
#elif LWIP_IPV6
    s = lwip_socket(AF_INET6, SOCK_RAW, IP6_NEXTH_ICMP6);
#endif
    {
#if LWIP_IPV4
        s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
#endif
    }

    if (s < 0)
        goto stop;

    ret = lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    LWIP_ASSERT("setting receive timeout failed", ret == 0);
    LWIP_UNUSED_ARG(ret);

    ping_start = wm_os_internal_get_time();
    while (1) {
        para->stat.sent_num++;
        if (lwip_ping_send(s, &para->ping_target, para->ping_data_size) == ERR_OK) {
            recv_start_tick = wm_os_internal_get_time();
#if LWIP_IPV4 && LWIP_IPV6
            if (IP_IS_V6(&para->ping_target)) {
                if ((recv_len = lwip_ping6_recv(s, &ttl)) >= 0) {
                    para->stat.rcvd_num++;
                    elapsed_time = (wm_os_internal_get_time() - recv_start_tick) * 1000UL / HZ;
                    para->stat.time_spent += elapsed_time;
                }
            } else
#elif LWIP_IPV6
            if ((recv_len = lwip_ping6_recv(s, &ttl)) >= 0) {
                para->stat.rcvd_num++;
                elapsed_time = (wm_os_internal_get_time() - recv_start_tick) * 1000UL / HZ;
                para->stat.time_spent += elapsed_time;
            }
#endif
            {
#if LWIP_IPV4
                if ((recv_len = lwip_ping_recv(s, &ttl)) >= 0) {
                    para->stat.rcvd_num++;
                    elapsed_time = (wm_os_internal_get_time() - recv_start_tick) * 1000UL / HZ;
                    para->stat.time_spent += elapsed_time;
                }
#endif
            }
            if (0 > recv_len) {
#if LWIP_IPV4 && LWIP_IPV6
                if (IP_IS_V6(&para->ping_target))
                    printf("From %s icmp_seq=%u timeout\n", inet6_ntoa(para->ping_target), (unsigned int)send_times);
                else
#elif LWIP_IPV6
                printf("From %s icmp_seq=%u timeout\n", inet6_ntoa(para->ping_target), (unsigned int)send_times);
#endif
#if LWIP_IPV4
                    printf("From %s icmp_seq=%u timeout\n", inet_ntoa(para->ping_target), (unsigned int)send_times);
#endif
            }
        } else {
#if LWIP_IPV4 && LWIP_IPV6
            if (IP_IS_V6(&para->ping_target))
                printf("Send %s - error\n", inet6_ntoa(para->ping_target));
            else
#elif LWIP_IPV6
            printf("Send %s - error\n", inet6_ntoa(para->ping_target));
#endif
#if LWIP_IPV4
                printf("Send %s - error\n", inet_ntoa(para->ping_target));
#endif
        }

        send_times++;
        if (send_times >= para->ping_max_send_times) {
            /* send ping times reached, stop */
            break;
        }

        if (para->ping_max_time) {
            diff = wm_os_internal_get_time() - ping_start;
            if (diff >= para->ping_max_time)
                break;
        }

        wm_os_internal_time_delay(PING_DELAY); /* take a delay */
    }

    lwip_close(s);
    para->stat.total_time = wm_os_internal_get_time() - ping_start;
    if (para->cb)
        para->cb(para->cb_param, &para->stat);
    para->ping_running = 0;
stop:
    wm_os_internal_task_del(NULL);
    return;
}

int wm_ping_async(const ip_addr_t *ping_addr, wm_ping_param_t *para, wm_ping_stat_cb_func_t cb, void *cb_para)
{
    int ping_index;
    wm_ping_ctx_t *ctx;

    for (ctx = NULL, ping_index = 0; ping_index < PING_MAX_INSTANCE; ping_index++) {
        ctx = &ping_ctx_list[ping_index];
        if (!ctx->ping_running)
            break;
    }

    printf("ping instance index %d \r\n", ping_index);
    if (ping_index >= PING_MAX_INSTANCE)
        return (-1);

    memset(ctx, 0, sizeof(*ctx));

    ctx->ping_running = 1;
    memcpy(&ctx->ping_target, ping_addr, sizeof(ip_addr_t));

    if (para->ping_data_size == 0)
        ctx->ping_data_size = para->ping_data_size;
    else
        ctx->ping_data_size = PING_DATA_SIZE;

    if (para->ping_timeout == 0)
        ctx->ping_timeout = PING_RCV_TIMEO;
    else
        ctx->ping_timeout = para->ping_timeout;

    ctx->ping_max_send_times = para->ping_max_send_times;
    ctx->ping_max_time       = para->ping_max_time;

    ctx->cb       = cb;
    ctx->cb_param = cb_para;

    if (WM_OS_STATUS_SUCCESS != wm_os_internal_task_create(&ctx->ping_task, WM_DEFAULT_PING_TASK_NAME, wm_ping_task,
                                                           (void *)ctx, WM_DEFAULT_PING_TASK_STACK_SIZE,
                                                           WM_DEFAULT_PING_TASK_PRIO, 0)) {
        ctx->ping_running = 0;
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static void ping_stat_cb(void *cb_para, wm_ping_stat_t *stat)
{
    printf("Time tootal = %ums\r\n", (unsigned int)stat->total_time);
    printf("Sent = %u, Rcvd = %u, Lost = %u\r\n", (unsigned int)stat->sent_num, (unsigned int)stat->rcvd_num,
           (unsigned int)(stat->sent_num - stat->rcvd_num));
    printf("Avg = %ums\r\n", (unsigned int)(stat->rcvd_num ? stat->time_spent / stat->rcvd_num : 0));
}

int wm_net_ping(const char *target)
{
    wm_ping_param_t para = { 0 };

    para.ping_data_size      = 0; /*default*/
    para.ping_timeout        = 0; /*default*/
    para.ping_max_send_times = 4; /*default*/

    return wm_net_ping_withcb(target, &para, ping_stat_cb, NULL);
}

int wm_net_ping_withcb(const char *target, wm_ping_param_t *pararm, wm_ping_stat_cb_func_t cb, void *cb_param)
{
    int ret             = -1;
    ip_addr_t ping_addr = { 0 };
#if LWIP_IPV4
    struct sockaddr_in address;
#endif
#if LWIP_IPV6
    struct sockaddr_in6 address6;
#endif
    struct addrinfo *res  = NULL;
    struct addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL };

    /* convert URL to IP */
    if (lwip_getaddrinfo(target, NULL, &hints, &res) != 0) {
        wm_log_error("ping: unknown host %s", target);
        return WM_ERR_FAILED;
    }
#if LWIP_IPV4 && LWIP_IPV6
    if (AF_INET6 == res->ai_family) {
        memcpy(&address6.sin6_addr, &((struct sockaddr_in6 *)(res->ai_addr))->sin6_addr, sizeof(address6.sin6_addr));
        ret = inet6_aton(inet6_ntoa(address6.sin6_addr), &ping_addr);
        IP_SET_TYPE_VAL(ping_addr, IPADDR_TYPE_V6);
    } else
#elif LWIP_IPV6
    if (AF_INET6 == res->ai_family) {
        memcpy(&address6.sin6_addr, &((struct sockaddr_in6 *)(res->ai_addr))->sin6_addr, sizeof(address6.sin6_addr));
        ret = inet6_aton(inet6_ntoa(address6.sin6_addr), &ping_addr);
        IP_SET_TYPE_VAL(ping_addr, IPADDR_TYPE_V6);
    }
#endif
    {
#if LWIP_IPV4
        address.sin_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;
        ret              = inet_aton(inet_ntoa(address.sin_addr), &ping_addr);
#endif
    }
    lwip_freeaddrinfo(res);

    if (0 == ret) {
        wm_log_info("ping: inet_aton fail %d", ret);
        return WM_ERR_FAILED;
    }

    if (cb)
        ret = wm_ping_async(&ping_addr, pararm, cb, cb_param);
    else
        ret = wm_ping_async(&ping_addr, pararm, ping_stat_cb, NULL);
    return ret;
}
