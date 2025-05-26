/**
 * @file     wm_atcmd.h
 *
 * @brief    AT Command Module Header File
 *
 */

/**
 *  Copyright 2022-2024 WinnerMicro Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __WM_ATCMD_H__
#define __WM_ATCMD_H__

#include "wmsdk_config.h"
#include "wm_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_ATCMD_Macros WM AT Command Macros
 * @brief WinnerMicro AT Command Macros
 */

/**
 * @addtogroup WM_ATCMD_Macros
 * @{
 */

#define WM_ATCMD_QUESTION_MARK         '?' /**< Question mark character */
#define WM_ATCMD_EQUAL_MARK            '=' /**< Equal mark character */

#define WM_ATCMD_L_SQ_BRACKET          '['  /**< Left square bracket */
#define WM_ATCMD_R_SQ_BRACKET          ']'  /**< Right square bracket */
#define WM_ATCMD_L_ANGLE_BRACKET       '<'  /**< Left angle bracket */
#define WM_ATCMD_R_ANGLE_BRACKET       '>'  /**< Right angle bracket */
#define WM_ATCMD_DOUBLE_QUOTAION_MARKS '"'  /**< Double quotation marks */
#define WM_ATCMD_COMMA_MARK            ','  /**< Comma */
#define WM_ATCMD_CR                    '\r' /**< Carriage return */
#define WM_ATCMD_LF                    '\n' /**< Line feed */
#define WM_ATCMD_PLUS_MARK             '+'  /**< Plus mark */
#define WM_ATCMD_ESCAPE_CHR            '\\' /**< Escape character */

#ifndef MAX_ARGC
#define MAX_ARGC 20 /**< Maximum number of command line parameters */
#endif

#ifndef AT_CMD_START_STR_LEN
#define AT_CMD_START_STR_LEN 4 /**< Start the length of the at command */
#endif

#ifndef AT_CMD_START_STR
#define AT_CMD_START_STR "AT" /**< Start the string of the at command */
#endif

#ifndef AT_CMD_END_STR_LEN
#define AT_CMD_END_STR_LEN 4 /**< End the length of the at command */
#endif

#ifndef AT_CMD_END_STR
#define AT_CMD_END_STR "\r\n" /**< At command end string */
#endif

#ifndef AT_CMD_LEN
#define AT_CMD_LEN 150 /**< At command length */
#endif

#ifndef AT_CMD_RECV_BUFF_LEN
#define AT_CMD_RECV_BUFF_LEN 256 /**< At command receive buf length */
#endif

#ifndef AT_RESULT_BUFFER_SIZE
#define AT_RESULT_BUFFER_SIZE 256 /**< At size of the result buffer */
#endif

#define WM_AT_DUMP_BUFF(buf, len)                          \
    do {                                                   \
        for (int i = 0; i < len; i++) {                    \
            wm_atcmd_print_result("%c", ((char *)buf)[i]); \
        }                                                  \
        wm_atcmd_print_result("\r\n");                     \
    } while (0)

#define WM_ATCMD_GET_WIFI_CTL(at_server)      (((wm_at_server_ctl_t *)at_server)->wm_atcmd_wifi_param)
#define WM_ATCMD_GET_TCPIP_CTL(at_server)     (((wm_at_server_ctl_t *)at_server)->wm_atcmd_tcpip_param)
#define WM_ATCMD_GET_BT_CTL(at_server)        (((wm_at_server_ctl_t *)at_server)->wm_atcmd_bt_param)
#define WM_ATCMD_GET_SYS_CTL(at_server)       (((wm_at_server_ctl_t *)at_server)->wm_atcmd_sys_param)
#define WM_ATCMD_SET_ERRNO(at_server, errno)  (((wm_at_server_ctl_t *)at_server)->errno = errno)

#define WM_AT_ERRCOD_BASE_OFFSET              (1 << 24)
#define WM_AT_ERRCODE_SUBTYPE_OFFSET(subtype) (subtype << 16)
#define WM_AT_ERRCODE_SUBTYPE(subtype)        (WM_AT_ERRCOD_BASE_OFFSET | WM_AT_ERRCODE_SUBTYPE_OFFSET(subtype))

