/* main.c - Application main entry point */

/*
 * Copyright (c) 2023 WinnerMicro Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#define LOG_TAG "sc_hid"
#include "wm_log.h"
#include "wmsdk_config.h"
#include "wm_cli.h"

#define OUTPUT_REPORT_MAX_LEN              1
#define OUTPUT_REPORT_BIT_MASK_CAPS_LOCK   0x02
#define OUTPUT_REPORT_BIT_MASK_NUM_LOCK    0x01
#define OUTPUT_REPORT_BIT_MASK_SCROLL_LOCK 0x04

#define INPUT_REP_KEYS_REF_ID              1
#define INPUT_REP_CONSUMER_CTRL_REF_ID     2
#define INPUT_REP_SYSTEM_CTRL_REF_ID       3
#define OUTPUT_REP_KEYS_REF_ID             1

enum {
    HIDS_REMOTE_WAKE          = BIT(0),
    HIDS_NORMALLY_CONNECTABLE = BIT(1),
};

struct hids_info {
    uint16_t version; /* version number of base USB HID Specification */
    uint8_t code;     /* country HID Device hardware is localized for. */
    uint8_t flags;
} __packed;

struct hids_report {
    uint8_t id;   /* report id */
    uint8_t type; /* report type */
} __packed;

static struct hids_info info = {
    .version = 0x0000,
    .code    = 0x00,
    .flags   = HIDS_NORMALLY_CONNECTABLE,
};

enum {
    HIDS_INPUT   = 0x01,
    HIDS_OUTPUT  = 0x02,
    HIDS_FEATURE = 0x03,
};

static struct hids_report keyboard_input = {
    .id   = 0x01,
    .type = HIDS_INPUT,
};

static struct hids_report led_output = {
    .id   = 0x01,
    .type = HIDS_OUTPUT,
};

static struct hids_report consumer_input = {
    .id   = 0x02,
    .type = HIDS_INPUT,
};

static struct hids_report system_ctrl_input = {
    .id   = 0x03,
    .type = HIDS_INPUT,
};

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL), BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

static uint8_t simulate_input;
static uint8_t ctrl_point;
static struct bt_conn *current_conn = NULL;

static struct k_work_delayable defer_sc_worker;
static void defer_sc_work_handler(struct k_work *work);

