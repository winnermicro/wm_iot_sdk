/**
 * @file wm_nm_api.h
 *
 * @brief WM net manager Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef __WM_NM_API_H__
#define __WM_NM_API_H__
#include "wm_nm_def.h"
#include "wm_event.h"
#ifdef __cplusplus
extern "C" {
#endif

WM_EVENT_DECLARE_GROUP(WM_NM_EV); /**< Declare net manager event group. */

/**
 * @defgroup WM_NM_NETIF_APIs WM net manager APIs of netif
 * @brief WinnerMicro Net Manager APIs
 */

/**
 * @addtogroup WM_NM_NETIF_APIs
 * @{
 */

/**
  * @brief  Query available netif
  *
  * @return
  *     - WM_NM_INVALID_NETIF: No available netif found
  *     - others: Available netif
  */
wm_nm_netif_t wm_nm_get_netif(void);

/**
  * @brief  Query number of available netif
  *
  * @return
  *     - 0: No available netif
  *     - others: number of available netif
  */
int wm_nm_get_netif_number(void);

/**
  * @brief  Query next available netif
  *
  * @param[in] netif: Current netif
  *
  * @return
  *     - WM_NM_INVALID_NETIF: No next netif found
  *     - others: Available netif
  */
wm_nm_netif_t wm_nm_get_next_netif(wm_nm_netif_t netif);

/**
  * @brief  Query netif name from net manager
  *
  * @param[in] netif: Netif
  *
  * @return
  *     - NULL: Invalid netif
  *     - others: Netif name
  */
const char *wm_nm_get_netif_name(wm_nm_netif_t netif);

/**
  * @brief  Query netif by name
  *
  * @param[in] name: Netif name
  *
  * @return
  *     - WM_NM_INVALID_NETIF: No found netif
  *     - others: Available netif
  */
wm_nm_netif_t wm_nm_get_netif_by_name(const char *name);

/**
  * @brief  Query the netif flag
  *
  * @param[in] netif: Netif handle
  * @param[in] flag: flag write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_get_netif_flag(wm_nm_netif_t netif, uint32_t *flag);

/**
  * @brief  Query the netif mtu
  *
  * @param[in] netif: Netif handle
  * @param[in] mtu: mtu write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_get_netif_mtu(wm_nm_netif_t netif, uint32_t *mtu);

/**
  * @brief  Set the MAC address
  *
  * @param[in] netif: Netif handle
  * @param[in] mac: MAC address buffer
  * @param[in] len: MAC address length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_set_netif_mac(wm_nm_netif_t netif, uint8_t *mac, size_t len);

/**
  * @brief  Query the MAC address
  *
  * @param[in] netif: Netif handle
  * @param[in] mac: MAC address write back buffer
  * @param[in] max_len: write back buffer maximum length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_get_netif_mac(wm_nm_netif_t netif, uint8_t *mac, size_t *max_len);

/**
  * @brief  Set the IPv4 address
  *
  * @param[in] netif: Netif handle
  * @param[in] ip_info: include IPv4 address, net mask, gateway
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note Set the IPv4 address, it will be stop DHCP client
  *
  */
int wm_nm_set_netif_ip_info(wm_nm_netif_t netif, wm_netif_ip_info_t *ip_info);

/**
  * @brief  Query the IPv4 address
  *
  * @param[in] netif: Netif handle
  * @param[in] ip_info: Net info write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note After setting the IP, DHCP server/client will be stopped
  *
  */
int wm_nm_get_netif_ip_info(wm_nm_netif_t netif, wm_netif_ip_info_t *ip_info);

/**
  * @brief  Enable IPv6 function
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_enable_netif_ip6(wm_nm_netif_t netif);

/**
  * @brief  disable IPv6 function
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_disable_netif_ip6(wm_nm_netif_t netif);

/**
  * @brief  Query the IPv6 enable or disable
  *
  * @param[in] netif: Netif handle
  * @return
  *    - FALSE: invalid argument or IPv6 is not enabled
  *    - TRUE: IPv6 is enabled
  */
bool wm_nm_get_netif_ip6_state(wm_nm_netif_t netif);

