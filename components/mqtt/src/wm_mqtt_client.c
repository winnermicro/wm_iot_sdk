#include "wmsdk_config.h"
#include "wm_task_config.h"
#include <stdio.h>
#include <string.h>
#include "wm_nvs.h"
#include "wm_key_config.h"
#include "wm_utils.h"
#include "wm_event.h"
#include "SSLSocket.h"
#include "MQTTClient.h"
#include "MQTTAsync.h"
#include "MQTTAsyncUtils.h"
#include "MQTTClientPersistence.h"
#include "wm_mqtt_client.h"

#define LOG_TAG "mqtt"
#include "wm_log.h"

#define DISCONNECT_TIMEOUT 10 * 1000

#define URI_MQTT           "mqtt://"
#define URI_MQTTS          "mqtts://"

WM_EVENT_DEFINE_GROUP(WM_MQTT_EV); /**< Define mqtt event group          */

extern const char mqtt_client_tls_ca[];
extern const char mqtt_client_tls_cert[];
extern const char mqtt_client_tls_key[];

static char **string_array_dup(const char **src)
{
    char **dst   = NULL;
    size_t count = 0;
    int i;
    const char **p;

    if (!src)
        return NULL;

    for (p = src; *p != NULL; p++)
        count++;

    dst = wm_os_internal_malloc(sizeof(char *) * count + 1);
    if (!dst)
        return NULL;

    for (i = 0; i < count; i++) {
        dst[i] = strdup(src[i]);
        if (!dst[i]) {
            for (--i; i >= 0; i--)
                wm_os_internal_free((void *)dst[i]);
            wm_os_internal_free((void *)dst);
            return NULL;
        }
    }

    dst[i] = NULL;
    return dst;
}

static void string_array_free(const char **src)
{
    size_t count = 0;
    int i;
    const char **p;

    if (!src)
        return;

    for (p = src; *p != NULL; p++) {
        count++;
    }

    for (i = 0; i < count; i++) {
        if (src[i])
            wm_os_internal_free((void *)src[i]);
    }

    wm_os_internal_free((void *)src);
    return;
}

static char *uri_dup(const char *uri)
{
    char *pos = NULL;
    char *pfx = NULL;
    char *ret = NULL;

    if (!strncmp(uri, URI_MQTT, strlen(URI_MQTT)) || !strncmp(uri, URI_MQTTS, strlen(URI_MQTTS))) {
        pos = strstr(uri, URI_MQTTS);
        if (pos) {
            pfx = URI_SSL;
            pos += strlen(URI_MQTTS);
        } else {
            pos = strstr(uri, URI_MQTT);
            pfx = URI_TCP;
            pos += strlen(URI_MQTT);
        }

        ret = wm_os_internal_malloc(strlen(pfx) + strlen(pos) + 1);
        if (!ret) {
            return NULL;
        }

        memcpy(ret, pfx, strlen(pfx));
        memcpy(ret + strlen(pfx), pos, strlen(pos));
        ret[strlen(pfx) + strlen(pos)] = '\0';
    } else {
        ret = strdup(uri);
    }

    return ret;
}

static void mqtt_client_cleanup_handle(wm_mqtt_client_handle_t handle)
{
    if (!handle)
        return;

    if (handle->alpn)
        string_array_free((const char **)handle->alpn);
    if (handle->key)
        wm_os_internal_free(handle->key);
    if (handle->cert)
        wm_os_internal_free(handle->cert);
    if (handle->ca)
        wm_os_internal_free(handle->ca);
    if (handle->will_msg)
        wm_os_internal_free(handle->will_msg);
    if (handle->will_topic)
        wm_os_internal_free(handle->will_topic);
    if (handle->password)
        wm_os_internal_free(handle->password);
    if (handle->username)
        wm_os_internal_free(handle->username);
    if (handle->client_id)
        wm_os_internal_free(handle->client_id);
    if (handle->uri)
        wm_os_internal_free(handle->uri);
    wm_os_internal_free(handle);
}

static void pahomqtt_delivered(void *context, MQTTClient_deliveryToken dt)
{
    wm_mqtt_event_data_t data;

    wm_log_debug("Message with token value %d delivery confirmed", dt);

    data.mqtt_client_published_info.msg_id = (int)dt;
    wm_event_send(WM_MQTT_EV, WM_EVENT_MQTT_CLIENT_PUBLISHED, &data, sizeof(data));
}

