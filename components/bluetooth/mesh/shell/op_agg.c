/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"

static int cmd_seq_start(size_t argc, char *argv[])
{
	uint16_t elem_addr;
	int err = 0;

	elem_addr = wm_atoi(argv[1],&err);
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	bt_mesh_shell_target_ctx.dst = elem_addr;
	printf("mesh dst set to 0x%04x", elem_addr);

	err = bt_mesh_op_agg_cli_seq_start(bt_mesh_shell_target_ctx.net_idx,
					   bt_mesh_shell_target_ctx.app_idx,
					   bt_mesh_shell_target_ctx.dst, elem_addr);
	if (err) {
		printf("Failed to configure Opcodes Aggregator Context (err %d)", err);
	}

	return 0;
}

static int cmd_seq_send(size_t argc, char *argv[])
{
	int err;

	err = bt_mesh_op_agg_cli_seq_send();
	if (err) {
		printf("Failed to send Opcodes Aggregator Sequence message (err %d)", err);
	}

	return 0;
}

static int cmd_seq_abort(size_t argc, char *argv[])
{
	bt_mesh_op_agg_cli_seq_abort();

	return 0;
}

WM_CLI_CMD_DEFINE(mesh_models_opagg_seq_start, cmd_seq_start, configure aggregator context, mesh_models_opagg_seq_start <ElemAddr> - mesh_models_opagg_seq_start cmd);
WM_CLI_CMD_DEFINE(mesh_models_opagg_seq_send, cmd_seq_send, send aggregator sequence, mesh_models_opagg_seq_send - mesh_models_opagg_seq_send cmd);
WM_CLI_CMD_DEFINE(mesh_models_opagg_seq_abort, cmd_seq_abort, abort sending aggregator sequence, mesh_models_opagg_seq_abort - mesh_models_opagg_seq_abort cmd);

