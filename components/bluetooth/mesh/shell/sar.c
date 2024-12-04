/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"
#include "../sar_cfg_internal.h"
static void cmd_tx_get(int argc, char *argv[])
{
	struct bt_mesh_sar_tx rsp;
	int err;

	err = bt_mesh_sar_cfg_cli_transmitter_get(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, &rsp);
	if (err) {
		printf("Failed to send SAR Transmitter Get (err %d)\n", err);
		return;
	}

	printf("Transmitter Get: %u %u %u %u %u %u %u\n",
		    rsp.seg_int_step, rsp.unicast_retrans_count,
		    rsp.unicast_retrans_without_prog_count,
		    rsp.unicast_retrans_int_step, rsp.unicast_retrans_int_inc,
		    rsp.multicast_retrans_count, rsp.multicast_retrans_int);

}

static void cmd_tx_set(int argc, char *argv[])
{
	struct bt_mesh_sar_tx set, rsp;
	int err = 0;

	set.seg_int_step = wm_atoi(argv[1], &err);
	set.unicast_retrans_count = wm_atoi(argv[2], &err);
	set.unicast_retrans_without_prog_count = wm_atoi(argv[3], &err);
	set.unicast_retrans_int_step = wm_atoi(argv[4], &err);
	set.unicast_retrans_int_inc = wm_atoi(argv[5], &err);
	set.multicast_retrans_count = wm_atoi(argv[6], &err);
	set.multicast_retrans_int = wm_atoi(argv[7], &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_sar_cfg_cli_transmitter_set(bt_mesh_shell_target_ctx.net_idx,
						  bt_mesh_shell_target_ctx.dst, &set, &rsp);
	if (err) {
		printf("Failed to send SAR Transmitter Set (err %d)\n", err);
		return ;
	}

	printf("Transmitter Set: %u %u %u %u %u %u %u\n",
		    rsp.seg_int_step, rsp.unicast_retrans_count,
		    rsp.unicast_retrans_without_prog_count,
		    rsp.unicast_retrans_int_step, rsp.unicast_retrans_int_inc,
		    rsp.multicast_retrans_count, rsp.multicast_retrans_int);

}

static void cmd_rx_get(int argc, char *argv[])
{
	struct bt_mesh_sar_rx rsp;
	int err;

	err = bt_mesh_sar_cfg_cli_receiver_get(bt_mesh_shell_target_ctx.net_idx,
					       bt_mesh_shell_target_ctx.dst, &rsp);
	if (err) {
		printf("Failed to send SAR Receiver Get (err %d)\n", err);
		return ;
	}

	printf("Receiver Get: %u %u %u %u %u\n", rsp.seg_thresh,
		    rsp.ack_delay_inc, rsp.ack_retrans_count,
		    rsp.discard_timeout, rsp.rx_seg_int_step);

}

static void cmd_rx_set(int argc, char *argv[])
{
	struct bt_mesh_sar_rx set, rsp;
	int err = 0;

	set.seg_thresh = wm_atoi(argv[1], &err);
	set.ack_delay_inc = wm_atoi(argv[2], &err);
	set.ack_retrans_count = wm_atoi(argv[3], &err);
	set.discard_timeout = wm_atoi(argv[4], &err);
	set.rx_seg_int_step = wm_atoi(argv[5], &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	err = bt_mesh_sar_cfg_cli_receiver_set(bt_mesh_shell_target_ctx.net_idx,
					       bt_mesh_shell_target_ctx.dst, &set, &rsp);
	if (err) {
		printf("Failed to send SAR Receiver Set (err %d)\n", err);
		return ;
	}

	printf("Receiver Set: %u %u %u %u %u\n", rsp.seg_thresh,
		    rsp.ack_delay_inc, rsp.ack_retrans_count,
		    rsp.discard_timeout, rsp.rx_seg_int_step);

}

WM_CLI_CMD_DEFINE(mesh_models_sar_tx_get, cmd_tx_get, get tx sar transmit setting, mesh_models_sar_tx_get  - mesh_models_sar_tx_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_sar_tx_set, cmd_tx_set, set tx sar transmit setting, mesh_models_sar_tx_set <SegIntStep> <UniRetransCnt> <UniRetransWithoutProgCnt> <UniRetransIntStep> <UniRetransIntInc> <MultiRetransCnt> <MultiRetransInt> - mesh_models_sar_tx_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_sar_rx_get, cmd_rx_get, get rx sar transmit setting, mesh_models_sar_rx_get  - mesh_models_sar_rx_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_sar_rx_set, cmd_rx_set, set rx sar transmit setting, mesh_models_sar_rx_set <SegThresh> <AckDelayInc> <DiscardTimeout> <RxSegIntStep> <AckRetransCount> - mesh_models_sar_rx_set cmd);

