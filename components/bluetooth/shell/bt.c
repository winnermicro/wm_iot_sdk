/** @file
 * @brief Bluetooth shell module
 *
 * Provide some Bluetooth shell commands that can be useful to applications.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <sys/util.h>
#include <sys/util_macro.h>
#include <kernel.h>

#include <settings/settings.h>

#include <bluetooth/hci.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#if defined(CONFIG_BT_BREDR)
#include <bluetooth/rfcomm.h>
#endif
#include <bluetooth/sdp.h>
#include <bluetooth/iso.h>

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_cli.h"
#include "wm_utils.h"


#include "bt.h"
#include "ll.h"
#include "hci.h"
//#include "../audio/shell/audio.h"

static bool no_settings_load;

uint8_t selected_id = BT_ID_DEFAULT;

#if defined(CONFIG_BT_CONN)
struct bt_conn *default_conn;

/* Connection context for BR/EDR legacy pairing in sec mode 3 */
static struct bt_conn *pairing_conn;

static struct bt_le_oob oob_local;
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
static struct bt_le_oob oob_remote;
#endif /* CONFIG_BT_SMP || CONFIG_BT_BREDR) */
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_SMP)
static struct bt_conn_auth_info_cb auth_info_cb;
#endif /* CONFIG_BT_SMP */

#define NAME_LEN 30

#define KEY_STR_LEN 33

#define ADV_DATA_DELIMITER ", "

#define AD_SIZE 9

/*
 * Based on the maximum number of parameters for HCI_LE_Generate_DHKey
 * See BT Core Spec V5.2 Vol. 4, Part E, section 7.8.37
 */
#define HCI_CMD_MAX_PARAM 65

#if defined(CONFIG_BT_BROADCASTER)
enum {
	SHELL_ADV_OPT_CONNECTABLE,
	SHELL_ADV_OPT_DISCOVERABLE,
	SHELL_ADV_OPT_EXT_ADV,
	SHELL_ADV_OPT_APPEARANCE,
	SHELL_ADV_OPT_KEEP_RPA,

	SHELL_ADV_OPT_NUM,
};

static ATOMIC_DEFINE(adv_opt, SHELL_ADV_OPT_NUM);
#if defined(CONFIG_BT_EXT_ADV)
uint8_t selected_adv;
struct bt_le_ext_adv *adv_sets[CONFIG_BT_EXT_ADV_MAX_ADV_SET];
static ATOMIC_DEFINE(adv_set_opt, SHELL_ADV_OPT_NUM)[CONFIG_BT_EXT_ADV_MAX_ADV_SET];
#endif /* CONFIG_BT_EXT_ADV */
#endif /* CONFIG_BT_BROADCASTER */

#define HELP_NONE "[none]\n"
#define HELP_ONOFF <on, off>
#define HELP_ADDR <address: XX:XX:XX:XX:XX:XX>
#define HELP_ADDR_LE <address: XX:XX:XX:XX:XX:XX> <type: (public|random)>
#define HELP_BT_INIT "type help this cmd for help\n"

#if defined(CONFIG_BT_EXT_ADV)
#define EXT_ADV_SCAN_OPT " [coded] [no-1m]"
#define EXT_ADV_PARAM                                                                              \
	<type: conn-scan conn-nscan, nconn-scan nconn-nscan>                                     \
	[ext-adv] [no-2m] [coded] [anon] [tx-power] [scan-reports]                               \
	[filter-accept-list: fal, fal-scan, fal-conn] [identity] [name]                          \
	[name-ad] [directed <address: XX:XX:XX:XX:XX:XX> <type: (public|random)>] [mode: low] [dir-rpa] \
	[disable-37] [disable-38] [disable-39]
#else
#define EXT_ADV_SCAN_OPT ""
#endif /* defined(CONFIG_BT_EXT_ADV) */


static void print_data_hex(const uint8_t *data, uint8_t len)
{
	if (len == 0)
		return;

	for (int16_t i = len - 1; i >= 0; i--) {
		printf("%02x", data[i]);
	}
    printf("\n");
}

static void print_data_set(uint8_t set_value_len,
			   const uint8_t *scan_data, uint8_t scan_data_len)
{
	uint8_t idx = 0;

	if (scan_data_len == 0 || set_value_len > scan_data_len) {
		return;
	}

	do {
		print_data_hex(&scan_data[idx], set_value_len);
		idx += set_value_len;
	} while (idx + set_value_len <= scan_data_len);

	if (idx < scan_data_len) {
		print_data_hex(&scan_data[idx], scan_data_len - idx);
	}
}


#if defined(CONFIG_BT_OBSERVER) || defined(CONFIG_BT_USER_PHY_UPDATE)
static const char *phy2str(uint8_t phy)
{
	switch (phy) {
	case 0: return "No packets";
	case BT_GAP_LE_PHY_1M: return "LE 1M";
	case BT_GAP_LE_PHY_2M: return "LE 2M";
	case BT_GAP_LE_PHY_CODED: return "LE Coded";
	default: return "Unknown";
	}
}
#endif

#if defined(CONFIG_BT_CONN) || (defined(CONFIG_BT_BROADCASTER) && defined(CONFIG_BT_EXT_ADV))
static void print_le_addr(const char *desc, const bt_addr_le_t *addr)
{
	char addr_str[BT_ADDR_LE_STR_LEN];

	const char *addr_desc = bt_addr_le_is_identity(addr) ? "identity" :
				bt_addr_le_is_rpa(addr) ? "resolvable" :
				"non-resolvable";

	bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

	printf("%s address: %s (%s)\n", desc, addr_str,
		    addr_desc);
}
#endif /* CONFIG_BT_CONN || (CONFIG_BT_BROADCASTER && CONFIG_BT_EXT_ADV) */

#if defined(CONFIG_BT_CENTRAL)
static int cmd_scan_off(void);
static void cmd_connect_le(int argc, char *argv[]);
static void cmd_scan_filter_clear_name(int argc,
				      char *argv[]);

static struct bt_auto_connect {
	bt_addr_le_t addr;
	bool addr_set;
	bool connect_name;
} auto_connect;
#endif

#if defined(CONFIG_BT_OBSERVER)
static struct bt_scan_filter {
	char name[NAME_LEN];
	bool name_set;
	char addr[BT_ADDR_STR_LEN];
	bool addr_set;
	int8_t rssi;
	bool rssi_set;
	uint16_t pa_interval;
	bool pa_interval_set;
} scan_filter;

static const char scan_response_label[] = "[DEVICE]: ";
static bool scan_verbose_output;

/**
 * @brief Compares two strings without case sensitivy
 *
 * @param substr The substring
 * @param str The string to find the substring in
 *
 * @return true if @substr is a substring of @p, else false
 */
static bool is_substring(const char *substr, const char *str)
{
	const size_t str_len = strlen(str);
	const size_t sub_str_len = strlen(substr);

	if (sub_str_len > str_len) {
		return false;
	}

	for (size_t pos = 0; pos < str_len; pos++) {
		if (tolower(substr[0]) == tolower(str[pos])) {
			if (pos + sub_str_len > str_len) {
				return false;
			}

			if (strncasecmp(substr, &str[pos], sub_str_len) == 0) {
				return true;
			}
		}
	}

	return false;
}

static bool data_cb(struct bt_data *data, void *user_data)
{
	char *name = user_data;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
	case BT_DATA_BROADCAST_NAME:
		memcpy(name, data->data, MIN(data->data_len, NAME_LEN - 1));
		return false;
	default:
		return true;
	}
}

static bool data_verbose_cb(struct bt_data *data, void *user_data)
{
	printf("%*sType 0x%02x: ",
		      strlen(scan_response_label), "", data->type);

	switch (data->type) {
	case BT_DATA_UUID16_SOME:
	case BT_DATA_UUID16_ALL:
	case BT_DATA_SOLICIT16:
		print_data_set(BT_UUID_SIZE_16, data->data, data->data_len);
		break;
	case BT_DATA_SVC_DATA16:
		/* Data starts with a UUID16 (2 bytes),
		 * the rest is unknown and printed as single bytes
		 */
		if (data->data_len < BT_UUID_SIZE_16) {
			printf("BT_DATA_SVC_DATA16 data length too short (%u)",
				      data->data_len);
			break;
		}
		print_data_set(BT_UUID_SIZE_16, data->data, BT_UUID_SIZE_16);
		if (data->data_len > BT_UUID_SIZE_16) {
			printf("adv contents:");
			print_data_set(1, data->data + BT_UUID_SIZE_16,
				       data->data_len - BT_UUID_SIZE_16);
		}
		break;
	case BT_DATA_UUID32_SOME:
	case BT_DATA_UUID32_ALL:
		print_data_set(BT_UUID_SIZE_32, data->data, data->data_len);
		break;
	case BT_DATA_SVC_DATA32:
		/* Data starts with a UUID32 (4 bytes),
		 * the rest is unknown and printed as single bytes
		 */
		if (data->data_len < BT_UUID_SIZE_32) {
			printf("BT_DATA_SVC_DATA32 data length too short (%u)",
				      data->data_len);
			break;
		}
		print_data_set(BT_UUID_SIZE_32, data->data, BT_UUID_SIZE_32);
		if (data->data_len > BT_UUID_SIZE_32) {
			print_data_set(1, data->data + BT_UUID_SIZE_32,
				       data->data_len - BT_UUID_SIZE_32);
		}
		break;
	case BT_DATA_UUID128_SOME:
	case BT_DATA_UUID128_ALL:
	case BT_DATA_SOLICIT128:
		print_data_set(BT_UUID_SIZE_128, data->data, data->data_len);
		break;
	case BT_DATA_SVC_DATA128:
		/* Data starts with a UUID128 (16 bytes),
		 * the rest is unknown and printed as single bytes
		 */
		if (data->data_len < BT_UUID_SIZE_128) {
			printf("BT_DATA_SVC_DATA128 data length too short (%u)",
				      data->data_len);
			break;
		}
		print_data_set(BT_UUID_SIZE_128, data->data, BT_UUID_SIZE_128);
		if (data->data_len > BT_UUID_SIZE_128) {
			print_data_set(1, data->data + BT_UUID_SIZE_128,
				       data->data_len - BT_UUID_SIZE_128);
		}
		break;
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
	case BT_DATA_BROADCAST_NAME:
		printf("%.*s", data->data_len, data->data);
		break;
	case BT_DATA_PUB_TARGET_ADDR:
	case BT_DATA_RAND_TARGET_ADDR:
	case BT_DATA_LE_BT_DEVICE_ADDRESS:
		print_data_set(BT_ADDR_SIZE, data->data, data->data_len);
		break;
	case BT_DATA_CSIS_RSI:
		print_data_set(3, data->data, data->data_len);
		break;
	default:
		print_data_set(1, data->data, data->data_len);
	}

	printf("\n");

	return true;
}

static const char *scan_response_type_txt(uint8_t type)
{
	switch (type) {
	case BT_GAP_ADV_TYPE_ADV_IND:
		return "ADV_IND";
	case BT_GAP_ADV_TYPE_ADV_DIRECT_IND:
		return "ADV_DIRECT_IND";
	case BT_GAP_ADV_TYPE_ADV_SCAN_IND:
		return "ADV_SCAN_IND";
	case BT_GAP_ADV_TYPE_ADV_NONCONN_IND:
		return "ADV_NONCONN_IND";
	case BT_GAP_ADV_TYPE_SCAN_RSP:
		return "SCAN_RSP";
	case BT_GAP_ADV_TYPE_EXT_ADV:
		return "EXT_ADV";
	default:
		return "UNKNOWN";
	}
}

bool passes_scan_filter(const struct bt_le_scan_recv_info *info, const struct net_buf_simple *buf)
{

	if (scan_filter.rssi_set && (scan_filter.rssi > info->rssi)) {
		return false;
	}

	if (scan_filter.pa_interval_set &&
	    (scan_filter.pa_interval > BT_CONN_INTERVAL_TO_MS(info->interval))) {
		return false;
	}

	if (scan_filter.addr_set) {
		char le_addr[BT_ADDR_LE_STR_LEN] = {0};

		bt_addr_le_to_str(info->addr, le_addr, sizeof(le_addr));

		if (!is_substring(scan_filter.addr, le_addr)) {
			return false;
		}
	}

	if (scan_filter.name_set) {
		struct net_buf_simple buf_copy;
		char name[NAME_LEN] = {0};

		/* call to bt_data_parse consumes netbufs so shallow clone for verbose output */
		net_buf_simple_clone(buf, &buf_copy);
		bt_data_parse(&buf_copy, data_cb, name);

		if (!is_substring(scan_filter.name, name)) {
			return false;
		}
	}

	return true;
}

static void scan_recv(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf)
{
	char le_addr[BT_ADDR_LE_STR_LEN];
	char name[NAME_LEN];
	struct net_buf_simple buf_copy;

	if (!passes_scan_filter(info, buf)) {
		return;
	}

	if (scan_verbose_output) {
		/* call to bt_data_parse consumes netbufs so shallow clone for verbose output */
		net_buf_simple_clone(buf, &buf_copy);
	}

	(void)memset(name, 0, sizeof(name));

	bt_data_parse(buf, data_cb, name);
	bt_addr_le_to_str(info->addr, le_addr, sizeof(le_addr));

	printf("%s%s, AD evt type %u, RSSI %i %s "
		    "C:%u S:%u D:%d SR:%u E:%u Prim: %s, Secn: %s, "
		    "Interval: 0x%04x (%u us), SID: 0x%x\n",
		    scan_response_label,
		    le_addr, info->adv_type, info->rssi, name,
		    (info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) != 0,
		    (info->adv_props & BT_GAP_ADV_PROP_SCANNABLE) != 0,
		    (info->adv_props & BT_GAP_ADV_PROP_DIRECTED) != 0,
		    (info->adv_props & BT_GAP_ADV_PROP_SCAN_RESPONSE) != 0,
		    (info->adv_props & BT_GAP_ADV_PROP_EXT_ADV) != 0,
		    phy2str(info->primary_phy), phy2str(info->secondary_phy),
		    info->interval, BT_CONN_INTERVAL_TO_US(info->interval),
		    info->sid);

	if (scan_verbose_output) {
		printf("%*s[SCAN DATA START - %s]\n",
			   strlen(scan_response_label), "",
			   scan_response_type_txt(info->adv_type));
		bt_data_parse(&buf_copy, data_verbose_cb, NULL);
		printf("%*s[SCAN DATA END]\n", strlen(scan_response_label), "");
	}

	/* Store address for later use */
#if defined(CONFIG_BT_CENTRAL)
	auto_connect.addr_set = true;
	bt_addr_le_copy(&auto_connect.addr, info->addr);

	/* Use the above auto_connect.addr address to automatically connect */
	if ((info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) != 0U && auto_connect.connect_name) {
		auto_connect.connect_name = false;

		cmd_scan_off();

		/* "name" is what would be in argv[0] normally */
		cmd_scan_filter_clear_name(1, (char *[]){ "name" });

		/* "connect" is what would be in argv[0] normally */
		cmd_connect_le(1, (char *[]){ "connect" });
	}
#endif /* CONFIG_BT_CENTRAL */
}

static void scan_timeout(void)
{
	printf("Scan timeout\n");

#if defined(CONFIG_BT_CENTRAL)
	if (auto_connect.connect_name) {
		auto_connect.connect_name = false;
		/* "name" is what would be in argv[0] normally */
		cmd_scan_filter_clear_name(1, (char *[]){ "name" });
	}
#endif /* CONFIG_BT_CENTRAL */
}
#endif /* CONFIG_BT_OBSERVER */

#if defined(CONFIG_BT_EXT_ADV)
#if defined(CONFIG_BT_BROADCASTER)
static void adv_sent(struct bt_le_ext_adv *adv,
		     struct bt_le_ext_adv_sent_info *info)
{
	printf("Advertiser[%d] %p sent %d\n",
		    bt_le_ext_adv_get_index(adv), adv, info->num_sent);
}

