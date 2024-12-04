#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "wm_wifi.h"
#include "wm_wifi_event.h"
#include "wm_nm_api.h"
#include "wm_nm_wifi.h"

#define LOG_TAG "wm_nm_wifi"
#include "wm_log.h"

#define WM_NM_QUEUE_SIZE           10
#define WM_NM_WIFI_CONNECT_TIMEOUT 30000 /* wifi connection timeout 30s */

typedef struct {
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
    wm_wifi_config_t *sta_config;
    int sta_state;
#endif
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
    wm_wifi_config_t *ap_config;
    int ap_state;
    uint8_t channel; // Hardware limitation: Station and softap must be on the same channel
#endif
    wm_os_queue_t *queue;
#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
    wm_os_timer_t *re_con_timer;
#endif
} wm_nm_wifi_t;
static wm_nm_wifi_t s_nm_wifi = { 0 };

static void wm_nm_wifi_task(void *arg)
{
    (void)arg;
    int ret;
    wm_nm_msg_t *msg = NULL;
    while (1) {
        ret = wm_os_internal_queue_receive(s_nm_wifi.queue, (void **)&msg, WM_OS_WAIT_TIME_MAX);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_warn("wait msg queue fail(%d)", ret);
            continue;
        }
        assert(msg != NULL);
        switch (msg->type) {
#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
            case WM_NM_STA_START_CONNECT:
            {
                if (msg->sta.ssid[0]) {
                    if (s_nm_wifi.sta_config == NULL) {
                        s_nm_wifi.sta_config = wm_os_internal_calloc(sizeof(wm_wifi_config_t), 1);
                        if (s_nm_wifi.sta_config == NULL) {
                            wm_log_error("malloc sta config fail");
                            break;
                        }
                        s_nm_wifi.sta_config->sta.listen_interval = 3;
                        s_nm_wifi.sta_config->sta.pmf_cfg.capable = 1;
                    }
                    memcpy(s_nm_wifi.sta_config->sta.ssid, msg->sta.ssid, sizeof(s_nm_wifi.sta_config->sta.ssid));
                    memcpy(s_nm_wifi.sta_config->sta.password, msg->sta.pwd, sizeof(s_nm_wifi.sta_config->sta.password));
                } else {
                    if (s_nm_wifi.sta_config == NULL || !s_nm_wifi.sta_config->sta.ssid[0]) {
                        wm_log_warn("have not station config, ignoring start connect request");
                        break;
                    }
                }

                if (wm_wifi_init() != WM_ERR_SUCCESS) {
                    wm_log_error("wifi init fail");

#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
                    wm_nm_wifi_station_reconnect_start(CONFIG_NM_STA_RECONNECT_INTERVAL);
#endif
                    break;
                }

                if (wm_wifi_set_config(WM_WIFI_IF_STA, s_nm_wifi.sta_config) != WM_ERR_SUCCESS) {
                    wm_log_error("set wifi config fail");

#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
                    wm_nm_wifi_station_reconnect_start(CONFIG_NM_STA_RECONNECT_INTERVAL);
#endif
                    break;
                }

                if (wm_wifi_connect() != WM_ERR_SUCCESS) {
                    wm_log_error("connect %s fail", s_nm_wifi.sta_config->sta.ssid);

#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
                    wm_nm_wifi_station_reconnect_start(CONFIG_NM_STA_RECONNECT_INTERVAL);
#endif
                    break;
                }

                s_nm_wifi.sta_state = WM_NM_WIFI_STA_CONNECTING;

#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
                wm_nm_wifi_station_reconnect_start(WM_NM_WIFI_CONNECT_TIMEOUT);
#endif
                wm_log_info("start connecting %s(%s)", s_nm_wifi.sta_config->sta.ssid, s_nm_wifi.sta_config->sta.password);
                break;
            }
            case WM_NM_STA_DISCONNECT:
            {
                wm_wifi_disconnect();
                if (s_nm_wifi.sta_config) {
                    memset(s_nm_wifi.sta_config, 0, sizeof(wm_wifi_config_t));
                    wm_os_internal_free(s_nm_wifi.sta_config);
                    s_nm_wifi.sta_config = NULL;
                }
                s_nm_wifi.sta_state = WM_NM_EXIT;
                wm_log_info("disconnect from ap");
                break;
            }
#endif
#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
            case WM_NM_RF_CHANNEL_CHANGE:
            {
                s_nm_wifi.channel = msg->channel;

                /** softAP is not start, or station and softAP is the same channel */
                if (s_nm_wifi.ap_state != WM_NM_WIFI_AP_READY || s_nm_wifi.ap_config->ap.channel == s_nm_wifi.channel) {
                    break;
                }
                // ap_state == WM_NM_WIFI_AP_READY => must restart softAP
            }
            case WM_NM_AP_START:
            {
                int ret = 0;

                if (msg->ap.ssid[0]) {
                    if (s_nm_wifi.ap_config == NULL) {
                        s_nm_wifi.ap_config = wm_os_internal_calloc(sizeof(wm_wifi_config_t), 1);
                        if (s_nm_wifi.ap_config == NULL) {
                            wm_log_error("malloc ap_config fail");
                            break;
                        }
                    }
                    memcpy(s_nm_wifi.ap_config->ap.ssid, msg->ap.ssid, sizeof(s_nm_wifi.ap_config->ap.ssid));
                    s_nm_wifi.ap_config->ap.ssid_len = strlen(msg->ap.ssid);
                    if (msg->ap.pwd[0]) {
                        memcpy(s_nm_wifi.ap_config->ap.password, msg->ap.pwd, sizeof(s_nm_wifi.ap_config->ap.password));
                        s_nm_wifi.ap_config->ap.authmode = WM_WIFI_AUTH_WPA2_PSK;
                    } else {
                        memset(s_nm_wifi.ap_config->ap.password, 0, sizeof(s_nm_wifi.ap_config->ap.password));
                        s_nm_wifi.ap_config->ap.authmode = WM_WIFI_AUTH_OPEN;
                    }
                } else {
                    if (s_nm_wifi.ap_config == NULL || !s_nm_wifi.ap_config->ap.ssid[0]) {
                        wm_log_warn("have not sofAP config, ignoring ap start request");
                        break;
                    }
                }

#if CONFIG_NM_WIFI_SOFTAP_AUTO_CHANNEL
                s_nm_wifi.ap_config->ap.channel = s_nm_wifi.channel;
#endif

#if CONFIG_NM_WIFI_SOFTAP_CHANNEL
                s_nm_wifi.ap_config->ap.channel = CONFIG_NM_WIFI_SOFTAP_CHANNEL;
#endif

                if (s_nm_wifi.ap_config->ap.channel < 1 || s_nm_wifi.ap_config->ap.channel > 13) {
                    s_nm_wifi.ap_config->ap.channel = 1;
                }
                s_nm_wifi.ap_config->ap.max_connection  = CONFIG_NM_WIFI_SOFTAP_MAX_STA_CONN;
                s_nm_wifi.ap_config->ap.pairwise_cipher = WM_WIFI_CIPHER_TYPE_CCMP;

                if (wm_wifi_init() != WM_ERR_SUCCESS) {
                    wm_log_error("wifi init fail");
                    break;
                }
                ret = wm_wifi_set_config(WM_WIFI_IF_AP, s_nm_wifi.ap_config);
                if (ret) {
                    wm_log_error("set wifi config fail");
                    break;
                }
                ret = wm_wifi_ap_start();
                if (ret) {
                    wm_log_error("start ap fail");
                    break;
                }
                wm_log_info("start ap (%s) success", s_nm_wifi.ap_config->ap.ssid);
                break;
            }
            case WM_NM_AP_STOP:
            {
                wm_wifi_ap_stop();
                s_nm_wifi.ap_state = WM_NM_EXIT;
                if (s_nm_wifi.ap_config) {
                    memset(s_nm_wifi.ap_config, 0, sizeof(wm_wifi_config_t));
                    wm_os_internal_free(s_nm_wifi.ap_config);
                    s_nm_wifi.ap_config = NULL;
                }
                wm_log_warn("stop softAP");
                break;
            }
#endif
            default:
            {
                break;
            }
        }

        if (msg != NULL) {
            wm_os_internal_free(msg);
            msg = NULL;
        }
    }
    wm_log_warn("exit net manager task");
    wm_os_internal_task_del(NULL);
}

