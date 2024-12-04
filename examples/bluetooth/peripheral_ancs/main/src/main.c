/* main.c - Application main entry point */

/*
 * Copyright (c) 2023 WinnerMicro Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <settings/settings.h>

#include "ancs_client.h"
#include "wmsdk_config.h"
#include "wm_cli.h"

#define LOG_TAG "ancs"
#include "wm_log.h"

/** Length of the Service Changed characteristic value. */
#define BT_GATTP_SERVICE_CHANGED_VALUE_LENGTH 4

enum {
    DISCOVERY_ANCS_SC_ONGOING,
    DISCOVERY_ANCS_SC_SUCCEEDED,
    DISCOVERY_ANCS_DS_ONGOING,
    DISCOVERY_ANCS_DS_SUCCEEDED,
    DISCOVERY_ANCS_NS_ONGOING,
    DISCOVERY_ANCS_NS_SUCCEEDED,
    DISCOVERY_ANCS_CP_ONGOING,
    DISCOVERY_ANCS_CP_SUCCEEDED,
    SERVICE_CHANGED_INDICATED
};

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_LIMITED | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_SOLICIT128, BT_UUID_ANCS_VAL),
};

static struct bt_ancs_client ancs_c;
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params_sc, subscribe_params_ds, subscribe_params_ns;
static atomic_t discovery_flags;

/* Local copy to keep track of the newest arriving notifications. */
static struct bt_ancs_evt_notif *notification_current = NULL;
static struct bt_ancs_evt_notif *notification_latest  = NULL;
static struct bt_ancs_evt_notif *notification_it      = NULL;

/* Local copy of the newest notification attribute. */
static struct bt_ancs_attr notif_attr_latest;
/* Local copy of the newest app attribute. */
static struct bt_ancs_attr notif_attr_app_id_latest;

static struct bt_uuid_16 uuid            = BT_UUID_INIT_16(0);
static struct bt_uuid_128 uuid_ancs      = BT_UUID_INIT_128(0);
static struct bt_uuid_128 uuid_ancs_char = BT_UUID_INIT_128(0);

static const struct bt_uuid *ccc_uuid = BT_UUID_GATT_CCC;

static struct k_work_delayable defer_sc_worker;

static void defer_work_handler(struct k_work *work);

static void defer_sc_work_handler(struct k_work *work);

/* String literals for the iOS notification categories.
 * Used then printing to UART.
 */
static const char *lit_catid[BT_ANCS_CATEGORY_ID_COUNT] = {
    "Other", "Incoming Call", "Missed Call",        "Voice Mail",           "Social",   "Schedule",
    "Email", "News",          "Health And Fitness", "Business And Finance", "Location", "Entertainment"
};

/* String literals for the iOS notification event types.
 * Used then printing to UART.
 */
static const char *lit_eventid[BT_ANCS_EVT_ID_COUNT] = { "Added", "Modified", "Removed" };

/* String literals for the iOS notification attribute types.
 * Used when printing to UART.
 */
static const char *lit_attrid[BT_ANCS_NOTIF_ATTR_COUNT] = {
    "App Identifier", "Title", "Subtitle", "Message", "Message Size", "Date", "Positive Action Label", "Negative Action Label"
};

/* String literals for the iOS notification attribute types.
 * Used When printing to UART.
 */
static const char *lit_appid[BT_ANCS_APP_ATTR_COUNT] = { "Display Name" };

/**@brief Function for printing an iOS notification.
 *
 * @param[in] notif  Pointer to the iOS notification.
 */
static void notif_print(const struct bt_ancs_evt_notif *notif)
{
    wm_log_info("------Notification-------");
    wm_log_info("Event:       %s", lit_eventid[notif->evt_id]);
    wm_log_info("Category ID: %s", lit_catid[notif->category_id]);
    wm_log_info("Category Cnt:%u", (unsigned int)notif->category_count);
    wm_log_info("UID:         %u", (unsigned int)notif->notif_uid);

    if (notif->evt_flags.silent) {
        wm_log_info("Flags: Silent");
    }
    if (notif->evt_flags.important) {
        wm_log_info("Flags: Important");
    }
    if (notif->evt_flags.pre_existing) {
        wm_log_info("Flags: Pre-existing");
    }
    if (notif->evt_flags.positive_action) {
        wm_log_info("Flags: Positive Action");
    }
    if (notif->evt_flags.negative_action) {
        wm_log_info("Flags: Negative Action");
    }
}

