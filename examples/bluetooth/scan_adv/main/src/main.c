/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <sys/util.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#define LOG_TAG "scan_adv"
#include "wm_log.h"

static uint8_t mfg_data[] = { 0x0c, 0x07, 0x00 };

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 3),
};

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf)
{
    mfg_data[2]++;
}

int main(void)
{
    struct bt_le_scan_param scan_param = {
        .type     = BT_HCI_LE_SCAN_PASSIVE,
        .options  = BT_LE_SCAN_OPT_NONE,
        .interval = 0x0010,
        .window   = 0x0010,
    };
    int err;

    wm_log_info("Starting Scanner/Advertiser Demo");

    /* Initialize the Bluetooth Subsystem */
    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return 0;
    }

    wm_log_info("Bluetooth initialized");

    err = bt_le_scan_start(&scan_param, scan_cb);
    if (err) {
        wm_log_error("Starting scanning failed (err %d)", err);
        return 0;
    }

    do {
        k_sleep(K_MSEC(400));

        /* Start advertising */
        err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, ad, ARRAY_SIZE(ad), NULL, 0);
        if (err) {
            wm_log_error("Advertising failed to start (err %d)", err);
            return 0;
        }

        k_sleep(K_MSEC(2000));

        err = bt_le_adv_stop();
        if (err) {
            wm_log_error("Advertising failed to stop (err %d)", err);
            return 0;
        }
    } while (1);
    return 0;
}
