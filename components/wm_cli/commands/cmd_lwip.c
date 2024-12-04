#include "wmsdk_config.h"
#include <stdio.h>
#include "wm_cli.h"
#include "wm_utils.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/ip_addr.h"

static void show_netif_info(struct netif *netif)
{
    wm_cli_printf("%c%c%d: ", netif->name[0], netif->name[1], netif->num);
    wm_cli_printf("flags=%x<%s%s%s%s>  mtu %hu\r\n", netif->flags,
    netif->flags & NETIF_FLAG_LINK_UP ? "UP" : "DOWN",
    netif->flags & NETIF_FLAG_BROADCAST ? " BROADCAST" : "",
    netif->flags & NETIF_FLAG_UP ? " RUNNING" : " STOPPED",
    netif->flags & NETIF_FLAG_IGMP ? " MULTICAST" : "",
    netif->mtu);
#ifdef CONFIG_LWIP_ENABLE_IPV4
   {
        char ip[16];
        char gateway[16];
        char netmask[16];

        ipaddr_ntoa_r(&netif->ip_addr, ip, sizeof(ip));
        ipaddr_ntoa_r(&netif->netmask, netmask, sizeof(netmask));
        ipaddr_ntoa_r(&netif->gw, gateway, sizeof(gateway));
        wm_cli_printf("     inet %s  netmask %s  gateway %s\r\n", ip,netmask,gateway);
    }
#endif
#ifdef CONFIG_LWIP_ENABLE_IPV6
    for (uint8_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++)
    {
        if (!ip_addr_isany(&netif->ip6_addr[i]))
            wm_cli_printf("     inet6 %s\r\n", ip6addr_ntoa(netif_ip6_addr(netif, i)));
    }
#endif
    wm_cli_printf("     ether %02x:%02x:%02x:%02x:%02x:%02x\r\n\r\n", netif->hwaddr[0], netif->hwaddr[1],
        netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
}

static void cmd_ifconfig(int argc, char *argv[])
{
    struct netif *netif;

    NETIF_FOREACH(netif) {
        if (argc >= 2)
        {
            if ((3 == strlen(argv[1])) && (argv[1][0] == netif->name[0]) && (argv[1][1] == netif->name[1]) && ((argv[1][2] - '0') == netif->num))
            {
                if (argc == 5)
                {
#if LWIP_IPV4
                    netifapi_dhcp_stop(netif);
#endif
#if LWIP_IPV6_DHCP6
                    netifapi_dhcp6_stop(netif);
#endif
                    ip_addr_t ipaddr;
                    ip_addr_t netmask;
                    ip_addr_t gw;
                    err_t err = ipaddr_aton(argv[2], &ipaddr) ? ERR_OK : ERR_ARG;
                    if (!err)
                        err = ipaddr_aton(argv[3], &netmask) ? ERR_OK : ERR_ARG;
                    if (!err)
                        err = ipaddr_aton(argv[4], &gw) ? ERR_OK : ERR_ARG;
#if LWIP_IPV4
                    if (!err)
                        err = netifapi_netif_set_addr(netif, (const ip4_addr_t *)&ipaddr,
                                (const ip4_addr_t *)&netmask, (const ip4_addr_t *)&gw);
#endif
                    if (!err)
                        wm_cli_printf("success\r\n");
                    else
                        wm_cli_printf("failed\r\n");
                }
                else if (argc == 3)
                {
                    if (!strncmp(argv[2], "up", 2))
                    {
                        netifapi_netif_set_up(netif);
                    }
                    else if (!strncmp(argv[2], "down", 4))
                    {
                        netifapi_netif_set_down(netif);
                    }
                }
                else if (argc == 2)
                {
                    show_netif_info(netif);
                }

                break;
            }
            else
            {
                continue;
            }
        }
        else
        {
            show_netif_info(netif);
        }
    }
}
WM_CLI_CMD_DEFINE(ifconfig, cmd_ifconfig, ifconfig cmd, ifconfig [if ip mask gw] -- show/set netif info); //cppcheck # [syntaxError]