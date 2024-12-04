/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/mesh.h>
#include <ctype.h>
#include <string.h>

#include "mesh/net.h"
#include "mesh/access.h"
#include "utils.h"

bool bt_mesh_shell_mdl_first_get(uint16_t id, struct bt_mesh_model **mod)
{
	const struct bt_mesh_comp *comp = bt_mesh_comp_get();

	for (int i = 0; i < comp->elem_count; i++) {
		*mod = bt_mesh_model_find(&comp->elem[i], id);
		if (*mod) {
			return true;
		}
	}

	return false;
}

int bt_mesh_shell_mdl_instance_set(struct bt_mesh_model **mod,
				 uint16_t mod_id, uint8_t elem_idx)
{
	struct bt_mesh_model *mod_temp;
	const struct bt_mesh_comp *comp = bt_mesh_comp_get();

	if (elem_idx >= comp->elem_count) {
		printf("Invalid element index\n");
		return -EINVAL;
	}

	mod_temp = bt_mesh_model_find(&comp->elem[elem_idx], mod_id);

	if (mod_temp) {
		*mod = mod_temp;
	} else {
		printf("Unable to find model instance for element index %d\n", elem_idx);
		return -ENODEV;
	}

	return 0;
}

int bt_mesh_shell_mdl_print_all(uint16_t mod_id)
{
	const struct bt_mesh_comp *comp = bt_mesh_comp_get();
	struct bt_mesh_model *mod;

	for (int i = 0; i < comp->elem_count; i++) {
		mod = bt_mesh_model_find(&comp->elem[i], mod_id);
		if (mod) {
			printf("Client model instance found at addr 0x%.4X. Element index: %d\n",
				    comp->elem[i].addr, mod->elem_idx);
		}
	}

	return 0;
}

int bt_mesh_shell_mdl_cmds_help(size_t argc, char **argv)
{
	printf("\nFor a detailed description of the commands and arguments in this shell module,\n"
		"please refer to the Zephyr Project documentation online.\n");
    return 0;
}
