
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

#define LOG_TAG "ipv6 only"
#include "wm_log.h"

#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

#if LWIP_IPV4
#error "This is IPv6 only project, Pls disable IPv4"
#endif

#if !LWIP_IPV6
#error "Pls enable IPv6"
#endif

static void task_ip6(void *pvParameters)
{
    char rx_buffer[128];
    char tx_buffer[128];
    int ip_protocol = 0;
    uint32_t msg_id = 0;

    while (1) {
        struct sockaddr_in6 dest_addr = { 0 };
        inet6_aton(CONFIG_EXAMPLE_IPV6_ADDR, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port   = htons(CONFIG_EXAMPLE_PORT);

        struct sockaddr_in6 src_addr = { 0 };
        inet6_aton("::", &src_addr.sin6_addr);
        src_addr.sin6_family = AF_INET6;
        src_addr.sin6_port   = htons(CONFIG_EXAMPLE_PORT);

        int sock = socket(AF_INET6, SOCK_DGRAM, ip_protocol);
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

        wm_log_info("Socket created, sending to %s:%d", CONFIG_EXAMPLE_IPV6_ADDR, CONFIG_EXAMPLE_PORT);

        while (1) {
            snprintf(tx_buffer, sizeof(tx_buffer), "Message from W800, id is %u\n", msg_id++);

            int err = sendto(sock, (void *)tx_buffer, strlen(tx_buffer), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                wm_log_error("Error occurred during sending: errno %d", errno);
                break;
            }
            wm_log_info("Message sent");

            struct sockaddr_storage source_addr;
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
                wm_log_info("Received %d bytes from %s:", len, CONFIG_EXAMPLE_IPV6_ADDR);
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

static void wm_nm_event_callback(wm_event_group_t group, int event, void *data, void *priv)
{
    ip_addr_t addr;
    wm_ip6_addr_t ip6;

    inet_pton(AF_INET6, CONFIG_EXAMPLE_IPV6_ADDR, &addr);

    switch (event) {
        case WM_NM_WIFI_STA_GOT_IP6:
            assert(priv != NULL);

            // CONFIG_EXAMPLE_IPV6_ADDR is IPv6 GL address
            if (ip6_addr_isglobal(&addr)) {
                if (wm_nm_get_netif_ip6_global(WM_NETIF_TYPE_WIFI_STA, &ip6) == WM_ERR_SUCCESS) {
                    xSemaphoreGive((SemaphoreHandle_t)priv);
                }
            } else {
                if (wm_nm_get_netif_ip6_linklocal(WM_NETIF_TYPE_WIFI_STA, &ip6) == WM_ERR_SUCCESS) {
                    xSemaphoreGive((SemaphoreHandle_t)priv);
                }
            }

            break;

        default:
            break;
    }
}

static void wifi_init_sta(void)
{
    SemaphoreHandle_t sem        = NULL;
    wm_nm_wifi_station_cfg_t cfg = { CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD };
    int ret;

    sem = xSemaphoreCreateCounting(1, 0);

    if (sem == NULL) {
        wm_log_error("OOM");
        return;
    }

    wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_callback, sem);

    ret = wm_nm_start_wifi_station(&cfg);

    if (ret != WM_ERR_SUCCESS) {
        vSemaphoreDelete(sem);
        wm_log_error("wifi station start fail: %d", ret);
        return;
    }

    xSemaphoreTake(sem, portMAX_DELAY);
    vSemaphoreDelete(sem);

    wm_log_info("GOT IPv6!");
}

int main(void)
{
    wifi_init_sta();

    xTaskCreate(task_ip6, "task_ip6", 512, NULL, 5, NULL);

    return 0;
}
