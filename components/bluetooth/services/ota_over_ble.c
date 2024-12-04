/*
 * Copyright (c) 2018 WinnerMicro Semiconductor 
 *
 */

#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <settings/settings.h>
#include <bluetooth/services/ota_over_ble.h>

#include <logging/log.h>

/** @brief UUID of the OTA Service. **/
#define BT_UUID_OTA_SVC      0xFFFA
#define BT_UUID_OTA_READ     0xFFFB
#define BT_UUID_OTA_WRITE    0xFFFC
#define BT_UUID_OTA_NOTIFY   0xFFFD

#define BT_UUID_OTA_SERVICE  BT_UUID_DECLARE_16(BT_UUID_OTA_SVC)
#define BT_UUID_OTA_RX       BT_UUID_DECLARE_16(BT_UUID_OTA_WRITE)
#define BT_UUID_OTA_RD       BT_UUID_DECLARE_16(BT_UUID_OTA_READ)
#define BT_UUID_OTA_TX       BT_UUID_DECLARE_16(BT_UUID_OTA_NOTIFY)


typedef enum {
    OTA_BLE_IDLE,
    OTA_BLE_ADVERTISING,
    OTA_BLE_CONNECTED
} ota_ble_state_t;

static struct bt_conn *current_conn;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_OTA_SVC)),
};

static struct bt_svc_cb svc_cb;
static struct k_sem ota_sem;
static bool ble_ota_exit                              = false;
static ota_ble_state_t ble_ota_state                  = OTA_BLE_IDLE;

static void ota_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    if (svc_cb.send_enabled) {
        LOG_INF("Notification has been turned %s %p", value == BT_GATT_CCC_NOTIFY ? "on" : "off", attr);
        svc_cb.send_enabled(value == BT_GATT_CCC_NOTIFY ? BT_SVC_SEND_STATUS_ENABLED : BT_SVC_SEND_STATUS_DISABLED);
    }
}

static ssize_t on_receive(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset,
                          uint8_t flags)
{
    if (svc_cb.received) {
        svc_cb.received(conn, buf, len);
    }
    return len;
}
static ssize_t on_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
  uint8_t *value = attr->user_data;

  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, 0);
}

/* OTA Service Declaration */
BT_GATT_SERVICE_DEFINE(ota_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_OTA_SERVICE),
                        BT_GATT_CHARACTERISTIC(BT_UUID_OTA_RX, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE,
                                               NULL, on_receive, NULL),
                        BT_GATT_CHARACTERISTIC(BT_UUID_OTA_RD, BT_GATT_CHRC_READ, BT_GATT_PERM_READ,
                                              on_read, NULL, NULL),

                        BT_GATT_CHARACTERISTIC(BT_UUID_OTA_TX, BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE,
                                              NULL, NULL, NULL),
                        BT_GATT_CCC(ota_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));


static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
    LOG_DBG("%s: MTU exchange %s (%u)", __func__, err == 0U ? "successful" : "failed", bt_gatt_get_mtu(conn));
}

static struct bt_gatt_exchange_params mtu_exchange_params = { .func = mtu_exchange_cb };

static int mtu_exchange(struct bt_conn *conn)
{
    int err;

    LOG_DBG("%s: Current MTU = %u", __func__, bt_gatt_get_mtu(conn));

    LOG_DBG("%s: Exchange MTU...", __func__);
    err = bt_gatt_exchange_mtu(conn, &mtu_exchange_params);
    if (err) {
        LOG_ERR("%s: MTU exchange failed (err %d)", __func__, err);
    }

    return err;
}

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

    if (ble_ota_state != OTA_BLE_ADVERTISING) {
        return;
    }

    LOG_DBG("Connected %s", addr);

    mtu_exchange(conn);

    current_conn = bt_conn_ref(conn);

    if (svc_cb.connected) {
        svc_cb.connected(conn, err);
    }
    ble_ota_state = OTA_BLE_CONNECTED;
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_DBG("Disconnected: %s (reason 0x%02x)", addr, reason);

    if (current_conn == conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
        if (svc_cb.disconnected) {
            svc_cb.disconnected(conn, reason);
        }
        ble_ota_state = OTA_BLE_ADVERTISING;

        /*wps module is closing*/
        if (ble_ota_exit) {
            k_sem_give(&ota_sem);
            bt_le_adv_stop();
            ble_ota_state = OTA_BLE_IDLE;
        }
    }
}

