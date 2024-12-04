/*
 * Driver interface for RADIUS server or WPS ER only (no driver)
 * Copyright (c) 2008, Atheros Communications
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "wmsdk_config.h"
#include "wm_task_config.h"
#include "includes.h"

#include "common.h"
#include "driver.h"
#include "eloop.h"

#include "wpa_supplicant_i.h"
#include "bss.h"

#include "freertos/FreeRTOS.h"
#include "wm_ft_param.h"
#include "wm_nvs.h"
#include "wm_key_config.h"
#ifdef CONFIG_USE_EVENT_POST
#include "wm_event.h"
#endif
#include "wm_wifi_drv.h"
#include "wm_wifi_event.h"

struct wm_wifi_driver_event_data {
    enum wpa_event_type event;
    union wpa_event_data data;
};

struct wm_wifi_driver_data {
	void *ctx; //wpa_s

	u8 mac_addr[ETH_ALEN];

	u8 bssid[ETH_ALEN];
	u8 ssid[SSID_MAX_LEN];
	u8 ssid_len;
};

struct wm_wifi_driver_data *wm_wifi_driver_ctx_sta = NULL;
#ifdef CONFIG_AP
struct wm_wifi_driver_data *wm_wifi_driver_ctx_ap = NULL;
#endif

static uint8_t wifi_driver_inited = 0;

#ifdef CONFIG_USE_EVENT_POST
extern int wifi_rssi_threshold;
#endif
#ifdef CONFIG_CONNET_ALG_OPT
extern struct wpa_ssid * wpa_config_get_network(struct wpa_config *config, int id);
#endif

void wm_wifi_driver_free_event(void *eloop_ctx, void *user_ctx)
{
    struct wm_wifi_driver_event_data *event = user_ctx;

    if (EVENT_ASSOC == event->event)
    {
        if (event->data.assoc_info.addr)
            os_free((uint8_t *)event->data.assoc_info.addr);
        if (event->data.assoc_info.req_ies)
            os_free((uint8_t *)event->data.assoc_info.req_ies);
        if (event->data.assoc_info.resp_ies)
            os_free((uint8_t *)event->data.assoc_info.resp_ies);
    }
    else if (EVENT_ASSOC_REJECT == event->event)
    {
        if (event->data.assoc_reject.bssid)
            os_free((uint8_t *)event->data.assoc_reject.bssid);
        if (event->data.assoc_reject.resp_ies)
            os_free((uint8_t *)event->data.assoc_reject.resp_ies);
    }
    else if (EVENT_DISASSOC == event->event)
    {
        if (event->data.disassoc_info.addr)
            os_free((uint8_t *)event->data.disassoc_info.addr);
    }
    else if (EVENT_MICHAEL_MIC_FAILURE == event->event)
    {
        if (event->data.michael_mic_failure.src)
            os_free((uint8_t *)event->data.michael_mic_failure.src);
    }
    else if (EVENT_RX_MGMT == event->event)
    {
        if (event->data.rx_mgmt.frame)
            os_free((uint8_t *)event->data.rx_mgmt.frame);
    }
    else if (EVENT_RX_FROM_UNKNOWN == event->event)
    {
        if (event->data.rx_from_unknown.addr)
            os_free((uint8_t *)event->data.rx_from_unknown.addr);
    }

    os_free(event);
}

void wm_wifi_driver_proc_event(void *eloop_ctx, void *user_ctx)
{
    struct wm_wifi_driver_data *drv = eloop_ctx;
    struct wm_wifi_driver_event_data *event = user_ctx;

    wpa_supplicant_event(drv->ctx, event->event, &event->data);

    wm_wifi_driver_free_event(eloop_ctx, user_ctx);
}

int wm_wifi_driver_send_event(int is_ap, int event, union wpa_event_data *data)
{
    int ret;
    void *drv = NULL;
    struct wm_wifi_driver_event_data *e = NULL;

    e = os_zalloc(sizeof(struct wm_wifi_driver_event_data));
    if (!e)
        return -1;

    e->event = event;
    if (data)
        os_memcpy(&e->data, data, sizeof(union wpa_event_data));

#ifdef CONFIG_AP
    if (is_ap)
        drv = wm_wifi_driver_ctx_ap;
    else
#endif
        drv = wm_wifi_driver_ctx_sta;

    ret = eloop_register_timeout(0, 0, wm_wifi_driver_proc_event, drv, e);
    if (ret)
        os_free(e);

    return ret;
}

static void wm_wifi_driver_scan_callback(void)
{
    wm_wifi_driver_send_event(0, EVENT_SCAN_RESULTS, NULL);
}

static void wm_wifi_driver_asso_event_callback(wm_wifi_drv_asso_event_data_t *event)
{
    union wpa_event_data data;

    os_memset(&data, 0, sizeof(data));

    if (WLAN_STATUS_SUCCESS == event->status_code)
    {
        data.assoc_info.addr = os_malloc(6);
        if (!data.assoc_info.addr) return;
        os_memcpy((uint8_t *)data.assoc_info.addr, event->bssid, 6);

        if (event->req_ies && event->req_ies_len)
        {
            data.assoc_info.req_ies = os_malloc(event->req_ies_len);
            if (!data.assoc_info.req_ies)
            {
                os_free((uint8_t *)data.assoc_info.addr);
                return;
            }
            os_memcpy((uint8_t *)data.assoc_info.req_ies, event->req_ies, event->req_ies_len);
            data.assoc_info.req_ies_len = event->req_ies_len;
        }

        if (event->resp_ies && event->resp_ies_len)
        {
            data.assoc_info.resp_ies = os_malloc(event->resp_ies_len);
            if (!data.assoc_info.resp_ies)
            {
                os_free((uint8_t *)data.assoc_info.addr);
                os_free((uint8_t *)data.assoc_info.req_ies);
                return;
            }
            os_memcpy((uint8_t *)data.assoc_info.resp_ies, event->resp_ies, event->resp_ies_len);
            data.assoc_info.resp_ies_len = event->resp_ies_len;
        }

        data.assoc_info.freq = event->freq;
        data.assoc_info.reassoc = event->reassoc;

        if (wm_wifi_driver_send_event(0, EVENT_ASSOC, &data))
        {
            os_free((uint8_t *)data.assoc_info.addr);
            os_free((uint8_t *)data.assoc_info.req_ies);
            os_free((uint8_t *)data.assoc_info.resp_ies);
        }
    }
    else
    {
        data.assoc_reject.bssid = os_malloc(6);
        if (!data.assoc_reject.bssid) return;
        os_memcpy((uint8_t *)data.assoc_reject.bssid, event->bssid, 6);
    
		data.assoc_reject.status_code = event->status_code;
		data.assoc_reject.timed_out = event->timeout_value ? 1 : 0;
		data.assoc_reject.timeout_reason = "assoc";

        if (event->resp_ies && event->resp_ies_len)
        {
            data.assoc_reject.resp_ies = os_malloc(event->resp_ies_len);
            if (!data.assoc_reject.resp_ies)
            {
                os_free((uint8_t *)data.assoc_reject.bssid);
                return;
            }
            os_memcpy((uint8_t *)data.assoc_reject.resp_ies, event->resp_ies, event->resp_ies_len);
            data.assoc_reject.resp_ies_len = event->resp_ies_len;
        }

		if (wm_wifi_driver_send_event(0, EVENT_ASSOC_REJECT, &data))
        {
            os_free((uint8_t *)data.assoc_reject.bssid);
            os_free((uint8_t *)data.assoc_reject.resp_ies);
        }
    }
}

static void wm_wifi_driver_disasso_event_callback(uint8_t *bssid, uint16_t reason)
{
    union wpa_event_data data;

    os_memset(&data, 0, sizeof(data));

    data.disassoc_info.addr = os_malloc(6);
    if (!data.disassoc_info.addr) return;
    os_memcpy((uint8_t *)data.disassoc_info.addr, bssid, 6);

    data.disassoc_info.reason_code = reason;

    if (wm_wifi_driver_send_event(0, EVENT_DISASSOC, &data))
    {
        os_free((uint8_t *)data.disassoc_info.addr);
    }
}

static void wm_wifi_driver_update_stat_event(void *eloop_ctx, void *user_ctx)
{
    struct wm_wifi_driver_data *drv = eloop_ctx;
    struct wpa_supplicant *wpa_s = drv->ctx;
    int rssi = (int)user_ctx;

    if (wpa_s->current_bss)
    {
        wpa_s->current_bss->level = rssi;
    }

#ifdef CONFIG_USE_EVENT_POST
    if (rssi < wifi_rssi_threshold)
    {
        wm_wifi_event_data_t data;
        data.sta_bss_rssi_low_info.rssi = rssi;
        wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_STA_BSS_RSSI_LOW, &data,sizeof(data));
    }
#endif
}

static void wm_wifi_driver_update_stat_event_callback(int rssi)
{
    eloop_register_timeout(0, 0, wm_wifi_driver_update_stat_event, wm_wifi_driver_ctx_sta, (void *)rssi);
}

#ifdef CONFIG_SAE
static void wm_wifi_driver_external_auth(void *eloop_ctx, void *user_ctx)
{
    union wpa_event_data data;
    struct wm_wifi_driver_data *drv = eloop_ctx;

	memset(&data, 0, sizeof(union wpa_event_data));
	
	data.external_auth.action = EXT_AUTH_START;
	data.external_auth.bssid = drv->bssid;
	data.external_auth.ssid_len = drv->ssid_len;
	data.external_auth.ssid = drv->ssid;
	data.external_auth.key_mgmt_suite = (uint32_t)user_ctx;

	wm_wifi_driver_send_event(0, EVENT_EXTERNAL_AUTH, &data);
}

static void wm_wifi_driver_external_auth_callback(uint32_t akm)
{
    eloop_register_timeout(0, 0, wm_wifi_driver_external_auth, wm_wifi_driver_ctx_sta, (void *)akm);
}
#endif

#ifdef CONFIG_AP
static void wm_wifi_driver_softap_failed_callback(void)
{
    wm_wifi_driver_send_event(1, EVENT_INTERFACE_DISABLED, NULL);
    wm_wifi_driver_send_event(1, EVENT_INTERFACE_ENABLED, NULL);
}

static void wm_wifi_driver_mic_err_callback(int unicast, const uint8_t *addr)
{
    union wpa_event_data data;

    os_memset(&data, 0, sizeof(data));

    data.michael_mic_failure.src = os_malloc(6);
    if (!data.michael_mic_failure.src) return;
    os_memcpy((uint8_t *)data.michael_mic_failure.src, addr, 6);

    data.michael_mic_failure.unicast = unicast;

    if (wm_wifi_driver_send_event(1, EVENT_MICHAEL_MIC_FAILURE, &data))
    {
        os_free((uint8_t *)data.michael_mic_failure.src);
    }
}

static void wm_wifi_driver_unknown_sta_callback(const uint8_t *addr)
{
    union wpa_event_data data;

    os_memset(&data, 0, sizeof(data));

    data.rx_from_unknown.addr = os_malloc(6);
    if (!data.rx_from_unknown.addr) return;
    os_memcpy((uint8_t *)data.rx_from_unknown.addr, addr, 6);

    data.rx_from_unknown.bssid = wm_wifi_driver_ctx_ap->mac_addr;

    if (wm_wifi_driver_send_event(1, EVENT_RX_FROM_UNKNOWN, &data))
    {
        os_free((uint8_t *)data.rx_from_unknown.addr);
    }
}
#endif

#if defined(CONFIG_SAE) || defined(CONFIG_AP)
static void wm_wifi_driver_rx_mgmt_callback(const uint8_t *mgmt, uint32_t mgmt_len)
{
    u8 from_sta = 1;
    union wpa_event_data data;
    struct ieee80211_mgmt *frame;

    os_memset(&data, 0, sizeof(data));

    data.rx_mgmt.frame = os_malloc(mgmt_len);
    if (!data.rx_mgmt.frame) return;
    os_memcpy((uint8_t *)data.rx_mgmt.frame, mgmt, mgmt_len);

    data.rx_mgmt.frame_len = mgmt_len;

    frame = (struct ieee80211_mgmt *)mgmt;
    if (memcmp(wm_wifi_driver_ctx_sta->bssid, frame->bssid, ETH_ALEN))
        from_sta = 0;

    if (wm_wifi_driver_send_event(!from_sta, EVENT_RX_MGMT, &data))
    {
        os_free((uint8_t *)data.rx_mgmt.frame);
    }
}
#endif

static void *wm_wifi_driver_init(void *ctx, const char *ifname)
{
	struct wm_wifi_driver_data *drv;
	u8 def_mac[WM_MAC_ADDR_LEN] = {0};

	drv = os_zalloc(sizeof(struct wm_wifi_driver_data));
	if (drv == NULL) {
		wpa_printf(MSG_ERROR, "Could not allocate memory for none "
			   "driver data");
		return NULL;
	}
	drv->ctx = ctx;

    if (!strcmp(ifname, CONFIG_IFACE_NAME_STA))
    {
        if (wm_sys_get_mac_addr(WM_MAC_TYPE_STA, def_mac, sizeof(def_mac)) != WM_ERR_SUCCESS) {
            return NULL;
        }
        wm_wifi_driver_ctx_sta = drv;
    }
#ifdef CONFIG_AP
    else
    {
        if (wm_sys_get_mac_addr(WM_MAC_TYPE_SAP, def_mac, sizeof(def_mac)) != WM_ERR_SUCCESS) {
            return NULL;
        }
        wm_wifi_driver_ctx_ap = drv;
    }
#endif

    os_memcpy(drv->mac_addr, def_mac, ETH_ALEN);

    if (!wifi_driver_inited)
    {
    	u8 def_gain[WM_FT_TX_GAIN_LEN] = {0xFF};
        wm_ft_get_tx_gain(def_gain);
        wm_wifi_drv_cfg_t drv_cfg;
        os_memset(&drv_cfg, 0, sizeof(drv_cfg));
        drv_cfg.task_prio = WM_TASK_WIFI_DRV_PRIO/*CONFIG_WIFI_MASCW_TASK_PRIO*/;
        drv_cfg.task_stk_size = WM_TASK_WIFI_DRV_STACK/*CONFIG_WIFI_MASCW_TASK_STACK_SIZE*/;
        drv_cfg.tx_buf_cnt = CONFIG_WIFI_MASCW_TX_BUF_NUM;
        drv_cfg.rx_buf_cnt = CONFIG_WIFI_MASCW_RX_BUF_NUM;
        drv_cfg.tx_gain = def_gain;
        drv_cfg.mac_addr = def_mac;
