/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/byteorder.h>
#include <kernel.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#define LOG_TAG "sc_only"
#include "wm_log.h"

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

static struct k_work_delayable defer_worker;
static struct bt_conn *current_conn;

static void defer_work_handler(struct k_work *work)
{
    int sec_err;
    (void)work;

    sec_err = bt_conn_set_security(current_conn, BT_SECURITY_L4);
    if (sec_err) {
        wm_log_error("Failed to set security (err %d)", sec_err);
    }
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        wm_log_error("Failed to connect to %s (%u)", addr, err);
        return;
    }

    wm_log_info("Connected %s", addr);

    current_conn = bt_conn_ref(conn);

    k_work_schedule(&defer_worker, K_MSEC(500));
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Disconnected from %s (reason 0x%02x)", addr, reason);

    bt_conn_unref(current_conn);
}

static void identity_resolved(struct bt_conn *conn, const bt_addr_le_t *rpa, const bt_addr_le_t *identity)
{
    char addr_identity[BT_ADDR_LE_STR_LEN];
    char addr_rpa[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
    bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

    wm_log_info("Identity resolved %s -> %s", addr_rpa, addr_identity);
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        wm_log_info("Security changed: %s level %u", addr, level);
    } else {
        wm_log_error("Security failed: %s level %u err %d", addr, level, err);
    }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected         = connected,
    .disconnected      = disconnected,
    .identity_resolved = identity_resolved,
    .security_changed  = security_changed,
};

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Passkey for %s: %06u", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Pairing cancelled: %s", addr);
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    wm_log_info("Pairing Complete");
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    wm_log_info("Pairing Failed (%d). Disconnecting.", reason);
    bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = auth_passkey_display,
    .passkey_entry   = NULL,
    .cancel          = auth_cancel,
};

static struct bt_conn_auth_info_cb auth_cb_info = {
    .pairing_complete = pairing_complete,
    .pairing_failed   = pairing_failed,
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
    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    k_work_init_delayable(&defer_worker, defer_work_handler);

    bt_conn_auth_cb_register(&auth_cb_display);
    bt_conn_auth_info_cb_register(&auth_cb_info);

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        wm_log_error("Advertising failed to start (err %d)", err);
        return 0;
    }

    wm_log_info("Advertising successfully started");
    return 0;
}