static void adv_scanned(struct bt_le_ext_adv *adv,
			struct bt_le_ext_adv_scanned_info *info)
{
	char str[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(info->addr, str, sizeof(str));

	printf("Advertiser[%d] %p scanned by %s\n",
		    bt_le_ext_adv_get_index(adv), adv, str);
}
#endif /* CONFIG_BT_BROADCASTER */

#if defined(CONFIG_BT_PERIPHERAL)
static void adv_connected(struct bt_le_ext_adv *adv,
			  struct bt_le_ext_adv_connected_info *info)
{
	char str[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(info->conn), str, sizeof(str));

	printf("Advertiser[%d] %p connected by %s\n",
		    bt_le_ext_adv_get_index(adv), adv, str);
}
#endif /* CONFIG_BT_PERIPHERAL */

#if defined(CONFIG_BT_PRIVACY)
static bool adv_rpa_expired(struct bt_le_ext_adv *adv)
{
	uint8_t adv_index = bt_le_ext_adv_get_index(adv);

	bool keep_rpa = atomic_test_bit(adv_set_opt[adv_index],
					  SHELL_ADV_OPT_KEEP_RPA);
	printf("Advertiser[%d] %p RPA %s\n",
		    adv_index, adv,
		    keep_rpa ? "not expired" : "expired");

	return keep_rpa;
}
#endif /* defined(CONFIG_BT_PRIVACY) */

#endif /* CONFIG_BT_EXT_ADV */

#if !defined(CONFIG_BT_CONN)
#if 0 /* FIXME: Add support for changing prompt */
static const char *current_prompt(void)
{
	return NULL;
}
#endif
#endif /* !CONFIG_BT_CONN */

#if defined(CONFIG_BT_CONN)
#if 0 /* FIXME: Add support for changing prompt */
static const char *current_prompt(void)
{
	static char str[BT_ADDR_LE_STR_LEN + 2];
	static struct bt_conn_info info;

	if (!default_conn) {
		return NULL;
	}

	if (bt_conn_get_info(default_conn, &info) < 0) {
		return NULL;
	}

	if (info.type != BT_CONN_TYPE_LE) {
		return NULL;
	}

	bt_addr_le_to_str(info.le.dst, str, sizeof(str) - 2);
	strcat(str, "> ");
	return str;
}
#endif

void conn_addr_str(struct bt_conn *conn, char *addr, size_t len)
{
	struct bt_conn_info info;

	if (bt_conn_get_info(conn, &info) < 0) {
		addr[0] = '\0';
		return;
	}

	switch (info.type) {
#if defined(CONFIG_BT_BREDR)
	case BT_CONN_TYPE_BR:
		bt_addr_to_str(info.br.dst, addr, len);
		break;
#endif
	case BT_CONN_TYPE_LE:
		bt_addr_le_to_str(info.le.dst, addr, len);
		break;
	default:
		break;
	}
}

static void print_le_oob( struct bt_le_oob *oob)
{
	char addr[BT_ADDR_LE_STR_LEN];
	char c[KEY_STR_LEN];
	char r[KEY_STR_LEN];

	bt_addr_le_to_str(&oob->addr, addr, sizeof(addr));

	bin2hex(oob->le_sc_data.c, sizeof(oob->le_sc_data.c), c, sizeof(c));
	bin2hex(oob->le_sc_data.r, sizeof(oob->le_sc_data.r), r, sizeof(r));

	printf("OOB data:\n");
	printf("%-29s %-32s %-32s\n", "addr", "random", "confirm");
	printf("%29s %32s %32s\n", addr, r, c);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];
	struct bt_conn_info info;
	int info_err;

	conn_addr_str(conn, addr, sizeof(addr));

	if (err) {
		printf("Failed to connect to %s (0x%02x)\n", addr,
			     err);
		goto done;
	}

	printf("Connected: %s\n", addr);

	info_err = bt_conn_get_info(conn, &info);
	if (info_err != 0) {
		printf("Failed to connection information: %d\n", info_err);
		goto done;
	}

	if (info.role == BT_CONN_ROLE_CENTRAL) {
		if (default_conn != NULL) {
			bt_conn_unref(default_conn);
		}

		default_conn = bt_conn_ref(conn);
	} else if (info.role == BT_CONN_ROLE_PERIPHERAL) {
		if (default_conn == NULL) {
			default_conn = bt_conn_ref(conn);
		}
	}

done:
	/* clear connection reference for sec mode 3 pairing */
	if (pairing_conn) {
		bt_conn_unref(pairing_conn);
		pairing_conn = NULL;
	}
}

static void disconnected_set_new_default_conn_cb(struct bt_conn *conn, void *user_data)
{
	struct bt_conn_info info;

	if (default_conn != NULL) {
		/* nop */
		return;
	}

	if (bt_conn_get_info(conn, &info) != 0) {
		printf("Unable to get info: conn %p\n", conn);
		return;
	}

	if (info.state == BT_CONN_STATE_CONNECTED) {
		char addr_str[BT_ADDR_LE_STR_LEN];

		default_conn = bt_conn_ref(conn);

		bt_addr_le_to_str(info.le.dst, addr_str, sizeof(addr_str));
		printf("Selected conn is now: %s\n", addr_str);
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	conn_addr_str(conn, addr, sizeof(addr));
	printf("Disconnected: %s (reason 0x%02x)\n", addr, reason);

	if (default_conn == conn) {
		bt_conn_unref(default_conn);
		default_conn = NULL;

		/* If we are connected to other devices, set one of them as default */
		bt_conn_foreach(BT_CONN_TYPE_LE, disconnected_set_new_default_conn_cb, NULL);
	}
}

static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param)
{
	printf("LE conn  param req: int (0x%04x, 0x%04x) lat %d to %d\n", param->interval_min, param->interval_max,
		    param->latency, param->timeout);

	return true;
}

static void le_param_updated(struct bt_conn *conn, uint16_t interval,
			     uint16_t latency, uint16_t timeout)
{
	printf("LE conn param updated: int 0x%04x lat %d to %d\n", interval, latency, timeout);
}

#if defined(CONFIG_BT_SMP)
static void identity_resolved(struct bt_conn *conn, const bt_addr_le_t *rpa,
			      const bt_addr_le_t *identity)
{
	char addr_identity[BT_ADDR_LE_STR_LEN];
	char addr_rpa[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
	bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

	printf("Identity resolved %s -> %s\n", addr_rpa,
	      addr_identity);
}
#endif

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
static const char *security_err_str(enum bt_security_err err)
{
	switch (err) {
	case BT_SECURITY_ERR_SUCCESS:
		return "Success";
	case BT_SECURITY_ERR_AUTH_FAIL:
		return "Authentication failure";
	case BT_SECURITY_ERR_PIN_OR_KEY_MISSING:
		return "PIN or key missing";
	case BT_SECURITY_ERR_OOB_NOT_AVAILABLE:
		return "OOB not available";
	case BT_SECURITY_ERR_AUTH_REQUIREMENT:
		return "Authentication requirements";
	case BT_SECURITY_ERR_PAIR_NOT_SUPPORTED:
		return "Pairing not supported";
	case BT_SECURITY_ERR_PAIR_NOT_ALLOWED:
		return "Pairing not allowed";
	case BT_SECURITY_ERR_INVALID_PARAM:
		return "Invalid parameters";
	case BT_SECURITY_ERR_UNSPECIFIED:
		return "Unspecified";
	default:
		return "Unknown";
	}
}

static void security_changed(struct bt_conn *conn, bt_security_t level,
			     enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	conn_addr_str(conn, addr, sizeof(addr));

	if (!err) {
		printf("Security changed: %s level %u\n", addr,
			    level);
	} else {
		printf("Security failed: %s level %u reason: %s (%d)\n",
			    addr, level, security_err_str(err), err);
	}
}
#endif

#if defined(CONFIG_BT_REMOTE_INFO)
static const char *ver_str(uint8_t ver)
{
	const char * const str[] = {
		"1.0b", "1.1", "1.2", "2.0", "2.1", "3.0", "4.0", "4.1", "4.2",
		"5.0", "5.1", "5.2", "5.3", "5.4"
	};

	if (ver < ARRAY_SIZE(str)) {
		return str[ver];
	}

	return "unknown";
}

static void remote_info_available(struct bt_conn *conn,
				  struct bt_conn_remote_info *remote_info)
{
	struct bt_conn_info info;

	bt_conn_get_info(conn, &info);

	if (IS_ENABLED(CONFIG_BT_REMOTE_VERSION)) {
		printf("Remote LMP version %s (0x%02x) subversion 0x%04x manufacturer 0x%04x\n", ver_str(remote_info->version),
			    remote_info->version, remote_info->subversion,
			    remote_info->manufacturer);
	}

	if (info.type == BT_CONN_TYPE_LE) {
		uint8_t features[8];
		char features_str[2 * sizeof(features) +  1];

		sys_memcpy_swap(features, remote_info->le.features,
				sizeof(features));
		bin2hex(features, sizeof(features),
			features_str, sizeof(features_str));
		printf("LE Features: 0x%s \n", features_str);
	}
}
#endif /* defined(CONFIG_BT_REMOTE_INFO) */

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
void le_data_len_updated(struct bt_conn *conn,
			 struct bt_conn_le_data_len_info *info)
{
	printf("LE data len updated: TX (len: %d time: %d) RX (len: %d time: %d)\n", info->tx_max_len,
		    info->tx_max_time, info->rx_max_len, info->rx_max_time);
}
#endif

#if defined(CONFIG_BT_USER_PHY_UPDATE)
void le_phy_updated(struct bt_conn *conn,
		    struct bt_conn_le_phy_info *info)
{
	printf("LE PHY updated: TX PHY %s, RX PHY %s\n",
		    phy2str(info->tx_phy), phy2str(info->rx_phy));
}
#endif

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
	.le_param_req = le_param_req,
	.le_param_updated = le_param_updated,
#if defined(CONFIG_BT_SMP)
	.identity_resolved = identity_resolved,
#endif
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
	.security_changed = security_changed,
#endif
#if defined(CONFIG_BT_REMOTE_INFO)
	.remote_info_available = remote_info_available,
#endif
#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
	.le_data_len_updated = le_data_len_updated,
#endif
#if defined(CONFIG_BT_USER_PHY_UPDATE)
	.le_phy_updated = le_phy_updated,
#endif
};
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_OBSERVER)
static struct bt_le_scan_cb scan_callbacks = {
	.recv = scan_recv,
	.timeout = scan_timeout,
};
#endif /* defined(CONFIG_BT_OBSERVER) */

#if defined(CONFIG_BT_EXT_ADV)
#if defined(CONFIG_BT_BROADCASTER)
static struct bt_le_ext_adv_cb adv_callbacks = {
	.sent = adv_sent,
	.scanned = adv_scanned,
#if defined(CONFIG_BT_PERIPHERAL)
	.connected = adv_connected,
#endif /* CONFIG_BT_PERIPHERAL */
#if defined(CONFIG_BT_PRIVACY)
	.rpa_expired = adv_rpa_expired,
#endif /* defined(CONFIG_BT_PRIVACY) */

};
#endif /* CONFIG_BT_BROADCASTER */
#endif /* CONFIG_BT_EXT_ADV */


#if defined(CONFIG_BT_PER_ADV_SYNC)
struct bt_le_per_adv_sync *per_adv_syncs[CONFIG_BT_PER_ADV_SYNC_MAX];
size_t selected_per_adv_sync;

static void per_adv_sync_sync_cb(struct bt_le_per_adv_sync *sync,
				 struct bt_le_per_adv_sync_synced_info *info)
{
	const bool is_past_peer = info->conn != NULL;
	char le_addr[BT_ADDR_LE_STR_LEN];
	char past_peer[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(info->addr, le_addr, sizeof(le_addr));

	if (is_past_peer) {
		conn_addr_str(info->conn, past_peer, sizeof(past_peer));
	}

	printf("PER_ADV_SYNC[%u]: [DEVICE]: %s synced, Interval 0x%04x (%u us), PHY %s, SD 0x%04X, PAST peer %s\n",
		    bt_le_per_adv_sync_get_index(sync), le_addr,
		    info->interval, BT_CONN_INTERVAL_TO_US(info->interval),
		    phy2str(info->phy), info->service_data,
		    is_past_peer ? past_peer : "not present");

	if (info->conn) { /* if from PAST */
		for (int i = 0; i < ARRAY_SIZE(per_adv_syncs); i++) {
			if (!per_adv_syncs[i]) {
				per_adv_syncs[i] = sync;
				break;
			}
		}
	}
}

static void per_adv_sync_terminated_cb(
	struct bt_le_per_adv_sync *sync,
	const struct bt_le_per_adv_sync_term_info *info)
{
	char le_addr[BT_ADDR_LE_STR_LEN];

	for (int i = 0; i < ARRAY_SIZE(per_adv_syncs); i++) {
		if (per_adv_syncs[i] == sync) {
			per_adv_syncs[i] = NULL;
			break;
		}
	}

	bt_addr_le_to_str(info->addr, le_addr, sizeof(le_addr));
	printf("PER_ADV_SYNC[%u]: [DEVICE]: %s sync terminated\n",
		    bt_le_per_adv_sync_get_index(sync), le_addr);
}

static void per_adv_sync_recv_cb(
	struct bt_le_per_adv_sync *sync,
	const struct bt_le_per_adv_sync_recv_info *info,
	struct net_buf_simple *buf)
{
	char le_addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(info->addr, le_addr, sizeof(le_addr));
	printf("PER_ADV_SYNC[%u]: [DEVICE]: %s, tx_power %i, RSSI %i, CTE %u, data length %u\n",
		    bt_le_per_adv_sync_get_index(sync), le_addr, info->tx_power,
		    info->rssi, info->cte_type, buf->len);
}

static void per_adv_sync_biginfo_cb(struct bt_le_per_adv_sync *sync,
				    const struct bt_iso_biginfo *biginfo)
{
	char le_addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(biginfo->addr, le_addr, sizeof(le_addr));
	printf("BIG_INFO PER_ADV_SYNC[%u]: [DEVICE]: %s, sid 0x%02x, num_bis %u, "
		    "nse 0x%02x, interval 0x%04x (%u us), bn 0x%02x, pto 0x%02x, irc 0x%02x, "
		    "max_pdu 0x%04x, sdu_interval 0x%04x, max_sdu 0x%04x, phy %s, framing 0x%02x, "
		    "%sencrypted\n",
		    bt_le_per_adv_sync_get_index(sync), le_addr, biginfo->sid, biginfo->num_bis,
		    biginfo->sub_evt_count, biginfo->iso_interval,
		    BT_CONN_INTERVAL_TO_US(biginfo->iso_interval), biginfo->burst_number,
		    biginfo->offset, biginfo->rep_count, biginfo->max_pdu, biginfo->sdu_interval,
		    biginfo->max_sdu, phy2str(biginfo->phy), biginfo->framing,
		    biginfo->encryption ? "" : "not ");
}

static struct bt_le_per_adv_sync_cb per_adv_sync_cb = {
	.synced = per_adv_sync_sync_cb,
	.term = per_adv_sync_terminated_cb,
	.recv = per_adv_sync_recv_cb,
	.biginfo = per_adv_sync_biginfo_cb,
};
#endif /* CONFIG_BT_PER_ADV_SYNC */

static void bt_ready(int err)
{
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printf("Bluetooth initialized\n");

	if (IS_ENABLED(CONFIG_SETTINGS) && !no_settings_load) {
		settings_load();
		printf("Settings Loaded\n");
	}

	if (IS_ENABLED(CONFIG_BT_SMP_OOB_LEGACY_PAIR_ONLY)) {
		bt_le_oob_set_legacy_flag(true);
	}

#if defined(CONFIG_BT_OBSERVER)
	bt_le_scan_cb_register(&scan_callbacks);
#endif

#if defined(CONFIG_BT_CONN)
	default_conn = NULL;

	bt_conn_cb_register(&conn_callbacks);
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_PER_ADV_SYNC)
	bt_le_per_adv_sync_cb_register(&per_adv_sync_cb);
#endif /* CONFIG_BT_PER_ADV_SYNC */

#if defined(CONFIG_BT_SMP)
	bt_conn_auth_info_cb_register(&auth_info_cb);
#endif /* CONFIG_BT_SMP */
}

