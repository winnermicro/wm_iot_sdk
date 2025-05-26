#include <stdio.h>
#include "wmsdk_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_http_client.h"
#include "cJSON/cJSON.h"

#define LOG_TAG "llm"
#include "wm_log.h"

#define HTTPC_CLIENT_DEFAULT_TIMEOUT 60000

/* The buffer for HTTP Tx and Rx */
#define HTTPC_CLIENT_BUFFER_SIZE     (1024 * 4)

/* The buffer to store sending JSON content */
/* Most of LLM has a similar JSON content structure
{
    "model": "deepseek-chat",
    "messages": [
        {"role": "system", "content": "You are a helpful assistant."},
        {"role": "user", "content": "Hello!"}
    ],
    "stream": false
}
*/

/**
 * @brief Process the response body from the LLM server.
 * 
 * This function parses the JSON response body, extracts relevant information
 * such as the role and content, and logs the information.
 * 
 * @param body The response body received from the LLM server.
 */
static void process_resp(const char *body)
{
    cJSON *choices;
    cJSON *message;
    cJSON *role;
    cJSON *content;

    cJSON *resp = cJSON_Parse(body);
    if (!resp) {
        return;
    }

    if (cJSON_GetObjectItem(resp, "error")) {
        wm_log_error("err body=\r\n%s", body);
        goto END;
    }

    choices = cJSON_GetObjectItem(resp, "choices");
    if (!choices) {
        wm_log_error("err no choices body=\r\n%s", body);
        goto END;
    }

    message = cJSON_GetObjectItem(cJSON_GetArrayItem(choices, 0), "message");
    if (!message) {
        wm_log_error("err no message body=\r\n%s", body);
        goto END;
    }

    role    = cJSON_GetObjectItem(message, "role");
    content = cJSON_GetObjectItem(message, "content");

    if (content && content->type == cJSON_String) {
        wm_log_raw_info("%s:  %s\r\n", role ? role->valuestring : "", content->valuestring);
    }

END:
    cJSON_Delete(resp);
}

/**
 * @brief Handle HTTP client events.
 * 
 * This function logs different HTTP client events such as connection, disconnection,
 * header sent, header received, data received, and finish.
 * 
 * @param session The HTTP client session.
 * @param event The HTTP client event.
 * @param param The event parameter.
 * @param priv Private data.
 */
static void wm_httpc_event_handle(wm_http_client_t session, wm_http_client_event_t event, wm_http_client_event_param_t *param,
                                  void *priv)
{
    switch (event) {
        case WM_HTTP_CLIENT_EVENT_CONNECTED:
        {
            wm_log_info("WM_HTTP_CLIENT_EVENT_CONNECTED");
            break;
        }
        case WM_HTTP_CLIENT_EVENT_DISCONNECTED:
        {
            wm_log_info("WM_HTTP_CLIENT_EVENT_DISCONNECTED");
            break;
        }
        case WM_HTTP_CLIENT_EVENT_HEADER_SENTED:
        {
            wm_log_info("WM_HTTP_CLIENT_EVENT_HEADER_SENTED");
            //wm_log_info("%s", (char *)(param->data));
            break;
        }
        case WM_HTTP_CLIENT_EVENT_RECV_HEADER:
        {
            wm_log_info("WM_HTTP_CLIENT_EVENT_RECV_HEADER");
            //wm_log_info("%s", (char *)(param->data));
            break;
        }
        case WM_HTTP_CLIENT_EVENT_RECV_DATA:
        {
            wm_log_info("WM_HTTP_CLIENT_EVENT_RECV_DATA");
            //wm_log_info("%s", (char *)(param->data));
            break;
        }
        case WM_HTTP_CLIENT_EVENT_FINISH:
        {
            wm_log_info("WM_HTTP_CLIENT_EVENT_FINISH");
            break;
        }
        default:
            break;
    }
}

/**
 * @brief Send an HTTP request to the LLM server.
 * 
 * This function opens an HTTP request, adds headers, sends the request,
 * receives the response, and processes the received data.
 * 
 * @param cfg The HTTP client configuration.
 * @param url The URL of the LLM server.
 * @param apikey The API key for authentication.
 * @param data The data to be sent in the request.
 * @param len The length of the data.
 * @return The result of the operation.
 */