#if defined(CONFIG_WPA_SUPPLICANT_LOG_LEVEL_INFO) || defined(CONFIG_WPA_SUPPLICANT_LOG_LEVEL_DEBUG) || defined(CONFIG_WPA_SUPPLICANT_LOG_LEVEL_MSGDUMP) || defined(CONFIG_WPA_SUPPLICANT_LOG_LEVEL_EXCESSIVE)
        drv_cfg.show_info = 1;
#endif
        drv_cfg.scan_completed_callback = wm_wifi_driver_scan_callback;
        drv_cfg.asso_event_callback = wm_wifi_driver_asso_event_callback;
        drv_cfg.disasso_event_callback = wm_wifi_driver_disasso_event_callback;
        drv_cfg.update_stat_callback = wm_wifi_driver_update_stat_event_callback;
#ifdef CONFIG_SAE
        drv_cfg.external_auth_callback = wm_wifi_driver_external_auth_callback;
#endif
#ifdef CONFIG_AP
        drv_cfg.softap_failed_callback = wm_wifi_driver_softap_failed_callback;
        drv_cfg.mic_err_callback = wm_wifi_driver_mic_err_callback;
        drv_cfg.unknown_sta_callback = wm_wifi_driver_unknown_sta_callback;
#endif
#if defined(CONFIG_SAE) || defined(CONFIG_AP)
        drv_cfg.rx_mgmt_callback = wm_wifi_driver_rx_mgmt_callback;