static void cmd_init(int argc, char *argv[])
{
	int err;
	bool sync = false;

	for (size_t argn = 1; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "no-settings-load")) {
			no_settings_load = true;
		} else if (!strcmp(arg, "sync")) {
			sync = true;
		} else {
			printf("%s", HELP_BT_INIT);
			return ;
		}
	}

	if (sync) {
		err = bt_enable(NULL);
		bt_ready(err);
	} else {
		err = bt_enable(bt_ready);
		if (err) {
			printf("Bluetooth init failed (err %d)\n",err);
		}
	}

	return ;
}

static void cmd_disable(int argc, char *argv[])
{
	bt_disable();
}

#ifdef CONFIG_SETTINGS
static void cmd_settings_load(int argc, char *argv[])
{
	int err;

	err = settings_load();
	if (err) {
		printf("Settings load failed (err %d)\n", err);
		return;
	}

	printf("Settings loaded\n");
	return ;
}
#endif

#if defined(CONFIG_BT_HCI)
static void cmd_hci_cmd(int argc, char *argv[])
{
	uint8_t ogf;
	uint16_t ocf;
	struct net_buf *buf = NULL, *rsp;
	int err;
	static uint8_t hex_data[HCI_CMD_MAX_PARAM];
	int hex_data_len;

	hex_data_len = 0;
	ogf = strtoul(argv[1], NULL, 16);
	ocf = strtoul(argv[2], NULL, 16);

	if (argc > 3) {
		size_t len;

		if (strlen(argv[3]) > 2 * HCI_CMD_MAX_PARAM) {
			printf("Data field too large\n");
			return ;
		}

		len = hex2bin(argv[3], strlen(argv[3]), &hex_data[hex_data_len],
			      sizeof(hex_data) - hex_data_len);
		if (!len) {
			printf("HCI command illegal data field\n");
			return ;
		}

		buf = bt_hci_cmd_create(BT_OP(ogf, ocf), len);
		net_buf_add_mem(buf, hex_data, len);
	}

	err = bt_hci_cmd_send_sync(BT_OP(ogf, ocf), buf, &rsp);
	if (err) {
		printf("HCI command failed (err %d)\n", err);
		return;
	} else {
		print_data_set(1,rsp->data, rsp->len);
		net_buf_unref(rsp);
	}

	return ;
}
#endif /* CONFIG_BT_HCI */

static void cmd_name(int argc, char *argv[])
{
	int err;

	if (argc < 2) {
		printf("Bluetooth Local Name: %s\n", bt_get_name());
		return ;
	}

	err = bt_set_name(argv[1]);
	if (err) {
		printf("Unable to set name %s (err %d)\n", argv[1],
			    err);
		return;
	}

	return ;
}

static void cmd_appearance(int argc, char *argv[])
{
	if (argc == 1) {
		printf("Bluetooth Appearance: 0x%04x\n", bt_get_appearance());
		return ;
	}

#if defined(CONFIG_BT_DEVICE_APPEARANCE_DYNAMIC)
	uint16_t app;
	int err;
	const char *val;

	val = argv[1];
	if (strlen(val) != 6 || strncmp(val, "0x", 2) ||
	    !hex2bin(&val[2], strlen(&val[2]), ((uint8_t *)&app), sizeof(app))) {
		printf("Argument must be 0x followed by exactly 4 hex digits.\n");
		return ;
	}

	app = sys_be16_to_cpu(app);

	err = bt_set_appearance(app);
	if (err) {
		printf("bt_set_appearance(0x%04x) failed with err %d\n", app, err);
		return;
	}
#else /* defined(CONFIG_BT_DEVICE_APPEARANCE_DYNAMIC) */
    printf("BT_DEVICE_APPEARANCE_DYNAMIC not support\n");
#endif
	return ;
}

static void cmd_id_create(int argc, char *argv[])
{
	char addr_str[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr;
	int err;

	if (argc > 1) {
		err = bt_addr_le_from_str(argv[1], "random", &addr);
		if (err) {
			printf("Invalid address\n");
		}
	} else {
		bt_addr_le_copy(&addr, BT_ADDR_LE_ANY);
	}

	err = bt_id_create(&addr, NULL);
	if (err < 0) {
		printf("Creating new ID failed (err %d)\n", err);
		return;
	}

	bt_addr_le_to_str(&addr, addr_str, sizeof(addr_str));
	printf("New identity (%d) created: %s\n", err, addr_str);

	return ;
}

static void cmd_id_reset(int argc, char *argv[])
{
	char addr_str[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr;
	uint8_t id;
	int err;

	if (argc < 2) {
		printf("Identity identifier not specified\n");
		return ;
	}

	id = strtol(argv[1], NULL, 10);

	if (argc > 2) {
		err = bt_addr_le_from_str(argv[2], "random", &addr);
		if (err) {
			printf("Invalid address");
			return;
		}
	} else {
		bt_addr_le_copy(&addr, BT_ADDR_LE_ANY);
	}

	err = bt_id_reset(id, &addr, NULL);
	if (err < 0) {
		printf("Resetting ID %u failed (err %d)\n", id, err);
		return;
	}

	bt_addr_le_to_str(&addr, addr_str, sizeof(addr_str));
	printf("Identity %u reset: %s\n", id, addr_str);

	return ;
}

static void cmd_id_delete(int argc, char *argv[])
{
	uint8_t id;
	int err;

	if (argc < 2) {
		printf("Identity identifier not specified\n");
		return ;
	}

	id = strtol(argv[1], NULL, 10);

	err = bt_id_delete(id);
	if (err < 0) {
		printf("Deleting ID %u failed (err %d)\n", id, err);
		return;
	}

	printf("Identity %u deleted\n", id);

	return ;
}

static void cmd_id_show(int argc, char *argv[])
{
	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
	size_t i, count = CONFIG_BT_ID_MAX;

	bt_id_get(addrs, &count);

	for (i = 0; i < count; i++) {
		char addr_str[BT_ADDR_LE_STR_LEN];

		bt_addr_le_to_str(&addrs[i], addr_str, sizeof(addr_str));
		printf("%s%u: %s\n", i == selected_id ? "*" : " ", i,
		      addr_str);
	}

	return ;
}

static void cmd_id_select(int argc, char *argv[])
{
	char addr_str[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
	size_t count = CONFIG_BT_ID_MAX;
	uint8_t id;

	id = strtol(argv[1], NULL, 10);

	bt_id_get(addrs, &count);
	if (count <= id) {
		printf("Invalid identity\n");
		return ;
	}

	bt_addr_le_to_str(&addrs[id], addr_str, sizeof(addr_str));
	printf("Selected identity: %s\n", addr_str);
	selected_id = id;

	return ;
}

#if defined(CONFIG_BT_OBSERVER)
static int cmd_active_scan_on( uint32_t options,
			      uint16_t timeout)
{
	int err;
	struct bt_le_scan_param param = {
			.type       = BT_LE_SCAN_TYPE_ACTIVE,
			.options    = BT_LE_SCAN_OPT_NONE,
			.interval   = BT_GAP_SCAN_FAST_INTERVAL,
			.window     = BT_GAP_SCAN_FAST_WINDOW,
			.timeout    = timeout, };

	param.options |= options;

	err = bt_le_scan_start(&param, NULL);
	if (err) {
		printf("Bluetooth set active scan failed \n"
		      "(err %d)", err);
		return err;
	} else {
		printf("Bluetooth active scan enabled\n");
	}

	return 0;
}

static int cmd_passive_scan_on( uint32_t options,
			       uint16_t timeout)
{
	struct bt_le_scan_param param = {
			.type       = BT_LE_SCAN_TYPE_PASSIVE,
			.options    = BT_LE_SCAN_OPT_NONE,
			.interval   = 0x10,
			.window     = 0x10,
			.timeout    = timeout, };
	int err;

	param.options |= options;

	err = bt_le_scan_start(&param, NULL);
	if (err) {
		printf("Bluetooth set passive scan failed \n"
			    "(err %d)", err);
		return err;
	} else {
		printf("Bluetooth passive scan enabled\n");
	}

	return 0;
}

static int cmd_scan_off(void)
{
	int err;

	err = bt_le_scan_stop();
	if (err) {
		printf("Stopping scanning failed (err %d)\n", err);
		return err;
	} else {
		printf("Scan successfully stopped\n");
	}

	return 0;
}

static void cmd_scan(int argc, char *argv[])
{
	const char *action;
	uint32_t options = 0;
	uint16_t timeout = 0;

	/* Parse duplicate filtering data */
	for (size_t argn = 2; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "dups")) {
			options |= BT_LE_SCAN_OPT_FILTER_DUPLICATE;
		} else if (!strcmp(arg, "nodups")) {
			options &= ~BT_LE_SCAN_OPT_FILTER_DUPLICATE;
		} else if (!strcmp(arg, "fal")) {
			options |= BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST;
		} else if (!strcmp(arg, "coded")) {
			options |= BT_LE_SCAN_OPT_CODED;
		} else if (!strcmp(arg, "no-1m")) {
			options |= BT_LE_SCAN_OPT_NO_1M;
		} else if (!strcmp(arg, "timeout")) {
			if (++argn == argc) {
				///printf("%s", HELP_BT_INIT);;
				///return ;
				return;
			}

			timeout = strtoul(argv[argn], NULL, 16);
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}
	}

	action = argv[1];
	if (!strcmp(action, "on")) {
		cmd_active_scan_on(options, timeout);
	} else if (!strcmp(action, "off")) {
		cmd_scan_off();
	} else if (!strcmp(action, "passive")) {
		cmd_passive_scan_on(options, timeout);
	} else {
		printf("%s", HELP_BT_INIT);;
		return ;
	}

	return ;
}

static void cmd_scan_verbose_output(int argc, char *argv[])
{
	const char *verbose_state;

	verbose_state = argv[1];
	if (!strcmp(verbose_state, "on")) {
		scan_verbose_output = true;
	} else if (!strcmp(verbose_state, "off")) {
		scan_verbose_output = false;
	} else {
		printf("%s", HELP_BT_INIT);;
		return ;
	}

	return ;
}

static void cmd_scan_filter_set_name(int argc,
				    char *argv[])
{
	const char *name_arg = NULL; 

    if(argc>1) {
        name_arg = argv[1];
    	if (strlen(name_arg) >= sizeof(scan_filter.name)) {
    		printf("Name is too long (max %d): %s\n",
    			    sizeof(scan_filter.name), name_arg);
    		return ;
    	}

    	strcpy(scan_filter.name, name_arg);
    	scan_filter.name_set = true;
    }else
    {
        (void)memset(scan_filter.name, 0, sizeof(scan_filter.name));
	    scan_filter.name_set = false;
    }

	return ;
}

static void cmd_scan_filter_set_addr(int argc,
				    char *argv[])
{
	const size_t max_cpy_len = sizeof(scan_filter.addr) - 1;
	const char *addr_arg = argv[1];

	/* Validate length including null terminator. */
	if (strlen(addr_arg) > max_cpy_len) {
		printf("Invalid address string: %s\n",
			    addr_arg);
		return ;
	}

	/* Validate input to check if valid (subset of) BT address */
	for (size_t i = 0; i < strlen(addr_arg); i++) {
		const char c = addr_arg[i];
		uint8_t tmp;

		if (c != ':' && char2hex(c, &tmp) < 0) {
			printf("Invalid address string: %s\n",
					addr_arg);
			return ;
		}
	}

	strncpy(scan_filter.addr, addr_arg, max_cpy_len);
	scan_filter.addr[max_cpy_len] = '\0'; /* ensure NULL termination */
	scan_filter.addr_set = true;

	return ;
}

static void cmd_scan_filter_set_rssi(int argc, char *argv[])
{
	long rssi = INT8_MIN;

	if (argc > 1) {
        rssi = strtol(argv[1], NULL, 10);
		if (IN_RANGE(rssi, INT8_MIN, INT8_MAX)) {
			scan_filter.rssi = (int8_t)rssi;
			scan_filter.rssi_set = true;
			printf("RSSI cutoff set at %d dB\n", scan_filter.rssi);

			return ;
		}

		printf("value out of bounds (%d to %d)\n", INT8_MIN, INT8_MAX);
	}else
    {
        scan_filter.rssi = (int8_t)rssi;
        scan_filter.rssi_set = false;
    }

	return ;
}

static void cmd_scan_filter_set_pa_interval(int argc,
					   char *argv[])
{
	unsigned long pa_interval;

	if (argc > 1) {
        pa_interval = strtoul(argv[1], NULL, 10);
		if (IN_RANGE(pa_interval,
			     BT_GAP_PER_ADV_MIN_INTERVAL,
			     BT_GAP_PER_ADV_MAX_INTERVAL)) {
			scan_filter.pa_interval = (uint16_t)pa_interval;
			scan_filter.pa_interval_set = true;
			printf("PA interval cutoff set at %u\n",
				    scan_filter.pa_interval);

			return ;
		}

		printf("value out of bounds (%d to %d)\n",
			    BT_GAP_PER_ADV_MIN_INTERVAL,
			    BT_GAP_PER_ADV_MAX_INTERVAL);

	}else
    {
        scan_filter.pa_interval_set = false;
    }   

	return ;
}

static void cmd_scan_filter_clear_all(int argc,
				     char *argv[])
{
	(void)memset(&scan_filter, 0, sizeof(scan_filter));

	return ;
}

static void cmd_scan_filter_clear_name(int argc,
				      char *argv[])
{
	(void)memset(scan_filter.name, 0, sizeof(scan_filter.name));
	scan_filter.name_set = false;

	return ;
}

static void cmd_scan_filter_clear_addr(int argc,
				      char *argv[])
{
	(void)memset(scan_filter.addr, 0, sizeof(scan_filter.addr));
	scan_filter.addr_set = false;

	return ;
}

#endif /* CONFIG_BT_OBSERVER */

#if defined(CONFIG_BT_BROADCASTER)
static ssize_t ad_init(struct bt_data *data_array, const size_t data_array_size,
		       const atomic_t *adv_options)
{
	const bool discoverable = atomic_test_bit(adv_options, SHELL_ADV_OPT_DISCOVERABLE);
	const bool appearance = atomic_test_bit(adv_options, SHELL_ADV_OPT_APPEARANCE);
#if defined(CONFIG_BT_EXT_ADV) && (CONFIG_BT_EXT_ADV == 1)
	const bool adv_ext = atomic_test_bit(adv_options, SHELL_ADV_OPT_EXT_ADV);
#endif

	static uint8_t ad_flags;
	size_t ad_len = 0;

	/* Set BR/EDR Not Supported if LE-only device */
	ad_flags = IS_ENABLED(CONFIG_BT_BREDR) ? 0 : BT_LE_AD_NO_BREDR;

	if (discoverable) {
		/* A privacy-enabled Set Member should advertise RSI values only when in
		 * the GAP Limited Discoverable mode.
		 */
		if (IS_ENABLED(CONFIG_BT_PRIVACY) &&
		    IS_ENABLED(CONFIG_BT_CSIP_SET_MEMBER) /*&&
		    svc_inst != NULL*/) {
			ad_flags |= BT_LE_AD_LIMITED;
		} else {
			ad_flags |= BT_LE_AD_GENERAL;
		}
	}

	if (ad_flags != 0) {
		__ASSERT(data_array_size > ad_len, "No space for AD_FLAGS");
		data_array[ad_len].type = BT_DATA_FLAGS;
		data_array[ad_len].data_len = sizeof(ad_flags);
		data_array[ad_len].data = &ad_flags;
		ad_len++;
	}

	if (appearance) {
		const uint16_t appearance2 = bt_get_appearance();
		static uint8_t appearance_data[sizeof(appearance2)];

		__ASSERT(data_array_size > ad_len, "No space for appearance");
		sys_put_le16(appearance2, appearance_data);
		data_array[ad_len].type = BT_DATA_GAP_APPEARANCE;
		data_array[ad_len].data_len = sizeof(appearance_data);
		data_array[ad_len].data = appearance_data;
		ad_len++;
	}
#if defined(CONFIG_BT_CSIP_SET_MEMBER) && (CONFIG_BT_CSIP_SET_MEMBER == 1)
	if (IS_ENABLED(CONFIG_BT_CSIP_SET_MEMBER)) {
		ssize_t csis_ad_len;

		csis_ad_len = csis_ad_data_add(&data_array[ad_len],
					       data_array_size - ad_len, discoverable);
		if (csis_ad_len < 0) {
			printf("Failed to add CSIS data (err %d)\n", csis_ad_len);
			return ad_len;
		}

		ad_len += csis_ad_len;
	}
#endif

#if defined(CONFIG_BT_AUDIO) && (CONFIG_BT_AUDIO == 1)
	if (IS_ENABLED(CONFIG_BT_AUDIO) && IS_ENABLED(CONFIG_BT_EXT_ADV) && adv_ext) {
		const bool connectable = atomic_test_bit(adv_options, SHELL_ADV_OPT_CONNECTABLE);
		ssize_t audio_ad_len;

		audio_ad_len = audio_ad_data_add(&data_array[ad_len], data_array_size - ad_len,
						 discoverable, connectable);
		if (audio_ad_len < 0) {
			return audio_ad_len;
		}

		ad_len += audio_ad_len;
	}
#endif

	return ad_len;
}

static void cmd_advertise(int argc, char *argv[])
{
	struct bt_le_adv_param param = {};
	struct bt_data ad[3];
	bool discoverable = true;
	bool appearance = false;
	ssize_t ad_len;
	int err;

	if (!strcmp(argv[1], "off")) {
		if (bt_le_adv_stop() < 0) {
			printf("Failed to stop advertising\n");
			return ;
		} else {
			printf("Advertising stopped\n");
		}

		return ;
	}

	param.id = selected_id;
	param.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
	param.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;

	if (!strcmp(argv[1], "on")) {
		param.options = (BT_LE_ADV_OPT_CONNECTABLE |
				 BT_LE_ADV_OPT_USE_NAME);
	} else if (!strcmp(argv[1], "scan")) {
		param.options = BT_LE_ADV_OPT_USE_NAME;
	} else if (!strcmp(argv[1], "nconn")) {
		param.options = 0U;
	} else {
		goto fail;
	}

	for (size_t argn = 2; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "discov")) {
			discoverable = true;
		} else if (!strcmp(arg, "non_discov")) {
			discoverable = false;
		} else if (!strcmp(arg, "appearance")) {
			appearance = true;
		} else if (!strcmp(arg, "fal")) {
			param.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
			param.options |= BT_LE_ADV_OPT_FILTER_CONN;
		} else if (!strcmp(arg, "fal-scan")) {
			param.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
		} else if (!strcmp(arg, "fal-conn")) {
			param.options |= BT_LE_ADV_OPT_FILTER_CONN;
		} else if (!strcmp(arg, "identity")) {
			param.options |= BT_LE_ADV_OPT_USE_IDENTITY;
		} else if (!strcmp(arg, "no-name")) {
			param.options &= ~BT_LE_ADV_OPT_USE_NAME;
		} else if (!strcmp(arg, "name-ad")) {
			param.options |= BT_LE_ADV_OPT_USE_NAME;
			param.options |= BT_LE_ADV_OPT_FORCE_NAME_IN_AD;
		} else if (!strcmp(arg, "one-time")) {
			param.options |= BT_LE_ADV_OPT_ONE_TIME;
		} else if (!strcmp(arg, "disable-37")) {
			param.options |= BT_LE_ADV_OPT_DISABLE_CHAN_37;
		} else if (!strcmp(arg, "disable-38")) {
			param.options |= BT_LE_ADV_OPT_DISABLE_CHAN_38;
		} else if (!strcmp(arg, "disable-39")) {
			param.options |= BT_LE_ADV_OPT_DISABLE_CHAN_39;
		} else {
			goto fail;
		}
	}

	atomic_clear(adv_opt);
	atomic_set_bit_to(adv_opt, SHELL_ADV_OPT_CONNECTABLE,
			  (param.options & BT_LE_ADV_OPT_CONNECTABLE) > 0);
	atomic_set_bit_to(adv_opt, SHELL_ADV_OPT_DISCOVERABLE, discoverable);
	atomic_set_bit_to(adv_opt, SHELL_ADV_OPT_APPEARANCE, appearance);

	ad_len = ad_init(ad, ARRAY_SIZE(ad), adv_opt);
	if (ad_len < 0) {
		return ;
	}

	err = bt_le_adv_start(&param, ad_len > 0 ? ad : NULL, ad_len, NULL, 0);
	if (err < 0) {
		printf("Failed to start advertising (err %d)\n",
			    err);
		return ;
	} else {
		printf("Advertising started\n");
	}

	return ;