static const uint8_t report_map[] = {
    0x05, 0x01, /* Usage Page (Generic Desktop) */
    0x09, 0x06, /* Usage (Keyboard) */
    0xA1, 0x01, /* Collection (Application) */

    /* Keys */
    0x85, INPUT_REP_KEYS_REF_ID, 0x05, 0x07, /* Usage Page (Key Codes) */
    0x19, 0xe0,                              /* Usage Minimum (224) */
    0x29, 0xe7,                              /* Usage Maximum (231) */
    0x15, 0x00,                              /* Logical Minimum (0) */
    0x25, 0x01,                              /* Logical Maximum (1) */
    0x75, 0x01,                              /* Report Size (1) */
    0x95, 0x08,                              /* Report Count (8) */
    0x81, 0x02,                              /* Input (Data, Variable, Absolute) */

    0x95, 0x01, /* Report Count (1) */
    0x75, 0x08, /* Report Size (8) */
    0x81, 0x01, /* Input (Constant) reserved byte(1) */

    0x95, 0x06, /* Report Count (6) */
    0x75, 0x08, /* Report Size (8) */
    0x15, 0x00, /* Logical Minimum (0) */
    0x25, 0x65, /* Logical Maximum (101) */
    0x05, 0x07, /* Usage Page (Key codes) */
    0x19, 0x00, /* Usage Minimum (0) */
    0x29, 0x65, /* Usage Maximum (101) */
    0x81, 0x00, /* Input (Data, Array) Key array(6 bytes) */
    /* LED */
    0x85, OUTPUT_REP_KEYS_REF_ID, 0x95, 0x05, /* Report Count (5) */
    0x75, 0x01,                               /* Report Size (1) */
    0x05, 0x08,                               /* Usage Page (Page# for LEDs) */
    0x19, 0x01,                               /* Usage Minimum (1) */
    0x29, 0x05,                               /* Usage Maximum (5) */
    0x91, 0x02,                               /* Output (Data, Variable, Absolute), */
                                              /*       Led report */
    0x95, 0x01,                               /*       Report Count (1) */
    0x75, 0x03,                               /*       Report Size (3) */
    0x91, 0x01,                               /*       Output (Data, Variable, Absolute), */
                                              /*       Led report padding */
    0xC0,                                     /*       End Collection (Application) */

    /* Report ID 2  consumer control*/
    0x05, 0x0C,                           /*      Usage Page (Consumer Devices) */
    0x09, 0x01,                           /*      Usage (Consumer Control)   */
    0xA1, 0x01,                           /*      Collection (Application)   */
    0x85, INPUT_REP_CONSUMER_CTRL_REF_ID, /*	Report ID=3 */
    0x05, 0x0C,                           /*      Usage Page (Consumer Devices) */
    0x15, 0x00,                           /*      Logical Minimum (0) */
    0x25, 0x01,                           /*      Logical Maximum (1) */
    0x75, 0x01,                           /*      Report Size (1)     */
    0x95, 0x20,                           /*      Report Count (32)   */
    0x0A, 0x24, 0x02,                     /*	   Usage (AC Back) */
    0x0A, 0x25, 0x02,                     /*	   Usage (AC Forward) */
    0x0A, 0x26, 0x02,                     /*	   Usage (AC Stop) */
    0x0A, 0x27, 0x02,                     /*	   Usage (AC Fresh) */
    0x0A, 0x21, 0x02,                     /*	   Usage (AC Search) */
    0x0A, 0x2A, 0x02,                     /*	   Usage (AC Bookmarks) */
    0x0A, 0x23, 0x02,                     /*	   Usage (AC Home) */
    0x0A, 0x8A, 0x01,                     /*	   Usage (AL Email Reader) */

    0x09, 0xE2,       /*	   Usage (Mute) */
    0x09, 0xEA,       /*	   Usage (Volume Down) */
    0x09, 0xE9,       /*	   Usage (Volume Up) */
    0x09, 0xCD,       /*	   Usage (Play/Pause) */
    0x09, 0xB7,       /*	   Usage (Stop) */
    0x09, 0xB6,       /*	   Usage (Scan Previous Track) */
    0x09, 0xB5,       /*	   Usage (Scan Next Track) */
    0x0A, 0x83, 0x01, /*	   Usage (AL Consumer Control Configuration) */

    0x0A, 0x94, 0x01, /*	   Usage (AL Local Machine Browser) */
    0x0A, 0x92, 0x01, /*	   Usage (AL Calculator) */
    0x0A, 0x09, 0x02, /*	   Usage (AC Properties) */
    0x09, 0xB2,       /*	   Usage (Record) */
    0x09, 0xB3,       /*	   Usage (Fast Forward) */
    0x09, 0xB4,       /*	   Usage (Rewind) */
    0x09, 0x8D,       /*	   Usage (Media Select Program guide) */
    0x09, 0x04,       /*	   Usage (MicroPhone) */

    0x09, 0x30,       /*	   Usage (Power) */
    0x0A, 0x5D, 0x02, /*	   Usage (AC yes) */
    0x0A, 0x5E, 0x02, /*	   Usage (AC no) */
    0x0A, 0x5F, 0x02, /*	   Usage (AC cancel) */
    0x0A, 0xB1, 0x01, /*	   Usage (AL Screen Saver) */
    0x09, 0xB8,       /*	   Usage (Eject) */
    0x0A, 0x92, 0x01, /*	   Usage (AL Calculator) */
    0x0A, 0x92, 0x01, /*	   Usage (AL Calculator) */
    0x81, 0x62,       /*      INPUT (Data,Var,Abs,NPrf,Null)*/
    0xC0,             /* End Collection	 */

    /* Report ID 3	system control*/
    0x05, 0x01,                         /* Usage page (Generic Desktop) */
    0x09, 0x80,                         /* Usage (System Control) */
    0xA1, 0x01,                         /*  Collection (Application) */
    0x85, INPUT_REP_SYSTEM_CTRL_REF_ID, /*Report Id (3) */
    0x15, 0x00,                         /* 	Logical minimum (0) */
    0x25, 0x01,                         /* 	Logical maximum (1) */
    0x75, 0x01,                         /* 	Report Size (1) */
    0x95, 0x08,                         /* 	Report Count (8) */
    0x09, 0xA8,                         /*	Usage (System hibernate) */
    0x09, 0xA7,                         /*  Usage (System sperker mute) */
    0x09, 0xA0,                         /*	Usage (System undock) */
    0x09, 0xA1,                         /*	Usage (System dock) */
    0x09, 0xA2,                         /*  Usage (System setup) */

    0x09, 0x81, /*	Usage (System power down) */
    0x09, 0x82, /*  Usage (System sleep) */
    0x09, 0x83, /*	Usage (System wake up) */
    0x81, 0x62, /*	Input (Data,Value,Relative,Bit Field) */
    0xC0,       /*  End Collection	  */
};