/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] attr Pointer to an iOS notification attribute.
 */
static void notif_attr_print(const struct bt_ancs_attr *attr)
{
    if (attr->attr_len != 0) {
        wm_log_info("%s: %s", lit_attrid[attr->attr_id], (char *)attr->attr_data);
    } else if (attr->attr_len == 0) {
        wm_log_info("%s: (N/A)", lit_attrid[attr->attr_id]);
    }
}

/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] attr Pointer to an iOS App attribute.
 */
static void app_attr_print(const struct bt_ancs_attr *attr)
{
    if (attr->attr_len != 0) {
        wm_log_info("%s: %s", lit_appid[attr->attr_id], (char *)attr->attr_data);
    } else if (attr->attr_len == 0) {
        wm_log_info("%s: (N/A)", lit_appid[attr->attr_id]);
    }
}

/**@brief Function for printing out errors that originated from the Notification Provider (iOS).
 *
 * @param[in] err_code_np Error code received from NP.
 */
static void err_code_print(uint8_t err_code_np)
{
    switch (err_code_np) {
        case BT_ATT_ERR_ANCS_NP_UNKNOWN_COMMAND:
            wm_log_info("Error: Command ID was not recognized by the Notification Provider.");
            break;

        case BT_ATT_ERR_ANCS_NP_INVALID_COMMAND:
            wm_log_info("Error: Command failed to be parsed on the Notification Provider.");
            break;

        case BT_ATT_ERR_ANCS_NP_INVALID_PARAMETER:
            wm_log_info("Error: Parameter does not refer to an existing object on the Notification Provider.");
            break;

        case BT_ATT_ERR_ANCS_NP_ACTION_FAILED:
            wm_log_info("Error: Perform Notification Action Failed on the Notification Provider.");
            break;

        default:
            break;
    }
}

static void bt_ancs_notification_source_handler(struct bt_ancs_client *ancs_c, int err, const struct bt_ancs_evt_notif *notif)
{
    struct bt_ancs_evt_notif *it  = NULL;
    struct bt_ancs_evt_notif *itt = NULL;
    if (!err) {
        notif_print(notif);
        if (notif->evt_id == BT_ANCS_EVENT_ID_NOTIFICATION_ADDED) { //Add;
            it = (struct bt_ancs_evt_notif *)k_malloc(sizeof(struct bt_ancs_evt_notif));
            if (it == NULL) {
                return;
            }
            memcpy(it, notif, sizeof(struct bt_ancs_evt_notif));
            it->next             = notification_latest;
            notification_latest  = it;
            notification_current = it;
            wm_log_debug("Add UID %d notify %p", notif->notif_uid, notification_latest);
        } else if (notif->evt_id == BT_ANCS_EVENT_ID_NOTIFICATION_REMOVED) {
            it = notification_latest;

            /*process header*/
            if (it->notif_uid == notif->notif_uid) {
                notification_latest  = it->next;
                notification_current = it->next;
                k_free(it);
                wm_log_info("remove latest notification");
                return;
            }
            /*process left item*/
            while (it->next) {
                if (it->next->notif_uid == notif->notif_uid) {
                    itt      = it->next;
                    it->next = itt->next;
                    /*reassign current*/
                    if (itt->notif_uid == notification_current->notif_uid) {
                        notification_current = itt->next;
                    }
                    k_free(itt);
                    wm_log_info("remove middle notification");
                    return;
                }
                it = it->next;
            }

            wm_log_warn("impossible run here\n");
        }
    }
}

static void bt_ancs_data_source_handler(struct bt_ancs_client *ancs_c, const struct bt_ancs_attr_response *response)
{
    switch (response->command_id) {
        case BT_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES:
            notif_attr_latest = response->attr;
            notif_attr_print(&notif_attr_latest);
            if (response->attr.attr_id == BT_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER) {
                notif_attr_app_id_latest = response->attr;
            }
            break;

        case BT_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES:
            app_attr_print(&response->attr);
            break;

        default:
            /* No implementation needed. */
            break;
    }
}

