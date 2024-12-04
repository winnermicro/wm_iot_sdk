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
#include <settings/settings.h>

#define LOG_TAG "sc_central"
#include "wm_log.h"
#include "wm_cli.h"

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
//To work with peripheral_sc_general example, this feature should be same as peripheral setting
//#define CONFIG_NOTIFY_EXAMPLE

static struct bt_conn *default_conn;
static struct bt_uuid_16 uuid         = BT_UUID_INIT_16(0);
static const struct bt_uuid *ccc_uuid = BT_UUID_GATT_CCC;

/**peripheral char handle and property, the value is from gatt_discovery*/
static uint16_t gatt_rw_handle = 0;
static uint16_t gatt_rw_prop   = 0;

static struct k_sem app_sem;

static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;
static struct bt_gatt_write_params write_params;
static struct k_work_delayable defer_sec_worker;
static struct k_work_delayable defer_discovery_worker;

static void defer_sec_work_handler(struct k_work *work);
static void defer_discovery_work_handler(struct k_work *work);

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

#if defined(CONFIG_BT_SMP)

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];
    char passkey_str[7];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    snprintk(passkey_str, 7, "%06u", passkey);

    wm_log_info("passkey for %s: %s, please confirm it on remote device", addr, passkey_str);
}

#if defined(CONFIG_BT_PASSKEY_KEYPRESS)
static void auth_passkey_display_keypress(struct bt_conn *conn, enum bt_conn_auth_keypress type)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("passkey keypress notification from %s: type %d", addr, type);
}
#endif

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];
    char passkey_str[7];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    snprintk(passkey_str, 7, "%06u", passkey);

    wm_log_info("confirm passkey for %s: %s", addr, passkey_str);
}

static void auth_passkey_entry(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("input passkey for %s", addr);
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("auth canceled: %s", addr);

    /* clear connection reference for sec mode 3 pairing */
    ///if (pairing_conn) {
    /// bt_conn_unref(pairing_conn);
    /// pairing_conn = NULL;
    ///}
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("comfirm auth pairing with %s", addr);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("%s with %s", bonded ? "BONDED" : "PAIRED", addr);
}

static const char *security_err_str(enum bt_security_err err)
{
    switch (err) {
        case BT_SECURITY_ERR_SUCCESS:
            return "Success";
        case BT_SECURITY_ERR_AUTH_FAIL:
            return "Authentication failure";
        case BT_SECURITY_ERR_PIN_OR_KEY_MISSING:
            return "PIN or key missing";
        case BT_SECURITY_ERR_OOB_NOT_AVAILABLE:
            return "OOB not available";
        case BT_SECURITY_ERR_AUTH_REQUIREMENT:
            return "Authentication requirements";
        case BT_SECURITY_ERR_PAIR_NOT_SUPPORTED:
            return "Pairing not supported";
        case BT_SECURITY_ERR_PAIR_NOT_ALLOWED:
            return "Pairing not allowed";
        case BT_SECURITY_ERR_INVALID_PARAM:
            return "Invalid parameters";
        case BT_SECURITY_ERR_UNSPECIFIED:
            return "Unspecified";
        default:
            return "Unknown";
    }
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("pairing failed with %s reason: %s (%d)", addr, security_err_str(err), err);
}

#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
enum bt_security_err pairing_accept(struct bt_conn *conn, const struct bt_conn_pairing_feat *const feat)
{
    wm_log_info("Remote pairing features: "
                "IO: 0x%02x, OOB: %d, AUTH: 0x%02x, Key: %d, "
                "Init Kdist: 0x%02x, Resp Kdist: 0x%02x",
                feat->io_capability, feat->oob_data_flag, feat->auth_req, feat->max_enc_key_size, feat->init_key_dist,
                feat->resp_key_dist);

    return BT_SECURITY_ERR_SUCCESS;
}
#endif /* CONFIG_BT_SMP_APP_PAIRING_ACCEPT */

void bond_deleted(uint8_t id, const bt_addr_le_t *peer)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(peer, addr, sizeof(addr));
    wm_log_info("bond deleted with %s, ID %u", addr, id);
}

static struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = auth_passkey_display,
#if defined(CONFIG_BT_PASSKEY_KEYPRESS)
    .passkey_display_keypress = auth_passkey_display_keypress,
