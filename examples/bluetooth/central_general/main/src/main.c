/*
 * Copyright (c) 2024 WinnerMicro
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

#define LOG_TAG "central"
#include "wm_log.h"

#define BT_UUID_SERVICE         0xFFF0
#define BT_UUID_CHAR_READ_WRITE 0xFFF1
#define BT_UUID_CHAR_INDCIATE   0xFFF2
#define BT_UUID_CHAR_NOTIFY     0xFFF3

#define DEVICE_SERVICE          BT_UUID_DECLARE_16(BT_UUID_SERVICE)
#define DEVICE_CHAR_READ_WRITE  BT_UUID_DECLARE_16(BT_UUID_CHAR_READ_WRITE)
#define DEVICE_CHAR_INDICATE    BT_UUID_DECLARE_16(BT_UUID_CHAR_INDCIATE)
#define DEVICE_CHAR_NOTIFY      BT_UUID_DECLARE_16(BT_UUID_CHAR_NOTIFY)

#define CONFIG_RX_STAT

//define it for notification, otherwise for indication
//#define CONFIG_NOTIFY_EXAMPLE

static struct bt_conn *default_conn;
static struct bt_uuid_16 uuid         = BT_UUID_INIT_16(0);
static const struct bt_uuid *ccc_uuid = BT_UUID_GATT_CCC;

/**peripheral char handle and prop, the value is from gatt_discovery*/
static uint16_t gatt_rw_handle = 0;
static uint16_t gatt_rw_prop   = 0;

static struct k_sem app_sem;

static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;
static struct bt_gatt_write_params write_params;

#ifdef CONFIG_RX_STAT
static uint32_t g_rx_bytes  = 0;
static uint32_t g_timestamp = 0;
#endif

static void start_scan(void);

