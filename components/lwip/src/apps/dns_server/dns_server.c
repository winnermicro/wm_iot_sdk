#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"

#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/apps/dns_server.h"

#include "wm_netif.h"

#if CONFIG_COMPONENT_WIFI_ENABLED
#include "wm_wifi_types.h"
#endif /** CONFIG_COMPONENT_WIFI_ENABLED */

static DNS_SERVER DnsServer;

/* Use for WiFi provisioning */
static INT8U g_froce_local = 0;

void DNSS_Force_local(INT8U on_off)
{
    g_froce_local = on_off;
}

static INT32S _DnsCompareName(INT8U *MyDns, INT8U *Query)
{
    INT8U n;

    if ((strlen((const char *)Query) - 1) > 32) {
        return 1;
    }

    do {
        n = *Query++;
        /* @see RFC 1035 - 4.1.4. Message compression. */
        if ((n & 0xc0) == 0xc0) {
            /* Compressed name. */
            break;
        } else {
            /* Not compressed name. */
            while (n > 0) {
                if ((*MyDns) != (*Query)) {
                    return 1;
                }
                ++Query;
                ++MyDns;
                --n;
            };
            ++MyDns;
        }
    } while (*Query != 0);

    return ((*(--MyDns) == 0) ? 0 : 1);
}

static void _DNSNameErrGenAndSend(ip_addr_t *Addr, INT16U Port, PDNS_QUERY pDnsQuery, INT8U *QueryName, INT16U TansactionId)
{
    INT32U Len;
    INT8U *Body;
    PDNS_HEADER pDnsHeader;
    struct pbuf *pDnsBuf;
    INT8U *pDnsReply;

    Len       = ((sizeof(DNS_HEADER) + (strlen((const char *)QueryName) + 1) + sizeof(DNS_QUERY) + 3) >> 2) << 2;
    pDnsReply = mem_malloc(Len);
    if (pDnsReply == NULL) {
        return;
    }

    pDnsHeader = (PDNS_HEADER)pDnsReply;
    Body       = (INT8U *)(pDnsHeader + 1);
    Len        = 0;

    /* Header. */
    pDnsHeader->TansactionId = TansactionId;
    pDnsHeader->DnsFlag1     = DNS_FLAG1_RESPONSE;
    pDnsHeader->DnsFlag2     = DNS_FLAG2_ERR_NAME;
    pDnsHeader->Quentions    = htons(1);
    pDnsHeader->AnswerRR     = 0;
    pDnsHeader->AuthorityRR  = 0;
    pDnsHeader->AdditionalRR = 0;
    Len += sizeof(DNS_HEADER);

    /* Querry. */
    MEMCPY(Body, QueryName, strlen((const char *)QueryName) + 1);
    Body += strlen((const char *)QueryName) + 1;
    Len += strlen((const char *)QueryName) + 1;
    MEMCPY(Body, pDnsQuery, sizeof(DNS_QUERY));
    Len += sizeof(DNS_QUERY);

    pDnsBuf = pbuf_alloc(PBUF_TRANSPORT, Len, PBUF_RAM);
    if (pDnsBuf == NULL) {
        mem_free(pDnsReply);
        return;
    }
    pbuf_take(pDnsBuf, (const void *)pDnsReply, Len);

    /* Send to the client. */
    udp_sendto(DnsServer.Socket, pDnsBuf, Addr, Port);
    pbuf_free(pDnsBuf);
    mem_free(pDnsReply);
}