#define WM_AT_SUBTYPE_OK                      WM_AT_ERRCODE_SUBTYPE(0)
#define WM_AT_SUBTYPE_COMMON_ERROR            WM_AT_ERRCODE_SUBTYPE(1)
#define WM_AT_SUBTYPE_NO_TERMINATOR           WM_AT_ERRCODE_SUBTYPE(2)
#define WM_AT_SUBTYPE_NO_AT                   WM_AT_ERRCODE_SUBTYPE(3)
#define WM_AT_SUBTYPE_PARA_LENGTH_MISMATCH    WM_AT_ERRCODE_SUBTYPE(4)
#define WM_AT_SUBTYPE_PARA_TYPE_MISMATCH      WM_AT_ERRCODE_SUBTYPE(5)
#define WM_AT_SUBTYPE_PARA_NUM_MISMATCH       WM_AT_ERRCODE_SUBTYPE(6)
#define WM_AT_SUBTYPE_PARA_INVALID            WM_AT_ERRCODE_SUBTYPE(7)
#define WM_AT_SUBTYPE_PARA_PARSE_FAIL         WM_AT_ERRCODE_SUBTYPE(8)
#define WM_AT_SUBTYPE_UNSUPPORT_CMD           WM_AT_ERRCODE_SUBTYPE(9)
#define WM_AT_SUBTYPE_CMD_EXEC_FAIL           WM_AT_ERRCODE_SUBTYPE(10)
#define WM_AT_SUBTYPE_CMD_PROCESSING          WM_AT_ERRCODE_SUBTYPE(11)
#define WM_AT_SUBTYPE_CMD_OP_ERROR            WM_AT_ERRCODE_SUBTYPE(12)

#define AT_WAITING_FOREVER                    -1

/**
 * @}
 */

/**
 * @defgroup WM_ATCMD_Type_Definitions WM AT Command Type Definitions
 * @brief WinnerMicro AT Command Type Definitions
 */

/**
 * @addtogroup WM_ATCMD_Type_Definitions
 * @{
 */

/**
 * @brief AT command message handler function pointer type definition.
 *
 * This type definition represents a function pointer that handles AT command messages.
 * @param param A pointer to the parameter associated with the message.
 */
typedef void (*wm_atcmd_msg_func)(void *param);

/**
 * @}
 */

/**
 * @defgroup WM_ATCMD_Enumerations WM AT Command Enumerations
 * @brief WinnerMicro AT Command Enumerations
 */

/**
 * @addtogroup WM_ATCMD_Enumerations
 * @{
 */

/**
 * @brief AT message types enumeration.
 *
 * This enumeration defines the different types of messages that can be processed by the AT command module.
 */
typedef enum {
    WM_ATCMD_BASE        = 0x100,             /**< Base value for AT command message types. */
    WM_ATCMD_MSG_RECVCMD = WM_ATCMD_BASE + 1, /**< Triggered when a command is received. */
    WM_ATCMD_MSG_PASSTHROUGH_RECV,            /**< Triggered when unvarnished transmission data is received. */
    WM_ATCMD_MSG_EXIT_PASSTHROUGH,            /**< Triggered when exiting passthrough mode. */
    WM_ATCMD_MSG_NUM_MAX                      /**< Maximum number of message types. */
} wm_at_msg_type_e;

/**
 * @brief Reasons for exiting passthrough mode.
 *
 * This enumeration lists the possible reasons for exiting passthrough mode.
 */
typedef enum wm_atcmd_exit_passthrough_reason {
    WM_ATCMD_EXIT_REASON_USER_ACTIVE = 0, /**< User actively exits passthrough mode. */
    WM_ATCMD_EXIT_REASON_RECV_EXIT_CODE,  /**< Exits due to receiving an exit code. */
    WM_ATCMD_EXIT_REASON_TIMEOUT,         /**< Exits due to a timeout. */
    WM_ATCMD_EXIT_REASON_NO_MEM,          /**< Exits due to lack of memory. */
    WM_ATCMD_EXIT_REASON_MAX              /**< Maximum number of exit reasons. */
} wm_atcmd_exit_passthrough_reason_e;

/**
 * @brief Device types for AT commands.
 *
 * This enumeration specifies the different types of devices that can be used with AT commands.
 */
