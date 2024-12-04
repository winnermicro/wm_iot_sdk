/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "blob.h"

#include <stdlib.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>

#include "utils.h"

extern const struct shell *bt_mesh_shell_ctx_shell;

/***************************************************************************************************
 * Implementation of models' instances
 **************************************************************************************************/

static uint8_t blob_rx_sum;
bool bt_mesh_shell_blob_valid;
static const char *blob_data = "blob";

static int blob_io_open(const struct bt_mesh_blob_io *io,
		    const struct bt_mesh_blob_xfer *xfer,
		    enum bt_mesh_blob_io_mode mode)
{
	blob_rx_sum = 0;
	bt_mesh_shell_blob_valid = true;
	return 0;
}

static int blob_chunk_wr(const struct bt_mesh_blob_io *io,
			 const struct bt_mesh_blob_xfer *xfer,
			 const struct bt_mesh_blob_block *block,
			 const struct bt_mesh_blob_chunk *chunk)
{
	int i;

	for (i = 0; i < chunk->size; ++i) {
		blob_rx_sum += chunk->data[i];
		if (chunk->data[i] !=
		    blob_data[(i + chunk->offset) % sizeof(blob_data)]) {
			bt_mesh_shell_blob_valid = false;
		}
	}

	return 0;
}

static int blob_chunk_rd(const struct bt_mesh_blob_io *io,
			 const struct bt_mesh_blob_xfer *xfer,
			 const struct bt_mesh_blob_block *block,
			 const struct bt_mesh_blob_chunk *chunk)
{
	for (int i = 0; i < chunk->size; ++i) {
		chunk->data[i] =
			blob_data[(i + chunk->offset) % sizeof(blob_data)];
	}

	return 0;
}

static const struct bt_mesh_blob_io dummy_blob_io = {
	.open = blob_io_open,
	.rd = blob_chunk_rd,
	.wr = blob_chunk_wr,
};

const struct bt_mesh_blob_io *bt_mesh_shell_blob_io;

#if defined(CONFIG_BT_MESH_SHELL_BLOB_CLI)

static struct {
	struct bt_mesh_blob_cli_inputs inputs;
	struct bt_mesh_blob_target targets[32];
	struct bt_mesh_blob_target_pull pull[32];
	uint8_t target_count;
	struct bt_mesh_blob_xfer xfer;
} blob_cli_xfer;

static void blob_cli_lost_target(struct bt_mesh_blob_cli *cli,
				 struct bt_mesh_blob_target *target,
				 enum bt_mesh_blob_status reason)
{
	printf("Mesh Blob: Lost target 0x%04x (reason: %u)\n",
		    target->addr, reason);
}

static void blob_cli_caps(struct bt_mesh_blob_cli *cli,
			  const struct bt_mesh_blob_cli_caps *caps)
{
	static const char * const modes[] = {
		"none",
		"push",
		"pull",
		"all",
	};

	if (!caps) {
		printf("None of the targets can be used for BLOB transfer\n");
		return;
	}

	printf("Mesh BLOB: capabilities:\n");
	printf("\tMax BLOB size: %u bytes\n", caps->max_size);
	printf("\tBlock size: %u-%u (%u-%u bytes)\n",
		    caps->min_block_size_log, caps->max_block_size_log,
		    1 << caps->min_block_size_log,
		    1 << caps->max_block_size_log);
	printf("\tMax chunks: %u\n", caps->max_chunks);
	printf("\tChunk size: %u\n", caps->max_chunk_size);
	printf("\tMTU size: %u\n", caps->mtu_size);
	printf("\tModes: %s\n", modes[caps->modes]);
}

static void blob_cli_end(struct bt_mesh_blob_cli *cli,
			 const struct bt_mesh_blob_xfer *xfer, bool success)
{
	if (success) {
		printf("Mesh BLOB transfer complete.\n");
	} else {
		printf("Mesh BLOB transfer failed.\n");
	}
}

