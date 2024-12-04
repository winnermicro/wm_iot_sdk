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

#define LOG_TAG "udp multicast"
#include "wm_log.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define CONFIG_EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

#define UDP_PORT            CONFIG_EXAMPLE_PORT

#define MULTICAST_TTL       CONFIG_EXAMPLE_MULTICAST_TTL

#define MULTICAST_IPV4_ADDR CONFIG_EXAMPLE_MULTICAST_IPV4_ADDR
#define MULTICAST_IPV6_ADDR CONFIG_EXAMPLE_MULTICAST_IPV6_ADDR

#ifdef CONFIG_EXAMPLE_IPV4
/* Add a socket, either IPV4-only or IPV6 dual mode, to the IPV4
   multicast group */
static int socket_add_ipv4_multicast_group(int sock, bool assign_source_if)
{
    struct ip_mreq imreq = { 0 };
    struct in_addr iaddr = { 0 };
    int err              = 0;
    // Configure source interface
    imreq.imr_interface.s_addr = IPADDR_ANY;
    // Configure multicast address to listen to
    err = inet_aton(MULTICAST_IPV4_ADDR, &imreq.imr_multiaddr.s_addr);
    if (err != 1) {
        wm_log_error("Configured IPV4 multicast address '%s' is invalid.", MULTICAST_IPV4_ADDR);
        // Errors in the return value have to be negative
        err = -1;
        goto err;
    }
    wm_log_info("Configured IPV4 Multicast address %s", inet_ntoa(imreq.imr_multiaddr.s_addr));
    if (!IP_MULTICAST(ntohl(imreq.imr_multiaddr.s_addr))) {
        wm_log_warn("Configured IPV4 multicast address '%s' is not a valid multicast address. This will probably not work.",
                    MULTICAST_IPV4_ADDR);
    }

    if (assign_source_if) {
        // Assign the IPv4 multicast source interface, via its IP
        // (only necessary if this socket is IPV4 only)
        err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr, sizeof(struct in_addr));
        if (err < 0) {
            wm_log_error("Failed to set IP_MULTICAST_IF. Error %d", errno);
            goto err;
        }
    }

    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imreq, sizeof(struct ip_mreq));
    if (err < 0) {
        wm_log_error("Failed to set IP_ADD_MEMBERSHIP. Error %d", errno);
        goto err;
    }

err:
    return err;
}
#endif /* CONFIG_EXAMPLE_IPV4 */

#ifdef CONFIG_EXAMPLE_IPV4_ONLY
static int create_multicast_ipv4_socket(void)
{
    struct sockaddr_in saddr = { 0 };
    int sock                 = -1;
    int err                  = 0;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        wm_log_error("Failed to create socket. Error %d", errno);
        return -1;
    }

    // Bind the socket to any address
    saddr.sin_family      = PF_INET;
    saddr.sin_port        = htons(UDP_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err                   = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (err < 0) {
        wm_log_error("Failed to bind socket. Error %d", errno);
        goto err;
    }

    // Assign multicast TTL (set separately from normal interface TTL)
    uint8_t ttl = MULTICAST_TTL;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (err < 0) {
        wm_log_error("Failed to set IP_MULTICAST_TTL. Error %d", errno);
        goto err;
    }

    // this is also a listening socket, so add it to the multicast
    // group for listening...
    err = socket_add_ipv4_multicast_group(sock, true);
    if (err < 0) {
        goto err;
    }

    // All set, socket is configured for sending and receiving
    return sock;

err:
    close(sock);
    return -1;
}
#endif /* CONFIG_EXAMPLE_IPV4_ONLY */

#ifdef CONFIG_EXAMPLE_IPV6
static int create_multicast_ipv6_socket(void)
{
    struct sockaddr_in6 saddr = { 0 };
#ifdef CONFIG_EXAMPLE_IPV6
    int netif_index = 0;
#endif
    struct in6_addr if_inaddr = { 0 };
    struct ip6_addr if_ipaddr = { 0 };
    struct ipv6_mreq v6imreq  = { 0 };
    int sock                  = -1;
    int err                   = 0;

    sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6);
    if (sock < 0) {
        wm_log_error("Failed to create socket. Error %d", errno);
        return -1;
    }

    // Bind the socket to any address
    saddr.sin6_family = AF_INET6;
    saddr.sin6_port   = htons(UDP_PORT);
    memset(&saddr.sin6_addr.un, 0, sizeof(saddr.sin6_addr.un));
    err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in6));
    if (err < 0) {
        wm_log_error("Failed to bind socket. Error %d", errno);
        goto err;
    }

    // Selct the interface to use as multicast source for this socket.
    memset(&if_inaddr.un, 0, sizeof(if_inaddr.un));
    // this is also a listening socket, so add it to the multicast
    // group for listening...