static void bt_ancs_write_response_handler(struct bt_ancs_client *ancs_c, uint8_t err)
{
    err_code_print(err);
}

static uint8_t indicate_sc_func(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data,
                                uint16_t length)
{
    int err;

    err = (length == BT_GATTP_SERVICE_CHANGED_VALUE_LENGTH) ? 0 : -EINVAL;

    if (err) {
        wm_log_error("servcie change cback invalid param");
    } else {
        atomic_set_bit(&discovery_flags, SERVICE_CHANGED_INDICATED);
    }

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t discover_for_sc_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
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

    wm_log_debug("[ATTRIBUTE] uuid %s, handle %u", str, attr->handle);

    if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_GATT)) {
        memcpy(&uuid, BT_UUID_GATT_SC, sizeof(uuid));
        discover_params.uuid         = &uuid.uuid;
        discover_params.start_handle = attr->handle + 1;
        discover_params.type         = BT_GATT_DISCOVER_CHARACTERISTIC;

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_SC_ONGOING);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }
    } else if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_GATT_SC)) {
        discover_params.uuid             = ccc_uuid;
        discover_params.start_handle     = attr->handle + 2;
        discover_params.type             = BT_GATT_DISCOVER_DESCRIPTOR;
        subscribe_params_sc.value_handle = bt_gatt_attr_value_handle(attr);

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_SC_ONGOING);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }
    } else {
        subscribe_params_sc.notify     = indicate_sc_func;
        subscribe_params_sc.value      = BT_GATT_CCC_INDICATE;
        subscribe_params_sc.ccc_handle = attr->handle;

        err = bt_gatt_subscribe(conn, &subscribe_params_sc);
        if (err && err != -EALREADY) {
            wm_log_error("Subscribe failed (err %d)", err);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_SC_ONGOING);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));

        } else {
            wm_log_info("[SUBSCRIBED] SC");
            atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_SC_ONGOING);
            atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_SC_SUCCEEDED);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }

        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_STOP;
}

static int discovery_sc_service(struct bt_conn *conn)
{
    int err;

    memcpy(&uuid, BT_UUID_GATT, sizeof(uuid));
    discover_params.uuid         = &uuid.uuid;
    discover_params.func         = discover_for_sc_func;
    discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
    discover_params.end_handle   = BT_ATT_LAST_ATTRIBUTE_HANDLE;
    discover_params.type         = BT_GATT_DISCOVER_PRIMARY;

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
        wm_log_error("Discover subscribe failed(err %d)", err);
    } else {
        atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_SC_ONGOING);
    }

    return err;
}

static uint8_t notify_ns_func(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data, uint16_t length)
{
    bt_ancs_parse_notif(&ancs_c, data, length);

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t notify_ds_func(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data, uint16_t length)
{
    bt_ancs_parse_get_attrs_response(&ancs_c, data, length);

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t discover_for_ancs_notify_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                             struct bt_gatt_discover_params *params)
{
    int err = 0;
    char str[BT_UUID_STR_LEN];

    if (!attr) {
        wm_log_info("Discover complete");
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }
    bt_uuid_to_str(attr->uuid, str, sizeof(str));

    wm_log_info("[ATTRIBUTE] uuid %s, handle %u", str, attr->handle);

    if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_ANCS)) {
        discover_params.uuid         = &uuid_ancs_char.uuid;
        discover_params.start_handle = attr->handle + 1;
        discover_params.type         = BT_GATT_DISCOVER_CHARACTERISTIC;

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }
    } else if (!bt_uuid_cmp(discover_params.uuid, &uuid_ancs_char.uuid)) {
        discover_params.uuid         = ccc_uuid;
        discover_params.start_handle = attr->handle + 2;
        discover_params.type         = BT_GATT_DISCOVER_DESCRIPTOR;
        if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING)) {
            subscribe_params_ns.value_handle = bt_gatt_attr_value_handle(attr);
        } else if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING)) {
            subscribe_params_ds.value_handle = bt_gatt_attr_value_handle(attr);
        } else {
            wm_log_warn("impossible run here");
        }

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }
    } else {
        if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING)) {
            subscribe_params_ns.notify     = notify_ns_func;
            subscribe_params_ns.value      = BT_GATT_CCC_NOTIFY;
            subscribe_params_ns.ccc_handle = attr->handle;
            err                            = bt_gatt_subscribe(conn, &subscribe_params_ns);

        } else if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING)) {
            subscribe_params_ds.notify     = notify_ds_func;
            subscribe_params_ds.value      = BT_GATT_CCC_NOTIFY;
            subscribe_params_ds.ccc_handle = attr->handle;
            err                            = bt_gatt_subscribe(conn, &subscribe_params_ds);
        } else {
            wm_log_warn("impossible run here");
        }

        if (err && err != -EALREADY) {
            wm_log_error("Subscribe failed (err %d)", err);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));

        } else {
            if (atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING)) {
                atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_NS_SUCCEEDED);
                wm_log_info("[SUBSCRIBED] NS");
            } else if (atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING)) {
                atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_DS_SUCCEEDED);
                wm_log_info("[SUBSCRIBED] DS");
            }
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }

        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_STOP;
}