#endif
        if (wm_wifi_drv_init(&drv_cfg))
        {
            os_free(drv);
            return NULL;
        }

        wifi_driver_inited = 1;
    }

	return drv;
}

static void wm_wifi_driver_deinit(void *priv)
{
    u8 need_deinit = 0;
	struct wm_wifi_driver_data *drv = priv;

	os_free(drv);

    if (drv == wm_wifi_driver_ctx_sta)
    {
        wm_wifi_driver_ctx_sta = NULL;
    }
#ifdef CONFIG_AP
    else
    {
        wm_wifi_driver_ctx_ap = NULL;
        need_deinit = 1;
    }
#else  //CONFIG_AP
    need_deinit = 1;
#endif //CONFIG_AP

    if (need_deinit && wifi_driver_inited)
    {
        if (!wm_wifi_drv_deinit())
            wifi_driver_inited = 0;
    }
}

static struct hostapd_hw_modes *wm_wifi_driver_get_hw_feature_data(void *priv,
                            							 u16 *num_modes,
                            							 u16 *flags, u8 *dfs)
{
    int i;
    struct hostapd_hw_modes *modes;
    struct hostapd_channel_data *chan;

    modes = os_zalloc(sizeof(struct hostapd_hw_modes));
	if (!modes)
    {
		return NULL;
    }

    modes->mode = HOSTAPD_MODE_IEEE80211G;
	modes->num_channels = wm_wifi_drv_get_channel_num();
	modes->channels = os_zalloc(modes->num_channels *
					     sizeof(struct hostapd_channel_data));
	if (!modes->channels)
    {
        os_free(modes);
		return NULL;
    }

	chan = modes->channels;
	for (i = 0 ; i < modes->num_channels; i++, chan++) {
		if (i != 13) chan->freq = 2412 + i * 5;
		else chan->freq = 2484;
    	chan->chan = i + 1;
    	chan->flag = HOSTAPD_CHAN_HT40;
        chan->allowed_bw = HOSTAPD_CHAN_WIDTH_20 | HOSTAPD_CHAN_WIDTH_40P | HOSTAPD_CHAN_WIDTH_40M;
	}

    int leg_rate[] = {10, 20, 55, 110, 60, 90, 120, 180, 240, 360, 480, 540};
	modes->rates = os_zalloc(sizeof(leg_rate));
	if (!modes->rates)
    {
        os_free(modes->channels);
        os_free(modes);
		return NULL;
    }

    os_memcpy(modes->rates, leg_rate, sizeof(leg_rate));
    modes->num_rates = sizeof(leg_rate) / sizeof(int);

	modes->flags |= HOSTAPD_MODE_FLAG_HT_INFO_KNOWN;

    if (wm_wifi_driver_ctx_sta == priv)
    {
        wm_wifi_drv_get_sta_hw_ht_cap(&modes->ht_capab, modes->mcs_set, &modes->a_mpdu_params);
    }
    else
    {
        wm_wifi_drv_get_ap_hw_ht_cap(&modes->ht_capab, modes->mcs_set, &modes->a_mpdu_params);
    }

	*num_modes = 1;

    return modes;
}