#ifdef CONFIG_EXAMPLE_IPV6
    // Configure multicast address to listen to
    err = inet6_aton(MULTICAST_IPV6_ADDR, &v6imreq.ipv6mr_multiaddr);
    if (err != 1) {
        wm_log_error("Configured IPV6 multicast address '%s' is invalid.", MULTICAST_IPV6_ADDR);
        goto err;
    }
    wm_log_info("Configured IPV6 Multicast address %s", inet6_ntoa(v6imreq.ipv6mr_multiaddr));
    ip6_addr_t multi_addr;
    inet6_addr_to_ip6addr(&multi_addr, &v6imreq.ipv6mr_multiaddr);
    if (!ip6_addr_ismulticast(&multi_addr)) {
        wm_log_warn("Configured IPV6 multicast address '%s' is not a valid multicast address. This will probably not work.",
                    MULTICAST_IPV6_ADDR);
    }
    // Configure source interface
    v6imreq.ipv6mr_interface = (unsigned int)netif_index;
    err                      = setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &v6imreq, sizeof(struct ipv6_mreq));
    if (err < 0) {
        wm_log_error("Failed to set IPV6_ADD_MEMBERSHIP. Error %d", errno);
        goto err;
    }
#endif

#if CONFIG_EXAMPLE_IPV4_V6
    // Add the common IPV4 config options
    err = socket_add_ipv4_multicast_group(sock, false);
    if (err < 0) {
        goto err;
    }
#endif

#if CONFIG_EXAMPLE_IPV4_V6
    int only = 0;
#else
    int only = 1; /* IPV6-only socket */
#endif
    err = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &only, sizeof(int));
    if (err < 0) {
        wm_log_error("Failed to set IPV6_V6ONLY. Error %d", errno);
        goto err;
    }
    wm_log_info("Socket set IPV6-only");

    // All set, socket is configured for sending and receiving
    return sock;

err:
    close(sock);
    return -1;
}
#endif