static int pahomqtt_msg_arrived(void *context, char *topic, int topic_len, MQTTClient_message *message)
{
    wm_mqtt_event_data_t data = { 0 };

    wm_log_debug("Message arrived, QoS [%d], topic [%s], payload_length [%d]", message->qos, topic, message->payloadlen);

    if (message->payloadlen) {
        data.mqtt_client_data_info.payload = malloc(message->payloadlen);
        if (data.mqtt_client_data_info.payload == NULL) {
            wm_log_error("OOM, %s message drop!", topic);
            goto exit;
        }
        memcpy(data.mqtt_client_data_info.payload, message->payload, message->payloadlen);
        data.mqtt_client_data_info.payload_len = message->payloadlen;
    }
    data.mqtt_client_data_info.topic         = topic != NULL ? strdup(topic) : NULL;
    data.mqtt_client_data_info.topic_len     = topic_len ? topic_len : strlen(topic);
    data.mqtt_client_data_info.msgid         = message->msgid;
    data.mqtt_client_data_info.qos           = message->qos;
    data.mqtt_client_data_info.dup           = message->dup;
    data.mqtt_client_data_info.retained      = message->retained;
    data.mqtt_client_data_info.client_handle = context;
    wm_event_send(WM_MQTT_EV, WM_EVENT_MQTT_CLIENT_DATA, &data, sizeof(wm_mqtt_event_data_t));

exit:
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic);
    return 1;
}

static void pahomqtt_conn_lost(void *context, char *cause)
{
    wm_mqtt_event_data_t data;
    wm_log_info("Connection lost, cause: %s", cause ? cause : "");

    data.mqtt_client_disconnect_info.client_handle = context;
    wm_event_send(WM_MQTT_EV, WM_EVENT_MQTT_CLIENT_DISCONNECTED, &data, sizeof(data));
}

static void pahomqtt_log_trace_callback(enum MQTTCLIENT_TRACE_LEVELS level, char *message)
{
    wm_log_debug("%s", message);
}

static int wm_mqtt_client_init_internal(wm_mqtt_client_handle_t handle, MQTTClient *client)
{
    int rc;

    if ((rc = MQTTClient_create(client, (const char *)handle->uri, (const char *)handle->client_id, MQTTCLIENT_PERSISTENCE_NONE,
                                NULL)) != MQTTCLIENT_SUCCESS) {
        wm_log_debug("Failed to create client, return code %d", rc);

        return WM_ERR_MQTT_CLIENT_BASE + rc; //WM_ERR_FAILED;
    }

    if ((rc = MQTTClient_setCallbacks(*client, *client, pahomqtt_conn_lost, pahomqtt_msg_arrived, pahomqtt_delivered)) !=
        MQTTCLIENT_SUCCESS) {
        wm_log_debug("Failed to set callbacks, return code %d", rc);
        return WM_ERR_MQTT_CLIENT_BASE + rc; //WM_ERR_FAILED;
    }

    return WM_MQTT_CLIENT_ERROR_SUCCESS;
}

static int wm_mqtt_client_deinit_internal(wm_mqtt_client_handle_t handle)
{
    int ret;
    uint8_t need_event = false;

    if (!handle)
        return WM_ERR_INVALID_PARAM;

    if (!handle->client || !handle->config)
        return WM_ERR_INVALID_PARAM;

    if (MQTTClient_isConnected((MQTTClient)handle->client))
        need_event = true;

    ret = MQTTClient_disconnect((MQTTClient)handle->client, DISCONNECT_TIMEOUT);
    if (ret != MQTTCLIENT_SUCCESS) {
        wm_log_debug("Failed to disconnect, return code %d", ret);
    }

    wm_log_debug("mqtt client destroy");
    MQTTClient_destroy((MQTTClient *)&handle->client);

    if (need_event)
        wm_event_send(WM_MQTT_EV, WM_EVENT_MQTT_CLIENT_DISCONNECTED, NULL, 0);

    return WM_MQTT_CLIENT_ERROR_SUCCESS;
}

