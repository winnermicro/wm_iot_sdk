/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>
#include <kernel.h>
#include <sys/printk.h>
#include <sys/util.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

/* Private includes for raw Network & Transport layer access */
#include "mesh/mesh.h"
#include "mesh/net.h"
#include "mesh/rpl.h"
#include "mesh/transport.h"
#include "mesh/foundation.h"
#include "mesh/settings.h"
#include "mesh/access.h"
#include "utils.h"
#include "dfu.h"
#include "blob.h"

#define CID_NVAL   0xffff
#define COMPANY_ID_LF 0x05F1
#define COMPANY_ID_NORDIC_SEMI 0x05F9


struct bt_mesh_shell_target bt_mesh_shell_target_ctx = {
	.dst = BT_MESH_ADDR_UNASSIGNED,
};

/* Default net, app & dev key values, unless otherwise specified */
const uint8_t bt_mesh_shell_default_key[16] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

#if defined(CONFIG_BT_MESH_SHELL_HEALTH_SRV_INSTANCE)
static uint8_t cur_faults[BT_MESH_SHELL_CUR_FAULTS_MAX];
static uint8_t reg_faults[BT_MESH_SHELL_CUR_FAULTS_MAX * 2];

static void get_faults(uint8_t *faults, uint8_t faults_size, uint8_t *dst, uint8_t *count)
{
	uint8_t i, limit = *count;

	for (i = 0U, *count = 0U; i < faults_size && *count < limit; i++) {
		if (faults[i]) {
			*dst++ = faults[i];
			(*count)++;
		}
	}
}

static int fault_get_cur(struct bt_mesh_model *model, uint8_t *test_id,
			 uint16_t *company_id, uint8_t *faults, uint8_t *fault_count)
{
	printf("Sending current faults");

	*test_id = 0x00;
	*company_id = BT_COMP_ID_LF;

	get_faults(cur_faults, sizeof(cur_faults), faults, fault_count);

	return 0;
}

static int fault_get_reg(struct bt_mesh_model *model, uint16_t cid,
			 uint8_t *test_id, uint8_t *faults, uint8_t *fault_count)
{
	if (cid != CONFIG_BT_COMPANY_ID) {
		printf("Faults requested for unknown Company ID 0x%04x\n", cid);
		return -EINVAL;
	}

	printf("Sending registered faults\n");

	*test_id = 0x00;

	get_faults(reg_faults, sizeof(reg_faults), faults, fault_count);

	return 0;
}

static int fault_clear(struct bt_mesh_model *model, uint16_t cid)
{
	if (cid != CONFIG_BT_COMPANY_ID) {
		return -EINVAL;
	}

	(void)memset(reg_faults, 0, sizeof(reg_faults));

	return 0;
}

static int fault_test(struct bt_mesh_model *model, uint8_t test_id,
		      uint16_t cid)
{
	if (cid != CONFIG_BT_COMPANY_ID) {
		return -EINVAL;
	}

	if (test_id != 0x00) {
		return -EINVAL;
	}

	return 0;
}

static void attention_on(struct bt_mesh_model *model)
{
	printf("Attention On\n");
}

static void attention_off(struct bt_mesh_model *model)
{
	printf("Attention Off\n");
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
	.fault_get_cur = fault_get_cur,
	.fault_get_reg = fault_get_reg,
	.fault_clear = fault_clear,
	.fault_test = fault_test,
	.attn_on = attention_on,
	.attn_off = attention_off,
};
#endif /* CONFIG_BT_MESH_SHELL_HEALTH_SRV_INSTANCE */

#ifdef CONFIG_BT_MESH_LARGE_COMP_DATA_SRV
static uint8_t health_tests[] = {
	BT_MESH_HEALTH_TEST_INFO(COMPANY_ID_LF, 6, 0x01, 0x02, 0x03, 0x04, 0x34, 0x15),
	BT_MESH_HEALTH_TEST_INFO(COMPANY_ID_NORDIC_SEMI, 3, 0x01, 0x02, 0x03),
};

static struct bt_mesh_models_metadata_entry health_srv_meta[] = {
	BT_MESH_HEALTH_TEST_INFO_METADATA(health_tests),
	BT_MESH_MODELS_METADATA_END,
};
#endif

struct bt_mesh_health_srv bt_mesh_shell_health_srv = {
#if defined(CONFIG_BT_MESH_SHELL_HEALTH_SRV_INSTANCE)
	.cb = &health_srv_cb,
#endif
#ifdef CONFIG_BT_MESH_LARGE_COMP_DATA_SRV
	.metadata = health_srv_meta,
#endif
};

#if defined(CONFIG_BT_MESH_SHELL_HEALTH_CLI)
static void show_faults(uint8_t test_id, uint16_t cid, uint8_t *faults, size_t fault_count)
{
	size_t i;

	if (!fault_count) {
		printf("Health Test ID 0x%02x Company ID "
				"0x%04x: no faults\n", test_id, cid);
		return;
	}

	printf("Health Test ID 0x%02x Company ID 0x%04x Fault "
			"Count %zu:\n", test_id, cid, fault_count);

	for (i = 0; i < fault_count; i++) {
		printf("\t0x%02x", faults[i]);
	}
    printf("\n");
}

static void health_current_status(struct bt_mesh_health_cli *cli, uint16_t addr,
				  uint8_t test_id, uint16_t cid, uint8_t *faults,
				  size_t fault_count)
{
	printf("Health Current Status from 0x%04x\n", addr);
	show_faults(test_id, cid, faults, fault_count);
}

static void health_fault_status(struct bt_mesh_health_cli *cli, uint16_t addr,
				uint8_t test_id, uint16_t cid, uint8_t *faults,
				size_t fault_count)
{
	printf("Health Fault Status from 0x%04x\n", addr);
	show_faults(test_id, cid, faults, fault_count);
}

static void health_attention_status(struct bt_mesh_health_cli *cli,
				    uint16_t addr, uint8_t attention)
{
	printf("Health Attention Status from 0x%04x: %u\n", addr, attention);
}

static void health_period_status(struct bt_mesh_health_cli *cli, uint16_t addr,
				 uint8_t period)
{
	printf("Health Fast Period Divisor Status from 0x%04x: %u\n", addr, period);
}

struct bt_mesh_health_cli bt_mesh_shell_health_cli = {
	.current_status = health_current_status,
	.fault_status = health_fault_status,
	.attention_status = health_attention_status,
	.period_status = health_period_status,
};
#endif /* CONFIG_BT_MESH_SHELL_HEALTH_CLI */

