/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <bluetooth/mesh.h>

#include "utils.h"

#include "../dfu_slot.h"
#include "../dfd_srv_internal.h"
#include "../access.h"

static struct bt_mesh_model *mod;

static void print_receivers_status( struct bt_mesh_dfd_srv *srv,
				   enum bt_mesh_dfd_status status)
{
	printf("{\"status\": %d, \"target_cnt\": %d}\n", status, srv->target_cnt);
}

static void print_dfd_status( struct bt_mesh_dfd_srv *srv,
			     enum bt_mesh_dfd_status status)
{
	printf("{ \"status\": %d, \"phase\": %d\n", status,
		      srv->phase);

	if (srv->phase != BT_MESH_DFD_PHASE_IDLE && srv->dfu.xfer.slot) {
		printf(", \"group\": %d, \"app_idx\": %d, "
			      "\"ttl\": %d, \"timeout_base\": %d, \"xfer_mode\": %d, "
			      "\"apply\": %d, \"slot_idx\": %d", srv->inputs.group,
			      srv->inputs.app_idx, srv->inputs.ttl, srv->inputs.timeout_base,
			      srv->dfu.xfer.blob.mode, srv->apply, srv->slot_idx);
	}

	printf(" }\n");
}

static void print_fw_status(enum bt_mesh_dfd_status status,
			    uint16_t idx, const uint8_t *fwid, size_t fwid_len)
{
	printf("{ \"status\": %d, \"slot_cnt\": %d, \"idx\": %d",
		      status, bt_mesh_dfu_slot_count(), idx);
	if (fwid) {
		printf(", \"fwid\": \"");
		for (size_t i = 0; i < fwid_len; i++) {
			printf("%02x", fwid[i]);
		}
		printf("\"");
	}
	printf(" }\n");
}

static enum bt_mesh_dfu_iter slot_space_cb(const struct bt_mesh_dfu_slot *slot,
					   void *user_data)
{
	size_t *total = user_data;

	*total += slot->size;

	return BT_MESH_DFU_ITER_CONTINUE;
}

static void cmd_dfd_receivers_add(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
		return ;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	if (bt_mesh_dfu_cli_is_busy(&dfd_srv->dfu)) {
		print_receivers_status(dfd_srv,
				       BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION);
        printf("dfu client EBUSY\n");
		return ;
	}

	char *outer_state, *inner_state;

	char *token = strtok_r(argv[1], ";", &outer_state);

	while (token) {
		char *addr_str = strtok_r(token, ",", &inner_state);
		char *img_idx_str = strtok_r(NULL, ",", &inner_state);
		int err = 0;

		if (addr_str == NULL || img_idx_str == NULL) {
			return ;
		}

		uint16_t addr = wm_atoi(addr_str, &err);;
		uint8_t img_idx = wm_atoi(img_idx_str, &err);;

		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}

		enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_receiver_add(
			dfd_srv, addr, img_idx);

		if (status != BT_MESH_DFD_SUCCESS) {
			print_receivers_status(dfd_srv, status);
            printf("dfu srv receiver add failed\n");
            return;
			//return status == BT_MESH_DFD_ERR_INSUFFICIENT_RESOURCES ?-ENOSPC : -EINVAL;
		}

		token = strtok_r(NULL, ";", &outer_state);
	}

	print_receivers_status(dfd_srv, BT_MESH_DFD_SUCCESS);
}

static void cmd_dfd_receivers_delete_all(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client EENV\n");
		return ;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_receivers_delete_all(
		dfd_srv);

	print_receivers_status(dfd_srv, status);

	if (status != BT_MESH_DFD_SUCCESS) {
		//return status == BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION ? -EBUSY : -EINVAL;
	}
}

static void cmd_dfd_receivers_get(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;
	int err = 0;

	uint16_t first = wm_atoi(argv[1], &err);;
	uint16_t cnt = wm_atoi(argv[2], &err);;

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (cnt == 0 || dfd_srv->target_cnt <= first) {
        printf("dfu client EINVAL\n");
		return ;
	}

	cnt = MIN(cnt, dfd_srv->target_cnt - first);
	uint8_t progress = bt_mesh_dfu_cli_progress(&dfd_srv->dfu) / 2;

	printf("{\n\t\"target_cnt\": %d,\n\t\"targets\": {",
		    dfd_srv->target_cnt);
	for (int i = 0; i < cnt; i++) {
		const struct bt_mesh_dfu_target *t = &dfd_srv->targets[i + first];

		printf("\t\t\"%d\": { \"blob_addr\": %d, \"phase\": %d, "
			    "\"status\": %d, \"blob_status\": %d, \"progress\": %d, "
			    "\"img_idx\": %d }%s", i + first, t->blob.addr, t->phase, t->status,
			    t->blob.status, progress, t->img_idx, (i == cnt - 1) ? "" : ",");
	}
	printf("\t}\n}");

}

