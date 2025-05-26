
.. _httpd_websockets:

Httpd and Websocket
====================

Brief
---------------

    This document corresponds to the ``components/websockets`` component. This component is transplanted from
    the open source ``libwebsockets 4.3.3`` . It mainly transplants and adapts three aspects of functions:
    ``http server``, ``Websocket server``, and ``Websocket client``. At the same time, it also adapts the
    functions of ``HTTPS`` and ``WSS``.

    When transplanting ``libwebsockets``, its API was not modified, nor were additional interfaces encapsulated
    on its basis. Therefore, during development, in addition to referring to the relevant examples in this SDK,
    you can also refer to the native examples and API interfaces of libwebsockets.


libwebsockets
---------------

    Official website : `https://libwebsockets.org/ <https://libwebsockets.org/>`_

    Source Code : `https://github.com/warmcat/libwebsockets <https://github.com/warmcat/libwebsockets>`_

    API Document: `https://libwebsockets.org/lws-api-doc-main/html/index.html <https://libwebsockets.org/lws-api-doc-main/html/index.html>`_


Functions
---------------

    1. :ref:`Configuration <config>`

    2. :ref:`Websocket Client <websocket_client>`

    3. :ref:`Websocket Server <websocket_server>`

    4. :ref:`Http Server <http_server>`

.. _config:

Configuration
-------------

    Configurations in this component:

    ``CONFIG_COMPONENT_WEBSOCKET_ENABLED`` Enable websockets

    ``CONFIG_WEBSOCKET_SECURITY`` Enable security, so HTTPS, WSS can be used

    Configurations in other components:

    ``CONFIG_COMPONENT_LWIP_ENABLED`` Enable LWIP, must enable it

    ``LWIP_ENABLE_IPV6`` Enable IPV6


.. _websocket_client:

Websocket Client
-------------------

    When using ``libwebsockets`` as a ``WS client`` to connect to a server, send data to the server, and receive data from the server,
    the process usually involves several steps: creating a context, connecting to the server, scheduling, and event handling.
    The sending and receiving of data are carried out during the handling of callback events.

    **Create Context**

        The following is the process of creating the ``websocket client`` context:

        .. code:: C

            /*set protocols information*/
            static struct lws_protocols g_protocols[] = {
            {
                .name           = "echo-protocol",
                .callback       = client_callback,
                .rx_buffer_size = 1024,
                .tx_packet_size = 1024,
                },
                LWS_PROTOCOL_LIST_TERM
            };

            int is_ssl_connection = 1;

            /* set context creation information */
            struct lws_context_creation_info info;
            memset(&info, 0, sizeof info);
            info.port      = CONTEXT_PORT_NO_LISTEN;
            info.protocols = g_protocols;
            info.options   = (is_ssl_connection ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0);
            info.gid       = -1;
            info.uid       = -1;

            if (is_ssl_connection) {
                /*set CA certificate*/
                info.client_ssl_ca_mem     = g_ca_root_cert;
                info.client_ssl_ca_mem_len = strlen(g_ca_root_cert) + 1;
            }

            /* create libwebsockets context*/
            context = lws_create_context(&info);
            if (!context) {
                lwsl_err("lws init failed\n");
                return WM_ERR_FAILED;
            }


    **Connect to Server**

        The following is the process of connecting ``websocket client``  to ``server``:

        .. code:: C

            #define EXAMPLE_ALLOW_SELF_SIGNED 0

            struct lws *lws             = NULL;
            struct lws_client_connect_info ccinfo;

            const char* url_host = "echo.websocket.org";
            const char* url_path = "/";
            int url_port = 80;

            memset(&ccinfo, 0, sizeof ccinfo);
            ccinfo.context        = context;
            ccinfo.address        = url_host;
            ccinfo.port           = url_port;
            ccinfo.path           = url_path;
            ccinfo.host           = ccinfo.address;
            ccinfo.origin         = ccinfo.address;
            ccinfo.protocol       = g_protocols[0].name;
            ccinfo.ssl_connection = (is_ssl_connection ? LCCSCF_USE_SSL : 0);

            if (is_ssl_connection && EXAMPLE_ALLOW_SELF_SIGNED) {
                ccinfo.ssl_connection |= (LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK);
            }

            lws = lws_client_connect_via_info(&ccinfo);
            if (!lws){
                wm_log_error("connect fail");
                return WM_ERR_FAILED;
            }


    **Dispatch**

        .. code:: C

            while (!g_done) {
                lws_service(context, 100);
            }


    **Event process**

        ``LWS_CALLBACK_CLIENT_ESTABLISHED`` : Create websockets and connect end
        ``LWS_CALLBACK_CLIENT_RECEIVE`` : received some data from the server side
        ``LWS_CALLBACK_CLIENT_WRITEABLE`` : send some data to Server in the event

        .. code:: C

            static int client_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
            {
                wm_log_debug("reason=%d", reason);

                switch (reason) {
                    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                    {
                        wm_log_error("ev=CLIENT_CONNECTION_ERROR: %s", in ? (char *)in : "(null)");
                        g_done = 1;
                        break;
                    }
                    case LWS_CALLBACK_CLIENT_ESTABLISHED:
                    {
                        /* attach to websocket server ok */
                        const char *msg = "Hello, server!";
                        unsigned char buf[LWS_PRE + 1024];
                        unsigned char *p = &buf[LWS_PRE];

                        wm_log_info("ev=LWS_CALLBACK_CLIENT_ESTABLISHED");
                        wm_log_info("send message: \n%s", (char *)msg);

                        memcpy(p, msg, strlen(msg));
                        /* send msg to websocket server */
                        if (lws_write(wsi, p, strlen(msg), LWS_WRITE_TEXT) < 0) {
                            wm_log_error("Failed to send data to server");
                        }
                        break;
                    }
                    case LWS_CALLBACK_CLIENT_WRITEABLE:
                    {
                        break;
                    }
                    case LWS_CALLBACK_CLIENT_RECEIVE:
                    {
                        /* receive msg from websocket server */
                        wm_log_info("ev=LWS_CALLBACK_RECEIVE");
                        wm_log_info("Received message: \n%s", (char *)in);

                        break;
                    }
                    case LWS_CALLBACK_CLIENT_CLOSED:
                    {
                        wm_log_info("ev=LWS_CALLBACK_CLIENT_CLOSED");
                        g_done = 1;
                        break;
                    }
                    default:
                        break;
                }
                return 0;
            }

.. _websocket_server:

Webosocket Server
--------------------

    When using ``libwebsockets`` as a ``WS server`` to allow clients to connect, send data to clients, and receive data from clients,
    the process usually consists of several parts: creating a context, scheduling, and handling events. The sending and
    receiving of data are carried out during the handling of callback events.

    **Create Context**

        The following is the process of creating the websocket server context:

        .. code:: C

            struct lws_context_creation_info info = { 0 };
            struct lws_context *context;

            static struct lws_protocols protocols[] = {
                {
                    "echo_server", callback_websocket, 0, EXAMPLE_MAX_RECV_BUF_SIZE,
                },
                LWS_PROTOCOL_LIST_TERM
            };

            memset(&info, 0, sizeof(info));
            info.port      = CONFIG_EXAMPLE_PORT; //80 or 443
            info.protocols = protocols;
            info.gid       = -1;
            info.uid       = -1;

            #ifdef CONFIG_EXAMPLE_SECURITY
                info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

                info.server_ssl_cert_mem            = (unsigned char *)g_cert_mem;
                info.server_ssl_cert_mem_len        = strlen(g_cert_mem);
                info.server_ssl_private_key_mem     = (unsigned char *)g_key_mem;
                info.server_ssl_private_key_mem_len = strlen(g_key_mem);
            #endif

            /* Create the WebSocket context */
            context = lws_create_context(&info);
            if (!context) {
                wm_log_error("lws context creation failed");
                return WM_ERR_FAILED;
            }

    **Dispatch**

        .. code:: C

            while (1) {
                lws_service(context, 100);
            }


    **Event process**

        .. code:: C

            /* process websocket event */
            static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
            {
                switch (reason) {
                    case LWS_CALLBACK_ESTABLISHED:
                        /* Connected from client */
                        wm_log_info("Client connected\n");
                        break;

                    case LWS_CALLBACK_RECEIVE:
                        /* received data from client */
                        wm_log_info("Received msg from client: \n%.*s", (int)len, (char *)in);
                        lws_write(wsi, in, len, LWS_WRITE_TEXT);
                        wm_log_info("Send msg to client: \n%.*s", (int)len, (char *)in);
                        break;

                    case LWS_CALLBACK_CLIENT_WRITEABLE:
                        break;

                    case LWS_CALLBACK_CLOSED:
                        /* closed by client */
                        wm_log_info("Client disconnected\n");
                        break;

                    default:
                        break;
                }

                return 0;
            }


