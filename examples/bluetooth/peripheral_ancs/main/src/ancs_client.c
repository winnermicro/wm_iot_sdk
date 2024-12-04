/*
 * Copyright (c) 2018 WinnerMicro Semiconductor 
 *
 */

#include <sys/byteorder.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ancs_client.h"

#define LOG_TAG "ancs"
#include "wm_log.h"

enum {
    ANCS_NS_NOTIF_ENABLED,
    ANCS_DS_NOTIF_ENABLED,
    ANCS_CP_WRITE_PENDING
};

/**< Index of the Event ID field when parsing notifications. */
#define BT_ANCS_NOTIF_EVT_ID_INDEX       0
/**< Index of the Flags field when parsing notifications. */
#define BT_ANCS_NOTIF_FLAGS_INDEX        1
/**< Index of the Category ID field when parsing notifications. */
#define BT_ANCS_NOTIF_CATEGORY_ID_INDEX  2
/**< Index of the Category Count field when parsing notifications. */
#define BT_ANCS_NOTIF_CATEGORY_CNT_INDEX 3
/**< Index of the Notification UID field when parsing notifications. */
#define BT_ANCS_NOTIF_NOTIF_UID          4

/* Allocated size for attribute data. */
#define ATTR_DATA_SIZE                   BT_ANCS_ATTR_DATA_MAX

/* Buffers to store attribute data. */
static uint8_t *attr_appid;
static uint8_t *attr_title;
static uint8_t *attr_subtitle;
static uint8_t *attr_message;
static uint8_t *attr_message_size;
static uint8_t *attr_date;
static uint8_t *attr_posaction;
static uint8_t *attr_negaction;
static uint8_t *attr_disp_name;

/**@brief Enumeration for keeping track of the state-based encoding while requesting
 *        app attributes.
 */
enum encode_app_attr {
    /** Currently encoding the command ID. */
    APP_ATTR_COMMAND_ID,
    /** Currently encoding the app ID. */
    APP_ATTR_APP_ID,
    /** Currently encoding the attribute ID. */
    APP_ATTR_ATTR_ID,
    /** Encoding done. */
    APP_ATTR_DONE,
    /** Encoding aborted. */
    APP_ATTR_ABORT
};

/**@brief Function for determining whether an attribute is requested.
 *
 * @param[in] ancs_c iOS notification structure. This structure must be supplied by
 *                   the application. It identifies the particular client instance to use.
 *
 * @return True  If it is requested
 * @return False If it is not requested.
 */
static bool app_attr_is_requested(struct bt_ancs_client *ancs_c, uint32_t attr_id)
{
    return ancs_c->ancs_app_attr_list[attr_id].get;
}

/**@brief Function for encoding the command ID as part of assembling a "get app attributes" command.
 *
 * @param[in,out] buf        Pointer to the BLE GATT write buffer structure.
 */
static enum encode_app_attr app_attr_encode_cmd_id(struct net_buf_simple *buf)
{
    wm_log_debug("Encoding command ID.");

    if (net_buf_simple_tailroom(buf) < 1) {
        return APP_ATTR_ABORT;
    }

    /* Encode Command ID. */
    net_buf_simple_add_u8(buf, BT_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES);

    return APP_ATTR_APP_ID;
}

/**@brief Function for encoding the app ID as part of assembling a "get app attributes" command.
 *
 * @param[in,out] buf        Pointer to the BLE GATT write buffer structure.
 * @param[in]     app_id     The app ID of the app for which to request app attributes.
 * @param[in]     app_id_len Length of the app ID.
 */
static enum encode_app_attr app_attr_encode_app_id(struct net_buf_simple *buf, const uint8_t *app_id, const uint32_t app_id_len)
{
    wm_log_debug("Encoding app ID.");

    if (net_buf_simple_tailroom(buf) < app_id_len + 1) {
        return APP_ATTR_ABORT;
    }

    /* Encode app identifier. */
    net_buf_simple_add_mem(buf, app_id, app_id_len);
    net_buf_simple_add_u8(buf, '\0');

    return APP_ATTR_ATTR_ID;
}