static void cmd_init(int argc, char *argv[])
{
	printf("Mesh shell initialized\n");

#if defined(CONFIG_BT_MESH_SHELL_DFU_CLI) || defined(CONFIG_BT_MESH_SHELL_DFU_SRV)
	bt_mesh_shell_dfu_cmds_init();
#endif
#if defined(CONFIG_BT_MESH_SHELL_BLOB_CLI) || defined(CONFIG_BT_MESH_SHELL_BLOB_SRV) || \
	defined(CONFIG_BT_MESH_SHELL_BLOB_IO_FLASH)
	bt_mesh_shell_blob_cmds_init();
#endif

	if (IS_ENABLED(CONFIG_BT_MESH_RPR_SRV)) {
		bt_mesh_prov_enable(BT_MESH_PROV_REMOTE);
	}

}

static void cmd_reset(int argc, char *argv[])
{
#if defined(CONFIG_BT_MESH_CDB)
	bt_mesh_cdb_clear();
# endif
	bt_mesh_reset();
	printf("Local node reset complete\n");

}

#if defined(CONFIG_BT_MESH_SHELL_LOW_POWER)
static void cmd_lpn(int argc, char *argv[])
{
	static bool enabled;
	bool onoff;
	int err = 0;

	if (argc < 2) {
		printf("%s\n", enabled ? "enabled" : "disabled");
		return ;
	}

	onoff = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (onoff) {
		if (enabled) {
			printf("LPN already enabled\n");
			return ;
		}

		err = bt_mesh_lpn_set(true);
		if (err) {
			printf("Enabling LPN failed (err %d)\n", err);
		} else {
			enabled = true;
		}
	} else {
		if (!enabled) {
			printf("LPN already disabled\n");
			return ;
		}

		err = bt_mesh_lpn_set(false);
		if (err) {
			printf("Enabling LPN failed (err %d)\n", err);
		} else {
			enabled = false;
		}
	}

}

static void cmd_poll(int argc, char *argv[])
{
	int err;

	err = bt_mesh_lpn_poll();
	if (err) {
		printf("Friend Poll failed (err %d)\n", err);
	}

}

static void lpn_established(uint16_t net_idx, uint16_t friend_addr,
					uint8_t queue_size, uint8_t recv_win)
{
	printf("Friendship (as LPN) established to "
			"Friend 0x%04x Queue Size %d Receive Window %d\n",
			friend_addr, queue_size, recv_win);
}

static void lpn_terminated(uint16_t net_idx, uint16_t friend_addr)
{
	printf("Friendship (as LPN) lost with Friend "
			"0x%04x\n", friend_addr);
}

BT_MESH_LPN_CB_DEFINE(lpn_cb) = {
	.established = lpn_established,
	.terminated = lpn_terminated,
};
#endif /* CONFIG_BT_MESH_SHELL_LOW_POWER */

#if defined(CONFIG_BT_MESH_SHELL_GATT_PROXY)
#if defined(CONFIG_BT_MESH_GATT_PROXY)
static void cmd_ident(int argc, char *argv[])
{
	int err;

	err = bt_mesh_proxy_identity_enable();
	if (err) {
		printf("Failed advertise using Node Identity (err %d)\n", err);
	}
}
#endif /* CONFIG_BT_MESH_GATT_PROXY */

#if defined(CONFIG_BT_MESH_PROXY_CLIENT)
static void cmd_proxy_connect(int argc,
			     char *argv[])
{
	uint16_t net_idx;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return err;
	}

	err = bt_mesh_proxy_connect(net_idx);
	if (err) {
		printf("Proxy connect failed (err %d)\n", err);
	}

}

static void cmd_proxy_disconnect(int argc,
				char *argv[])
{
	uint16_t net_idx;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return err;
	}

	err = bt_mesh_proxy_disconnect(net_idx);
	if (err) {
		printf("Proxy disconnect failed (err %d)\n", err);
	}

}

#if defined(CONFIG_BT_MESH_PROXY_SOLICITATION)
static void cmd_proxy_solicit(int argc,
			     char *argv[])
{
	uint16_t net_idx;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_proxy_solicit(net_idx);
	if (err) {
		printf("Failed to advertise solicitation PDU (err %d)\n",
			    err);
	}

}
#endif /* CONFIG_BT_MESH_PROXY_SOLICITATION */
#endif /* CONFIG_BT_MESH_PROXY_CLIENT */
#endif /* CONFIG_BT_MESH_SHELL_GATT_PROXY */

