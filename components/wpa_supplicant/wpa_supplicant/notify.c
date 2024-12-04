/*
 * wpa_supplicant - Event notifications
 * Copyright (c) 2009-2010, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/wpa_ctrl.h"
#include "config.h"
#include "wpa_supplicant_i.h"
#include "wps_supplicant.h"
#include "binder/binder.h"
#include "dbus/dbus_common.h"
#include "dbus/dbus_new.h"
#include "rsn_supp/wpa.h"
#include "fst/fst.h"
#include "crypto/tls.h"
#include "bss.h"
#include "driver_i.h"
#include "scan.h"
#include "p2p_supplicant.h"
#include "sme.h"
#include "notify.h"
#ifdef CONFIG_USE_EVENT_POST
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi_types.h"
#include "wm_wifi_event.h"
#include "ap/hostapd.h"
#include "ap/sta_info.h"
#endif

int wpas_notify_supplicant_initialized(struct wpa_global *global)
{
#ifdef CONFIG_CTRL_IFACE_DBUS_NEW
	if (global->params.dbus_ctrl_interface) {
		global->dbus = wpas_dbus_init(global);
		if (global->dbus == NULL)
			return -1;
	}
#endif /* CONFIG_CTRL_IFACE_DBUS_NEW */

#ifdef CONFIG_BINDER
	global->binder = wpas_binder_init(global);
	if (!global->binder)
		return -1;
#endif /* CONFIG_BINDER */

	return 0;
}


void wpas_notify_supplicant_deinitialized(struct wpa_global *global)
{
#ifdef CONFIG_CTRL_IFACE_DBUS_NEW
	if (global->dbus)
		wpas_dbus_deinit(global->dbus);
#endif /* CONFIG_CTRL_IFACE_DBUS_NEW */

#ifdef CONFIG_BINDER
	if (global->binder)
		wpas_binder_deinit(global->binder);
#endif /* CONFIG_BINDER */
}


int wpas_notify_iface_added(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return 0;
#endif

	if (wpas_dbus_register_interface(wpa_s))
		return -1;

	return 0;
}


void wpas_notify_iface_removed(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	/* unregister interface in new DBus ctrl iface */
	wpas_dbus_unregister_interface(wpa_s);
}


void wpas_notify_state_changed(struct wpa_supplicant *wpa_s,
			       enum wpa_states new_state,
			       enum wpa_states old_state)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	/* notify the new DBus API */
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_STATE);

#ifdef CONFIG_FST
	if (wpa_s->fst && !is_zero_ether_addr(wpa_s->bssid)) {
		if (new_state == WPA_COMPLETED)
			fst_notify_peer_connected(wpa_s->fst, wpa_s->bssid);
		else if (old_state >= WPA_ASSOCIATED &&
			 new_state < WPA_ASSOCIATED)
			fst_notify_peer_disconnected(wpa_s->fst, wpa_s->bssid);
	}
#endif /* CONFIG_FST */

	if (new_state == WPA_COMPLETED)
		wpas_p2p_notif_connected(wpa_s);
	else if (old_state >= WPA_ASSOCIATED && new_state < WPA_ASSOCIATED)
		wpas_p2p_notif_disconnected(wpa_s);

	sme_state_changed(wpa_s);

#ifdef ANDROID
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_STATE_CHANGE
		     "id=%d state=%d BSSID=" MACSTR " SSID=%s",
		     wpa_s->current_ssid ? wpa_s->current_ssid->id : -1,
		     new_state,
		     MAC2STR(wpa_s->bssid),
		     wpa_s->current_ssid && wpa_s->current_ssid->ssid ?
		     wpa_ssid_txt(wpa_s->current_ssid->ssid,
				  wpa_s->current_ssid->ssid_len) : "");
#endif /* ANDROID */