.. _http_server:

Http Server
---------------

    When using ``libwebsockets`` as an ``http server`` to allow clients to connect, the process usually involves several parts:
    creating a context, scheduling, and handling events. The sending and receiving of data are carried out during the
    handling of callback events. The processes of creating a context and scheduling are similar to those of a websocket server.
    However, there are some differences in the handling of callback events.


    **Event process**

        ``LWS_CALLBACK_HTTP`` : argument ``in`` is the request path, ``len`` is the length of the path.
        ``LWS_CALLBACK_HTTP_BODY`` : argument in the body of the  Client request , the  ``HTTP header`` is not included.
        ``LWS_CALLBACK_HTTP_BODY_COMPLETION`` : body received.

        .. code:: C

            static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
            {
                switch (reason) {
                    case LWS_CALLBACK_HTTP:
                    {
                        /*receive http header*/
                        wm_log_info("LWS_CALLBACK_HTTP");
                        wm_log_info("path:%s", (char *)in);
                        return 0;
                    }

                    case LWS_CALLBACK_HTTP_BODY:
                    {
                        /*receive http body*/
                        wm_log_info("LWS_CALLBACK_HTTP_BODY");
                        if (in) {
                            wm_log_info("recv body=\n%.*s", len, (char *)in);
                            return 0;
                        }
                        break;
                    }

                    case LWS_CALLBACK_HTTP_BODY_COMPLETION:
                    {
                        /*receive http body end*/
                        const char *response_json = "{\"status\": \"success\", \"message\": \"Received your request.\"}";
                        unsigned char buf[LWS_PRE + 256];
                        unsigned char *p = &buf[LWS_PRE];
                        int n;

                        wm_log_info("LWS_CALLBACK_HTTP_BODY_COMPLETION");

                        n = sprintf((char *)p,
                                    "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: application/json\r\n"
                                    "Content-Length: %u\r\n"
                                    "Connection: close\r\n"
                                    "\r\n"
                                    "%s",
                                    strlen(response_json), response_json);
                        /*write response to http client*/
                        lws_write(wsi, p, n, LWS_WRITE_HTTP);
                        wm_log_info("response=\n%s", response_json);

                        break;
                    }

                    default:
                        break;
                }

                return lws_callback_http_dummy(wsi, reason, user, in, len);
            }


    .. note::
        When creating an ``HTTP server`` using ``libwebsockets``, due to the lack of a ``Virtual File System (VFS)``, the file service part is not supported
        for the time being. In the reference examples, only POST and GET data request services are supported.

    .. note::
        When creating an ``HTTP server`` or a ``WebSockets server`` using libwebsockets, the number of supported clients is approximately 3 to 5.
        The reason is the limitation of the number of the LWIP connections, Increasing the ``MEMP_NUM_NETCONN`` macro in the ``lwipopts.h`` file can support more clients.

Examples
----------
    Websocket examples :ref:`examples/network <network_example>` :

    ``websocket client`` : refer to examples/network/websocket/websocket_client/

    ``websocket server`` : refer to examples/network/websocket/websocket_server/

    ``http server`` : refer to examples/network/httpd/http_server_post/