/**
  * @brief  Query the IPv6 address
  *
  * @param[in] netif: Netif handle
  * @param[in] ip6: IPv6 linklocal address write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_get_netif_ip6_linklocal(wm_nm_netif_t netif, wm_ip6_addr_t *ip6);

/**
  * @brief  Query the IPv6 address
  *
  * @param[in] netif: Netif handle
  * @param[in] ip6: IPv6 global address write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_get_netif_ip6_global(wm_nm_netif_t netif, wm_ip6_addr_t *ip6);

/**
  * @brief  Set DHCP server options
  *
  * @param[in] netif: Netif handle
  * @param[in] dhcps_option: reference wm_nm_dhcps_option_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note After DHCP server start, The original IP info will be overwritten
  *
  */
int wm_nm_set_netif_dhcps_option(wm_nm_netif_t netif, wm_nm_dhcps_option_t *dhcps_option);

/**
  * @brief  Query DHCP server options
  *
  * @param[in] netif: Netif handle
  * @param[in] dhcps_option: DHCP server options write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_get_netif_dhcps_option(wm_nm_netif_t netif, wm_nm_dhcps_option_t *dhcps_option);

/**
  * @brief  start DHCP server
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_start_netif_dhcps(wm_nm_netif_t netif);

/**
  * @brief  stop DHCP server
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_stop_netif_dhcps(wm_nm_netif_t netif);

/**
  * @brief  start DHCP client
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note if set static IP, start DHCP will fail,
  * @note if you want restart DHCP, must set ip to "0.0.0.0" first.
  *
  */
int wm_nm_start_netif_dhcpc(wm_nm_netif_t netif);

/**
  * @brief  stop DHCP client
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_stop_netif_dhcpc(wm_nm_netif_t netif);

/**
  * @brief  Set DHCP server start
  *
  * @param[in] type: DNS main server or backup server
  * @param[in] dns: DNS server address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note After setting DNS, DHCP server allocated DNS server will be ignored
  *
  */
int wm_nm_set_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns);

/**
  * @brief  Query DHCP server start
  *
  * @param[in] type: DNS main server or backup server
  * @param[in] dns: DNS server address write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_get_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns);

/**
  * @brief start DNS server
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note It only works on the softap mode.
  *
  */
int wm_nm_start_netif_dnss(wm_nm_netif_t netif);

/**
  * @brief stop DNS server
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note It only works on the softap mode.
  *
  */
int wm_nm_stop_netif_dnss(wm_nm_netif_t netif);

/**
 * @}
 */

/**
 * @defgroup WM_NM_WIFI_APIs WM net manager APIs of Wi-Fi
 * @brief WinnerMicro Net Manager APIs
 */

/**
 * @addtogroup WM_NM_WIFI_APIs
 * @{
 */

/**
  * @brief  Start Wi-Fi station mode, It will be init Wi-Fi and connected to the specified AP,
  *         as well as start the DHCP client
  *
  *@attention  This is asynchronous API. Please subscribe to the NM event,
  *             or call wm_nm_get_wifi_station_state to query the connection results
  *
  * @param[in] cfg: reference wm_nm_wifi_station_cfg_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_start_wifi_station(wm_nm_wifi_station_cfg_t *cfg);

/**
  * @brief  Query NM Wi-Fi station state
  *
  * @return
  *    - reference wm_nm_state_e
  */
wm_nm_state_e wm_nm_get_wifi_station_state(void);

/**
  * @brief  Stop Wi-Fi station mode, It will be disconnect from AP.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_nm_stop_wifi_station(void);

/**
  * @brief  Start Wi-Fi softAP, It will be started soft AP and DHCP server
  *
  * @param[in] cfg: reference wm_nm_wifi_softap_cfg_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @note   This is asynchronous API. Please subscribe to the NM event,
  *         or call wm_nm_get_wifi_softap_state to query the softAP state
  *
  */
int wm_nm_start_wifi_softap(wm_nm_wifi_softap_cfg_t *cfg);

/**
  * @brief  Query NM Wi-Fi softAP state
  *
  * @return
  *    - reference wm_nm_state_e
  */
wm_nm_state_e wm_nm_get_wifi_softap_state(void);

/**
  * @brief  Stop Wi-Fi softAP.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_nm_stop_wifi_softap(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_NM_API_H__ */