#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
static void wm_nm_sta_re_connect_timer_handle(void *ptmr, void *parg)
{
    (void)ptmr;
    (void)parg;
    wm_nm_msg_t msg = { 0 };
    msg.type        = WM_NM_STA_START_CONNECT;
    (void)wm_nm_core_send_msg(&msg);
}
#endif /* CONFIG_NM_WIFI_STA_AUTO_CONNECT */

int wm_nm_wifi_init(void)
{
    static wm_os_task_t task = NULL;
    wm_os_status_t err;

#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
    err = wm_os_internal_timer_create_ms(&s_nm_wifi.re_con_timer, wm_nm_sta_re_connect_timer_handle, NULL, WM_OS_WAIT_TIME_MAX,
                                         false, "nm_sta_con_out");
    if (WM_OS_STATUS_SUCCESS != err) {
        wm_log_error("net manager timer init failed");
        return WM_ERR_FAILED;
    }
#endif /* CONFIG_NM_WIFI_STA_AUTO_CONNECT */

    err = wm_os_internal_queue_create(&s_nm_wifi.queue, WM_NM_QUEUE_SIZE);
    if (WM_OS_STATUS_SUCCESS != err) {
        wm_log_error("net manager queue init failed");
        return WM_ERR_FAILED;
    }

    err = wm_os_internal_task_create(&task, "wm_nm", wm_nm_wifi_task, NULL, WM_TASK_NET_MANAGER_STACK, WM_TASK_NET_MANAGER_PRIO,
                                     0);
    if (WM_OS_STATUS_SUCCESS != err) {
        wm_log_error("net manager task init failed");
        return WM_ERR_FAILED;
    }
    return WM_ERR_SUCCESS;
}