static int wm_llm_send_http_req(wm_http_client_config_t *cfg, char *url, char *apikey, char *data, uint32_t len)
{
    int ret = WM_ERR_FAILED;
    size_t size, total = 0;
    char *buffer             = NULL;
    wm_http_client_t session = 0;
    int nRetCode;

    buffer = (char *)malloc(HTTPC_CLIENT_BUFFER_SIZE);
    if (buffer == NULL) {
        ret = WM_ERR_NO_MEM;
        goto CLEANUP;
    }
    memset(buffer, 0, HTTPC_CLIENT_BUFFER_SIZE);

    session = wm_http_client_open_request(cfg);
    if (!session) {
        ret = WM_ERR_FAILED;
        goto CLEANUP;
    }

    nRetCode = wm_http_client_add_request_headers(session, "Authorization", apikey);
    if (nRetCode != HTTP_CLIENT_SUCCESS) {
        ret = WM_ERR_FAILED;
        goto CLEANUP;
    }

    ret = wm_http_client_send_request(session, url, data, len, true, 0, 0);
    if (ret != WM_ERR_SUCCESS) {
        goto CLEANUP;
    }

    ret = wm_http_client_recv_response(session, HTTPC_CLIENT_DEFAULT_TIMEOUT);
    if (ret != WM_ERR_SUCCESS) {
        goto CLEANUP;
    }
    wm_log_info("Start to receive data from remote server...");

    // Continuously receive data until an error occurs or the end of the data stream is reached
    while (1) {
        ret = wm_http_client_read_data(session, buffer + total, HTTPC_CLIENT_BUFFER_SIZE - total, HTTPC_CLIENT_DEFAULT_TIMEOUT,
                                       &size);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        total += size;

        if (total >= (HTTPC_CLIENT_BUFFER_SIZE - 1)) { //HTTP recv buffer needs to '\0'
            wm_log_info("\n\n LLM Answer content too long (>=%d)\n", HTTPC_CLIENT_BUFFER_SIZE);
            break;
        }
    }

    // Process the received data
    if (strlen(buffer) > 0) {
        wm_log_info("\n\n【LLM Answer】:");
        process_resp(buffer);
        wm_log_info("\n\n【LLM Answer End】:");
    }

CLEANUP:
    if (buffer) {
        free(buffer);
    }
    if (session) {
        wm_http_client_close_request(&session);
    }
    return ret;
}

/**
 * @brief Send a POST request to the LLM server.
 * 
 * This function constructs a JSON payload, configures the HTTP client,
 * and sends a POST request to the LLM server.
 * 
 * @param url The URL of the LLM server.
 * @param model The model to be used.
 * @param apikey The API key for authentication.
 * @param role The role in the JSON payload.
 * @param content The content in the JSON payload.
 * @return The result of the operation.
 */
int wm_llm_post(char *url, char *model, char *apikey, char *role, char *content)
{
    int ret                     = WM_ERR_FAILED;
    wm_http_client_config_t cfg = { 0 };

    if (!model || !role || !content || !apikey) {
        return WM_ERR_FAILED;
    }

    size_t json_data_len =
        snprintf(NULL, 0, "{\"model\":\"%s\",\"messages\":[{\"role\":\"%s\",\"content\":\"%s\"}],\"api-key\":\"%s\"}", model,
                 role, content, apikey) +
        1;

    if (json_data_len >= HTTPC_CLIENT_BUFFER_SIZE) {
        wm_log_info("input string exceed max support len (%d)", HTTPC_CLIENT_BUFFER_SIZE);
        return WM_ERR_FAILED;
    }

    char *json_data = (char *)malloc(json_data_len);
    if (json_data == NULL) {
        return WM_ERR_FAILED;
    }

    snprintf(json_data, json_data_len,
             "{\"model\":\"%s\",\"messages\":[{\"role\":\"%s\",\"content\":\"%s\"}],\"api-key\":\"%s\"}", model, role, content,
             apikey);

    cfg.method        = WM_HTTP_CLIENT_REQUEST_TYPE_POST;
    cfg.content_type  = WM_HTTP_CLIENT_CONTENT_APPLICATION_JSON;
    cfg.event_handler = wm_httpc_event_handle;

    //wm_log_info("json_data=%s", json_data);
    ret = wm_llm_send_http_req(&cfg, url, apikey, json_data, strlen(json_data));

    free(json_data);

    return ret;
}

/**
 * @brief Start the AI LLM demo.
 * 
 * This function logs a message indicating the start of the AI LLM demo
 * and provides information on how to get help.
 */
void app_ai_llm_demo(void)
{
    wm_log_info("Start to chat with LLM by input cli command!");
    wm_log_info("CMD: llm --help  for the details");
}