typedef enum wm_atcmd_dev_type {
    WM_ATCMD_DEV_TYPE_UART = 0, /**< UART device type. */
    WM_ATCMD_DEV_TYPE_SPI,      /**< SPI device type. */
    WM_ATCMD_DEV_TYPE_I2C,      /**< I2C device type. */
    WM_ATCMD_DEV_TYPE_NET,      /**< Network device type. */
    WM_ATCMD_DEV_TYPE_BT,       /**< Bluetooth device type. */
    WM_ATCMD_DEV_TYPE_BLE,      /**< Bluetooth Low Energy device type. */
    WM_ATCMD_DEV_TYPE_MAX       /**< Maximum number of device types. */
} wm_atcmd_dev_type_e;

/**
 * @brief Types of AT commands.
 *
 * This enumeration distinguishes between different types of AT commands based on their functionality.
 */
typedef enum wm_at_type {
    AT_TYPE_QUERY = 0, /**< Query type AT command, indicated by '?'. */
    AT_TYPE_TEST  = 1, /**< Test type AT command, indicated by '=?'. */
    AT_TYPE_EXEC  = 2, /**< Execute type AT command. */
    AT_TYPE_MAX        /**< Maximum number of AT command types. */
} wm_at_type_e;

/**
 * @brief Results of AT command processing.
 *
 * This enumeration lists the possible results that can be returned after processing an AT command.
 */
typedef enum wm_at_result {
    WM_AT_RESULT_OK = 0,     /**< AT command executed successfully. */
    WM_AT_RESULT_FAIL,       /**< AT command resulted in a generic error. */
    WM_AT_RESULT_NULL,       /**< No string returned as a result of the AT command. */
    WM_AT_RESULT_CMD_ERR,    /**< AT command not found. */
    WM_AT_RESULT_PARSE_FAIL, /**< Failed to parse the AT command. */
    AT_RESULT_MAX            /**< Maximum number of result types. */
} wm_at_result_e;

/**
 * @}
 */

/**
 * @defgroup WM_ATCMD_Structures WM AT Command Structures
 * @brief WinnerMicro AT Command Structures
 */

/**
 * @addtogroup WM_ATCMD_Structures
 * @{
 */

/**
 * @brief AT message structure.
 *
 * This structure represents an AT command message and includes message ID, length, data, and handler.
 */
typedef struct wm_at_msg {
    int msgid;                /**< Message ID */
    int data_len;             /**< Length of the message data */
    uint8_t *data;            /**< Pointer to the message data */
    wm_atcmd_msg_func handle; /**< Function to handle the message */
    void *param;              /**< Parameter for the message handler */
} wm_at_msg_t;

/**
 * @brief Device operations structure.
 *
 * This structure defines the operations for an AT command device, including sending and receiving functions.
 */
typedef struct wm_dev_ops {
    int (*atcmd_send)(const void *buffer, int size);         /**< Function to send data */
    int (*atcmd_recv)(void *buffer, int *size, int timeout); /**< Function to receive data */
} wm_dev_ops_t;

/**
 * @brief AT command device structure.
 *
 * This structure represents an AT command device, including its type and operations.
 */
typedef struct wm_atcmd_dev {
    wm_atcmd_dev_type_e type; /**< Type of the device */
    wm_dev_ops_t ops;         /**< Operations associated with the device */
} wm_atcmd_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_ATCMD_Type_Definitions WM AT Command Type Definitions
 * @brief WinnerMicro AT Command Type Definitions
 */

/**
 * @addtogroup WM_ATCMD_Type_Definitions
 * @{
 */

/**
 * @brief AT command action function pointer type definition.
 *
 * This type definition represents a function pointer that performs an action based on the AT command type.
 * @param type The type of the AT command.
 * @param argc The number of arguments for the command.
 * @param argv The arguments for the command.
 * @return An integer value indicating the result of the action.
 */
typedef int (*wm_atcmd_act)(wm_at_type_e type, int argc, char **argv);

/**
 * @brief AT command entry function pointer type definition.
 *
 * This type definition represents a function pointer that serves as an entry point for processing AT commands.
 * @param dev A pointer to the AT command device structure.
 * @param atcmd_buf A pointer to the buffer containing the AT command.
 * @return An integer value indicating the result of the entry action.
 */
typedef int (*wm_atcmd_entry)(wm_atcmd_dev_t *dev, char *atcmd_buf);

