/**
 * @file
 * @brief Shell APIs for Bluetooth IAS
 *
 * Copyright (c) 2022 Codecoup
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bt.h"
#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/conn.h>

#include <bluetooth/gatt.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/services/ias.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_cli.h"


static void alert_stop(void)
{
	printf("Alert stopped\n");
}

static void alert_start(void)
{
	printf("Mild alert started\n");
}

static void alert_high_start(void)
{
	printf("High alert started\n");
}

BT_IAS_CB_DEFINE(ias_callbacks) = {
	.no_alert = alert_stop,
	.mild_alert = alert_start,
	.high_alert = alert_high_start,
};

static int cmd_ias_local_alert_stop(int argc, char **argv)
{
	const int result = bt_ias_local_alert_stop();

	if (result) {
		printf("Local alert stop failed: %d", result);
	} else {
		printf("Local alert stopped");
	}

	return result;
}

WM_CLI_CMD_DEFINE(btiasalertstop, cmd_ias_local_alert_stop, stop local alert cmd, btiasalertstop -- btiasalertstop cmd);