/**@brief Function for encoding the attribute ID as part of assembling a
 *        "get app attributes" command.
 *
 * @param[in]     ancs_c     iOS notification structure. This structure must be supplied by
 *                           the application. It identifies the particular client instance to use.
 * @param[in,out] buf        Pointer to the BLE GATT write buffer structure.
 */
static enum encode_app_attr app_attr_encode_attr_id(struct bt_ancs_client *ancs_c, struct net_buf_simple *buf)
{
    uint32_t i;

    wm_log_debug("Encoding attribute ID.");

    ancs_c->number_of_requested_attr = 0;

    /* Encode Attribute ID. */
    for (i = 0; i < BT_ANCS_APP_ATTR_COUNT; i++) {
        if (app_attr_is_requested(ancs_c, i)) {
            if (net_buf_simple_tailroom(buf) < 1) {
                return APP_ATTR_ABORT;
            }
            net_buf_simple_add_u8(buf, i);
            ancs_c->number_of_requested_attr++;
        }
    }

    return APP_ATTR_DONE;
}

/**@brief Function for sending a "get app attributes" request.
 *
 * @details Since the app ID may not fit in a single write, long write
 *          with a state machine is used to encode the "get app attributes" request.
 *
 * @param[in] ancs_c     iOS notification structure. This structure must be supplied by
 *                       the application. It identifies the particular client instance to use.
 * @param[in] app_id     The app ID of the app for which to request app attributes.
 * @param[in] app_id_len Length of the app ID.
 */
static int app_attr_get(struct bt_ancs_client *ancs_c, const uint8_t *app_id, uint32_t app_id_len, bt_ancs_write_cb func)
{
    enum encode_app_attr state = APP_ATTR_COMMAND_ID;
    int err;

    if (atomic_test_and_set_bit(&ancs_c->state, ANCS_CP_WRITE_PENDING)) {
        return -EBUSY;
    }

    struct net_buf_simple buf;

    net_buf_simple_init_with_data(&buf, ancs_c->cp_data, sizeof(ancs_c->cp_data));
    net_buf_simple_reset(&buf);

    while (state != APP_ATTR_DONE && state != APP_ATTR_ABORT) {
        switch (state) {
            case APP_ATTR_COMMAND_ID:
                state = app_attr_encode_cmd_id(&buf);
                break;
            case APP_ATTR_APP_ID:
                state = app_attr_encode_app_id(&buf, app_id, app_id_len);
                break;
            case APP_ATTR_ATTR_ID:
                state = app_attr_encode_attr_id(ancs_c, &buf);
                break;
            case APP_ATTR_DONE:
                break;
            case APP_ATTR_ABORT:
                break;
            default:
                break;
        }
    }

    if (state == APP_ATTR_DONE) {
        err = bt_ancs_cp_write(ancs_c, buf.len, func);

        ancs_c->parse_info.expected_number_of_attrs = ancs_c->number_of_requested_attr;
    } else {
        err = -ENOMEM;
    }

    return err;
}

int bt_ancs_app_attr_request(struct bt_ancs_client *ancs_c, const uint8_t *app_id, uint32_t len, bt_ancs_write_cb func)
{
    if (!len) {
        return -EINVAL;
    }

    ancs_c->parse_info.parse_state = BT_ANCS_PARSE_STATE_COMMAND_ID;

    return app_attr_get(ancs_c, app_id, len, func);
}

static bool all_req_attrs_parsed(struct bt_ancs_client *ancs_c)
{
    return !ancs_c->parse_info.expected_number_of_attrs;
}

static bool attr_is_requested(struct bt_ancs_client *ancs_c, struct bt_ancs_attr attr)
{
    return ancs_c->parse_info.attr_list[attr.attr_id].get;
}

/**@brief Function for parsing command id and notification id.
 *        Used in the @ref parse_get_notif_attrs_response state machine.
 *
 * @details UID and command ID will be received only once at the beginning of the first
 *          GATT notification of a new attribute request for a given iOS notification.
 *
 * @param[in] ancs_c   Pointer to an ANCS instance to which the event belongs.
 * @param[in] data_src Pointer to data that was received from the Notification Provider.
 * @param[in] index    Pointer to an index that helps us keep track of the current data
 *                     to be parsed.
 *
 * @return The next parse state.
 */
