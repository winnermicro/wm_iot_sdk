#ifndef __WM_MQTT_CLIENT_H__
#define __WM_MQTT_CLIENT_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_MQTT_CLIENT_DEFs WM MQTT Client Defines
 * @brief WinnerMicro MQTT Client Defines
 */

/**
 * @addtogroup WM_MQTT_CLIENT_DEFs
 * @{
 */

WM_EVENT_DECLARE_GROUP(WM_MQTT_EV); /**< Declare mqtt event group          */

enum wm_event_mqtt_client {
    WM_EVENT_MQTT_CLIENT_CONNECTED,    /*!< mqtt client connected event       */
    WM_EVENT_MQTT_CLIENT_DISCONNECTED, /*!< mqtt client disconnected event    */
    WM_EVENT_MQTT_CLIENT_DATA,         /*!< mqtt client data event            */
    WM_EVENT_MQTT_CLIENT_PUBLISHED,    /*!< mqtt client published event       */
};

typedef enum wm_mqtt_client_auth_mode_en {
    WM_MQTT_CLIENT_AUTH_MODE_NOT_CERT            = 0, /**< No certificate validation. */
    WM_MQTT_CLIENT_AUTH_MODE_AUTH_SERVER_CERT    = 1, /**< Verify the server certificate. */
    WM_MQTT_CLIENT_AUTH_MODE_PROVIDE_CLIENT_CERT = 2, /**< Verify the server certificate. */
    WM_MQTT_CLIENT_AUTH_MODE_MUTUAL_AUTH              /**< Verify the server certificate. */
} wm_mqtt_client_auth_mode_t;

typedef enum wm_mqtt_client_error_code_en {
    WM_MQTT_CLIENT_ERROR_SUCCESS =
        WM_ERR_SUCCESS, /**< No error. Indicates successful completion of an MQTT client operation. */
    WM_MQTT_CLIENT_ERROR_FAILURE =
        (WM_ERR_MQTT_CLIENT_BASE - 1), /**< A generic error code indicating the failure of an MQTT client operation. */
    WM_MQTT_CLIENT_ERROR_PERSISTENCE_ERROR = (WM_ERR_MQTT_CLIENT_BASE - 2), /**< Application-specific persistence error. */
    WM_MQTT_CLIENT_ERROR_DISCONNECTED      = (WM_ERR_MQTT_CLIENT_BASE - 3), /**< The client is disconnected. */
    WM_MQTT_CLIENT_ERROR_MAX_MESSAGES_INFLIGHT =
        (WM_ERR_MQTT_CLIENT_BASE -
         4), /**< The maximum number of messages allowed to be simultaneously in-flight has been reached. */
    WM_MQTT_CLIENT_ERROR_BAD_UTF8_STRING = (WM_ERR_MQTT_CLIENT_BASE - 5), /**< An invalid UTF-8 string has been detected. */
    WM_MQTT_CLIENT_ERROR_NULL_PARAMETER =
        (WM_ERR_MQTT_CLIENT_BASE - 6), /**< A NULL parameter has been supplied when this is invalid. */
    WM_MQTT_CLIENT_ERROR_TOPICNAME_TRUNCATED =
        (WM_ERR_MQTT_CLIENT_BASE -
         7), /**< The topic has been truncated (the topic string includes embedded NULL characters). */
    WM_MQTT_CLIENT_ERROR_BAD_STRUCTURE =
        (WM_ERR_MQTT_CLIENT_BASE - 8), /**< A structure parameter does not have the correct eyecatcher and version number. */
    WM_MQTT_CLIENT_ERROR_BAD_QOS =
        (WM_ERR_MQTT_CLIENT_BASE - 9), /**< A QoS value that falls outside of the acceptable range (0,1,2). */
    WM_MQTT_CLIENT_ERROR_SSL_NOT_SUPPORTED =
        (WM_ERR_MQTT_CLIENT_BASE - 10), /**< Attempting SSL connection using non-SSL version of library. */
    WM_MQTT_CLIENT_ERROR_BAD_MQTT_VERSION = (WM_ERR_MQTT_CLIENT_BASE - 11), /**< unrecognized MQTT version. */
    WM_MQTT_CLIENT_ERROR_BAD_PROTOCOL     = (WM_ERR_MQTT_CLIENT_BASE - 14), /**< protocol prefix invalid. */
    WM_MQTT_CLIENT_ERROR_BAD_MQTT_OPTION =
        (WM_ERR_MQTT_CLIENT_BASE - 15), /**< option not applicable to the requested version of MQTT. */
    WM_MQTT_CLIENT_ERROR_WRONG_MQTT_VERSION =
        (WM_ERR_MQTT_CLIENT_BASE - 16), /**< call not applicable to the requested version of MQTT. */
    WM_MQTT_CLIENT_ERROR_0_LEN_WILL_TOPIC = (WM_ERR_MQTT_CLIENT_BASE - 17) /**< 0 length will topic on connect. */
} wm_mqtt_client_error_code_t;

typedef union wm_mqtt_event_data_un {
    struct {
        char *topic;
        uint16_t topic_len;
        char *payload;
        uint16_t payload_len;
        int msgid;
        uint8_t qos;
        uint8_t dup;
        uint8_t retained;
        void *client_handle; /**< same as wm_mqtt_client_handle_t->client */
    } mqtt_client_data_info;

    struct {
        int msg_id;
    } mqtt_client_published_info;

    struct {
        void *client_handle; /**< same as wm_mqtt_client_handle_t->client */
    } mqtt_client_disconnect_info;

} wm_mqtt_event_data_t;

