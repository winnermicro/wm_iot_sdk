/** @file
 *  @brief Bluetooth subsystem controller APIs.
 */

/**
 * @file    controller.h
 *
 * @brief   Bluetooth API
 *
 * @author  WinnerMicro
 *
 * Copyright (c) 2020 Winner Microelectronics Co., Ltd.
 */

#ifndef _WM_BT_CONTROLLER_H_
#define _WM_BT_CONTROLLER_H_

/**
 * @brief Bluetooth Controller
 * @defgroup bt_ctrl Bluetooth Controller
 * @ingroup bluetooth
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Bluetooth Error Status */
typedef enum
{
    WM_BT_STATUS_SUCCESS, /**< success */
    WM_BT_STATUS_FAIL,
    WM_BT_STATUS_NOT_READY,
    WM_BT_STATUS_NOMEM, /**< alloc memory failed */
    WM_BT_STATUS_BUSY,
    WM_BT_STATUS_DONE,        /**< request already completed */
    WM_BT_STATUS_UNSUPPORTED,
    WM_BT_STATUS_PARM_INVALID,
    WM_BT_STATUS_UNHANDLED,
    WM_BT_STATUS_AUTH_FAILURE,
    WM_BT_STATUS_RMT_DEV_DOWN,
    WM_BT_STATUS_AUTH_REJECTED,
    WM_BT_STATUS_THREAD_FAILED, /**< create internal thread failed */
    WM_BT_STATUS_INTERNAL_ERROR, /**< controller stack internal error */
    WM_BT_STATUS_CTRL_ENABLE_FAILED,
    WM_BT_STATUS_HOST_ENABLE_FAILED,
    WM_BT_STATUS_CTRL_DISABLE_FAILED,
    WM_BT_STATUS_HOST_DISABLE_FAILED,

} wm_bt_status_t;

typedef enum
{
    WM_BT_CTRL_IDLE     =              (1<<0),
    WM_BT_CTRL_ENABLED  =              (1<<1),
    WM_BT_CTRL_SLEEPING =              (1<<2),
    WM_BT_CTRL_BLE_ROLE_MASTER =       (1<<3),
    WM_BT_CTRL_BLE_ROLE_SLAVE =        (1<<4),
    WM_BT_CTRL_BLE_ROLE_END =          (1<<5),
    WM_BT_CTRL_BLE_STATE_IDLE =        (1<<6),
    WM_BT_CTRL_BLE_STATE_ADVERTISING = (1<<7),
    WM_BT_CTRL_BLE_STATE_SCANNING =    (1<<8),
    WM_BT_CTRL_BLE_STATE_INITIATING =  (1<<9),
    WM_BT_CTRL_BLE_STATE_STOPPING =    (1<<10),
    WM_BT_CTRL_BLE_STATE_TESTING =     (1<<11),
} wm_bt_ctrl_status_t;

typedef struct
{
  uint8_t  uart_index;  /**< uart port index, 1~4 */
  uint32_t band_rate;   /**< band rate: 115200 ~ 2M */
  uint8_t data_bit;     /**< data bit:5 ~ 8 */
  uint8_t verify_bit;   /**< 0:NONE, 1 ODD, 2 EVEN */
  uint8_t stop_bit;     /**< 0:1bit; 1:1.5bit; 2:2bits */
} wm_bt_hci_if_t;

typedef enum
{
	WM_BLE_PWR_TYPE_CONN_HDL0,
	WM_BLE_PWR_TYPE_CONN_HDL1,
	WM_BLE_PWR_TYPE_CONN_HDL2,
	WM_BLE_PWR_TYPE_CONN_HDL3,
	WM_BLE_PWR_TYPE_CONN_HDL4,
	WM_BLE_PWR_TYPE_CONN_HDL5,
	WM_BLE_PWR_TYPE_CONN_HDL6,
	WM_BLE_PWR_TYPE_CONN_HDL7,
	WM_BLE_PWR_TYPE_CONN_HDL8,
	WM_BLE_PWR_TYPE_ADV,
	WM_BLE_PWR_TYPE_SCAN,
	WM_BLE_PWR_TYPE_DEFAULT,
} wm_ble_power_type_t;