static void cmd_dfd_capabilities_get(int argc, char *argv[])
{
	size_t size = 0;
	/* Remaining size */
	(void)bt_mesh_dfu_slot_foreach(slot_space_cb, &size);
	size = MIN(size, CONFIG_BT_MESH_DFD_SRV_SLOT_SPACE);

	printf("{ \"targets_max\": %d, \"slot_cnt\": %d, \"slot_max_size\": %d, "
		    "\"slot_space\": %d, \"remaining_space\": %d, \"oob_supported\": false }\n",
		    CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX, CONFIG_BT_MESH_DFU_SLOT_CNT,
		    CONFIG_BT_MESH_DFD_SRV_SLOT_MAX_SIZE, CONFIG_BT_MESH_DFD_SRV_SLOT_SPACE,
		    CONFIG_BT_MESH_DFD_SRV_SLOT_SPACE - size);

}

static void cmd_dfd_get(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	print_dfd_status(dfd_srv, BT_MESH_DFD_SUCCESS);
}

static void cmd_dfd_start(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return ;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;
	struct bt_mesh_dfd_start_params params;
	int err = 0;

	params.app_idx = wm_atoi(argv[1], &err);;
	params.slot_idx = wm_atoi(argv[2], &err);;
	if (argc > 3) {
		params.group = wm_atoi(argv[3], &err);;
	} else {
		params.group = BT_MESH_ADDR_UNASSIGNED;
	}

	if (argc > 4) {
		params.apply = strcmp(argv[4], "true") ? false : true;
	} else {
		params.apply = true;
	}

	if (argc > 5) {
		params.ttl = wm_atoi(argv[5], &err);;
	} else {
		params.ttl = BT_MESH_TTL_DEFAULT;
	}

	if (argc > 6) {
		params.timeout_base = wm_atoi(argv[6], &err);;
	} else {
		params.timeout_base = 0U;
	}

	if (argc > 7) {
		params.xfer_mode = (enum bt_mesh_blob_xfer_mode)wm_atoi(argv[7], &err);;
	} else {
		params.xfer_mode = BT_MESH_BLOB_XFER_MODE_PUSH;
	}

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_start(dfd_srv, &params);

	print_dfd_status(dfd_srv, status);
	if (status != BT_MESH_DFD_SUCCESS) {
        printf("dfu start failed \n");
	}

}

static void cmd_dfd_suspend(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return ;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_suspend(dfd_srv);

	print_dfd_status(dfd_srv, status);
	if (status != BT_MESH_DFD_SUCCESS) {
        printf("dfu suspend failed \n");
	}
}

static void cmd_dfd_cancel(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return ;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_cancel(dfd_srv, NULL);

	print_dfd_status(dfd_srv, status);
	if (status != BT_MESH_DFD_SUCCESS) {
        printf("dfu cancel failed \n");
	}

}

static void cmd_dfd_apply(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return ;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_apply(dfd_srv);

	print_dfd_status(dfd_srv, status);
	if (status != BT_MESH_DFD_SUCCESS) {
        printf("dfu apply failed \n");
	}
}

static void cmd_dfd_fw_get(int argc, char *argv[])
{
	uint8_t fwid[CONFIG_BT_MESH_DFU_FWID_MAXLEN];
	size_t hexlen = strlen(argv[1]);
	size_t fwid_len = hex2bin(argv[1], hexlen, fwid, CONFIG_BT_MESH_DFU_FWID_MAXLEN);

	if (fwid_len != ((hexlen + 1) / 2)) {
		return ;
	}

	int idx = bt_mesh_dfu_slot_get(fwid, fwid_len, NULL);

	if (idx >= 0) {
		print_fw_status(BT_MESH_DFD_SUCCESS, idx, fwid, fwid_len);
	} else {
		print_fw_status(BT_MESH_DFD_ERR_FW_NOT_FOUND, 0xffff, fwid, fwid_len);
	}
}