static int wm_wifi_driver_get_capa(void *priv, struct wpa_driver_capa *capa)
{
    os_memset(capa, 0, sizeof(*capa));

    capa->key_mgmt = WPA_DRIVER_CAPA_KEY_MGMT_WPA |
                     WPA_DRIVER_CAPA_KEY_MGMT_WPA_PSK |
                     WPA_DRIVER_CAPA_KEY_MGMT_WPA2 |
                     WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK;

	capa->enc = WPA_DRIVER_CAPA_ENC_WEP40 |
                WPA_DRIVER_CAPA_ENC_WEP104 |
#ifdef CONFIG_SAE
                WPA_DRIVER_CAPA_ENC_BIP |
#endif
                WPA_DRIVER_CAPA_ENC_TKIP |
                WPA_DRIVER_CAPA_ENC_CCMP;

                capa->auth = WPA_DRIVER_AUTH_OPEN |
                WPA_DRIVER_AUTH_SHARED;

	capa->flags = WPA_DRIVER_FLAGS_SET_KEYS_AFTER_ASSOC_DONE |
#ifdef CONFIG_AP
                  WPA_DRIVER_FLAGS_AP |
                  WPA_DRIVER_FLAGS_AP_UAPSD |
                  WPA_DRIVER_FLAGS_AP_MLME |
#endif
#ifdef CONFIG_SAE
                  WPA_DRIVER_FLAGS_SAE |
#endif
                  WPA_DRIVER_FLAGS_HT_2040_COEX |
                  WPA_DRIVER_FLAGS_EAPOL_TX_STATUS;

	capa->wmm_ac_supported = 0;
	capa->max_scan_ssids = 1;
	capa->max_stations = 8;
	capa->num_multichan_concurrent = 2;

    return 0;
}

