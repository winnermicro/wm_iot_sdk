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

static struct bt_mesh_model *mod;

static void show_faults(uint8_t test_id, uint16_t cid, uint8_t *faults,
			size_t fault_count)
{
	size_t i;

	if (!fault_count) {
		printf("Health Test ID 0x%02x Company ID 0x%04x: no faults\n", test_id,
			    cid);
		return;
	}

	printf("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\n", test_id, cid,
		    fault_count);

	for (i = 0; i < fault_count; i++) {
		printf("\t0x%02x\n", faults[i]);
	}
}

static void cmd_fault_get(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_HEALTH_CLI, &mod)) {
		return ;
	}

	struct bt_mesh_health_cli *cli = mod->user_data;
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);
	uint8_t faults[32];
	size_t fault_count;
	uint8_t test_id;
	uint16_t cid;
	int err = 0;

	cid = wm_atoi(argv[1], &err);;
	if (err) {
		printf("Unable to parse input string argument");
		return ;
	}

	fault_count = sizeof(faults);

	err = bt_mesh_health_cli_fault_get(cli, ctx.addr ? &ctx : NULL, cid, &test_id, faults,
					   &fault_count);
	if (err) {
		printf("Failed to send Health Fault Get (err %d)", err);
	} else {
		show_faults(test_id, cid, faults, fault_count);
	}

}

static int fault_clear(int argc, char *argv[], bool acked)
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_HEALTH_CLI, &mod)) {
		return -ENODEV;
	}

	struct bt_mesh_health_cli *cli = mod->user_data;
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);
	uint8_t test_id;
	uint16_t cid;
	int err = 0;

	cid = wm_atoi(argv[1], &err);;
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	if (acked) {
		uint8_t faults[32];
		size_t fault_count = sizeof(faults);

		err = bt_mesh_health_cli_fault_clear(cli, ctx.addr ? &ctx : NULL, cid, &test_id,
						     faults, &fault_count);
		if (err) {
			printf("Failed to send Health Fault Clear (err %d)", err);
		} else {
			show_faults(test_id, cid, faults, fault_count);
		}

		return err;
	}

	err = bt_mesh_health_cli_fault_clear_unack(cli, ctx.addr ? &ctx : NULL, cid);
	if (err) {
		printf("Health Fault Clear Unacknowledged failed (err %d)", err);
	}

	return err;
}

static void cmd_fault_clear(int argc, char *argv[])
{
	fault_clear(argc, argv, true);
}

static void cmd_fault_clear_unack(int argc, char *argv[])
{
	fault_clear(argc, argv, false);
}

static int fault_test(int argc, char *argv[], bool acked)
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_HEALTH_CLI, &mod)) {
		return -ENODEV;
	}

	struct bt_mesh_health_cli *cli = mod->user_data;
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);
	uint8_t test_id;
	uint16_t cid;
	int err = 0;

	cid = wm_atoi(argv[1], &err);;
	test_id = wm_atoi(argv[2], &err);;
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	if (acked) {
		uint8_t faults[32];
		size_t fault_count = sizeof(faults);

		err = bt_mesh_health_cli_fault_test(cli, ctx.addr ? &ctx : NULL, cid, test_id,
						    faults, &fault_count);
		if (err) {
			printf("Failed to send Health Fault Test (err %d)", err);
		} else {
			show_faults(test_id, cid, faults, fault_count);
		}

		return err;
	}

	err = bt_mesh_health_cli_fault_test_unack(cli, ctx.addr ? &ctx : NULL, cid, test_id);
	if (err) {
		printf("Health Fault Test Unacknowledged failed (err %d)", err);
	}

	return err;
}

static void cmd_fault_test(int argc, char *argv[])
{
	fault_test(argc, argv, true);
}

static void cmd_fault_test_unack(int argc, char *argv[])
{
	fault_test(argc, argv, false);
}

static void cmd_period_get(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_HEALTH_CLI, &mod)) {
		return ;
	}

	struct bt_mesh_health_cli *cli = mod->user_data;
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);
	uint8_t divisor;
	int err;

	err = bt_mesh_health_cli_period_get(cli, ctx.addr ? &ctx : NULL, &divisor);
	if (err) {
		printf("Failed to send Health Period Get (err %d)", err);
	} else {
		printf("Health FastPeriodDivisor: %u", divisor);
	}

}

