/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"

static int cmd_priv_beacon_get(const struct shell *sh, size_t argc, char *argv[])
{
	struct bt_mesh_priv_beacon val;
	int err;

	err = bt_mesh_priv_beacon_cli_get(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst,
					  &val);
	if (err) {
		printf("Failed to send Private Beacon Get (err %d)", err);
		return 0;
	}

	printf("Private Beacon state: %u, %u", val.enabled, val.rand_interval);

	return 0;
}

static int cmd_priv_beacon_set(const struct shell *sh, size_t argc, char *argv[])
{
	struct bt_mesh_priv_beacon val;
	int err = 0;

	val.enabled = wm_atoi(argv[1],  &err);
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	val.rand_interval = wm_atoi(argv[2],  &err);
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	err = bt_mesh_priv_beacon_cli_set(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst,
					  &val);
	if (err) {
		printf("Failed to send Private Beacon Set (err %d)", err);
		return 0;
	}

	return 0;
}

static int cmd_priv_gatt_proxy_get(const struct shell *sh, size_t argc, char *argv[])
{
	uint8_t state;
	int err;

	err = bt_mesh_priv_beacon_cli_gatt_proxy_get(bt_mesh_shell_target_ctx.net_idx,
						     bt_mesh_shell_target_ctx.dst, &state);
	if (err) {
		printf("Failed to send Private GATT Proxy Get (err %d)", err);
		return 0;
	}

	printf("Private GATT Proxy state: %u", state);

	return 0;
}

static int cmd_priv_gatt_proxy_set(const struct shell *sh, size_t argc, char *argv[])
{
	uint8_t state;
	int err = 0;

	state = wm_atoi(argv[1],  &err);
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	err = bt_mesh_priv_beacon_cli_gatt_proxy_set(bt_mesh_shell_target_ctx.net_idx,
						     bt_mesh_shell_target_ctx.dst, &state);
	if (err) {
		printf("Failed to send Private GATT Proxy Set (err %d)", err);
		return 0;
	}

	return 0;
}

static int cmd_priv_node_id_get(const struct shell *sh, size_t argc, char *argv[])
{
	struct bt_mesh_priv_node_id val;
	uint16_t key_net_idx;
	int err = 0;

	key_net_idx = wm_atoi(argv[1],  &err);

	err = bt_mesh_priv_beacon_cli_node_id_get(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, key_net_idx, &val);
	if (err) {
		printf("Failed to send Private Node Identity Get (err %d)", err);
		return 0;
	}

	printf("Private Node Identity state: (net_idx: %u, state: %u, status: %u)",
		    val.net_idx, val.state, val.status);

	return 0;
}

static int cmd_priv_node_id_set(const struct shell *sh, size_t argc, char *argv[])
{
	struct bt_mesh_priv_node_id val;
	int err = 0;

	val.net_idx = wm_atoi(argv[1],  &err);
	val.state = wm_atoi(argv[2],  &err);

	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	err = bt_mesh_priv_beacon_cli_node_id_set(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, &val);
	if (err) {
		printf("Failed to send Private Node Identity Set (err %d)", err);
		return 0;
	}

	return 0;
}

WM_CLI_CMD_DEFINE(mesh_models_prb_priv_beacon_get, cmd_priv_beacon_get, get private beacon info, mesh_models_prb_priv_beacon_get - mesh_models_prb_priv_beacon_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_prb_priv_beacon_set, cmd_priv_beacon_set, set private beacon info, mesh_models_prb_priv_beacon_set <Val(off, on)> <RandInt(10s steps)> - mesh_models_prb_priv_beacon_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_prb_priv_gatt_proxy_get, cmd_priv_gatt_proxy_get, get private gatt proxy info, mesh_models_prb_priv_gatt_proxy_get - mesh_models_prb_priv_gatt_proxy_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_prb_priv_gatt_proxy_set, cmd_priv_gatt_proxy_set, set private gatt proxy info, mesh_models_prb_priv_gatt_proxy_set <Val(0, 1)> - mesh_models_prb_priv_gatt_proxy_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_prb_priv_node_id_get, cmd_priv_node_id_get, get private node identity, mesh_models_prb_priv_node_id_get <NetKeyIdx> - mesh_models_prb_priv_node_id_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_prb_priv_node_id_set, cmd_priv_node_id_set, set private node identity, mesh_models_prb_priv_node_id_set <NetKeyIdx> <State> - mesh_models_prb_priv_node_id_set cmd);

