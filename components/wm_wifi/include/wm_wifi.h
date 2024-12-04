#ifndef __WM_WIFI_H__
#define __WM_WIFI_H__

#include "wm_wifi_types.h"
#include "wm_wifi_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_WIFI_APIs WM WiFi APIs
 * @brief WinnerMicro WiFi APIs
 */

/**
 * @addtogroup WM_WIFI_APIs
 * @{
 */

/**
  * @brief  Initialize WiFi
  *         Allocate resource for WiFi. This WiFi also starts WiFi task
  *
  * @attention This API must be called before all other WiFi API can be called
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_init(void);

/**
  * @brief  Deinit WiFi
  *         Free all resource allocated in wm_wifi_init and stop WiFi task
  *
  * @attention This API should be called if you want to remove WiFi driver from the system
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_deinit(void);

/**
  * @brief     Set the configuration of STA or AP
  *
  * @attention 1. For station configuration, bssid_set needs to be 0;
  *               and it needs to be 1 only when users need to check the MAC address of the AP.
  * @attention 2. Limited to only one channel, so when in the softap+station mode,
  *               the softap will adjust its channel automatically to be the same as
  *               the channel of the station.
  *
  * @param     interface  interface
  * @param     conf  station or softap configuration
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_config(wm_wifi_interface_t interface, wm_wifi_config_t *conf);

/**
  * @brief     Get configuration of specified interface
  *
  * @param     interface  interface
  * @param[out]  conf  station or softap configuration
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_config(wm_wifi_interface_t interface, wm_wifi_config_t *conf);

/**
  * @brief     Set protocol type of specified interface
  *            The default protocol is (WM_WIFI_PROTOCOL_11B|WM_WIFI_PROTOCOL_11G|WM_WIFI_PROTOCOL_11N)
  *
  * @attention Currently only support 802.11b or 802.11bg or 802.11bgn mode
  *
  * @param     ifx  interfaces
  * @param     protocol  WiFi protocol bitmap
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_protocol(wm_wifi_interface_t ifx, wm_wifi_protocol_type_t protocol);

/**
  * @brief     Get the current protocol bitmap of the specified interface
  *
  * @param     ifx  interface
  * @param[out] protocol  store current WiFi protocol bitmap of interface ifx
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_protocol(wm_wifi_interface_t ifx, wm_wifi_protocol_type_t *protocol);

/**
  * @brief     Set the bandwidth of specified interface
  *
  * @attention WM_WIFI_BW_HT40 is supported only when the interface support 11N
  *
  * @param     ifx  interface to be configured
  * @param     bw  bandwidth
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_bandwidth(wm_wifi_interface_t ifx, wm_wifi_bandwidth_t bw);

/**
  * @brief     Get the bandwidth of specified interface
  *
  * @param     ifx interface to be configured
  * @param[out] bw  store bandwidth of interface ifx
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_bandwidth(wm_wifi_interface_t ifx, wm_wifi_bandwidth_t *bw);

/**
  * @brief     Set the WiFi API configuration storage type
  *
  * @attention 1. The default value is WM_WIFI_STORAGE_FLASH
  *
  * @param     storage : storage type
  *
  * @return
  *   - WM_ERR_SUCCESS: succeed
  *   - others: failed
  */
int wm_wifi_set_storage(wm_wifi_storage_t storage);

