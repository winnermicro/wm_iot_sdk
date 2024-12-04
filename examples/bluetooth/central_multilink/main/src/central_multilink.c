/* main.c - Application main entry point */

/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <errno.h>
#include <kernel.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <sys/byteorder.h>

#define LOG_TAG "multilink"
#include <logging/log.h>

#define SCAN_INTERVAL 0x0020 /* 20 ms */
#define SCAN_WINDOW   0x0010 /* 10 ms */
#define INIT_INTERVAL 0x0020 /* 20 ms */
#define INIT_WINDOW   0x0010 /* 10 ms */
#define CONN_INTERVAL 0x0028 /* 50 ms */
#define CONN_LATENCY  0
#define CONN_TIMEOUT  MIN(MAX((CONN_INTERVAL * 125 * MAX(CONFIG_BT_MAX_CONN, 6) / 1000), 10), 3200)

static void start_scan(void);

static struct bt_conn *conn_connecting;
static uint8_t volatile conn_count;
static bool volatile is_disconnecting;
struct k_sem wait_sem;
struct k_work_delayable deferred_work;

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type, struct net_buf_simple *ad)
{
    struct bt_conn_le_create_param create_param = {
        .options        = BT_CONN_LE_OPT_NONE,
        .interval       = INIT_INTERVAL,
        .window         = INIT_WINDOW,
        .interval_coded = 0,
        .window_coded   = 0,
        .timeout        = 0,
    };
    struct bt_le_conn_param conn_param = {
        .interval_min = CONN_INTERVAL,
        .interval_max = CONN_INTERVAL,
        .latency      = CONN_LATENCY,
        .timeout      = CONN_TIMEOUT,
    };
    char addr_str[BT_ADDR_LE_STR_LEN];
    int err;

    if (conn_connecting) {
        wm_log_warn("Discard scan report");
        return;
    }

    /* We're only interested in connectable events */
    if (type != BT_GAP_ADV_TYPE_ADV_IND && type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND && type != BT_GAP_ADV_TYPE_EXT_ADV) {
        return;
    }

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

    /* connect only to devices in close proximity */
    if (rssi < -70) {
        return;
    }
    wm_log_info("Device found: %s (RSSI %d)", addr_str, rssi);

    if (bt_le_scan_stop()) {
        LOG_WRN("Scanning  stop failed");
        return;
    }

    wm_log_info("Stop scan and connect to: %s (RSSI %d)", addr_str, rssi);
    conn_param.interval_max = conn_param.interval_max + conn_count * 5;
    conn_param.interval_min = conn_param.interval_min + conn_count * 4;
    err                     = bt_conn_le_create(addr, &create_param, &conn_param, &conn_connecting);
    if (err) {
        wm_log_error("Create conn to %s failed (%d)", addr_str, err);
        k_work_schedule(&deferred_work, K_MSEC(1000));
    }
}

static void start_scan(void)
{
    struct bt_le_scan_param scan_param = {
        .type     = BT_HCI_LE_SCAN_PASSIVE,
        .options  = BT_LE_SCAN_OPT_NONE,
        .interval = SCAN_INTERVAL,
        .window   = SCAN_WINDOW,
    };
    int err;

    err = bt_le_scan_start(&scan_param, device_found);
    if (err) {
        wm_log_error("Scanning failed to start (err %d)", err);
        k_work_schedule(&deferred_work, K_MSEC(1000));
        return;
    }

    wm_log_info("Scanning successfully started");
}

#if defined(CONFIG_BT_GATT_CLIENT)
static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
    wm_log_info("MTU exchange %u %s (%u)", bt_conn_index(conn), err == 0U ? "successful" : "failed", bt_gatt_get_mtu(conn));
}

static struct bt_gatt_exchange_params mtu_exchange_params[CONFIG_BT_MAX_CONN];