/**
 * @brief Passthrough transmit function pointer type definition.
 *
 * This type definition represents a function pointer that handles the transmission of passthrough data.
 * @param buffer A pointer to the buffer containing the data to transmit.
 * @param size The size of the data to transmit.
 * @param priv A pointer to private data associated with the transmission.
 * @return An integer value indicating the result of the transmission.
 */
typedef int (*wm_atcmd_tx_passthrough)(void *buffer, int size, void *priv);

/**
 * @brief Passthrough exit notification function pointer type definition.
 *
 * This type definition represents a function pointer that is notified when exiting passthrough mode.
 * @param reason The reason for exiting passthrough mode.
 * @param priv A pointer to private data associated with the notification.
 * @return An integer value indicating the result of the notification.
 */
typedef int (*wm_atcmd_tx_passthrough_exit_notify)(wm_atcmd_exit_passthrough_reason_e reason, void *priv);

/**
 * @}
 */

/**
 * @defgroup WM_ATCMD_Structures WM AT Command Structures
 * @brief WinnerMicro AT Command Structures
 */

/**
 * @addtogroup WM_ATCMD_Structures
 * @{
 */

/**
 * @brief AT command result structure.
 *
 * This structure contains the result of an AT command, including the result type and a string message.
 */
typedef struct wm_atcmd_result_str {
    wm_at_result_e result_type; /**< Type of the result */
    char *result_str;           /**< String message associated with the result */
} wm_atcmd_result_str_t;

/**
 * @brief AT command item structure.
 *
 * This structure defines an individual AT command, including its name, handler, and usage description.
 */
typedef struct wm_atcmd_item {
    char *cmd_name;          /**< Name of the AT command */
    wm_atcmd_act cmd_handle; /**< Function to handle the AT command */
    char *cmd_usg;           /**< Usage description of the AT command */
} wm_atcmd_item_t;

/**
 * @brief Passthrough parameter structure.
 *
 * This structure contains parameters related to passthrough mode, including timeout, handlers, and buffers.
 */
typedef struct wm_at_passthrough_param {
    char is_passthrough;                             /**< Flag indicating if in passthrough mode */
    uint32_t timeout;                                /**< Timeout for passthrough mode */
    uint32_t time;                                   /**< Timestamp for passthrough mode entry */
    wm_atcmd_tx_passthrough tx_handle;               /**< Function to handle passthrough data transmission */
    void *priv;                                      /**< Private data for passthrough handling */
    wm_atcmd_tx_passthrough_exit_notify exit_notify; /**< Notification function when exiting passthrough mode */
    void *pt_exit_priv;                              /**< Private data for exit notification */
    char *recv_buffer;                               /**< Buffer for receiving passthrough data */
    short recv_buffer_len;                           /**< Length of the receive buffer */
} wm_at_passthrough_param_t;

/**
 * @brief AT command server control structure.
 *
 * This structure contains control information for the AT command server, including tasks, queues, and device parameters.
 */
typedef struct wm_at_server_ctl {
    wm_os_task_t at_comm_recv_task; /**< Task for receiving AT commands */
    wm_os_task_t at_comm_proc_task; /**< Task for processing AT commands */
    wm_os_queue_t *queue;           /**< Queue for managing AT command messages */
    wm_os_timer_t *timer;           /**< Timer for message processing delay */
#if CONFIG_WM_ATCMD_SHOW_REMAIN_HEAP
    wm_os_timer_t *heap_timer; /**< Timer for showing remaining heap */
#endif
    wm_atcmd_dev_t dev;   /**< Device for AT commands */
    wm_os_mutex_t *mutex; /**< Mutex for synchronizing AT command access */
    short atcmd_num;      /**< Number of AT commands */
    bool echo_mode;
    wm_atcmd_item_t *at_cmd_table;               /**< Table of AT command items */
    uint32_t errno;                              /**< Error number for AT command processing */
    wm_at_passthrough_param_t passthrough_param; /**< Parameters for passthrough mode */
    char recv_buffer[AT_CMD_RECV_BUFF_LEN];      /**< Buffer for receiving AT command data */
    short cur_recv_len;                          /**< Current length of received data */
    uint16_t at_recv_listnum;                    /**< Number of received AT commands */
    char start_str[AT_CMD_START_STR_LEN];        /**< Start string for AT commands */
    char end_str[AT_CMD_END_STR_LEN];            /**< End string for AT commands */
    wm_atcmd_entry atcmd_enter;                  /**< Entry function for AT commands */
#if CONFIG_WM_ATCMD_SYS_ENABLE
    void *wm_atcmd_sys_param; /**< Parameters for SYS AT commands */
#endif
#if CONFIG_WM_ATCMD_WIFI_ENABLE
    void *wm_atcmd_wifi_param; /**< Parameters for Wi-Fi AT commands */
#endif
#if CONFIG_WM_ATCMD_TCP_ENABLE
    void *wm_atcmd_tcpip_param; /**< Parameters for TCP/IP AT commands */
#endif
#if CONFIG_WM_ATCMD_MQTT_ENABLE
    void *wm_at_mqtt_param; /**< Parameters for MQTT AT commands */
#endif
#if CONFIG_WM_ATCMD_HTTP_ENABLE
    void *wm_atcmd_http_param; /**< Parameters for HTTP AT commands */
#endif
#if CONFIG_WM_ATCMD_BT_ENABLE
    void *wm_atcmd_bt_param; /**< Parameters for Bluetooth AT commands */
#endif
} wm_at_server_ctl_t;