#ifdef CONFIG_USE_EVENT_POST
    static bool gtk_rekey = false;
    if ((WPA_GROUP_HANDSHAKE == new_state) && (WPA_COMPLETED == old_state)) {
        gtk_rekey = true;
        return;
    } else if ((WPA_4WAY_HANDSHAKE == new_state) && (WPA_COMPLETED == old_state)) { //maybe WPA_COMPLETED recved 3/4
        gtk_rekey = true;
        return;
    }
    if (gtk_rekey) {
        if ((WPA_GROUP_HANDSHAKE != new_state) && (WPA_COMPLETED != new_state)) {
            gtk_rekey = false; //gtk rekey fail
        } else {
            gtk_rekey = WPA_GROUP_HANDSHAKE == new_state ? true : false; //gtk rekey success, or still in gtk rekey
            return;
        }
    }
    if (WPA_COMPLETED == new_state) {
        if (wpa_s->current_bss) {
            wm_wifi_event_data_t data;
            os_memcpy(data.sta_connected_info.ssid, wpa_s->current_bss->ssid, wpa_s->current_bss->ssid_len);
            data.sta_connected_info.ssid_len = wpa_s->current_bss->ssid_len;
            os_memcpy(data.sta_connected_info.bssid, wpa_s->current_bss->bssid, 6);
            ieee80211_freq_to_chan(wpa_s->current_bss->freq, &(data.sta_connected_info.channel));
            if (wpa_s->current_bss->caps & IEEE80211_CAP_PRIVACY) {
                if (wpa_s->pairwise_cipher & (WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104)) {
                    data.sta_connected_info.authmode = WM_WIFI_AUTH_WEP;
                } else if ((wpa_s->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_SAE)) == (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_SAE)) {
                    data.sta_connected_info.authmode = WM_WIFI_AUTH_WPA2_WPA3_PSK;
                } else if (wpa_s->key_mgmt & WPA_KEY_MGMT_PSK) {
                    if ((wpa_s->wpa_proto & (WPA_PROTO_RSN | WPA_PROTO_WPA)) == (WPA_PROTO_RSN | WPA_PROTO_WPA)) {
                        data.sta_connected_info.authmode = WM_WIFI_AUTH_WPA_WPA2_PSK;
                    } else if (wpa_s->wpa_proto & WPA_PROTO_RSN) {
                        data.sta_connected_info.authmode = WM_WIFI_AUTH_WPA2_PSK;
                    } else if (wpa_s->wpa_proto & WPA_PROTO_WPA) {
                        data.sta_connected_info.authmode = WM_WIFI_AUTH_WPA_PSK;
                    }
                } else if (wpa_s->key_mgmt & WPA_KEY_MGMT_SAE) {
                    data.sta_connected_info.authmode = WM_WIFI_AUTH_WPA3_PSK;
                }
            } else {
                data.sta_connected_info.authmode = WM_WIFI_AUTH_OPEN;
            }
            wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_STA_CONNECTED, &data,sizeof(data));
        } else {
            wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_AP_START, NULL,0);
        }
    } else if (WPA_DISCONNECTED == new_state) {
        if (!strcmp(CONFIG_IFACE_NAME_STA, wpa_s->ifname)) {
            if (wpa_s->current_bss || wpa_s->current_ssid || (wpa_s->conf && wpa_s->conf->ssid)) {
                wm_wifi_event_data_t data = {0};
                if (wpa_s->current_bss) {
                    os_memcpy(data.sta_disconnected_info.ssid, wpa_s->current_bss->ssid, wpa_s->current_bss->ssid_len);
                    data.sta_disconnected_info.ssid_len = wpa_s->current_bss->ssid_len;
                    os_memcpy(data.sta_disconnected_info.bssid, wpa_s->current_bss->bssid, 6);
                } else if (wpa_s->current_ssid) {
                    os_memcpy(data.sta_disconnected_info.ssid, wpa_s->current_ssid->ssid, wpa_s->current_ssid->ssid_len);
                    data.sta_disconnected_info.ssid_len = wpa_s->current_ssid->ssid_len;
                    os_memcpy(data.sta_disconnected_info.bssid, wpa_s->current_ssid->bssid, 6);
                }
                data.sta_disconnected_info.reason = wpa_s->disconnect_reason < 0 ? -wpa_s->disconnect_reason : wpa_s->disconnect_reason;
                wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_STA_DISCONNECTED, &data,sizeof(data));
            }
        } else {
            if (WPA_COMPLETED == old_state)
                wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_AP_STOP, NULL,0);
        }
    }