wm_mqtt_client_handle_t wm_mqtt_client_init(wm_mqtt_client_config_t *config)
{
    MQTTClient client = NULL;
    wm_mqtt_client_handle_t handle;
    MQTTClient_connectOptions *conn_opt = NULL;
    MQTTClient_SSLOptions *ssl_opt      = NULL;
    MQTTClient_willOptions *will_opt    = NULL;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts      = MQTTClient_SSLOptions_initializer;
    MQTTClient_willOptions will_opts    = MQTTClient_willOptions_initializer;
    char *mqtt_client_id                = config->client_id;
    char client_id[32];
    int rc;
    int ret;

    if (config == NULL || config->uri == NULL) {
        return WM_INVALID_HANDLE;
    }

    handle = wm_os_internal_malloc(sizeof(*handle));
    if (!handle)
        return WM_INVALID_HANDLE;

    conn_opt = wm_os_internal_malloc(sizeof(*conn_opt));
    if (!conn_opt) {
        wm_os_internal_free(handle);
        return WM_INVALID_HANDLE;
    }

    if (!strncmp(config->uri, URI_SSL, strlen(URI_SSL)) || !strncmp(config->uri, URI_WSS, strlen(URI_WSS)) ||
        !strncmp(config->uri, URI_MQTTS, strlen(URI_MQTTS))) {
        ssl_opt = wm_os_internal_malloc(sizeof(*ssl_opt));
        if (!ssl_opt) {
            wm_os_internal_free(conn_opt);
            wm_os_internal_free(handle);
            return WM_INVALID_HANDLE;
        }
        memset(ssl_opt, 0, sizeof(*ssl_opt));
    }

    if (config->will_topic != NULL) {
        will_opt = wm_os_internal_malloc(sizeof(*will_opt));
        if (!will_opt) {
            if (ssl_opt)
                wm_os_internal_free(ssl_opt);
            wm_os_internal_free(conn_opt);
            wm_os_internal_free(handle);
            return WM_INVALID_HANDLE;
        }
        memset(will_opt, 0, sizeof(*will_opt));
    }

    memset(handle, 0, sizeof(*handle));
    memset(conn_opt, 0, sizeof(*conn_opt));

    handle->uri = uri_dup(config->uri);
    if (!handle->uri) {
        if (will_opt)
            wm_os_internal_free(will_opt);
        if (ssl_opt)
            wm_os_internal_free(ssl_opt);
        wm_os_internal_free(conn_opt);
        wm_os_internal_free(handle);
        return WM_INVALID_HANDLE;
    }

    wm_log_debug("uri: %s", handle->uri);

    if (!mqtt_client_id) {
        uint8_t mac[WM_MAC_ADDR_LEN];

        ret = wm_sys_get_mac_addr(WM_MAC_TYPE_STA, mac, sizeof(mac));
        if (ret != WM_ERR_SUCCESS) {
            wm_os_internal_free(handle->uri);
            if (will_opt)
                wm_os_internal_free(will_opt);
            if (ssl_opt)
                wm_os_internal_free(ssl_opt);
            wm_os_internal_free(conn_opt);
            wm_os_internal_free(handle);
            return WM_INVALID_HANDLE;
        }
        sprintf(client_id, "%s_%02x%02X%02X", CONFIG_CHIP_NAME, mac[3], mac[4], mac[5]);
        mqtt_client_id = &client_id[0];
    }

    handle->client_id = strdup(mqtt_client_id);
    if (!handle->client_id) {
        wm_os_internal_free(handle->uri);
        if (will_opt)
            wm_os_internal_free(will_opt);
        if (ssl_opt)
            wm_os_internal_free(ssl_opt);
        wm_os_internal_free(conn_opt);
        wm_os_internal_free(handle);
        return WM_INVALID_HANDLE;
    }

    if (config->debug) {
        MQTTClient_setTraceLevel(MQTTCLIENT_TRACE_MAXIMUM);
    } else {
        MQTTClient_setTraceLevel(MQTTCLIENT_TRACE_ERROR);
    }
    MQTTClient_setTraceCallback(pahomqtt_log_trace_callback);

    if (config->keep_alive == 0) {
        conn_opts.keepAliveInterval = 60;
    } else {
        conn_opts.keepAliveInterval = config->keep_alive;
    }

    conn_opts.cleansession = config->clean_session;

    if (config->username != NULL) {
        wm_log_debug("username: %s, password: %s", config->username, config->password);

        handle->username = strdup(config->username);
        if (!handle->username) {
            wm_os_internal_free(handle->client_id);
            wm_os_internal_free(handle->uri);
            if (will_opt)
                wm_os_internal_free(will_opt);
            if (ssl_opt)
                wm_os_internal_free(ssl_opt);
            wm_os_internal_free(conn_opt);
            wm_os_internal_free(handle);
            return WM_INVALID_HANDLE;
        }

        handle->password = strdup(config->password);
        if (!handle->password) {
            wm_os_internal_free(handle->username);
            wm_os_internal_free(handle->client_id);
            wm_os_internal_free(handle->uri);
            if (will_opt)
                wm_os_internal_free(will_opt);
            if (ssl_opt)
                wm_os_internal_free(ssl_opt);
            wm_os_internal_free(conn_opt);
            wm_os_internal_free(handle);
            return WM_INVALID_HANDLE;
        }

        conn_opts.username = handle->username;
        conn_opts.password = handle->password;
    }

    memcpy(conn_opt, &conn_opts, sizeof(*conn_opt));

    if (config->will_topic != NULL) {
        handle->will_topic = strdup(config->will_topic);
        if (!handle->will_topic) {
            if (handle->password)
                wm_os_internal_free(handle->password);
            if (handle->username)
                wm_os_internal_free(handle->username);
            wm_os_internal_free(handle->client_id);
            wm_os_internal_free(handle->uri);
            wm_os_internal_free(will_opt);
            if (ssl_opt)
                wm_os_internal_free(ssl_opt);
            wm_os_internal_free(conn_opt);
            wm_os_internal_free(handle);
            return WM_INVALID_HANDLE;
        }

        handle->will_msg = strdup(config->will_msg);
        if (!handle->will_msg) {
            wm_os_internal_free(handle->will_topic);
            if (handle->password)
                wm_os_internal_free(handle->password);
            if (handle->username)
                wm_os_internal_free(handle->username);
            wm_os_internal_free(handle->client_id);
            wm_os_internal_free(handle->uri);
            wm_os_internal_free(will_opt);
            if (ssl_opt)
                wm_os_internal_free(ssl_opt);
            wm_os_internal_free(conn_opt);
            wm_os_internal_free(handle);
            return WM_INVALID_HANDLE;
        }

        will_opts.topicName = handle->will_topic;
        will_opts.message   = handle->will_msg;
        will_opts.qos       = config->will_qos;
        will_opts.retained  = config->will_retain;
        //conn_opts.will = &will_opts;

        memcpy(will_opt, &will_opts, sizeof(*will_opt));
        conn_opt->will = will_opt;
    }

    if (!strncmp(handle->uri, URI_SSL, strlen(URI_SSL)) || !strncmp(handle->uri, URI_WSS, strlen(URI_WSS))) {
        wm_log_debug("mqtt_config mode: %d", config->auth_mode);

        if (config->auth_mode == WM_MQTT_CLIENT_AUTH_MODE_AUTH_SERVER_CERT) {
            if (config->ca == NULL) {
                ssl_opts.trustStore = mqtt_client_tls_ca;
            } else {
                handle->ca = strdup(config->ca);
                if (!handle->ca) {
                    if (handle->will_msg)
                        wm_os_internal_free(handle->will_msg);
                    if (handle->will_topic)
                        wm_os_internal_free(handle->will_topic);
                    if (handle->password)
                        wm_os_internal_free(handle->password);
                    if (handle->username)
                        wm_os_internal_free(handle->username);
                    wm_os_internal_free(handle->client_id);
                    wm_os_internal_free(handle->uri);
                    if (will_opt)
                        wm_os_internal_free(will_opt);
                    wm_os_internal_free(ssl_opt);
                    wm_os_internal_free(conn_opt);
                    wm_os_internal_free(handle);
                    return WM_INVALID_HANDLE;
                }

                ssl_opts.trustStore = handle->ca;
            }
            ssl_opts.enableServerCertAuth = true;
        } else if (config->auth_mode == WM_MQTT_CLIENT_AUTH_MODE_PROVIDE_CLIENT_CERT) {
            if (config->key == NULL || config->cert == NULL) {
                ssl_opts.keyStore   = mqtt_client_tls_cert;
                ssl_opts.privateKey = mqtt_client_tls_key;
            } else {
                handle->cert = strdup(config->cert);
                if (!handle->cert) {
                    if (handle->ca)
                        wm_os_internal_free(handle->ca);
                    if (handle->will_msg)
                        wm_os_internal_free(handle->will_msg);
                    if (handle->will_topic)
                        wm_os_internal_free(handle->will_topic);
                    if (handle->password)
                        wm_os_internal_free(handle->password);
                    if (handle->username)
                        wm_os_internal_free(handle->username);
                    wm_os_internal_free(handle->client_id);
                    wm_os_internal_free(handle->uri);
                    if (will_opt)
                        wm_os_internal_free(will_opt);
                    wm_os_internal_free(ssl_opt);
                    wm_os_internal_free(conn_opt);
                    wm_os_internal_free(handle);
                    return WM_INVALID_HANDLE;
                }

                handle->key = strdup(config->key);
                if (!handle->key) {
                    if (handle->cert)
                        wm_os_internal_free(handle->cert);
                    if (handle->ca)
                        wm_os_internal_free(handle->ca);
                    if (handle->will_msg)
                        wm_os_internal_free(handle->will_msg);
                    if (handle->will_topic)
                        wm_os_internal_free(handle->will_topic);
                    if (handle->password)
                        wm_os_internal_free(handle->password);
                    if (handle->username)
                        wm_os_internal_free(handle->username);
                    wm_os_internal_free(handle->client_id);
                    wm_os_internal_free(handle->uri);
                    if (will_opt)
                        wm_os_internal_free(will_opt);
                    wm_os_internal_free(ssl_opt);
                    wm_os_internal_free(conn_opt);
                    wm_os_internal_free(handle);
                    return WM_INVALID_HANDLE;
                }

                ssl_opts.keyStore   = handle->cert;
                ssl_opts.privateKey = handle->key;
            }
        } else if (config->auth_mode == WM_MQTT_CLIENT_AUTH_MODE_MUTUAL_AUTH) {
            if (config->ca == NULL || config->cert == NULL || config->key == NULL) {
                ssl_opts.trustStore = mqtt_client_tls_ca;
                ssl_opts.keyStore   = mqtt_client_tls_cert;
                ssl_opts.privateKey = mqtt_client_tls_key;
            } else {
                handle->ca = strdup(config->ca);
                if (!handle->ca) {
                    if (handle->will_msg)
                        wm_os_internal_free(handle->will_msg);
                    if (handle->will_topic)
                        wm_os_internal_free(handle->will_topic);
                    if (handle->password)
                        wm_os_internal_free(handle->password);
                    if (handle->username)
                        wm_os_internal_free(handle->username);
                    wm_os_internal_free(handle->client_id);
                    wm_os_internal_free(handle->uri);
                    if (will_opt)
                        wm_os_internal_free(will_opt);
                    wm_os_internal_free(ssl_opt);
                    wm_os_internal_free(conn_opt);
                    wm_os_internal_free(handle);
                    return WM_INVALID_HANDLE;
                }

                handle->cert = strdup(config->cert);
                if (!handle->cert) {
                    if (handle->ca)
                        wm_os_internal_free(handle->ca);
                    if (handle->will_msg)
                        wm_os_internal_free(handle->will_msg);
                    if (handle->will_topic)
                        wm_os_internal_free(handle->will_topic);
                    if (handle->password)
                        wm_os_internal_free(handle->password);
                    if (handle->username)
                        wm_os_internal_free(handle->username);
                    wm_os_internal_free(handle->client_id);
                    wm_os_internal_free(handle->uri);
                    if (will_opt)
                        wm_os_internal_free(will_opt);
                    wm_os_internal_free(ssl_opt);
                    wm_os_internal_free(conn_opt);
                    wm_os_internal_free(handle);
                    return WM_INVALID_HANDLE;
                }

                handle->key = strdup(config->key);
                if (!handle->key) {
                    if (handle->cert)
                        wm_os_internal_free(handle->cert);
                    if (handle->ca)
                        wm_os_internal_free(handle->ca);
                    if (handle->will_msg)
                        wm_os_internal_free(handle->will_msg);
                    if (handle->will_topic)
                        wm_os_internal_free(handle->will_topic);
                    if (handle->password)
                        wm_os_internal_free(handle->password);
                    if (handle->username)
                        wm_os_internal_free(handle->username);
                    wm_os_internal_free(handle->client_id);
                    wm_os_internal_free(handle->uri);
                    if (will_opt)
                        wm_os_internal_free(will_opt);
                    wm_os_internal_free(ssl_opt);
                    wm_os_internal_free(conn_opt);
                    wm_os_internal_free(handle);
                    return WM_INVALID_HANDLE;
                }

                ssl_opts.trustStore = handle->ca;
                ssl_opts.keyStore   = handle->cert;
                ssl_opts.privateKey = handle->key;
            }
            ssl_opts.enableServerCertAuth = true;
            wm_log_debug("mode: MQTT_AUTH_MODE_MUTUAL_AUTH");
        }

        // if (ssl_opts.trustStore != NULL)
        //     wm_log_debug("ca: \r\n%s", ssl_opts.trustStore);

        // if (ssl_opts.keyStore != NULL)
        //     wm_log_debug("cert: \r\n%s", ssl_opts.keyStore);

        // if (ssl_opts.privateKey != NULL)
        //     wm_log_debug("key: \r\n%s", ssl_opts.privateKey);

        if (config->alpn) {
            handle->alpn = string_array_dup((const char **)config->alpn);
            if (!handle->alpn) {
                if (handle->key)
                    wm_os_internal_free(handle->key);
                if (handle->cert)
                    wm_os_internal_free(handle->cert);
                if (handle->ca)
                    wm_os_internal_free(handle->ca);
                if (handle->will_msg)
                    wm_os_internal_free(handle->will_msg);
                if (handle->will_topic)
                    wm_os_internal_free(handle->will_topic);
                if (handle->password)
                    wm_os_internal_free(handle->password);
                if (handle->username)
                    wm_os_internal_free(handle->username);
                wm_os_internal_free(handle->client_id);
                wm_os_internal_free(handle->uri);
                if (will_opt)
                    wm_os_internal_free(will_opt);
                wm_os_internal_free(ssl_opt);
                wm_os_internal_free(conn_opt);
                wm_os_internal_free(handle);
                return WM_INVALID_HANDLE;
            }

            ssl_opts.protos     = (const unsigned char *)handle->alpn;
            ssl_opts.protos_len = 2;
        }

        //conn_opts.ssl = &ssl_opts;

        memcpy(ssl_opt, &ssl_opts, sizeof(*ssl_opt));
        conn_opt->ssl = ssl_opt;
    }

    rc = wm_mqtt_client_init_internal(handle, &client);
    if (rc) {
        wm_log_error("mqtt client init falied");
        MQTTClient_destroy(&client);
        if (will_opt)
            wm_os_internal_free(will_opt);
        if (ssl_opt)
            wm_os_internal_free(ssl_opt);
        wm_os_internal_free(conn_opt);
        mqtt_client_cleanup_handle(handle);
        return WM_INVALID_HANDLE;
    }

    handle->client = (void *)client;
    handle->config = (void *)conn_opt;

    return handle;
}