fail:
	printf("%s", HELP_BT_INIT);;
	return ;
}

#if defined(CONFIG_BT_PERIPHERAL)
static void cmd_directed_adv(int argc, char *argv[])
{
	int err;
	bt_addr_le_t addr;
	struct bt_le_adv_param param;

	err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	param = *BT_LE_ADV_CONN_DIR(&addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	for (size_t argn = 3; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "low")) {
			param.options |= BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY;
			param.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;
			param.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
		} else if (!strcmp(arg, "identity")) {
			param.options |= BT_LE_ADV_OPT_USE_IDENTITY;
		} else if (!strcmp(arg, "dir-rpa")) {
			param.options |= BT_LE_ADV_OPT_DIR_ADDR_RPA;
		} else if (!strcmp(arg, "disable-37")) {
			param.options |= BT_LE_ADV_OPT_DISABLE_CHAN_37;
		} else if (!strcmp(arg, "disable-38")) {
			param.options |= BT_LE_ADV_OPT_DISABLE_CHAN_38;
		} else if (!strcmp(arg, "disable-39")) {
			param.options |= BT_LE_ADV_OPT_DISABLE_CHAN_39;
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}
	}

	err = bt_le_adv_start(&param, NULL, 0, NULL, 0);
	if (err) {
		printf("Failed to start directed advertising (%d)\n",
			    err);
		return ;
	} else {
		printf("Started directed advertising\n");
	}

	return ;
}
#endif /* CONFIG_BT_PERIPHERAL */

#if defined(CONFIG_BT_EXT_ADV)
static bool adv_param_parse(int argc, char *argv[],
			   struct bt_le_adv_param *param)
{
	memset(param, 0, sizeof(struct bt_le_adv_param));

	if (!strcmp(argv[1], "conn-scan")) {
		param->options |= BT_LE_ADV_OPT_CONNECTABLE;
		param->options |= BT_LE_ADV_OPT_SCANNABLE;
	} else if (!strcmp(argv[1], "conn-nscan")) {
		param->options |= BT_LE_ADV_OPT_CONNECTABLE;
	} else if (!strcmp(argv[1], "nconn-scan")) {
		param->options |= BT_LE_ADV_OPT_SCANNABLE;
	} else if (!strcmp(argv[1], "nconn-nscan")) {
		/* Acceptable option, nothing to do */
	} else {
		return false;
	}

	for (size_t argn = 2; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "ext-adv")) {
			param->options |= BT_LE_ADV_OPT_EXT_ADV;
		} else if (!strcmp(arg, "coded")) {
			param->options |= BT_LE_ADV_OPT_CODED;
		} else if (!strcmp(arg, "no-2m")) {
			param->options |= BT_LE_ADV_OPT_NO_2M;
		} else if (!strcmp(arg, "anon")) {
			param->options |= BT_LE_ADV_OPT_ANONYMOUS;
		} else if (!strcmp(arg, "tx-power")) {
			param->options |= BT_LE_ADV_OPT_USE_TX_POWER;
		} else if (!strcmp(arg, "scan-reports")) {
			param->options |= BT_LE_ADV_OPT_NOTIFY_SCAN_REQ;
		} else if (!strcmp(arg, "fal")) {
			param->options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
			param->options |= BT_LE_ADV_OPT_FILTER_CONN;
		} else if (!strcmp(arg, "fal-scan")) {
			param->options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
		} else if (!strcmp(arg, "fal-conn")) {
			param->options |= BT_LE_ADV_OPT_FILTER_CONN;
		} else if (!strcmp(arg, "identity")) {
			param->options |= BT_LE_ADV_OPT_USE_IDENTITY;
		} else if (!strcmp(arg, "name")) {
			param->options |= BT_LE_ADV_OPT_USE_NAME;
		} else if (!strcmp(arg, "name-ad")) {
			param->options |= BT_LE_ADV_OPT_USE_NAME;
			param->options |= BT_LE_ADV_OPT_FORCE_NAME_IN_AD;
		} else if (!strcmp(arg, "low")) {
			param->options |= BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY;
		} else if (!strcmp(arg, "dir-rpa")) {
			param->options |= BT_LE_ADV_OPT_DIR_ADDR_RPA;
		} else if (!strcmp(arg, "disable-37")) {
			param->options |= BT_LE_ADV_OPT_DISABLE_CHAN_37;
		} else if (!strcmp(arg, "disable-38")) {
			param->options |= BT_LE_ADV_OPT_DISABLE_CHAN_38;
		} else if (!strcmp(arg, "disable-39")) {
			param->options |= BT_LE_ADV_OPT_DISABLE_CHAN_39;
		} else if (!strcmp(arg, "directed")) {
			static bt_addr_le_t addr;

			if ((argn + 2) >= argc) {
				return false;
			}

			if (bt_addr_le_from_str(argv[argn + 1], argv[argn + 2],
						&addr)) {
				return false;
			}

			param->peer = &addr;
			argn += 2;
		} else {
			return false;
		}
	}

	param->id = selected_id;
	param->sid = 0;
	if (param->peer &&
	    !(param->options & BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY)) {
		param->interval_min = 0;
		param->interval_max = 0;
	} else {
		param->interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
		param->interval_max = BT_GAP_ADV_FAST_INT_MAX_2;
	}

	return true;
}

static int cmd_adv_create(int argc, char *argv[])
{
	struct bt_le_adv_param param;
	struct bt_le_ext_adv *adv;
	uint8_t adv_index;
	int err;

	if (!adv_param_parse(argc, argv, &param)) {
		printf("%s", HELP_BT_INIT);;
		return ;
	}

	err = bt_le_ext_adv_create(&param, &adv_callbacks, &adv);
	if (err) {
		printf("Failed to create advertiser set (%d)\n", err);
		return ;
	}

	adv_index = bt_le_ext_adv_get_index(adv);
	adv_sets[adv_index] = adv;

	atomic_clear(adv_set_opt[adv_index]);
	atomic_set_bit_to(adv_set_opt[adv_index], SHELL_ADV_OPT_CONNECTABLE,
			  (param.options & BT_LE_ADV_OPT_CONNECTABLE) > 0);
	atomic_set_bit_to(adv_set_opt[adv_index], SHELL_ADV_OPT_EXT_ADV,
			  (param.options & BT_LE_ADV_OPT_EXT_ADV) > 0);

	printf("Created adv id: %d, adv: %p\n", adv_index, adv);

	return ;
}

static void cmd_adv_param(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	struct bt_le_adv_param param;
	int err;

	if (!adv_param_parse(argc, argv, &param)) {
		printf("%s", HELP_BT_INIT);;
		return ;
	}

	err = bt_le_ext_adv_update_param(adv, &param);
	if (err) {
		printf("Failed to update advertiser set (%d)\n", err);
		return ;
	}

	return ;
}

static void cmd_adv_data(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	static uint8_t hex_data[1650];
	bool appearance = false;
	struct bt_data *data;
	struct bt_data ad[AD_SIZE];
	struct bt_data sd[AD_SIZE];
	size_t hex_data_len;
	size_t ad_len = 0;
	size_t sd_len = 0;
	size_t len = 0;
	bool discoverable = false;
	size_t *data_len;
	int err;

	if (!adv) {
		return ;
	}

	hex_data_len = 0;
	data = ad;
	data_len = &ad_len;

	for (size_t argn = 1; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (strcmp(arg, "scan-response") &&
		    *data_len == ARRAY_SIZE(ad)) {
			/* Maximum entries limit reached. */
			printf("Failed to set advertising data: "
					   "Maximum entries limit reached\n");

			return ;
		}

		if (!strcmp(arg, "discov")) {
			discoverable = true;
		} else if (!strcmp(arg, "non_discov")) {
			discoverable = false;
		} else if (!strcmp(arg, "appearance")) {
			appearance = true;
		} else if (!strcmp(arg, "scan-response")) {
			if (data == sd) {
				printf("Failed to set advertising data: "
						   "duplicate scan-response option\n");
				return ;
			}

			data = sd;
			data_len = &sd_len;
		} else {
			len = hex2bin(arg, strlen(arg), &hex_data[hex_data_len],
				      sizeof(hex_data) - hex_data_len);

			if (!len || (len - 1) != (hex_data[hex_data_len])) {
				printf("Failed to set advertising data: "
						   "malformed hex data\n");
				return ;
			}

			data[*data_len].type = hex_data[hex_data_len + 1];
			data[*data_len].data_len = len - 2;
			data[*data_len].data = &hex_data[hex_data_len + 2];
			(*data_len)++;
			hex_data_len += len;
		}
	}

	atomic_set_bit_to(adv_set_opt[selected_adv], SHELL_ADV_OPT_DISCOVERABLE, discoverable);
	atomic_set_bit_to(adv_set_opt[selected_adv], SHELL_ADV_OPT_APPEARANCE,
			  appearance);

	len = ad_init(&data[*data_len], AD_SIZE - *data_len, adv_set_opt[selected_adv]);
	if (len < 0) {
		printf("Failed to initialize stack advertising data\n");

		return ;
	}

	if (data == ad) {
		ad_len += len;
	} else {
		sd_len += len;
	}

	err = bt_le_ext_adv_set_data(adv, ad_len > 0 ? ad : NULL, ad_len,
					  sd_len > 0 ? sd : NULL, sd_len);
	if (err) {
		printf("Failed to set advertising set data (%d)\n",
			    err);
		return ;
	}

	return ;
}

static void cmd_adv_start(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	struct bt_le_ext_adv_start_param param;
	uint8_t num_events = 0;
	int32_t timeout = 0;
	int err;

	if (!adv) {
		printf("Advertiser[%d] not created\n", selected_adv);
		return ;
	}

	for (size_t argn = 1; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "timeout")) {
			if (++argn == argc) {
				goto fail_show_help;
			}

			timeout = strtoul(argv[argn], NULL, 16);
		}

		if (!strcmp(arg, "num-events")) {
			if (++argn == argc) {
				goto fail_show_help;
			}

			num_events = strtoul(argv[argn], NULL, 16);
		}
	}

	param.timeout = timeout;
	param.num_events = num_events;

	err = bt_le_ext_adv_start(adv, &param);
	if (err) {
		printf("Failed to start advertising set (%d)\n", err);
		return ;
	}

	printf("Advertiser[%d] %p set started\n", selected_adv, adv);
	return ;