#endif /* CONFIG_USE_EVENT_POST */
}


void wpas_notify_disconnect_reason(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_DISCONNECT_REASON);
}


void wpas_notify_auth_status_code(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_AUTH_STATUS_CODE);
}


void wpas_notify_assoc_status_code(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_ASSOC_STATUS_CODE);
}


void wpas_notify_roam_time(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_ROAM_TIME);
}


void wpas_notify_roam_complete(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_ROAM_COMPLETE);
}


void wpas_notify_session_length(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_SESSION_LENGTH);
}


void wpas_notify_bss_tm_status(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_BSS_TM_STATUS);
}


void wpas_notify_network_changed(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_CURRENT_NETWORK);
}


void wpas_notify_ap_scan_changed(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_AP_SCAN);
}


void wpas_notify_bssid_changed(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_CURRENT_BSS);
}


void wpas_notify_auth_changed(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_CURRENT_AUTH_MODE);
}


void wpas_notify_network_enabled_changed(struct wpa_supplicant *wpa_s,
					 struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_network_enabled_changed(wpa_s, ssid);
}


void wpas_notify_network_selected(struct wpa_supplicant *wpa_s,
				  struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_network_selected(wpa_s, ssid->id);
}


void wpas_notify_network_request(struct wpa_supplicant *wpa_s,
				 struct wpa_ssid *ssid,
				 enum wpa_ctrl_req_type rtype,
				 const char *default_txt)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_network_request(wpa_s, ssid, rtype, default_txt);
}


void wpas_notify_scanning(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	/* notify the new DBus API */
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_SCANNING);
}


void wpas_notify_scan_done(struct wpa_supplicant *wpa_s, int success)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_scan_done(wpa_s, success);

#ifdef CONFIG_USE_EVENT_POST
    if (MANUAL_SCAN_REQ == wpa_s->last_scan_req) {
        extern int wm_wifi_scan_get_ap_num_internal(uint16_t *number, void *ctx);
        wm_wifi_event_data_t data;
        uint16_t number = 0;
        wm_wifi_scan_get_ap_num_internal(&number, wpa_s);
        data.scan_done_info.status = !success;
        data.scan_done_info.number = number;
        wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_SCAN_DONE, &data,sizeof(data));
    }
#endif
}


void wpas_notify_scan_results(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_wps_notify_scan_results(wpa_s);
}


void wpas_notify_wps_credential(struct wpa_supplicant *wpa_s,
				const struct wps_credential *cred)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

#ifdef CONFIG_WPS
	/* notify the new DBus API */
	wpas_dbus_signal_wps_cred(wpa_s, cred);
#endif /* CONFIG_WPS */
}


void wpas_notify_wps_event_m2d(struct wpa_supplicant *wpa_s,
			       struct wps_event_m2d *m2d)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

#ifdef CONFIG_WPS
	wpas_dbus_signal_wps_event_m2d(wpa_s, m2d);
#endif /* CONFIG_WPS */
}


void wpas_notify_wps_event_fail(struct wpa_supplicant *wpa_s,
				struct wps_event_fail *fail)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

#ifdef CONFIG_WPS
	wpas_dbus_signal_wps_event_fail(wpa_s, fail);
#endif /* CONFIG_WPS */
}


void wpas_notify_wps_event_success(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

#ifdef CONFIG_WPS
	wpas_dbus_signal_wps_event_success(wpa_s);
#endif /* CONFIG_WPS */
}

void wpas_notify_wps_event_pbc_overlap(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

#ifdef CONFIG_WPS
	wpas_dbus_signal_wps_event_pbc_overlap(wpa_s);
#endif /* CONFIG_WPS */
}


