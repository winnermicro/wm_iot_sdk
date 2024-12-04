/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"

static int cmd_srpl_clear(size_t argc,
			  char *argv[])
{
	int err = 0;
	uint8_t len;

	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);

	uint16_t range_start = wm_atoi(argv[1], &err);
	bool acked = wm_atoi(argv[2], &err);

	if (argc > 3) {
		len = wm_atoi(argv[3], &err);
	} else {
		len = 0;
	}

	if (err < 0) {
		printf("Invalid command parameter (err %d)", err);
		return err;
	}

	if (acked) {
		uint16_t start_rsp;
		uint8_t len_rsp;

		err = bt_mesh_sol_pdu_rpl_clear(&ctx, range_start, len,
						&start_rsp, &len_rsp);

		if (err) {
			printf("Failed to clear Solicitation PDU RPL (err %d)", err);
		} else {
			printf("Cleared Solicitation PDU RPL with range start=%u len=%u",
				    start_rsp, len_rsp);
		}

		return err;
	}

	err = bt_mesh_sol_pdu_rpl_clear_unack(&ctx, range_start, len);
	if (err) {
		printf("Failed to clear Solicitation PDU RPL (err %d)", err);
	}

	return err;
}

WM_CLI_CMD_DEFINE(mesh_models_sol_pdu_rpl_sol_pdu_rpl_clear, cmd_srpl_clear, clear advertisement solicatin pdu rpl, mesh_models_sol_pdu_rpl_sol_pdu_rpl_clear <RngStart> <Ackd> [RngLen] - mesh_models_sol_pdu_rpl_sol_pdu_rpl_clear cmd);

