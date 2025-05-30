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
#include <bluetooth/services/bas.h>
#include <bluetooth/services/hrs.h>
#include <bluetooth/services/ias.h>

#include "cts.h"

#define LOG_TAG "peripheral"
#include "wm_pm.h"
#include "wm_log.h"

/* Custom Service Variables */
#define BT_UUID_CUSTOM_SERVICE_VAL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

static struct bt_uuid_128 vnd_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_VAL);

static struct bt_uuid_128 vnd_enc_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1));

static struct bt_uuid_128 vnd_auth_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2));

#define VND_MAX_LEN 20

static uint8_t vnd_value[VND_MAX_LEN + 1]      = { 'V', 'e', 'n', 'd', 'o', 'r' };
static uint8_t vnd_auth_value[VND_MAX_LEN + 1] = { 'V', 'e', 'n', 'd', 'o', 'r' };
static uint8_t vnd_wwr_value[VND_MAX_LEN + 1]  = { 'V', 'e', 'n', 'd', 'o', 'r' };

static ssize_t read_vnd(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}

static ssize_t write_vnd(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset,
                         uint8_t flags)
{
    uint8_t *value = attr->user_data;

    if (offset + len > VND_MAX_LEN) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);
    value[offset + len] = 0;

    return len;
}

static uint8_t simulate_vnd;
static uint8_t indicating;
static struct bt_gatt_indicate_params ind_params;

static void vnd_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    simulate_vnd = (value == BT_GATT_CCC_INDICATE) ? 1 : 0;
}

static void indicate_cb(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t err)
{
    wm_log_info("Indication %s", err != 0U ? "fail" : "success");
}

static void indicate_destroy(struct bt_gatt_indicate_params *params)
{
    wm_log_info("Indication complete");
    indicating = 0U;
}

#define VND_LONG_MAX_LEN 74
static uint8_t vnd_long_value[VND_LONG_MAX_LEN + 1] = {
    'V', 'e', 'n', 'd', 'o', 'r', ' ', 'd', 'a', 't', 'a', '1', 'V', 'e', 'n', 'd', 'o', 'r', ' ', 'd', 'a', 't', 'a', '2', 'V',
    'e', 'n', 'd', 'o', 'r', ' ', 'd', 'a', 't', 'a', '3', 'V', 'e', 'n', 'd', 'o', 'r', ' ', 'd', 'a', 't', 'a', '4', 'V', 'e',
    'n', 'd', 'o', 'r', ' ', 'd', 'a', 't', 'a', '5', 'V', 'e', 'n', 'd', 'o', 'r', ' ', 'd', 'a', 't', 'a', '6', '.', ' '
};

static ssize_t write_long_vnd(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
                              uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;

    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        return 0;
    }

    if (offset + len > VND_LONG_MAX_LEN) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);
    value[offset + len] = 0;

    return len;
}

static const struct bt_uuid_128 vnd_long_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef3));

static struct bt_gatt_cep vnd_long_cep = {
    .properties = BT_GATT_CEP_RELIABLE_WRITE,
};

static int signed_value;

static ssize_t read_signed(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(signed_value));
}

static ssize_t write_signed(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
                            uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;

    if (offset + len > sizeof(signed_value)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    return len;
}

static const struct bt_uuid_128 vnd_signed_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x13345678, 0x1234, 0x5678, 0x1334, 0x56789abcdef3));

static const struct bt_uuid_128 vnd_write_cmd_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef4));

static ssize_t write_without_rsp_vnd(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
                                     uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;

    if (!(flags & BT_GATT_WRITE_FLAG_CMD)) {
        /* Write Request received. Reject it since this Characteristic
		 * only accepts Write Without Response.
		 */
        return BT_GATT_ERR(BT_ATT_ERR_WRITE_REQ_REJECTED);
    }

    if (offset + len > VND_MAX_LEN) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);
    value[offset + len] = 0;

    return len;
}

