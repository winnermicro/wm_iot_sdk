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

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf)
{
    char info[64] = { 0 };
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    sprintf(info, "addr:%s, rssi %d, adv_type %d", addr_str, rssi, adv_type);

    wm_log_dump(WM_LOG_LEVEL_INFO, info, 32, buf->data, buf->len);
}

int main(void)
{
    struct bt_le_scan_param scan_param = {
        .type     = BT_HCI_LE_SCAN_ACTIVE,
        .options  = BT_LE_SCAN_OPT_NONE,
        .interval = 0x0010,
        .window   = 0x0010,
    };
    int err;

    wm_log_info("Starting Scanner Demo");

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
        k_sleep(K_MSEC(1000));
    } while (1);

    return 0;
}