int wm_mqtt_client_deinit(wm_mqtt_client_handle_t handle)
{
    int ret;
    MQTTClient_connectOptions *config;

    if (!handle || !handle->client) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_mqtt_client_deinit_internal(handle);
    if (ret)
        return ret;

    config = (MQTTClient_connectOptions *)handle->config;

    if (config->ssl)
        wm_os_internal_free(config->ssl);
    if (config->will)
        wm_os_internal_free(config->will);
    wm_os_internal_free(config);

    mqtt_client_cleanup_handle(handle);

    return WM_MQTT_CLIENT_ERROR_SUCCESS;
}

int wm_mqtt_client_connect(wm_mqtt_client_handle_t handle)
{
    int rc;

    if (!handle || !handle->client) {
        return WM_ERR_INVALID_PARAM;
    }

    if (MQTTClient_isConnected((MQTTClient)handle->client)) {
        return WM_MQTT_CLIENT_ERROR_SUCCESS;
    }

    if ((rc = MQTTClient_connect((MQTTClient)handle->client, (MQTTClient_connectOptions *)handle->config)) !=
        MQTTCLIENT_SUCCESS) {
        wm_log_error("Failed to connect, return code %04x", rc);
        return WM_ERR_MQTT_CLIENT_BASE + rc; //WM_ERR_FAILED;
    }

    wm_log_debug("MQTTClient connected!");
    wm_event_send(WM_MQTT_EV, WM_EVENT_MQTT_CLIENT_CONNECTED, NULL, 0);

    return WM_MQTT_CLIENT_ERROR_SUCCESS;
}

