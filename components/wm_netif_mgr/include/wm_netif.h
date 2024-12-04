#ifndef __WM_NETIF_H__
#define __WM_NETIF_H__

#include "wmsdk_config.h"
#include "wm_ip_addr.h"
#include "lwipopts.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_NETIF_MTU 1500

typedef enum {
    WM_NETIF_TYPE_WIFI_STA, /**< wifi station interface */
    WM_NETIF_TYPE_WIFI_AP,  /**< wifi soft-AP interface */

#if CONFIG_WM_NETIF_ENABLE_ETH
    WM_NETIF_TYPE_ETH, /**< ethernet interface */
#endif

    WM_NETIF_TYPE_MAX,
} wm_netif_type_t;

typedef struct {
    wm_netif_type_t type; /**< netif type @wm_netif_type_t */
    void *netif;          /**< wm_netif_t internal pointer */
} wm_netif_t;

/**
 * @brief Start DNS Server
 *
 * @param[in]  interface interface type
 *
 * @return None
 *
 * @note It only works on the softap interface.
 * The configured from the kconfig configuration value CONFIG_LWIP_ENABLE_DNS_SERVER.
 */
void wm_netif_start_dnss(wm_netif_type_t interface);

/**
 * @brief Stop DNS Server
 *
 * @param[in]  interface interface type
 *
 * @return None
 *
 * @note It only works on the softap interface.
 */
void wm_netif_stop_dnss(wm_netif_type_t interface);

/**
 * @brief Start DHCP Server
 *
 * @param[in] interface interface type
 * @param[in] ip IP information to set on the specified interface
 * @param[in] netmask net mask information to set on the specified interface
 * @param[in] gw gate way information to set on the specified interface
 *
 * @return None
 *
 * @note It only works on the softap interface.
 * The api will set the interface to the up,
 * then configure the interface address, and finally start the dhcp server
 */
void wm_netif_start_dhcps(wm_netif_type_t interface, wm_ip_addr_t *ipaddr, wm_ip_addr_t *netmask, wm_ip_addr_t *gw);

/**
 * @brief Stop DHCP Server
 *
 * @param[in]  interface interface type
 *
 * @return None
 *
 * @note It only works on the softap interface.
 * The api first stops the dhcp server and finally sets the interface to the down
 */
void wm_netif_stop_dhcps(wm_netif_type_t interface);

/**
 * @brief  Set DNS Server information
 *
 * @param[in]  interface interface type
 * @param[in]  index index of DNS Server to set: 0, 1
 * @param[in]  ip  DNS Server address to set
 *
 * @return     None
 */
void wm_netif_set_dns(wm_netif_type_t interface, uint8_t index, wm_ip_addr_t *ip);

/**
 * @brief  Get DNS Server information
 *
 * @param[in]  interface interface type
 * @param[in]  index index of DNS Server to set: 0, 1
 * @param[out] ip  DNS Server address to get
 *
 * @return     None
 */
void wm_netif_get_dns(wm_netif_type_t interface, uint8_t index, wm_ip_addr_t *ip);

/**
 * @brief  Set interface's IP address information
 *
 * This function is mainly used to set a static IP on an interface.
 *
 * @param[in] interface interface type
 * @param[in] ip IP information to set on the specified interface
 * @param[in] netmask net mask information to set on the specified interface
 * @param[in] gw gate way information to set on the specified interface
 *
 * @return    None
 *
 * @note DHCP client/server must be stopped (if enabled for this interface) before setting new IP information.
 *
 * @note Calling this interface for may generate a WM_EVENT_WIFI_STA_GOT_IP or WM_EVENT_WIFI_STA_GOT_IP6 event.
 */
void wm_netif_set_ipaddr(wm_netif_type_t interface, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw);

/**
 * @brief  Get interface's IP address information
 *
 * @param[in]  interface interface type
 * @param[out] ip IP information to get on the specified interface
 * @param[out] netmask net mask information to get on the specified interface
 * @param[out] gw gate way information to get on the specified interface
 *
 * @return     None
 *
 * @note that valid addresses can only be obtained after the network connection.
 */
void wm_netif_get_ipaddr(wm_netif_type_t interface, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw);

