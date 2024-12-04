#ifndef __WM_WIFI_DRV_H__
#define __WM_WIFI_DRV_H__

#include "wm_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WM_WIFI_DRV_AUTH_OPEN = 0,
    WM_WIFI_DRV_AUTH_WEP,
    WM_WIFI_DRV_AUTH_WPA,
    WM_WIFI_DRV_AUTH_WPA2,
    WM_WIFI_DRV_AUTH_WPA3,
    WM_WIFI_DRV_AUTH_WPAI,
} wm_wifi_drv_auth_mode_t;

typedef enum {
    WM_WIFI_DRV_KEY_ALG_NONE,
    WM_WIFI_DRV_KEY_ALG_WEP,
    WM_WIFI_DRV_KEY_ALG_TKIP,
    WM_WIFI_DRV_KEY_ALG_CCMP,
    WM_WIFI_DRV_KEY_ALG_BIP_CMAC_128
} wm_wifi_drv_key_alg_t;

typedef enum {
    WM_WIFI_DRV_AUTH_ALG_OPEN   = 0x1,
    WM_WIFI_DRV_AUTH_ALG_SHARED = 0x2,
    WM_WIFI_DRV_AUTH_ALG_SAE    = 0x10
} wm_wifi_drv_auth_alg_t;

typedef struct {
    uint8_t *bssid;
    uint32_t freq;
    int reassoc;
    uint8_t *req_ies;
    uint8_t *resp_ies;
    uint16_t req_ies_len;
    uint16_t resp_ies_len;
    uint16_t status_code;
    uint32_t timeout_value;
} wm_wifi_drv_asso_event_data_t;

typedef struct {
    uint32_t task_prio;
    uint32_t task_stk_size;

    uint8_t tx_buf_cnt;
    uint8_t rx_buf_cnt;

    uint8_t *tx_gain;
    uint8_t *mac_addr;
    uint8_t *ver;

    uint8_t show_info;

    uint8_t tempcomp_flag;
    uint8_t psm_chipsleep_flag;

    void (*disasso_event_callback)(uint8_t *bssid, uint16_t reason);
    void (*scan_completed_callback)(void);
    void (*asso_event_callback)(wm_wifi_drv_asso_event_data_t *event);
    void (*update_stat_callback)(int rssi);
    void (*external_auth_callback)(uint32_t akm);

    void (*rx_mgmt_callback)(const uint8_t *mgmt, uint32_t mgmt_len);

    void (*softap_failed_callback)(void);
    void (*mic_err_callback)(int unicast, const uint8_t *addr);
    void (*unknown_sta_callback)(const uint8_t *addr);
} wm_wifi_drv_cfg_t;

typedef struct {
    uint8_t *bssid;
    uint8_t *ssid;
    int *freqs;
    uint8_t *ie;
    uint16_t ie_len;
    uint8_t ssid_len;
    uint8_t type;
#ifdef CONFIG_CONNET_ALG_OPT
    uint8_t fast_scan;      //0-all channel, 1-fast scan
    uint8_t sort_type;      //0-sort by rssi, 1-sort by secuty
    uint8_t auth_threshold; //ref wm_wifi_drv_auth_mode_t
    int rssi_threshold;     //used for fast scan
#endif
} wm_wifi_drv_scan_param_t;

typedef struct {
    int auth_alg;
    uint8_t *bssid;
    int freq;
    uint8_t *ssid;
    uint8_t ssid_len;
    uint8_t use_mfp;
    uint8_t *wpa_ie;
    size_t wpa_ie_len;
    int pairwise_suite;
    int group_suite;
    int key_mgmt_suite;
} wm_wifi_drv_asso_param_t;

typedef struct {
    int is_for_ap;
    wm_wifi_drv_key_alg_t alg;
    const uint8_t *addr;
    int key_idx;
    int set_tx;
    const uint8_t *seq;
    size_t seq_len;
    const uint8_t *key;
    size_t key_len;
} wm_wifi_drv_key_param_t;

typedef struct {
    const uint8_t *bssid;
    int freq;
    int dtim_period;
    int beacon_int;
    const uint8_t *head;
    size_t head_len;
    const uint8_t *tail;
    size_t tail_len;
    int privacy;
    uint32_t key_mgmt_suites;
    wm_wifi_drv_auth_alg_t auth_alg;
    uint32_t wpa_version;
    uint32_t pairwise_ciphers;
    uint32_t group_cipher;
} wm_wifi_drv_softap_param_t;

int wm_wifi_drv_init(wm_wifi_drv_cfg_t *cfg);
int wm_wifi_drv_deinit(void);

void wm_wifi_drv_set_tempcomp_flag(int flag);

void wm_wifi_drv_set_psm_chipsleep_flag(uint32_t flag);

