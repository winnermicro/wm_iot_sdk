/**
 * @file wm_nm_def.h
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
#ifndef __WM_NM_DEF_H__
#define __WM_NM_DEF_H__
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_ip_addr.h"
#include "wm_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_LWIP_ENABLE_IPV4
#define WM_NM_IPV4 1
#endif

#if CONFIG_LWIP_ENABLE_IPV6
#define WM_NM_IPV6 1
#endif
/**
 * @defgroup WM_NM_Macros WM Net Manager Macros
 * @brief WinnerMicro Net Manager Macros
 */

/**
 * @addtogroup WM_NM_Macros
 * @{
 */

#define WIFI_STATION_NETIF_NAME "w_sta" /**< Name of station netif. */
#define WIFI_SOFTAP_NETIF_NAME  "w_ap"  /**< Name of softAP netif. */
#define WM_NM_INVALID_NETIF     (-1)    /**< Invalid net manager netif handle. */

/**
 * @}
 */

typedef wm_netif_type_t wm_nm_netif_t; /**< Net manager netif handle. */

/**
 * @defgroup WM_NM_Enumerations WM Net manager Enumerations
 * @brief WinnerMicro Net manager Enumerations
 */

/**
 * @addtogroup WM_NM_Enumerations
 * @{
 */

/**
 * @brief Flag of net manager's netif
 */
typedef enum {
    NM_NETIF_FLAG_UP        = 0x01U, /** If set, the netif has broadcast capability. */
    NM_NETIF_FLAG_BROADCAST = 0x02U, /** If set, the interface has an active link */
    NM_NETIF_FLAG_LINK_UP   = 0x04U, /** If set, the netif is an ethernet device using ARP */
    NM_NETIF_FLAG_ETHARP    = 0x08U, /** If set, the netif is an ethernet device using ARP. */
    NM_NETIF_FLAG_ETHERNET  = 0x10U, /** If set, the netif is an ethernet device. */
    NM_NETIF_FLAG_IGMP      = 0x20U, /** If set, the netif has IGMP capability. */
    NM_NETIF_FLAG_MLD6      = 0x40U, /** If set, the netif has MLD6 capability. */
} wm_nm_netif_flag_e;

/**
 * @brief Type of DNS server
 */
typedef enum {
    WM_NM_DNS_MAIN = 0, /**< DNS main server address. */
    WM_NM_DNS_BACKUP,   /**< DNS backup server address. */
} wm_nm_dns_type_t;

/**
 * @brief State of net manager
 */
typedef enum {
    WM_NM_INIT,                   /**< net manager state: initializing. */
    WM_NM_WIFI_STA_CONNECTING,    /**< net manager state: connecting to AP. only for Wi-Fi station. */
    WM_NM_WIFI_STA_CONNECTED,     /**< net manager state: connected to AP. only for Wi-Fi station. */
    WM_NM_WIFI_STA_DISCONNECTED,  /**< net manager state: disconnect from AP. only for Wi-Fi station. */
    WM_NM_WIFI_STA_GOT_IP,        /**< net manager state: got IPv4. only for Wi-Fi station. */
    WM_NM_WIFI_STA_GOT_IP6,       /**< net manager state: got global IPv6. only for Wi-Fi station. */
    WM_NM_WIFI_STA_LOST_IP,       /**< net manager state: lost IPv4. only for Wi-Fi station. */
    WM_NM_WIFI_STA_LOST_IP6,      /**< net manager state: lost global IPv6. only for Wi-Fi station. */
    WM_NM_WIFI_AP_READY,          /**< net manager state: softAP is ready. only for Wi-Fi softap. */
    WM_NM_WIFI_AP_ASSIGNED_IP_ID, /**< net manager state: softAP assigns IP to station. only for Wi-Fi softap. */
    WM_NM_EXIT,                   /**< net manager state: exit, cannot be used. */
} wm_nm_state_e;

/**
 * @}
 */

/**
 * @defgroup WM_NM_Structures WM Net manager Structures
 * @brief WinnerMicro Net manager Structures
 */

/**
 * @addtogroup WM_NM_Structures
 * @{
 */

/**
 * @brief struct of ip info
 */
typedef struct {
    wm_ip_addr_t ip;      /**< Interface IPV4 address. */
    wm_ip_addr_t netmask; /**< Interface IPV4 netmask. */
    wm_ip_addr_t gw;      /**< Interface IPV4 gateway address. */
} wm_netif_ip_info_t;

/**
 * @brief struct of dhcp server option
 */
typedef struct {
    wm_ip_addr_t server_ip; /**< DHCP server ip. */
    wm_ip_addr_t start_ip;  /**< DHCP server assign IP starting address. */
    wm_ip_addr_t ip_mask;   /**< DHCP server assign IP netmask. */
    wm_ip_addr_t gateway;   /**< DHCP server assign gateway. */
    wm_ip_addr_t dns1;      /**< DHCP server assign DNS server IP address1. */
    wm_ip_addr_t dns2;      /**< DHCP server assign DNS server IP address2. */
    uint32_t lease_time;    /**< DHCP server assign IP lease time(seconds). */
} wm_nm_dhcps_option_t;

/**
 * @brief struct of Wi-Fi station config
 */
typedef struct {
    const char *ssid;     /**< SSID of target AP. */
    const char *password; /**< Password of target AP. (if authenticate mode is open, password = NULL) */
} wm_nm_wifi_station_cfg_t;

/**
 * @brief struct of Wi-Fi softap config
 */
typedef struct {
    const char *ssid;     /**< SSID of soft-AP. */
    const char *password; /**< Password of soft-AP. (if authenticate mode is open, password = NULL) */
} wm_nm_wifi_softap_cfg_t;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //__WM_NM_DEF_H__