static void _DNSAnswerGenAndSend(ip_addr_t *Addr, INT16U Port, PDNS_QUERY pDnsQuery, INT8U *QueryName, INT16U TansactionId)
{
    INT32U Len;
    INT8U *Body;
    INT32U ServerIpAddr;
    PDNS_HEADER pDnsHeader;
    DNS_ANSWER DnsAnswer;
    struct pbuf *pDnsBuf;
    INT8U *pDnsReply;
    INT16U Tmp;

    Len = ((sizeof(DNS_HEADER) + (strlen((const char *)QueryName) + 1) + sizeof(DNS_QUERY) + 2 + sizeof(DNS_ANSWER) + 2 + 4 +
            3) >>
           2)
          << 2;
    pDnsReply = mem_malloc(Len);
    if (pDnsReply == NULL) {
        return;
    }

    pDnsHeader = (PDNS_HEADER)pDnsReply;
    Body       = (INT8U *)(pDnsHeader + 1);
    Len        = 0;

    /* Header. */
    pDnsHeader->TansactionId = TansactionId;
    pDnsHeader->DnsFlag1     = DNS_FLAG1_RESPONSE;
    pDnsHeader->DnsFlag2     = DNS_FLAG2_ERR_NONE;
    pDnsHeader->Quentions    = htons(1);
    pDnsHeader->AnswerRR     = htons(1);
    pDnsHeader->AuthorityRR  = 0;
    pDnsHeader->AdditionalRR = 0;
    Len += sizeof(DNS_HEADER);

    /* Querry. */
    MEMCPY(Body, QueryName, strlen((const char *)QueryName) + 1);
    Body += strlen((const char *)QueryName) + 1;
    Len += strlen((const char *)QueryName) + 1;
    MEMCPY(Body, pDnsQuery, sizeof(DNS_QUERY));
    Body += sizeof(DNS_QUERY);
    Len += sizeof(DNS_QUERY);

    /* NAME: provided as offset to first occurence in response. */
    Tmp = DNS_NAME_OFFSET | sizeof(DNS_HEADER);
    Tmp = htons(Tmp);
    MEMCPY(Body, &Tmp, sizeof(INT16U));
    Body += sizeof(INT16U);
    Len += sizeof(INT16U);

    /* Answer. */
    DnsAnswer.Type  = htons(DNS_RRTYPE_A);
    DnsAnswer.Class = htons(DNS_RRCLASS_IN);
    DnsAnswer.Ttl   = htonl(DNS_DEFAULT_TTL);
    MEMCPY(Body, &DnsAnswer, sizeof(DNS_ANSWER));
    Body += sizeof(DNS_ANSWER);
    Len += sizeof(DNS_ANSWER);

    /* Length. */
    Tmp = htons(4);
    MEMCPY(Body, &Tmp, sizeof(INT16U));
    Body += sizeof(INT16U);
    Len += sizeof(INT16U);

    /* IP Address. */
    ServerIpAddr = ip_addr_get_ip4_u32(&DnsServer.HostIp);
    MEMCPY(Body, &ServerIpAddr, 4);
    Body += 4;
    Len += 4;

    pDnsBuf = pbuf_alloc(PBUF_TRANSPORT, Len, PBUF_RAM);
    if (pDnsBuf == NULL) {
        mem_free(pDnsReply);
        return;
    }
    pbuf_take(pDnsBuf, (const void *)pDnsReply, Len);

    /* Send to the client. */
    udp_sendto(DnsServer.Socket, pDnsBuf, Addr, Port);
    pbuf_free(pDnsBuf);
    mem_free(pDnsReply);
}

typedef struct {
    ip_addr_t Addr;
    INT16U Port;
} dns_client_info_t;

static void dnss_proxy_timeout(void *arg)
{
    char *cur_dns_idx = arg;
    // try next DNS server
    *cur_dns_idx = (*cur_dns_idx + 1) % DNS_MAX_SERVERS;
}

static void dns_proxy_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p == NULL || p->payload == NULL) {
        return;
    }

    sys_untimeout(dnss_proxy_timeout, NULL); // cancel timer

    dns_client_info_t *info = (dns_client_info_t *)arg;

    if (info != NULL) {
        struct pbuf *pDnsBuf = pbuf_alloc(PBUF_TRANSPORT, p->len, PBUF_RAM);
        if (pDnsBuf == NULL) {
            return;
        }

        pbuf_take(pDnsBuf, (const void *)p->payload, p->len);

        /* Send to the client. */
        udp_sendto(DnsServer.Socket, pDnsBuf, &info->Addr, info->Port);
        pbuf_free(pDnsBuf);
    }
}

