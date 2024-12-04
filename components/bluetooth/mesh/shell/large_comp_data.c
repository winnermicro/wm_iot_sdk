/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"

extern const struct shell *bt_mesh_shell_ctx_shell;

static void status_print(int err, char *msg, uint16_t addr, struct bt_mesh_large_comp_data_rsp *rsp)
{
	if (err) {
		printf("Failed to send %s Get message (err %d)\n", msg, err);
		return;
	}

	printf("%s [0x%04x]: page: %u offset: %u total size: %u ", msg, addr, rsp->page,
		    rsp->offset, rsp->total_size);
    for(int i = 0; i < rsp->data->len; i++)
    {
        printf("%02x", rsp->data->data[i]);
    }
    printf("\n");
}

static void cmd_large_comp_data_get(int argc, char *argv[])
{
	NET_BUF_SIMPLE_DEFINE(comp, 64);
	struct bt_mesh_large_comp_data_rsp rsp = {
		.data = &comp,
	};
	uint8_t page;
	uint16_t offset;
	int err = 0;

	net_buf_simple_init(&comp, 0);

	page = wm_atoi(argv[1], &err);
	offset = wm_atoi(argv[2], &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_large_comp_data_get(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst, page, offset, &rsp);
	status_print(err, "Composition Data", bt_mesh_shell_target_ctx.dst, &rsp);

}

static void cmd_models_metadata_get(int argc, char *argv[])
{
	NET_BUF_SIMPLE_DEFINE(metadata, 64);
	struct bt_mesh_large_comp_data_rsp rsp = {
		.data = &metadata,
	};
	uint8_t page;
	uint16_t offset;
	int err = 0;

	net_buf_simple_init(&metadata, 0);

	page = wm_atoi(argv[1], &err);
	offset = wm_atoi(argv[2], &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_models_metadata_get(bt_mesh_shell_target_ctx.net_idx,
					  bt_mesh_shell_target_ctx.dst, page, offset, &rsp);
	status_print(err, "Models Metadata", bt_mesh_shell_target_ctx.dst, &rsp);

}

WM_CLI_CMD_DEFINE(mesh_models_lcd_large_comp_data_get, cmd_large_comp_data_get, get large composition data, mesh_models_lcd_large_comp_data_get <page> <offset> - mesh_models_lcd_large_comp_data_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_lcd_models_metadata_get, cmd_models_metadata_get, get model composition data, mesh_models_lcd_models_metadata_get <page> <offset> - mesh_models_lcd_models_metadata_get cmd);