#if defined(CONFIG_BT_MESH_SHELL_PROV)
static void cmd_input_num(int argc, char *argv[])
{
	int err = 0;
	uint32_t val;

	val = wm_atoi(argv[1],  &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_input_number(val);
	if (err) {
		printf("Numeric input failed (err %d)\n", err);
	}

}

static void cmd_input_str(int argc, char *argv[])
{
	int err = bt_mesh_input_string(argv[1]);

	if (err) {
		printf("String input failed (err %d)\n", err);
	}

}

static const char *bearer2str(bt_mesh_prov_bearer_t bearer)
{
	switch (bearer) {
	case BT_MESH_PROV_ADV:
		return "PB-ADV";
	case BT_MESH_PROV_GATT:
		return "PB-GATT";
	case BT_MESH_PROV_REMOTE:
		return "PB-REMOTE";
	default:
		return "unknown";
	}
}

#if defined(CONFIG_BT_MESH_SHELL_PROV_CTX_INSTANCE)
static uint8_t dev_uuid[16] = { 0xdd, 0xdd };

static void prov_complete(uint16_t net_idx, uint16_t addr)
{

	printf("Local node provisioned, net_idx 0x%04x address "
			"0x%04x\n", net_idx, addr);

	bt_mesh_shell_target_ctx.net_idx = net_idx,
	bt_mesh_shell_target_ctx.dst = addr;
}

static void reprovisioned(uint16_t addr)
{
	printf("Local node re-provisioned, new address 0x%04x\n",
		    addr);

	if (bt_mesh_shell_target_ctx.dst == bt_mesh_primary_addr()) {
		bt_mesh_shell_target_ctx.dst = addr;
	}
}

static void prov_node_added(uint16_t net_idx, uint8_t uuid[16], uint16_t addr,
			    uint8_t num_elem)
{
	printf("Node provisioned, net_idx 0x%04x address "
			"0x%04x elements %d\n", net_idx, addr, num_elem);

	bt_mesh_shell_target_ctx.net_idx = net_idx,
	bt_mesh_shell_target_ctx.dst = addr;
}

#if defined(CONFIG_BT_MESH_PROVISIONER)
static const char * const output_meth_string[] = {
	"Blink",
	"Beep",
	"Vibrate",
	"Display Number",
	"Display String",
};

static const char *const input_meth_string[] = {
	"Push",
	"Twist",
	"Enter Number",
	"Enter String",
};

static void capabilities(const struct bt_mesh_dev_capabilities *cap)
{
	printf("Provisionee capabilities:\n");
	printf("\tStatic OOB is %s supported\n", cap->oob_type & 1 ? "" : "not ");

	printf("\tAvailable output actions (%d bytes max):%s\n", cap->output_size,
			cap->output_actions ? "" : "\n\t\tNone");
	for (int i = 0; i < ARRAY_SIZE(output_meth_string); i++) {
		if (cap->output_actions & BIT(i)) {
			printf("\t\t%s\n", output_meth_string[i]);
		}
	}

	printf("\tAvailable input actions (%d bytes max):%s", cap->input_size,
			cap->input_actions ? "" : "\n\t\tNone");
	for (int i = 0; i < ARRAY_SIZE(input_meth_string); i++) {
		if (cap->input_actions & BIT(i)) {
			printf("\t\t%s\n", input_meth_string[i]);
		}
	}
}
#endif

static void prov_input_complete(void)
{
	printf("Input complete\n");
}

static void prov_reset(void)
{
	printf("The local node has been reset and needs "
			"reprovisioning\n");
}

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
	switch (action) {
	case BT_MESH_BLINK:
		printf("OOB blink Number: %u\n", number);
		break;
	case BT_MESH_BEEP:
		printf("OOB beep Number: %u\n", number);
		break;
	case BT_MESH_VIBRATE:
		printf("OOB vibrate Number: %u\n", number);
		break;
	case BT_MESH_DISPLAY_NUMBER:
		printf("OOB display Number: %u\n", number);
		break;
	default:
		printf("Unknown Output action %u (number %u) requested!\n",
				    action, number);

		return -EINVAL;
	}

	return 0;
}

static int output_string(const char *str)
{
	printf("OOB String: %s\n", str);
	return 0;
}

static int input(bt_mesh_input_action_t act, uint8_t size)
{

	switch (act) {
	case BT_MESH_ENTER_NUMBER:
		printf("Enter a number (max %u digits) with: Input-num <num>\n", size);
		break;
	case BT_MESH_ENTER_STRING:
		printf("Enter a string (max %u chars) with: Input-str <str>\n", size);
		break;
	case BT_MESH_TWIST:
		printf("\"Twist\" a number (max %u digits) with: Input-num <num>\n", size);
		break;
	case BT_MESH_PUSH:
		printf("\"Push\" a number (max %u digits) with: Input-num <num>\n", size);
		break;
	default:
		printf("Unknown Input action %u (size %u) requested!\n", act, size);
		return -EINVAL;
	}

	return 0;
}

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	printf("Provisioning link opened on %s\n", bearer2str(bearer));
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	printf("Provisioning link closed on %s\n", bearer2str(bearer));
}

static uint8_t static_val[32];

struct bt_mesh_prov bt_mesh_shell_prov = {
	.uuid = dev_uuid,
	.link_open = link_open,
	.link_close = link_close,
	.complete = prov_complete,
	.reprovisioned = reprovisioned,
	.node_added = prov_node_added,
	.reset = prov_reset,
	.static_val = NULL,
	.static_val_len = 0,
	.output_size = 6,
	.output_actions = (BT_MESH_BLINK | BT_MESH_BEEP | BT_MESH_VIBRATE | BT_MESH_DISPLAY_NUMBER |
			   BT_MESH_DISPLAY_STRING),
	.output_number = output_number,
	.output_string = output_string,
	.input_size = 6,
	.input_actions =
		(BT_MESH_ENTER_NUMBER | BT_MESH_ENTER_STRING | BT_MESH_TWIST | BT_MESH_PUSH),
	.input = input,
	.input_complete = prov_input_complete,
#if defined(CONFIG_BT_MESH_PROVISIONER)
	.capabilities = capabilities
#endif
};

static void cmd_static_oob(int argc, char *argv[])
{
	if (argc < 2) {
		bt_mesh_shell_prov.static_val = NULL;
		bt_mesh_shell_prov.static_val_len = 0U;
	} else {
		bt_mesh_shell_prov.static_val_len = hex2bin(argv[1], strlen(argv[1]),
					      static_val, 32);
		if (bt_mesh_shell_prov.static_val_len) {
			bt_mesh_shell_prov.static_val = static_val;
		} else {
			bt_mesh_shell_prov.static_val = NULL;
		}
	}

	if (bt_mesh_shell_prov.static_val) {
		printf("Static OOB value set (length %u)\n",
			    bt_mesh_shell_prov.static_val_len);
	} else {
		printf("Static OOB value cleared\n");
	}

}

static void cmd_uuid(int argc, char *argv[])
{
	uint8_t uuid[16];
	size_t len;

	if (argc < 2) {
		char uuid_hex_str[32 + 1];

		bin2hex(dev_uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));

		printf("Device UUID: %s\n", uuid_hex_str);
		return ;
	}

	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	if (len < 1) {
		return ;
	}

	memcpy(dev_uuid, uuid, len);
	(void)memset(dev_uuid + len, 0, sizeof(dev_uuid) - len);

	printf("Device UUID set\n");


}

static void print_unprovisioned_beacon(uint8_t uuid[16],
				       bt_mesh_prov_oob_info_t oob_info,
				       uint32_t *uri_hash)
{
	char uuid_hex_str[32 + 1];

	bin2hex(uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));

	printf("PB-ADV UUID %s, OOB Info 0x%04x, URI Hash 0x%x\n",
			uuid_hex_str, oob_info,
			(uri_hash == NULL ? 0 : *uri_hash));
}

#if defined(CONFIG_BT_MESH_PB_GATT_CLIENT)
static void pb_gatt_unprovisioned(uint8_t uuid[16],
				  bt_mesh_prov_oob_info_t oob_info)
{
	char uuid_hex_str[32 + 1];

	bin2hex(uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));

	printf("PB-GATT UUID %s, OOB Info 0x%04x\n", uuid_hex_str, oob_info);
}
#endif