/* Vendor Primary Service Declaration */
BT_GATT_SERVICE_DEFINE(
    vnd_svc, BT_GATT_PRIMARY_SERVICE(&vnd_uuid),
    BT_GATT_CHARACTERISTIC(&vnd_enc_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_INDICATE,
                           BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT, read_vnd, write_vnd, vnd_value),
    BT_GATT_CCC(vnd_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE_ENCRYPT),
    BT_GATT_CHARACTERISTIC(&vnd_auth_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN, read_vnd, write_vnd, vnd_auth_value),
    BT_GATT_CHARACTERISTIC(&vnd_long_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_EXT_PROP,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE | BT_GATT_PERM_PREPARE_WRITE, read_vnd, write_long_vnd,
                           &vnd_long_value),
    BT_GATT_CEP(&vnd_long_cep),
    BT_GATT_CHARACTERISTIC(&vnd_signed_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_AUTH,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_signed, write_signed, &signed_value),
    BT_GATT_CHARACTERISTIC(&vnd_write_cmd_uuid.uuid, BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_WRITE, NULL,
                           write_without_rsp_vnd, &vnd_wwr_value));

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HRS_VAL), BT_UUID_16_ENCODE(BT_UUID_BAS_VAL),
                  BT_UUID_16_ENCODE(BT_UUID_CTS_VAL)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
    wm_log_info("Updated MTU: TX: %d RX: %d bytes", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = { .att_mtu_updated = mtu_updated };

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

static void alert_stop(void)
{
    wm_log_info("Alert stopped");
}

static void alert_start(void)
{
    wm_log_info("Mild alert started");
}

static void alert_high_start(void)
{
    wm_log_info("High alert started");
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

BT_IAS_CB_DEFINE(ias_callbacks) = {
    .no_alert   = alert_stop,
    .mild_alert = alert_start,
    .high_alert = alert_high_start,
};

static void bt_ready(void)
{
    int err;
    struct bt_le_adv_param param = {};

    wm_log_info("Bluetooth initialized");

    cts_init();

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }
    param.id           = BT_ID_DEFAULT;
    param.interval_min = 320;
    param.interval_max = 320;
    param.options |= BT_LE_ADV_OPT_USE_NAME;
    param.options |= BT_LE_ADV_OPT_CONNECTABLE;
    err = bt_le_adv_start(&param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        wm_log_error("Advertising failed to start (err %d)", err);
        return;
    }

    wm_log_info("Advertising successfully started");
}

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

static struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = auth_passkey_display,
    .passkey_entry   = NULL,
    .cancel          = auth_cancel,
};

static void bas_notify(void)
{
    uint8_t battery_level = bt_bas_get_battery_level();

    battery_level--;

    if (!battery_level) {
        battery_level = 100U;
    }

    bt_bas_set_battery_level(battery_level);
}

static void hrs_notify(void)
{
    static uint8_t heartrate = 90U;

    /* Heartrate measurements simulation */
    heartrate++;
    if (heartrate == 160U) {
        heartrate = 90U;
    }

    bt_hrs_notify(heartrate);
}

int main(void)
{
    struct bt_gatt_attr *vnd_ind_attr;
    char str[BT_UUID_STR_LEN];
    int err;
    wm_pm_config_t config = { WM_PM_MODE_DEEP_SLEEP, 0, 0 };

    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return 0;
    }

    bt_ready();

    /**Turn on system lowpower mode*/
    err = wm_pm_set_config(&config);

    if (err) {
        wm_log_error("System lowpower init failed (err %d)", err);
    }

    /**Turn on bluetooth lowpower mode*/
    bt_lowpower_enable();

    bt_gatt_cb_register(&gatt_callbacks);

    bt_conn_auth_cb_register(&auth_cb_display);

    vnd_ind_attr = bt_gatt_find_by_uuid(vnd_svc.attrs, vnd_svc.attr_count, &vnd_enc_uuid.uuid);
    bt_uuid_to_str(&vnd_enc_uuid.uuid, str, sizeof(str));
    wm_log_info("Indicate VND attr %p (UUID %s)", vnd_ind_attr, str);

    /* Implement notification. At the moment there is no suitable way
	 * of starting delayed work so we do it here
	 */
    while (1) {
        k_sleep(K_SECONDS(1));

        /* Current Time Service updates only when time is changed */
        cts_notify();

        /* Heartrate measurements simulation */
        hrs_notify();

        /* Battery level simulation */
        bas_notify();

        /* Vendor indication simulation */
        if (simulate_vnd && vnd_ind_attr) {
            if (indicating) {
                continue;
            }

            ind_params.attr    = vnd_ind_attr;
            ind_params.func    = indicate_cb;
            ind_params.destroy = indicate_destroy;
            ind_params.data    = &indicating;
            ind_params.len     = sizeof(indicating);

            if (bt_gatt_indicate(NULL, &ind_params) == 0) {
                indicating = 1U;
            }
        }
    }
    return 0;
}