static void _DNSS_Proxy(ip_addr_t *Addr, INT16U Port, PDNS_QUERY pDnsQuery, INT8U *QueryName, INT16U TansactionId,
                        struct pbuf *p)
{
    static char cur_dns_idx              = 0;
    static dns_client_info_t client_info = { 0 };
    static struct udp_pcb *dns_pcbs      = NULL;

    const ip_addr_t *dst = NULL;
    struct pbuf *pbuf    = NULL;

    if (dns_pcbs == NULL) {
        dns_pcbs = udp_new_ip_type(IPADDR_TYPE_ANY);

        if (dns_pcbs == NULL) {
            return;
        }

        int err;
        u16_t proxy_port = (0x8000) | (Port & 0xFFF);

        do {
            err = udp_bind(dns_pcbs, IP_ANY_TYPE, proxy_port);
            ++proxy_port;
        } while (err == ERR_USE);

        /* initialize DNS client */
        udp_recv(dns_pcbs, dns_proxy_recv, &client_info);
    }

    memcpy(&client_info.Addr, Addr, sizeof(ip_addr_t));
    client_info.Port = Port;

    for (; cur_dns_idx < DNS_MAX_SERVERS; cur_dns_idx++) {
        dst = dns_getserver(cur_dns_idx);
        if (!ip_addr_isany(dst)) {
            break;
        }
    }

    if (ip_addr_isany(dst)) {
        return;
    }

    pbuf = pbuf_alloc(PBUF_TRANSPORT, p->len, PBUF_RAM);
    if (pbuf == NULL) {
        return;
    }

    pbuf_take(pbuf, p->payload, p->len);
    udp_sendto(dns_pcbs, pbuf, dst, DNS_SERVER_PORT);
    pbuf_free(pbuf);

    sys_timeout(10000, dnss_proxy_timeout, &cur_dns_idx); // timeout is 10 seconds
}

/*   DNSS_RecvCb   */
/*-------------------------------------------------------------------------
	Description:	
		When an incoming DNS message is to me, this function process it and trigger the state machine.
		
	Arguments:
		Arg: Pointer to the user supplied argument.
		Pcb: Pointer to the udp_pcb which received data.
		P: Pointer to the packet buffer that was received.
		Addr: The remote IP address from which the packet was received.
		Port: The remote port from which the packet was received	.
		
	Return Value:
		None.
		
	Note:	
		
-------------------------------------------------------------------------*/
void DNSS_RecvCb(void *Arg, struct udp_pcb *Pcb, struct pbuf *P, ip_addr_t *Addr, INT16U Port)
{
    PDNS_HEADER pDnsHeader;
    DNS_QUERY DnsQuery;
    //INT16U nQuestions, nAnswers;
    INT8U *pDnsName;
    INT8U *pDnsMsg;

    do {
        pDnsMsg = mem_malloc(P->tot_len);
        if (pDnsMsg == NULL) {
            break;
        }
        pbuf_copy_partial(P, pDnsMsg, P->tot_len, 0);

        pDnsHeader = (PDNS_HEADER)pDnsMsg;

        /* Get the quention number and answer number. */
        //nQuestions = ntohs(pDnsHeader->Quentions);
        //nAnswers = ntohs(pDnsHeader->AnswerRR);

        /* Filter out the response frame and the unstandard query frame. */
        if ((pDnsHeader->DnsFlag1 & DNS_FLAG1_RESPONSE) || ((pDnsHeader->DnsFlag1 & (0xf << 3)) != DNS_FLAG1_OPCODE_STANDARD)) {
            break;
        }

        /* Locate the dns name. */
        pDnsName = (INT8U *)(pDnsHeader + 1);

        /* Get the query class and type. */
        MEMCPY(&DnsQuery, (pDnsName + strlen((const char *)pDnsName) + 1), sizeof(DnsQuery));

        /* Check the query class and type. */
        if ((DnsQuery.Class != htons(DNS_RRCLASS_IN)) && (DnsQuery.Type != htons(DNS_RRTYPE_A))) {
            break;
        }

        if (_DnsCompareName(DnsServer.DnsName, pDnsName) != 0) {
            /* Not my dns name, so notify the client name error. */
            wm_netif_t *netif = wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA);
            if (g_froce_local) {
                /* Force the local address to the client. */
                _DNSAnswerGenAndSend(Addr, Port, &DnsQuery, pDnsName, pDnsHeader->TansactionId);
            } else {
                if (!netif || !netif->netif) {
                    _DNSNameErrGenAndSend(Addr, Port, &DnsQuery, pDnsName, pDnsHeader->TansactionId);
                } else {
                    _DNSS_Proxy(Addr, Port, &DnsQuery, pDnsName, pDnsHeader->TansactionId, P);
                }
            }
        } else {
            /* My dns name, so send the answer to the client. */
            _DNSAnswerGenAndSend(Addr, Port, &DnsQuery, pDnsName, pDnsHeader->TansactionId);
        }
    } while (0);

    if (pDnsMsg) {
        mem_free(pDnsMsg);
    }
    pbuf_free(P);
}

