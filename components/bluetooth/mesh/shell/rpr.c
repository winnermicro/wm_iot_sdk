/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"

static struct bt_mesh_model *mod;

/***************************************************************************************************
 * Implementation of the model's instance
 **************************************************************************************************/

extern const struct shell *bt_mesh_shell_ctx_shell;

static void rpr_scan_report(struct bt_mesh_rpr_cli *cli,
			    const struct bt_mesh_rpr_node *srv,
			    struct bt_mesh_rpr_unprov *unprov,
			    struct net_buf_simple *adv_data)
{
	char uuid_hex_str[32 + 1];

	bin2hex(unprov->uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));

	printf(
		    "Server 0x%04x:\n"
		    "\tuuid:   %s\n"
		    "\tOOB:    0x%04x",
		    srv->addr, uuid_hex_str, unprov->oob);

	while (adv_data && adv_data->len > 2) {
		uint8_t len, type;
		uint8_t data[31];

		len = net_buf_simple_pull_u8(adv_data);
		if (len == 0) {
			/* No data in this AD Structure. */
			continue;
		}

		if (len > adv_data->len) {
			/* Malformed AD Structure. */
			break;
		}

		type = net_buf_simple_pull_u8(adv_data);
		if ((--len) > 0) {
			uint8_t dlen;

			/* Pull all length, but print only what fits into `data` array. */
			dlen = MIN(len, sizeof(data) - 1);
			memcpy(data, net_buf_simple_pull_mem(adv_data, len), dlen);
			len = dlen;
		}
		data[len] = '\0';

		if (type == BT_DATA_URI) {
			printf( "\tURI:    \"\\x%02x%s\"",
				    data[0], &data[1]);
		} else if (type == BT_DATA_NAME_COMPLETE) {
			printf( "\tName:   \"%s\"", data);
		} else {
			char string[64 + 1];

			bin2hex(data, len, string, sizeof(string));
			printf( "\t0x%02x:  %s", type, string);
		}
	}
}

struct bt_mesh_rpr_cli bt_mesh_shell_rpr_cli = {
	.scan_report = rpr_scan_report,
};

/***************************************************************************************************
 * Shell Commands
 **************************************************************************************************/

static int cmd_scan(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_scan_status rsp;
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	uint8_t uuid[16] = {0};
	uint8_t timeout;
	int err = 0;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	timeout = wm_atoi(argv[1], &err);
	if (err) {
		printf"Unable to parse input string argument");
		return err;
	}

	if (argc > 2) {
		hex2bin(argv[2], strlen(argv[2]), uuid, 16);
	}

	err = bt_mesh_rpr_scan_start((struct bt_mesh_rpr_cli *)mod->user_data,
				     &srv, argc > 2 ? uuid : NULL, timeout,
				     BT_MESH_RPR_SCAN_MAX_DEVS_ANY, &rsp);
	if (err) {
		printf"Scan start failed: %d", err);
		return err;
	}

	if (rsp.status == BT_MESH_RPR_SUCCESS) {
		printf"Scan started.");
	} else {
		printf"Scan start response: %d", rsp.status);
	}

	return 0;
}

static int cmd_scan_ext(size_t argc, char *argv[])
{
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	uint8_t ad_types[CONFIG_BT_MESH_RPR_AD_TYPES_MAX];
	uint8_t uuid[16] = {0};
	uint8_t timeout;
	int i, err = 0;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	hex2bin(argv[2], strlen(argv[2]), uuid, 16);

	for (i = 0; i < argc - 3; i++) {
		ad_types[i] = wm_atoi(argv[3 + i], &err);
	}

	timeout = wm_atoi(argv[1], &err);
	if (err) {
		printf"Unable to parse input string argument");
		return err;
	}

	err = bt_mesh_rpr_scan_start_ext((struct bt_mesh_rpr_cli *)mod->user_data,
					 &srv, uuid, timeout, ad_types,
					 (argc - 3));
	if (err) {
		printf"Scan start failed: %d", err);
		return err;
	}

	printf"Extended scan started.");

	return 0;
}

static int cmd_scan_srv(size_t argc, char *argv[])
{
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	uint8_t ad_types[CONFIG_BT_MESH_RPR_AD_TYPES_MAX];
	int i, err = 0;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	for (i = 0; i < argc - 1; i++) {
		ad_types[i] = wm_atoi(argv[1 + i], &err);
	}

	if (err) {
		printf"Unable to parse input string argument");
		return err;
	}

	err = bt_mesh_rpr_scan_start_ext((struct bt_mesh_rpr_cli *)mod->user_data,
					 &srv, NULL, 0, ad_types, (argc - 1));
	if (err) {
		printf"Scan start failed: %d", err);
		return err;
	}

	return 0;
}

static int cmd_scan_caps(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_caps caps;
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	err = bt_mesh_rpr_scan_caps_get((struct bt_mesh_rpr_cli *)mod->user_data, &srv, &caps);
	if (err) {
		printf"Scan capabilities get failed: %d", err);
		return err;
	}

	printf"Remote Provisioning scan capabilities of 0x%04x:",
		    bt_mesh_shell_target_ctx.dst);
	printf"\tMax devices:     %u", caps.max_devs);
	printf"\tActive scanning: %s",
		    caps.active_scan ? "true" : "false");
	return 0;
}

static int cmd_scan_get(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_scan_status rsp;
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	err = bt_mesh_rpr_scan_get((struct bt_mesh_rpr_cli *)mod->user_data, &srv, &rsp);
	if (err) {
		printf"Scan get failed: %d", err);
		return err;
	}

	printf"Remote Provisioning scan on 0x%04x:", bt_mesh_shell_target_ctx.dst);
	printf"\tStatus:         %u", rsp.status);
	printf"\tScan type:      %u", rsp.scan);
	printf"\tMax devices:    %u", rsp.max_devs);
	printf"\tRemaining time: %u", rsp.timeout);
	return 0;
}