static int mtu_exchange(struct bt_conn *conn)
{
    uint8_t conn_index;
    int err;

    conn_index = bt_conn_index(conn);

    wm_log_info("MTU (%u): %u", conn_index, bt_gatt_get_mtu(conn));

    mtu_exchange_params[conn_index].func = mtu_exchange_cb;

    err = bt_gatt_exchange_mtu(conn, &mtu_exchange_params[conn_index]);
    if (err) {
        wm_log_error("MTU exchange failed (err %d)", err);
    } else {
        wm_log_info("Exchange pending...");
    }

    return err;
}
#endif /* CONFIG_BT_GATT_CLIENT */

static void deferred_scan_work(struct k_work *work)
{
    (void)work;
    start_scan();
}

static void connected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (reason) {
        wm_log_warn("Failed to connect to %s (%u)", addr, reason);

        bt_conn_unref(conn_connecting);
        conn_connecting = NULL;

        k_work_schedule(&deferred_work, K_MSEC(1000));
        return;
    }

    conn_connecting = NULL;
    conn_count++;

    wm_log_info("Connected (%u): %s", conn_count, addr);

    if (conn_count < CONFIG_BT_MAX_CONN) {
        k_work_schedule(&deferred_work, K_MSEC(1000));
    } else {
        k_sem_give(&wait_sem);
    }

#if defined(CONFIG_BT_SMP)
    int err = bt_conn_set_security(conn, BT_SECURITY_L2);

    if (err) {
        wm_log_error("Failed to set security (%d).", err);
    }
#endif

#if defined(CONFIG_BT_GATT_CLIENT)
    mtu_exchange(conn);
#endif
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Disconnected: %s (reason 0x%02x), conn_count is %d", addr, reason, conn_count);

    bt_conn_unref(conn);

    if ((conn_count == 1U) && is_disconnecting) {
        is_disconnecting = false;
        k_work_schedule(&deferred_work, K_MSEC(1000));
    }
    conn_count--;
}

static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("LE conn param req: %s int (0x%04x, 0x%04x) lat %d to %d", addr, param->interval_min, param->interval_max,
                param->latency, param->timeout);

    return false;
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

static void iteration_disconnect(struct bt_conn *conn, void *data)
{
    char addr[BT_ADDR_LE_STR_LEN];
    int err;
    int retry_counter = 6;
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

retry_disconn:
    wm_log_info("Disconnecting %s...", addr);
    err = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err) {
        wm_log_warn("Failed disconnection %s.", addr);
        k_sem_take(&wait_sem, K_SECONDS(1));
        retry_counter--;
        if (retry_counter == 0) {
            wm_log_error("Attention, Failed disconnection %s.", addr);
            return;
        }
        goto retry_disconn;
    }
    wm_log_info("success.");
    //k_sem_take(&wait_sem, K_SECONDS(1));
}

int init_central(uint8_t iterations)
{
    int err;
    int total_counter = 0;

    err = bt_enable(NULL);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return err;
    }

    wm_log_info("Bluetooth initialized, CONFIG_BT_MAX_CONN is %d , loop is %d", CONFIG_BT_MAX_CONN, iterations);

    bt_conn_cb_register(&conn_callbacks);

    k_sem_init(&wait_sem, 0, 1);

    k_work_init_delayable(&deferred_work, deferred_scan_work);

    start_scan();

    while (iterations--) {
    c_wait:
        err = k_sem_take(&wait_sem, K_SECONDS(2));
        if (err) {
            goto c_wait;
        }

        wm_log_info("Ready disconnecting all");

        k_sem_take(&wait_sem, K_SECONDS(3));

        if (conn_count == 0) {
            start_scan();
            continue;
        }

        wm_log_info("Start disconnecting all...");
        k_sem_take(&wait_sem, K_SECONDS(3));
        if (conn_count == 0) {
            start_scan();
            continue;
        }
        is_disconnecting = true;

        bt_conn_foreach(BT_CONN_TYPE_LE, iteration_disconnect, NULL);

        while (is_disconnecting) {
            k_sem_take(&wait_sem, K_MSEC(10));
        }
        total_counter++;

        wm_log_info("All disconnected, iterations[%d].", total_counter);
    }

    return 0;
}