static void cmd_beacon_listen(int argc,
			     char *argv[])
{
	int err = 0;
	bool val = wm_atoi(argv[1],  &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (val) {
		bt_mesh_shell_prov.unprovisioned_beacon = print_unprovisioned_beacon;
#if defined(CONFIG_BT_MESH_PB_GATT_CLIENT)
		bt_mesh_shell_prov.unprovisioned_beacon_gatt = pb_gatt_unprovisioned;
#endif
	} else {
		bt_mesh_shell_prov.unprovisioned_beacon = NULL;
		bt_mesh_shell_prov.unprovisioned_beacon_gatt = NULL;
	}

}
#endif /* CONFIG_BT_MESH_SHELL_PROV_CTX_INSTANCE */

#if defined(CONFIG_BT_MESH_PB_GATT_CLIENT)
static void cmd_provision_gatt(int argc,
			      char *argv[])
{
	static uint8_t uuid[16];
	uint8_t attention_duration;
	uint16_t net_idx;
	uint16_t addr;
	size_t len;
	int err = 0;

	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	(void)memset(uuid + len, 0, sizeof(uuid) - len);

	net_idx = wm_atoi(argv[2], &err);
	addr = wm_atoi(argv[3],  &err);
	attention_duration = wm_atoi(argv[4],  &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_provision_gatt(uuid, net_idx, addr, attention_duration);
	if (err) {
		printf("Provisioning failed (err %d)\n", err);
	}

}
#endif /* CONFIG_BT_MESH_PB_GATT_CLIENT */

#if defined(CONFIG_BT_MESH_PROV_DEVICE)
static void cmd_pb(bt_mesh_prov_bearer_t bearer, 
		 int argc, char *argv[])
{
	int err = 0;
	bool onoff;

	if (argc < 2) {
        printf("see %s help\n", argv[0]);
		return ;
	}

	onoff = wm_atoi(argv[1],  &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (onoff) {
		err = bt_mesh_prov_enable(bearer);
		if (err) {
			printf("Failed to enable %s (err %d)\n",
				    bearer2str(bearer), err);
		} else {
			printf("%s enabled\n", bearer2str(bearer));
		}
	} else {
		err = bt_mesh_prov_disable(bearer);
		if (err) {
			printf("Failed to disable %s (err %d)\n",
				    bearer2str(bearer), err);
		} else {
			printf("%s disabled\n", bearer2str(bearer));
		}
	}

}

#if defined(CONFIG_BT_MESH_PB_ADV)
static void cmd_pb_adv(int argc, char *argv[])
{
	cmd_pb(BT_MESH_PROV_ADV, argc, argv);
}
#endif /* CONFIG_BT_MESH_PB_ADV */

#if defined(CONFIG_BT_MESH_PB_GATT)
static void cmd_pb_gatt(int argc, char *argv[])
{
	cmd_pb(BT_MESH_PROV_GATT, argc, argv);
}
#endif /* CONFIG_BT_MESH_PB_GATT */
#endif /* CONFIG_BT_MESH_PROV_DEVICE */

#if defined(CONFIG_BT_MESH_PROVISIONER)
static void cmd_remote_pub_key_set(int argc, char *argv[])
{
	size_t len;
	uint8_t pub_key[64];
	int err = 0;

	len = hex2bin(argv[1], strlen(argv[1]), pub_key, sizeof(pub_key));
	if (len < 1) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_prov_remote_pub_key_set(pub_key);

	if (err) {
		printf("Setting remote pub key failed (err %d)\n", err);
	}

}

static void cmd_auth_method_set_input(int argc, char *argv[])
{
	int err = 0;
	bt_mesh_input_action_t action = wm_atoi(argv[1],  &err);
	uint8_t size = wm_atoi(argv[2],  &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_auth_method_set_input(action, size);
	if (err) {
		printf("Setting input OOB authentication action failed (err %d)\n", err);
	}

}

static void cmd_auth_method_set_output(int argc, char *argv[])
{
	int err = 0;
	bt_mesh_output_action_t action = wm_atoi(argv[1],  &err);
	uint8_t size = wm_atoi(argv[2],  &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_auth_method_set_output(action, size);
	if (err) {
		printf("Setting output OOB authentication action failed (err %d)\n", err);
	}

}

static void cmd_auth_method_set_static(int argc, char *argv[])
{
	size_t len;
	uint8_t static_oob_auth[32];
	int err = 0;

	len = hex2bin(argv[1], strlen(argv[1]), static_oob_auth, sizeof(static_oob_auth));
	if (len < 1) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_auth_method_set_static(static_oob_auth, len);
	if (err) {
		printf("Setting static OOB authentication failed (err %d)\n", err);
	}
}

static void cmd_auth_method_set_none(int argc, char *argv[])
{
	int err = bt_mesh_auth_method_set_none();

	if (err) {
		printf("Disabling authentication failed (err %d)\n", err);
	}

}

static void cmd_provision_adv(int argc,
			     char *argv[])
{
	uint8_t uuid[16];
	uint8_t attention_duration;
	uint16_t net_idx;
	uint16_t addr;
	size_t len;
	int err = 0;

	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	(void)memset(uuid + len, 0, sizeof(uuid) - len);

	net_idx = wm_atoi(argv[2], &err);
	addr = wm_atoi(argv[3],  &err);
	attention_duration = wm_atoi(argv[4],  &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_provision_adv(uuid, net_idx, addr, attention_duration);
	if (err) {
		printf("Provisioning failed (err %d)\n", err);
	}

}
#endif /* CONFIG_BT_MESH_PROVISIONER */

static void cmd_provision_local(int argc, char *argv[])
{
	uint8_t *net_key = (uint8_t *)bt_mesh_shell_default_key;
	uint16_t net_idx, addr;
	uint32_t iv_index;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	addr = wm_atoi(argv[2], &err);

	if (argc > 3) {
		iv_index = wm_atoi(argv[3],  &err);
	} else {
		iv_index = 0U;
	}

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_subnet *sub;

		sub = bt_mesh_cdb_subnet_get(net_idx);
		if (!sub) {
			printf("No cdb entry for subnet 0x%03x\n", net_idx);
			return ;
		}

		if (bt_mesh_cdb_subnet_key_export(sub, SUBNET_KEY_TX_IDX(sub), net_key)) {
			printf("Unable to export key for subnet 0x%03x\n", net_idx);
			return ;
		}
	}

	err = bt_mesh_provision(net_key, net_idx, 0, iv_index, addr, bt_mesh_shell_default_key);
	if (err) {
		printf("Provisioning failed (err %d)\n", err);
	}

}

static void cmd_comp_change(int argc, char *argv[])
{
	bt_mesh_comp_change_prepare();
}
#endif /* CONFIG_BT_MESH_SHELL_PROV */

#if defined(CONFIG_BT_MESH_SHELL_TEST)
static void cmd_net_send(int argc, char *argv[])
{
	NET_BUF_SIMPLE_DEFINE(msg, 32);

	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT(bt_mesh_shell_target_ctx.net_idx,
							  bt_mesh_shell_target_ctx.app_idx,
							  bt_mesh_shell_target_ctx.dst,
							  BT_MESH_TTL_DEFAULT);
	struct bt_mesh_net_tx tx = {
		.ctx = &ctx,
		.src = bt_mesh_primary_addr(),
	};

	size_t len;
	int err;

	len = hex2bin(argv[1], strlen(argv[1]),
		      msg.data, net_buf_simple_tailroom(&msg) - 4);
	net_buf_simple_add(&msg, len);

	err = bt_mesh_trans_send(&tx, &msg, NULL, NULL);
	if (err) {
		printf("Failed to send (err %d)\n", err);
	}

}

#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
static void cmd_iv_update(int argc, char *argv[])
{
	if (bt_mesh_iv_update()) {
		printf("Transitioned to IV Update In Progress "
			    "state\n");
	} else {
		printf("Transitioned to IV Update Normal state\n");
	}

	printf("IV Index is 0x%08x\n", bt_mesh.iv_index);

}

static void cmd_iv_update_test(int argc,
			      char *argv[])
{
	int err = 0;
	bool enable;

	enable = wm_atoi(argv[1],  &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (enable) {
		printf("Enabling IV Update test mode\n");
	} else {
		printf("Disabling IV Update test mode\n");
	}

	bt_mesh_iv_update_test(enable);

}
#endif /* CONFIG_BT_MESH_IV_UPDATE_TEST */

static void cmd_rpl_clear(int argc, char *argv[])
{
	bt_mesh_rpl_clear();
}

#if defined(CONFIG_BT_MESH_SHELL_HEALTH_SRV_INSTANCE)
static struct bt_mesh_elem *primary_element(void)
{
	const struct bt_mesh_comp *comp = bt_mesh_comp_get();

	if (comp) {
		return &comp->elem[0];
	}

	return NULL;
}

static void cmd_add_fault(int argc, char *argv[])
{
	uint8_t fault_id;
	uint8_t i;
	struct bt_mesh_elem *elem;
	int err = 0;

	elem = primary_element();
	if (elem == NULL) {
		printf("Element not found!\n");
		return ;
	}

	fault_id = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (!fault_id) {
		printf("The Fault ID must be non-zero!\n");
		return ;
	}

	for (i = 0U; i < sizeof(cur_faults); i++) {
		if (!cur_faults[i]) {
			cur_faults[i] = fault_id;
			break;
		}
	}

	if (i == sizeof(cur_faults)) {
		printf("Fault array is full. Use \"del-fault\" to "
			    "clear it\n");
		return ;
	}

	for (i = 0U; i < sizeof(reg_faults); i++) {
		if (!reg_faults[i]) {
			reg_faults[i] = fault_id;
			break;
		}
	}

	if (i == sizeof(reg_faults)) {
		printf("No space to store more registered faults\n");
	}

	bt_mesh_health_srv_fault_update(elem);

}

static void cmd_del_fault(int argc, char *argv[])
{
	uint8_t fault_id;
	uint8_t i;
	struct bt_mesh_elem *elem;
	int err = 0;

	elem = primary_element();
	if (elem == NULL) {
		printf("Element not found!\n");
		return ;
	}

	if (argc < 2) {
		(void)memset(cur_faults, 0, sizeof(cur_faults));
		printf("All current faults cleared\n");
		bt_mesh_health_srv_fault_update(elem);
		return ;
	}

	fault_id = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (!fault_id) {
		printf("The Fault ID must be non-zero!\n");
		return ;
	}

	for (i = 0U; i < sizeof(cur_faults); i++) {
		if (cur_faults[i] == fault_id) {
			cur_faults[i] = 0U;
			printf("Fault cleared\n");
		}
	}

	bt_mesh_health_srv_fault_update(elem);

}
#endif /* CONFIG_BT_MESH_SHELL_HEALTH_SRV_INSTANCE */
#endif /* CONFIG_BT_MESH_SHELL_TEST */

#if defined(CONFIG_BT_MESH_SHELL_CDB)
static void cmd_cdb_create(int argc,
			  char *argv[])
{
	uint8_t net_key[16];
	size_t len;
	int err;

	if (argc < 2) {
		bt_rand(net_key, 16);
	} else {
		len = hex2bin(argv[1], strlen(argv[1]), net_key,
			      sizeof(net_key));
		memset(net_key + len, 0, sizeof(net_key) - len);
	}

	err = bt_mesh_cdb_create(net_key);
	if (err < 0) {
		printf("Failed to create CDB (err %d)\n", err);
	}

}

static void cmd_cdb_clear(int argc,
			 char *argv[])
{
	bt_mesh_cdb_clear();

	printf("Cleared CDB\n");

}

static void cdb_print_nodes(void)
{
	char key_hex_str[32 + 1], uuid_hex_str[32 + 1];
	struct bt_mesh_cdb_node *node;
	int i, total = 0;
	bool configured;
	uint8_t dev_key[16];

	printf("Address  Elements  Flags  %-32s  DevKey\n", "UUID");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); ++i) {
		node = &bt_mesh_cdb.nodes[i];
		if (node->addr == BT_MESH_ADDR_UNASSIGNED) {
			continue;
		}

		configured = atomic_test_bit(node->flags,
					     BT_MESH_CDB_NODE_CONFIGURED);

		total++;
		bin2hex(node->uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));
		if (bt_mesh_cdb_node_key_export(node, dev_key)) {
			printf("Unable to export key for node 0x%04x", node->addr);
			continue;
		}
		bin2hex(dev_key, 16, key_hex_str, sizeof(key_hex_str));
		printf("0x%04x   %-8d  %-5s  %s  %s\n", node->addr,
			    node->num_elem, configured ? "C" : "-",
			    uuid_hex_str, key_hex_str);
	}

	printf("\n> Total nodes: %d\n", total);
}

static void cdb_print_subnets()
{
	struct bt_mesh_cdb_subnet *subnet;
	char key_hex_str[32 + 1];
	int i, total = 0;
	uint8_t net_key[16];

	printf("NetIdx  NetKey\n");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.subnets); ++i) {
		subnet = &bt_mesh_cdb.subnets[i];
		if (subnet->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		if (bt_mesh_cdb_subnet_key_export(subnet, 0, net_key)) {
			printf("Unable to export key for subnet 0x%03x\n",
					subnet->net_idx);
			continue;
		}

		total++;
		bin2hex(net_key, 16, key_hex_str, sizeof(key_hex_str));
		printf("0x%03x   %s\n", subnet->net_idx, key_hex_str);
	}

	printf("\n> Total subnets: %d\n", total);
}

static void cdb_print_app_keys()
{
	struct bt_mesh_cdb_app_key *key;
	char key_hex_str[32 + 1];
	int i, total = 0;
	uint8_t app_key[16];

	printf("NetIdx  AppIdx  AppKey\n");

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.app_keys); ++i) {
		key = &bt_mesh_cdb.app_keys[i];
		if (key->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		if (bt_mesh_cdb_app_key_export(key, 0, app_key)) {
			printf("Unable to export app key 0x%03x\n", key->app_idx);
			continue;
		}

		total++;
		bin2hex(app_key, 16, key_hex_str, sizeof(key_hex_str));
		printf("0x%03x   0x%03x   %s\n", key->net_idx, key->app_idx, key_hex_str);
	}

	printf("\n> Total app-keys: %d\n", total);
}

static void cmd_cdb_show(int argc,
			char *argv[])
{
	if (!atomic_test_bit(bt_mesh_cdb.flags, BT_MESH_CDB_VALID)) {
		printf("No valid networks\n");
		return ;
	}

	printf("Mesh Network Information\n");
	printf("========================\n");

	cdb_print_nodes();
	printf("---\n");
	cdb_print_subnets();
	printf("---\n");
	cdb_print_app_keys();

}

static void cmd_cdb_node_add(int argc,
			    char *argv[])
{
	struct bt_mesh_cdb_node *node;
	uint8_t uuid[16], dev_key[16];
	uint16_t addr, net_idx;
	uint8_t num_elem;
	size_t len;
	int err = 0;

	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	memset(uuid + len, 0, sizeof(uuid) - len);

	addr = wm_atoi(argv[2], &err);
	num_elem = wm_atoi(argv[3],  &err);
	net_idx = wm_atoi(argv[4],  &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc < 6) {
		bt_rand(dev_key, 16);
	} else {
		len = hex2bin(argv[5], strlen(argv[5]), dev_key,
			      sizeof(dev_key));
		memset(dev_key + len, 0, sizeof(dev_key) - len);
	}

	node = bt_mesh_cdb_node_alloc(uuid, addr, num_elem, net_idx);
	if (node == NULL) {
		printf("Failed to allocate node\n");
		return ;
	}

	err = bt_mesh_cdb_node_key_import(node, dev_key);
	if (err) {
		printf("Unable to import device key into cdb\n");
		return ;
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(node);
	}

	printf("Added node 0x%04x\n", node->addr);

}

static void cmd_cdb_node_del(int argc,
			    char *argv[])
{
	struct bt_mesh_cdb_node *node;
	uint16_t addr;
	int err = 0;

	addr = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	node = bt_mesh_cdb_node_get(addr);
	if (node == NULL) {
		printf("No node with address 0x%04x\n", addr);
		return ;
	}

	bt_mesh_cdb_node_del(node, true);

	printf("Deleted node 0x%04x\n", addr);

}

static void cmd_cdb_subnet_add(int argc,
			     char *argv[])
{
	struct bt_mesh_cdb_subnet *sub;
	uint8_t net_key[16];
	uint16_t net_idx;
	size_t len;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc < 3) {
		bt_rand(net_key, 16);
	} else {
		len = hex2bin(argv[2], strlen(argv[2]), net_key,
			      sizeof(net_key));
		memset(net_key + len, 0, sizeof(net_key) - len);
	}

	sub = bt_mesh_cdb_subnet_alloc(net_idx);
	if (sub == NULL) {
		printf("Could not add subnet\n");
		return ;
	}

	if (bt_mesh_cdb_subnet_key_import(sub, 0, net_key)) {
		printf("Unable to import key for subnet 0x%03x\n", net_idx);
		return ;
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_subnet_store(sub);
	}

	printf("Added Subnet 0x%03x\n", net_idx);

}