static int discovery_ancs_notify_char(struct bt_conn *conn, bool source)
{
    int err;

    memcpy(&uuid_ancs, BT_UUID_ANCS, sizeof(uuid_ancs));
    discover_params.uuid         = &uuid_ancs.uuid;
    discover_params.func         = discover_for_ancs_notify_func;
    discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
    discover_params.end_handle   = BT_ATT_LAST_ATTRIBUTE_HANDLE;
    discover_params.type         = BT_GATT_DISCOVER_PRIMARY;

    /*PRE set discovery parameter of next step*/
    if (source) {
        memcpy(&uuid_ancs_char, BT_UUID_ANCS_NOTIFICATION_SOURCE, sizeof(uuid_ancs_char));
        atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING);
    } else {
        memcpy(&uuid_ancs_char, BT_UUID_ANCS_DATA_SOURCE, sizeof(uuid_ancs_char));
        atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING);
    }

    err = bt_gatt_discover(conn, &discover_params);

    if (err) {
        if (source) {
            atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING);
        } else {
            atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING);
        }
        wm_log_error("Discover subscribe failed(err %d)", err);
    }

    return err;
}

static uint8_t discover_for_ancs_cp_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
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

    if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_ANCS)) {
        discover_params.uuid         = &uuid_ancs_char.uuid;
        discover_params.start_handle = attr->handle + 1;
        discover_params.type         = BT_GATT_DISCOVER_CHARACTERISTIC;

        err = bt_gatt_discover(conn, &discover_params);
        if (err) {
            wm_log_error("Discover failed (err %d)", err);
            atomic_test_and_clear_bit(&discovery_flags, DISCOVERY_ANCS_CP_ONGOING);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }
    } else if (!bt_uuid_cmp(discover_params.uuid, &uuid_ancs_char.uuid)) {
        struct bt_gatt_chrc *gatt_chrc = attr->user_data;
        ancs_c.handle_cp               = gatt_chrc->value_handle;
        atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_CP_ONGOING);
        atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_CP_SUCCEEDED);
        wm_log_info("got cp handle %d", ancs_c.handle_cp);
        k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_STOP;
}

static int discovery_ancs_cp_char(struct bt_conn *conn)
{
    int err;

    memcpy(&uuid_ancs, BT_UUID_ANCS, sizeof(uuid_ancs));
    discover_params.uuid         = &uuid_ancs.uuid;
    discover_params.func         = discover_for_ancs_cp_func;
    discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
    discover_params.end_handle   = BT_ATT_LAST_ATTRIBUTE_HANDLE;
    discover_params.type         = BT_GATT_DISCOVER_PRIMARY;

    /*PRE set discovery parameter of next step*/
    memcpy(&uuid_ancs_char, BT_UUID_ANCS_CONTROL_POINT, sizeof(uuid_ancs_char));

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
        wm_log_error("Discover subscribe failed(err %d)", err);
    } else {
        atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_CP_ONGOING);
    }

    return err;
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Pairing completed: %s, bonded: %d", addr, bonded);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    wm_log_info("Pairing failed conn: %s, reason %d", addr, reason);

    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

