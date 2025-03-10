/** @file
 *  @brief Bluetooth Telephone and Media Audio Profile shell
 *
 */

/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/audio/tmap.h>
#include <kernel.h>
#include <shell/shell.h>
#include <sys/util.h>

#include "shell/bt.h"

#define TMAP_CG_SUPPORTED                                                                          \
	(IS_ENABLED(CONFIG_BT_CAP_INITIATOR) && IS_ENABLED(CONFIG_BT_BAP_UNICAST_CLIENT) &&        \
	 IS_ENABLED(CONFIG_BT_TBS) && IS_ENABLED(CONFIG_BT_VCP_VOL_CTLR))

#define TMAP_CT_SUPPORTED                                                                          \
	(IS_ENABLED(CONFIG_BT_CAP_ACCEPTOR) && IS_ENABLED(CONFIG_BT_BAP_UNICAST_SERVER) &&         \
	 IS_ENABLED(CONFIG_BT_TBS_CLIENT) &&                                                       \
	 (IS_ENABLED(CONFIG_BT_ASCS_ASE_SNK) &&                                                    \
	  IS_ENABLED(CONFIG_BT_VCP_VOL_REND) == IS_ENABLED(CONFIG_BT_ASCS_ASE_SNK)))

#define TMAP_UMS_SUPPORTED                                                                         \
	(IS_ENABLED(CONFIG_BT_CAP_INITIATOR) &&                                                    \
	 IS_ENABLED(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK) && IS_ENABLED(CONFIG_BT_VCP_VOL_CTLR) && \
	 IS_ENABLED(CONFIG_BT_MCS))

#define TMAP_UMR_SUPPORTED                                                                         \
	(IS_ENABLED(CONFIG_BT_CAP_ACCEPTOR) && IS_ENABLED(CONFIG_BT_ASCS_ASE_SNK) &&               \
	 IS_ENABLED(CONFIG_BT_VCP_VOL_REND))

#define TMAP_BMS_SUPPORTED                                                                         \
	(IS_ENABLED(CONFIG_BT_CAP_INITIATOR) && IS_ENABLED(CONFIG_BT_BAP_BROADCAST_SOURCE))

#define TMAP_BMR_SUPPORTED                                                                         \
	(IS_ENABLED(CONFIG_BT_CAP_ACCEPTOR) && IS_ENABLED(CONFIG_BT_BAP_BROADCAST_SINK))

static int cmd_tmap_init(const struct shell *sh, size_t argc, char **argv)
{
	const enum bt_tmap_role role = (TMAP_CG_SUPPORTED ? BT_TMAP_ROLE_CG : 0U) |
				       (TMAP_CT_SUPPORTED ? BT_TMAP_ROLE_CT : 0U) |
				       (TMAP_UMS_SUPPORTED ? BT_TMAP_ROLE_UMS : 0U) |
				       (TMAP_UMR_SUPPORTED ? BT_TMAP_ROLE_UMR : 0U) |
				       (TMAP_BMS_SUPPORTED ? BT_TMAP_ROLE_BMS : 0U) |
				       (TMAP_BMR_SUPPORTED ? BT_TMAP_ROLE_BMR : 0U);
	int err;

	shell_info(sh, "Registering TMAS with role: %u", role);

	err = bt_tmap_register(role);
	if (err != 0) {
		shell_error(sh, "bt_tmap_register (err %d)", err);

		return -ENOEXEC;
	}

	return 0;
}

static void tmap_discover_cb(enum bt_tmap_role role, struct bt_conn *conn, int err)
{
	if (err != 0) {
		shell_error(ctx_shell, "tmap discovery (err %d)", err);
		return;
	}

	shell_print(ctx_shell, "tmap discovered for conn %p: role 0x%02x", conn, role);
}

static const struct bt_tmap_cb tmap_cb = {
	.discovery_complete = tmap_discover_cb,
};

static int cmd_tmap_discover(const struct shell *sh, size_t argc, char **argv)
{
	int err;

	if (default_conn == NULL) {
		shell_error(sh, "Not connected");

		return -ENOEXEC;
	}

	if (!ctx_shell) {
		ctx_shell = sh;
	}

	err = bt_tmap_discover(default_conn, &tmap_cb);
	if (err != 0) {
		shell_error(sh, "bt_tmap_discover (err %d)", err);

		return -ENOEXEC;
	}

	return err;
}

static int cmd_tmap(const struct shell *sh, size_t argc, char **argv)
{
	if (argc > 1) {
		shell_error(sh, "%s unknown parameter: %s", argv[0], argv[1]);
	} else {
		shell_error(sh, "%s missing subcomand", argv[0]);
	}

	return -ENOEXEC;
}

SHELL_STATIC_SUBCMD_SET_CREATE(tmap_cmds,
	SHELL_CMD_ARG(init, NULL, "Initialize and register the TMAS", cmd_tmap_init, 1, 0),
	SHELL_CMD_ARG(discover, NULL, "Discover TMAS on remote device", cmd_tmap_discover, 1, 0),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_ARG_REGISTER(tmap, &tmap_cmds, "Bluetooth tmap client shell commands", cmd_tmap, 1, 1);