typedef enum
{
	WM_BT_LOG_NONE = 0,
	WM_BT_LOG_ERROR = 1,
	WM_BT_LOG_WARNING = 2,
	WM_BT_LOG_API = 3,
	WM_BT_LOG_EVENT = 4,
	WM_BT_LOG_DEBUG = 5,
	WM_BT_LOG_VERBOSE = 6,
} wm_bt_log_level_t;

typedef enum
{
	WM_AUDIO_OVER_HCI = 0,
	WM_AUDIO_INTERNAL_MODE,
} wm_sco_data_path_t;

typedef struct
{
	void (*notify_controller_avaiable_hci_buffer)(int cnt);
	void (*notify_host_recv_h4)(uint8_t *ptr, uint16_t length);
} wm_bt_host_if_t;

typedef void (*wm_bt_controller_sleep_enter_func_ptr)(uint32_t sleep_duration_ms);

typedef void (*wm_bt_controller_sleep_exit_func_ptr)(void);

typedef void (*wm_bt_app_pending_process_func_ptr)(void);


/**
 * @brief          enable the bluetooth controller stack
 *
 * @param[in]      *p_hci_if     pointer on uart property
 * @param[in]       log_level    @ref wm_bt_log_level_t
 *
 * @return @ref wm_bt_status_t
 *
 */
wm_bt_status_t wm_bt_ctrl_enable(wm_bt_hci_if_t *p_hci_if, wm_bt_log_level_t log_level);


/**
 * @brief          disable the bluetooth controller stack
 *
 *
 * @return         @ref wm_bt_status_t
 *
 */
wm_bt_status_t wm_bt_ctrl_disable(void);

/**
 * @brief          configure the ble emit power of different ble handle type
 *
 * @param[in]      power_type     @ref wm_ble_power_type_t
 * @param[in]      power_level_index    [1,2,3,4,5] map to[1,4,7,10,13]dBm
 *
 * @return @ref wm_bt_status_t
 *
 * @note           power_type, supports WM_BLE_PWR_TYPE_DEFAULT only. 
 */
wm_bt_status_t wm_ble_set_tx_power(wm_ble_power_type_t power_type, int8_t power_level_index);

/**
 * @brief          get the ble emit power of different ble handle type
 *
 * @param[in]      power_type     @ref wm_ble_power_type_t
 *
 * @retval         power value db
 *
 * @note           power_type, supports WM_BLE_PWR_TYPE_DEFAULT only. 
 */
int8_t  wm_ble_get_tx_power(wm_ble_power_type_t power_type);

/**
 * @brief          configure the classic/enhanced bluetooth transmit power
 *
 * @param[in]      min_power_level    power level[1,13]dBm
 * @param[in]      max_power_level    power level[1,13]dBm
 *
 * @return @ref wm_bt_status_t
 *
 */
wm_bt_status_t wm_bredr_set_tx_power(int8_t min_power_level,int8_t max_power_level);

/**
 * @brief          get the classic/enhanced bluetooth transmit power level
 *
 * @param[in]      *min_power_level    pointer on min_power_level
 * @param[in]      *max_power_level    pointer on max_power_level
 *
 * @return @ref wm_bt_status_t
 *
 */
wm_bt_status_t  wm_bredr_get_tx_power(int8_t* min_power_level, int8_t* max_power_level);

/**
 * @brief          configure the voice output path
 *
 * @param[in]      data_path    @ref wm_sco_data_path_t
 *
 * @return @ref wm_bt_status_t
 *
 */
wm_bt_status_t wm_bredr_sco_datapath_set(wm_sco_data_path_t data_path);

/**
 * @brief          get controller stack status
 *
 *
 * @retval         controller status(bit mapped)
 *
 */