static const u8 *wm_wifi_driver_get_mac_addr(void *priv)
{
    struct wm_wifi_driver_data *drv = priv;

    return drv->mac_addr;
}

static int wm_wifi_driver_set_mac_addr(void *priv, const u8 *addr)
{
    struct wm_wifi_driver_data *drv = priv;

    if (wm_wifi_driver_ctx_sta == drv)
        wm_wifi_drv_set_mac(addr);
    os_memcpy(drv->mac_addr, addr, ETH_ALEN);
    wpa_supplicant_update_mac_addr(drv->ctx);

    return 0;
}

static int wm_wifi_driver_set_country(void *priv, const char *alpha2)
{
    wm_wifi_drv_set_country(alpha2);

    return 0;
}

static int wm_wifi_driver_get_country(void *priv, char *alpha2)
{
    wm_wifi_drv_get_country(alpha2);

    return 0;
}

static int wm_wifi_driver_scan2(void *priv, struct wpa_driver_scan_params *params)
{
    uint8_t type = 0;
    uint8_t *ssid = NULL;
    uint8_t ssid_len = 0;
    wm_wifi_drv_scan_param_t param;
#ifdef CONFIG_CONNET_ALG_OPT
    struct wm_wifi_driver_data *drv = priv;
    struct wpa_supplicant *wpa_s = drv->ctx;
    struct wpa_ssid *ssid_conf;
#endif

    if (wm_wifi_driver_ctx_sta != priv)
        return -1;

    if (params->num_ssids)
    {
        ssid= (uint8_t *)params->ssids[0].ssid;
        ssid_len = params->ssids[0].ssid_len;
    }
    else
    {
        type = 1;
    }

    param.type = type;
    param.bssid = (uint8_t *)params->bssid;
    param.ssid = ssid;
    param.ssid_len = ssid_len;
    param.freqs = params->freqs;
    param.ie = (uint8_t *)params->extra_ies;
    param.ie_len = params->extra_ies_len;
#ifdef CONFIG_CONNET_ALG_OPT
    ssid_conf = wpa_config_get_network(wpa_s->conf, 0);
    if (!ssid_conf || (NORMAL_SCAN_REQ != wpa_s->last_scan_req))
    {
        param.fast_scan = 0;
        param.sort_type = 0;
        param.auth_threshold = 0;
        param.rssi_threshold = 0;
    }
    else
    {
        param.fast_scan = ssid_conf->fast_scan;
        param.sort_type = ssid_conf->sort_type;
        param.auth_threshold = ssid_conf->auth_threshold;
        param.rssi_threshold = ssid_conf->rssi_threshold;
    }
#endif

    return wm_wifi_drv_scan_ex(&param) ? -1 : 0;
}

static struct wpa_scan_results *wm_wifi_driver_get_scan_results2(void *priv)
{
    int i;
    struct wpa_scan_res *bss;
    struct wpa_scan_results *res;

    if (wm_wifi_driver_ctx_sta != priv)
        return NULL;