static int period_set(int argc, char *argv[], bool acked)
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_HEALTH_CLI, &mod)) {
		return -ENODEV;
	}

	struct bt_mesh_health_cli *cli = mod->user_data;
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);
	uint8_t divisor;
	int err = 0;

	divisor = wm_atoi(argv[1], &err);;
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	if (acked) {
		uint8_t updated_divisor;

		err = bt_mesh_health_cli_period_set(cli, ctx.addr ? &ctx : NULL, divisor,
						    &updated_divisor);
		if (err) {
			printf("Failed to send Health Period Set (err %d)", err);
		} else {
			printf("Health FastPeriodDivisor: %u", updated_divisor);
		}

		return err;
	}

	err = bt_mesh_health_cli_period_set_unack(cli, ctx.addr ? &ctx : NULL, divisor);
	if (err) {
		printf("Failed to send Health Period Set (err %d)", err);
	}

	return err;
}

static void cmd_period_set(int argc, char *argv[])
{
	period_set(argc, argv, true);
}

static void cmd_period_set_unack(int argc, char *argv[])
{
	period_set(argc, argv, false);
}

static void cmd_attention_get(int argc, char *argv[])
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_HEALTH_CLI, &mod)) {
		return ;
	}

	struct bt_mesh_health_cli *cli = mod->user_data;
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);
	uint8_t attention;
	int err;

	err = bt_mesh_health_cli_attention_get(cli, ctx.addr ? &ctx : NULL, &attention);
	if (err) {
		printf("Failed to send Health Attention Get (err %d)", err);
	} else {
		printf("Health Attention Timer: %u", attention);
	}

}

static int attention_set(int argc, char *argv[], bool acked)
{
	if (!mod && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_HEALTH_CLI, &mod)) {
		return -ENODEV;
	}

	struct bt_mesh_health_cli *cli = mod->user_data;
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(bt_mesh_shell_target_ctx.app_idx,
							      bt_mesh_shell_target_ctx.dst);
	uint8_t attention;
	int err = 0;

	attention = wm_atoi(argv[1], &err);;
	if (err) {
		printf("Unable to parse input string argument");
		return err;
	}

	if (acked) {
		uint8_t updated_attention;

		err = bt_mesh_health_cli_attention_set(cli, ctx.addr ? &ctx : NULL, attention,
						       &updated_attention);
		if (err) {
			printf("Failed to send Health Attention Set (err %d)", err);
		} else {
			printf("Health Attention Timer: %u", updated_attention);
		}

		return err;
	}

	err = bt_mesh_health_cli_attention_set_unack(cli, ctx.addr ? &ctx : NULL, attention);
	if (err) {
		printf("Failed to send Health Attention Set (err % d) ", err);
	}

	return err;
}

static void cmd_attention_set(int argc, char *argv[])
{
	attention_set(argc, argv, true);
}

static void cmd_attention_set_unack(int argc, char *argv[])
{
	attention_set(argc, argv, false);
}

BT_MESH_SHELL_MDL_INSTANCE_CMDS(instance_cmds, BT_MESH_MODEL_ID_HEALTH_CLI, mod);

WM_CLI_CMD_DEFINE(mesh_models_health_fault_get, cmd_fault_get, get health information, mesh_models_health_fault_get <CID> -- mesh_models_health_fault_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_fault_clear, cmd_fault_clear, clear health information, mesh_models_health_fault_clear <CID> -- mesh_models_health_fault_clear cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_fault_clear_unack, cmd_fault_clear_unack, clear unacked health information, mesh_models_health_fault_clear_unack <CID> -- mesh_models_health_fault_clear_unack cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_fault_test, cmd_fault_test, send health test information, mesh_models_health_fault_test <CID> <TestID> -- mesh_models_health_fault_test cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_fault_test_unack, cmd_fault_test_unack, send health unacked test information, mesh_models_health_fault_test_unack <CID> <TestID> -- mesh_models_health_fault_test_unack cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_period_get, cmd_period_get, send health get period information, mesh_models_health_period_get -- mesh_models_health_period_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_period_set, cmd_period_set, send health set period divisor, mesh_models_health_period_set <Divisor> -- mesh_models_health_period_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_period_set_unack, cmd_period_set_unack, send health set unacked period divisor, mesh_models_health_period_set_unack <Divisor> -- mesh_models_health_period_set_unack cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_attention_get, cmd_attention_get, send health get attention information, mesh_models_health_attention_get -- mesh_models_health_attention_get cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_attention_set, cmd_attention_set, send health set attention duration, mesh_models_health_attention_set <Time(s)> -- mesh_models_health_attention_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_attention_set_unack, cmd_attention_set_unack, send health set unacked attention duration, mesh_models_health_attention_set_unack <Time(s)> -- mesh_models_health_attention_set_unack cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_set, cmd_instance_cmds_set, set health instance, mesh_models_health_set <ElemIdx> -- mesh_models_health_set cmd);
WM_CLI_CMD_DEFINE(mesh_models_health_get_all, cmd_instance_cmds_get_all, get health instance, mesh_models_health_get_all -- mesh_models_health_get_all cmd);