/*   DNSS_Config   */
/*-------------------------------------------------------------------------
	Description:	
		This function is used to updata the server's dns name.
	Arguments:
		DnsName	: Pointer the server's dns name.
		
	Return Value:
		The DNS Server error code:
			DNSS_ERR_SUCCESS - No error
			DNSS_ERR_PARAM - Input parameter error
	Note:	
		The length of the DNS name must be less than 32.
-------------------------------------------------------------------------*/
INT8S DNSS_Config(INT8U *DnsName)
{
    if ((DnsName == NULL) || (strlen((const char *)DnsName) > 32)) {
        /* The length of the DNS name must be less than 32. */
        return DNSS_ERR_PARAM;
    }

    memset(DnsServer.DnsName, 0, 32);
    MEMCPY(DnsServer.DnsName, DnsName, strlen((const char *)DnsName));

    return DNSS_ERR_SUCCESS;
}

/*   DNSS_Start   */
/*-------------------------------------------------------------------------
	Description:	
		This function is used to start the dns server's service.
	Arguments:
		DnsName	: Specify the server's dns name
		Netif: Pointer to the Lwip network interface.
		
	Return Value:
		The DNS Server error code:
			DNSS_ERR_SUCCESS - No error
			DNSS_ERR_PARAM - Input parameter error
			DNSS_ERR_MEM - Out of memory
			DNSS_ERR_LINKDOWN - The NI is inactive
	Note:	
		The dns server must be started after the network interface was actived.
-------------------------------------------------------------------------*/
INT8S DNSS_Start(struct netif *Netif, const char *DnsName)
{
    if ((Netif == NULL) || (strlen((const char *)DnsName) > 32)) {
        return DNSS_ERR_PARAM;
    }

    if (netif_is_up(Netif) == 0) {
        return DNSS_ERR_LINKDOWN;
    }

    memset(&DnsServer, 0, sizeof(DnsServer));

    MEMCPY(DnsServer.DnsName, DnsName, strlen((const char *)DnsName));
    ip_addr_set(&DnsServer.HostIp, &Netif->ip_addr);

    DnsServer.Socket = udp_new();
    if (DnsServer.Socket == NULL) {
        return DNSS_ERR_MEM;
    }

    /* Set up local and remote port for the pcb. */
    udp_bind(DnsServer.Socket, IP_ADDR_ANY, DNS_SERVER_PORT);

    /* Set up the recv callback and argument. */
    udp_recv(DnsServer.Socket, (udp_recv_fn)DNSS_RecvCb, Netif);

    return DNSS_ERR_SUCCESS;
}

/*   DNSS_Stop   */
/*-------------------------------------------------------------------------
	Description:	
		This function is used to stop the dns server's service.
	Arguments:
		None.		
		
	Return Value:
		None.
		
	Note:	
		
-------------------------------------------------------------------------*/
void DNSS_Stop(void)
{
    if (DnsServer.Socket) {
        udp_remove(DnsServer.Socket);
    }

    DnsServer.Socket = NULL;
}