void wpas_notify_network_added(struct wpa_supplicant *wpa_s,
			       struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;

	/*
	 * Networks objects created during any P2P activities should not be
	 * exposed out. They might/will confuse certain non-P2P aware
	 * applications since these network objects won't behave like
	 * regular ones.
	 */
	if (!ssid->p2p_group && wpa_s->global->p2p_group_formation != wpa_s) {
		wpas_dbus_register_network(wpa_s, ssid);
		wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_NETWORK_ADDED "%d",
			     ssid->id);
	}
#endif
}


void wpas_notify_persistent_group_added(struct wpa_supplicant *wpa_s,
					struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	wpas_dbus_register_persistent_group(wpa_s, ssid);
#endif /* CONFIG_P2P */
}


void wpas_notify_persistent_group_removed(struct wpa_supplicant *wpa_s,
					  struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	wpas_dbus_unregister_persistent_group(wpa_s, ssid->id);
#endif /* CONFIG_P2P */
}


void wpas_notify_network_removed(struct wpa_supplicant *wpa_s,
				 struct wpa_ssid *ssid)
{
	if (wpa_s->next_ssid == ssid)
		wpa_s->next_ssid = NULL;
	if (wpa_s->wpa)
		wpa_sm_pmksa_cache_flush(wpa_s->wpa, ssid);
#ifdef CONFIG_P2P
	if (!ssid->p2p_group && wpa_s->global->p2p_group_formation != wpa_s &&
	    !wpa_s->p2p_mgmt)
#endif
	{
		wpas_dbus_unregister_network(wpa_s, ssid->id);
		wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_NETWORK_REMOVED "%d",
			     ssid->id);
	}
#ifdef CONFIG_P2P
	if (network_is_persistent_group(ssid))
		wpas_notify_persistent_group_removed(wpa_s, ssid);
#endif

	wpas_p2p_network_removed(wpa_s, ssid);

#ifdef CONFIG_PASN
	if (wpa_s->pasn.ssid == ssid)
		wpa_s->pasn.ssid = NULL;
#endif /* CONFIG_PASN */
}


void wpas_notify_bss_added(struct wpa_supplicant *wpa_s,
			   u8 bssid[], unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_register_bss(wpa_s, bssid, id);
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_BSS_ADDED "%u " MACSTR,
		     id, MAC2STR(bssid));
}


void wpas_notify_bss_removed(struct wpa_supplicant *wpa_s,
			     u8 bssid[], unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_unregister_bss(wpa_s, bssid, id);
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_BSS_REMOVED "%u " MACSTR,
		     id, MAC2STR(bssid));
}


void wpas_notify_bss_freq_changed(struct wpa_supplicant *wpa_s,
				  unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_FREQ, id);
}


void wpas_notify_bss_signal_changed(struct wpa_supplicant *wpa_s,
				    unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_SIGNAL,
					  id);
}


void wpas_notify_bss_privacy_changed(struct wpa_supplicant *wpa_s,
				     unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_PRIVACY,
					  id);
}


void wpas_notify_bss_mode_changed(struct wpa_supplicant *wpa_s,
				  unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_MODE, id);
}


void wpas_notify_bss_wpaie_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_WPA, id);
}


void wpas_notify_bss_rsnie_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_RSN, id);
}


void wpas_notify_bss_wps_changed(struct wpa_supplicant *wpa_s,
				 unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

#ifdef CONFIG_WPS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_WPS, id);
#endif /* CONFIG_WPS */
}


void wpas_notify_bss_ies_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_IES, id);
}


void wpas_notify_bss_rates_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_RATES, id);
}


void wpas_notify_bss_seen(struct wpa_supplicant *wpa_s, unsigned int id)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_AGE, id);
}


void wpas_notify_blob_added(struct wpa_supplicant *wpa_s, const char *name)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_blob_added(wpa_s, name);
}


void wpas_notify_blob_removed(struct wpa_supplicant *wpa_s, const char *name)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_blob_removed(wpa_s, name);
}