fail_show_help:
	printf("%s", HELP_BT_INIT);;
	return ;
}

static void cmd_adv_stop(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	int err;

	if (!adv) {
		printf("Advertiser[%d] not created\n", selected_adv);
		return ;
	}

	err = bt_le_ext_adv_stop(adv);
	if (err) {
		printf("Failed to stop advertising set (%d)\n", err);
		return ;
	}

	printf("Advertiser set stopped\n");
	return ;
}

static void cmd_adv_delete(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	int err;

	if (!adv) {
		printf("Advertiser[%d] not created\n", selected_adv);
		return ;
	}

	err = bt_le_ext_adv_delete(adv);
	if (err) {
		printf("Failed to delete advertiser set\n");
		return ;
	}

	adv_sets[selected_adv] = NULL;
	return ;
}

static void cmd_adv_select(int argc, char *argv[])
{
	if (argc == 2) {
		uint8_t id = strtol(argv[1], NULL, 10);

		if (!(id < ARRAY_SIZE(adv_sets))) {
			return ;
		}

		selected_adv = id;
		return ;
	}

	for (int i = 0; i < ARRAY_SIZE(adv_sets); i++) {
		if (adv_sets[i]) {
			printf("Advertiser[%d] %p\n", i, adv_sets[i]);
		}
	}

	return ;
}

static void cmd_adv_info(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	struct bt_le_ext_adv_info info;
	int err;

	if (!adv) {
		return ;
	}

	err = bt_le_ext_adv_get_info(adv, &info);
	if (err) {
		printf("OOB data failed\n");
		return ;
	}

	printf("Advertiser[%d] %p\n", selected_adv, adv);
	printf("Id: %d, TX power: %d dBm\n", info.id, info.tx_power);
	print_le_addr("Address", info.addr);

	return ;
}

#if defined(CONFIG_BT_PERIPHERAL)
static void cmd_adv_oob(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	int err;

	if (!adv) {
		return ;
	}

	err = bt_le_ext_adv_oob_get_local(adv, &oob_local);
	if (err) {
		printf("OOB data failed\n");
		return ;
	}

	print_le_oob(sh, &oob_local);

	return ;
}
#endif /* CONFIG_BT_PERIPHERAL */

#if defined(CONFIG_BT_PRIVACY)
static void cmd_adv_rpa_expire(int argc, char *argv[])
{
	if (!strcmp(argv[1], "on")) {
		atomic_clear_bit(adv_set_opt[selected_adv], SHELL_ADV_OPT_KEEP_RPA);
		printf("RPA will expire on next timeout\n");
	} else if (!strcmp(argv[1], "off")) {
		atomic_set_bit(adv_set_opt[selected_adv], SHELL_ADV_OPT_KEEP_RPA);
		printf("RPA will not expire on RPA timeout\n");
	} else {
		printf("Invalid argument: %s\n", argv[1]);
		return ;
	}

	return ;
}
#endif

#if defined(CONFIG_BT_PER_ADV)
static void cmd_per_adv(int argc, char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];

	if (!adv) {
		printf("No extended advertisement set selected\n");
		return ;
	}

	if (!strcmp(argv[1], "off")) {
		if (bt_le_per_adv_stop(adv) < 0) {
			printf(
				    "Failed to stop periodic advertising\n");
		} else {
			printf("Periodic advertising stopped\n");
		}
	} else if (!strcmp(argv[1], "on")) {
		if (bt_le_per_adv_start(adv) < 0) {
			printf(
				    "Failed to start periodic advertising\n");
		} else {
			printf("Periodic advertising started\n");
		}
	} else {
		printf("Invalid argument: %s\n", argv[1]);
		return ;
	}

	return ;
}

static void cmd_per_adv_param(int argc,
			     char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	struct bt_le_per_adv_param param;
	int err;

	if (!adv) {
		printf("No extended advertisement set selected\n");
		return ;
	}

	if (argc > 1) {
		param.interval_min = strtol(argv[1], NULL, 16);
	} else {
		param.interval_min = BT_GAP_ADV_SLOW_INT_MIN;
	}

	if (argc > 2) {
		param.interval_max = strtol(argv[2], NULL, 16);
	} else {
		param.interval_max = param.interval_min * 1.2;

	}

	if (param.interval_min > param.interval_max) {
		printf(
			    "Min interval shall be less than max interval\n");
		return ;
	}

	if (argc > 3 && !strcmp(argv[3], "tx-power")) {
		param.options = BT_LE_ADV_OPT_USE_TX_POWER;
	} else {
		param.options = 0;
	}

	err = bt_le_per_adv_set_param(adv, &param);
	if (err) {
		printf("Failed to set periodic advertising "
			    "parameters (%d)\n", err);
		return ;
	}

	return ;
}

static ssize_t pa_ad_init(struct bt_data *data_array,
			  const size_t data_array_size)
{
	size_t ad_len = 0;

	if (IS_ENABLED(CONFIG_BT_AUDIO)) {
		ssize_t audio_pa_ad_len;

		audio_pa_ad_len = audio_pa_data_add(&data_array[ad_len],
						    data_array_size - ad_len);
		if (audio_pa_ad_len < 0U) {
			return audio_pa_ad_len;
		}

		ad_len += audio_pa_ad_len;
	}

	return ad_len;
}

static void cmd_per_adv_data(int argc,
			    char *argv[])
{
	struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	static uint8_t hex_data[256];
	static struct bt_data ad[2U];
	ssize_t stack_ad_len;
	uint8_t ad_len = 0;
	int err;

	if (!adv) {
		printf("No extended advertisement set selected\n");
		return ;
	}

	if (argc > 1) {
		size_t hex_len = 0U;

		(void)memset(hex_data, 0, sizeof(hex_data));
		hex_len = hex2bin(argv[1U], strlen(argv[1U]), hex_data,
				  sizeof(hex_data));

		if (hex_len == 0U) {
			printf("Could not parse adv data\n");

			return ;
		}

		ad[ad_len].data_len = hex_data[0U];
		ad[ad_len].type = hex_data[1U];
		ad[ad_len].data = &hex_data[2U];
		ad_len++;
	}

	stack_ad_len = pa_ad_init(&ad[ad_len], ARRAY_SIZE(ad) - ad_len);
	if (stack_ad_len < 0) {
		printf("Failed to get stack PA data\n");

		return ;
	}
	ad_len += stack_ad_len;

	err = bt_le_per_adv_set_data(adv, ad, ad_len);
	if (err) {
		printf(
			    "Failed to set periodic advertising data (%d)\n",
			    err);
		return ;
	}

	return ;
}
#endif /* CONFIG_BT_PER_ADV */
#endif /* CONFIG_BT_EXT_ADV */
#endif /* CONFIG_BT_BROADCASTER */

#if defined(CONFIG_BT_PER_ADV_SYNC)

static void cmd_per_adv_sync_create(int argc,
				   char *argv[])
{
	struct bt_le_per_adv_sync *per_adv_sync = per_adv_syncs[selected_per_adv_sync];
	int err;
	struct bt_le_per_adv_sync_param create_params = { 0 };
	uint32_t options = 0;

	if (per_adv_sync != NULL) {
		printf("Selected per-adv-sync is not NULL\n");
		return ;
	}

	err = bt_addr_le_from_str(argv[1], argv[2], &create_params.addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	/* Default values */
	create_params.timeout = 1000; /* 10 seconds */
	create_params.skip = 10;

	create_params.sid = strtol(argv[3], NULL, 16);

	for (int j = 4; j < argc; j++) {
		if (!strcmp(argv[j], "aoa")) {
			options |= BT_LE_PER_ADV_SYNC_OPT_DONT_SYNC_AOA;
		} else if (!strcmp(argv[j], "aod_1us")) {
			options |= BT_LE_PER_ADV_SYNC_OPT_DONT_SYNC_AOD_1US;
		} else if (!strcmp(argv[j], "aod_2us")) {
			options |= BT_LE_PER_ADV_SYNC_OPT_DONT_SYNC_AOD_2US;
		} else if (!strcmp(argv[j], "only_cte")) {
			options |=
				BT_LE_PER_ADV_SYNC_OPT_SYNC_ONLY_CONST_TONE_EXT;
		} else if (!strcmp(argv[j], "timeout")) {
			if (++j == argc) {
				printf("%s", HELP_BT_INIT);;
				return ;
			}

			create_params.timeout = strtoul(argv[j], NULL, 16);
		} else if (!strcmp(argv[j], "skip")) {
			if (++j == argc) {
				printf("%s", HELP_BT_INIT);;
				return ;
			}

			create_params.skip = strtoul(argv[j], NULL, 16);
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}

		/* TODO: add support to parse using the per adv list */
	}

	create_params.options = options;

	err = bt_le_per_adv_sync_create(&create_params, &per_adv_syncs[selected_per_adv_sync]);
	if (err) {
		printf("Per adv sync failed (%d)\n", err);
	} else {
		printf("Per adv sync pending\n");
	}

	return ;
}

static void cmd_per_adv_sync_delete(int argc,
				   char *argv[])
{
	struct bt_le_per_adv_sync *per_adv_sync = per_adv_syncs[selected_per_adv_sync];
	int err;

	if (!per_adv_sync) {
		printf("Selected per-adv-sync is NULL\n");
		return ;
	}

	err = bt_le_per_adv_sync_delete(per_adv_sync);

	if (err) {
		printf("Per adv sync delete failed (%d)\n", err);
	} else {
		printf("Per adv sync deleted\n");
		per_adv_syncs[selected_per_adv_sync] = NULL;
	}

	return ;
}

static void cmd_per_adv_sync_select(int argc, char *argv[])
{
	if (argc == 2) {
		unsigned long id;
		int err = 0;

		id = strtoul(argv[1], NULL, 10);
		if (err != 0) {
			printf("Could not parse id: %d\n", err);
			return ;
		}

		if (id > ARRAY_SIZE(adv_sets)) {
			printf("Invalid id: %lu", id);
			return ;
		}

		selected_per_adv_sync = id;
		return ;
	}

	for (size_t i = 0U; i < ARRAY_SIZE(adv_sets); i++) {
		if (adv_sets[i]) {
			printf("PER_ADV_SYNC[%d] %p\n", i, adv_sets[i]);
		}
	}

	return ;
}

#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER)
static void cmd_past_subscribe(int argc,
			      char *argv[])
{
	struct bt_le_per_adv_sync_transfer_param param;
	int err;
	int i = 0;
	bool global = true;

	if (i == ARRAY_SIZE(per_adv_syncs)) {
		printf("Cannot create more per adv syncs\n");
		return ;
	}

	/* Default values */
	param.options = 0;
	param.timeout = 1000; /* 10 seconds */
	param.skip = 10;

	for (int j = 1; j < argc; j++) {
		if (!strcmp(argv[j], "aoa")) {
			param.options |=
				BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOA;
		} else if (!strcmp(argv[j], "aod_1us")) {
			param.options |=
				BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOD_1US;
		} else if (!strcmp(argv[j], "aod_2us")) {
			param.options |=
				BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOD_2US;
		} else if (!strcmp(argv[j], "only_cte")) {
			param.options |=
				BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_ONLY_CTE;
		} else if (!strcmp(argv[j], "timeout")) {
			if (++j == argc) {
				printf("%s", HELP_BT_INIT);;
				return ;
			}

			param.timeout = strtoul(argv[j], NULL, 16);
		} else if (!strcmp(argv[j], "skip")) {
			if (++j == argc) {
				printf("%s", HELP_BT_INIT);;
				return ;
			}

			param.skip = strtoul(argv[j], NULL, 16);
		} else if (!strcmp(argv[j], "conn")) {
			if (!default_conn) {
				printf("Not connected\n");
				return ;
			}
			global = false;
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}
	}

	bt_le_per_adv_sync_cb_register(&per_adv_sync_cb);

	err = bt_le_per_adv_sync_transfer_subscribe(
		global ? NULL : default_conn, &param);

	if (err) {
		printf("PAST subscribe failed (%d)\n", err);
	} else {
		printf("Subscribed to PAST\n");
	}

	return ;
}

static void cmd_past_unsubscribe(int argc,
				char *argv[])
{
	int err;

	if (argc > 1) {
		if (!strcmp(argv[1], "conn")) {
			if (default_conn) {
				err =
					bt_le_per_adv_sync_transfer_unsubscribe(
						default_conn);
			} else {
				printf("Not connected\n");
				return ;
			}
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}
	} else {
		err = bt_le_per_adv_sync_transfer_unsubscribe(NULL);
	}

	if (err) {
		printf("PAST unsubscribe failed (%d)\n", err);
	}

	return ;
}
#endif /* CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER */

#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER)
static void cmd_per_adv_sync_transfer(int argc,
				     char *argv[])
{
	int err;
	int index;
	struct bt_le_per_adv_sync *per_adv_sync;

	if (argc > 1) {
		index = strtol(argv[1], NULL, 10);
	} else {
		index = 0;
	}

	if (index >= ARRAY_SIZE(per_adv_syncs)) {
		printf("Maximum index is %d but %d was requested\n",
			    ARRAY_SIZE(per_adv_syncs) - 1, index);
	}

	per_adv_sync = per_adv_syncs[index];
	if (!per_adv_sync) {
		return ;
	}

	err = bt_le_per_adv_sync_transfer(per_adv_sync, default_conn, 0);
	if (err) {
		printf("Periodic advertising sync transfer failed (%d)\n", err);
	}

	return ;
}
#endif /* CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER */
#endif /* CONFIG_BT_PER_ADV_SYNC */

#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER) && defined(CONFIG_BT_PER_ADV)
static void cmd_per_adv_set_info_transfer(int argc,
					 char *argv[])
{
	const struct bt_le_ext_adv *adv = adv_sets[selected_adv];
	int err;

	if (default_conn == NULL) {
		printf("at least, one connection is required\n");
		return ;
	}

	err = bt_le_per_adv_set_info_transfer(adv, default_conn, 0U);
	if (err) {
		printf("Periodic advertising sync transfer failed (%d)\n", err);
	}

	return ;
}
#endif /* CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER && CONFIG_BT_PER_ADV */

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
static void cmd_connect_le(int argc, char *argv[])
{
	int err;
	bt_addr_le_t addr;
	struct bt_conn *conn;
	uint32_t options = 0;

	/* When no arguments are specified, connect to the last scanned device. */
	if (argc == 1) {
		if (auto_connect.addr_set) {
			bt_addr_le_copy(&addr, &auto_connect.addr);
		} else {
			printf("No connectable adv stored, please trigger a scan first.\n");
			printf("%s", HELP_BT_INIT);;

			return ;
		}
	} else {
		err = bt_addr_le_from_str(argv[1], argv[2], &addr);
		if (err) {
			printf("Invalid peer address (err %d)\n", err);
			return ;
		}
	}

#if defined(CONFIG_BT_EXT_ADV)
	for (size_t argn = 3; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "coded")) {
			options |= BT_CONN_LE_OPT_CODED;
		} else if (!strcmp(arg, "no-1m")) {
			options |= BT_CONN_LE_OPT_NO_1M;
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}
	}
#endif /* defined(CONFIG_BT_EXT_ADV) */

	struct bt_conn_le_create_param *create_params =
		BT_CONN_LE_CREATE_PARAM(options,
					BT_GAP_SCAN_FAST_INTERVAL,
					BT_GAP_SCAN_FAST_INTERVAL);

	err = bt_conn_le_create(&addr, create_params, BT_LE_CONN_PARAM_DEFAULT,
				&conn);
	if (err) {
		printf("Connection failed (%d)\n", err);
		return ;
	} else {

		printf("Connection pending\n");

		/* unref connection obj in advance as app user */
		bt_conn_unref(conn);
	}

	return ;
}