int wm_mqtt_client_disconnect(wm_mqtt_client_handle_t handle)
{
    int ret;
    uint8_t need_event = false;

    if (!handle || !handle->client) {
        return WM_ERR_INVALID_PARAM;
    }

    if (MQTTClient_isConnected((MQTTClient)handle->client))
        need_event = true;

    ret = MQTTClient_disconnect((MQTTClient)handle->client, DISCONNECT_TIMEOUT);
    if (ret != MQTTCLIENT_SUCCESS) {
        wm_log_error("Failed to disconnect, return code %d", ret);
    }

    if (need_event) {
        wm_event_send(WM_MQTT_EV, WM_EVENT_MQTT_CLIENT_DISCONNECTED, NULL, 0);
    }

    return WM_MQTT_CLIENT_ERROR_SUCCESS;
}

int wm_mqtt_client_reconnect(wm_mqtt_client_handle_t handle)
{
    int ret;

    if (!handle || !handle->client) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_mqtt_client_deinit_internal(handle);

    if (!ret)
        ret = wm_mqtt_client_init_internal(handle, &handle->client);

    if (!ret)
        ret = wm_mqtt_client_connect(handle);

    return ret;
}

int wm_mqtt_client_subscribe(wm_mqtt_client_handle_t handle, const char *topic, uint8_t qos)
{
    int ret;
    int msg_id = 0;

    if (!handle || !handle->client) {
        return WM_ERR_NO_INITED;
    }

    if (topic == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_log_debug("mqtt client subscribe, topic %s, qos %hhu", topic, qos);

    ret = MQTTClient_subscribe_wm((MQTTClient)handle->client, topic, qos, &msg_id);
    if (ret != MQTTCLIENT_SUCCESS) {
        wm_log_debug("Failed to subscribe, return code %d", ret);
        return WM_ERR_MQTT_CLIENT_BASE + ret; //WM_ERR_FAILED;
    } else {
        return msg_id; //WM_MQTT_CLIENT_ERROR_SUCCESS;
    }
}

int wm_mqtt_client_unsubscribe(wm_mqtt_client_handle_t handle, const char *topic)
{
    int ret;
    int msg_id = 0;

    if (!handle || !handle->client) {
        return WM_ERR_NO_INITED;
    }

    if (topic == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_log_debug("mqtt client unsubscribe, topic %s", topic);

    ret = MQTTClient_unsubscribe_wm((MQTTClient)handle->client, topic, &msg_id);
    if (ret != MQTTCLIENT_SUCCESS) {
        return WM_ERR_MQTT_CLIENT_BASE + ret; //WM_ERR_FAILED;
    } else {
        return msg_id; //WM_MQTT_CLIENT_ERROR_SUCCESS;
    }
}

int wm_mqtt_client_publish(wm_mqtt_client_handle_t handle, const char *topic, const char *message, uint16_t msg_len,
                           uint8_t qos, uint8_t retain)
{
    int ret;
    MQTTClient_deliveryToken token = 0;
    MQTTClient_message pubmsg      = MQTTClient_message_initializer;

    if (!handle || !handle->client) {
        return WM_ERR_NO_INITED;
    }

    if (topic == NULL || message == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_log_debug("mqtt client publish, topic %s", topic);

    pubmsg.payload    = (void *)message;
    pubmsg.payloadlen = msg_len ? msg_len : strlen(message);
    pubmsg.qos        = qos;
    pubmsg.retained   = retain;

    ret = MQTTClient_publishMessage((MQTTClient)handle->client, topic, &pubmsg, &token);
    if (ret != MQTTCLIENT_SUCCESS) {
        wm_log_debug("Failed to publish message, return code %d", ret);

        return WM_ERR_MQTT_CLIENT_BASE + ret; //WM_ERR_FAILED;
    } else {
        wm_log_debug("mqtt client publish, token: %d", token);

        if (qos > 0) {
            return (int)token;
        } else {
            return WM_MQTT_CLIENT_ERROR_SUCCESS; //qos0, msg_id=0
        }
    }
}