/**
 * @brief  Restore WiFi stack persistent settings to default values
 *
 * This function will reset settings made using the following APIs:
 * - wm_wifi_set_bandwidth,
 * - wm_wifi_set_protocol,
 * - wm_wifi_set_config related
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_wifi_restore(void);

/**
  * @brief     Connect WiFi station to the AP.
  *
  * @attention 1. If connected to an AP, call wm_wifi_disconnect to disconnect.
  * @attention 2. It will reconnecting after received disconnect event.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_connect(void);

/**
  * @brief     Disconnect WiFi station from the AP.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_disconnect(void);

/**
  * @brief     Get information of AP which the station is associated with
  *
  * @attention When the obtained country information is empty, it means that the AP does not carry country information
  *
  * @param     ap_info  the wm_wifi_ap_record_t to hold AP information
  *            sta can get the connected ap's phy mode info through the struct member
  *            phy_11n, phy_11ax in the wm_wifi_ap_record_t struct.
  *            For example, phy_11n = 1 imply that ap support 802.11n mode
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_sta_get_ap_info(wm_wifi_ap_record_t *ap_info);

/**
  * @brief     Scan all available APs.
  *
  * @param     config  configuration of scanning
  * @param     block if block is true, this API will block the caller until the scan is done, otherwise
  *                         it will return immediately
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_scan_start(const wm_wifi_scan_config_t *config, bool block);

/**
  * @brief     Stop the scan in process
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_scan_stop(void);

/**
  * @brief     Get number of APs found in last scan
  *
  * @param[out] number  store number of APIs found in last scan
  *
  * @attention This API can only be called when the scan is completed, otherwise it may get wrong value.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_scan_get_ap_num(uint16_t *number);

/**
  * @brief     Get AP list found in last scan
  *
  * @param[inout]  number As input param, it stores max AP number ap_records can hold.
  *                As output param, it receives the actual AP number this API returns.
  * @param         ap_records  wifi_ap_record_t array to hold the found APs
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_scan_get_ap_records(uint16_t *number, wm_wifi_ap_record_t *ap_records);

/**
  * @brief  Create softap control block and start softap
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_ap_start(void);

/**
  * @brief  Stop softap and free softap control block
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_ap_stop(void);

/**
  * @brief     Get STAs associated with softap
  *
  * @attention SSC only API
  *
  * @param[out] sta  station list
  *             ap can get the connected sta's phy mode info through the struct member
  *             phy_11n, phy_11ax in the wifi_sta_info_t struct.
  *             For example, phy_11n = 1 imply that sta support 802.11n mode
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_ap_get_sta_list(wm_wifi_sta_list_t *sta);

/**
  * @brief     Get AID of STA connected with softap
  *
  * @param     mac  STA's mac address
  * @param[out]  aid  Store the AID corresponding to STA mac
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid);

/**
  * @brief     deauthenticate all stations or associated id equals to aid
  *
  * @param     aid  when aid is 0, deauthenticate all stations, otherwise deauthenticate station whose associated id is aid
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_deauth_sta(uint16_t aid);

/**
  * @brief     deauthenticate all stations or mac address equals to mac
  *
  * @param     mac  when mac all is 0xFF, deauthenticate all stations, otherwise deauthenticate station whose mac address is mac
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_deauth_sta_by_mac(uint8_t mac[6]);

/**
  * @brief     Set current WiFi power save type
  *
  * @attention Default power save type is WM_WIFI_PS_MIN_MODEM.
  *
  * @param     type  power save type
  *
  * @return    WM_ERR_SUCCESS: succeed
  */
int wm_wifi_set_ps(wm_wifi_ps_type_t type);

/**
  * @brief     Get current WiFi power save type
  *
  * @attention Default power save type is WM_WIFI_PS_MIN_MODEM.
  *
  * @param[out]  type: store current power save type
  *
  * @return    WM_ERR_SUCCESS: succeed
  */
int wm_wifi_get_ps(wm_wifi_ps_type_t *type);

/**
  * @brief     Set primary/secondary channel
  *
  * @attention 1. When in station mode, this API should not be called when STA is scanning or connecting to an external AP
  * @attention 2. When in softap mode, this API should not be called when softap has connected to external STAs
  * @attention 3. When in station+softap mode, this API should not be called when in the scenarios described above
  *
  * @param     primary  for HT20, primary is the channel number, for HT40, primary is the primary channel
  * @param     second   for HT20, second is ignored, for HT40, second is the second channel
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_channel(uint8_t primary, wm_wifi_second_chan_t second);

/**
  * @brief     Get the primary/secondary channel
  *
  * @param     primary   store current primary channel
  * @param[out]  second  store current second channel
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_channel(uint8_t *primary, wm_wifi_second_chan_t *second);

/**
  * @brief     Set MAC address of the WiFi station or the softap interface.
  *
  * @attention 1. softap and station have different MAC addresses, do not set them to be the same.
  * @attention 2. The bit 0 of the first byte of MAC address can not be 1. For example, the MAC address
  *               can set to be "1a:XX:XX:XX:XX:XX", but can not be "15:XX:XX:XX:XX:XX".
  * @attention 3. Existing mac addresses in tcip/ip are not modified.
  *
  * @param     ifx  interface
  * @param     mac  the MAC address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_mac(wm_wifi_interface_t ifx, const uint8_t mac[6]);

/**
  * @brief     Get mac of specified interface
  *
  * @param      ifx  interface
  * @param[out] mac  store mac of the interface ifx
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_mac(wm_wifi_interface_t ifx, uint8_t mac[6]);

/**
  * @brief Register the RX callback function in the promiscuous mode.
  *
  * Each time a packet is received, the registered callback function will be called.
  *
  * @param cb  callback
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_set_promiscuous_rx_cb(wm_wifi_promiscuous_cb_t cb);

/**
  * @brief     Enable the promiscuous mode.
  *
  * @param     en  false - disable, true - enable
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_wifi_set_promiscuous(bool en);

/**
  * @brief     Get the promiscuous mode.
  *
  * @param[out] en  store the current status of promiscuous mode
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_promiscuous(bool *en);

/**
  * @brief Enable the promiscuous mode packet type filter.
  *
  * @param filter the packet type filtered in promiscuous mode.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note The default filter is to filter all packets except WM_WIFI_PKT_MISC
  */