#if !defined(CONFIG_BT_FILTER_ACCEPT_LIST)
static void cmd_auto_conn(int argc, char *argv[])
{
	bt_addr_le_t addr;
	int err;

	err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	if (argc < 4) {
		err = bt_le_set_auto_conn(&addr, BT_LE_CONN_PARAM_DEFAULT);
	} else if (!strcmp(argv[3], "on")) {
		err = bt_le_set_auto_conn(&addr, BT_LE_CONN_PARAM_DEFAULT);
	} else if (!strcmp(argv[3], "off")) {
		err = bt_le_set_auto_conn(&addr, NULL);
	} else {
		printf("%s", HELP_BT_INIT);;
		return ;
	}
    if(err != 0)
    {
        printf("bt_le_set_auto_conn failed err=(%d)\n", err);
    }
	return ;
}
#endif /* !defined(CONFIG_BT_FILTER_ACCEPT_LIST) */

static void cmd_connect_le_name(int argc, char *argv[])
{
	///const uint16_t timeout_seconds = 10;
	const struct bt_le_scan_param param = {
		.type       = BT_LE_SCAN_TYPE_ACTIVE,
		.options    = BT_LE_SCAN_OPT_NONE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
		.timeout    = 0 //timeout_seconds * 100, /* 10ms units */
	};
	int err = 0;

	/* Set the name filter which we will use in the scan callback to
	 * automatically connect to the first device that passes the filter
	 */
	cmd_scan_filter_set_name(argc, argv);
	if (scan_filter.name_set == false) {
		printf("Bluetooth set scan filter name to %s failed (err %d)\n",
			    argv[1], err);
		return ;
	}

	err = bt_le_scan_start(&param, NULL);
	if (err) {
		printf("Bluetooth scan failed (err %d)\n", err);
		return ;
	}

	printf("Bluetooth active scan enabled\n");

	/* Set boolean to tell the scan callback to connect to this name */
	auto_connect.connect_name = true;

	return ;
}
#endif /* CONFIG_BT_CENTRAL */

static void cmd_disconnect(int argc, char *argv[])
{
	struct bt_conn *conn;
	int err;

	if (default_conn && argc < 3) {
		conn = bt_conn_ref(default_conn);
	} else {
		bt_addr_le_t addr;

		if (argc < 3) {
			printf("%s", HELP_BT_INIT);;
			return ;
		}

		err = bt_addr_le_from_str(argv[1], argv[2], &addr);
		if (err) {
			printf("Invalid peer address (err %d)\n",err);
			return ;
		}

		conn = bt_conn_lookup_addr_le(selected_id, &addr);
	}

	if (!conn) {
		printf("Not connected\n");
		return ;
	}

	err = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
	if (err) {
		printf("Disconnection failed (err %d)\n", err);
		return ;
	}

	bt_conn_unref(conn);

	return ;
}

static void cmd_select(int argc, char *argv[])
{
	char addr_str[BT_ADDR_LE_STR_LEN];
	struct bt_conn *conn;
	bt_addr_le_t addr;
	int err;

	err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	conn = bt_conn_lookup_addr_le(selected_id, &addr);
	if (!conn) {
		printf("No matching connection found\n");
		return ;
	}

	if (default_conn) {
		bt_conn_unref(default_conn);
	}

	default_conn = conn;

	bt_addr_le_to_str(&addr, addr_str, sizeof(addr_str));
	printf("Selected conn is now: %s\n", addr_str);

	return ;
}

static const char *get_conn_type_str(uint8_t type)
{
	switch (type) {
	case BT_CONN_TYPE_LE: return "LE";
	case BT_CONN_TYPE_BR: return "BR/EDR";
	case BT_CONN_TYPE_SCO: return "SCO";
	default: return "Invalid";
	}
}

static const char *get_conn_role_str(uint8_t role)
{
	switch (role) {
	case BT_CONN_ROLE_CENTRAL: return "central";
	case BT_CONN_ROLE_PERIPHERAL: return "peripheral";
	default: return "Invalid";
	}
}

static void cmd_info(int argc, char *argv[])
{
	struct bt_conn *conn = NULL;
	struct bt_conn_info info;
	bt_addr_le_t addr;
	int err;

	switch (argc) {
	case 1:
		if (default_conn) {
			conn = bt_conn_ref(default_conn);
		}
		break;
	case 2:
		addr.type = BT_ADDR_LE_PUBLIC;
		err = bt_addr_from_str(argv[1], &addr.a);
		if (err) {
			printf("Invalid peer address (err %d)\n",
				    err);
			return ;
		}
		conn = bt_conn_lookup_addr_le(selected_id, &addr);
		break;
	case 3:
		err = bt_addr_le_from_str(argv[1], argv[2], &addr);

		if (err) {
			printf("Invalid peer address (err %d)\n",
				    err);
			return ;
		}
		conn = bt_conn_lookup_addr_le(selected_id, &addr);
		break;
	}

	if (!conn) {
		printf("Not connected\n");
		return ;
	}

	err = bt_conn_get_info(conn, &info);
	if (err) {
		printf("Failed to get info\n");
		goto done;
	}

	printf("Type: %s, Role: %s, Id: %u\n",
		    get_conn_type_str(info.type),
		    get_conn_role_str(info.role),
		    info.id);

	if (info.type == BT_CONN_TYPE_LE) {
		print_le_addr("Remote", info.le.dst);
		print_le_addr("Local", info.le.src);
		print_le_addr("Remote on-air", info.le.remote);
		print_le_addr("Local on-air", info.le.local);

		printf("Interval: 0x%04x (%u us)\n",
			    info.le.interval,
			    BT_CONN_INTERVAL_TO_US(info.le.interval));
		printf("Latency: 0x%04x\n",
			    info.le.latency);
		printf("Supervision timeout: 0x%04x (%d ms)\n",
			    info.le.timeout, info.le.timeout * 10);
#if defined(CONFIG_BT_USER_PHY_UPDATE)
		printf("LE PHY: TX PHY %s, RX PHY %s\n",
			    phy2str(info.le.phy->tx_phy),
			    phy2str(info.le.phy->rx_phy));
#endif
#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
		printf("LE data len: TX (len: %d time: %d)"
			    " RX (len: %d time: %d)\n",
			    info.le.data_len->tx_max_len,
			    info.le.data_len->tx_max_time,
			    info.le.data_len->rx_max_len,
			    info.le.data_len->rx_max_time);
#endif
	}

#if defined(CONFIG_BT_BREDR)
	if (info.type == BT_CONN_TYPE_BR) {
		char addr_str[BT_ADDR_STR_LEN];

		bt_addr_to_str(info.br.dst, addr_str, sizeof(addr_str));
		printf("Peer address %s\n", addr_str);
	}
#endif /* defined(CONFIG_BT_BREDR) */

done:
	bt_conn_unref(conn);

	return ;
}

static void cmd_conn_update(int argc, char *argv[])
{
	struct bt_le_conn_param param;
	int err;

	if (default_conn == NULL) {
		printf("at least, one connection is required\n");
		return ;
	}

	param.interval_min = strtoul(argv[1], NULL, 16);
	param.interval_max = strtoul(argv[2], NULL, 16);
	param.latency = strtoul(argv[3], NULL, 16);
	param.timeout = strtoul(argv[4], NULL, 16);

	err = bt_conn_le_param_update(default_conn, &param);
	if (err) {
		printf("conn update failed (err %d).\n", err);
	} else {
		printf("conn update initiated.\n");
	}

	return ;
}

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
static uint16_t tx_time_calc(uint8_t phy, uint16_t max_len)
{
	/* Access address + header + payload + MIC + CRC */
	uint16_t total_len = 4 + 2 + max_len + 4 + 3;

	switch (phy) {
	case BT_GAP_LE_PHY_1M:
		/* 1 byte preamble, 8 us per byte */
		return 8 * (1 + total_len);
	case BT_GAP_LE_PHY_2M:
		/* 2 byte preamble, 4 us per byte */
		return 4 * (2 + total_len);
	case BT_GAP_LE_PHY_CODED:
		/* S8: Preamble + CI + TERM1 + 64 us per byte + TERM2 */
		return 80 + 16 + 24 + 64 * (total_len) + 24;
	default:
		return 0;
	}
}

static void cmd_conn_data_len_update(int argc,
				    char *argv[])
{
	struct bt_conn_le_data_len_param param;
	int err;

	if (default_conn == NULL) {
		printf("at least, one connection is required\n");
		return ;
	}

	param.tx_max_len = strtoul(argv[1], NULL, 10);

	if (argc > 2) {
		param.tx_max_time = strtoul(argv[2], NULL, 10);
	} else {
		/* Assume 1M if not able to retrieve PHY */
		uint8_t phy = BT_GAP_LE_PHY_1M;

#if defined(CONFIG_BT_USER_PHY_UPDATE)
		struct bt_conn_info info;

		err = bt_conn_get_info(default_conn, &info);
		if (!err) {
			phy = info.le.phy->tx_phy;
		}
#endif
		param.tx_max_time = tx_time_calc(phy, param.tx_max_len);
		printf("Calculated tx time: %d\n", param.tx_max_time);
	}



	err = bt_conn_le_data_len_update(default_conn, &param);
	if (err) {
		printf("data len update failed (err %d).\n", err);
	} else {
		printf("data len update initiated.\n");
	}

	return ;
}
#endif

#if defined(CONFIG_BT_USER_PHY_UPDATE)
static void cmd_conn_phy_update(int argc,
			       char *argv[])
{
	struct bt_conn_le_phy_param param;
	int err;

	if (default_conn == NULL) {
		printf("at least, one connection is required\n");
		return ;
	}

	param.pref_tx_phy = strtoul(argv[1], NULL, 16);
	param.pref_rx_phy = param.pref_tx_phy;
	param.options = BT_CONN_LE_PHY_OPT_NONE;

	for (size_t argn = 2; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "s2")) {
			param.options |= BT_CONN_LE_PHY_OPT_CODED_S2;
		} else if (!strcmp(arg, "s8")) {
			param.options |= BT_CONN_LE_PHY_OPT_CODED_S8;
		} else {
			param.pref_rx_phy = strtoul(arg, NULL, 16);
		}
	}

	err = bt_conn_le_phy_update(default_conn, &param);
	if (err) {
		printf("PHY update failed (err %d).\n", err);
	} else {
		printf("PHY update initiated.\n");
	}

	return ;
}
#endif

#if defined(CONFIG_BT_CENTRAL)
static void cmd_chan_map(int argc, char *argv[])
{
	uint8_t chan_map[5] = {};
	int err;

	if (hex2bin(argv[1], strlen(argv[1]), chan_map, 5) == 0) {
		printf("Invalid channel map\n");
		return ;
	}
	sys_mem_swap(chan_map, 5);

	err = bt_le_set_chan_map(chan_map);
	if (err) {
		printf("Failed to set channel map (err %d)\n", err);
	} else {
		printf("Channel map set\n");
	}

	return ;
}
#endif /* CONFIG_BT_CENTRAL */

static void cmd_oob(int argc, char *argv[])
{
	int err;

	err = bt_le_oob_get_local(selected_id, &oob_local);
	if (err) {
		printf("OOB data failed\n");
		return ;
	}

	print_le_oob(&oob_local);

	return ;
}

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
static void cmd_oob_remote(int argc,
			     char *argv[])
{
	int err;
	bt_addr_le_t addr;

	err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	bt_addr_le_copy(&oob_remote.addr, &addr);

	if (argc == 5) {
		hex2bin(argv[3], strlen(argv[3]), oob_remote.le_sc_data.r,
			sizeof(oob_remote.le_sc_data.r));
		hex2bin(argv[4], strlen(argv[4]), oob_remote.le_sc_data.c,
			sizeof(oob_remote.le_sc_data.c));
		bt_le_oob_set_sc_flag(true);
	} else {
		printf("%s", HELP_BT_INIT);;
		return ;
	}

	return ;
}

static void cmd_oob_clear(int argc, char *argv[])
{
	memset(&oob_remote, 0, sizeof(oob_remote));
	bt_le_oob_set_sc_flag(false);

	return ;
}
#endif /* CONFIG_BT_SMP || CONFIG_BT_BREDR) */

static void cmd_clear(int argc, char *argv[])
{
	bt_addr_le_t addr;
	int err;

	if (strcmp(argv[1], "all") == 0) {
		err = bt_unpair(selected_id, NULL);
		if (err) {
			printf("Failed to clear pairings (err %d)\n",
			      err);
			return ;
		} else {
			printf("Pairings successfully cleared\n");
		}

		return ;
	}

	if (argc < 3) {
#if defined(CONFIG_BT_BREDR)
		addr.type = BT_ADDR_LE_PUBLIC;
		err = bt_addr_from_str(argv[1], &addr.a);
#else
		printf("Both address and address type needed\n");
		return ;
#endif
	} else {
		err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	}

	if (err) {
		printf("Invalid address\n");
		return ;
	}

	err = bt_unpair(selected_id, &addr);
	if (err) {
		printf("Failed to clear pairing (err %d)\n", err);
	} else {
		printf("Pairing successfully cleared\n");
	}

	return ;
}
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
static void cmd_security(int argc, char *argv[])
{
	int err, sec;
	struct bt_conn_info info;

	if (!default_conn || (bt_conn_get_info(default_conn, &info) < 0)) {
		printf("Not connected\n");
		return ;
	}

	if (argc < 2) {
		printf("BT_SECURITY_L%d\n", bt_conn_get_security(default_conn));

		return ;
	}

	sec = *argv[1] - '0';

	if ((info.type == BT_CONN_TYPE_BR &&
	    (sec < BT_SECURITY_L0 || sec > BT_SECURITY_L3))) {
		printf("Invalid BR/EDR security level (%d)\n", sec);
		return ;
	}

	if ((info.type == BT_CONN_TYPE_LE &&
	    (sec < BT_SECURITY_L1 || sec > BT_SECURITY_L4))) {
		printf("Invalid LE security level (%d)\n", sec);
		return ;
	}

	if (argc > 2) {
		if (!strcmp(argv[2], "force-pair")) {
			sec |= BT_SECURITY_FORCE_PAIR;
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}
	}

	err = bt_conn_set_security(default_conn, sec);
	if (err) {
		printf("Setting security failed (err %d)\n", err);
	}

	return ;
}

static void cmd_bondable(int argc, char *argv[])
{
	const char *bondable;

	bondable = argv[1];
	if (!strcmp(bondable, "on")) {
		bt_set_bondable(true);
	} else if (!strcmp(bondable, "off")) {
		bt_set_bondable(false);
	} else {
		printf("%s", HELP_BT_INIT);;
		return ;
	}

	return ;
}

static void bond_info(const struct bt_bond_info *info, void *user_data)
{
	char addr[BT_ADDR_LE_STR_LEN];
	int *bond_count = user_data;

	bt_addr_le_to_str(&info->addr, addr, sizeof(addr));
	printf("Remote Identity: %s\n", addr);
	(*bond_count)++;
}

static void cmd_bonds(int argc, char *argv[])
{
	int bond_count = 0;

	printf("Bonded devices:\n");
	bt_foreach_bond(selected_id, bond_info, &bond_count);
	printf("Total %d\n", bond_count);

	return ;
}

static const char *role_str(uint8_t role)
{
	switch (role) {
	case BT_CONN_ROLE_CENTRAL:
		return "Central";
	case BT_CONN_ROLE_PERIPHERAL:
		return "Peripheral";
	}

	return "Unknown";
}