static void cmd_cdb_subnet_del(int argc,
			     char *argv[])
{
	struct bt_mesh_cdb_subnet *sub;
	uint16_t net_idx;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	sub = bt_mesh_cdb_subnet_get(net_idx);
	if (sub == NULL) {
		printf("No subnet with NetIdx 0x%03x\n", net_idx);
		return ;
	}

	bt_mesh_cdb_subnet_del(sub, true);

	printf("Deleted subnet 0x%03x\n", net_idx);

}

static void cmd_cdb_app_key_add(int argc,
			      char *argv[])
{
	struct bt_mesh_cdb_app_key *key;
	uint16_t net_idx, app_idx;
	uint8_t app_key[16];
	size_t len;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	app_idx = wm_atoi(argv[2], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc < 4) {
		bt_rand(app_key, 16);
	} else {
		len = hex2bin(argv[3], strlen(argv[3]), app_key,
			      sizeof(app_key));
		memset(app_key + len, 0, sizeof(app_key) - len);
	}

	key = bt_mesh_cdb_app_key_alloc(net_idx, app_idx);
	if (key == NULL) {
		printf("Could not add AppKey\n");
		return ;
	}

	if (bt_mesh_cdb_app_key_import(key, 0, app_key)) {
		printf("Unable to import app key 0x%03x\n", app_idx);
		return ;
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_app_key_store(key);
	}

	printf("Added AppKey 0x%03x\n", app_idx);

	return ;
}

