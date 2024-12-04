/**
 * @file wm_nm_core.h
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
#ifndef __WM_NM_CORE_H__
#define __WM_NM_CORE_H__
#include "wm_osal.h"
#include "wm_nm_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef wm_netif_t wm_nm_internal_netif_t;

typedef struct {
    bool is_static_ip;          /**< if set, user uses static IP*/
    wm_netif_ip_info_t ip_info; /**< static IP for netif*/
    bool ip6_disable;           /**< if set, disable IP6 traffic */
} wm_nm_netif_param_t;

typedef struct {
    wm_nm_internal_netif_t *netif;               /**< root node*/
    wm_os_mutex_t *mutex;                        /**< mutex for net manager*/
    wm_nm_netif_param_t info[WM_NETIF_TYPE_MAX]; /**< netif parameter */
    bool dhcps_disabled;                         /**< DHCP server is disabled? */
    wm_nm_dhcps_option_t dhcps_option;           /**< DHCP server option */
    bool dnss_disabled;                          /**< DNS server is disabled? */
} wm_nm_param_t;

/**
  * @brief  get net manager static control parameters
  *
  * @return
  *    - NULL: failed
  *    - others: succeed
  */
wm_nm_param_t *wm_nm_get_core_param(void);

/**
  * @brief  query wm_nm_internal_netif_t through netif
  *
  * @return
  *    - NULL: failed
  *    - others: succeed
  */
wm_nm_internal_netif_t *wm_nm_query_internal_netif(wm_nm_netif_t netif);

/**
  * @brief  lock net manager mutex
  *
  * @return
  */
void wm_nm_mutex_lock(void);

/**
  * @brief  unlock net manager mutex
  *
  * @return
  */
void wm_nm_mutex_unlock(void);

#ifdef __cplusplus
}
#endif

#endif //__WM_NM_CORE_H__
