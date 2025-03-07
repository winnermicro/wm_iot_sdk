/**
 * @file main.c
 *
 * @brief WiFi SoftAP TCP Server Application's demo main
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
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

#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "wm_nm_api.h"

#define LOG_TAG "softap tcp svr"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID    CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASS    CONFIG_EXAMPLE_WIFI_PASSWORD
#define EXAMPLE_WIFI_CHANNEL CONFIG_EXAMPLE_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN CONFIG_EXAMPLE_MAX_STA_CONN

#define PORT                 CONFIG_EXAMPLE_PORT
#define KEEPALIVE_IDLE       CONFIG_EXAMPLE_KEEPALIVE_IDLE
#define KEEPALIVE_INTERVAL   CONFIG_EXAMPLE_KEEPALIVE_INTERVAL
#define KEEPALIVE_COUNT      CONFIG_EXAMPLE_KEEPALIVE_COUNT

static void tcp_client_task(void *pvParameters)
{
    int client_socket = *(int *)pvParameters;
    int len;
    char rx_buffer[128];

    do {
        len = recv(client_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0) {
            wm_log_error("Error occurred during receiving: errno %d", errno);
        } else if (len == 0) {
            wm_log_warn("Connection closed");
        } else {
            rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
            wm_log_info("Received %d bytes: %s", len, rx_buffer);

            // send() can return less bytes than supplied length.
            // loop for robust implementation.
            int to_write = len;
            while (to_write > 0) {
                int written = send(client_socket, rx_buffer + (len - to_write), to_write, 0);
                if (written < 0) {
                    wm_log_error("Error occurred during sending: errno %d", errno);
                    // Failed to retransmit, giving up
                    goto CLEAN_UP;
                }
                to_write -= written;
            }
        }
    } while (len > 0);

CLEAN_UP:
    close(client_socket);
    vTaskDelete(NULL);
}

static void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family  = (int)pvParameters;
    int ip_protocol  = 0;
    int keepAlive    = 1;
    int keepIdle     = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount    = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr    = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family         = AF_INET;
        dest_addr_ip4->sin_port           = htons(PORT);
        ip_protocol                       = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        wm_log_error("Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    wm_log_info("Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        wm_log_error("Socket unable to bind: errno %d", errno);
        wm_log_error("IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    wm_log_info("Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        wm_log_error("Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1) {
        wm_log_info("Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock           = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            wm_log_error("Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

        // Convert ip address to string
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
        wm_log_info("Socket accepted ip address: %s", addr_str);

        xTaskCreate(tcp_client_task, "tcp_client", 512, &sock, 5, NULL);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

static void event_handler_ap(wm_event_group_t group, int event, wm_wifi_event_data_t *data, void *priv)
{
    uint8_t *mac;
    if (event == WM_EVENT_WIFI_AP_STACONNECTED) {
        mac = data->ap_staconnected_info.addr;
        wm_log_info("station %02X:%02X:%02X:%02X:%02X:%02X join, AID=%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                    data->ap_staconnected_info.aid);
    } else if (event == WM_EVENT_WIFI_AP_STADISCONNECTED) {
        mac = data->ap_stadisconnected_info.addr;
        wm_log_info("station %02X:%02X:%02X:%02X:%02X:%02X leave, AID=%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                    data->ap_stadisconnected_info.aid);
    }
}

void wifi_init_softap(void)
{
    int err;
    char ip[16];
    wm_ip_addr_t ipaddr;
    wm_wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_WIFI_SSID),
            .channel = EXAMPLE_WIFI_CHANNEL,
            .password = EXAMPLE_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WM_WIFI_AUTH_WPA2_PSK,
            .pairwise_cipher = WM_WIFI_CIPHER_TYPE_CCMP,
        },
    };

    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_ap, NULL);

    err = wm_wifi_init();

    if (strlen(EXAMPLE_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WM_WIFI_AUTH_OPEN;
    }

    if (!err)
        err = wm_wifi_set_config(WM_WIFI_IF_AP, &wifi_config);
    if (!err)
        err = wm_wifi_ap_start();

    if (!err) {
        wm_log_info("wifi_init_softap finished. SSID:%s password:%s channel:%d", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS,
                    EXAMPLE_WIFI_CHANNEL);

        wm_netif_get_ipaddr(WM_NETIF_TYPE_WIFI_AP, &ipaddr, NULL, NULL);
        ipaddr_ntoa_r((ip_addr_t *)&ipaddr, ip, sizeof(ip));
        wm_log_info("softap ip: %s", ip);
    } else {
        wm_log_error("wifi_init_softap failed. SSID:%s password:%s channel:%d", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS,
                     EXAMPLE_WIFI_CHANNEL);

        wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)event_handler_ap, NULL);
    }
}

int main(void)
{
    wifi_init_softap();

    xTaskCreate(tcp_server_task, "tcp_server", 512, (void *)AF_INET, 5, NULL);

    return 0;
}