#endif
    .passkey_entry    = NULL,
    .passkey_confirm  = NULL,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_display_yes_no = {
    .passkey_display  = auth_passkey_display,
    .passkey_entry    = NULL,
    .passkey_confirm  = auth_passkey_confirm,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_input = {
    .passkey_display  = NULL,
    .passkey_entry    = auth_passkey_entry,
    .passkey_confirm  = NULL,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_all = {
    .passkey_display  = auth_passkey_display,
    .passkey_entry    = auth_passkey_entry,
    .passkey_confirm  = auth_passkey_confirm,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_cb auth_cb_status = {
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .pairing_accept = pairing_accept,
#endif
};

static struct bt_conn_auth_info_cb auth_info_cb = {
    .pairing_failed   = auth_pairing_failed,
    .pairing_complete = auth_pairing_complete,
    .bond_deleted     = bond_deleted,
};
#endif

static void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
    wm_log_info("Updated MTU: TX: %d RX: %d bytes", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = { .att_mtu_updated = mtu_updated };

static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("LE conn  param req: %s int (0x%04x, 0x%04x) lat %d to %d", addr, param->interval_min, param->interval_max,
                param->latency, param->timeout);

    return true;
}

static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("LE conn param updated: %s int 0x%04x lat %d to %d", addr, interval, latency, timeout);
}

#if defined(CONFIG_BT_USER_PHY_UPDATE)
static void le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("LE PHY Updated: %s Tx 0x%x, Rx 0x%x", addr, param->tx_phy, param->rx_phy);
}
#endif /* CONFIG_BT_USER_PHY_UPDATE */

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
static void le_data_len_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Data length updated: %s max tx %u (%u us) max rx %u (%u us)", addr, info->tx_max_len, info->tx_max_time,
                info->rx_max_len, info->rx_max_time);
}
#endif /* CONFIG_BT_USER_DATA_LEN_UPDATE */

#if defined(CONFIG_BT_SMP)
static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        wm_log_info("Security changed: %s level %u", addr, level);

        if (bt_conn_get_security(conn) >= BT_SECURITY_L2) {
            k_work_schedule(&defer_discovery_worker, K_MSEC(100));
        }
    } else {
        wm_log_error("Security failed: %s level %u err %d", addr, level, err);
    }
}
#endif