typedef struct wm_mqtt_client_config_st {
    char *uri;

    char *client_id;
    char *username;
    char *password;

    wm_mqtt_client_auth_mode_t auth_mode;

    char *ca;
    char *cert;
    char *key;
    char **alpn;

    char *will_topic;
    char *will_msg;
    uint8_t will_qos;
    uint8_t will_retain;

    uint16_t keep_alive;

    uint8_t clean_session;

    bool debug;
} wm_mqtt_client_config_t;

typedef struct wm_mqtt_client_handle_st {
    void *client;
    void *config;

    char *uri;

    char *client_id;
    char *username;
    char *password;

    char *ca;
    char *cert;
    char *key;
    char **alpn;

    char *will_topic;
    char *will_msg;
} *wm_mqtt_client_handle_t;

/**
 * @}
 */

/**
 * @defgroup WM_MQTT_CLIENT_APIs WM MQTT CLIENT APIs
 * @brief WinnerMicro MQTT Client APIs
 */

/**
 * @addtogroup WM_MQTT_CLIENT_APIs
 * @{
 */

/**
 * @brief Creates *MQTT* client handle based on the configuration
 *
 * @param config    *MQTT* configuration structure
 * Notes:
 *     protocol prefix in serverURI should be:
 *     @li @em tcp:// or mqtt://  - Insecure TCP
 *     @li @em ssl:// or mqtts:// - Encrypted SSL/TLS
 *     @li @em ws://              - Insecure websockets
 *     @li @em wss://             - Secure web sockets
 *
 * @return wm_mqtt_client_handle_t if successfully created, WM_INVALID_HANDLE on error
 */
wm_mqtt_client_handle_t wm_mqtt_client_init(wm_mqtt_client_config_t *config);

/**
 * @brief Destroys the client handle
 *
 * Notes:
 *   If the mqtt client is already connected,
 *   this call to the API may produce WM_EVENT_MQTT_CLIENT_DISCONNECTED events
 *
 * @param handle    *MQTT* client handle
 *
 * @return WM_MQTT_CLIENT_ERROR_SUCCESS on success
 *         others on other error
 */
int wm_mqtt_client_deinit(wm_mqtt_client_handle_t handle);

/**
 * @brief Starts *MQTT* client with already created client handle
 *
 * Notes:
 * - 1. The API creates an mqtt client task
 *      that is not currently supported to be deleted when exiting the mqtt client.
 *      Even if there are multiple mqtt clients, only one mqtt task will be created
 * - 2. If the mqtt client is already connected,
 *      this call to the API may produce WM_EVENT_MQTT_CLIENT_CONNECTED events
 *
 * @param handle    *MQTT* client handle
 *
 * @return WM_MQTT_CLIENT_ERROR_SUCCESS on success
 *         others on other error
 */
int wm_mqtt_client_connect(wm_mqtt_client_handle_t handle);

/**
 * @brief Stops *MQTT* client
 *
 * Notes:
 *   If the mqtt client is already connected,
 *   this call to the API may produce WM_EVENT_MQTT_CLIENT_DISCONNECTED events
 *
 * @param handle    *MQTT* client handle
 *
 * @return WM_MQTT_CLIENT_ERROR_SUCCESS on success
 *         others on other error
 */
int wm_mqtt_client_disconnect(wm_mqtt_client_handle_t handle);

/**
 * @brief This api is typically used to force reconnection upon a specific event
 *
 * @param handle    *MQTT* client handle
 *
 * @return WM_MQTT_CLIENT_ERROR_SUCCESS on success
 *         others on other error
 */
int wm_mqtt_client_reconnect(wm_mqtt_client_handle_t handle);

/**
 * @brief Subscribe the client to defined topic with defined qos
 *
 * Notes:
 * - 1. Client must be connected to send subscribe message
 * - 2. This call to the API may produce WM_EVENT_MQTT_CLIENT_PUBLISHED events
 *
 * @param handle    *MQTT* client handle
 * @param topic     topic string
 * @param qos       QoS of subscribe message
 *
 * @return message_id of the subscribe message on success
 *         <0 on failure
 */
int wm_mqtt_client_subscribe(wm_mqtt_client_handle_t handle, const char *topic, uint8_t qos);

/**
 * @brief Unsubscribe the client from defined topic
 *
 * Notes:
 * - 1. Client must be connected to send unsubscribe message
 * - 2. This call to the API may produce WM_EVENT_MQTT_CLIENT_PUBLISHED events
 *
 * @param handle    *MQTT* client handle
 * @param topic     topic string
 *
 * @return message_id of the subscribe message on success
 *         <0 on failure
 */
int wm_mqtt_client_unsubscribe(wm_mqtt_client_handle_t handle, const char *topic);

/**
 * @brief Client to send a publish message to the broker
 *
 * Notes:
 * - 1. This API might block for several seconds, either due to network timeout(10s)
 * - 2. This call to the API may produce WM_EVENT_MQTT_CLIENT_PUBLISHED events
 *
 * @param handle    *MQTT* client handle
 * @param topic     topic string
 * @param message   payload string (set to NULL, sending empty payload message)
 * @param msg_len   payload length, if set to 0, length is calculated from payload
 * string
 * @param qos       QoS of publish message
 * @param retain    retain flag
 *
 * @return message_id of the publish message (for QoS 0 message_id will always
 * be zero) on success. -1 on failure.
 */
int wm_mqtt_client_publish(wm_mqtt_client_handle_t handle, const char *topic, const char *message, uint16_t msg_len,
                           uint8_t qos, uint8_t retain);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //__WM_MQTT_CLIENT_H__