void wpas_notify_debug_level_changed(struct wpa_global *global)
{
	wpas_dbus_signal_debug_level_changed(global);
}


void wpas_notify_debug_timestamp_changed(struct wpa_global *global)
{
	wpas_dbus_signal_debug_timestamp_changed(global);
}


void wpas_notify_debug_show_keys_changed(struct wpa_global *global)
{
	wpas_dbus_signal_debug_show_keys_changed(global);
}


void wpas_notify_suspend(struct wpa_global *global)
{
	struct wpa_supplicant *wpa_s;

	os_get_time(&global->suspend_time);
	wpa_printf(MSG_DEBUG, "System suspend notification");
	for (wpa_s = global->ifaces; wpa_s; wpa_s = wpa_s->next)
		wpa_drv_suspend(wpa_s);
}


void wpas_notify_resume(struct wpa_global *global)
{
#ifndef CONFIG_NO_STDOUT_DEBUG
	struct os_time now;
	int slept;
#endif
	struct wpa_supplicant *wpa_s;

#ifndef CONFIG_NO_STDOUT_DEBUG
	if (global->suspend_time.sec == 0)
		slept = -1;
	else {
		os_get_time(&now);
		slept = now.sec - global->suspend_time.sec;
	}
#endif

	wpa_printf(MSG_DEBUG, "System resume notification (slept %d seconds)",
		   slept);

	for (wpa_s = global->ifaces; wpa_s; wpa_s = wpa_s->next) {
		wpa_drv_resume(wpa_s);
		if (wpa_s->wpa_state == WPA_DISCONNECTED)
			wpa_supplicant_req_scan(wpa_s, 0, 100000);
	}
}


#ifdef CONFIG_P2P

void wpas_notify_p2p_find_stopped(struct wpa_supplicant *wpa_s)
{
	/* Notify P2P find has stopped */
	wpas_dbus_signal_p2p_find_stopped(wpa_s);
}


void wpas_notify_p2p_device_found(struct wpa_supplicant *wpa_s,
				  const u8 *dev_addr, int new_device)
{
	if (new_device) {
		/* Create the new peer object */
		wpas_dbus_register_peer(wpa_s, dev_addr);
	}

	/* Notify a new peer has been detected*/
	wpas_dbus_signal_peer_device_found(wpa_s, dev_addr);
}


void wpas_notify_p2p_device_lost(struct wpa_supplicant *wpa_s,
				 const u8 *dev_addr)
{
	wpas_dbus_unregister_peer(wpa_s, dev_addr);

	/* Create signal on interface object*/
	wpas_dbus_signal_peer_device_lost(wpa_s, dev_addr);
}


void wpas_notify_p2p_group_removed(struct wpa_supplicant *wpa_s,
				   const struct wpa_ssid *ssid,
				   const char *role)
{
	wpas_dbus_signal_p2p_group_removed(wpa_s, role);

	wpas_dbus_unregister_p2p_group(wpa_s, ssid);
}


void wpas_notify_p2p_go_neg_req(struct wpa_supplicant *wpa_s,
				const u8 *src, u16 dev_passwd_id, u8 go_intent)
{
	wpas_dbus_signal_p2p_go_neg_req(wpa_s, src, dev_passwd_id, go_intent);
}


void wpas_notify_p2p_go_neg_completed(struct wpa_supplicant *wpa_s,
				      struct p2p_go_neg_results *res)
{
	wpas_dbus_signal_p2p_go_neg_resp(wpa_s, res);
}


void wpas_notify_p2p_invitation_result(struct wpa_supplicant *wpa_s,
				       int status, const u8 *bssid)
{
	wpas_dbus_signal_p2p_invitation_result(wpa_s, status, bssid);
}


void wpas_notify_p2p_sd_request(struct wpa_supplicant *wpa_s,
				int freq, const u8 *sa, u8 dialog_token,
				u16 update_indic, const u8 *tlvs,
				size_t tlvs_len)
{
	wpas_dbus_signal_p2p_sd_request(wpa_s, freq, sa, dialog_token,
					update_indic, tlvs, tlvs_len);
}