    res = wm_wifi_drv_get_scan_result();
    if (res)
    {
        for (i = 0; i < res->num; i++)
        {
            bss = res->res[i];
            bss->flags = WPA_SCAN_QUAL_INVALID | WPA_SCAN_NOISE_INVALID | WPA_SCAN_LEVEL_DBM;
        }

        os_get_reltime(&res->fetch_time);
    }

    return res;
}

static int wm_wifi_driver_abort_scan(void *priv, u64 scan_cookie)
{
    if (wm_wifi_driver_ctx_sta != priv)
        return -1;

    return wm_wifi_drv_stop_scan();
}

static int wm_wifi_driver_associate(void *priv, struct wpa_driver_associate_params *params)
{
    wm_wifi_drv_asso_param_t arg;

    if (wm_wifi_driver_ctx_sta != priv)
        return 0;

    os_memcpy(wm_wifi_driver_ctx_sta->bssid, params->bssid, ETH_ALEN);
    wm_wifi_driver_ctx_sta->ssid_len = params->ssid_len;
    os_memcpy(wm_wifi_driver_ctx_sta->ssid, params->ssid, wm_wifi_driver_ctx_sta->ssid_len);

    arg.auth_alg = 0;
    if (params->auth_alg & WPA_AUTH_ALG_OPEN)
        arg.auth_alg |= WM_WIFI_DRV_AUTH_ALG_OPEN;
    if (params->auth_alg & WPA_AUTH_ALG_SHARED)
        arg.auth_alg |= WM_WIFI_DRV_AUTH_ALG_SHARED;
#ifdef CONFIG_SAE
    if (params->auth_alg & WPA_AUTH_ALG_SAE)
        arg.auth_alg |= WM_WIFI_DRV_AUTH_ALG_SAE;
    arg.use_mfp = MGMT_FRAME_PROTECTION_REQUIRED == params->mgmt_frame_protection ? 1 : 0;
#endif
    arg.bssid = (uint8_t *)params->bssid;
    arg.freq = params->freq.freq;
    arg.ssid = (uint8_t *)params->ssid;
    arg.ssid_len = params->ssid_len;
    arg.wpa_ie = (uint8_t *)params->wpa_ie;
    arg.wpa_ie_len = params->wpa_ie_len;
    arg.pairwise_suite = params->pairwise_suite;
    arg.group_suite = params->group_suite;
    arg.key_mgmt_suite = params->key_mgmt_suite;

    return wm_wifi_drv_associate(&arg);
}

static int wm_wifi_driver_get_bssid(void *priv, u8 *bssid)
{
    os_memcpy(bssid, wm_wifi_driver_ctx_sta->bssid, ETH_ALEN);

    return 0;
}

static int wm_wifi_driver_get_ssid(void *priv, u8 *ssid)
{
    os_memcpy(ssid, wm_wifi_driver_ctx_sta->ssid, wm_wifi_driver_ctx_sta->ssid_len);

    return wm_wifi_driver_ctx_sta->ssid_len;
}

static int wm_wifi_driver_set_key(void *priv, struct wpa_driver_set_key_params *params)
{
    int ret;
    int is_for_ap = 0;
    wm_wifi_drv_key_param_t arg;

    switch (params->alg)
    {
        case WPA_ALG_NONE:
        {
            arg.alg = WM_WIFI_DRV_KEY_ALG_NONE;
            break;
        }
        case WPA_ALG_WEP:
        {
            arg.alg = WM_WIFI_DRV_KEY_ALG_WEP;
            break;
        }
        case WPA_ALG_TKIP:
        {
            arg.alg = WM_WIFI_DRV_KEY_ALG_TKIP;
            break;
        }
        case WPA_ALG_CCMP:
        {
            arg.alg = WM_WIFI_DRV_KEY_ALG_CCMP;
            break;
        }
#ifdef CONFIG_SAE
        case WPA_ALG_BIP_CMAC_128:
        {
            arg.alg = WM_WIFI_DRV_KEY_ALG_BIP_CMAC_128;
            break;
        }
#endif
        default:
        {
            return -1;
        }
    }

    if (wm_wifi_driver_ctx_sta != priv)
        is_for_ap = 1;

    arg.is_for_ap = is_for_ap;
    arg.addr = params->addr;
    arg.key_idx = params->key_idx;
    arg.set_tx = params->set_tx;
    arg.seq = params->seq;
    arg.seq_len = params->seq_len;
    arg.key = params->key;
    arg.key_len = params->key_len;

    ret = wm_wifi_drv_set_key(&arg);

    return ret;
}

#ifdef CONFIG_SAE
static int wm_wifi_driver_send_external_auth_status(void *priv,
					                    struct external_auth *params)
{
    return wm_wifi_drv_set_external_auth_status(params->status);
}
#endif

