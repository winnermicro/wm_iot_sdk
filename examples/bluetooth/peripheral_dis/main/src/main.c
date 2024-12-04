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

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <settings/settings.h>

#define LOG_TAG "peripheral_dis"
#include "wm_log.h"

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_DIS_VAL)),
};

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        wm_log_error("Connection failed (err 0x%02x)", err);
    } else {
        wm_log_info("Connected");
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    wm_log_info("Disconnected (reason 0x%02x)", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

static int settings_runtime_load(void)
{
#if defined(CONFIG_BT_DIS_SETTINGS)
    settings_runtime_set("bt/dis/model", "Zephyr Model", sizeof("Zephyr Model"));
    settings_runtime_set("bt/dis/manuf", "Zephyr Manufacturer", sizeof("Zephyr Manufacturer"));
#if defined(CONFIG_BT_DIS_SERIAL_NUMBER)
    settings_runtime_set("bt/dis/serial", CONFIG_BT_DIS_SERIAL_NUMBER_STR, sizeof(CONFIG_BT_DIS_SERIAL_NUMBER_STR));
#endif
#if defined(CONFIG_BT_DIS_SW_REV)
    settings_runtime_set("bt/dis/sw", CONFIG_BT_DIS_SW_REV_STR, sizeof(CONFIG_BT_DIS_SW_REV_STR));
#endif
#if defined(CONFIG_BT_DIS_FW_REV)
    settings_runtime_set("bt/dis/fw", CONFIG_BT_DIS_FW_REV_STR, sizeof(CONFIG_BT_DIS_FW_REV_STR));
#endif
#if defined(CONFIG_BT_DIS_HW_REV)
    settings_runtime_set("bt/dis/hw", CONFIG_BT_DIS_HW_REV_STR, sizeof(CONFIG_BT_DIS_HW_REV_STR));
#endif
#endif
    return 0;
}

int main(void)
{
    int err;

    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return 0;
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        settings_load();
    }

    settings_runtime_load();

    wm_log_info("Bluetooth initialized");

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        wm_log_error("Advertising failed to start (err %d)", err);
        return 0;
    }

    wm_log_info("Advertising successfully started");
    return 0;
}