static enum bt_ancs_parse_state command_id_parse(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    enum bt_ancs_parse_state parse_state;

    ancs_c->parse_info.command_id = (enum bt_ancs_cmd_id_val)data_src[(*index)++];

    switch (ancs_c->parse_info.command_id) {
        case BT_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES:
            ancs_c->attr_response.command_id = BT_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES;
            ancs_c->parse_info.attr_list     = ancs_c->ancs_notif_attr_list;
            ancs_c->parse_info.attr_count    = BT_ANCS_NOTIF_ATTR_COUNT;
            parse_state                      = BT_ANCS_PARSE_STATE_NOTIF_UID;
            break;

        case BT_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES:
            ancs_c->attr_response.command_id        = BT_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES;
            ancs_c->parse_info.attr_list            = ancs_c->ancs_app_attr_list;
            ancs_c->parse_info.attr_count           = BT_ANCS_APP_ATTR_COUNT;
            ancs_c->parse_info.current_app_id_index = 0;
            parse_state                             = BT_ANCS_PARSE_STATE_APP_ID;
            break;

        default:
            /* no valid command_id, abort the rest of the parsing procedure. */
            wm_log_debug("Invalid Command ID");
            parse_state = BT_ANCS_PARSE_STATE_DONE;
            break;
    }
    return parse_state;
}

static enum bt_ancs_parse_state notif_uid_parse(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    ancs_c->attr_response.notif_uid = sys_get_le32(&data_src[*index]);
    *index += sizeof(uint32_t);
    return BT_ANCS_PARSE_STATE_ATTR_ID;
}

static enum bt_ancs_parse_state app_id_parse(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    if (ancs_c->parse_info.current_app_id_index >= sizeof(ancs_c->attr_response.app_id)) {
        wm_log_warn("App ID cannot be stored in response buffer.");
        return BT_ANCS_PARSE_STATE_DONE;
    }

    ancs_c->attr_response.app_id[ancs_c->parse_info.current_app_id_index] = data_src[(*index)++];

    if (ancs_c->attr_response.app_id[ancs_c->parse_info.current_app_id_index] != '\0') {
        ancs_c->parse_info.current_app_id_index++;
        return BT_ANCS_PARSE_STATE_APP_ID;
    } else {
        return BT_ANCS_PARSE_STATE_ATTR_ID;
    }
}

/**@brief Function for parsing the id of an iOS attribute.
 *        Used in the @ref parse_get_notif_attrs_response state machine.
 *
 * @details We only request attributes that are registered with @ref bt_ancs_attr_add
 *          once they have been reveiced we stop parsing.
 *
 * @param[in] ancs_c   Pointer to an ANCS instance to which the event belongs.
 * @param[in] data_src Pointer to data that was received from the Notification Provider.
 * @param[in] index    Pointer to an index that helps us keep track of the current data
 *                     to be parsed.
 *
 * @return The next parse state.
 */
static enum bt_ancs_parse_state attr_id_parse(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    ancs_c->attr_response.attr.attr_id = data_src[(*index)++];

    if (ancs_c->attr_response.attr.attr_id >= ancs_c->parse_info.attr_count) {
        wm_log_debug("Attribute ID Invalid.");
        return BT_ANCS_PARSE_STATE_DONE;
    }
    ancs_c->attr_response.attr.attr_data = ancs_c->parse_info.attr_list[ancs_c->attr_response.attr.attr_id].attr_data;

    if (all_req_attrs_parsed(ancs_c)) {
        wm_log_debug("All requested attributes received. ");
        return BT_ANCS_PARSE_STATE_DONE;
    }

    if (attr_is_requested(ancs_c, ancs_c->attr_response.attr)) {
        ancs_c->parse_info.expected_number_of_attrs--;
    }
    wm_log_debug("Attribute ID %i ", ancs_c->attr_response.attr.attr_id);
    return BT_ANCS_PARSE_STATE_ATTR_LEN1;
}