void bond_deleted(uint8_t id, const bt_addr_le_t *peer)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(peer, addr, sizeof(addr));
    wm_log_info("bonded device deleted %s, ID %u", addr, id);
}

static struct bt_conn_auth_info_cb auth_info_cb = {
    .pairing_failed   = pairing_failed,
    .pairing_complete = pairing_complete,
    .bond_deleted     = bond_deleted,
};

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
#if defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .pairing_accept = pairing_accept,
#endif

};

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
    wm_log_info("Updated MTU: TX: %d RX: %d bytes", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = { .att_mtu_updated = mtu_updated };

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (err) {
        wm_log_error("Connection failed (err 0x%02x)", err);
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    wm_log_info("Connected %s", addr);

    ancs_c.conn = bt_conn_ref(conn);
    k_work_schedule(&defer_sc_worker, K_MSEC(300));
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    wm_log_info("Disconnected from %s (reason 0x%02x)", addr, reason);
    if (conn == ancs_c.conn) {
        bt_conn_unref(ancs_c.conn);
        ancs_c.conn          = NULL;
        notification_latest  = NULL;
        notification_current = NULL;
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

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        wm_log_info("Security changed: %s level %u", addr, level);

        if (bt_conn_get_security(conn) >= BT_SECURITY_L2) {
            discovery_flags = ATOMIC_INIT(0);
            k_work_schedule(&ancs_c.defer_worker, K_MSEC(300));
        }
    } else {
        wm_log_error("Security failed: %s level %u err %d", addr, level, err);
    }
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

int action_notify(bool direction, struct bt_ancs_evt_notif *it)
{
    int err = 0;

    if (it == NULL) {
        wm_log_warn("bt_ancs_evt_notif is null.");
        return err;
    }

    if (direction) {
        if (it->evt_flags.positive_action) {
            wm_log_info("Performing Positive Action.");
            err =
                bt_ancs_notification_action(&ancs_c, it->notif_uid, BT_ANCS_ACTION_ID_POSITIVE, bt_ancs_write_response_handler);
            if (err) {
                wm_log_error("Failed performing action (err: %d)", err);
            }
        }

    } else {
        if (it->evt_flags.negative_action) {
            wm_log_info("Performing Negative Action.");
            err =
                bt_ancs_notification_action(&ancs_c, it->notif_uid, BT_ANCS_ACTION_ID_NEGATIVE, bt_ancs_write_response_handler);
            if (err) {
                wm_log_error("Failed performing action (err: %d)", err);
            }
        }
    }

    return err;
}

int request_notify_attr(struct bt_ancs_evt_notif *it)
{
    int err = 0;

    err = bt_ancs_request_attrs(&ancs_c, it, bt_ancs_write_response_handler);
    if (err) {
        wm_log_error("Failed requesting attributes for a notification (err: %d)", err);
    }

    return err;
}

int request_app_attr(void)
{
    int err = 0;

    if (notif_attr_app_id_latest.attr_id == BT_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER && notif_attr_app_id_latest.attr_len != 0) {
        wm_log_info("Request for %s:", notif_attr_app_id_latest.attr_data);
        err = bt_ancs_app_attr_request(&ancs_c, notif_attr_app_id_latest.attr_data, notif_attr_app_id_latest.attr_len,
                                       bt_ancs_write_response_handler);
        if (err) {
            wm_log_error("Failed requesting attributes for a given app (err: %d)", err);
        }
    }

    return err;
}

static void cmd_read_notify(int argc, char *argv[])
{
    notification_current = notification_latest;

    notification_it = notification_current;

    if (notification_current) {
        request_notify_attr(notification_current);
    } else {
        wm_log_warn("notification_latest is null");
    }
}
static void cmd_read_next_notify(int argc, char *argv[])
{
    if (notification_current == NULL) {
        notification_current = notification_latest;
    }

    if (notification_current == NULL) {
        wm_log_warn("notification_current is null");
        return;
    }

    notification_it = notification_current;

    request_notify_attr(notification_current);

    notification_current = notification_current->next;
}

static void cmd_read_app(int argc, char *argv[])
{
    request_app_attr();
}

static void cmd_action_n(int argc, char *argv[])
{
    action_notify(false, notification_it);
}
static void cmd_action_p(int argc, char *argv[])
{
    action_notify(true, notification_it);
}

static void defer_work_handler(struct k_work *work)
{
    (void)work;

    /* 1 Service Discovery at a time */
    if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING)) {
        wm_log_warn("discovery DS ongoing");
        return;
    }

    if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING)) {
        wm_log_warn("discovery NS ongoing");
        return;
    }

    if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_SC_ONGOING)) {
        wm_log_warn("discovery SC ongoing");
        return;
    }

    if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_CP_ONGOING)) {
        wm_log_warn("discovery CP ongoing");
        return;
    }

    if (!atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_SC_SUCCEEDED) &&
        !atomic_test_and_set_bit(&discovery_flags, DISCOVERY_ANCS_SC_ONGOING)) {
        discovery_sc_service(ancs_c.conn);
        return;
    }

    if (!atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_CP_SUCCEEDED) &&
        !atomic_test_and_set_bit(&discovery_flags, DISCOVERY_ANCS_CP_ONGOING)) {
        discovery_ancs_cp_char(ancs_c.conn);
        return;
    }

    if (!atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_DS_SUCCEEDED) &&
        !atomic_test_and_set_bit(&discovery_flags, DISCOVERY_ANCS_DS_ONGOING)) {
        discovery_ancs_notify_char(ancs_c.conn, false);
        return;
    }

    if (!atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_NS_SUCCEEDED) &&
        !atomic_test_and_set_bit(&discovery_flags, DISCOVERY_ANCS_NS_ONGOING)) {
        discovery_ancs_notify_char(ancs_c.conn, true);
        return;
    }
}