/**
 * @brief  Set interface maximum transmission unit.
 *
 * @param[in]  interface interface type
 * @param[out] ip6 locallink IPv6 information to get on the specified interface
 *
 * @return
 *         - WM_ERR_FAILED failed
 *         - WM_ERR_SUCCESS on success
 */
int wm_netif_get_ll_ip6(wm_netif_type_t interface, wm_ip_addr_t *ip6);

/**
 * @brief Start DHCP client
 *
 * @param[in]  interface interface type
 *
 * @return None
 *
 * @note It only works on the station interface.
 */
void wm_netif_start_dhcpc(wm_netif_type_t interface);

/**
 * @brief Stop DHCP client
 *
 * @param[in]  interface interface type
 *
 * @return None
 *
 * @note It only works on the station interface.
 */
void wm_netif_stop_dhcpc(wm_netif_type_t interface);

/**
 * @brief  Set interface maximum transmission unit.
 *
 * @param[in]  interface interface type
 * @param[in]  mtu
 *
 * @return
 *         - WM_ERR_FAILED failed
 *         - WM_ERR_SUCCESS on success
 */
int wm_netif_set_mtu(wm_netif_type_t interface, uint16_t mtu);

/**
 * @brief  Get interface maximum transmission unit.
 *
 * @param[in]  interface interface type
 *
 * @return
 *         - 0 failed
 *         - other mtu
 */
uint16_t wm_netif_get_mtu(wm_netif_type_t interface);

/**
 * @brief  Set the hostname of an interface
 *
 * @param[in]  interface interface type
 * @param[in]  hostname New hostname for the interface. Maximum length 32 bytes.
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other failed
 *
 * @note that the hostname can only be set after the network connection.
 * This setting will loss when the interface is down.
 */
int wm_netif_set_hostname(wm_netif_type_t interface, const char *hostname);

/**
 * @brief  Get interface hostname.
 *
 * @param[in]  interface interface type
 * @param[out] hostname Returns a pointer to the hostname. May be NULL if no hostname is set. If set non-NULL, pointer remains valid (and string may change if the hostname changes).
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other failed
 */
int wm_netif_get_hostname(wm_netif_type_t interface, const char *hostname);

/**
 * @brief  Get net interface index from network stack implementation
 *
 * @param[in]  wm_netif_t Handle to wm netif instance
 *
 * @return implementation specific index of interface represented with supplied wm netif
 *
 * @note This index could be used in `setsockopt()` to bind socket with multicast interface
 */
int wm_netif_get_netif_impl_index(wm_netif_t *netif);

/**
 * @brief  Get wm netif interface
 *
 * @param[in]  interface interface type
 *
 * @return     wm netif instance
 */
wm_netif_t *wm_netif_get_netif(wm_netif_type_t interface);

/**
 * @brief  create ipv6 linklocal address
 *
 * @param[in]  interface interface type
 * @param[in]  enable : 1 enable , 0 disable
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other failed
 */
int wm_netif_ip6_enable(wm_netif_type_t interface, uint8_t enable);

/**
 * @brief  add interface for the underlying TCP/IP stack
 *
 * @param[in]  interface interface type
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other failed
 */
int wm_netif_addif(wm_netif_type_t interface);

/**
 * @brief  delete interface for the underlying TCP/IP stack
 *
 * @param[in]  interface interface type
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other failed
 */
int wm_netif_delif(wm_netif_type_t interface);

/**
 * @brief  Resources are reclaimed when a task is destroyed for the underlying TCP/IP stack
 *
 * @param[in]  tcb Task block instance
 *
 * @return None
 */
void wm_netif_cleanup_tcb(void *tcb);

/**
 * @brief  Initialize the underlying TCP/IP stack
 *
 * @param[in]  sync_sem Semaphore used for synchronization, it can be NULL if not needed
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other if initializing failed
 *
 * @note This function should be called exactly once from application code, when the application starts up.
 */
int wm_netif_init(void *sync_sem);

/**
 * @brief  Deinitialize the wm netif component (and the underlying TCP/IP stack)
 *
 * @param[in]  sync_sem Semaphore used for synchronization, it can be NULL if not needed
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other if deinitialization failed
 */
int wm_netif_deinit(void *sync_sem);

#ifdef __cplusplus
}
#endif

#endif //__WM_NETIF_H__