int wm_wifi_set_promiscuous_filter(const wm_wifi_promiscuous_filter_t *filter);

/**
  * @brief     Get the promiscuous filter.
  *
  * @param[out] filter  store the current status of promiscuous filter
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_promiscuous_filter(wm_wifi_promiscuous_filter_t *filter);

/**
  * @brief Enable subtype filter of the control packet in promiscuous mode.
  *
  * @param filter the subtype of the control packet filtered in promiscuous mode.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note The default filter is to filter none control packet.
  */
int wm_wifi_set_promiscuous_ctrl_filter(const wm_wifi_promiscuous_filter_t *filter);

/**
  * @brief     Get the subtype filter of the control packet in promiscuous mode.
  *
  * @param[out] filter  store the current status of subtype filter of the control packet in promiscuous mode
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_promiscuous_ctrl_filter(wm_wifi_promiscuous_filter_t *filter);

/**
  * @brief     Send raw ieee80211 data
  *
  * @param     ifx interface if the Wi-Fi mode is station, the ifx should be WIFI_IF_STA. If the Wi-Fi
  *            mode is softap, the ifx should be WIFI_IF_AP. If the Wi-Fi mode is station+softap, the
  *            ifx should be WIFI_IF_STA or WIFI_IF_AP.
  * @param     buffer raw ieee80211 buffer
  * @param     len the length of raw buffer, the len must be <= 1500 Bytes and >= 24 Bytes
  * @param     en_sys_seq indicate whether use the internal sequence number. If en_sys_seq is false, the
  *            sequence in raw buffer is unchanged, otherwise it will be overwritten by WiFi driver with
  *            the system sequence number.
  *            Generally, if wm_wifi_80211_tx is called before the Wi-Fi connection has been set up, both
  *            en_sys_seq==true and en_sys_seq==false are fine. However, if the API is called after the Wi-Fi
  *            connection has been set up, en_sys_seq must be true.
  *
  * @return
  *    - WM_ERR_SUCCESS: success
  *    - others: failed
  */
int wm_wifi_80211_tx(wm_wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

/**
  * @brief      Config 80211 tx rate of specified interface
  *
  * @param      ifx  Interface to be configured.
  * @param      rate Phy rate to be configured.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_config_80211_tx_rate(wm_wifi_interface_t ifx, wm_wifi_phy_rate_t rate);

/**
  * @brief     Set the inactive time of STA or AP
  *
  * @attention 1. For Station, If the station does not receive a beacon frame from the connected softap during the inactive time,
  *               disconnect from softap. Default 6s.
  * @attention 2. For softap, If the softap doesn't receive any data from the connected STA during inactive time,
  *               the softap will force deauth the STA. Default is 300s.
  * @attention 3. The inactive time configuration is not stored into flash
  *
  * @param     ifx  interface to be configured.
  * @param     sec  Inactive time. Unit seconds.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_inactive_time(wm_wifi_interface_t ifx, uint16_t sec);

/**
  * @brief     Get inactive time of specified interface
  *
  * @param     ifx  Interface to be configured.
  * @param     sec  Inactive time. Unit seconds.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_inactive_time(wm_wifi_interface_t ifx, uint16_t *sec);

/**
  * @brief      Set RSSI threshold below which APP will get an event
  *
  * @attention  This API needs to be called every time after WM_EVENT_WIFI_STA_BSS_RSSI_LOW event is received.
  *
  * @param      rssi threshold value in dbm between -100 to 0
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_rssi_threshold(int32_t rssi);

/**
  * @brief     configure country
  *
  * @attention 1. The country configuration is stored into flash.
  * @attention 2. Supported country codes are "CN","JP","US","CA",
  *               "AL","AM","AT","AZ","BA","BE","BG","BY","CH","CY","CZ","DE",
  *               "DK","EE","EL","ES","FI","FR","GE","HR","HU","IE","IS","IT",
  *               "LI","LT","LU","LV","MD","ME","MK","MT","NL","NO","PL","PT",
  *               "RO","RS","RU","SE","SI","SK","TR","UA","UK"
  * @attention 3. The default country is "CN".
  *
  * @param     country   the configured country ISO code
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_set_country_code(const char country[3]);

/**
  * @brief     get the current country code
  *
  * @param     country  country code
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_wifi_get_country_code(char country[3]);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_WIFI_H__ */