static ssize_t read_info(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(struct hids_info));
}

static ssize_t read_report_map(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, report_map, sizeof(report_map));
}

static ssize_t read_report(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(struct hids_report));
}

static void input_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    simulate_input = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

static ssize_t read_input_report(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len,
                                 uint16_t offset)
{
    char str[BT_UUID_STR_LEN];
    bt_uuid_to_str(attr->uuid, str, sizeof(str));

    wm_log_debug("read intput report %s, handle 0x%04x", str, attr->handle);
    return bt_gatt_attr_read(conn, attr, buf, len, offset, NULL, 0);
}

static ssize_t write_output_led(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
                                uint16_t offset, uint8_t flags)
{
    wm_log_dump(WM_LOG_LEVEL_INFO, "LED", 16, buf, len);
    return 0;
}

static ssize_t write_ctrl_point(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
                                uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;

    wm_log_dump(WM_LOG_LEVEL_INFO, "CTRL POINT", 16, buf, len);

    if (offset + len > sizeof(ctrl_point)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    return len;
}

/* HID Service Declaration */
BT_GATT_SERVICE_DEFINE(hid_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_HIDS),
                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_INFO, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_info, NULL, &info),
                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT_MAP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_report_map,
                                              NULL, NULL),

                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ_AUTHEN, read_input_report, NULL, NULL),
                       BT_GATT_CCC(input_ccc_changed, BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN),
                       BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ, read_report, NULL, &keyboard_input),

                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ_AUTHEN, read_input_report, NULL, NULL),
                       BT_GATT_CCC(input_ccc_changed, BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN),
                       BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ, read_report, NULL, &consumer_input),

                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ_AUTHEN, read_input_report, NULL, NULL),
                       BT_GATT_CCC(input_ccc_changed, BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN),
                       BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ, read_report, NULL, &system_ctrl_input),

                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                                              BT_GATT_PERM_WRITE, NULL, write_output_led, NULL),
                       BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ, read_report, NULL, &led_output),

                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_CTRL_POINT, BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_WRITE,
                                              NULL, write_ctrl_point, &ctrl_point));

static void defer_sc_work_handler(struct k_work *work)
{
    int sec_err;
    (void)work;

    sec_err = bt_conn_set_security(current_conn, BT_SECURITY_L2);
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

    k_work_schedule(&defer_sc_worker, K_MSEC(300));
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Disconnected from %s (reason 0x%02x)", addr, reason);

    bt_conn_unref(conn);
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
    .connected        = connected,
    .disconnected     = disconnected,
    .security_changed = security_changed,
};

static void bt_ready(int err)
{
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return;
    }

    wm_log_info("Bluetooth initialized");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
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

int main(void)
{
    int err;

    err = bt_enable(bt_ready);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return err;
    }

    k_work_init_delayable(&defer_sc_worker, defer_sc_work_handler);

    bt_conn_auth_cb_register(&auth_cb_display);

    return err;
}