static void mcast_example_task(void *pvParameters)
{
    while (1) {
        int sock;

#ifdef CONFIG_EXAMPLE_IPV4_ONLY
        sock = create_multicast_ipv4_socket();
        if (sock < 0) {
            wm_log_error("Failed to create IPv4 multicast socket");
        }
#else
        sock = create_multicast_ipv6_socket();
        if (sock < 0) {
            wm_log_error("Failed to create IPv6 multicast socket");
        }
#endif

        if (sock < 0) {
            // Nothing to do!
            vTaskDelay(5 / portTICK_PERIOD_MS);
            continue;
        }

#ifdef CONFIG_EXAMPLE_IPV4
        // set destination multicast addresses for sending from these sockets
        struct sockaddr_in sdestv4 = {
            .sin_family = PF_INET,
            .sin_port   = htons(UDP_PORT),
        };
        // We know this inet_aton will pass because we did it above already
        inet_aton(MULTICAST_IPV4_ADDR, &sdestv4.sin_addr.s_addr);
#endif

#ifdef CONFIG_EXAMPLE_IPV6
        struct sockaddr_in6 sdestv6 = {
            .sin6_family = PF_INET6,
            .sin6_port   = htons(UDP_PORT),
        };
        // We know this inet_aton will pass because we did it above already
        inet6_aton(MULTICAST_IPV6_ADDR, &sdestv6.sin6_addr);
#endif

        // Loop waiting for UDP received, and sending UDP packets if we don't
        // see any.
        int err = 1;
        while (err > 0) {
            struct timeval tv = {
                .tv_sec  = 2,
                .tv_usec = 0,
            };
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(sock, &rfds);

            int s = select(sock + 1, &rfds, NULL, NULL, &tv);
            if (s < 0) {
                wm_log_error("Select failed: errno %d", errno);
                err = -1;
                break;
            } else if (s > 0) {
                if (FD_ISSET(sock, &rfds)) {
                    // Incoming datagram received
                    char recvbuf[48];
                    char raddr_name[32] = { 0 };

                    struct sockaddr_storage raddr; // Large enough for both IPv4 or IPv6
                    socklen_t socklen = sizeof(raddr);
                    int len           = recvfrom(sock, recvbuf, sizeof(recvbuf) - 1, 0, (struct sockaddr *)&raddr, &socklen);
                    if (len < 0) {
                        wm_log_error("multicast recvfrom failed: errno %d", errno);
                        err = -1;
                        break;
                    }

                    // Get the sender's address as a string
#ifdef CONFIG_EXAMPLE_IPV4
                    if (raddr.ss_family == PF_INET) {
                        inet_ntoa_r(((struct sockaddr_in *)&raddr)->sin_addr, raddr_name, sizeof(raddr_name) - 1);
                    }
#endif
#ifdef CONFIG_EXAMPLE_IPV6
                    if (raddr.ss_family == PF_INET6) {
                        inet6_ntoa_r(((struct sockaddr_in6 *)&raddr)->sin6_addr, raddr_name, sizeof(raddr_name) - 1);
                    }
#endif
                    wm_log_info("received %d bytes from %s:", len, raddr_name);

                    recvbuf[len] = 0; // Null-terminate whatever we received and treat like a string...
                    wm_log_info("%s", recvbuf);
                }
            } else { // s == 0
                // Timeout passed with no incoming data, so send something!
                static int send_count;
                const char sendfmt[] = "Multicast #%d sent\n";
                char sendbuf[48];
                char addrbuf[32] = { 0 };
                int len          = snprintf(sendbuf, sizeof(sendbuf), sendfmt, send_count++);
                if (len > sizeof(sendbuf)) {
                    wm_log_error("Overflowed multicast sendfmt buffer!!");
                    send_count = 0;
                    err        = -1;
                    break;
                }

                struct addrinfo hints = {
                    .ai_flags    = AI_PASSIVE,
                    .ai_socktype = SOCK_DGRAM,
                };
                struct addrinfo *res;

#ifdef CONFIG_EXAMPLE_IPV4 // Send an IPv4 multicast packet

#ifdef CONFIG_EXAMPLE_IPV4_ONLY
                hints.ai_family = AF_INET; // For an IPv4 socket
#else
                hints.ai_family = AF_INET6; // For an IPv4 socket with V4 mapped addresses
                hints.ai_flags |= AI_V4MAPPED;
#endif
                int err = getaddrinfo(CONFIG_EXAMPLE_MULTICAST_IPV4_ADDR, NULL, &hints, &res);
                if (err < 0) {
                    wm_log_error("getaddrinfo() failed for IPV4 destination address. error: %d", err);
                    break;
                }
                if (res == 0) {
                    wm_log_error("getaddrinfo() did not return any addresses");
                    break;
                }
#ifdef CONFIG_EXAMPLE_IPV4_ONLY
                ((struct sockaddr_in *)res->ai_addr)->sin_port = htons(UDP_PORT);
                inet_ntoa_r(((struct sockaddr_in *)res->ai_addr)->sin_addr, addrbuf, sizeof(addrbuf) - 1);
                wm_log_info("Sending to IPV4 multicast address %s:%d...", addrbuf, UDP_PORT);
#else
                ((struct sockaddr_in6 *)res->ai_addr)->sin6_port = htons(UDP_PORT);
                inet6_ntoa_r(((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, addrbuf, sizeof(addrbuf) - 1);
                wm_log_info("Sending to IPV6 (V4 mapped) multicast address %s port %d (%s)...", addrbuf, UDP_PORT,
                            CONFIG_EXAMPLE_MULTICAST_IPV4_ADDR);
#endif
                err = sendto(sock, sendbuf, len, 0, res->ai_addr, res->ai_addrlen);
                freeaddrinfo(res);
                if (err < 0) {
                    wm_log_error("IPV4 sendto failed. errno: %d", errno);
                    break;
                }
#endif
#ifdef CONFIG_EXAMPLE_IPV6
                hints.ai_family   = AF_INET6;
                hints.ai_protocol = 0;
                err               = getaddrinfo(CONFIG_EXAMPLE_MULTICAST_IPV6_ADDR, NULL, &hints, &res);
                if (err < 0) {
                    wm_log_error("getaddrinfo() failed for IPV6 destination address. error: %d", err);
                    break;
                }

                struct sockaddr_in6 *s6addr = (struct sockaddr_in6 *)res->ai_addr;
                s6addr->sin6_port           = htons(UDP_PORT);
                inet6_ntoa_r(s6addr->sin6_addr, addrbuf, sizeof(addrbuf) - 1);
                wm_log_info("Sending to IPV6 multicast address %s port %d...", addrbuf, UDP_PORT);
                err = sendto(sock, sendbuf, len, 0, res->ai_addr, res->ai_addrlen);
                freeaddrinfo(res);
                if (err < 0) {
                    wm_log_error("IPV6 sendto failed. errno: %d", errno);
                    break;
                }
#endif
            }
        }

        wm_log_error("Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }
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

    xTaskCreate(&mcast_example_task, "mcast_task", 512, NULL, 5, NULL);

    return 0;
}