static uint8_t get_progress(const struct bt_mesh_blob_xfer_info *info)
{
	uint8_t total_blocks;
	uint8_t blocks_not_rxed = 0;
	uint8_t blocks_not_rxed_size;
	int i;

	total_blocks = DIV_ROUND_UP(info->size, 1U << info->block_size_log);

	blocks_not_rxed_size = DIV_ROUND_UP(total_blocks, 8);

	for (i = 0; i < blocks_not_rxed_size; i++) {
		blocks_not_rxed += info->missing_blocks[i % 8] & (1 << (i % 8));
	}

	return  (total_blocks - blocks_not_rxed) / total_blocks;
}

static void xfer_progress(struct bt_mesh_blob_cli *cli,
			  struct bt_mesh_blob_target *target,
			  const struct bt_mesh_blob_xfer_info *info)
{
	uint8_t progress = get_progress(info);

	printf(
		    "BLOB transfer progress received from target 0x%04x:\n"
		    "\tphase: %d\n"
		    "\tprogress: %u%%\n",
		    target->addr, info->phase, progress);
}

static void xfer_progress_complete(struct bt_mesh_blob_cli *cli)
{
	printf("Determine BLOB transfer progress procedure complete\n");
}

static const struct bt_mesh_blob_cli_cb blob_cli_handlers = {
	.lost_target = blob_cli_lost_target,
	.caps = blob_cli_caps,
	.end = blob_cli_end,
	.xfer_progress = xfer_progress,
	.xfer_progress_complete = xfer_progress_complete,
};

struct bt_mesh_blob_cli bt_mesh_shell_blob_cli = {
	.cb = &blob_cli_handlers
};

#endif /* CONFIG_BT_MESH_SHELL_BLOB_CLI */

#if defined(CONFIG_BT_MESH_SHELL_BLOB_SRV)

static uint32_t blob_time;

static int blob_srv_start(struct bt_mesh_blob_srv *srv,
			  struct bt_mesh_msg_ctx *ctx,
			  struct bt_mesh_blob_xfer *xfer)
{
	printf("BLOB start\n");
	blob_time = k_uptime_get();
	return 0;
}

static void blob_srv_end(struct bt_mesh_blob_srv *srv, uint64_t id,
			 bool success)
{
	if (success) {
		uint32_t duration = k_uptime_delta(&blob_time);

		printf("BLOB completed in %u.%03u s\n",
			(uint32_t)(duration / MSEC_PER_SEC),
			(uint32_t)(duration % MSEC_PER_SEC));
	} else {
		printf("BLOB cancelled\n");
	}
}

static const struct bt_mesh_blob_srv_cb blob_srv_cb = {
	.start = blob_srv_start,
	.end = blob_srv_end,
};

struct bt_mesh_blob_srv bt_mesh_shell_blob_srv = {
	.cb = &blob_srv_cb
};

#endif /* CONFIG_BT_MESH_SHELL_BLOB_SRV */

void bt_mesh_shell_blob_cmds_init(void)
{
	bt_mesh_shell_blob_io = &dummy_blob_io;
}

/***************************************************************************************************
 * Shell Commands
 **************************************************************************************************/

#if defined(CONFIG_BT_MESH_SHELL_BLOB_IO_FLASH)

static struct bt_mesh_blob_io_flash blob_flash_stream;

static void cmd_flash_stream_set(int argc, char *argv[])
{
	uint8_t area_id;
	uint32_t offset = 0;
	int err = 0;

	if (argc < 2) {
		return ;
	}

	area_id = wm_atoi(argv[1], &err);

	if (argc >= 3) {
		offset = wm_atoi(argv[2], &err);
	}

	if (err) {
		printf("Unable to parse input string argument");
		return ;
	}

	err = bt_mesh_blob_io_flash_init(&blob_flash_stream, area_id, offset);
	if (err) {
		printf("Failed to init BLOB IO Flash module: %d\n", err);
	}

	bt_mesh_shell_blob_io = &blob_flash_stream.io;

	printf("Flash stream is initialized with area %u, offset: %u", area_id, offset);

}

