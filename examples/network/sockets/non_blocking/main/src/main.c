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

#if defined(CONFIG_EXAMPLE_TCP_CLIENT)
#define LOG_TAG "nonblocking-socket-client"
#elif defined(CONFIG_EXAMPLE_TCP_SERVER)
#define LOG_TAG "nonblocking-socket-server"
#endif
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

#ifndef CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS
#define CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS "0.0.0.0" //Default use "0.0.0.0"
#endif

#define CONFIG_LWIP_MAX_SOCKETS (5)
#define INVALID_SOCK            (-1)
#define YIELD_TO_ALL_MS         50

static void log_socket_error(const int sock, const int err, const char *message)
{
    wm_log_error("[sock=%d]: %s\n"
                 "error=%d: %s",
                 sock, message, err, strerror(err));
}

static int try_receive(const int sock, char *data, size_t max_len)
{
    int len = recv(sock, data, max_len, 0);
    if (len < 0) {
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0; // Not an error
        }
        if (errno == ENOTCONN) {
            wm_log_warn("[sock=%d]: Connection closed", sock);
            return -2; // Socket has been disconnected
        }
        log_socket_error(sock, errno, "Error occurred during receiving");
        return -1;
    }

    return len;
}

static int socket_send(const int sock, const char *data, const size_t len)
{
    int to_write = len;
    while (to_write > 0) {
        int written = send(sock, data + (len - to_write), to_write, 0);
        if (written < 0 && errno != EINPROGRESS && errno != EAGAIN && errno != EWOULDBLOCK) {
            log_socket_error(sock, errno, "Error occurred during sending");
            return -1;
        }
        to_write -= written;
    }
    return len;
}

#ifdef CONFIG_EXAMPLE_TCP_CLIENT

static void tcp_client_task(void *pvParameters)
{
    static const char *payload = "GET / HTTP/1.1\r\n\r\n";
    static char rx_buffer[128];

    struct addrinfo hints = { .ai_socktype = SOCK_STREAM };
    struct addrinfo *address_info;
    int sock = INVALID_SOCK;

    int res =
        getaddrinfo(CONFIG_EXAMPLE_TCP_CLIENT_CONNECT_ADDRESS, CONFIG_EXAMPLE_TCP_CLIENT_CONNECT_PORT, &hints, &address_info);
    if (res != 0 || address_info == NULL) {
        wm_log_error("couldn't get hostname for `%s` "
                     "getaddrinfo() returns %d, addrinfo=%p",
                     CONFIG_EXAMPLE_TCP_CLIENT_CONNECT_ADDRESS, res, address_info);
        goto error;
    }

    // Creating client's socket
    sock = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    if (sock < 0) {
        log_socket_error(sock, errno, "Unable to create socket");
        goto error;
    }
    wm_log_info("Socket created, connecting to %s:%s", CONFIG_EXAMPLE_TCP_CLIENT_CONNECT_ADDRESS,
                CONFIG_EXAMPLE_TCP_CLIENT_CONNECT_PORT);

    // Marking the socket as non-blocking
    int flags = fcntl(sock, F_GETFL);
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        log_socket_error(sock, errno, "Unable to set socket non blocking");
    }

    if (connect(sock, address_info->ai_addr, address_info->ai_addrlen) != 0) {
        if (errno == EINPROGRESS) {
            wm_log_debug("connection in progress");
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(sock, &fdset);

            // Connection in progress -> have to wait until the connecting socket is marked as writable, i.e. connection completes
            res = select(sock + 1, NULL, &fdset, NULL, NULL);
            if (res < 0) {
                log_socket_error(sock, errno, "Error during connection: select for socket to be writable");
                goto error;
            } else if (res == 0) {
                log_socket_error(sock, errno, "Connection timeout: select for socket to be writable");
                goto error;
            } else {
                int sockerr;
                socklen_t len = (socklen_t)sizeof(int);

                if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void *)(&sockerr), &len) < 0) {
                    log_socket_error(sock, errno, "Error when getting socket error using getsockopt()");
                    goto error;
                }
                if (sockerr) {
                    log_socket_error(sock, sockerr, "Connection error");
                    goto error;
                }
            }
        } else {
            log_socket_error(sock, errno, "Socket is unable to connect");
            goto error;
        }
    }

    wm_log_info("Client sends data to the server...");
    int len = socket_send(sock, payload, strlen(payload));
    if (len < 0) {
        wm_log_error("Error occurred during socket_send");
        goto error;
    }
    wm_log_info("Written: %.*s", len, payload);

    // Keep receiving until we have a reply
    do {
        len = try_receive(sock, rx_buffer, sizeof(rx_buffer) - 1);
        if (len < 0) {
            wm_log_error("Error occurred during try_receive");
            goto error;
        }
        vTaskDelay(pdMS_TO_TICKS(YIELD_TO_ALL_MS));
    } while (len == 0);

    rx_buffer[len] = 0;
    wm_log_info("Received: %s", rx_buffer);