static void cmd_cdb_app_key_del(int argc,
			      char *argv[])
{
	struct bt_mesh_cdb_app_key *key;
	uint16_t app_idx;
	int err = 0;

	app_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		printf("No AppKey 0x%03x\n", app_idx);
		return ;
	}

	bt_mesh_cdb_app_key_del(key, true);

	printf("Deleted AppKey 0x%03x\n", app_idx);

}
#endif /* CONFIG_BT_MESH_SHELL_CDB */

static void cmd_dst(int argc, char *argv[])
{
	int err = 0;

	if (argc < 2) {
		printf("Destination address: 0x%04x%s\n", bt_mesh_shell_target_ctx.dst,
			    bt_mesh_shell_target_ctx.dst == bt_mesh_primary_addr()
				    ? " (local)"
				    : "");
		return ;
	}

	if (!strcmp(argv[1], "local")) {
		bt_mesh_shell_target_ctx.dst = bt_mesh_primary_addr();
	} else {
		bt_mesh_shell_target_ctx.dst = wm_atoi(argv[1], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}
	}

	printf("Destination address set to 0x%04x%s\n", bt_mesh_shell_target_ctx.dst,
		    bt_mesh_shell_target_ctx.dst == bt_mesh_primary_addr() ? " (local)"
										   : "");
}