static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_DBG("LE conn param req: %s int (0x%04x, 0x%04x) lat %d to %d", addr, param->interval_min, param->interval_max,
            param->latency, param->timeout);

    return true;
}

static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_DBG("LE conn param updated: %s int 0x%04x lat %d to %d", addr, interval, latency, timeout);
}

#if defined(CONFIG_BT_SMP)
static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        LOG_DBG("Security changed: %s level %u", addr, level);
    } else {
        LOG_WRN("Security failed: %s level %u err %d", addr, level, err);
    }
}
#endif

#if defined(CONFIG_BT_USER_PHY_UPDATE)
static void le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_DBG("LE PHY Updated: %s Tx 0x%x, Rx 0x%x", addr, param->tx_phy, param->rx_phy);
}
#endif /* CONFIG_BT_USER_PHY_UPDATE */

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
static void le_data_len_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_DBG("Data length updated: %s max tx %u (%u us) max rx %u (%u us)", addr, info->tx_max_len, info->tx_max_time,
            info->rx_max_len, info->rx_max_time);
}
#endif /* CONFIG_BT_USER_DATA_LEN_UPDATE */

BT_CONN_CB_DEFINE(conn_callbacks) = {
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

int bt_ota_init(struct bt_svc_cb *callbacks)
{
    int err;

    if (bt_is_ready() == false) {
        err = bt_enable(NULL);
        if (err) {
            return err;
        }

        if (IS_ENABLED(CONFIG_SETTINGS)) {
            settings_load();
        }
    }

    if (callbacks) {
        svc_cb.received     = callbacks->received;
        svc_cb.sent         = callbacks->sent;
        svc_cb.send_enabled = callbacks->send_enabled;
        svc_cb.connected    = callbacks->connected;
        svc_cb.disconnected = callbacks->disconnected;
    }

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME_AD, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return 0;
    }
    ble_ota_state = OTA_BLE_ADVERTISING;
    k_sem_init(&ota_sem, 0, 1);
    ble_ota_exit = false;

    LOG_INF("OTA over BLE service ready");
    return 0;
}

int bt_ota_uninit(void)
{
    if (ble_ota_state == OTA_BLE_ADVERTISING) {
        LOG_DBG("ota service stop ble advertising");
        bt_le_adv_stop();

    } else if (ble_ota_state == OTA_BLE_CONNECTED) {
        LOG_DBG("ota service disconnect with remote device");
        ble_ota_exit = true;
        bt_conn_disconnect(current_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        k_sem_take(&ota_sem, K_MSEC(3000));
    } else {
        return 0;
    }

    /*Wait host stack state changed, before calling bt_deinit */
    k_sem_take(&ota_sem, K_MSEC(1000));

    ble_ota_state = OTA_BLE_IDLE;
    ble_ota_exit  = false;

    return 0;
}

static void gatt_complete(struct bt_conn *conn, void *user_data)
{

    if (svc_cb.sent) {
        svc_cb.sent(conn);
    }

}

int bt_ota_send(const uint8_t *data, uint16_t len)
{

    struct bt_gatt_notify_params params;

    if (!current_conn) {
        wm_log_warn("notification send to null connection");
        return -ENOTCONN;
    } 

    memset(&params, 0, sizeof(params));
    params.attr = &ota_svc.attrs[4];;
    params.data = data;
    params.len = len;
#if defined(CONFIG_BT_EATT)
    params.chan_opt = BT_ATT_CHAN_OPT_NONE;
#endif /* CONFIG_BT_EATT */
    params.func = gatt_complete;
    if (bt_gatt_is_subscribed(current_conn, params.attr, BT_GATT_CCC_NOTIFY)) {
        return bt_gatt_notify_cb(current_conn, &params);
    } else {
        return bt_gatt_notify_cb(current_conn, &params);
    }

}