static void defer_sc_work_handler(struct k_work *work)
{
    int sec_err;
    (void)work;

    sec_err = bt_conn_set_security(ancs_c.conn, BT_SECURITY_L4);
    if (sec_err) {
        wm_log_error("Failed to set security (err %d)", sec_err);
    }
}

int main(void)
{
    int err;

    wm_log_info("Starting Apple Notification Center Service client");

    err = bt_ancs_client_init(&ancs_c);
    if (err) {
        return err;
    }

    ancs_c.ns_notif_cb = bt_ancs_notification_source_handler;
    ancs_c.ds_notif_cb = bt_ancs_data_source_handler;
    k_work_init_delayable(&ancs_c.defer_worker, defer_work_handler);
    k_work_init_delayable(&defer_sc_worker, defer_sc_work_handler);

    err = bt_enable(NULL);
    if (err) {
        wm_log_info("BLE init failed (err %d)", err);
        return -1;
    }

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    bt_conn_cb_register(&conn_callbacks);

    bt_gatt_cb_register(&gatt_callbacks);

#if defined(CONFIG_BT_SMP)

    bt_conn_auth_cb_register(&auth_cb_display);

    err = bt_conn_auth_info_cb_register(&auth_info_cb);
    if (err) {
        wm_log_info("Failed to register authorization callbacks");
        return -1;
    }

#endif /* CONFIG_BT_SMP */

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME_AD, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        wm_log_info("Advertising failed to start (err %d)", err);
        return err;
    }

    wm_log_info("Advertising successfully started");

    for (;;) {
        k_sleep(K_MSEC(1000));
    }

    return 0;
}

/**@brief cli Function for reading out latest Notification information.
 *
 */
WM_CLI_CMD_DEFINE(read_notify, cmd_read_notify, read latest notification attribute, read notification-- read notify cmd);

/**@brief cli Function for reading out next Notification information if exists.
 *
 */

WM_CLI_CMD_DEFINE(read_notify_next, cmd_read_next_notify, read next notification attribute,
                  read next notification-- read next notify cmd);

/**@brief cli Function for requesting attributes for a given app.
 *
 */
WM_CLI_CMD_DEFINE(read_appl, cmd_read_app, read current application attribute, read application-- read application cmd);

/**@brief cli Function for performing a notification negative action.
 *
 */
WM_CLI_CMD_DEFINE(action_negative, cmd_action_n, perform action negative, action negative-- action_negative cmd);

/**@brief cli Function for performing a notification positive action.
 *
 */
WM_CLI_CMD_DEFINE(action_positive, cmd_action_p, perfrom action positive, action positive-- action_positive cmd);
