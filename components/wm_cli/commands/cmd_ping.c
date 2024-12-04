#include "wmsdk_config.h"
#include <string.h>
#include <getopt.h>
#include "wm_types.h"
#include "wm_error.h"
#include "wm_cli.h"
#include "wm_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

#if !LWIP_IPV6 && !LWIP_IPV4
#error "Pls enable IPv4 or IPv6"
#endif

/** ping identifier - must fit on a uint16_t */
#define ICMP_ECHO_ID 0xADDA

struct cmd_ping_param {
    char *host;
    uint32_t count;
    uint32_t interval_ms;
    uint32_t timeout_ms;
    uint32_t data_size;
    int tos;
    int ttl;
};

/* ping variables */
static uint16_t cmd_ping_seq_num;

#if LWIP_IPV4
/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = ICMP_ECHO_ID;
    ++cmd_ping_seq_num;
    iecho->seqno = htons(cmd_ping_seq_num);

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
    iecho->id     = ICMP_ECHO_ID;
    ++cmd_ping_seq_num;
    iecho->seqno = htons(cmd_ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char *)iecho)[sizeof(struct icmp6_echo_hdr) + i] = (char)i;
    }
    // iecho->chksum = inet_chksum(iecho, len);
}
#endif

/* Ping using the socket ip */
static int cmd_ping_send(int s, struct sockaddr *to, int to_len, int size)
{
    int err                     = 0;
    struct icmp_echo_hdr *iecho = NULL;
#if LWIP_IPV6
    int cksum_offset = 2;
#endif
    int ping_size = size;

    if (to->sa_family == AF_INET6) {
#if LWIP_IPV6
        ping_size += sizeof(struct icmp6_echo_hdr);
#endif
    }
#if LWIP_IPV4
    else {
        ping_size += sizeof(struct icmp_echo_hdr);
    }
#endif
    if (ping_size > 0xFFFF) {
        wm_cli_printf("ping_size is too big");
        return -1;
    }

    iecho = malloc(ping_size);
    if (iecho == NULL) {
        return ERR_MEM;
    }

    if (to->sa_family == AF_INET6) {
#if LWIP_IPV6
        ping6_prepare_echo((struct icmp6_echo_hdr *)iecho, (uint16_t)ping_size);
        setsockopt(s, IPPROTO_RAW, IPV6_CHECKSUM, &cksum_offset, sizeof(int));
#endif
    }
#if LWIP_IPV4
    else {
        ping_prepare_echo(iecho, (uint16_t)ping_size);
    }
#endif

    err = sendto(s, iecho, ping_size, 0, to, to_len);

    free(iecho);

    return (err == ping_size ? ERR_OK : ERR_VAL);
}

#if LWIP_IPV6
static int ping_recv_analysis_ip6(char *buf, int len, int *ttl)
{
    struct ip6_hdr *iphdr;
    struct icmp6_echo_hdr *iecho;

    if (len >= (int)(sizeof(struct ip6_hdr) + sizeof(struct icmp6_echo_hdr))) {
        iphdr = (struct ip6_hdr *)buf;
        iecho = (struct icmp6_echo_hdr *)(buf + IP6_HLEN);

        if ((iecho->id == ICMP_ECHO_ID) && (iecho->seqno == htons(cmd_ping_seq_num))) {
            *ttl = iphdr->_hoplim;
            return len;
        }
    }

    return -1;
}
#endif

#if LWIP_IPV4
static int ping_recv_analysis_ip4(char *buf, int len, int *ttl)
{
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
        iphdr = (struct ip_hdr *)buf;
        iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
        if ((iecho->id == ICMP_ECHO_ID) && (iecho->seqno == htons(cmd_ping_seq_num))) {
            *ttl = iphdr->_ttl;
            return len;
        }
    }

    return len;
}
#endif

