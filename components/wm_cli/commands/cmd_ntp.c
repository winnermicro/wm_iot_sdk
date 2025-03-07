#include "wmsdk_config.h"
#include <stdio.h>
#include "wm_cli.h"
#include "wm_utils.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/ip_addr.h"
#include "lwip/apps/wm_sntp.h"
#include "lwip/netdb.h"

#define NTP_USAGE                                   \
    "ntp usage:\r\n"                                \
    "  Set NTP server adress: ntp [server_url]\r\n" \
    "  Start NTP: ntp start\r\n"                    \
    "  Stop NTP: ntp stop\r\n"                      \
    "  Show NTP: ntp\r\n"

#define WM_SNTP_SYNC_TIMEOUT_TICK (30000)

static int ntp_server_address_analysis(const char *ntp_url, ip_addr_t *addr)
{
    return WM_ERR_FAILED;
}

static void cmd_ntp(int argc, char *argv[])
{
    static uint8_t ntp_idx = 0;
    char buffer[64];
    ip_addr_t addr;

    /* Show NTP */
    if (argc == 1) {
        if (wm_sntp_getservername(0) == NULL) {
            wm_cli_printf("NTP is stop\r\n");
        } else {
            wm_cli_printf("NTP is running: \r\n");
            for (int i = 0; i < CONFIG_LWIP_MAX_NTP_SERVERS; i++) {
                const char *server = wm_sntp_getservername(i);
                if (server != NULL) {
                    wm_cli_printf("%dst sntp server: %s\r\n", i, server);
                }
            }

            struct tm *tm = wm_get_time_sync(WM_SNTP_SYNC_TIMEOUT_TICK);
            if (tm == NULL) {
                wm_cli_printf("NTP sync failed\r\n");
            } else {
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
                wm_cli_printf("Beijing time: %s\r\n", buffer);
            }
        }
        return;
    } else if (argc > 2) {
        goto usage;
    }

    if (!strcmp(argv[1], "start")) {
        setenv("TZ", "UTC+8", 1);
        wm_sntp_start();
    } else if (!strcmp(argv[1], "stop")) {
        wm_sntp_stop();
    } else {
        if (WM_ERR_SUCCESS != ntp_server_address_analysis(argv[1], &addr)) {
            wm_cli_printf("NTP server url error (%s)\r\n", argv[1]);
            return;
        }

        for (int i = 0; i < CONFIG_LWIP_MAX_NTP_SERVERS; i++) {
            if (wm_sntp_getservername(i) == NULL) {
                ntp_idx = i;
            }
        }
        wm_sntp_setserver(ntp_idx, &addr);

        ntp_idx = (ntp_idx + 1) % CONFIG_LWIP_MAX_NTP_SERVERS;
    }

    return;

usage:
    wm_cli_printf(NTP_USAGE);
}

WM_CLI_CMD_DEFINE(ntp, cmd_ntp, ntp cmd,
                  ntp[start / stop / server_address]-- start / stop ntp); //cppcheck # [syntaxError]
