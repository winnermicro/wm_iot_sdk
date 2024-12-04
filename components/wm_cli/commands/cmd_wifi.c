#include "wmsdk_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wm_cli.h"
#include "wm_wifi.h"
#include "wm_nvs.h"
#include "wm_key_config.h"
#include "wm_osal.h"
#include "wm_wifi_event.h"
#include "lwipopts.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef CONFIG_WIFI_API_ENABLED
static QueueHandle_t cmd_wifi_sync_sem = NULL;
static bool cmd_wifi_status_connected = false;

static void cmd_wifi_event_handler(wm_event_group_t group, int event, void *data, void *priv)
{
    if (WM_EVENT_WIFI_STA_DISCONNECTED == event) {
        wm_cli_printf("sta disconnected\r\n");
        cmd_wifi_status_connected = false;
    } else if (WM_EVENT_WIFI_STA_GOT_IP == event) {
        wm_cli_printf("sta connected\r\n");
        cmd_wifi_status_connected = true;
    }

    if (priv)
        xSemaphoreGive(priv);
}

static void cmd_wifi(int argc, char *argv[])
{
    int ret;
    bool need_wait_disconnect = false;

    if (argc < 2)
        goto usage;

    if (!strcmp(argv[0], "wifi")) {
        if (!strcmp(argv[1], "connect")) {
            if (argc > 2) {
                wm_wifi_config_t sta_conf;
                memset(&sta_conf, 0, sizeof(sta_conf));
                strncpy((char *)sta_conf.sta.ssid, argv[2], 32);
                if (argc > 3) {
                    strncpy((char *)sta_conf.sta.password, argv[3], 64);
                    sta_conf.sta.pmf_cfg.capable  = true;
                    sta_conf.sta.pmf_cfg.required = false;
                }
                ret = wm_wifi_set_config(WM_WIFI_IF_STA, &sta_conf);
                if (cmd_wifi_status_connected)
                    need_wait_disconnect = true;
                ret |= wm_wifi_connect();
                if (!ret && cmd_wifi_sync_sem) {
                    wm_cli_printf("connecting, please wait...\r\n");
                    xSemaphoreTake(cmd_wifi_sync_sem, portMAX_DELAY);
                    if (need_wait_disconnect)
                        xSemaphoreTake(cmd_wifi_sync_sem, portMAX_DELAY);
                }
            }
        } else if (!strcmp(argv[1], "disconnect")) {
            ret = wm_wifi_disconnect();
            if (!ret && cmd_wifi_status_connected && cmd_wifi_sync_sem)
                xSemaphoreTake(cmd_wifi_sync_sem, portMAX_DELAY);
        } else if (!strcmp(argv[1], "scan")) {
            if (argc > 2) {
                wm_wifi_scan_config_t config;
                memset(&config, 0, sizeof(config));
                config.ssid = (uint8_t *)argv[2];
                wm_wifi_scan_start(&config, true);
            } else {
                wm_wifi_scan_start(NULL, true);
            }
            uint16_t number = 0;
            wm_wifi_scan_get_ap_num(&number);
            wm_wifi_ap_record_t *ap_records;
            ap_records = wm_os_internal_malloc(number * sizeof(wm_wifi_ap_record_t));
            if (ap_records) {
                wm_wifi_scan_get_ap_records(&number, ap_records);
                wm_cli_printf("bssid                channel    rssi    open     ssid\r\n");
                for (uint16_t i = 0; i < number; i++) {
                    wm_cli_printf("%02x-%02x-%02x-%02x-%02x-%02x    %-7d    %-4d    %-5s    %s\r\n", ap_records[i].bssid[0],
                                  ap_records[i].bssid[1], ap_records[i].bssid[2], ap_records[i].bssid[3],
                                  ap_records[i].bssid[4], ap_records[i].bssid[5], ap_records[i].primary, ap_records[i].rssi,
                                  ap_records[i].authmode == WM_WIFI_AUTH_OPEN ? "true" : "false", (char *)ap_records[i].ssid);
                }
                wm_os_internal_free(ap_records);
            }
        } else if (!strcmp(argv[1], "init")) {
            if (!cmd_wifi_sync_sem)
                cmd_wifi_sync_sem = xSemaphoreCreateCounting(1, 0);
            wm_event_add_callback(WM_WIFI_EV, WM_EVENT_WIFI_STA_DISCONNECTED, cmd_wifi_event_handler, cmd_wifi_sync_sem);
            wm_event_add_callback(WM_LWIP_EV, WM_EVENT_WIFI_STA_GOT_IP, cmd_wifi_event_handler, cmd_wifi_sync_sem);
            wm_wifi_init();
        } else if (!strcmp(argv[1], "deinit")) {
            wm_event_remove_callback(WM_LWIP_EV, WM_EVENT_WIFI_STA_GOT_IP, cmd_wifi_event_handler, NULL);
            wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_WIFI_STA_DISCONNECTED, cmd_wifi_event_handler, NULL);
            wm_wifi_deinit();
            if (cmd_wifi_sync_sem) {
                vSemaphoreDelete(cmd_wifi_sync_sem);
                cmd_wifi_sync_sem = NULL;
            }
        }
    } else {
        goto usage;
    }

    return;

usage:
    wm_cli_printf("%s usage:\r\n"
                  "  init  wifi  stack: %s init\r\n"
                  "  deinit wifi stack: %s deinit\r\n"
                  "  scan and results : %s scan    [ssid]\r\n"
                  "  sta join an ap   : %s connect <ssid> [password]\r\n"
                  "  sta leave form ap: %s disconnect\r\n",
                  argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}
WM_CLI_CMD_DEFINE(wifi, cmd_wifi, wifi cmd, wifi<args...> --operate wifi);
#endif //CONFIG_WIFI_API_ENABLED