/**@brief Function for parsing the length of an iOS attribute.
 *        Used in the @ref parse_get_notif_attrs_response state machine.
 *
 * @details The Length is 2 bytes. Since there is a chance we reveice the bytes in two different
 *          GATT notifications, we parse only the first byte here and then set the state machine
 *          ready to parse the next byte.
 *
 * @param[in] ancs_c   Pointer to an ANCS instance to which the event belongs.
 * @param[in] data_src Pointer to data that was received from the Notification Provider.
 * @param[in] index    Pointer to an index that helps us keep track of the current data
 *                     to be parsed.
 *
 * @return The next parse state.
 */
static enum bt_ancs_parse_state attr_len1_parse(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    ancs_c->attr_response.attr.attr_len = data_src[(*index)++];
    return BT_ANCS_PARSE_STATE_ATTR_LEN2;
}

/**@brief Function for parsing the length of an iOS attribute.
 *        Used in the @ref parse_get_notif_attrs_response state machine.
 *
 * @details Second byte of the length field. If the length is zero, it means that the attribute
 *          is not present and the state machine is set to parse the next attribute.
 *
 * @param[in] ancs_c   Pointer to an ANCS instance to which the event belongs.
 * @param[in] data_src Pointer to data that was received from the Notification Provider.
 * @param[in] index    Pointer to an index that helps us keep track of the current data
 *                     to be parsed.
 *
 * @return The next parse state.
 */
static enum bt_ancs_parse_state attr_len2_parse(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    ancs_c->attr_response.attr.attr_len |= (data_src[(*index)++] << 8);
    ancs_c->parse_info.current_attr_index = 0;

    if (ancs_c->attr_response.attr.attr_len != 0) {
        /* If the attribute has a length but there is no allocated
		 * space for this attribute
		 */
        if (!ancs_c->parse_info.attr_list[ancs_c->attr_response.attr.attr_id].attr_len ||
            !ancs_c->parse_info.attr_list[ancs_c->attr_response.attr.attr_id].attr_data) {
            return BT_ANCS_PARSE_STATE_ATTR_SKIP;
        } else {
            return BT_ANCS_PARSE_STATE_ATTR_DATA;
        }
    } else {
        wm_log_debug("Attribute LEN %i ", ancs_c->attr_response.attr.attr_len);
        if (attr_is_requested(ancs_c, ancs_c->attr_response.attr)) {
            if (ancs_c->ds_notif_cb) {
                ancs_c->ds_notif_cb(ancs_c, &ancs_c->attr_response);
            }
        }
        if (all_req_attrs_parsed(ancs_c)) {
            return BT_ANCS_PARSE_STATE_DONE;
        } else {
            return BT_ANCS_PARSE_STATE_ATTR_ID;
        }
    }
}

/**@brief Function for parsing the data of an iOS attribute.
 *        Used in the @ref parse_get_notif_attrs_response state machine.
 *
 * @details Read the data of the attribute into our local buffer.
 *
 * @param[in] ancs_c   Pointer to an ANCS instance to which the event belongs.
 * @param[in] data_src Pointer to data that was received from the Notification Provider.
 * @param[in] index    Pointer to an index that helps us keep track of the current data
 *                     to be parsed.
 *
 * @return The next parse state.
 */
