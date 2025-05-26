
.. _httpd_websockets:

Httpd and Websocket
====================

简介
---------------

    本文档对应 ``components/websockets`` 组件, 该组件移植开源 ``libwebsockets 4.3.3`` 版本 。主要移植和适配了 3 个方面的功： ``http server``, ``Websocket server``, ``Websocket client`` ， 同时也对 ``HTTPS`` 和 ``WSS`` 功能做了适配。

    移植 ``libwebsockets`` 时, 没有修改它的 API ，也没有在其基础上额外封装接口，所以在开发时，除了可以参考本 SDK 中相关 examples 外, 还可以参考 ``libwebsockets`` 原生的 examples 和 API 接口。


libwebsockets
---------------

    官网地址: `https://libwebsockets.org/ <https://libwebsockets.org/>`_

    源码地址: `https://github.com/warmcat/libwebsockets <https://github.com/warmcat/libwebsockets>`_

    接口文档: `https://libwebsockets.org/lws-api-doc-main/html/index.html <https://libwebsockets.org/lws-api-doc-main/html/index.html>`_


功能列表
---------------

    1. :ref:`配置 <config>`

    2. :ref:`Websocket Client <websocket_client>`

    3. :ref:`Websocket Server <websocket_server>`

    4. :ref:`Http Server <http_server>`

.. _config:

配置
-------------

    websockets 组件有 2 个配置

    ``CONFIG_COMPONENT_WEBSOCKET_ENABLED`` 配置打开 websockets 模块；

    ``CONFIG_WEBSOCKET_SECURITY`` 配置使用安全，打开后 HTTPS, WSS 才能使用。

    其他模块相关配置：

    ``CONFIG_COMPONENT_LWIP_ENABLED`` 打开 LWIP, 必须打开；

    ``LWIP_ENABLE_IPV6`` 打开后 websockets 模块才能支持 IPV6 功能。


.. _websocket_client:

Websocket Client
-------------------

    使用 ``libwebsockets`` 作为 ``WS client`` 链接服务器，发送数据给 Server 和接收 Server 发过来的程序，通常有创建 Context, 连接 server, 调度和事件处理几个步骤，其中发送和接收数据是在回调事件处理中进行的。


    **创建 Context**

        下面是创建 ``websocket client`` 端 context 的过程：

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


    **连接 Server**

        下面是 ``websocket client`` 连接 ``server`` 端的过程：

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


    **调度：**

        .. code:: C

            while (!g_done) {
                lws_service(context, 100);
            }


    **事件处理**

        ``LWS_CALLBACK_CLIENT_ESTABLISHED`` : 创建 websockets 连接完成，完成后可以开始数据发送；
        ``LWS_CALLBACK_CLIENT_RECEIVE`` : 接收到 Server 数据；
        ``LWS_CALLBACK_CLIENT_WRITEABLE`` : 在该事件中发送数据到 Server ；

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

    使用 ``libwebsockets`` 作为 ``WS server`` , 让 Client 连接进来，发送数据给 Client 和接收来自 Client 发过来的数据，通常有创建 Context, 调度和处理事件几个部分，其中发送和接收数据是在回调事件处理中进行的。


    **创建 Context**

        下面是创建 websocket server 端 context 的过程：

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

    **调度**

        .. code:: C

            while (1) {
                lws_service(context, 100);
            }


    **回调事件处理**

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

    使用 ``libwebsockets`` 作为 ``http server`` , 让 Client 连接进来，通常有创建 Context, 调度和处理事件几个部分，其中发送和接收数据是在回调事件处理中进行的。创建 context 和调度部分和 websocket server类似。回调事件事件处理有一些差异。

    **回调事件处理**

        ``LWS_CALLBACK_HTTP`` : 参数 in 是请求路径，len是路径长度
        ``LWS_CALLBACK_HTTP_BODY`` : 参数 in 是收到 Client 请求的消息提，不包括 ``HTTP header`` 部分
        ``LWS_CALLBACK_HTTP_BODY_COMPLETION`` ： body 接收完成事件

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
        使用 ``libwebsockets`` 创建 http Server，因缺少 ``VFS`` , 暂时不支持文件服务部分，参考示例中只支持 post, get 数据请求服务。

    .. note::
        使用 ``libwebsockets`` 创建 http Server 或者 Websockets server 时，支持的 Client 数量大概是 3 ~ 5 个，原因是 LWIP 连接数量限制，增大 lwipopts.h 文件中的 MEMP_NUM_NETCONN 宏可以支持更多。

应用实例
----------
    使用 websocket 基本示例请参照 :ref:`examples/network <network_example>` 其中：

    ``websocket client`` : 示例请参照 examples/network/websocket/websocket_client/

    ``websocket server`` : 示例请参照 examples/network/websocket/websocket_server/

    ``http server`` : 示例请参照 examples/network/httpd/http_server_post/

