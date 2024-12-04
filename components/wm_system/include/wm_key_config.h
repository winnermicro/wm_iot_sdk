#ifndef __WM_KEY_CONFIG_H__
#define __WM_KEY_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WM_GROUP_WIFI          "wifi"    /**< nvs store group for wifi module     */
#define WM_GROUP_BLE           "ble"     /**< nvs store group for ble module      */
#define WM_GROUP_NETWORK       "network" /**< nvs store group for network modules */
#define WM_GROUP_USER          "user"    /**< nvs store group for user examples   */

#define WM_KEY_STA_MAC_ADDR    "wm_sta_mac"
#define WM_KEY_SAP_MAC_ADDR    "wm_sap_mac"
#define WM_KEY_BT_MAC_ADDR     "wm_bt_mac"

#define WM_KEY_WIFI_CONFIG     "wm_cfg_wifi"
#define WM_KEY_WPAS_PKMINFO    "wpas_pmk_info"

#define WM_KEY_BLE_CONFIG      "wm_cfg_ble"

#define WM_KEY_AT_WIFI_CONFIG  "wm_cfg_at_wifi"

#define WM_KEY_AT_TCPIP_CONFIG "wm_cfg_at_tcpip"

#ifdef __cplusplus
}
#endif

#endif /* __WM_KEY_CONFIG_H__ */
