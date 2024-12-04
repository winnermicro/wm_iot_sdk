#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "wm_mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "wm_nm_api.h"

#define LOG_TAG "mqtt example"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

extern const uint8_t mqtt_eclipseprojects_io_pem_start[] asm("__binary_main_src_mqtt_eclipseprojects_io_pem");
extern const uint32_t mqtt_eclipseprojects_io_pem_length asm("__binary_main_src_mqtt_eclipseprojects_io_pem_length");

static void wifi_init_sta(void)
{
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };
    int ret                      = wm_nm_start_wifi_station(&cfg);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wifi station start fail: %d", ret);
        return;
    }
    while (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    wm_log_info("wifi station start success, network is ready!");
}

static void event_handler_mqtt_connected(wm_mqtt_event_data_t *data, void *priv)
{
    int msg_id;

    wm_log_info("mqtt connected");

    msg_id = wm_mqtt_client_publish(priv, "/topic/qos1", "data_3", 0, 1, 0);
    wm_log_info("sent publish successful, msg_id=%d", msg_id);

    msg_id = wm_mqtt_client_subscribe(priv, "/topic/qos0", 0);
    wm_log_info("sent subscribe successful, msg_id=%d", msg_id);

    msg_id = wm_mqtt_client_subscribe(priv, "/topic/qos1", 1);
    wm_log_info("sent subscribe successful, msg_id=%d", msg_id);

    msg_id = wm_mqtt_client_unsubscribe(priv, "/topic/qos1");
    wm_log_info("sent unsubscribe successful, msg_id=%d", msg_id);
}

static void event_handler_mqtt_disconnected(wm_mqtt_event_data_t *data, void *priv)
{
    wm_log_info("mqtt disconnected");
}

static void event_handler_mqtt_recved_data(wm_mqtt_event_data_t *data, void *priv)
{
    wm_log_info("mqtt recved data");
    wm_log_raw_info("TOPIC=%.*s\r\n", data->mqtt_client_data_info.topic_len, data->mqtt_client_data_info.topic);
    wm_log_raw_info("DATA=%.*s\r\n", data->mqtt_client_data_info.payload_len, data->mqtt_client_data_info.payload);

    free(data->mqtt_client_data_info.topic);
    free(data->mqtt_client_data_info.payload);
}

static void event_handler_mqtt_published(wm_mqtt_event_data_t *data, void *priv)
{
    wm_log_info("mqtt published, msg_id=%d", data->mqtt_client_published_info.msg_id);
}

static void event_handler_mqtt_event(wm_event_group_t group, int event, wm_mqtt_event_data_t *data, void *priv)
{
    switch (event) {
        case WM_EVENT_MQTT_CLIENT_CONNECTED:
            event_handler_mqtt_connected(data, priv);
            break;
        case WM_EVENT_MQTT_CLIENT_DISCONNECTED:
            event_handler_mqtt_disconnected(data, priv);
            break;
        case WM_EVENT_MQTT_CLIENT_DATA:
            event_handler_mqtt_recved_data(data, priv);
            break;
        case WM_EVENT_MQTT_CLIENT_PUBLISHED:
            event_handler_mqtt_published(data, priv);
            break;
        default:
            break;
    }
}

static void mqtt_app_start(void)
{
    wm_mqtt_client_config_t mqtt_cfg = { .uri = CONFIG_BROKER_URI, .ca = (char *)mqtt_eclipseprojects_io_pem_start };

    wm_mqtt_client_handle_t client = wm_mqtt_client_init(&mqtt_cfg);

    if (client) {
        wm_event_add_callback(WM_MQTT_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_mqtt_event, client);
        wm_mqtt_client_connect(client);
    }
}

int main(void)
{
    wifi_init_sta();

    mqtt_app_start();

    return 0;
}