static enum bt_ancs_parse_state attr_data_parse(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    /* We have not reached the end of the attribute, nor our max allocated internal size.
	 * Proceed with copying data over to our buffer.
	 */
    if ((ancs_c->parse_info.current_attr_index < ancs_c->parse_info.attr_list[ancs_c->attr_response.attr.attr_id].attr_len) &&
        (ancs_c->parse_info.current_attr_index < ancs_c->attr_response.attr.attr_len)) {
        /* Un-comment this line to see every byte of an attribute as it is parsed.
		 * Commented out by default since it can overflow the uart buffer.
		 */
        /* wm_log_debug("Byte copied to buffer: %c", data_src[(*index)]); */
        ancs_c->attr_response.attr.attr_data[ancs_c->parse_info.current_attr_index++] = data_src[(*index)++];
    }

    /* We have reached the end of the attribute, or our max allocated internal size.
	 * Stop copying data over to our buffer. NUL-terminate at the current index.
	 */
    if ((ancs_c->parse_info.current_attr_index == ancs_c->attr_response.attr.attr_len) ||
        (ancs_c->parse_info.current_attr_index ==
         ancs_c->parse_info.attr_list[ancs_c->attr_response.attr.attr_id].attr_len - 1)) {
        if (attr_is_requested(ancs_c, ancs_c->attr_response.attr)) {
            ancs_c->attr_response.attr.attr_data[ancs_c->parse_info.current_attr_index] = '\0';
        }

        /* If our max buffer size is smaller than the remaining attribute data, we must
		 * increase index to skip the data until the start of the next attribute.
		 */
        if (ancs_c->parse_info.current_attr_index < ancs_c->attr_response.attr.attr_len) {
            return BT_ANCS_PARSE_STATE_ATTR_SKIP;
        }
        wm_log_debug("Attribute finished!");
        if (attr_is_requested(ancs_c, ancs_c->attr_response.attr)) {
            if (ancs_c->ds_notif_cb) {
                ancs_c->ds_notif_cb(ancs_c, &ancs_c->attr_response);
            }
        }
        if (all_req_attrs_parsed(ancs_c)) {
            return BT_ANCS_PARSE_STATE_DONE;
        } else {
            return BT_ANCS_PARSE_STATE_ATTR_ID;
        }
    }
    return BT_ANCS_PARSE_STATE_ATTR_DATA;
}

static enum bt_ancs_parse_state attr_skip(struct bt_ancs_client *ancs_c, const uint8_t *data_src, uint32_t *index)
{
    /* We have not reached the end of the attribute, nor our max allocated internal size.
	 * Proceed with copying data over to our buffer.
	 */
    if (ancs_c->parse_info.current_attr_index < ancs_c->attr_response.attr.attr_len) {
        ancs_c->parse_info.current_attr_index++;
        (*index)++;
    }
    /* At the end of the attribute, determine if it should be passed to event handler and
	 * continue parsing the next attribute ID if we are not done with all the attributes.
	 */
    if (ancs_c->parse_info.current_attr_index == ancs_c->attr_response.attr.attr_len) {
        if (attr_is_requested(ancs_c, ancs_c->attr_response.attr)) {
            if (ancs_c->ds_notif_cb) {
                ancs_c->ds_notif_cb(ancs_c, &ancs_c->attr_response);
            }
        }
        if (all_req_attrs_parsed(ancs_c)) {
            return BT_ANCS_PARSE_STATE_DONE;
        } else {
            return BT_ANCS_PARSE_STATE_ATTR_ID;
        }
    }
    return BT_ANCS_PARSE_STATE_ATTR_SKIP;
}