static int wm_wifi_driver_deauthenticate(void *priv, const u8 *addr, u16 reason_code)
{
    if (wm_wifi_driver_ctx_sta != priv)
        return -1;

    os_memset(wm_wifi_driver_ctx_sta->bssid, 0, ETH_ALEN);

    wm_wifi_drv_disconnect();

    return 0;
}

static int wm_wifi_driver_send_action(void *priv, unsigned int freq, unsigned int wait,
                			   const u8 *dst, const u8 *src, const u8 *bssid,
                			   const u8 *data, size_t data_len, int no_cck)
{
    struct ieee80211_hdr *hdr;
    u8 *buf;

    buf = os_zalloc(IEEE80211_HDRLEN + data_len);
    if (buf == NULL)
        return -1;

    os_memcpy(buf + IEEE80211_HDRLEN, data, data_len);

    hdr = (struct ieee80211_hdr *)buf;
    hdr->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT, WLAN_FC_STYPE_ACTION);
    os_memcpy(hdr->addr1, dst, ETH_ALEN);
    os_memcpy(hdr->addr2, src, ETH_ALEN);
    os_memcpy(hdr->addr3, bssid, ETH_ALEN);

    int ret = wm_wifi_drv_send_mlme(buf, IEEE80211_HDRLEN + data_len);

    os_free(buf);

    return ret;
}

static int wm_wifi_driver_send_mlme(void *priv, const u8 *data, size_t data_len,
                      			 int noack, unsigned int freq, const u16 *csa_offs,
                      			 size_t csa_offs_len, int no_encrypt,
                      			 unsigned int wait)
{
    int ret;
    union wpa_event_data event_data;
    struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)data;

    ret = wm_wifi_drv_send_mlme((uint8_t *)data, data_len);
    if (!ret)
    {
        os_memset(&event_data, 0, sizeof(event_data));

        event_data.tx_status.type = WLAN_FC_GET_TYPE(mgmt->frame_control);
		event_data.tx_status.stype = WLAN_FC_GET_STYPE(mgmt->frame_control);
		event_data.tx_status.dst = mgmt->da;
		event_data.tx_status.data = data;
		event_data.tx_status.data_len = data_len;
		event_data.tx_status.ack = noack ? 0 : 1;

        ret = wm_wifi_driver_send_event(1, EVENT_TX_STATUS, &event_data);
    }

    return ret;
}

#ifdef CONFIG_AP
static int wm_wifi_driver_set_ap(void *priv, struct wpa_driver_ap_params *params)
{
    struct wm_wifi_driver_data *drv = priv;
    wm_wifi_drv_softap_param_t arg;

    arg.auth_alg = 0;
    if (params->auth_algs & WPA_AUTH_ALG_OPEN)
        arg.auth_alg |= WM_WIFI_DRV_AUTH_ALG_OPEN;
    if (params->auth_algs & WPA_AUTH_ALG_SHARED)
        arg.auth_alg |= WM_WIFI_DRV_AUTH_ALG_SHARED;

    arg.bssid = drv->mac_addr;
    arg.freq = params->freq->freq;
    arg.dtim_period = params->dtim_period;
    arg.beacon_int = params->beacon_int;
    arg.head = params->head;
    arg.head_len = params->head_len;
    arg.tail = params->tail;
    arg.tail_len = params->tail_len;
    arg.privacy = params->privacy;
    arg.key_mgmt_suites = params->key_mgmt_suites;
    arg.wpa_version = params->wpa_version;
    arg.pairwise_ciphers = params->pairwise_ciphers;
    arg.group_cipher = params->group_cipher;

    return wm_wifi_drv_softap_create(&arg);
}

static int wm_wifi_driver_deinit_ap(void *priv)
{
    wm_wifi_drv_softap_destroy();

    return 0;
}

static int wm_wifi_driver_sta_add(void *priv, struct hostapd_sta_add_params *params)
{
    return wm_wifi_drv_sta_add(params->addr, params->supp_rates, params->supp_rates_len);
}

static int wm_wifi_driver_sta_remove(void *priv, const u8 *addr)
{
    return wm_wifi_drv_sta_remove(addr);
}

static int wm_wifi_driver_hapd_send_eapol(void *priv, const u8 *addr, const u8 *data,
                                          size_t data_len, int encrypt,
                                          const u8 *own_addr, u32 flags)
{
    //struct wm_wifi_driver_data *drv = priv;

    size_t eapol_len = ETH_HLEN + data_len;
    u8 *eapol = os_malloc(eapol_len);
    if (!eapol) return -1;
    os_memcpy(eapol, addr, ETH_ALEN);
    os_memcpy(eapol + ETH_ALEN, own_addr, ETH_ALEN);//drv->mac_addr
    *(u16 *)(eapol + ETH_ALEN + ETH_ALEN) = host_to_be16(0x888e);
    os_memcpy(eapol + ETH_HLEN, data, data_len);
    int ret = wm_wifi_drv_send_eapol((uint8_t *)eapol, eapol_len);
    os_free(eapol);
    if (!ret)
    {
        union wpa_event_data event_data;
        os_memset(&event_data, 0, sizeof(event_data));

		event_data.eapol_tx_status.dst = addr;
		event_data.eapol_tx_status.data = data;
		event_data.eapol_tx_status.data_len = data_len;
		event_data.eapol_tx_status.ack = 1;

        ret = wm_wifi_driver_send_event(1, EVENT_EAPOL_TX_STATUS, &event_data);
    }

    return ret;
}