int wm_nm_core_send_msg(wm_nm_msg_t *msg)
{
    assert(msg != NULL && s_nm_wifi.queue != NULL);
    wm_nm_msg_t *bak = wm_os_internal_malloc(sizeof(wm_nm_msg_t));

    if (bak == NULL) {
        return WM_ERR_NO_MEM;
    }

    memcpy(bak, msg, sizeof(wm_nm_msg_t));

    if (wm_os_internal_queue_send(s_nm_wifi.queue, bak) != WM_ERR_SUCCESS) {
        wm_os_internal_free(bak);
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

int wm_nm_wifi_station_reconnect_start(int time_ms)
{
#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
    if (s_nm_wifi.sta_config && s_nm_wifi.sta_config->sta.ssid[0]) {
        wm_os_internal_timer_change_ms(s_nm_wifi.re_con_timer, time_ms);
    }
#endif /* CONFIG_NM_WIFI_STA_AUTO_CONNECT */
    return WM_ERR_SUCCESS;
}

int wm_nm_wifi_station_reconnect_stop(void)
{
#if CONFIG_NM_WIFI_STA_AUTO_CONNECT
    wm_os_internal_timer_stop(s_nm_wifi.re_con_timer);
#endif /* CONFIG_NM_WIFI_STA_AUTO_CONNECT */
    return WM_ERR_SUCCESS;
}

#if CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
wm_nm_state_e wm_nm_get_wifi_station_state_internal(void)
{
    return s_nm_wifi.sta_state;
}

void wm_nm_set_wifi_station_state_internal(wm_nm_state_e state)
{
    s_nm_wifi.sta_state = state;
}
#endif

#if CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED
wm_nm_state_e wm_nm_get_wifi_softap_state_internal(void)
{
    return s_nm_wifi.ap_state;
}

void wm_nm_set_wifi_softap_state_internal(wm_nm_state_e state)
{
    s_nm_wifi.ap_state = state;
}
#endif
