/*
 * Copyright (c) 2018 WinnerMicro Semiconductor 
 *
 */

#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <settings/settings.h>
#include <bluetooth/services/wps_over_ble.h>

#include <logging/log.h>

/** @brief UUID of the WPS Service. **/
#define BT_UUID_WPS_VAL      0x1824
#define BT_UUID_WPS_WRITE    0x2ABC
#define BT_UUID_WPS_INDICATE 0x2ABC

#define BT_UUID_WPS_SERVICE  BT_UUID_DECLARE_16(BT_UUID_WPS_VAL)
#define BT_UUID_WPS_RX       BT_UUID_DECLARE_16(BT_UUID_WPS_WRITE)
#define BT_UUID_WPS_TX       BT_UUID_DECLARE_16(BT_UUID_WPS_INDICATE)


typedef enum {
    WPS_BLE_IDLE,
    WPS_BLE_ADVERTISING,
    WPS_BLE_CONNECTED
} wps_ble_state_t;

static struct bt_conn *current_conn;

static uint8_t mfg_data[] = { 0x0C, 0x07, 0x00, 0x10 };

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 4),
};

static struct bt_svc_cb wps_cb;
static struct k_sem wps_sem;
static bool ble_wps_exit                              = false;
static bool ble_enable_by_wps                         = false;
static wps_ble_state_t ble_wps_state                  = WPS_BLE_IDLE;
static struct bt_gatt_indicate_params indicate_params = { 0 };

static void nus_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    if (wps_cb.send_enabled) {
        LOG_DBG("Indication has been turned %s", value == BT_GATT_CCC_INDICATE ? "on" : "off");
        wps_cb.send_enabled(value == BT_GATT_CCC_INDICATE ? BT_SVC_SEND_STATUS_ENABLED : BT_SVC_SEND_STATUS_DISABLED);
    }
}

static ssize_t on_receive(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset,
                          uint8_t flags)
{
    if (wps_cb.received) {
        wps_cb.received(conn, buf, len);
    }
    return len;
}

static void on_sent(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t err)
{
    ARG_UNUSED(params);

    if (err) {
        LOG_ERR("Data send, conn %p", (void *)conn);
        return;
    }
    if (wps_cb.sent) {
        wps_cb.sent(conn);
    }
}

static void on_destroy(struct bt_gatt_indicate_params *params)
{
    ARG_UNUSED(params);
}

/* WPS Service Declaration */
BT_GATT_SERVICE_DEFINE(wps_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_WPS_SERVICE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_WPS_RX, BT_GATT_CHRC_WRITE | BT_GATT_CHRC_INDICATE, BT_GATT_PERM_WRITE,
                                              NULL, on_receive, NULL),
                       BT_GATT_CCC(nus_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

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

    if (ble_wps_state != WPS_BLE_ADVERTISING) {
        return;
    }

    LOG_DBG("Connected %s", addr);

    current_conn = bt_conn_ref(conn);

    if (wps_cb.connected) {
        wps_cb.connected(conn, err);
    }
    ble_wps_state = WPS_BLE_CONNECTED;
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_DBG("Disconnected: %s (reason 0x%02x)", addr, reason);

    if (current_conn == conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
        if (wps_cb.disconnected) {
            wps_cb.disconnected(conn, reason);
        }
        ble_wps_state = WPS_BLE_ADVERTISING;

        /*wps module is closing*/
        if (ble_wps_exit) {
            k_sem_give(&wps_sem);
            bt_le_adv_stop();
            ble_wps_state = WPS_BLE_IDLE;
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

int bt_wps_init(struct bt_svc_cb *callbacks)
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
        ble_enable_by_wps = true;
    } else {
        ble_enable_by_wps = false;
    }

    if (callbacks) {
        wps_cb.received     = callbacks->received;
        wps_cb.sent         = callbacks->sent;
        wps_cb.send_enabled = callbacks->send_enabled;
        wps_cb.connected    = callbacks->connected;
        wps_cb.disconnected = callbacks->disconnected;
    }

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME_AD, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return 0;
    }
    ble_wps_state = WPS_BLE_ADVERTISING;
    k_sem_init(&wps_sem, 0, 1);
    ble_wps_exit = false;

    LOG_INF("WiFi provision service ready");
    return 0;
}

int bt_wps_uninit(void)
{
    if (ble_wps_state == WPS_BLE_ADVERTISING) {
        LOG_DBG("wps stop ble advertising");
        bt_le_adv_stop();

    } else if (ble_wps_state == WPS_BLE_CONNECTED) {
        LOG_DBG("wps disconnect with remote device");
        ble_wps_exit = true;
        bt_conn_disconnect(current_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        k_sem_take(&wps_sem, K_MSEC(3000));
    } else {
        return 0;
    }

    /*Wait host stack state changed, before calling bt_deinit */
    k_sem_take(&wps_sem, K_MSEC(1000));

    /*wps module is shared with example and at_cmd. if bt stack enabled by at_cmd,
      which should not be disable when wps module closing*/

    if (ble_enable_by_wps == true) {
        bt_disable();
    }

    ble_wps_state = WPS_BLE_IDLE;
    ble_wps_exit  = false;

    return 0;
}

int bt_wps_send(const uint8_t *data, uint16_t len)
{
    const struct bt_gatt_attr *attr = &wps_svc.attrs[1];

    indicate_params.attr    = attr;
    indicate_params.data    = data;
    indicate_params.len     = len;
    indicate_params.func    = on_sent;
    indicate_params.destroy = on_destroy;

    if (!current_conn) {
        return -ENOTCONN;
    } else if (bt_gatt_is_subscribed(current_conn, attr, BT_GATT_CCC_INDICATE)) {
        return bt_gatt_indicate(current_conn, &indicate_params);
    } else {
        /*the old wifi_prov app does not subscribe indication, which is fixed by:
        CONFIG_BT_GATT_ENFORCE_SUBSCRIPTION=n */
        return bt_gatt_indicate(current_conn, &indicate_params);
    }
}
