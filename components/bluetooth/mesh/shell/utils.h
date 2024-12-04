/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_cli.h"
#include "wm_utils.h"


#define BT_MESH_SHELL_MDL_INSTANCE_CMDS(cmd_set_name, mod_id, mod_ptr) \
	static void cmd_##cmd_set_name##_get_all(int argc, char *argv[]) \
	{ \
		bt_mesh_shell_mdl_print_all((mod_id)); \
	} \
	\
	static void cmd_##cmd_set_name##_set(int argc, \
					    char *argv[]) \
	{ \
		int err = 0; \
		uint8_t elem_idx = wm_atoi(argv[1], &err); \
	\
		if (err) { \
			printf("Unable to parse input string arg"); \
			return ; \
		} \
	\
		bt_mesh_shell_mdl_instance_set(&(mod_ptr), (mod_id), elem_idx); \
	} ;

bool bt_mesh_shell_mdl_first_get(uint16_t id, struct bt_mesh_model **mod);

int bt_mesh_shell_mdl_instance_set(struct bt_mesh_model **mod,
			      uint16_t mod_id, uint8_t elem_idx);

int bt_mesh_shell_mdl_print_all(uint16_t mod_id);

int bt_mesh_shell_mdl_cmds_help(size_t argc, char **argv);
