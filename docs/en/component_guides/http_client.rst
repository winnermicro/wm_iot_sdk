
HTTP Client
================

Overview
----------

The ``HTTP Client`` provides several APIs to perform HTTP protocol requests, and the relevant responses are handled through synchronous events. The steps to perform an HTTP request are as follows:

- ``wm_http_client_open_request()`` To use the HTTP client to request data, the first step is to allocate appropriate space for the request and perform pre-request initialization. HTTP Client provides corresponding ``synchronous events`` where users can define their own synchronous callbacks, allowing them to control and operate their HTTP requests. The specific synchronous events are as follows:

   - WM_HTTP_CLIENT_EVENT_CONNECTED(HTTP connection successful)
   - WM_HTTP_CLIENT_EVENT_DISCONNECTED(HTTP disconnected)
   - WM_HTTP_CLIENT_EVENT_HEADER_SENTED(HTTP request header sent successfully)
   - WM_HTTP_CLIENT_EVENT_RECV_HEADER(HTTP received request header information)
   - WM_HTTP_CLIENT_EVENT_RECV_DATA (HTTP received request data)
   - WM_HTTP_CLIENT_EVENT_FINISH (HTTP request completed)

- ``wm_http_client_send_request()`` : After completing the preparation work, you can perform the HTTP request, where you need to configure the request URL, data, and other contents.
- ``wm_http_client_recv_response()`` : Once we send the HTTP request, we will receive a response from the server. By calling this interface, we can obtain the request header information. By parsing the request header, we can get the type and length of the request data.
- ``wm_http_client_read_data()`` : After calling  ``wm_http_client_recv_response()``, we can call this interface to obtain the requested data.
- ``wm_http_client_close_request`` After completing a request, we need to close the connection and release the corresponding resources.

Application Example
---------------------

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

The HTTP Client supports SSL connections using ``mbedtls`` , which can be used by starting the URL with  ``https`` .


Attention
----------

The current version of HTTP client does not support IPv6