/**
 * @}
 */

/**
 * @defgroup WM_ATCMD_Macros WM AT Command Macros
 * @brief WinnerMicro AT Command Macros
 */

/**
 * @addtogroup WM_ATCMD_Macros
 * @{
 */

#define WM_ATCMD_SECTION ".wm_at_table"

#if CONFIG_WM_ATCMD_USAGE_ENABLE
#define WM_ATCMD_DEFINE(cmd, func, usage)                                                                 \
    static const struct wm_atcmd_item __atcmd_##func __attribute__((used, section(WM_ATCMD_SECTION))) = { \
        .cmd_name   = cmd,                                                                                \
        .cmd_handle = func,                                                                               \
        .cmd_usg    = usage,                                                                              \
    };
#else /* CONFIG_WM_ATCMD_USAGE_ENABLE */
#define WM_ATCMD_DEFINE(cmd, func, usage)                                                                 \
    static const struct wm_atcmd_item __atcmd_##func __attribute__((used, section(WM_ATCMD_SECTION))) = { \
        .cmd_name   = cmd,                                                                                \
        .cmd_handle = func,                                                                               \
        .cmd_usg    = NULL,                                                                               \
    };
#endif /* CONFIG_WM_ATCMD_USAGE_ENABLE */

/**
 * @}
 */

/**
 * @defgroup WM_ATCMD_Functions WM AT Command Functions
 * @brief WinnerMicro AT Command Functions
 */

/**
 * @addtogroup WM_ATCMD_Functions
 * @{
 */

/**
  * @brief     init atcmd model
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_atcmd_init(void);

/**
  * @brief         register atcmd dev ops
  *
  * @param[in]     dev_type  device type
  * @param[in]     ops       device ops func
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_atcmd_dev_register(wm_atcmd_dev_type_e dev_type, wm_dev_ops_t ops);

/**
  * @brief         print result, max size is defined as "AT_RESULT_BUFFER_SIZE"
  *
  * @param[in]     format  format str
  *
  * @return        None
  */
void wm_atcmd_print_result(char *format, ...);

/**
  * @brief         print raw data
  *
  * @param[in]     data raw data buffer
  * @param[in]     len  raw data len
  *
  * @return        None
  */
void wm_atcmd_print_raw_data(const void *data, int len);

/**
  * @brief         enter passthrough mode
  *
  * @param[in]     pt_tx_handle  passthrough tx func
  * @param[in]     priv          user private data
  * @param[in]     pt_exit_notify passthrough exit notify func
  * @param[in]     timeout       passthrough timeout
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_atcmd_enter_passthrough(wm_atcmd_tx_passthrough pt_tx_handle, wm_atcmd_tx_passthrough_exit_notify pt_exit_notify,
                               void *priv, uint32_t timeout);

/**
  * @brief         exit passthrough mode
  *
  * @param[in]     reason  exit reason
  *
  * @return        None
  */
void wm_atcmd_exit_passthrough(wm_atcmd_exit_passthrough_reason_e reason);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //__WM_ATCMD_H__
