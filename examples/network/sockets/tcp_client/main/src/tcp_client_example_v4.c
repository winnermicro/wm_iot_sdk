#include <stdio.h>
#include "wmsdk_config.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define LOG_TAG "tcp client"
#include "wm_log.h"

#if !LWIP_IPV4
#error "Pls enable IPv4"
#endif

#define PORT         CONFIG_EXAMPLE_PORT
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR

void tcp_client(void)
{
    static char rx_buffer[1600];
    int addr_family = 0;
    int ip_protocol = 0;

    while (1) {
        struct sockaddr_in dest_addr = { 0 };
        struct addrinfo hints        = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM };
        struct addrinfo *address_info;

        int res = getaddrinfo(HOST_IP_ADDR, NULL, &hints, &address_info);
        if (res != 0 || address_info == NULL) {
            wm_log_error("couldn't get hostname for `%s` getaddrinfo() returns %d, addrinfo=%p", HOST_IP_ADDR, res,
                         address_info);
            return;
        }

        memcpy(&dest_addr, address_info->ai_addr, address_info->ai_addrlen);
        freeaddrinfo(address_info);
        dest_addr.sin_port   = htons(PORT);
        dest_addr.sin_family = AF_INET;

        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            wm_log_error("Unable to create socket: errno %d", errno);
            break;
        }
        wm_log_info("Socket created, connecting to %s:%d", HOST_IP_ADDR, PORT);

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
            wm_log_info("Received %d bytes from %s:", len, HOST_IP_ADDR);

            int err = send(sock, rx_buffer, strlen(rx_buffer), 0);
            if (err < 0) {
                wm_log_error("Error occurred during sending: errno %d", errno);
                break;
            }
            wm_log_info("Send %d bytes to %s", len, HOST_IP_ADDR);
        }

        if (sock != -1) {
            wm_log_error("Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}