static void cmd_netidx(int argc, char *argv[])
{
	int err = 0;

	if (argc < 2) {
		printf("NetIdx: 0x%04x\n", bt_mesh_shell_target_ctx.net_idx);
		return ;
	}

	bt_mesh_shell_target_ctx.net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	printf("NetIdx set to 0x%04x\n", bt_mesh_shell_target_ctx.net_idx);
}

static void cmd_appidx(int argc, char *argv[])
{
	int err = 0;

	if (argc < 2) {
		printf("AppIdx: 0x%04x\n", bt_mesh_shell_target_ctx.app_idx);
		return ;
	}

	bt_mesh_shell_target_ctx.app_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	printf("AppIdx set to 0x%04x\n", bt_mesh_shell_target_ctx.app_idx);
}

#if defined(CONFIG_BT_MESH_STATISTIC)
static void cmd_stat_get(int argc, char *argv[])
{
	struct bt_mesh_statistic st;

	bt_mesh_stat_get(&st);

	printf("Received frames over:\n");
	printf("adv:       %d\n", st.rx_adv);
	printf("loopback:  %d\n", st.rx_loopback);
	printf("proxy:     %d\n", st.rx_proxy);
	printf("unknown:   %d\n", st.rx_uknown);

	printf("Transmitted frames: <planned> - <succeeded>\n");
	printf("relay adv:   %d - %d\n", st.tx_adv_relay_planned, st.tx_adv_relay_succeeded);
	printf("local adv:   %d - %d\n", st.tx_local_planned, st.tx_local_succeeded);
	printf("friend:      %d - %d\n", st.tx_friend_planned, st.tx_friend_succeeded);

}

static void cmd_stat_clear(int argc, char *argv[])
{
	bt_mesh_stat_reset();

}
#endif


/* List of Mesh subcommands.
 *
 * Each command is documented in doc/reference/bluetooth/mesh/shell.rst.
 *
 * Please keep the documentation up to date by adding any new commands to the
 * list.
 */

#if defined(CONFIG_BT_MESH_SHELL_CDB)
	/* Mesh Configuration Database Operations */
    WM_CLI_CMD_DEFINE(mesh_cdb_create, cmd_cdb_create, create configuration database, mesh_cdb_create [NetKey(1-16 hex)] -- mesh_cdb_create cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_clear, cmd_cdb_clear, clear configuration database, mesh_cdb_clear -- mesh_cdb_clear cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_show, cmd_cdb_show, display configuration database, mesh_cdb_show -- mesh_cdb_show cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_node_add, cmd_cdb_node_add, add a node to configuration database, mesh_cdb_node_add <UUID(1-16 hex)> <Addr> <ElemCnt> <NetKeyIdx> [DevKey(1-16 hex)]-- mesh_cdb_node_add cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_node_del, cmd_cdb_node_del, remove a node from configuration database, mesh_cdb_node_del <Addr> -- mesh_cdb_node_del cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_subnet_add, cmd_cdb_subnet_add, add a subnet to configuration database, mesh_cdb_subnet_add <NetKeyIdx> [<NetKey(1-16 hex)>]-- mesh_cdb_subnet_add cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_subnet_del, cmd_cdb_subnet_del, remove a subnet from configuration database, mesh_cdb_subnet_del <NetKeyIdx> -- mesh_cdb_subnet_del cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_appkey_add, cmd_cdb_app_key_add, add a appkey to configuration database, mesh_cdb_appkey_add <NetKeyIdx> <AppKeyIdx> [<AppKey(1-16 hex)>]-- mesh_cdb_appkey_add cmd);
    WM_CLI_CMD_DEFINE(mesh_cdb_appkey_del, cmd_cdb_app_key_del, remove a appkey from configuration database, mesh_cdb_appkey_del <AppKeyIdx> -- mesh_cdb_appkey_del cmd);
#endif

#if defined(CONFIG_BT_MESH_SHELL_PROV)

    WM_CLI_CMD_DEFINE(mesh_prov_input_num, cmd_input_num, input number for provision auth, mesh_prov_input_num <Number> -- mesh_prov_input_num cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_input_str, cmd_input_str, input string for provision auth, mesh_prov_input_str <String> -- mesh_prov_input_str cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_local, cmd_provision_local, provision local node, mesh_prov_local <NetKeyIdx> <Addr> [IVI] -- mesh_prov_local cmd);
#if defined(CONFIG_BT_MESH_SHELL_PROV_CTX_INSTANCE)
    WM_CLI_CMD_DEFINE(mesh_prov_static_oob, cmd_static_oob, input static oob data, mesh_prov_static_oob [Val] -- mesh_prov_static_oob cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_uuid, cmd_uuid, input device uuid, mesh_prov_uuid [UUID(1-16 hex)] -- mesh_prov_uuid cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_beacon_listen, cmd_beacon_listen, turn on off beacon listen, mesh_prov_beacon_listen <Val(0, 1)> -- mesh_prov_beacon_listen cmd);
#endif
    WM_CLI_CMD_DEFINE(mesh_prov_comp_change, cmd_comp_change, store composition        data , mesh_prov_comp_change -- mesh_prov_comp_change cmd);

    /* Provisioning operations */
#if defined(CONFIG_BT_MESH_PROV_DEVICE)
#if defined(CONFIG_BT_MESH_PB_GATT)
    WM_CLI_CMD_DEFINE(mesh_prov_pb_gatt, cmd_pb_gatt, turn on off gatt provision beacon, mesh_prov_pb_gatt <Val(0, 1)> -- mesh_prov_pb_gatt cmd);