static void cmd_dfd_fw_get_by_idx(int argc, char *argv[])
{
	int err = 0;
	uint16_t idx = wm_atoi(argv[1], &err);;
	const struct bt_mesh_dfu_slot *slot = bt_mesh_dfu_slot_at(idx);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (slot) {
		print_fw_status(BT_MESH_DFD_SUCCESS, idx, slot->fwid, slot->fwid_len);
	} else {
		print_fw_status(BT_MESH_DFD_ERR_FW_NOT_FOUND, idx, NULL, 0);
	}

}

static void cmd_dfd_fw_delete(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	uint8_t fwid_buf[CONFIG_BT_MESH_DFU_FWID_MAXLEN];
	size_t hexlen = strlen(argv[1]);
	size_t fwid_len = hex2bin(argv[1], hexlen, fwid_buf, CONFIG_BT_MESH_DFU_FWID_MAXLEN);

	if (fwid_len != ((hexlen + 1) / 2)) {
		return ;
	}

	const uint8_t *fwid = &fwid_buf[0];

	enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_fw_delete(dfd_srv,
								   &fwid_len, &fwid);

	print_fw_status(status, 0xffff, fwid, fwid_len);

	if (status != BT_MESH_DFD_SUCCESS) {
        printf("delete fw failed\n");
	}

}

static void cmd_dfd_fw_delete_all(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod)) {
        printf("dfu client ENOENV\n");
        return ;
	}

	struct bt_mesh_dfd_srv *dfd_srv = mod->user_data;

	enum bt_mesh_dfd_status status = bt_mesh_dfd_srv_fw_delete_all(dfd_srv);

	print_fw_status(status, 0xffff, NULL, 0);

	if (status != BT_MESH_DFD_SUCCESS) {
        printf("delete all fw failed\n");
	}
}

BT_MESH_SHELL_MDL_INSTANCE_CMDS(instance_cmds, BT_MESH_MODEL_ID_DFD_SRV, mod);
WM_CLI_CMD_DEFINE(mesh_models_dfd_receivers_add, cmd_dfd_receivers_add, add dfd receiver parameter, mesh_models_dfd_receivers_add <Addr> <FwIdx> [<Addr> <FwIdx>] -- mesh_models_dfd_receivers_add cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_receivers_delete_all, cmd_dfd_receivers_delete_all, delete dfd receiver parameter, mesh_models_dfd_receivers_delete_all -- mesh_models_dfd_receivers_delete_all cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_receivers_get, cmd_dfd_receivers_get, get dfd receiver parameter, mesh_models_dfd_receivers_get <First> <Count> -- mesh_models_dfd_receivers_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_capabilities_get, cmd_dfd_capabilities_get, get dfd capabilities parameter, mesh_models_dfd_capabilities_get -- mesh_models_dfd_capabilities_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_get, cmd_dfd_get, get dfd  parameter, mesh_models_dfd_get -- mesh_models_dfd_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_start, cmd_dfd_start, start dfd transfer, mesh_models_dfd_start <AppKeyIdx> <SlotIdx> [<Group> [<PolicyApply> [<TTL> [<TimeoutBase> [<XferMode>]]]]] -- mesh_models_dfd_start cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_suspend, cmd_dfd_suspend, suspend dfd operation, mesh_models_dfd_suspend -- mesh_models_dfd_suspend cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_cancel, cmd_dfd_cancel, cancel dfd  operation, mesh_models_dfd_cancel -- mesh_models_dfd_cancel cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_apply, cmd_dfd_apply, apply dfd  operation, mesh_models_dfd_apply -- mesh_models_dfd_apply cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_fw_get, cmd_dfd_fw_get, get firmware, mesh_models_dfd_fw_get <FwID> -- mesh_models_dfd_fw_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_fw_get_by_idx, cmd_dfd_fw_get_by_idx, get firmware by index, mesh_models_dfd_fw_get_by_idx <Idx> -- mesh_models_dfd_fw_get_by_idx cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_fw_delete, cmd_dfd_fw_delete, delete firmware, mesh_models_dfd_fw_delete <FwID> -- mesh_models_dfd_fw_delete cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_fw_delete_all, cmd_dfd_fw_delete_all, delete all firmware, mesh_models_dfd_fw_delete_all -- mesh_models_dfd_fw_delete_all cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_set, cmd_instance_cmds_set, set dfd instance, mesh_models_dfd_set <ElemIdx> -- mesh_models_dfd_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_dfd_get_all, cmd_instance_cmds_get_all, get dfd instance, mesh_models_dfd_get_all -- mesh_models_dfd_get_all cmd);