void wpas_notify_p2p_sd_response(struct wpa_supplicant *wpa_s,
				 const u8 *sa, u16 update_indic,
				 const u8 *tlvs, size_t tlvs_len)
{
	wpas_dbus_signal_p2p_sd_response(wpa_s, sa, update_indic,
					 tlvs, tlvs_len);
}


/**
 * wpas_notify_p2p_provision_discovery - Notification of provision discovery
 * @dev_addr: Who sent the request or responded to our request.
 * @request: Will be 1 if request, 0 for response.
 * @status: Valid only in case of response (0 in case of success)
 * @config_methods: WPS config methods
 * @generated_pin: PIN to be displayed in case of WPS_CONFIG_DISPLAY method
 *
 * This can be used to notify:
 * - Requests or responses
 * - Various config methods
 * - Failure condition in case of response
 */
void wpas_notify_p2p_provision_discovery(struct wpa_supplicant *wpa_s,
					 const u8 *dev_addr, int request,
					 enum p2p_prov_disc_status status,
					 u16 config_methods,
					 unsigned int generated_pin)
{
	wpas_dbus_signal_p2p_provision_discovery(wpa_s, dev_addr, request,
						 status, config_methods,
						 generated_pin);
}


void wpas_notify_p2p_group_started(struct wpa_supplicant *wpa_s,
				   struct wpa_ssid *ssid, int persistent,
				   int client, const u8 *ip)
{
	/* Notify a group has been started */
	wpas_dbus_register_p2p_group(wpa_s, ssid);

	wpas_dbus_signal_p2p_group_started(wpa_s, client, persistent, ip);
}


void wpas_notify_p2p_group_formation_failure(struct wpa_supplicant *wpa_s,
					     const char *reason)
{
	/* Notify a group formation failed */
	wpas_dbus_signal_p2p_group_formation_failure(wpa_s, reason);
}


void wpas_notify_p2p_wps_failed(struct wpa_supplicant *wpa_s,
				struct wps_event_fail *fail)
{
	wpas_dbus_signal_p2p_wps_failed(wpa_s, fail);
}


void wpas_notify_p2p_invitation_received(struct wpa_supplicant *wpa_s,
					 const u8 *sa, const u8 *go_dev_addr,
					 const u8 *bssid, int id, int op_freq)
{
	/* Notify a P2P Invitation Request */
	wpas_dbus_signal_p2p_invitation_received(wpa_s, sa, go_dev_addr, bssid,
						 id, op_freq);
}

#endif /* CONFIG_P2P */


static void wpas_notify_ap_sta_authorized(struct wpa_supplicant *wpa_s,
					  const u8 *sta,
					  const u8 *p2p_dev_addr)
{
#ifdef CONFIG_P2P
	wpas_p2p_notify_ap_sta_authorized(wpa_s, p2p_dev_addr);

	/*
	 * Create 'peer-joined' signal on group object -- will also
	 * check P2P itself.
	 */
	if (p2p_dev_addr)
		wpas_dbus_signal_p2p_peer_joined(wpa_s, p2p_dev_addr);
#endif /* CONFIG_P2P */

	/* Register the station */
	wpas_dbus_register_sta(wpa_s, sta);

	/* Notify listeners a new station has been authorized */
	wpas_dbus_signal_sta_authorized(wpa_s, sta);

#ifdef CONFIG_AP
#ifdef CONFIG_USE_EVENT_POST
    wm_wifi_event_data_t data;
    struct sta_info *entry;
    data.ap_staconnected_info.aid = 0;
    os_memcpy(data.ap_staconnected_info.addr, sta, 6);
    if (wpa_s->ap_iface) {
    	for (entry = wpa_s->ap_iface->bss[0]->sta_list; entry; entry = entry->next) {
    		if (!os_memcmp(sta, entry->addr, 6))
    		{
                data.ap_staconnected_info.aid = entry->aid;
                break;
    		}
    	}
	}

	wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_AP_STACONNECTED, &data,sizeof(data));
