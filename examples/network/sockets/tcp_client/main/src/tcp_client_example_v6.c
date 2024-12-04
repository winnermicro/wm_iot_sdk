#include <stdio.h>
#include "wmsdk_config.h"
#include "wm_netif.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define LOG_TAG "tcp client"
#include "wm_log.h"

#if !LWIP_IPV6
#error "Pls enable IPv6"
#endif

#define PORT         CONFIG_EXAMPLE_PORT
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR

void tcp_client(void)
{
    static char rx_buffer[1600];
    char host_ip[]    = HOST_IP_ADDR;
    int addr_family   = 0;
    int ip_protocol   = 0;
    wm_netif_t *netif = NULL;

    while (1) {
        struct sockaddr_in6 dest_addr = { 0 };
        inet_pton(AF_INET6, host_ip, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port   = htons(PORT);
        addr_family           = AF_INET6;
        ip_protocol           = IPPROTO_IPV6;

        int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            wm_log_error("Unable to create socket: errno %d", errno);
            break;
        }
        wm_log_info("Socket created, connecting to %s:%d", host_ip, PORT);

        if (NULL == (netif = wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA))) {
            wm_log_error("Failed to find interface ");
            break;
        }

        dest_addr.sin6_scope_id = wm_netif_get_netif_impl_index(netif);
        wm_log_info("Interface index: %u", dest_addr.sin6_scope_id);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            close(sock);
            wm_log_error("Socket unable to connect: errno %d", errno);
            break;
        }
        wm_log_info("Successfully connected");

        while (1) {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                wm_log_error("recv failed: errno %d", errno);
                break;
            }

            // Data received
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            wm_log_info("Received %d bytes from %s:", len, host_ip);

            int err = send(sock, rx_buffer, strlen(rx_buffer), 0);
            if (err < 0) {
                wm_log_error("Error occurred during sending: errno %d", errno);
                break;
            }
            wm_log_info("Send %d bytes to %s", len, host_ip);
        }

        if (sock != -1) {
            wm_log_error("Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}
