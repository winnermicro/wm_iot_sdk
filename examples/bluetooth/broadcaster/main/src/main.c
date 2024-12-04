/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#define LOG_TAG "broadcaster"
#include "wm_log.h"

static uint8_t mfg_data[] = { 0x0c, 0x07, 0x00 };

static const struct bt_data ad[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 3),
};

int main(void)
{
    int err;

    wm_log_info("Starting Broadcaster");

    /* Initialize the Bluetooth Subsystem */
    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return 0;
    }

    wm_log_info("Bluetooth initialized");

    do {
        k_msleep(1000);

        wm_log_info("Sending advertising data: 0x%02X", mfg_data[2]);

        /* Start advertising */
        err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);
        if (err) {
            wm_log_error("Advertising failed to start (err %d)", err);
            return 0;
        }

        k_msleep(3000);

        err = bt_le_adv_stop();
        if (err) {
            wm_log_error("Advertising failed to stop (err %d)", err);
            return 0;
        }

        mfg_data[2]++;

    } while (1);
    return 0;
}