#endif /* CONFIG_USE_EVENT_POST */
#endif /* CONFIG_AP */
}


static void wpas_notify_ap_sta_deauthorized(struct wpa_supplicant *wpa_s,
					    const u8 *sta,
					    const u8 *p2p_dev_addr)
{
#ifdef CONFIG_P2P
	/*
	 * Create 'peer-disconnected' signal on group object if this
	 * is a P2P group.
	 */
	if (p2p_dev_addr)
		wpas_dbus_signal_p2p_peer_disconnected(wpa_s, p2p_dev_addr);
#endif /* CONFIG_P2P */

	/* Notify listeners a station has been deauthorized */
	wpas_dbus_signal_sta_deauthorized(wpa_s, sta);

	/* Unregister the station */
	wpas_dbus_unregister_sta(wpa_s, sta);

#ifdef CONFIG_AP
#ifdef CONFIG_USE_EVENT_POST
	wm_wifi_event_data_t data;
    struct sta_info *entry;
    data.ap_stadisconnected_info.aid = 0;
    os_memcpy(data.ap_stadisconnected_info.addr, sta, 6);
    if (wpa_s->ap_iface) {
    	for (entry = wpa_s->ap_iface->bss[0]->sta_list; entry; entry = entry->next) {
    		if (!os_memcmp(sta, entry->addr, 6))
    		{
                data.ap_stadisconnected_info.aid = entry->aid;
                break;
    		}
    	}
	}

	wm_event_send(WM_WIFI_EV,WM_EVENT_WIFI_AP_STADISCONNECTED, &data,sizeof(data));
#endif /* CONFIG_USE_EVENT_POST */
#endif /* CONFIG_AP */
}


void wpas_notify_sta_authorized(struct wpa_supplicant *wpa_s,
				const u8 *mac_addr, int authorized,
				const u8 *p2p_dev_addr)
{
	if (authorized)
		wpas_notify_ap_sta_authorized(wpa_s, mac_addr, p2p_dev_addr);
	else
		wpas_notify_ap_sta_deauthorized(wpa_s, mac_addr, p2p_dev_addr);
}


void wpas_notify_certification(struct wpa_supplicant *wpa_s,
			       struct tls_cert_data *cert,
			       const char *cert_hash)
{
	int i;

	wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_EAP_PEER_CERT
		"depth=%d subject='%s'%s%s%s%s",
		cert->depth, cert->subject, cert_hash ? " hash=" : "",
		cert_hash ? cert_hash : "",
		cert->tod == 2 ? " tod=2" : "",
		cert->tod == 1 ? " tod=1" : "");

	if (cert->cert) {
		char *cert_hex;
		size_t len = wpabuf_len(cert->cert) * 2 + 1;
		cert_hex = os_malloc(len);
		if (cert_hex) {
			wpa_snprintf_hex(cert_hex, len, wpabuf_head(cert->cert),
					 wpabuf_len(cert->cert));
			wpa_msg_ctrl(wpa_s, MSG_INFO,
				     WPA_EVENT_EAP_PEER_CERT
				     "depth=%d subject='%s' cert=%s",
				     cert->depth, cert->subject, cert_hex);
			os_free(cert_hex);
		}
	}

	for (i = 0; i < cert->num_altsubject; i++)
		wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_EAP_PEER_ALT
			"depth=%d %s", cert->depth, cert->altsubject[i]);

	/* notify the new DBus API */
	wpas_dbus_signal_certification(wpa_s, cert->depth, cert->subject,
				       cert->altsubject, cert->num_altsubject,
				       cert_hash, cert->cert);
}


void wpas_notify_preq(struct wpa_supplicant *wpa_s,
		      const u8 *addr, const u8 *dst, const u8 *bssid,
		      const u8 *ie, size_t ie_len, u32 ssi_signal)
{
#ifdef CONFIG_AP
	wpas_dbus_signal_preq(wpa_s, addr, dst, bssid, ie, ie_len, ssi_signal);
#endif /* CONFIG_AP */
}