#endif
#if defined(CONFIG_BT_MESH_PB_ADV)
    WM_CLI_CMD_DEFINE(mesh_prov_pb_adv, cmd_pb_adv, turn on off advertisement provision beacon, mesh_prov_pb_adv <Val(0, 1)> -- mesh_prov_pb_adv cmd);
#endif
#endif /* CONFIG_BT_MESH_PROV_DEVICE */

#if defined(CONFIG_BT_MESH_PROVISIONER)
    WM_CLI_CMD_DEFINE(mesh_prov_auth_method_input, cmd_auth_method_set_input, input auth, mesh_prov_auth_method_input <Action> <Size> -- mesh_prov_auth_method_input cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_auth_method_output, cmd_auth_method_set_output, output auth, mesh_prov_auth_method_output <Action> <Size> -- mesh_prov_auth_method_output cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_auth_method_static, cmd_auth_method_set_static, set static auth, mesh_prov_auth_method_static <Val(1-16 hex)> -- mesh_prov_auth_method_static cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_auth_method_none, cmd_auth_method_set_none, set none auth, mesh_prov_auth_method_none  -- mesh_prov_auth_method_none cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_remote_pub_key, cmd_remote_pub_key_set, set remote public key, mesh_prov_remote_pub_key <PubKey> -- mesh_prov_remote_pub_key cmd);
    WM_CLI_CMD_DEFINE(mesh_prov_remote_adv, cmd_provision_adv, provision remote by adv, mesh_prov_remote_adv <UUID(1-16 hex)> <NetKeyIdx> <Addr> <AttDur(s)> -- mesh_prov_remote_adv cmd);
#endif

#if defined(CONFIG_BT_MESH_PB_GATT_CLIENT)
    WM_CLI_CMD_DEFINE(mesh_prov_remote_gatt, cmd_provision_gatt, provision remote by gatt connection, mesh_prov_remote_gatt <UUID(1-16 hex)> <NetKeyIdx> <Addr> <AttDur(s)> -- mesh_prov_remote_gatt cmd);
#endif
#endif

#if defined(CONFIG_BT_MESH_SHELL_TEST)
#if defined(CONFIG_BT_MESH_SHELL_HEALTH_SRV_INSTANCE)
    WM_CLI_CMD_DEFINE(mesh_test_health_srv_add_default, cmd_add_fault, add default id to health service, mesh_test_health_srv_add_default <FaultID> -- mesh_test_health_srv_add_default cmd);
    WM_CLI_CMD_DEFINE(mesh_test_health_srv_del_default, cmd_del_fault, del        default id to health service, mesh_test_health_srv_del_default [FaultID] -- mesh_test_health_srv_del_default cmd);
#endif
    WM_CLI_CMD_DEFINE(mesh_test_net_send, cmd_net_send, send net cmd, mesh_test_net_send <HexString> -- mesh_test_net_send cmd);
#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
    WM_CLI_CMD_DEFINE(mesh_test_iv_update, cmd_iv_update, update network iv, mesh_test_iv_update -- mesh_test_iv_update cmd);
    WM_CLI_CMD_DEFINE(mesh_test_iv_update_test, cmd_iv_update_test, update network test iv, mesh_test_iv_update_test <Val(0, 1)>-- mesh_test_iv_update_test cmd);
#endif
    WM_CLI_CMD_DEFINE(mesh_test_rpl_clear, cmd_rpl_clear, clear replay list db, mesh_test_rpl_clear -- mesh_test_rpl_clear cmd);

#endif /* CONFIG_BT_MESH_SHELL_TEST */



#if defined(CONFIG_BT_MESH_SHELL_GATT_PROXY)
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    WM_CLI_CMD_DEFINE(mesh_proxy_identity_enable, cmd_ident, enable mesh advertisement using identity, mesh_proxy_identity_enable -- mesh_proxy_identity_enable cmd);
#endif

#if defined(CONFIG_BT_MESH_PROXY_CLIENT)
    WM_CLI_CMD_DEFINE(mesh_proxy_connect, cmd_proxy_connect, connect remote node by gatt , mesh_proxy_connect <NetKeyIdx> -- mesh_proxy_connect cmd);
    WM_CLI_CMD_DEFINE(mesh_proxy_disconnect, cmd_proxy_disconnect, dis gatt connect , mesh_proxy_disconnect <NetKeyIdx> -- mesh_proxy_disconnect cmd);

#if defined(CONFIG_BT_MESH_PROXY_SOLICITATION)
    WM_CLI_CMD_DEFINE(mesh_proxy_solicit, cmd_proxy_solicit, advertise solicitation pdu , mesh_proxy_solicit <NetKeyIdx> -- mesh_proxy_solicit cmd);
#endif
#endif
#endif /* CONFIG_BT_MESH_SHELL_GATT_PROXY */

#if defined(CONFIG_BT_MESH_SHELL_LOW_POWER)
    WM_CLI_CMD_DEFINE(mesh_lpn_set, cmd_lpn, turn on or off low power mode , mesh_lpn_set <Val(0, 1)> -- mesh_lpn_set cmd);
    WM_CLI_CMD_DEFINE(mesh_lpn_poll, cmd_poll, friend poll mode , mesh_lpn_poll -- mesh_lpn_poll cmd);
#endif

    WM_CLI_CMD_DEFINE(mesh_target_dst, cmd_dst, set        destination address , mesh_target_dst [DstAddr] -- mesh_target_dst cmd);
    WM_CLI_CMD_DEFINE(mesh_target_net, cmd_netidx, set        network key index , mesh_target_net [NetKeyIdx] -- mesh_target_net cmd);
    WM_CLI_CMD_DEFINE(mesh_target_app, cmd_appidx, set        application key index , mesh_target_app [AppKeyIdx] -- mesh_target_app cmd);

#if defined(CONFIG_BT_MESH_STATISTIC)
    WM_CLI_CMD_DEFINE(mesh_stat_get, cmd_stat_get, get statiscation info , mesh_stat_get  -- mesh_stat_get cmd);
    WM_CLI_CMD_DEFINE(mesh_stat_clear, cmd_stat_clear, clear statiscation info , mesh_stat_clear  -- mesh_stat_clear cmd);
#endif

    WM_CLI_CMD_DEFINE(mesh_init, cmd_init, initialise        mesh profile , mesh_init  -- mesh_init cmd);
    WM_CLI_CMD_DEFINE(mesh_reset_local, cmd_reset, reset        local mesh profile , mesh_reset_local  -- mesh_reset_local cmd);


