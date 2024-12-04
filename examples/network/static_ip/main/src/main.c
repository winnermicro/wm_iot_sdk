#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "wm_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/netdb.h"
#include "wm_nm_api.h"

#define LOG_TAG "wifi station"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

#ifdef CONFIG_EXAMPLE_STATIC_DNS_AUTO
#define EXAMPLE_MAIN_DNS_SERVER   CONFIG_EXAMPLE_STATIC_GW_ADDR
#define EXAMPLE_BACKUP_DNS_SERVER "0.0.0.0"
#else
#define EXAMPLE_MAIN_DNS_SERVER   CONFIG_EXAMPLE_STATIC_DNS_SERVER_MAIN
#define EXAMPLE_BACKUP_DNS_SERVER CONFIG_EXAMPLE_STATIC_DNS_SERVER_BACKUP
#endif
#ifdef CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST
#define EXAMPLE_RESOLVE_DOMAIN CONFIG_EXAMPLE_STATIC_RESOLVE_DOMAIN
#endif

#ifdef CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST
static void resolve_domain_test(void)
{
    struct addrinfo *address_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int res = getaddrinfo(EXAMPLE_RESOLVE_DOMAIN, NULL, &hints, &address_info);
    if (res != 0 || address_info == NULL) {
        wm_log_error("couldn't get hostname for :%s: "
                     "getaddrinfo() returns %d, addrinfo=%p",
                     EXAMPLE_RESOLVE_DOMAIN, res, address_info);
    } else {
        if (address_info->ai_family == AF_INET) {
            struct sockaddr_in *p = (struct sockaddr_in *)address_info->ai_addr;
            wm_log_info("Resolved IPv4 address: %s", ipaddr_ntoa((const ip_addr_t *)&p->sin_addr.s_addr));
        }
#if CONFIG_LWIP_IPV6
        else if (address_info->ai_family == AF_INET6) {
            struct sockaddr_in6 *p = (struct sockaddr_in6 *)address_info->ai_addr;
            wm_log_info("Resolved IPv6 address: %s", ip6addr_ntoa((const ip6_addr_t *)&p->sin6_addr));
        }
#endif
    }
}
#endif /* CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST */

static void example_set_static_ip(void)
{
    wm_netif_ip_info_t ip_info;
    memset(&ip_info, 0, sizeof(ip_info));
    wm_ip_addr_set_ip4_u32(&ip_info.ip, ipaddr_addr(CONFIG_EXAMPLE_STATIC_IP_ADDR));
    wm_ip_addr_set_ip4_u32(&ip_info.netmask, ipaddr_addr(CONFIG_EXAMPLE_STATIC_NETMASK_ADDR));
    wm_ip_addr_set_ip4_u32(&ip_info.gw, ipaddr_addr(CONFIG_EXAMPLE_STATIC_GW_ADDR));
    if (wm_nm_set_netif_ip_info(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME), &ip_info) != WM_ERR_SUCCESS) {
        wm_log_error("set static ip error");
    } else {
        wm_log_debug("set static ip: %s, netmask: %s, gw: %s", CONFIG_EXAMPLE_STATIC_IP_ADDR,
                     CONFIG_EXAMPLE_STATIC_NETMASK_ADDR, CONFIG_EXAMPLE_STATIC_GW_ADDR);
    }
    wm_ip_addr_t dns;
    wm_ip_addr_set_ip4_u32(&dns, ipaddr_addr(EXAMPLE_MAIN_DNS_SERVER));
    if (wm_nm_set_dns(WM_NM_DNS_MAIN, &dns) != WM_ERR_SUCCESS) {
        wm_log_error("set main DNS ip error");
    }
    wm_ip_addr_set_ip4_u32(&dns, ipaddr_addr(EXAMPLE_BACKUP_DNS_SERVER));
    if (wm_nm_set_dns(WM_NM_DNS_BACKUP, &dns) != WM_ERR_SUCCESS) {
        wm_log_error("set backup DNS ip error");
    }
}

static void wifi_init_sta(void)
{
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };
    int ret                      = wm_nm_start_wifi_station(&cfg);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wifi station start fail: %d", ret);
        return;
    }
    while (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    wm_log_info("wifi station start success, network is ready!");
}

int main(void)
{
    wifi_init_sta();
    example_set_static_ip();

#if CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST
    resolve_domain_test();
#endif /* CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST */

    return 0;
}