void wpas_notify_eap_status(struct wpa_supplicant *wpa_s, const char *status,
			    const char *parameter)
{
	wpas_dbus_signal_eap_status(wpa_s, status, parameter);
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_EAP_STATUS
		     "status='%s' parameter='%s'",
		     status, parameter);
}


void wpas_notify_eap_error(struct wpa_supplicant *wpa_s, int error_code)
{
	wpa_msg(wpa_s, MSG_ERROR, WPA_EVENT_EAP_ERROR_CODE "%d", error_code);
}


void wpas_notify_network_bssid_set_changed(struct wpa_supplicant *wpa_s,
					   struct wpa_ssid *ssid)
{
	if (wpa_s->current_ssid != ssid)
		return;

	wpa_dbg(wpa_s, MSG_DEBUG,
		"Network bssid config changed for the current network - within-ESS roaming %s",
		ssid->bssid_set ? "disabled" : "enabled");

	wpa_drv_roaming(wpa_s, !ssid->bssid_set,
			ssid->bssid_set ? ssid->bssid : NULL);
}


void wpas_notify_network_type_changed(struct wpa_supplicant *wpa_s,
				      struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	if (ssid->disabled == 2) {
		/* Changed from normal network profile to persistent group */
		ssid->disabled = 0;
		wpas_dbus_unregister_network(wpa_s, ssid->id);
		ssid->disabled = 2;
		ssid->p2p_persistent_group = 1;
		wpas_dbus_register_persistent_group(wpa_s, ssid);
	} else {
		/* Changed from persistent group to normal network profile */
		wpas_dbus_unregister_persistent_group(wpa_s, ssid->id);
		ssid->p2p_persistent_group = 0;
		wpas_dbus_register_network(wpa_s, ssid);
	}
#endif /* CONFIG_P2P */
}


#ifdef CONFIG_MESH

void wpas_notify_mesh_group_started(struct wpa_supplicant *wpa_s,
				    struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_mesh_group_started(wpa_s, ssid);
}


void wpas_notify_mesh_group_removed(struct wpa_supplicant *wpa_s,
				    const u8 *meshid, u8 meshid_len,
				    u16 reason_code)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_mesh_group_removed(wpa_s, meshid, meshid_len,
					    reason_code);
}


void wpas_notify_mesh_peer_connected(struct wpa_supplicant *wpa_s,
				     const u8 *peer_addr)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_mesh_peer_connected(wpa_s, peer_addr);
}


void wpas_notify_mesh_peer_disconnected(struct wpa_supplicant *wpa_s,
					const u8 *peer_addr, u16 reason_code)
{
#ifdef CONFIG_P2P
	if (wpa_s->p2p_mgmt)
		return;
#endif

	wpas_dbus_signal_mesh_peer_disconnected(wpa_s, peer_addr, reason_code);
}

#endif /* CONFIG_MESH */


#ifdef CONFIG_INTERWORKING

void wpas_notify_interworking_ap_added(struct wpa_supplicant *wpa_s,
				       struct wpa_bss *bss,
				       struct wpa_cred *cred, int excluded,
				       const char *type, int bh, int bss_load,
				       int conn_capab)
{
	wpa_msg(wpa_s, MSG_INFO, "%s" MACSTR " type=%s%s%s%s id=%d priority=%d sp_priority=%d",
		excluded ? INTERWORKING_EXCLUDED : INTERWORKING_AP,
		MAC2STR(bss->bssid), type,
		bh ? " below_min_backhaul=1" : "",
		bss_load ? " over_max_bss_load=1" : "",
		conn_capab ? " conn_capab_missing=1" : "",
		cred->id, cred->priority, cred->sp_priority);

	wpas_dbus_signal_interworking_ap_added(wpa_s, bss, cred, type, excluded,
					       bh, bss_load, conn_capab);
}


void wpas_notify_interworking_select_done(struct wpa_supplicant *wpa_s)
{
	wpas_dbus_signal_interworking_select_done(wpa_s);
}

#endif /* CONFIG_INTERWORKING */
