/*
 * WPA Supplicant - Layer2 packet handling example with stub functions
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file can be used as a starting point for layer2 packet implementation.
 */

#include "includes.h"

#include "common.h"
#include "eloop.h"
#include "l2_packet.h"

#include "wmsdk_config.h"
#include "wm_wifi_drv.h"


struct l2_packet_data {
	char ifname[17];
	u8 own_addr[ETH_ALEN];
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len);
	void *rx_callback_ctx;
	int l2_hdr; /* whether to include layer 2 (Ethernet) header data
		     * buffers */
};


int l2_packet_get_own_addr(struct l2_packet_data *l2, u8 *addr)
{
	os_memcpy(addr, l2->own_addr, ETH_ALEN);
	return 0;
}


int l2_packet_send(struct l2_packet_data *l2, const u8 *dst_addr, u16 proto,
		   const u8 *buf, size_t len)
{
    int ret;

	if (l2 == NULL)
		return -1;

    if (l2->l2_hdr)
    {
	    ret = wm_wifi_drv_send_eapol((uint8_t *)buf, len);
    }
    else
    {
        size_t eapol_len = ETH_HLEN + len;
        u8 *eapol = os_malloc(eapol_len);
        if (!eapol) return -1;
        os_memcpy(eapol, dst_addr, ETH_ALEN);
        os_memcpy(eapol + ETH_ALEN, l2->own_addr, ETH_ALEN);
        *(u16 *)(eapol + ETH_ALEN + ETH_ALEN) = host_to_be16(proto);
        os_memcpy(eapol + ETH_HLEN, buf, len);
        ret = wm_wifi_drv_send_eapol((uint8_t *)eapol, eapol_len);
        os_free(eapol);
    }

	return ret;
}


static void l2_packet_receive(uint8_t *src, uint8_t *eapol, uint32_t len, void *arg)
{
	struct l2_packet_data *l2 = arg;

	l2->rx_callback(l2->rx_callback_ctx, src, eapol, len);
}

struct l2_packet_data * l2_packet_init(
	const char *ifname, const u8 *own_addr, unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	struct l2_packet_data *l2;

	l2 = os_zalloc(sizeof(struct l2_packet_data));
	if (l2 == NULL)
		return NULL;
	os_strlcpy(l2->ifname, ifname, sizeof(l2->ifname));
	os_memcpy(l2->own_addr, own_addr, ETH_ALEN);
	l2->rx_callback = rx_callback;
	l2->rx_callback_ctx = rx_callback_ctx;
	l2->l2_hdr = l2_hdr;

	if (!os_strcmp(ifname, CONFIG_IFACE_NAME_STA))
    	wm_wifi_drv_sta_eapol_event_cb_register(l2_packet_receive, l2);
	else
	    wm_wifi_drv_ap_eapol_event_cb_register(l2_packet_receive, l2);

	return l2;
}


struct l2_packet_data * l2_packet_init_bridge(
	const char *br_ifname, const char *ifname, const u8 *own_addr,
	unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	return l2_packet_init(br_ifname, own_addr, protocol, rx_callback,
			      rx_callback_ctx, l2_hdr);
}


void l2_packet_deinit(struct l2_packet_data *l2)
{
	if (l2 == NULL)
		return;

	/* TODO: close connection */

	os_free(l2);
}


int l2_packet_get_ip_addr(struct l2_packet_data *l2, char *buf, size_t len)
{
	/* TODO: get interface IP address */
	return -1;
}


void l2_packet_notify_auth_start(struct l2_packet_data *l2)
{
	/* This function can be left empty */
}


int l2_packet_set_packet_filter(struct l2_packet_data *l2,
				enum l2_packet_filter_type type)
{
    /* TODO:  */
	return 0;
}