static int cmd_scan_stop(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_scan_status rsp;
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	err = bt_mesh_rpr_scan_stop((struct bt_mesh_rpr_cli *)mod->user_data, &srv, &rsp);
	if (err || rsp.status) {
		printf"Scan stop failed: %d %u", err, rsp.status);
		return err;
	}

	printf"Remote Provisioning scan on 0x%04x stopped.",
		    bt_mesh_shell_target_ctx.dst);
	return 0;
}

static int cmd_link_get(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_link rsp;
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	err = bt_mesh_rpr_link_get((struct bt_mesh_rpr_cli *)mod->user_data, &srv, &rsp);
	if (err) {
		printf"Link get failed: %d %u", err, rsp.status);
		return err;
	}

	printf"Remote Provisioning Link on 0x%04x:", bt_mesh_shell_target_ctx.dst);
	printf"\tStatus: %u", rsp.status);
	printf"\tState:  %u", rsp.state);
	return 0;
}

static int cmd_link_close(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_link rsp;
	const struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	int err;

	err = bt_mesh_rpr_link_close((struct bt_mesh_rpr_cli *)mod->user_data, &srv, &rsp);
	if (err) {
		printf"Link close failed: %d %u", err, rsp.status);
		return err;
	}

	printf"Remote Provisioning Link on 0x%04x:", bt_mesh_shell_target_ctx.dst);
	printf"\tStatus: %u", rsp.status);
	printf"\tState:  %u", rsp.state);
	return 0;
}

static int cmd_provision_remote(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	uint8_t uuid[16];
	size_t len;
	uint16_t net_idx;
	uint16_t addr;
	int err = 0;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	len = hex2bin(argv[1], strlen(argv[1]), uuid, sizeof(uuid));
	(void)memset(uuid + len, 0, sizeof(uuid) - len);

	net_idx = wm_atoi(argv[2], &err);
	addr = wm_atoi(argv[3], &err);
	if (err) {
		printf"Unable to parse input string argument");
		return err;
	}

	err = bt_mesh_provision_remote((struct bt_mesh_rpr_cli *)mod->user_data,
				       &srv, uuid, net_idx, addr);
	if (err) {
		printf"Prov remote start failed: %d", err);
	}

	return err;
}

static int cmd_reprovision_remote(size_t argc, char *argv[])
{
	struct bt_mesh_rpr_node srv = {
		.addr = bt_mesh_shell_target_ctx.dst,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.ttl = BT_MESH_TTL_DEFAULT,
	};
	bool composition_changed;
	uint16_t addr;
	int err = 0;

	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_REMOTE_PROV_CLI, &mod)) {
		return -ENODEV;
	}

	addr = wm_atoi(argv[1], &err);
	if (err) {
		printf"Unable to parse input string argument");
		return err;
	}

	if (!BT_MESH_ADDR_IS_UNICAST(addr)) {
		printf"Must be a valid unicast address");
		return -EINVAL;
	}

	composition_changed = (argc > 2 && wm_atoi(argv[2], &err));
	if (err) {
		printf"Unable to parse input string argument");
		return err;
	}

	err = bt_mesh_reprovision_remote((struct bt_mesh_rpr_cli *)mod->user_data,
					 &srv, addr, composition_changed);
	if (err) {
		printf"Reprovisioning failed: %d", err);
	}

	return 0;
}

WM_CLI_CMD_DEFINE(mesh_models_rpr_instance_set, cmd_instance_cmds_set, set rpr instance, mesh_instance_set <ElemIdx> - mesh_instance_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_instance_get_all, cmd_instance_cmds_get_all, get rpr instance, mesh_instance_get_all - mesh_instance_get_all cmd);

WM_CLI_CMD_DEFINE(mesh_models_rpr_scan, cmd_scan, remote provision scan, mesh_models_rpr_scan <Timeout(s)> [<UUID(1-16 hex)>]- mesh_models_rpr_scan cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_scan_ext, cmd_scan_ext, remote provision scan extension, mesh_models_rpr_scan_ext <Timeout(s)> <UUID(1-16 hex)> [<ADType> ... ]<Timeout(s)> <UUID(1-16 hex)> [<ADType> ... ]- mesh_models_rpr_scan_ext cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_scan_srv, cmd_scan_srv, remote provision scan server, mesh_models_rpr_scan_srv [<ADType> ... ]- mesh_models_rpr_scan_srv cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_scan_caps, cmd_scan_caps, remote provision scan capibility, mesh_models_rpr_scan_caps - mesh_models_rpr_scan_caps cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_scan_get, cmd_scan_get, remote provision scan get, mesh_models_rpr_scan_get - mesh_models_rpr_scan_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_scan_stop, cmd_scan_stop, remote provision scan stop, mesh_models_rpr_scan_stop - mesh_models_rpr_scan_stop cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_link_get, cmd_link_get, remote provision get link, mesh_models_rpr_link_get - mesh_models_rpr_link_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_link_close, cmd_link_close, remote provision close link, mesh_models_rpr_link_close - mesh_models_rpr_link_close cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_prov_remote, cmd_provision_remote, provision remote device, mesh_models_rpr_prov_remote <UUID(1-16 hex)> <NetKeyIdx> <Addr>- mesh_models_rpr_prov_remote cmd);
WM_CLI_CMD_DEFINE(mesh_models_rpr_reprov_remote, cmd_reprovision_remote, reprovision remote device, mesh_models_rpr_reprov_remote <Addr> [<CompChanged(false, true)>]- mesh_models_rpr_reprov_remote cmd);