static void cmd_flash_stream_unset(int argc, char *argv[])
{
	bt_mesh_shell_blob_io = &dummy_blob_io;
	return 0;
}

#endif /* CONFIG_BT_MESH_SHELL_BLOB_IO_FLASH */

#if defined(CONFIG_BT_MESH_SHELL_BLOB_CLI)

static struct bt_mesh_model *mod_cli;

static void blob_cli_inputs_prepare(uint16_t group)
{
	int i;

	blob_cli_xfer.inputs.ttl = BT_MESH_TTL_DEFAULT;
	blob_cli_xfer.inputs.group = group;
	blob_cli_xfer.inputs.app_idx = bt_mesh_shell_target_ctx.app_idx;
	sys_slist_init(&blob_cli_xfer.inputs.targets);

	for (i = 0; i < blob_cli_xfer.target_count; ++i) {
		/* Reset target context. */
		uint16_t addr = blob_cli_xfer.targets[i].addr;

		memset(&blob_cli_xfer.targets[i], 0, sizeof(struct bt_mesh_blob_target));
		memset(&blob_cli_xfer.pull[i], 0, sizeof(struct bt_mesh_blob_target_pull));
		blob_cli_xfer.targets[i].addr = addr;
		blob_cli_xfer.targets[i].pull = &blob_cli_xfer.pull[i];

		sys_slist_append(&blob_cli_xfer.inputs.targets,
				 &blob_cli_xfer.targets[i].n);
	}
}

static void cmd_tx(int argc, char *argv[])
{
	uint16_t group;
	int err = 0;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI, &mod_cli)) {
		return ;
	}

	blob_cli_xfer.xfer.id = wm_atoi(argv[1], &err);
	blob_cli_xfer.xfer.size = wm_atoi(argv[2], &err);
	blob_cli_xfer.xfer.block_size_log = wm_atoi(argv[3], &err);
	blob_cli_xfer.xfer.chunk_size = wm_atoi(argv[4], &err);

	if (argc >= 6) {
		group = wm_atoi(argv[5], &err);
	} else {
		group = BT_MESH_ADDR_UNASSIGNED;
	}

	if (argc < 7 || !strcmp(argv[6], "push")) {
		blob_cli_xfer.xfer.mode = BT_MESH_BLOB_XFER_MODE_PUSH;
	} else if (!strcmp(argv[6], "pull")) {
		blob_cli_xfer.xfer.mode = BT_MESH_BLOB_XFER_MODE_PULL;
	} else {
		printf("Mode must be either push or pull");
		return ;
	}

	if (argc >= 8) {
		blob_cli_xfer.inputs.timeout_base = wm_atoi(argv[7], &err);
	} else {
		blob_cli_xfer.inputs.timeout_base = 0;
	}

	if (err) {
		printf("Unable to parse input string argument");
		return ;
	}

	if (!blob_cli_xfer.target_count) {
		printf("Failed: No targets");
		return ;
	}

	blob_cli_inputs_prepare(group);

	printf(
		    "Sending transfer 0x%x (mode: %s, %u bytes) to 0x%04x",
		    (uint32_t)blob_cli_xfer.xfer.id,
		    blob_cli_xfer.xfer.mode == BT_MESH_BLOB_XFER_MODE_PUSH ?
			    "push" :
			    "pull",
		    blob_cli_xfer.xfer.size, group);

	err = bt_mesh_blob_cli_send((struct bt_mesh_blob_cli *)mod_cli->user_data,
				    &blob_cli_xfer.inputs,
				    &blob_cli_xfer.xfer, bt_mesh_shell_blob_io);
	if (err) {
		printf("BLOB transfer TX failed (err: %d)", err);
	}

}

