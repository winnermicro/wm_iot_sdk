/*
 * Copyright (c) 2018 WinnerMicro Semiconductor 
 *
 */

#ifndef BT_WPS_SVC_H_
#define BT_WPS_SVC_H_

/**
 * @file
 * @defgroup bt_wps WinnerMicro WiFi Provision GATT Service
 * @{
 * @brief WinnerMicro WiFi Provision (WPS) GATT Service API.
 */

#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/svc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Initialize the service.
 *
 * @details This function:
 *          1)turn on bt stack
 *          2)registers a GATT service with two characteristics,
 *          TX and RX. A remote device that is connected to this service
 *          can send data to the RX Characteristic. When the remote enables
 *          indications, it is indicated when data is sent to the TX
 *          Characteristic.
 *          3)start advertising
 *
 * @param[in] callbacks  Struct with function pointers to callbacks for service
 *                       events. If no callbacks are needed, this parameter can
 *                       be NULL.
 *
 * @retval 0 If initialization is successful.
 *           Otherwise, a negative value is returned.
 */
int bt_wps_init(struct bt_svc_cb *callbacks);

/**@brief UnInitialize the service.
 *
 * @details This function:
 *           1)stop advertising or disconnect with remote peer
 *           2)turn off bt stack
 *
 * @retval 0 Always success
 */
int bt_wps_uninit(void);

/**@brief Send data.
 *
 * @details This function sends data to a connected peer
 *
 * @param[in] data Pointer to a data buffer.
 * @param[in] len  Length of the data in the buffer.
 *
 * @retval 0 If the data is sent.
 *           Otherwise, a negative value is returned.
 */
int bt_wps_send(const uint8_t *data, uint16_t len);

/**@brief Get maximum data length that can be used for @ref bt_wps_send.
 *
 * @param[in] conn Pointer to connection Object.
 *
 * @return Maximum data length.
 */
static inline uint32_t bt_wps_get_mtu(struct bt_conn *conn)
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
