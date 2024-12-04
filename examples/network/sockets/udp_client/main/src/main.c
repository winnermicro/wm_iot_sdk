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
#include "lwip/if_api.h"
#include "wm_nm_api.h"

#define LOG_TAG "udp client"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

#define PORT CONFIG_EXAMPLE_PORT

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_IPV6)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif

static void udp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char tx_buffer[128];
    char host_ip[]  = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
    uint32_t msg_id = 0;

    while (1) {
#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr = { 0 };
        struct addrinfo hints        = { .ai_family = AF_INET, .ai_socktype = SOCK_DGRAM };
        struct addrinfo *address_info;

        int res = getaddrinfo(HOST_IP_ADDR, NULL, &hints, &address_info);
        if (res != 0 || address_info == NULL) {
            wm_log_error("couldn't get hostname for `%s` getaddrinfo() returns %d, addrinfo=%p", HOST_IP_ADDR, res,
                         address_info);
            break;
        }
        memcpy(&dest_addr, address_info->ai_addr, address_info->ai_addrlen);
        freeaddrinfo(address_info);
        dest_addr.sin_port   = htons(PORT);
        dest_addr.sin_family = AF_INET;

        struct sockaddr_in src_addr = { 0 };
        src_addr.sin_addr.s_addr    = inet_addr("0.0.0.0");
        src_addr.sin_family         = AF_INET;
        src_addr.sin_port           = htons(PORT);

        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_IPV6)
        struct sockaddr_in6 dest_addr = { 0 };
        inet6_aton(HOST_IP_ADDR, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port   = htons(PORT);
        //dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
        addr_family                  = AF_INET6;
        struct sockaddr_in6 src_addr = { 0 };
        inet6_aton("::", &src_addr.sin6_addr);
        src_addr.sin6_family = AF_INET6;
        src_addr.sin6_port   = htons(PORT);
#endif

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            wm_log_error("Unable to create socket: errno %d", errno);
            break;
        }

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec  = 10;
        timeout.tv_usec = 0;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
            wm_log_error("Set recv timeout failed: error %s", strerror(errno));
        }

        if (bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
            wm_log_error("Unable to bind socket:  %s", strerror(errno));
        }

        wm_log_info("Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);

        while (1) {
            snprintf(tx_buffer, sizeof(tx_buffer), "Message from W800, id is %u", msg_id++);

            int err = sendto(sock, (void *)tx_buffer, strlen(tx_buffer), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                wm_log_error("Error occurred during sending: errno %d", errno);
                break;
            }
            wm_log_info("Message sent");

            struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(dest_addr);
            int len           = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                wm_log_error("recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                wm_log_info("Received %d bytes from %s:", len, host_ip);
                wm_log_info("%s", rx_buffer);
                if (strncmp(rx_buffer, "OK: ", 4) == 0) {
                    wm_log_info("Received expected message, reconnecting");
                    break;
                }
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            wm_log_error("Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

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

int main(void)
{
    wifi_init_sta();

#if defined(CONFIG_EXAMPLE_IPV6)
    vTaskDelay(2000); // Waiting to create IPv6 address
#endif

    xTaskCreate(udp_client_task, "udp_client", 512, NULL, 5, NULL);

    return 0;
}