static void cmd_target(int argc, char *argv[])
{
	struct bt_mesh_blob_target *t;
	int err = 0;

	if (blob_cli_xfer.target_count ==
	    ARRAY_SIZE(blob_cli_xfer.targets)) {
		printf("No more room");
		return ;
	}

	t = &blob_cli_xfer.targets[blob_cli_xfer.target_count];
	t->addr = wm_atoi(argv[1], &err);

	if (err) {
		printf("Unable to parse input string argument");
		return ;
	}

	printf("Added target 0x%04x", t->addr);

	blob_cli_xfer.target_count++;

}

static void cmd_caps(int argc, char *argv[])
{
	uint16_t group;
	int err = 0;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI, &mod_cli)) {
		return ;
	}

	printf("Retrieving transfer capabilities...");

	if (argc > 1) {
		group = wm_atoi(argv[1], &err);
	} else {
		group = BT_MESH_ADDR_UNASSIGNED;
	}

	if (argc > 2) {
		blob_cli_xfer.inputs.timeout_base = wm_atoi(argv[2], &err);
	} else {
		blob_cli_xfer.inputs.timeout_base = 0;
	}

	if (err) {
		printf("Unable to parse input string argument");
		return ;
	}

	if (!blob_cli_xfer.target_count) {
		printf("Failed: No targets");
		return ;
	}

	blob_cli_inputs_prepare(group);

	err = bt_mesh_blob_cli_caps_get((struct bt_mesh_blob_cli *)mod_cli->user_data,
					&blob_cli_xfer.inputs);
	if (err) {
		printf("Boundary check start failed (err: %d)", err);
	}

}

static void cmd_tx_cancel(int argc,
			      char *argv[])
{
	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI, &mod_cli)) {
		return ;
	}

	printf("Cancelling transfer");
	bt_mesh_blob_cli_cancel((struct bt_mesh_blob_cli *)mod_cli->user_data);

}

static void cmd_tx_get(int argc, char *argv[])
{
	uint16_t group;
	int err;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI, &mod_cli)) {
		return ;
	}

	if (argc > 1) {
		group = wm_atoi(argv[1], &err);
	} else {
		group = BT_MESH_ADDR_UNASSIGNED;
	}

	if (!blob_cli_xfer.target_count) {
		printf("Failed: No targets");
		return ;
	}

	blob_cli_inputs_prepare(group);

	err = bt_mesh_blob_cli_xfer_progress_get((struct bt_mesh_blob_cli *)mod_cli->user_data,
						 &blob_cli_xfer.inputs);
	if (err) {
		printf("ERR %d", err);
	}
	return ;
}

static void cmd_tx_suspend(int argc,
			       char *argv[])
{
	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI, &mod_cli)) {
		return ;
	}

	printf("Suspending transfer");
	bt_mesh_blob_cli_suspend((struct bt_mesh_blob_cli *)mod_cli->user_data);

}

static void cmd_tx_resume(int argc, char *argv[])
{
	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI, &mod_cli)) {
		return ;
	}

	printf("Resuming transfer");
	bt_mesh_blob_cli_resume((struct bt_mesh_blob_cli *)mod_cli->user_data);

}

#endif /* CONFIG_BT_MESH_SHELL_BLOB_CLI */

#if defined(CONFIG_BT_MESH_SHELL_BLOB_SRV)

static struct bt_mesh_model *mod_srv;

static void cmd_rx(int argc, char *argv[])
{
	uint16_t timeout_base;
	uint32_t id;
	int err = 0;

	if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_SRV, &mod_srv)) {
		return ;
	}

	id = wm_atoi(argv[1], &err);
	blob_rx_sum = 0;

	if (argc > 2) {
		timeout_base = wm_atoi(argv[2], &err);
	} else {
		timeout_base = 0U;
	}

	if (err) {
		printf("Unable to parse input string argument");
		return ;
	}

	printf("Receive BLOB 0x%x", id);
	err = bt_mesh_blob_srv_recv((struct bt_mesh_blob_srv *)mod_srv->user_data,
				    id, bt_mesh_shell_blob_io, BT_MESH_TTL_MAX, timeout_base);
	if (err) {
		printf("BLOB RX setup failed (%d)", err);
	}

}

