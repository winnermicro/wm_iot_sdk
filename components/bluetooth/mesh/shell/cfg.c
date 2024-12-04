/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>

#include "mesh/net.h"
#include "mesh/access.h"
#include "utils.h"
#include <bluetooth/mesh/shell.h>

#define CID_NVAL 0xffff

/* Default net & app key values, unless otherwise specified */
extern const uint8_t bt_mesh_shell_default_key[16];

static void cmd_reset(int argc, char *argv[])
{
	int err;
	bool reset = false;

	err = bt_mesh_cfg_cli_node_reset(bt_mesh_shell_target_ctx.net_idx,
					 bt_mesh_shell_target_ctx.dst, &reset);
	if (err) {
		printf("Unable to send Remote Node Reset (err %d)\n", err);
		return ;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_node *node = bt_mesh_cdb_node_get(bt_mesh_shell_target_ctx.dst);

		if (node) {
			bt_mesh_cdb_node_del(node, true);
		}
	}

	printf("Remote node reset complete\n");

}

static void cmd_timeout(int argc, char *argv[])
{
	int32_t timeout_ms;
	int err = 0;

	if (argc == 2) {
		int32_t timeout_s = wm_atoi(argv[1], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		if (timeout_s < 0 || timeout_s > (INT32_MAX / 1000)) {
			timeout_ms = SYS_FOREVER_MS;
		} else {
			timeout_ms = timeout_s * MSEC_PER_SEC;
		}

		bt_mesh_cfg_cli_timeout_set(timeout_ms);
	}

	timeout_ms = bt_mesh_cfg_cli_timeout_get();
	if (timeout_ms == SYS_FOREVER_MS) {
		printf("Message timeout: forever\n");
	} else {
		printf("Message timeout: %u seconds\n", timeout_ms / 1000);
	}

}

static void cmd_get_comp(int argc, char *argv[])
{
	NET_BUF_SIMPLE_DEFINE(buf, BT_MESH_RX_SDU_MAX);
	struct bt_mesh_comp_p0_elem elem;
	struct bt_mesh_comp_p0 comp;
	uint8_t page = 0x00;
	int err = 0;

	if (argc > 1) {
		page = wm_atoi(argv[1], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}
	}

	err = bt_mesh_cfg_cli_comp_data_get(bt_mesh_shell_target_ctx.net_idx,
					bt_mesh_shell_target_ctx.dst, page, &page, &buf);
	if (err) {
		printf("Getting composition failed (err %d)\n", err);
		return ;
	}

	if (page != 0 && page != 128 &&
	    ((page != 1 && page != 129) || !IS_ENABLED(CONFIG_BT_MESH_COMP_PAGE_1)) &&
	    ((page != 2 && page != 130) || !IS_ENABLED(CONFIG_BT_MESH_COMP_PAGE_2))) {
		printf("Got page %d. No parser available.\n", page);
		return ;
	}

	if (page == 0 || page == 128) {
		err = bt_mesh_comp_p0_get(&comp, &buf);

		if (err) {
			printf("Couldn't parse Composition data (err %d)\n",
				    err);
			return ;
		}

		printf("Got Composition Data for 0x%04x, page: %d:\n",
			    bt_mesh_shell_target_ctx.dst, page);
		printf("\tCID      0x%04x\n", comp.cid);
		printf("\tPID      0x%04x\n", comp.pid);
		printf("\tVID      0x%04x\n", comp.vid);
		printf("\tCRPL     0x%04x\n", comp.crpl);
		printf("\tFeatures 0x%04x\n", comp.feat);

		while (bt_mesh_comp_p0_elem_pull(&comp, &elem)) {
			int i;

			printf("\tElement @ 0x%04x:", elem.loc);

			if (elem.nsig) {
				printf("\t\tSIG Models:");
			} else {
				printf("\t\tNo SIG Models");
			}

			for (i = 0; i < elem.nsig; i++) {
				uint16_t mod_id = bt_mesh_comp_p0_elem_mod(&elem, i);

				printf("\t\t\t0x%04x", mod_id);
			}

			if (elem.nvnd) {
				printf("\t\tVendor Models:");
			} else {
				printf("\t\tNo Vendor Models\n");
			}

			for (i = 0; i < elem.nvnd; i++) {
				struct bt_mesh_mod_id_vnd mod =
					bt_mesh_comp_p0_elem_mod_vnd(&elem, i);

				printf("\t\t\tCompany 0x%04x: 0x%04x\n",
					    mod.company, mod.id);
			}
		}
	}

	if (IS_ENABLED(CONFIG_BT_MESH_COMP_PAGE_1) && (page == 1 || page == 129)) {
		/* size of 32 is chosen arbitrary, as sufficient for testing purposes */
		NET_BUF_SIMPLE_DEFINE(p1_buf, 32);
		NET_BUF_SIMPLE_DEFINE(p1_item_buf, 32);
		struct bt_mesh_comp_p1_elem p1_elem = { ._buf = &p1_buf };
		struct bt_mesh_comp_p1_model_item mod_item = { ._buf = &p1_item_buf };
		struct bt_mesh_comp_p1_ext_item ext_item = { 0 };
		int mod_idx = 1;

		if (!buf.len) {
			printf("Composition data empty\n");
			return ;
		}
		printf("Got Composition Data for 0x%04x, page: %d:",
			    bt_mesh_shell_target_ctx.dst, page);

		while (bt_mesh_comp_p1_elem_pull(&buf, &p1_elem)) {
			int i, j;

			printf("\tElement #%d description", mod_idx);

			for (i = 0; i < p1_elem.nsig; i++) {
				if (bt_mesh_comp_p1_item_pull(&p1_elem, &mod_item)) {
					printf("\t\tSIG Model Item #%d:", i+1);
					if (mod_item.cor_present) {
						printf(
							    "\t\t\tWith Corresponding ID %u",
							    mod_item.cor_id);
					} else {
						printf(
							    "\t\t\tWithout Corresponding ID");
					}
					printf(
						    "\t\t\tWith %u Extended Model Item(s)",
						    mod_item.ext_item_cnt);
				}
				for (j = 0; j < mod_item.ext_item_cnt; j++) {
					bt_mesh_comp_p1_pull_ext_item(&mod_item,
								      &ext_item);
					printf(
						    "\t\t\t\tExtended Item #%d:", j+1);
					if (ext_item.type == SHORT) {
						printf(
							    "\t\t\t\t\toffset: %u",
							    ext_item.short_item.elem_offset);
						printf(
							    "\t\t\t\t\tindex: %u",
							    ext_item.short_item.mod_item_idx);
					} else {
						printf(
							    "\t\t\t\t\toffset: %u",
							    ext_item.long_item.elem_offset);
						printf(
							    "\t\t\t\t\tindex: %u",
							    ext_item.long_item.mod_item_idx);
					}
				}
			}
			for (i = 0; i < p1_elem.nvnd; i++) {
				if (bt_mesh_comp_p1_item_pull(&p1_elem, &mod_item)) {
					printf("\t\tVendor Model Item #%d:", i+1);
					if (mod_item.cor_present) {
						printf(
							    "\t\t\tWith Corresponding ID %u",
							    mod_item.cor_id);
					} else {
						printf(
							    "\t\t\tWithout Corresponding ID");
					}
					printf(
						    "\t\t\tWith %u Extended Model Item(s)",
						    mod_item.ext_item_cnt);
				}
				for (j = 0; j < mod_item.ext_item_cnt; j++) {
					bt_mesh_comp_p1_pull_ext_item(&mod_item,
								      &ext_item);
					printf(
						    "\t\t\t\tExtended Item #%d:", j+1);
					if (ext_item.type == SHORT) {
						printf(
							    "\t\t\t\t\toffset: %u",
							    ext_item.short_item.elem_offset);
						printf(
							    "\t\t\t\t\tindex: %u",
							    ext_item.short_item.mod_item_idx);
					} else {
						printf(
							    "\t\t\t\t\toffset: %u",
							    ext_item.long_item.elem_offset);
						printf(
							    "\t\t\t\t\tindex: %u",
							    ext_item.long_item.mod_item_idx);
					}
				}
			}
			mod_idx++;
		}
	}

	if (IS_ENABLED(CONFIG_BT_MESH_COMP_PAGE_2) && (page == 2 || page == 130)) {
		/* size of 32 is chosen arbitrary, as sufficient for testing purposes */
		NET_BUF_SIMPLE_DEFINE(p2_elem_offset_buf, 32);
		NET_BUF_SIMPLE_DEFINE(p2_data_buf, 32);
		struct bt_mesh_comp_p2_record p2_elem = {
			.elem_buf = &p2_elem_offset_buf,
			.data_buf = &p2_data_buf
		};

		if (!buf.len) {
			printf("Composition data empty\n");
			return;
		}
		printf("Got Composition Data for 0x%04x, page: %d:\n",
			    bt_mesh_shell_target_ctx.dst, page);

		while (bt_mesh_comp_p2_record_pull(&buf, &p2_elem)) {

			printf("\tMesh Profile id: %04x ", p2_elem.id);
			printf("\t\tVersion: %d.%d.%d ", p2_elem.version.x,
				    p2_elem.version.y, p2_elem.version.z);
			printf("\t\tElement offsets:");

			while (p2_elem.elem_buf->len) {
				printf("\t\t\t%d ",
					    net_buf_simple_pull_u8(p2_elem.elem_buf));
			}

			if (p2_elem.data_buf->len) {
				printf("\t\t%d bytes of additional data is available",
					    p2_elem.data_buf->len);
			}
		}
	}

	if (buf.len) {
		printf("\t\t...truncated data!\n");
	}

}

static void cmd_beacon(int argc, char *argv[])
{
	uint8_t status;
	int err = 0;

	if (argc < 2) {
		err = bt_mesh_cfg_cli_beacon_get(bt_mesh_shell_target_ctx.net_idx,
					     bt_mesh_shell_target_ctx.dst, &status);
	} else {
		uint8_t val = wm_atoi(argv[1], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_beacon_set(bt_mesh_shell_target_ctx.net_idx,
					     bt_mesh_shell_target_ctx.dst, val, &status);
	}

	if (err) {
		printf("Unable to send Beacon Get/Set message (err %d)\n", err);
		return ;
	}

	printf("Beacon state is 0x%02x\n", status);

}

static void cmd_ttl(int argc, char *argv[])
{
	uint8_t ttl;
	int err = 0;

	if (argc < 2) {
		err = bt_mesh_cfg_cli_ttl_get(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst, &ttl);
	} else {
		uint8_t val = wm_atoi(argv[1], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_ttl_set(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst, val, &ttl);
	}

	if (err) {
		printf("Unable to send Default TTL Get/Set (err %d)\n", err);
		return ;
	}

	printf("Default TTL is 0x%02x\n", ttl);

}

static void cmd_friend(int argc, char *argv[])
{
	uint8_t frnd;
	int err = 0;

	if (argc < 2) {
		err = bt_mesh_cfg_cli_friend_get(bt_mesh_shell_target_ctx.net_idx,
					     bt_mesh_shell_target_ctx.dst, &frnd);
	} else {
		uint8_t val = wm_atoi(argv[1], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_friend_set(bt_mesh_shell_target_ctx.net_idx,
					     bt_mesh_shell_target_ctx.dst, val, &frnd);
	}

	if (err) {
		printf("Unable to send Friend Get/Set (err %d)\n", err);
		return ;
	}

	printf("Friend is set to 0x%02x\n", frnd);

}

static void cmd_gatt_proxy(int argc, char *argv[])
{
	uint8_t proxy;
	int err = 0;

	if (argc < 2) {
		err = bt_mesh_cfg_cli_gatt_proxy_get(bt_mesh_shell_target_ctx.net_idx,
						 bt_mesh_shell_target_ctx.dst, &proxy);
	} else {
		uint8_t val = wm_atoi(argv[1], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_gatt_proxy_set(bt_mesh_shell_target_ctx.net_idx,
						 bt_mesh_shell_target_ctx.dst, val, &proxy);
	}

	if (err) {
		printf("Unable to send GATT Proxy Get/Set (err %d)\n", err);
		return ;
	}

	printf("GATT Proxy is set to 0x%02x\n", proxy);

}

static void cmd_polltimeout_get(int argc, char *argv[])
{
	uint16_t lpn_address;
	int32_t poll_timeout;
	int err = 0;

	lpn_address = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_cfg_cli_lpn_timeout_get(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst, lpn_address, &poll_timeout);
	if (err) {
		printf("Unable to send LPN PollTimeout Get (err %d)\n", err);
		return ;
	}

	printf("PollTimeout value %d\n", poll_timeout);

}

static void cmd_net_transmit(int argc, char *argv[])
{
	uint8_t transmit;
	int err = 0;

	if (argc < 2) {
		err = bt_mesh_cfg_cli_net_transmit_get(bt_mesh_shell_target_ctx.net_idx,
						   bt_mesh_shell_target_ctx.dst, &transmit);
	} else {
		if (argc != 3) {
			printf("Wrong number of input arguments"
					"(2 arguments are required)\n");
			return ;
		}

		uint8_t count, interval, new_transmit;

		count = wm_atoi(argv[1], &err);
		interval = wm_atoi(argv[2], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		new_transmit = BT_MESH_TRANSMIT(count, interval);

		err = bt_mesh_cfg_cli_net_transmit_set(bt_mesh_shell_target_ctx.net_idx,
						   bt_mesh_shell_target_ctx.dst, new_transmit,
						   &transmit);
	}

	if (err) {
		printf("Unable to send network transmit Get/Set (err %d)\n", err);
		return ;
	}

	printf("Transmit 0x%02x (count %u interval %ums)\n", transmit,
		    BT_MESH_TRANSMIT_COUNT(transmit), BT_MESH_TRANSMIT_INT(transmit));

}

static void cmd_relay(int argc, char *argv[])
{
	uint8_t relay, transmit;
	int err = 0;

	if (argc < 2) {
		err = bt_mesh_cfg_cli_relay_get(bt_mesh_shell_target_ctx.net_idx,
					    bt_mesh_shell_target_ctx.dst, &relay, &transmit);
	} else {
		uint8_t count, interval, new_transmit;
		uint8_t val = wm_atoi(argv[1], &err);

		if (val) {
			if (argc > 2) {
				count = wm_atoi(argv[2], &err);
			} else {
				count = 2U;
			}

			if (argc > 3) {
				interval = wm_atoi(argv[3], &err);
			} else {
				interval = 20U;
			}

			new_transmit = BT_MESH_TRANSMIT(count, interval);
		} else {
			new_transmit = 0U;
		}

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_relay_set(bt_mesh_shell_target_ctx.net_idx,
					    bt_mesh_shell_target_ctx.dst, val, new_transmit, &relay,
					    &transmit);
	}

	if (err) {
		printf("Unable to send Relay Get/Set (err %d)\n", err);
		return ;
	}

	printf("Relay is 0x%02x, Transmit 0x%02x (count %u interval %ums)\n", relay,
		    transmit, BT_MESH_TRANSMIT_COUNT(transmit), BT_MESH_TRANSMIT_INT(transmit));

}

static void cmd_net_key_add(int argc, char *argv[])
{
	bool has_key_val = (argc > 2);
	uint8_t key_val[16];
	uint16_t key_net_idx;
	uint8_t status;
	int err = 0;

	key_net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[2], strlen(argv[2]), key_val, sizeof(key_val));
		(void)memset(key_val + len, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, bt_mesh_shell_default_key, sizeof(key_val));
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_subnet *subnet;

		subnet = bt_mesh_cdb_subnet_get(key_net_idx);
		if (subnet) {
			if (has_key_val) {
				printf("Subnet 0x%03x already has a value\n", key_net_idx);
				return ;
			}

			if (bt_mesh_cdb_subnet_key_export(subnet, 0, key_val)) {
				printf("Unable to export subnet key from cdb 0x%03x\n",
					    key_net_idx);
				return ;
			}
		} else {
			subnet = bt_mesh_cdb_subnet_alloc(key_net_idx);
			if (!subnet) {
				printf("No space for subnet in cdb\n");
				return ;
			}

			if (bt_mesh_cdb_subnet_key_import(subnet, 0, key_val)) {
				printf("Unable to import subnet key into cdb 0x%03x\n",
					    key_net_idx);
				return ;
			}
			bt_mesh_cdb_subnet_store(subnet);
		}
	}

	err = bt_mesh_cfg_cli_net_key_add(bt_mesh_shell_target_ctx.net_idx,
				      bt_mesh_shell_target_ctx.dst, key_net_idx, key_val, &status);
	if (err) {
		printf("Unable to send NetKey Add (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("NetKeyAdd failed with status 0x%02x\n", status);
	} else {
		printf("NetKey added with NetKey Index 0x%03x\n", key_net_idx);
	}

}

static void cmd_net_key_update(int argc, char *argv[])
{
	bool has_key_val = (argc > 2);
	uint8_t key_val[16];
	uint16_t key_net_idx;
	uint8_t status;
	int err = 0;

	key_net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[2], strlen(argv[2]), key_val, sizeof(key_val));
		(void)memset(key_val + len, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, bt_mesh_shell_default_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_cli_net_key_update(bt_mesh_shell_target_ctx.net_idx,
					 bt_mesh_shell_target_ctx.dst, key_net_idx, key_val,
					 &status);
	if (err) {
		printf("Unable to send NetKey Update (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("NetKeyUpdate failed with status 0x%02x\n", status);
	} else {
		printf("NetKey updated with NetKey Index 0x%03x\n", key_net_idx);
	}

}

static void cmd_net_key_get(int argc, char *argv[])
{
	uint16_t keys[16];
	size_t cnt;
	int err, i;

	cnt = ARRAY_SIZE(keys);

	err = bt_mesh_cfg_cli_net_key_get(bt_mesh_shell_target_ctx.net_idx,
				      bt_mesh_shell_target_ctx.dst, keys, &cnt);
	if (err) {
		printf("Unable to send NetKeyGet (err %d)\n", err);
		return ;
	}

	printf("NetKeys known by 0x%04x:", bt_mesh_shell_target_ctx.dst);
	for (i = 0; i < cnt; i++) {
		printf("\t0x%03x", keys[i]);
	}
    printf("\n");
}

static void cmd_net_key_del(int argc, char *argv[])
{
	uint16_t key_net_idx;
	uint8_t status;
	int err = 0;

	key_net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_cfg_cli_net_key_del(bt_mesh_shell_target_ctx.net_idx,
				      bt_mesh_shell_target_ctx.dst, key_net_idx, &status);
	if (err) {
		printf("Unable to send NetKeyDel (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("NetKeyDel failed with status 0x%02x\n", status);
	} else {
		printf("NetKey 0x%03x deleted\n", key_net_idx);
	}

}

static void cmd_app_key_add(int argc, char *argv[])
{
	uint8_t key_val[16];
	uint16_t key_net_idx, key_app_idx;
	bool has_key_val = (argc > 3);
	uint8_t status;
	int err = 0;

	key_net_idx = wm_atoi(argv[1], &err);
	key_app_idx = wm_atoi(argv[2], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[3], strlen(argv[3]), key_val, sizeof(key_val));
		(void)memset(key_val + len, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, bt_mesh_shell_default_key, sizeof(key_val));
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		struct bt_mesh_cdb_app_key *app_key;

		app_key = bt_mesh_cdb_app_key_get(key_app_idx);
		if (app_key) {
			if (has_key_val) {
				printf("App key 0x%03x already has a value\n", key_app_idx);
				return ;
			}

			if (bt_mesh_cdb_app_key_export(app_key, 0, key_val)) {
				printf("Unable to export app key 0x%03x from cdb\n",
					    key_app_idx);
				return ;
			}
		} else {
			app_key = bt_mesh_cdb_app_key_alloc(key_net_idx, key_app_idx);
			if (!app_key) {
				printf("No space for app key in cdb\n");
				return ;
			}

			if (bt_mesh_cdb_app_key_import(app_key, 0, key_val)) {
				printf("Unable to import app key 0x%03x into cdb\n",
					    key_app_idx);
				return ;
			}
			bt_mesh_cdb_app_key_store(app_key);
		}
	}

	err = bt_mesh_cfg_cli_app_key_add(bt_mesh_shell_target_ctx.net_idx,
				      bt_mesh_shell_target_ctx.dst, key_net_idx, key_app_idx,
				      key_val, &status);
	if (err) {
		printf("Unable to send App Key Add (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("AppKeyAdd failed with status 0x%02x\n", status);
	} else {
		printf("AppKey added, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n", key_net_idx,
			    key_app_idx);
	}

}

static void cmd_app_key_upd(int argc, char *argv[])
{
	uint8_t key_val[16];
	uint16_t key_net_idx, key_app_idx;
	bool has_key_val = (argc > 3);
	uint8_t status;
	int err = 0;

	key_net_idx = wm_atoi(argv[1], &err);
	key_app_idx = wm_atoi(argv[2], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (has_key_val) {
		size_t len;

		len = hex2bin(argv[3], strlen(argv[3]), key_val, sizeof(key_val));
		(void)memset(key_val + len, 0, sizeof(key_val) - len);
	} else {
		memcpy(key_val, bt_mesh_shell_default_key, sizeof(key_val));
	}

	err = bt_mesh_cfg_cli_app_key_update(bt_mesh_shell_target_ctx.net_idx,
					 bt_mesh_shell_target_ctx.dst, key_net_idx, key_app_idx,
					 key_val, &status);
	if (err) {
		printf("Unable to send App Key Update (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("AppKey update failed with status 0x%02x", status);
	} else {
		printf("AppKey updated, NetKeyIndex 0x%04x AppKeyIndex 0x%04x",
			    key_net_idx, key_app_idx);
	}

}

static void cmd_app_key_get(int argc, char *argv[])
{
	uint16_t net_idx;
	uint16_t keys[16];
	size_t cnt;
	uint8_t status;
	int err = 0;
	int i;

	cnt = ARRAY_SIZE(keys);
	net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_cfg_cli_app_key_get(bt_mesh_shell_target_ctx.net_idx,
				      bt_mesh_shell_target_ctx.dst, net_idx, &status, keys, &cnt);
	if (err) {
		printf("Unable to send AppKeyGet (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("AppKeyGet failed with status 0x%02x\n", status);
		return ;
	}

	printf("AppKeys for NetKey 0x%03x known by 0x%04x:", net_idx,
		    bt_mesh_shell_target_ctx.dst);
	for (i = 0; i < cnt; i++) {
		printf("\t0x%03x", keys[i]);
	}
    printf("\n");
}

static void cmd_node_id(int argc, char *argv[])
{
	uint16_t net_idx;
	uint8_t status, identify;
	int err = 0;

	net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc <= 2) {
		err = bt_mesh_cfg_cli_node_identity_get(bt_mesh_shell_target_ctx.net_idx,
						    bt_mesh_shell_target_ctx.dst, net_idx, &status,
						    &identify);
		if (err) {
			printf("Unable to send Node Identify Get (err %d)\n", err);
			return ;
		}
	} else {
		uint8_t new_identify = wm_atoi(argv[2], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_node_identity_set(bt_mesh_shell_target_ctx.net_idx,
						    bt_mesh_shell_target_ctx.dst, net_idx,
						    new_identify, &status, &identify);
		if (err) {
			printf("Unable to send Node Identify Set (err %d)\n", err);
			return ;
		}
	}

	if (status) {
		printf("Node Identify Get/Set failed with status 0x%02x\n", status);
	} else {
		printf("Node Identify Get/Set successful with identify 0x%02x\n", identify);
	}

}

static void cmd_app_key_del(int argc, char *argv[])
{
	uint16_t key_net_idx, key_app_idx;
	uint8_t status;
	int err = 0;

	key_net_idx = wm_atoi(argv[1], &err);
	key_app_idx = wm_atoi(argv[2], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_cfg_cli_app_key_del(bt_mesh_shell_target_ctx.net_idx,
				      bt_mesh_shell_target_ctx.dst, key_net_idx, key_app_idx,
				      &status);
	if (err) {
		printf("Unable to send App Key del(err %d)\n", err);
		return ;
	}

	if (status) {
		printf("AppKeyDel failed with status 0x%02x\n", status);
	} else {
		printf("AppKey deleted, NetKeyIndex 0x%04x AppKeyIndex 0x%04x\n",
			    key_net_idx, key_app_idx);
	}

}

static void cmd_mod_app_bind(int argc, char *argv[])
{
	uint16_t elem_addr, mod_app_idx, mod_id, cid;
	uint8_t status;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);
	mod_app_idx = wm_atoi(argv[2], &err);
	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_app_bind_vnd(bt_mesh_shell_target_ctx.net_idx,
						   bt_mesh_shell_target_ctx.dst, elem_addr,
						   mod_app_idx, mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_app_bind(bt_mesh_shell_target_ctx.net_idx,
					       bt_mesh_shell_target_ctx.dst, elem_addr, mod_app_idx,
					       mod_id, &status);
	}

	if (err) {
		printf("Unable to send Model App Bind (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model App Bind failed with status 0x%02x\n", status);
	} else {
		printf("AppKey successfully bound\n");
	}

}

static void cmd_mod_app_unbind(int argc, char *argv[])
{
	uint16_t elem_addr, mod_app_idx, mod_id, cid;
	uint8_t status;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);
	mod_app_idx = wm_atoi(argv[2], &err);
	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_app_unbind_vnd(bt_mesh_shell_target_ctx.net_idx,
						     bt_mesh_shell_target_ctx.dst, elem_addr,
						     mod_app_idx, mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_app_unbind(bt_mesh_shell_target_ctx.net_idx,
						 bt_mesh_shell_target_ctx.dst, elem_addr,
						 mod_app_idx, mod_id, &status);
	}

	if (err) {
		printf("Unable to send Model App Unbind (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model App Unbind failed with status 0x%02x\n", status);
	} else {
		printf("AppKey successfully unbound\n");
	}

}

static void cmd_mod_app_get(int argc, char *argv[])
{
	uint16_t elem_addr, mod_id, cid;
	uint16_t apps[16];
	uint8_t status;
	size_t cnt;
	int err = 0;
	int i;

	cnt = ARRAY_SIZE(apps);
	elem_addr = wm_atoi(argv[1], &err);
	mod_id = wm_atoi(argv[2], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 3) {
		cid = wm_atoi(argv[3], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_app_get_vnd(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, elem_addr, mod_id,
						  cid, &status, apps, &cnt);
	} else {
		err = bt_mesh_cfg_cli_mod_app_get(bt_mesh_shell_target_ctx.net_idx,
					      bt_mesh_shell_target_ctx.dst, elem_addr, mod_id,
					      &status, apps, &cnt);
	}

	if (err) {
		printf("Unable to send Model App Get (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model App Get failed with status 0x%02x\n", status);
	} else {
		printf("Apps bound to Element 0x%04x, Model 0x%04x %s:", elem_addr, mod_id,
			    argc > 3 ? argv[3] : "(SIG)");

		if (!cnt) {
			printf("\tNone.");
		}

		for (i = 0; i < cnt; i++) {
			printf("\t0x%04x", apps[i]);
		}
        printf("\n");
	}

}

static void cmd_mod_sub_add(int argc, char *argv[])
{
	uint16_t elem_addr, sub_addr, mod_id, cid;
	uint8_t status;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);
	sub_addr = wm_atoi(argv[2], &err);
	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_add_vnd(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, elem_addr, sub_addr,
						  mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_add(bt_mesh_shell_target_ctx.net_idx,
					      bt_mesh_shell_target_ctx.dst, elem_addr, sub_addr,
					      mod_id, &status);
	}

	if (err) {
		printf("Unable to send Model Subscription Add (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model Subscription Add failed with status 0x%02x\n", status);
	} else {
		printf("Model subscription was successful\n");
	}

}

static void cmd_mod_sub_del(int argc, char *argv[])
{
	uint16_t elem_addr, sub_addr, mod_id, cid;
	uint8_t status;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);
	sub_addr = wm_atoi(argv[2], &err);
	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_del_vnd(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, elem_addr, sub_addr,
						  mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_del(bt_mesh_shell_target_ctx.net_idx,
					      bt_mesh_shell_target_ctx.dst, elem_addr, sub_addr,
					      mod_id, &status);
	}

	if (err) {
		printf("Unable to send Model Subscription Delete (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model Subscription Delete failed with status 0x%02x\n", status);
	} else {
		printf("Model subscription deltion was successful\n");
	}

}

static void cmd_mod_sub_add_va(int argc, char *argv[])
{
	uint16_t elem_addr, sub_addr, mod_id, cid;
	uint8_t label[16];
	uint8_t status;
	size_t len;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);

	len = hex2bin(argv[2], strlen(argv[2]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);

	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_va_add_vnd(bt_mesh_shell_target_ctx.net_idx,
						     bt_mesh_shell_target_ctx.dst, elem_addr, label,
						     mod_id, cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_va_add(bt_mesh_shell_target_ctx.net_idx,
						 bt_mesh_shell_target_ctx.dst, elem_addr, label,
						 mod_id, &sub_addr, &status);
	}

	if (err) {
		printf("Unable to send Mod Sub VA Add (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Mod Sub VA Add failed with status 0x%02x\n", status);
	} else {
		printf("0x%04x subscribed to Label UUID %s (va 0x%04x)\n", elem_addr,
			    argv[2], sub_addr);
	}

}

static void cmd_mod_sub_del_va(int argc, char *argv[])
{
	uint16_t elem_addr, sub_addr, mod_id, cid;
	uint8_t label[16];
	uint8_t status;
	size_t len;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);

	len = hex2bin(argv[2], strlen(argv[2]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);

	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_va_del_vnd(bt_mesh_shell_target_ctx.net_idx,
						     bt_mesh_shell_target_ctx.dst, elem_addr, label,
						     mod_id, cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_va_del(bt_mesh_shell_target_ctx.net_idx,
						 bt_mesh_shell_target_ctx.dst, elem_addr, label,
						 mod_id, &sub_addr, &status);
	}

	if (err) {
		printf("Unable to send Model Subscription Delete (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model Subscription Delete failed with status 0x%02x\n", status);
	} else {
		printf("0x%04x unsubscribed from Label UUID %s (va 0x%04x)\n", elem_addr,
			    argv[2], sub_addr);
	}

}

static void cmd_mod_sub_ow(int argc, char *argv[])
{
	uint16_t elem_addr, sub_addr, mod_id, cid;
	uint8_t status;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);
	sub_addr = wm_atoi(argv[2], &err);
	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_overwrite_vnd(bt_mesh_shell_target_ctx.net_idx,
							bt_mesh_shell_target_ctx.dst, elem_addr,
							sub_addr, mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_overwrite(bt_mesh_shell_target_ctx.net_idx,
						    bt_mesh_shell_target_ctx.dst, elem_addr,
						    sub_addr, mod_id, &status);
	}

	if (err) {
		printf("Unable to send Model Subscription Overwrite (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model Subscription Overwrite failed with status 0x%02x\n", status);
	} else {
		printf("Model subscription overwrite was successful\n");
	}

}

static void cmd_mod_sub_ow_va(int argc, char *argv[])
{
	uint16_t elem_addr, sub_addr, mod_id, cid;
	uint8_t label[16];
	uint8_t status;
	size_t len;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);

	len = hex2bin(argv[2], strlen(argv[2]), label, sizeof(label));
	(void)memset(label + len, 0, sizeof(label) - len);

	mod_id = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 4) {
		cid = wm_atoi(argv[4], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_va_overwrite_vnd(bt_mesh_shell_target_ctx.net_idx,
							   bt_mesh_shell_target_ctx.dst, elem_addr,
							   label, mod_id, cid, &sub_addr, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_va_overwrite(bt_mesh_shell_target_ctx.net_idx,
						       bt_mesh_shell_target_ctx.dst, elem_addr,
						       label, mod_id, &sub_addr, &status);
	}

	if (err) {
		printf("Unable to send Mod Sub VA Overwrite (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Mod Sub VA Overwrite failed with status 0x%02x\n", status);
	} else {
		printf("0x%04x overwrite to Label UUID %s (va 0x%04x)\n", elem_addr, argv[2],
			    sub_addr);
	}

}

static void cmd_mod_sub_del_all(int argc, char *argv[])
{
	uint16_t elem_addr, mod_id, cid;
	uint8_t status;
	int err = 0;

	elem_addr = wm_atoi(argv[1], &err);
	mod_id = wm_atoi(argv[2], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 3) {
		cid = wm_atoi(argv[3], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_del_all_vnd(bt_mesh_shell_target_ctx.net_idx,
						      bt_mesh_shell_target_ctx.dst, elem_addr,
						      mod_id, cid, &status);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_del_all(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, elem_addr, mod_id,
						  &status);
	}

	if (err) {
		printf("Unable to send Model Subscription Delete All (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model Subscription Delete All failed with status 0x%02x\n", status);
	} else {
		printf("Model subscription deltion all was successful\n");
	}

}

static void cmd_mod_sub_get(int argc, char *argv[])
{
	uint16_t elem_addr, mod_id, cid;
	uint16_t subs[16];
	uint8_t status;
	size_t cnt;
	int err = 0;
	int i;

	cnt = ARRAY_SIZE(subs);
	elem_addr = wm_atoi(argv[1], &err);
	mod_id = wm_atoi(argv[2], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 3) {
		cid = wm_atoi(argv[3], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_mod_sub_get_vnd(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, elem_addr, mod_id,
						  cid, &status, subs, &cnt);
	} else {
		err = bt_mesh_cfg_cli_mod_sub_get(bt_mesh_shell_target_ctx.net_idx,
					      bt_mesh_shell_target_ctx.dst, elem_addr, mod_id,
					      &status, subs, &cnt);
	}

	if (err) {
		printf("Unable to send Model Subscription Get (err %d)\n", err);
		return ;
	}

	if (status) {
		printf("Model Subscription Get failed with status 0x%02x\n", status);
	} else {
		printf(
			    "Model Subscriptions for Element 0x%04x, Model 0x%04x %s:", elem_addr,
			    mod_id, argc > 3 ? argv[3] : "(SIG)");

		if (!cnt) {
			printf("\tNone.");
		}

		for (i = 0; i < cnt; i++) {
			printf("\t0x%04x", subs[i]);
		}
        printf("\n");
	}

}

static void cmd_krp(int argc, char *argv[])
{
	uint8_t status, phase;
	uint16_t key_net_idx;
	int err = 0;

	key_net_idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc < 3) {
		err = bt_mesh_cfg_cli_krp_get(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst, key_net_idx, &status,
					  &phase);
	} else {
		uint16_t trans = wm_atoi(argv[2], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_cfg_cli_krp_set(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst, key_net_idx, trans, &status,
					  &phase);
	}

	if (err) {
		printf("Unable to send key refresh phase Get/Set (err %d)\n", err);
		return ;
	}

	printf("Key refresh phase Get/Set with status 0x%02x and phase 0x%02x\n", status,
		    phase);

}

static int mod_pub_get( uint16_t addr, uint16_t mod_id, uint16_t cid)
{
	struct bt_mesh_cfg_cli_mod_pub pub;
	uint8_t status;
	int err;

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_cli_mod_pub_get(bt_mesh_shell_target_ctx.net_idx,
					      bt_mesh_shell_target_ctx.dst, addr, mod_id, &pub,
					      &status);
	} else {
		err = bt_mesh_cfg_cli_mod_pub_get_vnd(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, addr, mod_id, cid,
						  &pub, &status);
	}

	if (err) {
		printf("Model Publication Get failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printf("Model Publication Get failed (status 0x%02x)\n", status);
		return 0;
	}

	printf(
		    "Model Publication for Element 0x%04x, Model 0x%04x:\n"
		    "\tPublish Address:                0x%04x\n"
		    "\tAppKeyIndex:                    0x%04x\n"
		    "\tCredential Flag:                %u\n"
		    "\tPublishTTL:                     %u\n"
		    "\tPublishPeriod:                  0x%02x\n"
		    "\tPublishRetransmitCount:         %u\n"
		    "\tPublishRetransmitInterval:      %ums\n",
		    addr, mod_id, pub.addr, pub.app_idx, pub.cred_flag, pub.ttl, pub.period,
		    BT_MESH_PUB_TRANSMIT_COUNT(pub.transmit),
		    BT_MESH_PUB_TRANSMIT_INT(pub.transmit));

	return 0;
}

static int mod_pub_set( uint16_t addr, bool is_va, uint16_t mod_id,
		       uint16_t cid, char *argv[])
{
	struct bt_mesh_cfg_cli_mod_pub pub;
	uint8_t status, count, res_step, steps;
	uint16_t interval;
	uint8_t uuid[16];
	uint8_t len;
	int err = 0;

	if (!is_va) {
		pub.addr = wm_atoi(argv[0], &err);
		pub.uuid = NULL;
	} else {
		len = hex2bin(argv[0], strlen(argv[0]), uuid, sizeof(uuid));
		memset(uuid + len, 0, sizeof(uuid) - len);
		pub.uuid = (const uint8_t *)&uuid;
	}

	pub.app_idx = wm_atoi(argv[1], &err);
	pub.cred_flag = wm_atoi(argv[2], &err);
	pub.ttl = wm_atoi(argv[3], &err);
	res_step = wm_atoi(argv[4], &err);
	steps = wm_atoi(argv[5], &err);
	if ((res_step > 3) || (steps > 0x3F)) {
		printf("Invalid period");
		return -EINVAL;
	}

	pub.period = (steps << 2) + res_step;
	count = wm_atoi(argv[6], &err);
	if (count > 7) {
		printf("Invalid retransmit count\n");
		return -EINVAL;
	}

	interval = wm_atoi(argv[7], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return err;
	}

	if (interval > (31 * 50) || (interval % 50)) {
		printf("Invalid retransmit interval %u\n", interval);
		return -EINVAL;
	}

	pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

	if (cid == CID_NVAL) {
		err = bt_mesh_cfg_cli_mod_pub_set(bt_mesh_shell_target_ctx.net_idx,
					      bt_mesh_shell_target_ctx.dst, addr, mod_id, &pub,
					      &status);
	} else {
		err = bt_mesh_cfg_cli_mod_pub_set_vnd(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, addr, mod_id, cid,
						  &pub, &status);
	}

	if (err) {
		printf("Model Publication Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printf("Model Publication Set failed (status 0x%02x)\n", status);
	} else {
		printf("Model Publication successfully set\n");
	}

	return 0;
}

static void cmd_mod_pub(int argc, char *argv[])
{
	int err = 0;
	uint16_t addr, mod_id, cid;

	addr = wm_atoi(argv[1], &err);
	mod_id = wm_atoi(argv[2], &err);

	argc -= 3;
	argv += 3;

	if (argc == 1 || argc == 9) {
		cid = wm_atoi(argv[0], &err);
		argc--;
		argv++;
	} else {
		cid = CID_NVAL;
	}

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 0) {
		if (argc < 7) {
			printf("Invalid number of argument\n");
			return ;
		}

		mod_pub_set(addr, false, mod_id, cid, argv);
	} else {
		mod_pub_get(addr, mod_id, cid);
	}
}

static void cmd_mod_pub_va(int argc, char *argv[])
{
	int err = 0;
	uint16_t addr, mod_id, cid = CID_NVAL;

	addr = wm_atoi(argv[1], &err);
	mod_id = wm_atoi(argv[9], &err);

	if (argc > 10) {
		cid = wm_atoi(argv[10], &err);
	}

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	argv += 2;

	mod_pub_set(addr, true, mod_id, cid, argv);
}

static void hb_sub_print( struct bt_mesh_cfg_cli_hb_sub *sub)
{
	printf(
		    "Heartbeat Subscription:\n"
		    "\tSource:      0x%04x\n"
		    "\tDestination: 0x%04x\n"
		    "\tPeriodLog:   0x%02x\n"
		    "\tCountLog:    0x%02x\n"
		    "\tMinHops:     %u\n"
		    "\tMaxHops:     %u\n",
		    sub->src, sub->dst, sub->period, sub->count, sub->min, sub->max);
}

static int hb_sub_get(int argc, char *argv[])
{
	struct bt_mesh_cfg_cli_hb_sub sub;
	uint8_t status;
	int err;

	err = bt_mesh_cfg_cli_hb_sub_get(bt_mesh_shell_target_ctx.net_idx,
					 bt_mesh_shell_target_ctx.dst, &sub, &status);
	if (err) {
		printf("Heartbeat Subscription Get failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printf("Heartbeat Subscription Get failed (status 0x%02x)\n", status);
	} else {
		hb_sub_print(&sub);
	}

	return 0;
}

static int hb_sub_set(int argc, char *argv[])
{
	struct bt_mesh_cfg_cli_hb_sub sub;
	uint8_t status;
	int err = 0;

	sub.src = wm_atoi(argv[1], &err);
	sub.dst = wm_atoi(argv[2], &err);
	sub.period = wm_atoi(argv[3], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return err;
	}

	err = bt_mesh_cfg_cli_hb_sub_set(bt_mesh_shell_target_ctx.net_idx,
					 bt_mesh_shell_target_ctx.dst, &sub, &status);
	if (err) {
		printf("Heartbeat Subscription Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printf("Heartbeat Subscription Set failed (status 0x%02x)\n", status);
	} else {
		hb_sub_print(&sub);
	}

	return 0;
}

static void cmd_hb_sub(int argc, char *argv[])
{
	if (argc > 1) {
		if (argc < 4) {
			printf("Invalid number of argument\n");
			return ;
		}

		hb_sub_set(argc, argv);
	} else {
		hb_sub_get(argc, argv);
	}
}

static int hb_pub_get(int argc, char *argv[])
{
	struct bt_mesh_cfg_cli_hb_pub pub;
	uint8_t status;
	int err;

	err = bt_mesh_cfg_cli_hb_pub_get(bt_mesh_shell_target_ctx.net_idx,
					 bt_mesh_shell_target_ctx.dst, &pub, &status);
	if (err) {
		printf("Heartbeat Publication Get failed (err %d)", err);
		return 0;
	}

	if (status) {
		printf("Heartbeat Publication Get failed (status 0x%02x)", status);
		return 0;
	}

	printf("Heartbeat publication:\n");
	printf("\tdst 0x%04x count 0x%02x period 0x%02x\n", pub.dst, pub.count, pub.period);
	printf("\tttl 0x%02x feat 0x%04x net_idx 0x%04x\n", pub.ttl, pub.feat, pub.net_idx);

	return 0;
}

static int hb_pub_set(int argc, char *argv[])
{
	struct bt_mesh_cfg_cli_hb_pub pub;
	uint8_t status;
	int err = 0;

	pub.dst = wm_atoi(argv[1], &err);
	pub.count = wm_atoi(argv[2], &err);
	pub.period = wm_atoi(argv[3], &err);
	pub.ttl = wm_atoi(argv[4], &err);
	pub.feat = wm_atoi(argv[5], &err);
	pub.net_idx = wm_atoi(argv[6], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return err;
	}

	err = bt_mesh_cfg_cli_hb_pub_set(bt_mesh_shell_target_ctx.net_idx,
					 bt_mesh_shell_target_ctx.dst, &pub, &status);
	if (err) {
		printf("Heartbeat Publication Set failed (err %d)\n", err);
		return 0;
	}

	if (status) {
		printf("Heartbeat Publication Set failed (status 0x%02x)\n", status);
	} else {
		printf("Heartbeat publication successfully set\n");
	}

	return 0;
}

static void cmd_hb_pub(int argc, char *argv[])
{
	if (argc > 1) {
		if (argc < 7) {
			printf("Invalid number of argument\n");
			return ;
		}

		hb_pub_set(argc, argv);
	} else {
		hb_pub_get(argc, argv);
	}
}

WM_CLI_CMD_DEFINE(mesh_app_bind, cmd_mod_app_bind, send  app bind, mesh_app_bind <Addr> <AppKeyIdx> <MID> [CID] -- mesh_app_bind cmd);
WM_CLI_CMD_DEFINE(mesh_app_get, cmd_mod_app_get, send app get, mesh_app_get <ElemAddr> <MID> [CID] -- mesh_app_get cmd);
WM_CLI_CMD_DEFINE(mesh_app_unbind, cmd_mod_app_unbind,        remove app bind, mesh_app_unbind <Addr> <AppKeyIdx> <MID> [CID] -- mesh_app_unbind cmd);
WM_CLI_CMD_DEFINE(mesh_pub, cmd_mod_pub,        send publish , mesh_pub <Addr> <MID> [CID] [<PubAddr> <AppKeyIdx> <Cred(off, on)> <TTL> <PerRes> <PerSteps> <Count> <Int(ms)>] -- mesh_pub cmd);
WM_CLI_CMD_DEFINE(mesh_pub_va, cmd_mod_pub_va,        send publish va , mesh_pub_va <Addr> <UUID(1-16 hex)> <AppKeyIdx> <Cred(off, on)> <TTL> <PerRes> <PerSteps> <Count> <Int(ms)> <MID> [CID] -- mesh_pub_va cmd);
WM_CLI_CMD_DEFINE(mesh_sub_add, cmd_mod_sub_add, send appending subscribe attribute , cmd_mod_sub_add <ElemAddr> <SubAddr> <MID> [CID] -- cmd_mod_sub_add cmd);
WM_CLI_CMD_DEFINE(mesh_sub_del, cmd_mod_sub_del, send removing subscribe attribute , cmd_mod_sub_del <ElemAddr> <SubAddr> <MID> [CID] -- cmd_mod_sub_del cmd);
WM_CLI_CMD_DEFINE(mesh_sub_add_va, cmd_mod_sub_add_va, send appending subscribe va attribute , mesh_sub_add_va <ElemAddr> <LabelUUID(1-16 hex)> <MID> [CID] -- mesh_sub_add_va cmd);
WM_CLI_CMD_DEFINE(mesh_sub_del_va, cmd_mod_sub_del_va, send removing subscribe va attribute , mesh_sub_del_va <ElemAddr> <LabelUUID(1-16 hex)> <MID> [CID] -- mesh_sub_del_va cmd);
WM_CLI_CMD_DEFINE(mesh_sub_ow, cmd_mod_sub_ow, append send subscribe overwrite attribute , mesh_sub_ow <ElemAddr> <SubAddr> <MID> [CID] -- mesh_sub_ow cmd);
WM_CLI_CMD_DEFINE(mesh_sub_ow_va, cmd_mod_sub_ow_va, send subscribte va overwrite attribute , mesh_sub_ow_va <ElemAddr> <LabelUUID(1-16 hex)> <MID> [CID] -- mesh_sub_ow_va cmd);
WM_CLI_CMD_DEFINE(mesh_sub_del_all, cmd_mod_sub_del_all, send delte all subscribe attribute , mesh_sub_del_all <ElemAddr> <MID> [CID] -- mesh_sub_del_all cmd);
WM_CLI_CMD_DEFINE(mesh_sub_get, cmd_mod_sub_get, send get all subscribe attribute , mesh_sub_get <ElemAddr> <MID> [CID] -- mesh_sub_get cmd);

WM_CLI_CMD_DEFINE(mesh_models_cfg_netkey_add, cmd_net_key_add, send add network key , mesh_models_cfg_netkey_add <NetKeyIdx> [Key(1-16 hex)] -- mesh_models_cfg_netkey_add cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_netkey_upd, cmd_net_key_update, send update network key, mesh_models_cfg_netkey_upd <NetKeyIdx> [Key(1-16 hex)] -- mesh_models_cfg_netkey_upd cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_netkey_get, cmd_net_key_get, send get network key , mesh_models_cfg_netkey_get  -- mesh_netkey_get cmd);
WM_CLI_CMD_DEFINE(mesh_netkey_del, cmd_net_key_del, send delete network key , mesh_models_cfg_netkey_del <NetKeyIdx> -- mesh_netkey_del cmd);

WM_CLI_CMD_DEFINE(mesh_models_cfg_appkey_add, cmd_app_key_add, send add app key , mesh_models_cfg_appkey_add <NetKeyIdx> <AppKeyIdx> [Key(1-16 hex)] -- mesh_models_cfg_netkey_add cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_appkey_upd, cmd_app_key_upd, send update app key, mesh_models_cfg_appkey_upd <NetKeyIdx> <AppKeyIdx> [Key(1-16 hex)] -- mesh_models_cfg_netkey_upd cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_appkey_get, cmd_app_key_get, send get app key , mesh_models_cfg_appkey_get <NetKeyIdx> -- mesh_models_cfg_netkey_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_appkey_del, cmd_app_key_del, send delete app key , mesh_models_cfg_appkey_del <NetKeyIdx> <AppKeyIdx> -- mesh_models_cfg_netkey_del cmd);

WM_CLI_CMD_DEFINE(mesh_models_cfg_reset, cmd_reset, send reset cmd , mesh_models_cfg_reset -- mesh_models_cfg_reset cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_timeout, cmd_timeout, send timeout , mesh_models_cfg_timeout [Timeout(s)] -- mesh_models_cfg_timeout cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_get_comp, cmd_get_comp, send get composition data , mesh_models_cfg_get_comp [page] -- mesh_models_cfg_get_comp cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_beacon, cmd_beacon, send turn on or off beacon , mesh_models_cfg_beacon [Val(0, 1)] -- mesh_models_cfg_beacon cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_ttl, cmd_ttl, send ttl , mesh_cli_ttl [TTL] -- mesh_models_cfg_ttl cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_friend, cmd_friend, send turn on or off friend feature , mesh_models_cfg_friend [Val(0, 1)] -- mesh_models_cfg_friend cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_gatt_proxy, cmd_gatt_proxy, send turn on or off gatt_proxy feature , mesh_models_cfg_gatt_proxy [Val(0, 1)] -- mesh_models_cfg_gatt_proxy cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_relay, cmd_relay, send turn on or off relay feature , mesh_models_cfg_relay [Val(0, 1)] -- mesh_models_cfg_relay cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_node_id, cmd_node_id, send get or set node identify , mesh_models_cfg_node_id <NetKeyIdx> [Identify] -- mesh_models_cfg_node_id cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_polltimeout_get, cmd_polltimeout_get, send getting poll timeout , mesh_models_cfg_polltimeout_get <LPNAddr> -- mesh_models_cfg_polltimeout_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_net_transmit_param, cmd_net_transmit, send get or set net transmit parameter , mesh_models_cfg_net_transmit_param [<Count> <Int(ms)>] -- mesh_models_cfg_net_transmit_param cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_krp, cmd_krp, send get or set key refresh phase , mesh_models_cfg_krp <NetKeyIdx> [Phase] -- mesh_models_cfg_krp cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_hb_sub, cmd_hb_sub, send get or set heartbeat subscribe , mesh_models_cfg_hb_sub [<Src> <Dst> <Per>] -- mesh_models_cfg_hb_sub cmd);
WM_CLI_CMD_DEFINE(mesh_models_cfg_hb_pub, cmd_hb_pub, send get or set heartbeat publish , mesh_models_cfg_hb_pub [<Dst> <Count> <Per> <TTL> <Features> <NetKeyIdx>] -- mesh_models_cfg_hb_pub cmd);