static int cmd_ping_recv(int s, struct cmd_ping_param *params, struct sockaddr *from, int *ttl) //, int from_len
{
    char *buf;

    int fromlen = sizeof(struct sockaddr) + 20;
    int size    = params->data_size;
    int len;
    int ret;

#if LWIP_IPV6
    size += sizeof(struct ip6_hdr) + sizeof(struct icmp6_echo_hdr);
#elif LWIP_IPV4
    size += sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr);
#endif

    buf = malloc(size);
    if (!buf) {
        return -1;
    }
    int end_time = xTaskGetTickCount() * portTICK_PERIOD_MS + params->interval_ms;

    while ((len = recvfrom(s, buf, size, 0, from, (socklen_t *)&fromlen)) > 0) {
        if (from->sa_family == AF_INET6) {
#if LWIP_IPV6
            ret = ping_recv_analysis_ip6(buf, len, ttl);
#endif
        }
#if LWIP_IPV4
        else {
            ret = ping_recv_analysis_ip4(buf, len, ttl);
        }
#endif
        if (ret > 0) {
            break;
        }
        if (end_time < (xTaskGetTickCount() * portTICK_PERIOD_MS)) {
            free(buf);
            return -1;
        }
    }

    free(buf);
    return len;
}

static int cmd_ping_do(struct cmd_ping_param *params)
{
    char ip_str[32];
    struct timeval timeout = { params->timeout_ms / 1000, (params->timeout_ms % 1000) * 1000 };
    int s = -1, ttl = 0, recv_len;
    uint32_t send_times;
    uint32_t recv_start_tick;
    struct addrinfo hint, *res = NULL;
#if LWIP_IPV4
    struct sockaddr_in addr = { 0 };
#endif
#if LWIP_IPV6
    struct sockaddr_in6 addr6 = { 0 };
#endif

    uint32_t transmitted   = 0;
    uint32_t received      = 0;
    uint32_t total_time_ms = 0;
    uint32_t loss          = 0;
    struct sockaddr *to    = NULL;
    int to_len             = 0;

    send_times       = 0;
    cmd_ping_seq_num = 0;

    memset(&hint, 0, sizeof(hint));

    /* convert URL to IP */
    if (getaddrinfo(params->host, NULL, &hint, &res) != 0 || res == NULL) {
        wm_cli_printf("ping: unknown host %s\r\n", params->host);
        return WM_ERR_FAILED;
    }

#if !LWIP_IPV4 && LWIP_IPV6 //IPv6 only
    if (res->ai_family == AF_INET) {
        return -1;
    }
#elif LWIP_IPV4 && !LWIP_IPV6 //IPv4 only
    if (res->ai_family == AF_INET6) { //  cppcheck   #   SC.Z   [syntaxError]
        return -1;
    }
#endif

    if (res->ai_family == AF_INET6) { //  cppcheck   #   SC.Z   [no error]
#if LWIP_IPV6
        memcpy(&addr6, res->ai_addr, sizeof(addr6));
        to     = (struct sockaddr *)&addr6;
        to_len = sizeof(addr6);
#endif
    } else {
#if LWIP_IPV4
        memcpy(&addr, res->ai_addr, sizeof(addr));
        to     = (struct sockaddr *)&addr;
        to_len = sizeof(addr);
#endif
    }

    freeaddrinfo(res);
    /* new a socket */

    if (to->sa_family == AF_INET6) {
#if LWIP_IPV6
        s = socket(AF_INET6, SOCK_RAW, IP6_NEXTH_ICMP6);
#endif
    } else {
#if LWIP_IPV4
        s = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
#endif
    }

    if (s < 0) {
        wm_cli_printf("ping: create socket failed\r\n");
        return WM_ERR_FAILED;
    }

    if (to->sa_family == AF_INET6) {
#if LWIP_IPV6
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)to;
        inet_ntop(AF_INET6, &(addr6->sin6_addr), ip_str, sizeof(ip_str));
#endif
    }
#if LWIP_IPV4
    else {
        struct sockaddr_in *addr = (struct sockaddr_in *)to;
        inet_ntop(AF_INET, &(addr->sin_addr), ip_str, sizeof(ip_str));
    }
