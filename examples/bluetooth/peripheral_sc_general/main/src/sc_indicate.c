/*
 * Copyright (c) 2018 WinnerMicro Semiconductor 
 *
 */

#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <settings/settings.h>

#define LOG_TAG "sc_peripheral"
#include "wm_log.h"
#include "wmsdk_config.h"
#include "wm_cli.h"

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

#if CONFIG_BT_CTLR_SCAN_REQ_REPORT
static bt_addr_le_t bonded_address;
#endif

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
    wm_log_dump(WM_LOG_LEVEL_INFO, "content", 32, buf, len);
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

#if defined(CONFIG_BT_SMP)
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
#endif

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

#if CONFIG_BT_CTLR_SCAN_REQ_REPORT
    if (bonded) {
        bt_addr_le_copy(&bonded_address, bt_conn_get_dst(conn));
    }
#endif
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

#if CONFIG_BT_CTLR_SCAN_REQ_REPORT

static void scan_recv(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(info->addr, addr_str, sizeof(addr_str));

    if (bt_addr_le_cmp((const bt_addr_le_t *)&bonded_address, info->addr) == 0) {
        wm_log_info("Device found: %s (RSSI %d)", addr_str, info->rssi);
    }
}

static struct bt_le_scan_cb scan_cb = {
    .recv = scan_recv,
};

static void copy_last_bonded_addr(const struct bt_bond_info *info, void *data)
{
    bt_addr_le_copy(&bonded_address, &info->addr);
}

#endif

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

#if defined(CONFIG_BT_SMP)
    bt_conn_auth_cb_register(&auth_cb_display);
    bt_conn_auth_info_cb_register(&auth_info_cb);
#endif

    err = bt_le_adv_start(BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE, BT_GAP_ADV_FAST_INT_MIN_1, BT_GAP_ADV_FAST_INT_MIN_1,
                                          NULL) /*BT_LE_ADV_CONN*/,
                          ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        wm_log_error("Advertising failed to start (err %d)", err);
        return 0;
    }

#if CONFIG_BT_CTLR_SCAN_REQ_REPORT
    bt_addr_le_copy(&bonded_address, BT_ADDR_LE_NONE);
    bt_foreach_bond(BT_ID_DEFAULT, copy_last_bonded_addr, NULL);

    bt_le_scan_cb_register(&scan_cb);
#endif

    k_sem_init(&app_sem, 0, 1);

    wm_log_info("peripheral indicate service ready");

    while (1) {
        k_sem_take(&app_sem, K_FOREVER);

        mtu_exchange(current_conn);

        while (1) {
            k_sem_take(&app_sem, K_MSEC(1000));
            /*send dummy data*/
            err = app_send_via_indicate(access_value, bt_gatt_get_mtu(current_conn) - 7);
            access_value[0]++;

            if (err != 0) {
                wm_log_warn("indicate send error, err(%d)", err);
                break;
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

    if (!current_conn) {
        wm_log_warn("Not connected");
        return;
    }

    passkey = atoi(argv[1]);
    if (passkey > 999999) {
        wm_log_warn("Passkey should be between 0-999999");
        return;
    }

    err = bt_conn_auth_passkey_entry(current_conn, passkey);
    if (err) {
        wm_log_warn("Failed to set passkey (%d)", err);
        return;
    }

    return;
}

static void cmd_auth_cancel(int argc, char *argv[])
{
    if (!current_conn) {
        wm_log_warn("Not connected");
        return;
    }

    bt_conn_auth_cancel(current_conn);

    return;
}

static void cmd_auth_passkey_confirm(int argc, char *argv[])
{
    if (!current_conn) {
        wm_log_warn("Not connected");
        return;
    }

    bt_conn_auth_passkey_confirm(current_conn);
    return;
}

static void cmd_auth_pairing_confirm(int argc, char *argv[])
{
    if (!current_conn) {
        wm_log_warn("Not connected");
        return;
    }

    bt_conn_auth_pairing_confirm(current_conn);
    return;
}

static void cmd_security(int argc, char *argv[])
{
    int err, sec;
    struct bt_conn_info info;

    if (!current_conn || (bt_conn_get_info(current_conn, &info) < 0)) {
        wm_log_warn("Not connected");
        return;
    }

    if (argc < 2) {
        wm_log_warn("BT_SECURITY_L%d", bt_conn_get_security(current_conn));

        return;
    }

    sec = *argv[1] - '0';

    if ((info.type == BT_CONN_TYPE_LE && (sec < BT_SECURITY_L1 || sec > BT_SECURITY_L4))) {
        wm_log_warn("Invalid LE security level (%d)", sec);
        return;
    }

    err = bt_conn_set_security(current_conn, sec);
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

#endif
