/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"

static void cmd_od_priv_gatt_proxy_set(int argc, char *argv[])
{
	uint8_t val, val_rsp;
	int err = 0;

	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_DEV(bt_mesh_shell_target_ctx.net_idx,
							      bt_mesh_shell_target_ctx.dst);

	if (argc < 2) {
		err = bt_mesh_od_priv_proxy_cli_get(&ctx, &val_rsp);
	} else {
		val = wm_atoi(argv[1], &err);

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		err = bt_mesh_od_priv_proxy_cli_set(&ctx, val, &val_rsp);
	}

	if (err) {
		printf("Unable to send On-Demand Private GATT Proxy Get/Set (err %d)\n", err);
		return ;
	}

	printf("On-Demand Private GATT Proxy is set to 0x%02x\n", val_rsp);

}

WM_CLI_CMD_DEFINE(mesh_models_od_priv_proxy_od_priv_gatt_proxy, cmd_od_priv_gatt_proxy_set, send On-Demand Private GATT Proxy Get/Set, mesh_models_od_priv_proxy_od_priv_gatt_proxy [Dur(s)] - mesh_models_od_priv_proxy_od_priv_gatt_proxy cmd);

