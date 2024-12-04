/**
 * @file wm_nm_netstack.h
 *
 * @brief WM NM net network stack Module
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

#ifndef __WM_NM_NETSTACK_H__
#define __WM_NM_NETSTACK_H__
#include "wm_nm_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*add_netif)(wm_nm_netif_t netif); /**< add netif callback. */
typedef void (*del_netif)(wm_nm_netif_t netif); /**< delet netif callback. */

/**
  * @brief  init network stack
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_init(void);

/**
  * @brief  get netif name
  *
  * @param[in] netif: Netif handle
  * @param[in] name_buff: netif name write back address
  * @param[in] max_len: name_buff max length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_get_netif_name(void *netif, char *name_buff, size_t max_len);

/**
  * @brief  get netif flag (example: LINK_UP, LINK_DOWN...)
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - netif_flag: reference wm_nm_netif_flag_e
  */
uint32_t wm_net_stack_get_flag(void *netif);

/**
  * @brief  get netif MTU
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - MTU: Maximum Transmission Unit
  */
uint32_t wm_net_stack_get_mtu(void *netif);

/**
  * @brief  set netif MAC address
  *
  * @param[in] netif: Netif handle
  * @param[in] mac: MAC address buffer
  * @param[in] len: MAC length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_set_mac(void *netif, uint8_t *mac, size_t len);

/**
  * @brief  get netif MAC address
  *
  * @param[in] netif: Netif handle
  * @param[in] mac: mac address write back address
  * @param[in] max_len: mac max length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_get_mac(void *netif, uint8_t *mac, size_t *max_len);

/**
  * @brief  set netif ipv4 address, netmask, and gateway
  *
  * @param[in] netif: Netif handle
  * @param[in] ip: ipv4 address
  * @param[in] mask: ipv4 net mask
  * @param[in] gw: gateway address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_set_ip(void *netif, wm_ip4_addr_t *ip, wm_ip4_addr_t *mask, wm_ip4_addr_t *gw);

/**
  * @brief  get netif ipv4 address, netmask, and gateway
  *
  * @param[in] netif: Netif handle
  * @param[in] ip: ipv4 address write back address
  * @param[in] mask: ipv4 net mask write back address
  * @param[in] gw: gateway address write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_get_ip(void *netif, wm_ip4_addr_t *ip, wm_ip4_addr_t *mask, wm_ip4_addr_t *gw);

/**
  * @brief  get netif ipv6 local link address
  *
  * @param[in] netif: Netif handle
  * @param[in] ip6: ipv6 local link address write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_get_ip6_linklocal(void *netif, wm_ip6_addr_t *ip6);

/**
  * @brief  get netif ipv6 global address
  *
  * @param[in] netif: Netif handle
  * @param[in] ip6: ipv6 global address write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_get_ip6_global(void *netif, wm_ip6_addr_t *ip6);

/**
  * @brief  set DHCP server options
  *
  * @param[in] netif: Netif handle
  * @param[in] dhcps_option: DHCP server options, reference to wm_nm_dhcps_option_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_set_dhcps_option(void *netif, wm_nm_dhcps_option_t *dhcps_option);

/**
  * @brief  get DHCP server options
  *
  * @param[in] netif: Netif handle
  * @param[in] dhcps_option: DHCP server options write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_get_dhcps_option(void *netif, wm_nm_dhcps_option_t *dhcps_option);

/**
  * @brief  get DHCP server is start?
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - false: DHCP server is stopped
  *    - true: DHCP server is started
  */
bool wm_net_stack_dhcps_is_start(void *netif);

/**
  * @brief  set DHCP server start
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_dhcps_start(void *netif);

/**
  * @brief  set DHCP server stop
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_dhcps_stop(void *netif);

/**
  * @brief  set DHCP client start
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_dhcpc_start(void *netif);

/**
  * @brief  set DHCP client stop
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_dhcpc_stop(void *netif);

/**
  * @brief  set DNS server IP address
  *
  * @param[in] type: main or backup DNS server
  * @param[in] dns: DNS server IP address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_set_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns);

/**
  * @brief  get DNS server IP address
  *
  * @param[in] type: main or backup DNS server
  * @param[in] dns: DNS server IP write back address
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_get_dns(wm_nm_dns_type_t type, wm_ip_addr_t *dns);

/**
  * @brief  enable or disable IPv6 function
  *
  * @param[in] netif: Netif handle
  * @param[in] enable: true for enable IPv6, false for disable IPv6
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_set_ip6_state(void *netif, bool enable);

/**
  * @brief  query IPv6 function is enable?
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - true: IPv6 is enable
  *    - false: IPv6 is disable
  */
bool wm_net_stack_get_ip6_state(void *netif);

/**
  * @brief  set DNS server start
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_dnss_start(void *netif);

/**
  * @brief  set DNS server stop
  *
  * @param[in] netif: Netif handle
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_net_stack_dnss_stop(void *netif);

#ifdef __cplusplus
}
#endif

#endif //__WM_NM_NETSTACK_H__