error:
    if (sock != INVALID_SOCK) {
        close(sock);
    }
    freeaddrinfo(address_info);
    vTaskDelete(NULL);
}
#endif // CONFIG_EXAMPLE_TCP_CLIENT

#ifdef CONFIG_EXAMPLE_TCP_SERVER

/**
 * @brief Returns the string representation of client's address (accepted on this server)
 */
static inline char *get_clients_address(struct sockaddr_storage *source_addr)
{
    static char address_str[128];
    char *res = NULL;
    // Convert ip address to string
    if (source_addr->ss_family == PF_INET) {
        res = inet_ntoa_r(((struct sockaddr_in *)source_addr)->sin_addr, address_str, sizeof(address_str) - 1);
    }
#ifdef CONFIG_LWIP_IPV6
    else if (source_addr->ss_family == PF_INET6) {
        res = inet6_ntoa_r(((struct sockaddr_in6 *)source_addr)->sin6_addr, address_str, sizeof(address_str) - 1);
    }
#endif
    if (!res) {
        address_str[0] = '\0'; // Returns empty string if conversion didn't succeed
    }
    return address_str;
}

static void tcp_server_task(void *pvParameters)
{
    static char rx_buffer[128];
    SemaphoreHandle_t *server_ready = pvParameters;
    struct addrinfo hints           = { .ai_socktype = SOCK_STREAM };
    struct addrinfo *address_info;
    int listen_sock        = INVALID_SOCK;
    const size_t max_socks = CONFIG_LWIP_MAX_SOCKETS - 1;
    static int sock[CONFIG_LWIP_MAX_SOCKETS - 1];

    // Prepare a list of file descriptors to hold client's sockets, mark all of them as invalid, i.e. available
    for (int i = 0; i < max_socks; ++i) {
        sock[i] = INVALID_SOCK;
    }

    // Translating the hostname or a string representation of an IP to address_info
    int res = getaddrinfo(CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS, CONFIG_EXAMPLE_TCP_SERVER_BIND_PORT, &hints, &address_info);
    if (res != 0 || address_info == NULL) {
        wm_log_error("couldn't get hostname for `%s` "
                     "getaddrinfo() returns %d, addrinfo=%p",
                     CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS, res, address_info);
        goto error;
    }

    // Creating a listener socket
    listen_sock = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);

    if (listen_sock < 0) {
        log_socket_error(listen_sock, errno, "Unable to create socket");
        goto error;
    }
    wm_log_info("Listener socket created");

    // Marking the socket as non-blocking
    int flags = fcntl(listen_sock, F_GETFL);
    if (fcntl(listen_sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        log_socket_error(listen_sock, errno, "Unable to set socket non blocking");
        goto error;
    }
    wm_log_info("Socket marked as non blocking");

    // Binding socket to the given address
    int err = bind(listen_sock, address_info->ai_addr, address_info->ai_addrlen);
    if (err != 0) {
        log_socket_error(listen_sock, errno, "Socket unable to bind");
        goto error;
    }
    wm_log_info("Socket bound on %s:%s", CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS, CONFIG_EXAMPLE_TCP_SERVER_BIND_PORT);

    // Set queue (backlog) of pending connections to one (can be more)
    err = listen(listen_sock, 1);
    if (err != 0) {
        log_socket_error(listen_sock, errno, "Error occurred during listen");
        goto error;
    }
    wm_log_info("Socket listening");
    xSemaphoreGive(*server_ready);

    // Main loop for accepting new connections and serving all connected clients
    while (1) {
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);

        // Find a free socket
        int new_sock_index = 0;
        for (new_sock_index = 0; new_sock_index < max_socks; ++new_sock_index) {
            if (sock[new_sock_index] == INVALID_SOCK) {
                break;
            }
        }

        // We accept a new connection only if we have a free socket
        if (new_sock_index < max_socks) {
            // Try to accept a new connections
            sock[new_sock_index] = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);

            if (sock[new_sock_index] < 0) {
                if (errno == EWOULDBLOCK) { // The listener socket did not accepts any connection
                    // continue to serve open connections and try to accept again upon the next iteration
                    wm_log_verbose("No pending connections...");
                } else {
                    log_socket_error(listen_sock, errno, "Error when accepting connection");
                    goto error;
                }
            } else {
                // We have a new client connected -> print it's address
                wm_log_info("[sock=%d]: Connection accepted from IP:%s", sock[new_sock_index],
                            get_clients_address(&source_addr));

                // ...and set the client's socket non-blocking
                flags = fcntl(sock[new_sock_index], F_GETFL);
                if (fcntl(sock[new_sock_index], F_SETFL, flags | O_NONBLOCK) == -1) {
                    log_socket_error(sock[new_sock_index], errno, "Unable to set socket non blocking");
                    goto error;
                }
                wm_log_info("[sock=%d]: Socket marked as non blocking", sock[new_sock_index]);
            }
        }

        // We serve all the connected clients in this loop
        for (int i = 0; i < max_socks; ++i) {
            if (sock[i] != INVALID_SOCK) {
                // This is an open socket -> try to serve it
                int len = try_receive(sock[i], rx_buffer, sizeof(rx_buffer));
                if (len < 0) {
                    // Error occurred within this client's socket -> close and mark invalid
                    wm_log_info("[sock=%d]: try_receive() returned %d -> closing the socket", sock[i], len);
                    close(sock[i]);
                    sock[i] = INVALID_SOCK;
                } else if (len > 0) {
                    // Received some data -> echo back
                    wm_log_info("[sock=%d]: Received %.*s", sock[i], len, rx_buffer);

                    len = socket_send(sock[i], rx_buffer, len);
                    if (len < 0) {
                        // Error occurred on write to this socket -> close it and mark invalid
                        wm_log_info("[sock=%d]: socket_send() returned %d -> closing the socket", sock[i], len);
                        close(sock[i]);
                        sock[i] = INVALID_SOCK;
                    } else {
                        // Successfully echoed to this socket
                        wm_log_info("[sock=%d]: Written %.*s", sock[i], len, rx_buffer);
                    }
                }

            } // one client's socket
        } // for all sockets

        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(YIELD_TO_ALL_MS));
    }

error:
    if (listen_sock != INVALID_SOCK) {
        close(listen_sock);
    }

    for (int i = 0; i < max_socks; ++i) {
        if (sock[i] != INVALID_SOCK) {
            close(sock[i]);
        }
    }

    freeaddrinfo(address_info);
    vTaskDelete(NULL);
}
#endif // CONFIG_EXAMPLE_TCP_SERVER

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

#ifdef CONFIG_EXAMPLE_TCP_SERVER
    SemaphoreHandle_t server_ready = xSemaphoreCreateBinary();
    assert(server_ready);
    xTaskCreate(tcp_server_task, "tcp_server", 512, &server_ready, 5, NULL);
    xSemaphoreTake(server_ready, portMAX_DELAY);
    vSemaphoreDelete(server_ready);
#endif // CONFIG_EXAMPLE_TCP_SERVER

#ifdef CONFIG_EXAMPLE_TCP_CLIENT
    xTaskCreate(tcp_client_task, "tcp_client", 1024, NULL, 5, NULL);
#endif // CONFIG_EXAMPLE_TCP_CLIENT

    return 0;
}