static void gatt_complete(struct bt_conn *conn, void *user_data)
{
}

int input_report_via_notify(uint8_t index, const uint8_t *data, uint16_t len)
{
    struct bt_gatt_notify_params params;

    if (!current_conn) {
        wm_log_warn("notification send to null connection");
        return -ENOTCONN;
    }

    memset(&params, 0, sizeof(params));
    switch (index) {
        case 1:
            params.attr = &hid_svc.attrs[5];
            break;
        case 2:
            params.attr = &hid_svc.attrs[9];
            break;
        case 3:
            params.attr = &hid_svc.attrs[13];
            break;

        default:
            wm_log_warn("invalid report index");
            return -EINVAL;
    }
    params.data = data;
    params.len  = len;
#if defined(CONFIG_BT_EATT)
    params.chan_opt = BT_ATT_CHAN_OPT_NONE;
#endif /* CONFIG_BT_EATT */
    params.func = gatt_complete;
    if (bt_gatt_is_subscribed(current_conn, params.attr, BT_GATT_CCC_NOTIFY)) {
        return bt_gatt_notify_cb(current_conn, &params);
    } else {
        wm_log_warn("notification not enabled");
        return -ENOENT;
    }
}

static void cmd_keyboard_input(int argc, char *argv[])
{
    uint8_t input;
    uint8_t report_info[8] = { 0x00 };

    if (strcmp(argv[1], "capslock") == 0) {
        report_info[2] = 57;
        input_report_via_notify(INPUT_REP_KEYS_REF_ID, report_info, 8);
        report_info[2] = 0x00;
        input_report_via_notify(INPUT_REP_KEYS_REF_ID, report_info, 8);
        return;
    }

    /**support one byte input only*/
    if (strlen(argv[1]) != 1) {
        wm_log_warn("input invalid");
        return;
    }
    input = (uint8_t)argv[1][0];

    if (input >= 'a' && input <= 'z') {
        input = input - 'a' + 4;
    } else if (input > '0' && input <= '9') {
        input = input - '0' - 1 + 30;
    } else if (input == '0') {
        input = 39;
    } else if (input >= 'A' && input <= 'Z') {
        input = input - 'A' + 4;
    } else {
        wm_log_warn("input invalid");
        return;
    }
    report_info[2] = input;
    input_report_via_notify(INPUT_REP_KEYS_REF_ID, report_info, 8);
    report_info[2] = 0x00;
    report_info[0] = 0x00;

    input_report_via_notify(INPUT_REP_KEYS_REF_ID, report_info, 8);
}

static void cmd_system_control(int argc, char *argv[])
{
    uint8_t report_info[1];

    if (strcmp(argv[1], "wakeup") == 0) {
        report_info[0] = 0x80;
    } else if (strcmp(argv[1], "sleep") == 0) {
        report_info[0] = 0x40;
    } else if (strcmp(argv[1], "powerdown") == 0) {
        report_info[0] = 0x20;
    } else if (strcmp(argv[1], "setup") == 0) {
        report_info[0] = 0x10;
    } else if (strcmp(argv[1], "dock") == 0) {
        report_info[0] = 0x08;
    } else if (strcmp(argv[1], "undock") == 0) {
        report_info[0] = 0x04;
    } else if (strcmp(argv[1], "mute") == 0) {
        report_info[0] = 0x02;
    } else if (strcmp(argv[1], "hibernate") == 0) {
        report_info[0] = 0x01;
    } else {
        wm_log_warn("invalid input");
        return;
    }

    input_report_via_notify(INPUT_REP_SYSTEM_CTRL_REF_ID, report_info, 1);

    if (report_info[0] == 0x20) {
        k_sleep(K_SECONDS(5)); // only to simulate the long press powerdown key
    }

    report_info[0] = 0x00;
    input_report_via_notify(INPUT_REP_SYSTEM_CTRL_REF_ID, report_info, 1);
}