int wm_wifi_drv_scan_ex(wm_wifi_drv_scan_param_t *param);
void *wm_wifi_drv_get_scan_result(void);
int wm_wifi_drv_stop_scan(void);
int wm_wifi_drv_scan_set_time(uint32_t passive, uint32_t active_min, uint32_t active_max, uint32_t home_time);

int wm_wifi_drv_associate(wm_wifi_drv_asso_param_t *param);
void wm_wifi_drv_disconnect(void);

int wm_wifi_drv_set_external_auth_status(uint16_t status);

int wm_wifi_drv_send_mlme(uint8_t *buf, uint32_t len);

int wm_wifi_drv_send_eapol(uint8_t *buf, uint32_t len);

void wm_wifi_drv_sta_eapol_event_cb_register(void (*callback)(uint8_t *src, uint8_t *eapol, uint32_t len, void *arg),
                                             void *arg);
void wm_wifi_drv_ap_eapol_event_cb_register(void (*callback)(uint8_t *src, uint8_t *eapol, uint32_t len, void *arg), void *arg);

int wm_wifi_drv_sta_add(const uint8_t *addr, const uint8_t *supp_rates, uint32_t supp_rates_len);
int wm_wifi_drv_sta_remove(const uint8_t *addr);

int wm_wifi_drv_get_station_inactivity_duration(const uint8_t *addr);

int wm_wifi_drv_set_key(wm_wifi_drv_key_param_t *param);
int wm_wifi_drv_set_multicast_key(uint8_t *addr, uint8_t is_del);

int wm_wifi_drv_get_channel_num(void);

void wm_wifi_drv_set_mac(const uint8_t *mac_addr);

void wm_wifi_drv_get_sta_hw_ht_cap(uint16_t *capa, uint8_t *mcs, uint8_t *a_mpdu_params);
void wm_wifi_drv_get_ap_hw_ht_cap(uint16_t *capa, uint8_t *mcs, uint8_t *a_mpdu_params);

int wm_wifi_drv_softap_create(wm_wifi_drv_softap_param_t *param);
void wm_wifi_drv_softap_destroy(void);

void wm_wifi_drv_set_sta_rx_data_callback(int (*callback)(void *priv, uint8_t *buf, uint32_t buf_len), void *priv);
void wm_wifi_drv_set_ap_rx_data_callback(int (*callback)(void *priv, uint8_t *buf, uint32_t buf_len), void *priv);

int wm_wifi_drv_set_listen_interval(uint16_t listen_interval);

int wm_wifi_drv_set_ps(uint8_t type);
int wm_wifi_drv_get_ps(uint8_t *type);

int wm_wifi_drv_set_sta_protocol(uint8_t protocol);
int wm_wifi_drv_set_ap_protocol(uint8_t protocol);
int wm_wifi_drv_get_sta_protocol(uint8_t *protocol);
int wm_wifi_drv_get_ap_protocol(uint8_t *protocol);

int wm_wifi_drv_set_bandwidth(uint8_t bw);
int wm_wifi_drv_get_bandwidth(uint8_t *bw);

int wm_wifi_drv_get_channel_type(void);

int wm_wifi_drv_set_channel(uint8_t primary, int8_t second);
int wm_wifi_drv_get_channel(uint8_t *primary, int8_t *second);

int wm_wifi_drv_set_inactive_time(uint32_t sec);
int wm_wifi_drv_get_inactive_time(uint32_t *sec);

void wm_wifi_drv_set_country(const char *country_code);
void wm_wifi_drv_get_country(char *country_code);

void wm_wifi_drv_set_channel_amount(uint8_t start, uint8_t total);
void wm_wifi_drv_get_channel_amount(uint8_t *start, uint8_t *total);

int wm_wifi_drv_set_promiscuous_rx_callback(wm_wifi_promiscuous_cb_t cb);
int wm_wifi_drv_set_promiscuous(uint8_t enable);
int wm_wifi_drv_get_promiscuous(uint8_t *enable);
int wm_wifi_drv_set_promiscuous_filter(const wm_wifi_promiscuous_filter_t *filter);
int wm_wifi_drv_get_promiscuous_filter(wm_wifi_promiscuous_filter_t *filter);
int wm_wifi_drv_set_promiscuous_ctrl_filter(const wm_wifi_promiscuous_filter_t *filter);
int wm_wifi_drv_get_promiscuous_ctrl_filter(wm_wifi_promiscuous_filter_t *filter);

int wm_wifi_drv_send_raw(const void *data, uint16_t data_len, uint8_t raw_seq);
int wm_wifi_drv_set_raw_tx_rate(uint8_t rate_idx, uint8_t l_p_g);

uint8_t *wm_wifi_drv_acquire_buffer(uint32_t total_len);
void wm_wifi_drv_release_buffer(uint8_t *buffer, uint8_t is_ap);

int wm_wifi_drv_forbid_tx(uint32_t timeout);
void wm_wifi_drv_permit_tx(void);

#ifdef __cplusplus
}
#endif

#endif /* __WM_WIFI_DRV_H__ */