#endif

    /* set receive timeout */
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* set tos */
    setsockopt(s, IPPROTO_IP, IP_TOS, &params->tos, sizeof(params->tos));

    /* set ttl */
    setsockopt(s, IPPROTO_IP, IP_TTL, &params->ttl, sizeof(params->ttl));

    while (1) {
        int elapsed_time;

        if (cmd_ping_send(s, to, to_len, params->data_size) == ERR_OK) {
            recv_start_tick = xTaskGetTickCount();
            recv_len        = cmd_ping_recv(s, params, to, &ttl);
            elapsed_time    = (xTaskGetTickCount() - recv_start_tick) * portTICK_PERIOD_MS;
            transmitted++;
            total_time_ms += elapsed_time;

            if (recv_len >= 0) {
                received++;
                wm_cli_printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\r\n", recv_len, ip_str, send_times, ttl,
                              elapsed_time);
            } else {
                wm_cli_printf("From %s icmp_seq=%d timeout\r\n", ip_str, send_times);
            }
        } else {
            wm_cli_printf("Send %s - error (%d)\r\n", ip_str, errno);
        }

        send_times++;
        if (params->count && (send_times >= params->count)) {
            /* send ping times reached, stop */
            loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
            wm_cli_printf("\r\n--- %s ping statistics ---\r\n", ip_str);
            wm_cli_printf("%u packets transmitted, %u received, %u%% packet loss, time %ums\r\n\r\n", transmitted, received,
                          loss, total_time_ms);
            break;
        }

        vTaskDelay(params->interval_ms); /* take a delay */
    }

    close(s);

    return WM_ERR_SUCCESS;
}

static void cmd_ping_usage(const char *basename)
{
    wm_cli_printf("\r\n"
                  "Usage\r\n"
                  "  %s [options] <destination>\r\n"
                  "\r\n"
                  "Options:\r\n"
                  "  <destination>      dns name or ip address\r\n"
                  "  -c <count>         stop after <count> replies\r\n"
                  "  -i <interval>      milliseconds between sending each packet\r\n"
                  "  -s <size>          use <size> as number of data bytes to be sent\r\n"
                  "  -t <ttl>           define time to live\r\n"
                  "  -q <tos>           define type of service\r\n"
                  "  -w <timeout>       milliseconds to wait for response\r\n"
                  "  -h                 print help and exit\r\n",
                  basename);
}

static int cmd_ping_parse_args(struct cmd_ping_param *param, int argc, char **argv)
{
    int flag;

    optind = 1;
    while ((flag = getopt(argc, argv, "c:i:s:t:q:w:h")) != -1) {
        switch (flag) {
            case 'c':
                param->count = atoi(optarg);
                break;
            case 'i':
                param->interval_ms = atoi(optarg);
                break;
            case 's':
                param->data_size = atoi(optarg);
                break;
            case 't':
                param->ttl = atoi(optarg);
                break;
            case 'q':
                param->tos = atoi(optarg);
                break;
            case 'w':
                param->timeout_ms = atoi(optarg);
                break;
            default:
                cmd_ping_usage(argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        if (strlen(argv[optind])) {
            param->host = strdup(argv[optind]);
            if (!param->host)
                return 2;
        }
    }

    if (!param->host) {
        cmd_ping_usage(argv[0]);
        return 1;
    }

    return 0;
}

static void cmd_ping(int argc, char *argv[])
{
    struct cmd_ping_param *param;

    param = malloc(sizeof(*param));
    if (!param)
        return;

    memset(param, 0, sizeof(*param));
    param->data_size   = 32;
    param->interval_ms = 1000;
    param->timeout_ms  = 1000;
    param->tos         = 0;
    param->ttl         = 255;
    param->count       = 4;

    if (cmd_ping_parse_args(param, argc, argv)) {
        free(param);
        return;
    }

    cmd_ping_do(param);

    free(param->host);
    free(param);
}
WM_CLI_CMD_DEFINE(ping, cmd_ping, ping cmd, ping<params> --ping cmd);