static void connection_info(struct bt_conn *conn, void *user_data)
{
	char addr[BT_ADDR_LE_STR_LEN];
	int *conn_count = user_data;
	struct bt_conn_info info;

	if (bt_conn_get_info(conn, &info) < 0) {
		printf("Unable to get info: conn %p\n", conn);
		return;
	}

	switch (info.type) {
#if defined(CONFIG_BT_BREDR)
	case BT_CONN_TYPE_BR:
		bt_addr_to_str(info.br.dst, addr, sizeof(addr));
		printf(" #%u [BR][%s] %s\n", info.id, role_str(info.role), addr);
		break;
#endif
	case BT_CONN_TYPE_LE:
		bt_addr_le_to_str(info.le.dst, addr, sizeof(addr));
		printf("%s#%u [LE][%s] %s: Interval %u latency %u timeout %u\n",
			    conn == default_conn ? "*" : " ", info.id, role_str(info.role), addr,
			    info.le.interval, info.le.latency, info.le.timeout);
		break;
#if defined(CONFIG_BT_ISO)
	case BT_CONN_TYPE_ISO:
		bt_addr_le_to_str(info.le.dst, addr, sizeof(addr));
		printf(" #%u [ISO][%s] %s\n", info.id, role_str(info.role), addr);
		break;
#endif
	default:
		break;
	}

	(*conn_count)++;
}

static void cmd_connections(int argc, char *argv[])
{
	int conn_count = 0;

	printf("Connected devices:\n");
	bt_conn_foreach(BT_CONN_TYPE_ALL, connection_info, &conn_count);
	printf("Total %d\n", conn_count);

	return ;
}

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];
	char passkey_str[7];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	snprintk(passkey_str, 7, "%06u", passkey);

	printf("Passkey for %s: %s\n", addr, passkey_str);
}

#if defined(CONFIG_BT_PASSKEY_KEYPRESS)
static void auth_passkey_display_keypress(struct bt_conn *conn,
					  enum bt_conn_auth_keypress type)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Passkey keypress notification from %s: type %d\n",
		    addr, type);
}
#endif

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];
	char passkey_str[7];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	snprintk(passkey_str, 7, "%06u", passkey);

	printf("Confirm passkey for %s: %s\n", addr, passkey_str);
}

static void auth_passkey_entry(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Enter passkey for %s\n", addr);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	conn_addr_str(conn, addr, sizeof(addr));

	printf("Pairing cancelled: %s", addr);

	/* clear connection reference for sec mode 3 pairing */
	if (pairing_conn) {
		bt_conn_unref(pairing_conn);
		pairing_conn = NULL;
	}
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Confirm pairing for %s\n", addr);
}

#if !defined(CONFIG_BT_SMP_OOB_LEGACY_PAIR_ONLY)
static const char *oob_config_str(int oob_config)
{
	switch (oob_config) {
	case BT_CONN_OOB_LOCAL_ONLY:
		return "Local";
	case BT_CONN_OOB_REMOTE_ONLY:
		return "Remote";
	case BT_CONN_OOB_BOTH_PEERS:
		return "Local and Remote";
	case BT_CONN_OOB_NO_DATA:
	default:
		return "no";
	}
}
#endif /* !defined(CONFIG_BT_SMP_OOB_LEGACY_PAIR_ONLY) */

static void auth_pairing_oob_data_request(struct bt_conn *conn,
					  struct bt_conn_oob_info *oob_info)
{
	char addr[BT_ADDR_LE_STR_LEN];
	struct bt_conn_info info;
	int err;

	err = bt_conn_get_info(conn, &info);
	if (err) {
		return;
	}

#if !defined(CONFIG_BT_SMP_OOB_LEGACY_PAIR_ONLY)
	if (oob_info->type == BT_CONN_OOB_LE_SC) {
		struct bt_le_oob_sc_data *oobd_local =
			oob_info->lesc.oob_config != BT_CONN_OOB_REMOTE_ONLY
						  ? &oob_local.le_sc_data
						  : NULL;
		struct bt_le_oob_sc_data *oobd_remote =
			oob_info->lesc.oob_config != BT_CONN_OOB_LOCAL_ONLY
						  ? &oob_remote.le_sc_data
						  : NULL;

		if (oobd_remote &&
		    !bt_addr_le_eq(info.le.remote, &oob_remote.addr)) {
			bt_addr_le_to_str(info.le.remote, addr, sizeof(addr));
			printf("No OOB data available for remote %s\n",
				    addr);
			bt_conn_auth_cancel(conn);
			return;
		}

		if (oobd_local &&
		    !bt_addr_le_eq(info.le.local, &oob_local.addr)) {
			bt_addr_le_to_str(info.le.local, addr, sizeof(addr));
			printf("No OOB data available for local %s\n",
				    addr);
			bt_conn_auth_cancel(conn);
			return;
		}

		bt_le_oob_set_sc_data(conn, oobd_local, oobd_remote);

		bt_addr_le_to_str(info.le.dst, addr, sizeof(addr));
		printf("Set %s OOB SC data for %s, \n",
			    oob_config_str(oob_info->lesc.oob_config), addr);
		return;
	}
#endif /* CONFIG_BT_SMP_OOB_LEGACY_PAIR_ONLY */

	bt_addr_le_to_str(info.le.dst, addr, sizeof(addr));
	printf("Legacy OOB TK requested from remote %s\n", addr);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("%s with %s\n", bonded ? "Bonded" : "Paired",
		    addr);
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Pairing failed with %s reason: %s (%d)\n", addr,
		    security_err_str(err), err);
}

#if defined(CONFIG_BT_BREDR)
static void auth_pincode_entry(struct bt_conn *conn, bool highsec)
{
	char addr[BT_ADDR_STR_LEN];
	struct bt_conn_info info;

	if (bt_conn_get_info(conn, &info) < 0) {
		return;
	}

	if (info.type != BT_CONN_TYPE_BR) {
		return;
	}

	bt_addr_to_str(info.br.dst, addr, sizeof(addr));

	if (highsec) {
		printf("Enter 16 digits wide PIN code for %s\n",
			    addr);
	} else {
		printf("Enter PIN code for %s\n", addr);
	}

	/*
	 * Save connection info since in security mode 3 (link level enforced
	 * security) PIN request callback is called before connected callback
	 */
	if (!default_conn && !pairing_conn) {
		pairing_conn = bt_conn_ref(conn);
	}
}
#endif

#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
enum bt_security_err pairing_accept(
	struct bt_conn *conn, const struct bt_conn_pairing_feat *const feat)
{
	printf("Remote pairing features: "
			       "IO: 0x%02x, OOB: %d, AUTH: 0x%02x, Key: %d, "
			       "Init Kdist: 0x%02x, Resp Kdist: 0x%02x\n",
			       feat->io_capability, feat->oob_data_flag,
			       feat->auth_req, feat->max_enc_key_size,
			       feat->init_key_dist, feat->resp_key_dist);

	return BT_SECURITY_ERR_SUCCESS;
}
#endif /* CONFIG_BT_SMP_APP_PAIRING_ACCEPT */

void bond_deleted(uint8_t id, const bt_addr_le_t *peer)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(peer, addr, sizeof(addr));
	printf("Bond deleted for %s, id %u\n", addr, id);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
#if defined(CONFIG_BT_PASSKEY_KEYPRESS)
	.passkey_display_keypress = auth_passkey_display_keypress,
#endif
	.passkey_entry = NULL,
	.passkey_confirm = NULL,
#if defined(CONFIG_BT_BREDR)
	.pincode_entry = auth_pincode_entry,
#endif
	.oob_data_request = NULL,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
	.pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_display_yes_no = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.passkey_confirm = auth_passkey_confirm,
#if defined(CONFIG_BT_BREDR)
	.pincode_entry = auth_pincode_entry,
