/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <kernel.h>
#include <sys/byteorder.h>

#include <bluetooth/att.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#define LOG_TAG "central_mtu_update"
#include "wm_log.h"

#define MTU_TEST_SERVICE_TYPE        BT_UUID_128_ENCODE(0x2e2b8dc3, 0x06e0, 0x4f93, 0x9bb2, 0x734091c356f0)
#define MTU_TEST_SERVICE_NOTIFY_TYPE BT_UUID_128_ENCODE(0x2e2b8dc3, 0x06e0, 0x4f93, 0x9bb2, 0x734091c356f3)

#define BT_UUID_MTU_TEST             BT_UUID_DECLARE_128(MTU_TEST_SERVICE_TYPE)
#define BT_UUID_MTU_TEST_NOTIFY      BT_UUID_DECLARE_128(MTU_TEST_SERVICE_NOTIFY_TYPE)

static void start_scan(void);

static struct bt_conn *default_conn;

static struct bt_uuid_128 uuid        = BT_UUID_INIT_128(0);
static const struct bt_uuid *ccc_uuid = BT_UUID_GATT_CCC;

static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;

bt_gatt_notify_func_t notify_cb;

static uint8_t notify_func(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data, uint16_t length)
{
    if (!data) {
        wm_log_info("[UNSUBSCRIBED]\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }

    wm_log_info("[NOTIFICATION] data %p length %u", data, length);

    if (notify_cb != NULL) {
        notify_cb(conn, params, data, length);
    }

    return BT_GATT_ITER_STOP;
}

static uint8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
    int err;

    if (!attr) {
        wm_log_info("Discover complete");
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }

    wm_log_debug("[ATTRIBUTE] handle %u", attr->handle);

    if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_MTU_TEST)) {
        memcpy(&uuid, BT_UUID_MTU_TEST_NOTIFY, sizeof(uuid));
        discover_params.uuid         = &uuid.uuid;
        discover_params.start_handle = attr->handle + 1;
        discover_params.type         = BT_GATT_DISCOVER_CHARACTERISTIC;

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
        }
    } else if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_MTU_TEST_NOTIFY)) {
        discover_params.uuid          = ccc_uuid;
        discover_params.start_handle  = attr->handle + 2;
        discover_params.type          = BT_GATT_DISCOVER_DESCRIPTOR;
        subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
        }
    } else {
        subscribe_params.notify     = notify_func;
        subscribe_params.value      = BT_GATT_CCC_NOTIFY;
        subscribe_params.ccc_handle = attr->handle;

        err = bt_gatt_subscribe(conn, &subscribe_params);
        if (err && err != -EALREADY) {
            wm_log_error("Subscribe failed (err %d)", err);
        } else {
            wm_log_info("[SUBSCRIBED]");
        }

        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_STOP;
}

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
        wm_log_error("Create conn to %s failed (%u)", addr_str, err);
        start_scan();
    }
}

static void start_scan(void)
{
    int err;

    err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);
    if (err) {
        wm_log_error("Scanning failed to start (err %d)", err);
        return;
    }

    wm_log_info("Scanning successfully started");
}

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
    wm_log_info("%s: MTU exchange %s (%u)", __func__, err == 0U ? "successful" : "failed", bt_gatt_get_mtu(conn));
}

static struct bt_gatt_exchange_params mtu_exchange_params = { .func = mtu_exchange_cb };

static int mtu_exchange(struct bt_conn *conn)
{
    int err;

    wm_log_info("%s: Current MTU = %u", __func__, bt_gatt_get_mtu(conn));

    wm_log_info("%s: Exchange MTU...", __func__);
    err = bt_gatt_exchange_mtu(conn, &mtu_exchange_params);
    if (err) {
        wm_log_error("%s: MTU exchange failed (err %d)", __func__, err);
    }

    return err;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        wm_log_error("Failed to connect to %s (%u)", addr, err);

        bt_conn_unref(default_conn);
        default_conn = NULL;

        start_scan();
        return;
    }

    wm_log_info("Connected: %s", addr);

    (void)mtu_exchange(conn);

    if (conn == default_conn) {
        memcpy(&uuid, BT_UUID_MTU_TEST, sizeof(uuid));
        discover_params.uuid         = &uuid.uuid;
        discover_params.func         = discover_func;
        discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
        discover_params.end_handle   = BT_ATT_LAST_ATTRIBUTE_HANDLE;
        discover_params.type         = BT_GATT_DISCOVER_PRIMARY;

        err = bt_gatt_discover(default_conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed(err %d)", err);
            return;
        }
    }
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

void run_central_sample(bt_gatt_notify_func_t cb)
{
    int err;

    notify_cb = cb;

    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return;
    }

    wm_log_info("Bluetooth initialized");

    start_scan();
}
