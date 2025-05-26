if(CONFIG_COMPONENT_WEBSOCKET_ENABLED)

    # Add header path for public
    list(APPEND ADD_INCLUDE "port"
        "libwebsockets/include"
        "../freertos/include/freertos"
        "../mbedtls/src/include/mbedtls"
    )

    # Add internal header path
    list(APPEND ADD_PRIVATE_INCLUDE "libwebsockets/lib/tls"
        "libwebsockets/lib/tls/mbedtls/wrapper/include"
        "libwebsockets/lib/tls/mbedtls/wrapper/include/internal"
        "libwebsockets/lib/tls/mbedtls/wrapper/include/platform"
        "libwebsockets/lib/core"
        "libwebsockets/lib/core-net"
        "libwebsockets/lib/event-libs"
        "libwebsockets/lib/roles"
        "libwebsockets/lib/event-libs/poll"
        "libwebsockets/lib/misc"
        "libwebsockets/lib/roles/http"
        "libwebsockets/lib/roles/http/compression"
        "libwebsockets/lib/roles/h1"
        "libwebsockets/lib/roles/h2"
        "libwebsockets/lib/roles/ws"
        "libwebsockets/lib/roles/cgi"
        "libwebsockets/lib/roles/dbus"
        "libwebsockets/lib/roles/raw-proxy"
        "libwebsockets/lib/system/metrics"
        "libwebsockets/lib"
        "libwebsockets/lib/plat/freertos"
    )

    # Add source file
    list(APPEND ADD_SRCS
        libwebsockets/lib/core-net/adopt.c
        libwebsockets/lib/core-net/client/conmon.c
        libwebsockets/lib/core-net/client/client.c
        libwebsockets/lib/core-net/client/connect.c
        libwebsockets/lib/core-net/client/connect2.c
        libwebsockets/lib/core-net/client/connect3.c
        libwebsockets/lib/core-net/client/connect4.c
        libwebsockets/lib/core-net/client/sort-dns.c
        libwebsockets/lib/core-net/close.c

        libwebsockets/lib/core-net/dummy-callback.c
        libwebsockets/lib/core-net/network.c
        libwebsockets/lib/core-net/output.c
        libwebsockets/lib/core-net/pollfd.c
        libwebsockets/lib/core-net/service.c
        libwebsockets/lib/core-net/sorted-usec-list.c
        libwebsockets/lib/core-net/state.c
        libwebsockets/lib/core-net/vhost.c
        libwebsockets/lib/core-net/wsi-timeout.c
        libwebsockets/lib/core-net/wsi.c
        libwebsockets/lib/core/alloc.c
        libwebsockets/lib/core/buflist.c

        libwebsockets/lib/core/context.c
        libwebsockets/lib/core/libwebsockets.c
        libwebsockets/lib/core/logs.c
        libwebsockets/lib/core/lws_dll2.c
        libwebsockets/lib/event-libs/poll/poll.c
        libwebsockets/lib/misc/base64-decode.c
        libwebsockets/lib/misc/lejp.c
        libwebsockets/lib/misc/lws-ring.c
        libwebsockets/lib/misc/lwsac/lwsac.c
        libwebsockets/lib/misc/sha-1.c
        libwebsockets/lib/roles/h1/ops-h1.c
        libwebsockets/lib/roles/http/client/client-http.c
        libwebsockets/lib/roles/http/date.c
        libwebsockets/lib/roles/http/header.c
        libwebsockets/lib/roles/http/parsers.c
        libwebsockets/lib/roles/http/server/server.c
        libwebsockets/lib/roles/listen/ops-listen.c
        libwebsockets/lib/roles/pipe/ops-pipe.c
        libwebsockets/lib/roles/raw-file/ops-raw-file.c
        libwebsockets/lib/roles/raw-skt/ops-raw-skt.c
        libwebsockets/lib/roles/ws/client-parser-ws.c
        libwebsockets/lib/roles/ws/client-ws.c
        libwebsockets/lib/roles/ws/ops-ws.c
        libwebsockets/lib/roles/ws/server-ws.c
        libwebsockets/lib/system/system.c

        libwebsockets/lib/plat/freertos/freertos-init.c
        libwebsockets/lib/plat/freertos/freertos-pipe.c
        libwebsockets/lib/plat/freertos/freertos-misc.c
        libwebsockets/lib/plat/freertos/freertos-fds.c
        libwebsockets/lib/plat/freertos/freertos-service.c
        libwebsockets/lib/plat/freertos/freertos-sockets.c
    )

    # Add secure source file
    if (CONFIG_WEBSOCKET_SECURITY)

        list(APPEND ADD_SRCS
            libwebsockets/lib/tls/tls.c
            libwebsockets/lib/tls/tls-network.c
            libwebsockets/lib/tls/tls-server.c
            libwebsockets/lib/tls/tls-client.c
            libwebsockets/lib/tls/tls-sessions.c
            libwebsockets/lib/tls/mbedtls/mbedtls-session.c
            libwebsockets/lib/tls/mbedtls/mbedtls-ssl.c
            libwebsockets/lib/tls/mbedtls/mbedtls-tls.c
            libwebsockets/lib/tls/mbedtls/mbedtls-x509.c
            libwebsockets/lib/tls/mbedtls/mbedtls-server.c
            libwebsockets/lib/tls/mbedtls/mbedtls-client.c
            libwebsockets/lib/tls/mbedtls/mbedtls-extensions.c
            libwebsockets/lib/tls/mbedtls/wrapper/library/ssl_lib.c
            libwebsockets/lib/tls/mbedtls/wrapper/library/ssl_x509.c
            libwebsockets/lib/tls/mbedtls/wrapper/library/ssl_pkey.c
            libwebsockets/lib/tls/mbedtls/wrapper/library/ssl_methods.c
            libwebsockets/lib/tls/mbedtls/wrapper/library/ssl_lib.c
            libwebsockets/lib/tls/mbedtls/wrapper/library/ssl_cert.c
            libwebsockets/lib/tls/mbedtls/wrapper/platform/ssl_pm.c
            libwebsockets/lib/tls/mbedtls/wrapper/platform/ssl_port.c
            libwebsockets/lib/misc/cache-ttl/lws-cache-ttl.c
            libwebsockets/lib/misc/cache-ttl/heap.c
        )
    endif()

    register_component()

endif()