static void cmd_consumer_control(int argc, char *argv[])
{
    uint8_t report_info[5] = { 0x00, 0x00, 0x00, 0x00 };

    if (strcmp(argv[1], "email") == 0) {
        report_info[0] = 0x80;
    } else if (strcmp(argv[1], "home") == 0) {
        report_info[0] = 0x40;
    } else if (strcmp(argv[1], "bookmarks") == 0) {
        report_info[0] = 0x20;
    } else if (strcmp(argv[1], "search") == 0) {
        report_info[0] = 0x10;
    } else if (strcmp(argv[1], "fresh") == 0) {
        report_info[0] = 0x08;
    } else if (strcmp(argv[1], "stop") == 0) {
        report_info[0] = 0x04;
    } else if (strcmp(argv[1], "forward") == 0) {
        report_info[0] = 0x02;
    } else if (strcmp(argv[1], "back") == 0) {
        report_info[0] = 0x01;
    } else if (strcmp(argv[1], "cfg") == 0) {
        report_info[1] = 0x80;
    } else if (strcmp(argv[1], "next") == 0) {
        report_info[1] = 0x40;
    } else if (strcmp(argv[1], "previous") == 0) {
        report_info[1] = 0x20;
    } else if (strcmp(argv[1], "astop") == 0) {
        report_info[1] = 0x10;
    } else if (strcmp(argv[1], "playpause") == 0) {
        report_info[1] = 0x08;
    } else if (strcmp(argv[1], "volup") == 0) {
        report_info[1] = 0x04;
    } else if (strcmp(argv[1], "voldown") == 0) {
        report_info[1] = 0x02;
    } else if (strcmp(argv[1], "volmute") == 0) {
        report_info[1] = 0x01;
    } else if (strcmp(argv[1], "micphone") == 0) {
        report_info[2] = 0x80;
    } else if (strcmp(argv[1], "mediasel") == 0) {
        report_info[2] = 0x40;
    } else if (strcmp(argv[1], "rewind") == 0) {
        report_info[2] = 0x20;
    } else if (strcmp(argv[1], "fastforward") == 0) {
        report_info[2] = 0x10;
    } else if (strcmp(argv[1], "record") == 0) {
        report_info[2] = 0x08;
    } else if (strcmp(argv[1], "properties") == 0) {
        report_info[2] = 0x04;
    } else if (strcmp(argv[1], "calculator") == 0) {
        report_info[2] = 0x02;
    } else if (strcmp(argv[1], "browser") == 0) {
        report_info[2] = 0x01;
    } else if (strcmp(argv[1], "calculator") == 0) {
        report_info[3] = 0x80;
    } else if (strcmp(argv[1], "calculator") == 0) {
        report_info[3] = 0x40;
    } else if (strcmp(argv[1], "eject") == 0) {
        report_info[3] = 0x20;
    } else if (strcmp(argv[1], "screensaver") == 0) {
        report_info[3] = 0x10;
    } else if (strcmp(argv[1], "cancel") == 0) {
        report_info[3] = 0x08;
    } else if (strcmp(argv[1], "no") == 0) {
        report_info[3] = 0x04;
    } else if (strcmp(argv[1], "yes") == 0) {
        report_info[3] = 0x02;
    } else if (strcmp(argv[1], "power") == 0) {
        report_info[3] = 0x01;
    }

    input_report_via_notify(INPUT_REP_CONSUMER_CTRL_REF_ID, report_info, 4);
    report_info[0] = 0x00;
    report_info[1] = 0x00;
    report_info[2] = 0x00;
    report_info[3] = 0x00;

    input_report_via_notify(INPUT_REP_CONSUMER_CTRL_REF_ID, report_info, 4);
}

WM_CLI_CMD_DEFINE(cc, cmd_consumer_control, select consumer control, consumer control cmd list-- cc cmd);
WM_CLI_CMD_DEFINE(sc, cmd_system_control, select system control, system control cmd list-- sc cmd);
WM_CLI_CMD_DEFINE(in, cmd_keyboard_input, keyboard input, keyboard input cmd-- in cmd);
