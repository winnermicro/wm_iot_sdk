#include <assert.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "wm_event.h"
#include "wm_nm_api.h"
#include "lwip/apps/wm_sntp.h"
#include "lwip/netdb.h"

#define LOG_TAG "example"
#include "wm_log.h"

#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif
#define WM_SNTP_SYNC_TIMEOUT_TICK (30000)
#define WM_SNTP_SERVER_NAME_1     "ntp.ntsc.ac.cn"
#define WM_SNTP_SERVER_NAME_2     "ntp1.aliyun.com"

static void wm_sntp_from_default_server(void)
{
    char buffer[64];
    struct tm *tm;

    for (int i = 0; i < CONFIG_LWIP_MAX_NTP_SERVERS; i++) {
        const char *server = wm_sntp_getservername(i);
        if (server != NULL) {
            wm_log_info("default %dst sntp server: %s", i, server);
        }
    }

    setenv("TZ", "UTC+8", 1);
    tm = wm_get_time_sync(WM_SNTP_SYNC_TIMEOUT_TICK);
    if (tm == NULL) {
        wm_log_error("get NTP failed");
    } else {
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
        wm_log_info("Beijing time: %s", buffer);
    }
}

static void wm_sntp_from_server_name(void)
{
    char buffer[64];
    struct tm *tm;

    for (int i = 0; i < CONFIG_LWIP_MAX_NTP_SERVERS; i++) {
        wm_sntp_setservername(i, NULL);
    }
    wm_sntp_setservername(0, WM_SNTP_SERVER_NAME_1);

    setenv("TZ", "UTC+8", 1);
    tm = wm_get_time_sync(WM_SNTP_SYNC_TIMEOUT_TICK);

    if (tm == NULL) {
        wm_log_error("get NTP from %s failed", WM_SNTP_SERVER_NAME_1);
    } else {
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
        wm_log_info("get NTP from %s", WM_SNTP_SERVER_NAME_1);
        wm_log_info("Beijing time: %s", buffer);
    }
}

static void wm_sntp_from_server(void)
{
    char buffer[1024];
    struct tm *tm;
    struct hostent hostinfo;
    struct hostent *host;
    int herr;
    ip_addr_t *addr = NULL;

    for (int i = 0; i < CONFIG_LWIP_MAX_NTP_SERVERS; i++) {
        wm_sntp_setservername(i, NULL);
    }

    if (gethostbyname_r(WM_SNTP_SERVER_NAME_2, &hostinfo, buffer, sizeof(buffer), &host, &herr)) {
        wm_log_error("Domain name(%s) resolution failed", WM_SNTP_SERVER_NAME_2);
        return;
    }

    for (int i = 0; host->h_addr_list[i] != NULL; i++) {
        addr = (ip_addr_t *)host->h_addr_list[i];
        break;
    }

    wm_sntp_setserver(0, addr);

    setenv("TZ", "UTC+8", 1);
    tm = wm_get_time_sync(WM_SNTP_SYNC_TIMEOUT_TICK);

    if (tm == NULL) {
        inet_ntop(AF_INET, addr, buffer, sizeof(buffer));
        wm_log_error("get NTP from %s failed", buffer);
    } else {
        inet_ntop(AF_INET, addr, buffer, sizeof(buffer));
        wm_log_info("get NTP from %s", buffer);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
        wm_log_info("Beijing time: %s", buffer);
    }
}

int main(void)
{
    int count                    = 0;
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };

    if (wm_nm_start_wifi_station(&cfg) != WM_ERR_SUCCESS) {
        wm_log_error("wifi station start failed");
        return WM_ERR_FAILED;
    }

    while (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ++count;
        if (count % 5 == 0) {
            wm_log_info("waiting for connect to %s", CONFIG_EXAMPLE_WIFI_SSID);
        }
    }

    wm_log_info("start sntp demo");

    wm_sntp_start();

    /** start sntp by default config */
    wm_sntp_from_default_server();

    vTaskDelay(pdMS_TO_TICKS(1000));
    /** start sntp by domain name */
    wm_sntp_from_server_name();

    vTaskDelay(pdMS_TO_TICKS(1000));
    /** start sntp by IP addr */
    wm_sntp_from_server();

    wm_log_info("finish sntp");
    wm_sntp_stop();
    return WM_ERR_SUCCESS;
}