#endif
	.oob_data_request = NULL,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
	.pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_input = {
	.passkey_display = NULL,
	.passkey_entry = auth_passkey_entry,
	.passkey_confirm = NULL,
#if defined(CONFIG_BT_BREDR)
	.pincode_entry = auth_pincode_entry,
#endif
	.oob_data_request = NULL,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
	.pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_confirm = {
#if defined(CONFIG_BT_BREDR)
	.pincode_entry = auth_pincode_entry,
#endif
	.oob_data_request = NULL,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
	.pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_all = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = auth_passkey_entry,
	.passkey_confirm = auth_passkey_confirm,
#if defined(CONFIG_BT_BREDR)
	.pincode_entry = auth_pincode_entry,
#endif
	.oob_data_request = auth_pairing_oob_data_request,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
	.pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_oob = {
	.passkey_display = NULL,
	.passkey_entry = NULL,
	.passkey_confirm = NULL,
#if defined(CONFIG_BT_BREDR)
	.pincode_entry = NULL,
#endif
	.oob_data_request = auth_pairing_oob_data_request,
	.cancel = auth_cancel,
	.pairing_confirm = NULL,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
	.pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_status = {
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
	.pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_info_cb auth_info_cb = {
	.pairing_failed = auth_pairing_failed,
	.pairing_complete = auth_pairing_complete,
	.bond_deleted = bond_deleted,
};

static void cmd_auth(int argc, char *argv[])
{
	int err;

	if (!strcmp(argv[1], "all")) {
		err = bt_conn_auth_cb_register(&auth_cb_all);
	} else if (!strcmp(argv[1], "input")) {
		err = bt_conn_auth_cb_register(&auth_cb_input);
	} else if (!strcmp(argv[1], "display")) {
		err = bt_conn_auth_cb_register(&auth_cb_display);
	} else if (!strcmp(argv[1], "yesno")) {
		err = bt_conn_auth_cb_register(&auth_cb_display_yes_no);
	} else if (!strcmp(argv[1], "confirm")) {
		err = bt_conn_auth_cb_register(&auth_cb_confirm);
	} else if (!strcmp(argv[1], "oob")) {
		err = bt_conn_auth_cb_register(&auth_cb_oob);
	} else if (!strcmp(argv[1], "status")) {
		err = bt_conn_auth_cb_register(&auth_cb_status);
	} else if (!strcmp(argv[1], "none")) {
		err = bt_conn_auth_cb_register(NULL);
	} else {
		printf("%s", HELP_BT_INIT);;
		return ;
	}

	if (err) {
		printf("Failed to set auth handlers (%d)\n", err);
	}

	return ;
}

static void cmd_auth_cancel(
			  int argc, char *argv[])
{
	struct bt_conn *conn;

	if (default_conn) {
		conn = default_conn;
	} else if (pairing_conn) {
		conn = pairing_conn;
	} else {
		conn = NULL;
	}

	if (!conn) {
		printf("Not connected\n");
		return ;
	}

	bt_conn_auth_cancel(conn);

	return ;
}

static void cmd_auth_passkey_confirm(
				   int argc, char *argv[])
{
	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	bt_conn_auth_passkey_confirm(default_conn);
	return ;
}

static void cmd_auth_pairing_confirm(
				   int argc, char *argv[])
{
	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	bt_conn_auth_pairing_confirm(default_conn);
	return ;
}

#if defined(CONFIG_BT_FILTER_ACCEPT_LIST)
static void cmd_fal_add(int argc, char *argv[])
{
	bt_addr_le_t addr;
	int err;

	err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	err = bt_le_filter_accept_list_add(&addr);
	if (err) {
		printf("Add to fa list failed (err %d)\n", err);
		return ;
	}

	return ;
}

static void cmd_fal_rem(int argc, char *argv[])
{
	bt_addr_le_t addr;
	int err;

	err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	err = bt_le_filter_accept_list_remove(&addr);
	if (err) {
		printf("Remove from fa list failed (err %d)\n",
			    err);
		return ;
	}
	return ;
}

static void cmd_fal_clear(int argc, char *argv[])
{
	int err;

	err = bt_le_filter_accept_list_clear();
	if (err) {
		printf("Clearing fa list failed (err %d)", err);
		return ;
	}

	return ;
}

#if defined(CONFIG_BT_CENTRAL)
static void cmd_fal_connect(int argc, char *argv[])
{
	int err;
	const char *action = argv[1];
	uint32_t options = 0;

#if defined(CONFIG_BT_EXT_ADV)
	for (size_t argn = 2; argn < argc; argn++) {
		const char *arg = argv[argn];

		if (!strcmp(arg, "coded")) {
			options |= BT_CONN_LE_OPT_CODED;
		} else if (!strcmp(arg, "no-1m")) {
			options |= BT_CONN_LE_OPT_NO_1M;
		} else {
			printf("%s", HELP_BT_INIT);;
			return ;
		}
	}
#endif /* defined(CONFIG_BT_EXT_ADV) */
	struct bt_conn_le_create_param *create_params =
		BT_CONN_LE_CREATE_PARAM(options,
					BT_GAP_SCAN_FAST_INTERVAL,
					BT_GAP_SCAN_FAST_WINDOW);

	if (!strcmp(action, "on")) {
		err = bt_conn_le_create_auto(create_params,
					     BT_LE_CONN_PARAM_DEFAULT);
		if (err) {
			printf("Auto connect failed (err %d)\n", err);
			return ;
		}
	} else if (!strcmp(action, "off")) {
		err = bt_conn_create_auto_stop();
		if (err) {
			printf("Auto connect stop failed (err %d)\n",
				    err);
		}
		return ;
	}

	return ;
}
#endif /* CONFIG_BT_CENTRAL */
#endif /* defined(CONFIG_BT_FILTER_ACCEPT_LIST) */

#if defined(CONFIG_BT_FIXED_PASSKEY)
static void cmd_fixed_passkey(
			    int argc, char *argv[])
{
	unsigned int passkey;
	int err;

	if (argc < 2) {
		bt_passkey_set(BT_PASSKEY_INVALID);
		printf("Fixed passkey cleared\n");
		return ;
	}

	passkey = atoi(argv[1]);
	if (passkey > 999999) {
		printf("Passkey should be between 0-999999\n");
		return ;
	}

	err = bt_passkey_set(passkey);
	if (err) {
		printf("Setting fixed passkey failed (err %d)\n",
			    err);
	}

	return ;
}
#endif

static void cmd_auth_passkey(
			   int argc, char *argv[])
{
	unsigned int passkey;
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	passkey = atoi(argv[1]);
	if (passkey > 999999) {
		printf("Passkey should be between 0-999999\n");
		return ;
	}

	err = bt_conn_auth_passkey_entry(default_conn, passkey);
	if (err) {
		printf("Failed to set passkey (%d)\n", err);
		return ;
	}

	return ;
}

#if defined(CONFIG_BT_PASSKEY_KEYPRESS)
static void cmd_auth_passkey_notify(
				  int argc, char *argv[])
{
	unsigned long type;
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	err = 0;
	type = strtoul(argv[1], NULL, 10);
	if (err || !IN_RANGE(type, BT_CONN_AUTH_KEYPRESS_ENTRY_STARTED,
			     BT_CONN_AUTH_KEYPRESS_ENTRY_COMPLETED)) {
		printf("<type> must be a value in range of enum bt_conn_auth_keypress\n");
		return ;
	}

	err = bt_conn_auth_keypress_notify(default_conn, type);
	if (err) {
		printf("bt_conn_auth_keypress_notify errno %d\n", err);
		return ;
	}

	return ;
}
#endif /* CONFIG_BT_PASSKEY_KEYPRESS */

#if !defined(CONFIG_BT_SMP_SC_PAIR_ONLY)
static void cmd_auth_oob_tk(int argc, char *argv[])
{
	uint8_t tk[16];
	size_t len;
	int err;

	len = hex2bin(argv[1], strlen(argv[1]), tk, sizeof(tk));
	if (len != sizeof(tk)) {
		printf("TK should be 16 bytes\n");
		return ;
	}

	err = bt_le_oob_set_legacy_tk(default_conn, tk);
	if (err) {
		printf("Failed to set TK (%d)\n", err);
		return ;
	}

	return ;
}
#endif /* !defined(CONFIG_BT_SMP_SC_PAIR_ONLY) */
#endif /* CONFIG_BT_SMP) || CONFIG_BT_BREDR */

WM_CLI_CMD_DEFINE(btinit, cmd_init, enable bt cmd, btinit [no-settings-load] [sync] -- btinit cmd);
WM_CLI_CMD_DEFINE(btdeinit, cmd_disable, disable bt cmd, btdeinit -- btinit cmd);
#ifdef CONFIG_SETTINGS
WM_CLI_CMD_DEFINE(btload, cmd_settings_load, load bt settings cmd, load bt settings -- btload cmd);
#endif
#if defined(CONFIG_BT_HCI)
WM_CLI_CMD_DEFINE(bthci, cmd_hci_cmd, send hci cmd, bthci <ogf> <ocf> <data> -- bthci cmd);
#endif
WM_CLI_CMD_DEFINE(btname, cmd_name, show/change bt name cmd, btname [new name value] -- btname cmd);
WM_CLI_CMD_DEFINE(btappearance, cmd_appearance, show/change bt appearance cmd, btappearance [new appearance value] -- btappearance cmd);
WM_CLI_CMD_DEFINE(btidcreate, cmd_id_create, create identity cmd, btidcreate [address: XX:XX:XX:XX:XX:XX] -- btidcreate cmd);
WM_CLI_CMD_DEFINE(btidreset, cmd_id_reset, reset identity cmd, btidreset [address: XX:XX:XX:XX:XX:XX] -- btidreset cmd);
WM_CLI_CMD_DEFINE(btiddelete, cmd_id_delete, delete identity cmd, btiddelete <ID> -- btiddelete cmd);
WM_CLI_CMD_DEFINE(btidshow, cmd_id_show, show identity list cmd, btidshow -- btidshow cmd);
WM_CLI_CMD_DEFINE(btidselect, cmd_id_select, select identity cmd, btidselect <ID> -- btidselect cmd);
#if defined(CONFIG_BT_OBSERVER)
WM_CLI_CMD_DEFINE(btscan, cmd_scan, scan cmd, btscan <value: on passive off> [filter: dups nodups] [fal] -- btscan cmd);
WM_CLI_CMD_DEFINE(btfiltername, cmd_scan_filter_set_name, scan filter by name cmd, btfiltername [name] -- btfiltername cmd);
WM_CLI_CMD_DEFINE(btfilterrssi, cmd_scan_filter_set_rssi, scan filter by rssi cmd, btfilterrssi <rssi> -- btfilterrssi cmd);
WM_CLI_CMD_DEFINE(btfilterintv, cmd_scan_filter_set_pa_interval, scan filter by interval cmd, btfilterintv <interval> -- btfilterrssi cmd);
WM_CLI_CMD_DEFINE(btfilterclear, cmd_scan_filter_clear_all, scan filter clear cmd, btfilterclear -- btfilterclear cmd);
WM_CLI_CMD_DEFINE(btfilteraddress, cmd_scan_filter_set_addr, scan filter by address cmd, btfilteraddress HELP_ADDR_LE -- btfilteraddress cmd);
WM_CLI_CMD_DEFINE(btfilteraddressclear, cmd_scan_filter_clear_addr, remove scan filter address cmd, btfilteraddressclear -- btfilteraddressclear cmd);
WM_CLI_CMD_DEFINE(btscanverbose, cmd_scan_verbose_output, scan verbose output cmd, btscanverbose <value: on off> -- btscan cmd);
#endif
#if defined(CONFIG_BT_BROADCASTER)
WM_CLI_CMD_DEFINE(btadv, cmd_advertise, advertise cmd, btadv 
                                  <type: off on scan nconn> [mode: discov non_discov]
                                  [filter-accept-list: fal fal-scan fal-conn] [identity] [no-name]
                                  [one-time] [name-ad] [appearance]
                                  [disable-37] [disable-38] [disable-39] -- btadv cmd);
#if defined(CONFIG_BT_PERIPHERAL)
WM_CLI_CMD_DEFINE(btdirectedadv, cmd_directed_adv, directed advertise cmd, btdirectedadv HELP_ADDR_LE [mode: low] [identity] [dir-rpa] -- btdirectedadv cmd);
#endif
#if defined(CONFIG_BT_EXT_ADV)
WM_CLI_CMD_DEFINE(btadvcreate, cmd_adv_create, create adv cmd, btadvcreate EXT_ADV_PARAM -- btadvcreate cmd);
WM_CLI_CMD_DEFINE(btadvparam, cmd_adv_param, config adv param cmd, btadvparam EXT_ADV_PARAM -- btadvparam cmd);
WM_CLI_CMD_DEFINE(btadvdata, cmd_adv_data, config adv data cmd, btadvdata <data> [scan-response <data>] <type: discov, hex> [appearance] , -- btadvdata cmd);
WM_CLI_CMD_DEFINE(btadvstart, cmd_adv_start, start adv cmd, btadvstart [timeout <timeout>] [num-events <num events>] -- btadvstart cmd);
WM_CLI_CMD_DEFINE(btadvstop, cmd_adv_stop, stop adv cmd, btadvstop -- btadvstop cmd);
WM_CLI_CMD_DEFINE(btadvdelete, cmd_adv_delete, delete selected adv cmd, btadvdelete -- btadvdelete cmd);
WM_CLI_CMD_DEFINE(btadvselect, cmd_adv_select, select adv cmd, btadvselect [adv] -- btadvselect cmd);
WM_CLI_CMD_DEFINE(btadvinfo, cmd_adv_info, show adv sets info cmd, btadvinfo -- btadvinfo cmd);
#if defined(CONFIG_BT_PERIPHERAL)
WM_CLI_CMD_DEFINE(btadvoob, cmd_adv_oob, show adv oob info cmd, btadvoob -- btadvoob cmd);
#endif
#if defined(CONFIG_BT_PRIVACY)
WM_CLI_CMD_DEFINE(btadvrpaexpire, cmd_adv_rpa_expire, on off rpa expire cmd, btadvrpaexpire HELP_ONOFF -- btadvrpaexpire cmd);
#endif
#if defined(CONFIG_BT_PER_ADV)
WM_CLI_CMD_DEFINE(btperadv, cmd_per_adv, on off periodic adv cmd, btperadv HELP_ONOFF -- btperadv cmd);
WM_CLI_CMD_DEFINE(btperadvparam, cmd_per_adv_param, config periodic adv param cmd, btperadvparam [<interval-min> [<interval-max> [tx_power]]] -- btperadvparam cmd);
WM_CLI_CMD_DEFINE(btperadvdata, cmd_per_adv_data, config periodic adv data cmd, btperadvdata [data] -- btperadvdata cmd);
#endif /* CONFIG_BT_PER_ADV */
#endif
#endif
#if defined(CONFIG_BT_PER_ADV_SYNC)
WM_CLI_CMD_DEFINE(btperadvsynccreate, cmd_per_adv_sync_create, create periodic sync adv cmd, btperadvsynccreate HELP_ADDR_LE  <sid> [skip <count>] [timeout <ms>] [aoa] [aod_1us] [aod_2us] [cte_only] -- btperadvsynccreate cmd);
WM_CLI_CMD_DEFINE(btperadvsyncdelete, cmd_per_adv_sync_delete, delete periodic sync adv cmd, btperadvsyncdelete [<index>] -- btperadvsyncdelete cmd);
WM_CLI_CMD_DEFINE(btperadvsyncselect, cmd_per_adv_sync_select, select periodic sync adv cmd, btperadvsyncselect [adv] -- btperadvsyncselect cmd);
#endif /* defined(CONFIG_BT_PER_ADV_SYNC) */

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER)
WM_CLI_CMD_DEFINE(btpastsubscribe, cmd_past_subscribe, subscribe periodic adv cmd, btpastsubscribe [conn] [skip <count>] [timeout <ms>] [aoa] [aod_1us] [aod_2us] [cte_only] -- btpastsubscribe cmd);
WM_CLI_CMD_DEFINE(btpastunsubscribe, cmd_past_unsubscribe, unsubscribe periodic adv cmd, btpastunsubscribe [conn] -- btpastunsubscribe cmd);
#endif /* CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER */

#if defined(CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER)
#if defined(CONFIG_BT_PER_ADV_SYNC)
WM_CLI_CMD_DEFINE(btperadvsynctransfer, cmd_per_adv_sync_transfer, periodic sync adv transfer cmd, btperadvsynctransfer [<index>] -- btperadvsynctransfer cmd);
#endif /* CONFIG_BT_PER_ADV_SYNC */
#if defined(CONFIG_BT_PER_ADV)
WM_CLI_CMD_DEFINE(btperadvsetinfotransfer, cmd_per_adv_set_info_transfer, adv set info transfer cmd, btperadvsetinfotransfer -- btperadvsetinfotransfer cmd);
#endif /* CONFIG_BT_PER_ADV */
#endif /* CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER */
#if defined(CONFIG_BT_CENTRAL)
WM_CLI_CMD_DEFINE(btconnect, cmd_connect_le, connect remote device cmd, btconnect HELP_ADDR_LE EXT_ADV_SCAN_OPT -- btconnect cmd);
#if !defined(CONFIG_BT_FILTER_ACCEPT_LIST)
WM_CLI_CMD_DEFINE(btautoconnect, cmd_auto_conn, auto-connect remote device cmd, btautoconnect HELP_ADDR_LE -- btautoconnect cmd);
#endif /* !defined(CONFIG_BT_FILTER_ACCEPT_LIST) */
WM_CLI_CMD_DEFINE(btconnectname, cmd_connect_le_name, connect remote device by name cmd, btconnectname <name filter> -- btconnectname cmd);
#endif /* CONFIG_BT_CENTRAL */
WM_CLI_CMD_DEFINE(btdisconnect, cmd_disconnect, disconnect with remote device cmd, btdisconnect HELP_ADDR_LE -- btdisconnect cmd);
WM_CLI_CMD_DEFINE(btselect, cmd_select, select default connection cmd, btselect HELP_ADDR_LE -- btselect cmd);
WM_CLI_CMD_DEFINE(btconninfo, cmd_info, show connection information cmd, btconninfo HELP_ADDR_LE -- btconninfo cmd);
WM_CLI_CMD_DEFINE(btconnupdate, cmd_conn_update, update connection param cmd, btconnupdate <min> <max> <latency> <timeout> -- btconnupdate cmd);
#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
WM_CLI_CMD_DEFINE(btdatalenupdate, cmd_conn_data_len_update, update data length cmd, btdatalenupdate <tx_max_len> [tx_max_time] -- btdatalenupdate cmd);
#endif
#if defined(CONFIG_BT_USER_PHY_UPDATE)
WM_CLI_CMD_DEFINE(btphyupdate, cmd_conn_phy_update, update phy feature cmd, btphyupdate <tx_phy> [rx_phy] [s2] [s8] -- btphyupdate cmd);
#endif
#if defined(CONFIG_BT_CENTRAL)
WM_CLI_CMD_DEFINE(btchannelmap, cmd_chan_map, channel map update cmd, btchannelmap <channel-map: XXXXXXXXXX> (36-0) -- btchannelmap cmd);
#endif /* CONFIG_BT_CENTRAL */
WM_CLI_CMD_DEFINE(btoob, cmd_oob, show oob information cmd, btoob -- btoob cmd);
WM_CLI_CMD_DEFINE(btclear, cmd_clear, clear pair cmd, btclear [all] ["HELP_ADDR_LE"] -- btclear cmd);	
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
WM_CLI_CMD_DEFINE(btsecurity, cmd_security, select security level cmd, btsecurity <security level BR/EDR: 0 - 3 LE: 1 - 4> [force-pair]-- btsecurity cmd);
WM_CLI_CMD_DEFINE(btbondable, cmd_bondable, on off bondable cmd, btbondable HELP_ONOFF -- btbondable cmd);
WM_CLI_CMD_DEFINE(btbonds, cmd_bonds, show all bonds information cmd, btbonds -- btbonds cmd);
WM_CLI_CMD_DEFINE(btconnections, cmd_connections, show all connections information cmd, btconnections -- btconnections cmd);
WM_CLI_CMD_DEFINE(btauth, cmd_auth, config auth method cmd, btauth <method: all input display yesno confirm oob status none> -- btauth cmd);
WM_CLI_CMD_DEFINE(btauthcancel, cmd_auth_cancel, cancel default connection auth cmd, btauthcancel -- btauthcancel cmd);
WM_CLI_CMD_DEFINE(btauthpasskey, cmd_auth_passkey, input auth passkey cmd, btauthpasskey <passkey>-- btauthpasskey cmd);

#if defined(CONFIG_BT_PASSKEY_KEYPRESS)
WM_CLI_CMD_DEFINE(btauthpasskeynotify, cmd_auth_passkey_notify, input auth passkey notify type cmd, btauthpasskeynotify <type>-- btauthpasskeynotify cmd);
#endif /* CONFIG_BT_PASSKEY_KEYPRESS */
WM_CLI_CMD_DEFINE(btauthpasskeyconfirm, cmd_auth_passkey_confirm, confirm auth passkey cmd, btauthpasskeyconfirm -- btauthpasskeyconfirm cmd);
WM_CLI_CMD_DEFINE(btauthpairingconfirm, cmd_auth_pairing_confirm, confirm auth pairing cmd, btauthpairingconfirm -- btauthpairingconfirm cmd);
#if !defined(CONFIG_BT_SMP_SC_PAIR_ONLY)
WM_CLI_CMD_DEFINE(btauthooktk, cmd_auth_oob_tk, input tk for oob cmd, btauthooktk "<tk>" -- btauthooktk cmd);
#endif /* !defined(CONFIG_BT_SMP_SC_PAIR_ONLY) */
WM_CLI_CMD_DEFINE(btoobremote, cmd_oob_remote, config remote oob cmd, btoobremote HELP_ADDR_LE" <oob rand> <oob confirm>" -- btoobremote cmd);
WM_CLI_CMD_DEFINE(btoobclear, cmd_oob_clear, clear remote oob cmd, btoobclear -- btoobclear cmd);
#if defined(CONFIG_BT_FILTER_ACCEPT_LIST)
WM_CLI_CMD_DEFINE(btfaladd, cmd_fal_add, clear add filter accept list cmd, btfaladd HELP_ADDR_LE -- btfaladd cmd);
WM_CLI_CMD_DEFINE(btfalrem, cmd_fal_rem, remove filter accept list cmd, btfalrem HELP_ADDR_LE -- btfalrem cmd);
WM_CLI_CMD_DEFINE(btfalclear, cmd_fal_clear,        clear filter accept list cmd, btfalclear -- btfalclear cmd);
#if defined(CONFIG_BT_CENTRAL)
WM_CLI_CMD_DEFINE(btfalconnect, cmd_fal_connect, connect with filter accept list enabled cmd, btfalconnect  HELP_ONOFF EXT_ADV_SCAN_OPT -- btfalconnect cmd);
#endif /* CONFIG_BT_CENTRAL */
#endif /* defined(CONFIG_BT_FILTER_ACCEPT_LIST) */
#if defined(CONFIG_BT_FIXED_PASSKEY)
WM_CLI_CMD_DEFINE(btfixedpasskey, cmd_fixed_passkey, input fixed auth passkey cmd, btfixedpasskey <passkey>-- btfixedpasskey cmd);
#endif
#endif /* CONFIG_BT_SMP || CONFIG_BT_BREDR) */
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_CTLR_DTM)
WM_CLI_CMD_DEFINE(bttesttx, cmd_test_tx, test tx cmd, bttesttx <chan> <len> <type> <phy>-- bttesttx cmd);
WM_CLI_CMD_DEFINE(bttestrx, cmd_test_rx, test rx        cmd, bttestrx <chan> <phy> <mod_idx>-- bttestrx cmd);
WM_CLI_CMD_DEFINE(bttestend, cmd_test_end, end test cmd, bttestend -- bttestend cmd);
#endif /* CONFIG_BT_CTLR_DTM */

