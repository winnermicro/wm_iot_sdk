/**
 * @file
 * @brief Shell APIs for Bluetooth IAS
 *
 * Copyright (c) 2022 Codecoup
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <bluetooth/gatt.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/services/ias.h>

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_cli.h"

#include "bt.h"


static void ias_discover_cb(struct bt_conn *conn, int err)
{
	if (err != 0) {
		printf("Failed to discover IAS err: %d\n", err);
	} else {
		printf("IAS discover success\n");
	}
}

static struct bt_ias_client_cb ias_client_callbacks = {
	.discover = ias_discover_cb,
};

static void cmd_ias_client_init(int argc, char **argv)
{
	int err;

	err = bt_ias_client_cb_register(&ias_client_callbacks);
	if (err != 0) {
		printf("IAS client init failed\n");
	} else {
		printf("IAS client initialized\n");
	}

	return ;
}

static void cmd_ias_client_discover(int argc, char **argv)
{
	int err;

	err = bt_ias_discover(default_conn);
	if (err != 0) {
		printf("IAS discover failed\n");
	}

	return ;
}

static void cmd_ias_client_set_alert(int argc, char **argv)
{
	int err = 0;

	if (strcmp(argv[1], "stop") == 0) {
		err = bt_ias_client_alert_write(default_conn,
						BT_IAS_ALERT_LVL_NO_ALERT);
	} else if (strcmp(argv[1], "mild") == 0) {
		err = bt_ias_client_alert_write(default_conn,
						BT_IAS_ALERT_LVL_MILD_ALERT);
	} else if (strcmp(argv[1], "high") == 0) {
		err = bt_ias_client_alert_write(default_conn,
						BT_IAS_ALERT_LVL_HIGH_ALERT);
	} else {
		printf("Invalid alert level %s\n", argv[1]);
		return ;
	}

	if (err != 0) {
		printf("Failed to send %s alert %d\n", argv[1], err);
	} else {
		printf("Sent alert %s\n", argv[1]);
	}

	return ;
}

static int cmd_ias_client(int argc, char **argv)
{
	if (argc > 1) {
		printf("%s unknown parameter: %s\n",
			    argv[0], argv[1]);
	} else {
		printf("%s Missing subcommand\n", argv[0]);
	}

	return ;
}

WM_CLI_CMD_DEFINE(btiasinit, cmd_ias_client_init, Initialize the client and register callbacks cmd, btiasinit -- btiasinit cmd);
WM_CLI_CMD_DEFINE(btiasdiscovery, cmd_ias_client_discover, Discover IAS cmd, btiasdiscovery -- btiasdiscovery cmd);
WM_CLI_CMD_DEFINE(btiasalert, cmd_ias_client_set_alert, Send alert <stop/mild/high> cmd, btiasalert <stop/mild/high> -- btiasalert cmd);