void bt_ancs_parse_get_attrs_response(struct bt_ancs_client *ancs_c, const uint8_t *data_src, const uint16_t data_len)
{
    uint32_t index;

    for (index = 0; index < data_len;) {
        switch (ancs_c->parse_info.parse_state) {
            case BT_ANCS_PARSE_STATE_COMMAND_ID:
                ancs_c->parse_info.parse_state = command_id_parse(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_NOTIF_UID:
                ancs_c->parse_info.parse_state = notif_uid_parse(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_APP_ID:
                ancs_c->parse_info.parse_state = app_id_parse(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_ATTR_ID:
                ancs_c->parse_info.parse_state = attr_id_parse(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_ATTR_LEN1:
                ancs_c->parse_info.parse_state = attr_len1_parse(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_ATTR_LEN2:
                ancs_c->parse_info.parse_state = attr_len2_parse(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_ATTR_DATA:
                ancs_c->parse_info.parse_state = attr_data_parse(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_ATTR_SKIP:
                ancs_c->parse_info.parse_state = attr_skip(ancs_c, data_src, &index);
                break;

            case BT_ANCS_PARSE_STATE_DONE:
                wm_log_debug("Parse state: Done ");
                index = data_len;
                break;

            default:
                /* Default case will never trigger intentionally.
			 * Go to the DONE state to minimize the consequences.
			 */
                ancs_c->parse_info.parse_state = BT_ANCS_PARSE_STATE_DONE;
                break;
        }
    }
}

/**@brief Function for checking whether the data in an iOS notification is out of bounds.
 *
 * @param[in] notif  An iOS notification.
 *
 * @retval 0       If the notification is within bounds.
 * @retval -EINVAL If the notification is out of bounds.
 */
static int bt_ancs_verify_notification_format(const struct bt_ancs_evt_notif *notif)
{
    if ((notif->evt_id >= BT_ANCS_EVT_ID_COUNT) || (notif->category_id >= BT_ANCS_CATEGORY_ID_COUNT)) {
        return -EINVAL;
    }
    return 0;
}

void bt_ancs_parse_notif(struct bt_ancs_client *ancs_c, const uint8_t *data_src, const uint16_t len)
{
    int err;
    struct bt_ancs_evt_notif notif;

    if (len != BT_ANCS_NOTIF_DATA_LENGTH) {
        if (ancs_c->ns_notif_cb) {
            ancs_c->ns_notif_cb(ancs_c, -EINVAL, &notif);
        }
        return;
    }

    notif.evt_id = (enum bt_ancs_evt_id_values)data_src[BT_ANCS_NOTIF_EVT_ID_INDEX];

    notif.evt_flags.silent = (data_src[BT_ANCS_NOTIF_FLAGS_INDEX] >> BT_ANCS_EVENT_FLAG_SILENT) & 0x01;

    notif.evt_flags.important = (data_src[BT_ANCS_NOTIF_FLAGS_INDEX] >> BT_ANCS_EVENT_FLAG_IMPORTANT) & 0x01;

    notif.evt_flags.pre_existing = (data_src[BT_ANCS_NOTIF_FLAGS_INDEX] >> BT_ANCS_EVENT_FLAG_PREEXISTING) & 0x01;

    notif.evt_flags.positive_action = (data_src[BT_ANCS_NOTIF_FLAGS_INDEX] >> BT_ANCS_EVENT_FLAG_POSITIVE_ACTION) & 0x01;

    notif.evt_flags.negative_action = (data_src[BT_ANCS_NOTIF_FLAGS_INDEX] >> BT_ANCS_EVENT_FLAG_NEGATIVE_ACTION) & 0x01;

    notif.category_id = (enum bt_ancs_category_id_val)data_src[BT_ANCS_NOTIF_CATEGORY_ID_INDEX];

    notif.category_count = data_src[BT_ANCS_NOTIF_CATEGORY_CNT_INDEX];
    notif.notif_uid      = sys_get_le32(&data_src[BT_ANCS_NOTIF_NOTIF_UID]);

    err = bt_ancs_verify_notification_format(&notif);

    if (ancs_c->ns_notif_cb) {
        ancs_c->ns_notif_cb(ancs_c, err, &notif);
    }
}

static uint16_t encode_notif_action(uint8_t *encoded_data, uint32_t uid, enum bt_ancs_action_id_values action_id)
{
    uint8_t index = 0;

    encoded_data[index++] = BT_ANCS_COMMAND_ID_PERFORM_NOTIF_ACTION;
    sys_put_le32(uid, &encoded_data[index]);
    index += sizeof(uint32_t);
    encoded_data[index++] = (uint8_t)action_id;

    return index;
}

static void bt_ancs_cp_write_callback(struct bt_conn *conn, uint8_t err, struct bt_gatt_write_params *params)
{
    struct bt_ancs_client *ancs_c;
    bt_ancs_write_cb write_cb;

    /* Retrieve ANCS client module context. */
    ancs_c = CONTAINER_OF(params, struct bt_ancs_client, cp_write_params);

    write_cb = ancs_c->cp_write_cb;
    atomic_clear_bit(&ancs_c->state, ANCS_CP_WRITE_PENDING);
    if (write_cb) {
        write_cb(ancs_c, err);
    }
}

int bt_ancs_cp_write(struct bt_ancs_client *ancs_c, uint16_t len, bt_ancs_write_cb func)
{
    int err;
    struct bt_gatt_write_params *write_params = &ancs_c->cp_write_params;

    write_params->func   = bt_ancs_cp_write_callback;
    write_params->handle = ancs_c->handle_cp;
    write_params->offset = 0;
    write_params->data   = ancs_c->cp_data;
    write_params->length = len;

    ancs_c->cp_write_cb = func;

    err = bt_gatt_write(ancs_c->conn, write_params);
    if (err) {
        atomic_clear_bit(&ancs_c->state, ANCS_CP_WRITE_PENDING);
    }

    return err;
}

int bt_ancs_notification_action(struct bt_ancs_client *ancs_c, uint32_t uuid, enum bt_ancs_action_id_values action_id,
                                bt_ancs_write_cb func)
{
    if (atomic_test_and_set_bit(&ancs_c->state, ANCS_CP_WRITE_PENDING)) {
        return -EBUSY;
    }

    uint8_t *data = ancs_c->cp_data;
    uint16_t len  = encode_notif_action(data, uuid, action_id);

    return bt_ancs_cp_write(ancs_c, len, func);
}

static int bt_ancs_get_notif_attrs(struct bt_ancs_client *ancs_c, const uint32_t uid, bt_ancs_write_cb func)
{
    if (atomic_test_and_set_bit(&ancs_c->state, ANCS_CP_WRITE_PENDING)) {
        return -EBUSY;
    }

    uint32_t index = 0;
    uint8_t *data  = ancs_c->cp_data;

    ancs_c->number_of_requested_attr = 0;

    /* Encode Command ID. */
    *(data + index++) = BT_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES;

    /* Encode Notification UID. */
    sys_put_le32(uid, data + index);
    index += sizeof(uint32_t);

    /* Encode Attribute ID. */
    for (uint32_t attr = 0; attr < BT_ANCS_NOTIF_ATTR_COUNT; attr++) {
        if (ancs_c->ancs_notif_attr_list[attr].get) {
            *(data + index++) = (uint8_t)attr;

            if ((attr == BT_ANCS_NOTIF_ATTR_ID_TITLE) || (attr == BT_ANCS_NOTIF_ATTR_ID_SUBTITLE) ||
                (attr == BT_ANCS_NOTIF_ATTR_ID_MESSAGE)) {
                /* Encode Length field. Only applicable for
				 * Title, Subtitle, and Message.
				 */
                sys_put_le16(ancs_c->ancs_notif_attr_list[attr].attr_len, data + index);
                index += sizeof(uint16_t);
            }

            ancs_c->number_of_requested_attr++;
        }
    }

    ancs_c->parse_info.expected_number_of_attrs = ancs_c->number_of_requested_attr;

    return bt_ancs_cp_write(ancs_c, index, func);
}

int bt_ancs_request_attrs(struct bt_ancs_client *ancs_c, const struct bt_ancs_evt_notif *notif, bt_ancs_write_cb func)
{
    int err;

    err = bt_ancs_verify_notification_format(notif);
    if (err) {
        return err;
    }

    ancs_c->parse_info.parse_state = BT_ANCS_PARSE_STATE_COMMAND_ID;

    return bt_ancs_get_notif_attrs(ancs_c, notif->notif_uid, func);
}

int bt_ancs_register_attr(struct bt_ancs_client *ancs_c, const enum bt_ancs_notif_attr_id_val id, uint8_t *data,
                          const uint16_t len)
{
    if (!ancs_c || !data) {
        return -EINVAL;
    }

    if (!len || len > BT_ANCS_ATTR_DATA_MAX) {
        return -EINVAL;
    }

    if ((size_t)id >= BT_ANCS_NOTIF_ATTR_COUNT) {
        return -EINVAL;
    }

    ancs_c->ancs_notif_attr_list[id].get       = true;
    ancs_c->ancs_notif_attr_list[id].attr_len  = len;
    ancs_c->ancs_notif_attr_list[id].attr_data = data;

    return 0;
}

int bt_ancs_register_app_attr(struct bt_ancs_client *ancs_c, const enum bt_ancs_app_attr_id_val id, uint8_t *data,
                              const uint16_t len)
{
    if (!ancs_c || !data) {
        return -EINVAL;
    }

    if (!len || len > BT_ANCS_ATTR_DATA_MAX) {
        return -EINVAL;
    }

    if ((size_t)id >= BT_ANCS_APP_ATTR_COUNT) {
        return -EINVAL;
    }

    ancs_c->ancs_app_attr_list[id].get       = true;
    ancs_c->ancs_app_attr_list[id].attr_len  = len;
    ancs_c->ancs_app_attr_list[id].attr_data = data;

    return 0;
}

int bt_ancs_client_init(struct bt_ancs_client *ancs_c)
{
    int err;

    if (!ancs_c) {
        return -EINVAL;
    }

    memset(ancs_c, 0, sizeof(struct bt_ancs_client));

    attr_appid = (uint8_t *)malloc(64);
    if (!attr_appid) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_appid, 0, 64);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER, attr_appid, 64);
    if (err) {
        goto free_init;
        ;
    }

    attr_disp_name = (uint8_t *)malloc(128);
    if (!attr_disp_name) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_disp_name, 0, 128);

    err = bt_ancs_register_app_attr(ancs_c, BT_ANCS_APP_ATTR_ID_DISPLAY_NAME, attr_disp_name, 128);
    if (err) {
        goto free_init;
        ;
    }

    attr_title = (uint8_t *)malloc(128);
    if (!attr_title) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_title, 0, 128);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_TITLE, attr_title, 128);
    if (err) {
        goto free_init;
        ;
    }

    attr_message = (uint8_t *)malloc(512);
    if (!attr_message) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_message, 0, 512);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_MESSAGE, attr_message, 512);
    if (err) {
        goto free_init;
        ;
    }

    attr_subtitle = (uint8_t *)malloc(64);
    if (!attr_subtitle) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_subtitle, 0, 64);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_SUBTITLE, attr_subtitle, 64);
    if (err) {
        goto free_init;
        ;
    }

    attr_message_size = (uint8_t *)malloc(64);
    if (!attr_message_size) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_message_size, 0, 64);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE, attr_message_size, 64);
    if (err) {
        goto free_init;
        ;
    }

    attr_date = (uint8_t *)malloc(128);
    if (!attr_date) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_date, 0, 128);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_DATE, attr_date, 128);
    if (err) {
        goto free_init;
        ;
    }

    attr_posaction = (uint8_t *)malloc(128);
    if (!attr_posaction) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_posaction, 0, 128);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL, attr_posaction, 128);
    if (err) {
        goto free_init;
        ;
    }

    attr_negaction = (uint8_t *)malloc(128);
    if (!attr_negaction) {
        err = -ENOMEM;
        goto free_init;
    }
    memset(attr_negaction, 0, 128);

    err = bt_ancs_register_attr(ancs_c, BT_ANCS_NOTIF_ATTR_ID_NEGATIVE_ACTION_LABEL, attr_negaction, 128);
    if (err) {
        goto free_init;
        ;
    }

    return 0;

free_init:

    if (attr_appid) {
        free(attr_appid);
        attr_appid = NULL;
    }

    if (attr_disp_name) {
        free(attr_disp_name);
        attr_disp_name = NULL;
    }

    if (attr_title) {
        free(attr_title);
        attr_title = NULL;
    }

    if (attr_message) {
        free(attr_message);
        attr_message = NULL;
    }

    if (attr_subtitle) {
        free(attr_subtitle);
        attr_subtitle = NULL;
    }

    if (attr_message_size) {
        free(attr_message_size);
        attr_message_size = NULL;
    }

    if (attr_date) {
        free(attr_date);
        attr_date = NULL;
    }

    if (attr_posaction) {
        free(attr_posaction);
        attr_posaction = NULL;
    }

    if (attr_negaction) {
        free(attr_negaction);
        attr_negaction = NULL;
    }

    return err;
}
