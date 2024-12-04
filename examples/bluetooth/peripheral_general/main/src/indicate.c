/*
 * Copyright (c) 2018 WinnerMicro Semiconductor 
 *
 */

#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <settings/settings.h>

#define LOG_TAG "peripheral"
#include "wm_log.h"
#include "wmsdk_config.h"

#if defined(CONFIG_INDICATE_EXAMPLE)

#define DEVICE_NAME             CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN         (sizeof(DEVICE_NAME) - 1)

#define BT_UUID_SERVICE         0xFFF0
#define BT_UUID_CHAR_READ_WRITE 0xFFF1
#define BT_UUID_CHAR_INDCIATE   0xFFF2

#define DEVICE_SERVICE          BT_UUID_DECLARE_16(BT_UUID_SERVICE)
#define DEVICE_CHAR_READ_WRITE  BT_UUID_DECLARE_16(BT_UUID_CHAR_READ_WRITE)
#define DEVICE_CHAR_INDICATE    BT_UUID_DECLARE_16(BT_UUID_CHAR_INDCIATE)

#define ACCESS_VALUE_LEN        20

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_SERVICE)),
};

static struct k_sem app_sem;
static bool app_indication_enable                     = false;
static struct bt_conn *current_conn                   = NULL;
static struct bt_gatt_indicate_params indicate_params = { 0 };
static uint8_t access_value[ACCESS_VALUE_LEN]         = {
    0x01,
    0x02,
    0x03,
    0x04,
};

static void indicate_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    char str[BT_UUID_STR_LEN];

    bt_uuid_to_str(attr->uuid, str, sizeof(str));

    if (value) {
        wm_log_info("Turn on %s %s", str, value == BT_GATT_CCC_INDICATE ? "indication" : "notification");
        app_indication_enable = true;
        k_sem_give(&app_sem);
    } else {
        app_indication_enable = false;
        wm_log_info("Turn off %s indication", str);
    }
}

static ssize_t on_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    uint8_t *value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(access_value));
}

static ssize_t on_receive(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset,
                          uint8_t flags)
{
    wm_log_info("recv(%d):", len);
    wm_log_dump(WM_LOG_LEVEL_DEBUG, "content", 16, buf, len);
    return len;
}

static void on_sent(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t err)
{
    ARG_UNUSED(params);

    if (err) {
        wm_log_error("Data sent, conn %p", (void *)conn);
        return;
    }
}

static void on_destroy(struct bt_gatt_indicate_params *params)
{
    ARG_UNUSED(params);
    k_sem_give(&app_sem);
}

/* Peripheral Service Declaration */
BT_GATT_SERVICE_DEFINE(indicate_svc, BT_GATT_PRIMARY_SERVICE(DEVICE_SERVICE),
                       BT_GATT_CHARACTERISTIC(DEVICE_CHAR_READ_WRITE, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                                              BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, on_read, on_receive, &access_value),

                       BT_GATT_CHARACTERISTIC(DEVICE_CHAR_INDICATE, BT_GATT_CHRC_INDICATE, BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(indicate_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    struct bt_conn_info info;
    int info_err;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    info_err = bt_conn_get_info(conn, &info);
    if (info_err || info.role != BT_CONN_ROLE_PERIPHERAL) {
        return;
    }

    wm_log_info("Connected %s", addr);

    current_conn = bt_conn_ref(conn);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Disconnected: %s (reason 0x%02x)", addr, reason);

    if (current_conn == conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }
}

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

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
static void le_data_len_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Data length updated: %s max tx %u (%u us) max rx %u (%u us)", addr, info->tx_max_len, info->tx_max_time,
                info->rx_max_len, info->rx_max_time);
}
#endif

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

static void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
    wm_log_info("Updated MTU: TX: %d RX: %d bytes", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = { .att_mtu_updated = mtu_updated };

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

static int app_send_via_indicate(const uint8_t *data, uint16_t len)
{
    const struct bt_gatt_attr *attr = &indicate_svc.attrs[3];

    indicate_params.attr    = attr;
    indicate_params.data    = data;
    indicate_params.len     = len;
    indicate_params.func    = on_sent;
    indicate_params.destroy = on_destroy;

    if (!current_conn) {
        wm_log_warn("Indication send to null connection");
        return -ENOTCONN;
    } else if (bt_gatt_is_subscribed(current_conn, attr, BT_GATT_CCC_INDICATE)) {
        return bt_gatt_indicate(current_conn, &indicate_params);
    } else {
        wm_log_warn("indication not enabled");
        return -ENOENT;
    }
}

int run_indicate_sample(void)
{
    int err;

    err = bt_enable(NULL);
    if (err) {
        return err;
    }

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    bt_conn_cb_register(&conn_callbacks);

    bt_gatt_cb_register(&gatt_callbacks);

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        wm_log_error("Advertising failed to start (err %d)", err);
        return 0;
    }

    k_sem_init(&app_sem, 0, 1);

    wm_log_info("peripheral indicate service ready");

    while (1) {
        k_sem_take(&app_sem, K_FOREVER);

        mtu_exchange(current_conn);

        while (1) {
            k_sem_take(&app_sem, K_MSEC(1000));
            /*send dummy data*/
            err = app_send_via_indicate(access_value, bt_gatt_get_mtu(current_conn) - 3);
            access_value[0]++;

            if (err != 0) {
                wm_log_warn("indicate send error, err(%d)", err);
                break;
            }
        }
    }

    return 0;
}

#endif