static uint8_t indicate_func(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data, uint16_t length)
{
    if (!data) {
        wm_log_info("[UNSUBSCRIBED]");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }
#ifdef CONFIG_RX_STAT
    g_rx_bytes += length;
    uint32_t delta = k_uptime_get() - g_timestamp;
    if (delta >= (1U * MSEC_PER_SEC)) {
        g_timestamp = k_uptime_get();
        wm_log_info("rx data rate:%d B/s", g_rx_bytes);
        g_rx_bytes = 0;
    }
#else
    wm_log_dump(WM_LOG_LEVEL_INFO, "[NOTIFICATION] data", 32, data, length);
#endif

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t discover_for_subscribe_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                           struct bt_gatt_discover_params *params)
{
    int err;
    char str[BT_UUID_STR_LEN];

    if (!attr) {
        wm_log_info("Discover complete");
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }
    bt_uuid_to_str(attr->uuid, str, sizeof(str));

    wm_log_info("[ATTRIBUTE] uuid %s, handle %u", str, attr->handle);

    if (!bt_uuid_cmp(discover_params.uuid, DEVICE_SERVICE)) {
#ifdef CONFIG_NOTIFY_EXAMPLE
        memcpy(&uuid, DEVICE_CHAR_NOTIFY, sizeof(uuid));
#else
        memcpy(&uuid, DEVICE_CHAR_INDICATE, sizeof(uuid));
#endif
        discover_params.uuid         = &uuid.uuid;
        discover_params.start_handle = attr->handle + 1;
        discover_params.type         = BT_GATT_DISCOVER_CHARACTERISTIC;

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
        }
#ifdef CONFIG_NOTIFY_EXAMPLE
    } else if (!bt_uuid_cmp(discover_params.uuid, DEVICE_CHAR_NOTIFY)) {
#else
    } else if (!bt_uuid_cmp(discover_params.uuid, DEVICE_CHAR_INDICATE)) {
#endif
        discover_params.uuid          = ccc_uuid;
        discover_params.start_handle  = attr->handle + 2;
        discover_params.type          = BT_GATT_DISCOVER_DESCRIPTOR;
        subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
        }
    } else {
        subscribe_params.notify = indicate_func;
#ifdef CONFIG_NOTIFY_EXAMPLE
        subscribe_params.value = BT_GATT_CCC_NOTIFY;
#else
        subscribe_params.value = BT_GATT_CCC_INDICATE;
#endif
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

static uint8_t discover_for_rw_handle_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                           struct bt_gatt_discover_params *params)
{
    int err;
    char str[BT_UUID_STR_LEN];

    if (!attr) {
        wm_log_info("Discover complete");
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }
    bt_uuid_to_str(attr->uuid, str, sizeof(str));

    wm_log_info("[ATTRIBUTE] uuid %s, handle %u", str, attr->handle);

    if (!bt_uuid_cmp(discover_params.uuid, DEVICE_CHAR_READ_WRITE)) {
        struct bt_gatt_chrc *gatt_chrc = attr->user_data;

        wm_log_info("handle %d, prop 0x%02x, value_handle %d ", attr->handle, gatt_chrc->properties, gatt_chrc->value_handle);
        gatt_rw_prop   = gatt_chrc->properties;
        gatt_rw_handle = gatt_chrc->value_handle;

        /*continue discovery for subscribe*/
        memcpy(&uuid, DEVICE_SERVICE, sizeof(uuid));
        discover_params.uuid         = &uuid.uuid;
        discover_params.func         = discover_for_subscribe_func;
        discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
        discover_params.end_handle   = BT_ATT_LAST_ATTRIBUTE_HANDLE;
        discover_params.type         = BT_GATT_DISCOVER_PRIMARY;

        err = bt_gatt_discover(default_conn, &discover_params);
        if (err) {
            wm_log_error("Discover subscribe failed(err %d)", err);
        }

        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_CONTINUE;
}

static bool __scan_device_found = false;
static bool scan_device_eir_parse(struct bt_data *data, void *user_data)
{
    uint16_t uuid16_value;

    switch (data->type) {
        case BT_DATA_UUID16_ALL:
        case BT_DATA_UUID16_SOME:
            uuid16_value = (uint16_t)data->data[0] | data->data[1] << 8;
            if (uuid16_value == BT_UUID_SERVICE) {
                __scan_device_found = true;
                return false;
            }
            break;
    }

    return true;
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

    __scan_device_found = false;

    /* connect only to devices in close proximity */
    if (rssi >= -70) {
        bt_data_parse(ad, scan_device_eir_parse, (void *)addr);
    }

    if (!__scan_device_found) {
        return;
    }

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    wm_log_info("Device found: %s (RSSI %d)", addr_str, rssi);

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
        memcpy(&uuid, DEVICE_CHAR_READ_WRITE, sizeof(uuid));
        discover_params.uuid         = &uuid.uuid;
        discover_params.func         = discover_for_rw_handle_func;
        discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
        discover_params.end_handle   = BT_ATT_LAST_ATTRIBUTE_HANDLE;
        discover_params.type         = BT_GATT_DISCOVER_CHARACTERISTIC;

        err = bt_gatt_discover(default_conn, &discover_params);
        if (err) {
            wm_log_error("Discover rw handle failed(err %d)", err);
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

static void write_with_response_func_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_write_params *params)
{
    if (err) {
        wm_log_warn("Write complete: err 0x%02x", err);
    }
}

static int gatt_write_with_response(struct bt_conn *conn, uint16_t handle, const uint8_t *data, uint16_t len)
{
    int err;

    write_params.length = len;
    write_params.data   = data;
    write_params.handle = handle;
    write_params.offset = 0;
    write_params.func   = write_with_response_func_cb;

    err = bt_gatt_write(conn, &write_params);

    return err;
}

static void write_without_response_func_cb(struct bt_conn *conn, void *user_data)
{
    wm_log_info("Write(no response) len %d complete", POINTER_TO_UINT(user_data));
}

static int gatt_write_without_response(struct bt_conn *conn, uint16_t handle, const uint8_t *data, uint16_t len)
{
    int err;

    err =
        bt_gatt_write_without_response_cb(conn, handle, data, len, false, write_without_response_func_cb, UINT_TO_POINTER(len));

    return err;
}

int main(void)
{
    int err;
    uint8_t vendor_write[244] = { 0 };

    k_sem_init(&app_sem, 0, 1);

    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return err;
    }

    wm_log_info("Bluetooth initialized");

    start_scan();

    while (1) {
        k_sleep(K_SECONDS(1));

        vendor_write[0]++; //test only;
        if (default_conn) {
            if (gatt_rw_prop & BT_GATT_CHRC_WRITE) {
                err = gatt_write_with_response(default_conn, gatt_rw_handle, vendor_write,
                                               MIN(bt_gatt_get_mtu(default_conn) - 3, sizeof(vendor_write)));
            } else if (gatt_rw_prop & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
                err = gatt_write_without_response(default_conn, gatt_rw_handle, vendor_write,
                                                  MIN(bt_gatt_get_mtu(default_conn) - 3, sizeof(vendor_write)));
            } else {
                err = 0;
            }

            if (err) {
                wm_log_warn("gatt write error, err(%d)", err);
            }
        }
    }

    return 0;
}
