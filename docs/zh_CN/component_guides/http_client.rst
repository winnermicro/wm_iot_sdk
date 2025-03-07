
HTTP Client
================

概述
--------

``HTTP Client`` 提供了一些 API 用于完成 HTTP 协议请求，相关的响应会通过同步事件进行回调。进行 HTTP 请求的具体步骤如下：

- ``wm_http_client_open_request()``：要使用 HTTP 客户端请求数据，我们首先要做的就是为请求分配适当的空间，并进行请求前的初始化工作。HTTP Client 提供了对应的 ``同步事件``，用户可以在接口中定义自己的同步回调，便于用户去控制和操作自己的 HTTP 请求。具体同步事件如下：

   - WM_HTTP_CLIENT_EVENT_CONNECTED（HTTP 连接成功）
   - WM_HTTP_CLIENT_EVENT_DISCONNECTED（HTTP 断开连接）
   - WM_HTTP_CLIENT_EVENT_HEADER_SENTED（HTTP 请求头发送成功）
   - WM_HTTP_CLIENT_EVENT_RECV_HEADER（HTTP 接收到请求头信息）
   - WM_HTTP_CLIENT_EVENT_RECV_DATA（HTTP 接收到请求数据）
   - WM_HTTP_CLIENT_EVENT_FINISH（HTTP 请求完成）

- ``wm_http_client_send_request()``：准备工作完成后，就可以进行 HTTP 请求，这里需要配置请求的 URL，数据等内容。
- ``wm_http_client_recv_response()``：一旦我们发出 HTTP 请求后，就会收到来自服务器的响应。我们调用该接口可以获取到请求头信息，通过解析请求头，我们可以获取到请求数据的类型，长度等信息。
- ``wm_http_client_read_data()``：在调用了 ``wm_http_client_recv_response()`` 之后，我们调用该接口才能获取到请求的数据。
- ``wm_http_client_close_request``：当我们完成一次请求之后，我们需要关闭这次连接，并释放相应的资源。

应用实例
------------

.. code:: C

    void wm_httpc_event_handle(wm_http_client_t session, wm_http_client_event_t event, wm_http_client_event_param_t *param, void *priv)
    {
        switch(event) 
        {
            case WM_HTTP_CLIENT_EVENT_CONNECTED:
                wm_log_info("WM_HTTP_CLIENT_EVENT_CONNECTED");
                break;
            case WM_HTTP_CLIENT_EVENT_DISCONNECTED:
                wm_log_info("WM_HTTP_CLIENT_EVENT_DISCONNECTED");
                break;
            case WM_HTTP_CLIENT_EVENT_HEADER_SENTED:
                wm_log_info("WM_HTTP_CLIENT_EVENT_HEADER_SENTED");
                wm_log_info("%s", (char *)(param->data));
                break;
            case WM_HTTP_CLIENT_EVENT_RECV_HEADER:
                wm_log_info("WM_HTTP_CLIENT_EVENT_RECV_HEADER");
                wm_log_info("%s", (char *)(param->data));
                break;
            case WM_HTTP_CLIENT_EVENT_RECV_DATA:
                wm_log_info("WM_HTTP_CLIENT_EVENT_RECV_DATA");
                wm_log_info("%s", (char *)(param->data));
                break;
            case WM_HTTP_CLIENT_EVENT_FINISH:
                wm_log_info("WM_HTTP_CLIENT_EVENT_FINISH");
                break;
        }
        
        return;
    }

    uint32_t wm_httpc_example_send_req(wm_http_client_config_t *cfg, char *url, char *data, uint32_t len)
    {
        int ret;
        size_t size, total = 0;
        char *buffer = NULL;
        wm_http_client_t session;
        size_t buffer_size = 1024;
        size_t default_timeout = 30000;

        do
        {
            buffer = (char *)malloc(buffer_size);
            if (buffer == NULL) {
                return WM_ERR_NO_MEM;
            }
            memset(buffer, 0, buffer_size);

            // Open the HTTP request handle
            session = wm_http_client_open_request(cfg);
            if (!session) {
                ret = WM_ERR_FAILED;
                break;
            }

            if ((ret = wm_http_client_send_request(session, url, data, len, true, 0, 0)) != WM_ERR_SUCCESS) {
                break;
            }

            // Retrieve the the headers and analyze them
            if ((ret = wm_http_client_recv_response(session, default_timeout)) != WM_ERR_SUCCESS) {            
                break;
            }

            wm_log_info("Start to receive data from remote server...");

            // Get the data until we get an error or end of stream code
            while (ret == WM_ERR_SUCCESS || ret != WM_ERR_EOS) {
                // Set the size of our buffer
                size = buffer_size;
                // Get the data
                ret = wm_http_client_read_data(session, buffer, size, default_timeout, &size);
                if(ret != WM_ERR_SUCCESS && ret != WM_ERR_EOS)
                    break;
                total += size;
            }
        } while(0);

        free(buffer);

        if (session)
            ret = wm_http_client_close_request(&session);

        return ret;
    }

    wm_http_client_config_t cfg = {0};
    cfg.method = WM_HTTP_CLIENT_REQUEST_TYPE_GET;
    cfg.event_handler = wm_httpc_event_handle;
    wm_httpc_example_send_req(&cfg, "http://www.baidu.com/img/bdlogo.gif", NULL, 0);


HTTPS
--------

HTTP Client 支持使用 ``mbedtls`` 的 SSL 连接，URL 以 ``https`` 开头即可使用 HTTPS 请求。


HTTP Client 的 meunconfig 配置
----------------------------------

主要配置如下：

.. list-table::
   :widths: 45 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值
     
   * - CONFIG_COMPONENT_HTTP_CLIENT_ENABLED
     - 是否启用 HTTP Client 组件
     - N

   * - CONFIG_WM_HTTP_CLIENT_SECURE
     - 是否支持安全功能
     - N

   * - CONFIG_WM_HTTP_CLIENT_PROXY
     - 是否支持代理功能
     - N

   * - CONFIG_WM_HTTP_CLIENT_DEBUG 
     - 是否显示调试消息
     - N


注意事项
--------

当前版本 HTTP client 不支持 IPv6
