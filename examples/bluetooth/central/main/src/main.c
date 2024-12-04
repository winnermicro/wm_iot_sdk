/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <errno.h>
#include <kernel.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <sys/byteorder.h>

#define LOG_TAG "central"
#include "wm_log.h"

static void start_scan(void);

static struct bt_conn *default_conn;

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type, struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    int err;

    if (default_conn) {
        return;
    }

    /* We're only interested in connectable events */
    if (type != BT_GAP_ADV_TYPE_ADV_IND && type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
        return;
    }

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    wm_log_info("Device found: %s (RSSI %d)", addr_str, rssi);

    /* connect only to devices in close proximity */
    if (rssi < -70) {
        return;
    }

    if (bt_le_scan_stop()) {
        return;
    }

    err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &default_conn);
    if (err) {
        wm_log_warn("Create conn to %s failed (%d)", addr_str, err);
        start_scan();
    }
}

static void start_scan(void)
{
    int err;

    /* This demo doesn't require active scan */
    err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);
    if (err) {
        wm_log_warn("Scanning failed to start (err %d)", err);
        return;
    }

    wm_log_debug("Scanning successfully started");
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        wm_log_warn("Failed to connect to %s (%u)", addr, err);

        bt_conn_unref(default_conn);
        default_conn = NULL;

        start_scan();
        return;
    }

    if (conn != default_conn) {
        return;
    }

    wm_log_info("Connected: %s", addr);

    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (conn != default_conn) {
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Disconnected: %s (reason 0x%02x)", addr, reason);

    bt_conn_unref(default_conn);
    default_conn = NULL;

    start_scan();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

int main(void)
{
    int err;

    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return 0;
    }

    wm_log_info("Bluetooth initialized");

    start_scan();
    return 0;
}