static void defer_discovery_work_handler(struct k_work *work)
{
    int err;
    (void)work;

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
static void defer_sec_work_handler(struct k_work *work)
{
    int sec_err;
    (void)work;
    (void)mtu_exchange(default_conn);

    sec_err = bt_conn_set_security(default_conn, BT_SECURITY_L2);
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

        bt_conn_unref(default_conn);
        default_conn = NULL;

        start_scan();
        return;
    }

    wm_log_info("Connected: %s", addr);

    k_work_schedule(&defer_sec_worker, K_MSEC(500));
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

static struct bt_conn_cb conn_callbacks = {
    .connected        = connected,
    .disconnected     = disconnected,
    .le_param_req     = le_param_req,
    .le_param_updated = le_param_updated,

#if defined(CONFIG_BT_SMP)
    .security_changed = security_changed,
#endif

#if defined(CONFIG_BT_USER_PHY_UPDATE)
    .le_phy_updated = le_phy_updated,
#endif /* CONFIG_BT_USER_PHY_UPDATE */

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
    .le_data_len_updated = le_data_len_updated,
#endif /* CONFIG_BT_USER_DATA_LEN_UPDATE */
};

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

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    wm_log_info("Bluetooth initialized");
    k_work_init_delayable(&defer_sec_worker, defer_sec_work_handler);
    k_work_init_delayable(&defer_discovery_worker, defer_discovery_work_handler);

    bt_conn_cb_register(&conn_callbacks);
    bt_gatt_cb_register(&gatt_callbacks);

#if defined(CONFIG_BT_SMP)
    bt_conn_auth_cb_register(&auth_cb_all);
    bt_conn_auth_info_cb_register(&auth_info_cb);
#endif

    start_scan();

    while (1) {
        k_sleep(K_SECONDS(1));

        vendor_write[0]++; //test only;

        if (default_conn) {
            if (gatt_rw_prop & BT_GATT_CHRC_WRITE) {
                err = gatt_write_with_response(default_conn, gatt_rw_handle, vendor_write,
                                               MIN(bt_gatt_get_mtu(default_conn) - 7, sizeof(vendor_write)));
            } else if (gatt_rw_prop & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
                err = gatt_write_without_response(default_conn, gatt_rw_handle, vendor_write,
                                                  MIN(bt_gatt_get_mtu(default_conn) - 7, sizeof(vendor_write)));
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

static void cmd_auth_method(int argc, char *argv[])
{
    int err;

    if (!strcmp(argv[1], "all")) {
        bt_conn_auth_cb_register(NULL);
        err = bt_conn_auth_cb_register(&auth_cb_all);
    } else if (!strcmp(argv[1], "input")) {
        bt_conn_auth_cb_register(NULL);
        err = bt_conn_auth_cb_register(&auth_cb_input);
    } else if (!strcmp(argv[1], "display")) {
        bt_conn_auth_cb_register(NULL);
        err = bt_conn_auth_cb_register(&auth_cb_display);
    } else if (!strcmp(argv[1], "yesno")) {
        bt_conn_auth_cb_register(NULL);
        err = bt_conn_auth_cb_register(&auth_cb_display_yes_no);
    } else if (!strcmp(argv[1], "status")) {
        bt_conn_auth_cb_register(NULL);
        err = bt_conn_auth_cb_register(&auth_cb_status);
    } else if (!strcmp(argv[1], "none")) {
        bt_conn_auth_cb_register(NULL);
        err = bt_conn_auth_cb_register(NULL);
    } else {
        wm_log_warn("%s", "invalid auth method");
        ;
        return;
    }

    if (err) {
        wm_log_warn("Failed to set auth handlers (%d)", err);
    }

    return;
}

static void cmd_input_passkey(int argc, char *argv[])
{
    unsigned int passkey;
    int err;

    if (!default_conn) {
        wm_log_warn("Not connected");
        return;
    }

    passkey = atoi(argv[1]);
    if (passkey > 999999) {
        wm_log_warn("Passkey should be between 0-999999");
        return;
    }

    err = bt_conn_auth_passkey_entry(default_conn, passkey);
    if (err) {
        wm_log_warn("Failed to set passkey (%d)", err);
        return;
    }

    return;
}

static void cmd_auth_cancel(int argc, char *argv[])
{
    if (!default_conn) {
        wm_log_warn("Not connected");
        return;
    }

    bt_conn_auth_cancel(default_conn);

    return;
}

static void cmd_auth_passkey_confirm(int argc, char *argv[])
{
    if (!default_conn) {
        wm_log_warn("Not connected");
        return;
    }

    bt_conn_auth_passkey_confirm(default_conn);
    return;
}

static void cmd_auth_pairing_confirm(int argc, char *argv[])
{
    if (!default_conn) {
        wm_log_warn("Not connected");
        return;
    }

    bt_conn_auth_pairing_confirm(default_conn);
    return;
}

static void cmd_security(int argc, char *argv[])
{
    int err, sec;
    struct bt_conn_info info;

    if (!default_conn || (bt_conn_get_info(default_conn, &info) < 0)) {
        wm_log_warn("Not connected");
        return;
    }

    if (argc < 2) {
        wm_log_warn("BT_SECURITY_L%d", bt_conn_get_security(default_conn));

        return;
    }

    sec = *argv[1] - '0';

    if ((info.type == BT_CONN_TYPE_LE && (sec < BT_SECURITY_L1 || sec > BT_SECURITY_L4))) {
        wm_log_warn("Invalid LE security level (%d)", sec);
        return;
    }

    err = bt_conn_set_security(default_conn, sec);
    if (err) {
        wm_log_warn("Setting security failed (err %d)", err);
    }

    return;
}

/** @brief cli Function for Setting security level for current connection.
*
*/
WM_CLI_CMD_DEFINE(security, cmd_security, select security level cmd, security<security level LE : 1 - 4> > --security cmd);

/** @brief cli Function for Setting security IO capability.
*
*/
WM_CLI_CMD_DEFINE(auth_method, cmd_auth_method, config auth method cmd,
                  auth_method<method : all input display yesno status> --auth_method cmd);

/** @brief cli Function for Inputting passkey.
*
*/
WM_CLI_CMD_DEFINE(auth_passkey, cmd_input_passkey, input auth passkey cmd, auth_passkey<passkey> --auth_passkey cmd);

/** @brief cli Function for cancelling bonding.
*
*/
WM_CLI_CMD_DEFINE(auth_cancel, cmd_auth_cancel, cancel default connection auth cmd, auth_cancel-- auth_cancel cmd);

/** @brief cli Function for confirming passkey information.
*
*/
WM_CLI_CMD_DEFINE(auth_passkey_confirm, cmd_auth_passkey_confirm, confirm auth passkey cmd,
                  auth_passkey_confirm-- auth_passkey_confirm cmd);

/** @brief cli Function for confirming pairing information.
*
*/
WM_CLI_CMD_DEFINE(auth_pairing_confirm, cmd_auth_pairing_confirm, confirm auth pairing cmd,
                  auth_pairing_confirm-- auth_pairing_confirm cmd);
