#ifndef __WM_NETIF_LWIP_H__
#define __WM_NETIF_LWIP_H__

void wm_netif_cleanup_tcb_lwip(void *tcb);

void wm_netif_init_lwip(void *sync_sem);
void wm_netif_deinit_lwip(void *sync_sem);

int wm_netif_addif_lwip(wm_netif_t *netif, const uint8_t *mac_addr, const uint8_t if_name[2], const char *hostname);
int wm_netif_delif_lwip(wm_netif_t *netif);

int wm_netif_get_netif_impl_index_lwip(wm_netif_t *netif);

void wm_netif_set_input_lwip(wm_netif_t *netif, bool enable);

void wm_netif_start_dhcpc_lwip(wm_netif_t *netif);
void wm_netif_stop_dhcpc_lwip(wm_netif_t *netif);

void wm_netif_set_ipaddr_lwip(wm_netif_t *netif, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw);
void wm_netif_get_ipaddr_lwip(wm_netif_t *netif, wm_ip_addr_t *ip, wm_ip_addr_t *netmask, wm_ip_addr_t *gw);

int wm_netif_get_ll_ip6_lwip(wm_netif_t *netif, wm_ip_addr_t *ip6);

void wm_netif_set_dns_lwip(uint8_t index, wm_ip_addr_t *ip);
void wm_netif_get_dns_lwip(uint8_t index, wm_ip_addr_t *ip);

int wm_netif_set_hostname_lwip(wm_netif_t *netif, const char *hostname);
int wm_netif_get_hostname_lwip(wm_netif_t *netif, const char *hostname);

int16_t wm_netif_ip6_enable_lwip(wm_netif_t *netif, uint8_t enable);

int16_t wm_netif_set_mtu_lwip(wm_netif_t *netif, uint16_t mtu);
uint16_t wm_netif_get_mtu_lwip(wm_netif_t *netif);
void wm_netif_set_mac_addr_lwip(wm_netif_t *netif, uint8_t mac_addr[6]);

void wm_netif_start_dhcps_lwip(wm_netif_t *netif, wm_ip_addr_t *ipaddr, wm_ip_addr_t *netmask, wm_ip_addr_t *gw);
void wm_netif_stop_dhcps_lwip(wm_netif_t *netif);

void wm_netif_start_dnss_lwip(wm_netif_t *netif);
void wm_netif_stop_dnss_lwip(wm_netif_t *netif);

#endif //__WM_NETIF_LWIP_H__
