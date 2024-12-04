#ifndef __WM_WIFI_EVENT_H__
#define __WM_WIFI_EVENT_H__

#include "wm_event.h"

#ifdef __cplusplus
extern "C" {
#endif

WM_EVENT_DECLARE_GROUP(WM_WIFI_EV); /**< declare wifi event group */

/** @brief wifi event type */
enum {
    WM_EVENT_WIFI_SCAN_DONE = 0,      /**< finish scanning AP */
    WM_EVENT_WIFI_STA_CONNECTED,      /**< station connected to AP */
    WM_EVENT_WIFI_STA_DISCONNECTED,   /**< station disconnected from AP */
    WM_EVENT_WIFI_STA_BSS_RSSI_LOW,   /**< AP's RSSI crossed configured threshold */
    WM_EVENT_WIFI_STA_BEACON_TIMEOUT, /**< station beacon timeout */

    WM_EVENT_WIFI_AP_START,           /**< softap start */
    WM_EVENT_WIFI_AP_STOP,            /**< softap stop */
    WM_EVENT_WIFI_AP_STACONNECTED,    /**< a station connected to softap */
    WM_EVENT_WIFI_AP_STADISCONNECTED, /**< a station disconnected from softap */

    WM_EVENT_WIFI_ONESHOT_SCAN_DONE,       /*!< Station oneshot has finished to scan for APs */
    WM_EVENT_WIFI_ONESHOT_FOUND_CHANNEL,   /*!< Station oneshot has found the channel of the target AP */
    WM_EVENT_WIFI_ONESHOT_GOT_SSID_PSWD,   /*!< Station oneshot got the SSID and password */
    WM_EVENT_WIFI_ONESHOT_GOT_CUSTOM_DATA, /*!< Station oneshot got the custom data */
    WM_EVENT_WIFI_ONESHOT_SEND_REPLY_DONE, /*!< Station oneshot has sent ACK to cellphone */
};

/** @brief Structure describing parameters for wifi event data */
typedef union {
    struct {
        uint32_t status; /**< status of scanning APs: 0 - success, 1 - failure */
        uint8_t number;  /**< number of scan results */
    } scan_done_info;

    struct {
        uint8_t ssid[32];             /**< SSID of connected AP */
        uint8_t ssid_len;             /**< SSID length of connected AP */
        uint8_t bssid[6];             /**< BSSID of connected AP*/
        uint8_t channel;              /**< channel of connected AP*/
        wm_wifi_auth_mode_t authmode; /**< authentication mode used by AP */
    } sta_connected_info;

    struct {
        uint8_t ssid[32];             /**< SSID of disconnected AP */
        uint8_t ssid_len;             /**< SSID length of disconnected AP */
        uint8_t bssid[6];             /**< BSSID of disconnected AP */
        wm_wifi_reason_code_t reason; /**< reason of disconnection */
    } sta_disconnected_info;

    struct {
        int rssi;
    } sta_bss_rssi_low_info;

    struct {
        uint8_t addr[6];
        uint8_t aid;
    } ap_staconnected_info;

    struct {
        uint8_t addr[6];
        uint8_t aid;
    } ap_stadisconnected_info;

    /** Argument structure for WM_EVENT_WIFI_ONESHOT_GOT_SSID_PSWD event */
    struct {
        uint8_t ssid[32];     /**< SSID of the AP. Null terminated string. */
        uint8_t password[64]; /**< Password of the AP. Null terminated string. */
        bool bssid_set;       /**< whether set MAC address of target AP or not. */
        uint8_t bssid[6];     /**< MAC address of target AP. */
    } wifi_oneshot_got_ssid_pswd_info;

    /** Argument structure for WM_EVENT_WIFI_ONESHOT_GOT_CUSTOM_DATA event */
    struct {
        uint8_t custom_data[64]; /**< custom data. Null terminated string. */
    } wifi_oneshot_got_custom_data_info;

} wm_wifi_event_data_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_WIFI_TYPES_H__ */