uint32_t wm_bt_ctrl_get_status(void);

/**
 * @brief          this function receive the hci message from host hci_h4 inteface
 *
 * @param[in]      *data    hci formated message
 * @param[in]       len     command length
 *
 * @return @ref wm_bt_ctrl_status_t
 *
 */
wm_bt_status_t wm_bt_host_send_packet( uint8_t *data, uint16_t len);

/**
 * @brief          this function register the host stack receive message function 
 *                 and indication the controller receive hci command avaiable
 *
 * @param[in]      p_host_if       @ref wm_bt_host_if_t
 *
 * @return @ref wm_bt_ctrl_status_t
 *
 */
wm_bt_status_t wm_bt_ctrl_if_register(const wm_bt_host_if_t *p_host_if);

/**
 * @brief          this function unregister the host stack receive message function 
 *                 and indication the controller receive hci command avaiable
 *
 *
 * @return @ref wm_bt_ctrl_status_t
 *
 */
wm_bt_status_t wm_bt_ctrl_if_unregister(void);


/**
 * @brief          this function configure the controller enter into sleep mode when controller
 *                 is in idle mode
 *
 * @param[in]      enable bool value
 *
 * @return @ref wm_bt_ctrl_status_t
 *
 */
wm_bt_status_t wm_bt_ctrl_sleep(bool enable);

/**
 * @brief          this function look up the controller is in sleep mode or not
 *
 *
 * @return bool value
 *
 */
bool  wm_bt_ctrl_is_sleep(void);

/**
 * @brief          this function wake up the controller, in other words exit sleep mode
 *
 * @return @ref wm_bt_ctrl_status_t
 *
 */
wm_bt_status_t wm_bt_ctrl_wakeup(void);

/**
 * @brief          this function check controller can handle hci commands yes or no
 *
 *
 * @return bool value
 *
 */

bool wm_bt_host_check_send_available(void);

/**
 * @brief          this function exit bluetooth test mode

 *
 * @return @ref wm_bt_ctrl_status_t
 *
 */
wm_bt_status_t wm_bt_disable_test_mode(void);

/**
 * @brief          this function enable bluetooth test mode
 *
 * @param[in]       p_hci_if : specific the uart port property
 *
 * @return @ref wm_bt_ctrl_status_t
 *
 */

wm_bt_status_t wm_bt_enable_test_mode(wm_bt_hci_if_t *p_hci_if);

/**
 * @brief          this function configure rf work mode
 *
 * @param[in]      enable : 1 bluetooth only mode, 0 wifi-bluetooth coexist mode
 *
 * @retval         None
 *
 */

void wm_bt_rf_mode(uint8_t enable);

/**
 * @brief          this function configure mesh feature of controller
 *
 * @param[in]      enable : 1 turn on mesh mode, 0 turn off mesh mode
 *
 * @retval         always WM_BT_STATUS_SUCCESS
 *
 */

wm_bt_status_t wm_bt_set_mesh_mode(uint8_t enable);

/**
 * @brief          this function register callback function when controller entering or exiting sleep mode 
 *
 * @param[in]      sleep_enter_func : sleep starting callback; 
 * @param[in]      sleep_exit_func : sleep exiting callback
 *
 * @retval         WM_BT_STATUS_SUCCESS or WM_BT_STATUS_UNSUPPORTED;
 *
 */

wm_bt_status_t wm_bt_register_sleep_callback(wm_bt_controller_sleep_enter_func_ptr sleep_enter_func, wm_bt_controller_sleep_exit_func_ptr sleep_exit_func);

/**
 * @brief          this function register blocking operation function.
 *
 * @param[in]      pending_func blocking operation function pointer
 *
 * @retval         always WM_BT_STATUS_SUCCESS;
 *
 */

wm_bt_status_t wm_bt_register_pending_process_callback(wm_bt_app_pending_process_func_ptr pending_func);


#ifdef __cplusplus
}
#endif
/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_CONTROLLER_H_ */