static int wm_wifi_driver_sta_deauth(void *priv, const u8 *own_addr, const u8 *addr,
			                         u16 reason)
{
    struct ieee80211_mgmt *hdr;
    u8 *buf;

    buf = os_zalloc(IEEE80211_HDRLEN + + sizeof(hdr->u.deauth));
    if (buf == NULL)
        return -1;

    hdr = (struct ieee80211_mgmt *)buf;
    hdr->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT, WLAN_FC_STYPE_DEAUTH);
    os_memcpy(hdr->da, addr, ETH_ALEN);
    os_memcpy(hdr->sa, own_addr, ETH_ALEN);
    os_memcpy(hdr->bssid, own_addr, ETH_ALEN);
    hdr->u.deauth.reason_code = host_to_le16(reason);

    int ret = wm_wifi_drv_send_mlme(buf, IEEE80211_HDRLEN + sizeof(hdr->u.deauth));

    os_free(buf);

    return ret;
}

static int wm_wifi_driver_sta_disassoc(void *priv, const u8 *own_addr, const u8 *addr,
                                       u16 reason)
{
    struct ieee80211_mgmt *hdr;
    u8 *buf;

    buf = os_zalloc(IEEE80211_HDRLEN + + sizeof(hdr->u.disassoc));
    if (buf == NULL)
        return -1;

    hdr = (struct ieee80211_mgmt *)buf;
    hdr->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT, WLAN_FC_STYPE_DISASSOC);
    os_memcpy(hdr->da, addr, ETH_ALEN);
    os_memcpy(hdr->sa, own_addr, ETH_ALEN);
    os_memcpy(hdr->bssid, own_addr, ETH_ALEN);
    hdr->u.disassoc.reason_code = host_to_le16(reason);

    int ret = wm_wifi_drv_send_mlme(buf, IEEE80211_HDRLEN + sizeof(hdr->u.disassoc));

    os_free(buf);

    return ret;
}

static int wm_wifi_driver_get_inact_sec(void *priv, const u8 *addr)
{
    int ret;

    ret = wm_wifi_drv_get_station_inactivity_duration(addr);
    if (-1 != ret)
    {
        ret = (ret * portTICK_PERIOD_MS) / 1000;
    }

    return ret;
}
#endif

const struct wpa_driver_ops wpa_driver_wm_wifi_ops = {
	.name = CONFIG_DRIVER_WM_WIFI_NAME,
	.desc = "winnermicro wifi driver fow w800",
	.init = wm_wifi_driver_init,
	.deinit = wm_wifi_driver_deinit,
	.get_hw_feature_data = wm_wifi_driver_get_hw_feature_data,
	.get_capa = wm_wifi_driver_get_capa,
	.get_mac_addr = wm_wifi_driver_get_mac_addr,
	.set_mac_addr = wm_wifi_driver_set_mac_addr,
	.set_country = wm_wifi_driver_set_country,
	.get_country = wm_wifi_driver_get_country,
    .scan2 = wm_wifi_driver_scan2,
	.get_scan_results2 = wm_wifi_driver_get_scan_results2,
	.abort_scan = wm_wifi_driver_abort_scan,
	.associate = wm_wifi_driver_associate,
	.get_bssid = wm_wifi_driver_get_bssid,
	.get_ssid = wm_wifi_driver_get_ssid,
	.set_key = wm_wifi_driver_set_key,
#ifdef CONFIG_SAE
	.send_external_auth_status = wm_wifi_driver_send_external_auth_status,
#endif
	.deauthenticate = wm_wifi_driver_deauthenticate,
	.send_action = wm_wifi_driver_send_action,
	.send_mlme = wm_wifi_driver_send_mlme,
#ifdef CONFIG_AP
    .set_ap = wm_wifi_driver_set_ap,
	.deinit_ap = wm_wifi_driver_deinit_ap,
	.sta_add = wm_wifi_driver_sta_add,
	.sta_remove = wm_wifi_driver_sta_remove,
	.hapd_send_eapol = wm_wifi_driver_hapd_send_eapol,
	.sta_deauth = wm_wifi_driver_sta_deauth,
	.sta_disassoc = wm_wifi_driver_sta_disassoc,
	.get_inact_sec = wm_wifi_driver_get_inact_sec,
#endif
};
