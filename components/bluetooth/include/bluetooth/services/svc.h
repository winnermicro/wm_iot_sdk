/*
 * Copyright (c) 2018 WinnerMicro Semiconductor 
 *
 */

#ifndef BT_SVC_H_
#define BT_SVC_H_

/**
 * @file
 * @defgroup bt_svc WinnerMicro GATT Service API
 * @{
 * @brief WinnerMicro GATT Service API.
 */

#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief svc send status. */
enum bt_svc_send_status {
    /** Send indication enabled. */
    BT_SVC_SEND_STATUS_ENABLED,
    /** Send indication disabled. */
    BT_SVC_SEND_STATUS_DISABLED,
};

/** @brief Pointers to the callback functions for service events. */
struct bt_svc_cb {
    /** @brief Data received callback.
	 *
	 * The data has been received as a write request on the WPS RX
	 * Characteristic.
	 *
	 * @param[in] conn  Pointer to connection object that has received data.
	 * @param[in] data  Received data.
	 * @param[in] len   Length of received data.
	 */
    void (*received)(struct bt_conn *conn, const uint8_t *const data, uint16_t len);

    /** @brief Data sent callback.
	 *
	 * The data has been sent as a notification and written on the WPS TX
	 * Characteristic.
	 *
	 * @param[in] conn Pointer to connection object, or NULL if sent to all
	 *                 connected peers.
	 */
    void (*sent)(struct bt_conn *conn);

    /** @brief Connected callback.
	 *
	 *  @param[in] conn New connection object.
	 *  @param[in] err HCI error. Zero for success, non-zero otherwise.
	 */
    void (*connected)(struct bt_conn *conn, uint8_t err);

    /** @brief Disconected callback.
	 *
     *  @param conn Connection object.
     *  @param reason BT_HCI_ERR_* reason for the disconnection.
     */
    void (*disconnected)(struct bt_conn *conn, uint8_t reason);

    /** @brief Send state callback.
	 *
	 * Indicate the CCCD descriptor status of the WPS TX characteristic.
	 *
	 * @param[in] status Send notification status.
	 */
    void (*send_enabled)(enum bt_svc_send_status status);
};


/**@brief Get maximum data length that can be used for @ref bt_wps_send.
 *
 * @param[in] conn Pointer to connection Object.
 *
 * @return Maximum data length.
 */
static inline uint32_t bt_svc_get_mtu(struct bt_conn *conn)
{
    /* According to 3.4.7.1 Handle Value Indication of the ATT protocol.
	 * Maximum supported notification is ATT_MTU - 3 */
    return bt_gatt_get_mtu(conn) - 3;
}

#ifdef __cplusplus
}
#endif

/**
 *@}
 */

#endif /* BT_WPS_H_ */

