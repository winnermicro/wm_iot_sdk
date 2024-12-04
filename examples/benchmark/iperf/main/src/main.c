#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"

#define LOG_TAG "iperf3"
#include "wm_log.h"

static void event_handler_sta_disconnected(wm_event_group_t group, int event, void *data, void *priv)
{
    if (WM_EVENT_WIFI_STA_DISCONNECTED == event) {
        wm_log_raw_info("sta disconnected\n");

        wm_wifi_connect();
    }
}

static void event_handler_sta_got_ip(wm_event_group_t group, int event, wm_lwip_event_data_t *data, void *priv)
{
#if CONFIG_LWIP_ENABLE_IPV4
    if (WM_EVENT_WIFI_STA_GOT_IP == event) {
        printf("got ip: %s\n", ip4addr_ntoa((const ip4_addr_t *)&data->sta_got_ip_info.ip));
    }
#endif

#if CONFIG_LWIP_ENABLE_IPV6
    if (WM_EVENT_WIFI_STA_GOT_IP6 == event) {
        wm_log_raw_info("got ip6 %s (index %hhu)\n", ip6addr_ntoa((ip6_addr_t *)&data->sta_got_ip6_info.ip),
                        data->sta_got_ip6_info.index);
    }
#endif
}

static void initialise_wifi(void)
{
    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, event_handler_sta_disconnected, NULL);
    wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_sta_got_ip, NULL);

    wm_wifi_set_ps(WM_WIFI_PS_NONE);
    wm_wifi_set_storage(WM_WIFI_STORAGE_RAM);
}

int main(void)
{
    initialise_wifi();

    wm_log_raw_info("\n ===================================================\n");
    wm_log_raw_info(" |       Steps to test WiFi throughput             |\n");
    wm_log_raw_info(" |                                                 |\n");
    wm_log_raw_info(" |  1. Print 'help' to gain overview of commands   |\n");
    wm_log_raw_info(" |  2. Configure device to station or softap       |\n");
    wm_log_raw_info(" |  3. Setup WiFi connection                       |\n");
    wm_log_raw_info(" |  4. Run iperf3 to test UDP/TCP RX/TX throughput |\n");
    wm_log_raw_info(" |                                                 |\n");
    wm_log_raw_info(" ===================================================\n\n");

    return 0;
}