static void cmd_rx_cancel(int argc, char *argv[])
{
	int err;

	if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_SRV, &mod_srv)) {
		return ;
	}

	printf("Cancelling BLOB rx");
	err = bt_mesh_blob_srv_cancel((struct bt_mesh_blob_srv *)mod_srv->user_data);
	if (err) {
		printf("BLOB cancel failed (%d)", err);
	}

}

#endif /* CONFIG_BT_MESH_SHELL_BLOB_SRV */

#if defined(CONFIG_BT_MESH_SHELL_BLOB_CLI)
BT_MESH_SHELL_MDL_INSTANCE_CMDS(cli_instance_cmds, BT_MESH_MODEL_ID_BLOB_CLI, mod_cli);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_target, cmd_target, add blob models taget address, mesh_models_blob_cli_target <Addr> -- mesh_models_blob_cli_target cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_caps, cmd_caps, retrieve transfer capability, mesh_models_blob_cli_caps [<Group> [<TimeoutBase>]] -- mesh_models_blob_cli_caps cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_tx, cmd_tx, start blob transfer, mesh_models_blob_cli_tx <ID> <Size> <BlockSizeLog> <ChunkSize> [<Group> [<Mode(push, pull)> [<TimeoutBase>]]] -- mesh_models_blob_cli_tx cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_tx_cancel, cmd_tx_cancel, cancel blob transfer, mesh_models_blob_tx_cli_cancel -- mesh_models_blob_tx_cli_cancel cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_tx_get, cmd_tx_get, get blob transfer, mesh_models_blob_cli_tx_get [Group] -- mesh_models_blob_cli_tx_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_tx_suspend, cmd_tx_suspend, suspend blob transfer , mesh_models_blob_cli_tx_suspend -- mesh_models_blob_cli_tx_suspend cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_tx_resume, cmd_tx_resume, resume blob transfer, mesh_models_blob_tx_cli_resume -- mesh_models_blob_tx_cli_resume cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_set, cmd_cli_instance_cmds_set, set blob instance, mesh_models_blob_cli_set <ElemIdx> -- mesh_models_blob_cli_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_cli_get_all, cmd_cli_instance_cmds_get_all, get blob instance, mesh_models_blob_cli_get_all -- mesh_models_blob_cli_get_all cmd);

#endif

#if defined(CONFIG_BT_MESH_SHELL_BLOB_SRV)
BT_MESH_SHELL_MDL_INSTANCE_CMDS(srv_instance_cmds, BT_MESH_MODEL_ID_BLOB_SRV, mod_srv);
WM_CLI_CMD_DEFINE(mesh_models_blob_srv_rx, cmd_rx, start blob receive, mesh_models_blob_srv_tx <ID> [<TimeoutBase(10s steps)>] -- mesh_models_blob_srv_tx cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_srv_rx_cancel, cmd_rx_cancel, cancel blob receive, mesh_models_blob_srv_rx_cancel -- mesh_models_blob_srv_rx_cancel cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_srv_set, cmd_srv_instance_cmds_set, set blob instance, mesh_models_blob_srv_set <ElemIdx> -- mesh_models_blob_srv_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_srv_get_all, cmd_srv_instance_cmds_get_all, get blob instance, mesh_models_blob_srv_get_all -- mesh_models_blob_srv_get_all cmd);
#endif

#if defined(CONFIG_BT_MESH_SHELL_BLOB_IO_FLASH)
WM_CLI_CMD_DEFINE(mesh_models_blob_flash_stream_set, cmd_flash_stream_set, set flash stream parameter, mesh_models_blob_flash_stream_set <AreaID> [<Offset>] -- mesh_models_blob_flash_stream_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_blob_flash_stream_unset, cmd_flash_stream_unset, reset flash stream parameter, mesh_models_blob_flash_stream_unset -- mesh_models_blob_flash_stream_unset cmd);
#endif

