/* CoAP client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * WARNING
 * libcoap is not multi-thread safe, so only this thread must make any coap_*()
 * calls.  Any external (to this thread) data transmitted in/out via libcoap
 * therefore has to be passed in/out by xQueue*() via this thread.
 */

#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "coap_config.h"
#include "coap3/coap.h"
#include "wm_nm_api.h"

#define LOG_TAG "coap_cli"
#include "wm_log.h"

#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

#ifndef CONFIG_COAP_CLIENT_SUPPORT
#error COAP_CLIENT_SUPPORT needs to be enabled
#endif /* COAP_CLIENT_SUPPORT */

#define COAP_DEFAULT_TIME_SEC 60

#ifdef CONFIG_EXAMPLE_COAP_MBEDTLS_PKI
extern const uint8_t mbedtls_demos_ca_cert[] asm("__binary_main_certs_ca_cert_rsa_sha256_pem");
extern const uint32_t mbedtls_demos_ca_cert_len asm("__binary_main_certs_ca_cert_rsa_sha256_pem_length");

extern const uint8_t mbedtls_demos_cert[] asm("__binary_main_certs_cert_rsa_pem");
extern const uint32_t mbedtls_demos_cert_len asm("__binary_main_certs_cert_rsa_pem_length");

extern const uint8_t mbedtls_demos_private_key[] asm("__binary_main_certs_private_key_rsa_pem");
extern const uint32_t mbedtls_demos_private_key_len asm("__binary_main_certs_private_key_rsa_pem_length");
#endif

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static int resp_wait           = 1;
static coap_optlist_t *optlist = NULL;
static int wait_ms;

static coap_response_t message_handler(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received,
                                       const coap_mid_t mid)
{
    const unsigned char *data = NULL;
    size_t data_len;
    size_t offset;
    size_t total;
    coap_pdu_code_t rcvd_code = coap_pdu_get_code(received);

    if (COAP_RESPONSE_CLASS(rcvd_code) == 2) {
        if (coap_get_data_large(received, &data_len, &data, &offset, &total)) {
            if (data_len != total) {
                wm_log_info("Unexpected partial data received offset %u, length %u\n", offset, data_len);
            }
            wm_log_info("Received:\n%.*s\n", (int)data_len, data);
            resp_wait = 0;
        }
        return COAP_RESPONSE_OK;
    }
    wm_log_info("%d.%02d", (rcvd_code >> 5), rcvd_code & 0x1F);
    if (coap_get_data_large(received, &data_len, &data, &offset, &total)) {
        wm_log_dump_info("recv:", data, data_len);
    }
    resp_wait = 0;
    return COAP_RESPONSE_OK;
}

static void coap_log_handler(coap_log_t level, const char *message)
{
    const char *cp = strchr(message, '\n');
    while (cp) {
        wm_log_info("%.*s", (int)(cp - message), message);
        message = cp + 1;
        cp      = strchr(message, '\n');
    }
    if (message[0] != '\000') {
        wm_log_info("%s", message);
    }
}

#ifdef CONFIG_EXAMPLE_COAP_MBEDTLS_PKI
static coap_session_t *coap_start_pki_session(coap_context_t *ctx, coap_address_t *dst_addr, coap_uri_t *uri,
                                              coap_proto_t proto)
{
    static coap_dtls_pki_t dtls_pki;

    memset(&dtls_pki, 0, sizeof(dtls_pki));
    dtls_pki.version                 = COAP_DTLS_PKI_SETUP_VERSION;
    dtls_pki.verify_peer_cert        = 0;
    dtls_pki.check_common_ca         = 0;
    dtls_pki.allow_self_signed       = 1;
    dtls_pki.allow_expired_certs     = 1;
    dtls_pki.cert_chain_validation   = 1;
    dtls_pki.cert_chain_verify_depth = 2;
    dtls_pki.check_cert_revocation   = 1;
    dtls_pki.allow_no_crl            = 1;
    dtls_pki.allow_expired_crl       = 1;
    dtls_pki.allow_bad_md_hash       = 1;
    dtls_pki.allow_short_rsa_length  = 1;
    dtls_pki.validate_cn_call_back   = NULL;
    dtls_pki.cn_call_back_arg        = NULL;
    dtls_pki.validate_sni_call_back  = NULL;
    dtls_pki.sni_call_back_arg       = NULL;

    dtls_pki.pki_key.key_type                    = COAP_PKI_KEY_PEM_BUF;
    dtls_pki.pki_key.key.pem_buf.public_cert     = mbedtls_demos_ca_cert;
    dtls_pki.pki_key.key.pem_buf.public_cert_len = mbedtls_demos_ca_cert_len;
    dtls_pki.pki_key.key.pem_buf.private_key     = mbedtls_demos_private_key;
    dtls_pki.pki_key.key.pem_buf.private_key_len = mbedtls_demos_private_key_len;
    dtls_pki.pki_key.key.pem_buf.ca_cert         = mbedtls_demos_cert;
    dtls_pki.pki_key.key.pem_buf.ca_cert_len     = mbedtls_demos_ca_cert_len;

    return coap_new_client_session_pki(ctx, NULL, dst_addr, proto, &dtls_pki);
}
#endif

static void coap_example_client(void *p)
{
    coap_address_t dst_addr;
    static coap_uri_t uri;
    const char *server_uri  = CONFIG_EXAMPLE_COAP_DEFAULT_URI;
    coap_context_t *ctx     = NULL;
    coap_session_t *session = NULL;
#ifdef CONFIG_EXAMPLE_COAP_MBEDTLS_PKI
    uint16_t port = COAPS_DEFAULT_PORT;
#else
    uint16_t port = COAP_DEFAULT_PORT;
#endif
    coap_pdu_t *request = NULL;
    unsigned char token[8];
    size_t tokenlength;
    coap_addr_info_t *info_list = NULL;
    coap_proto_t proto;
    char tmpbuf[INET6_ADDRSTRLEN];
#define BUFSIZE 100
    unsigned char uri_path[BUFSIZE];
    wm_log_info("CoAP Client Start !");
    /* Initialize libcoap library */

    coap_startup();

    /* Set up the CoAP logging */
    coap_set_log_handler(coap_log_handler);
    coap_set_log_level(COAP_MAX_LOGGING_LEVEL);
    coap_dtls_set_log_level(COAP_MAX_LOGGING_LEVEL);

    /* Set up the CoAP context */
    ctx = coap_new_context(NULL);
    if (!ctx) {
        wm_log_error("coap_new_context() failed");
        goto clean_up;
    }

    coap_context_set_block_mode(ctx, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
    coap_register_response_handler(ctx, message_handler);

    if (coap_split_uri((const uint8_t *)server_uri, strlen(server_uri), &uri) == -1) {
        wm_log_error("CoAP server uri %s error", server_uri);
        goto clean_up;
    }

    info_list = coap_resolve_address_info(&uri.host, port, port, port, port, 0, 1 << uri.scheme, COAP_RESOLVE_TYPE_REMOTE);

    if (info_list == NULL) {
        wm_log_error("failed to resolve address");
        goto clean_up;
    }
    proto = info_list->proto;
    memcpy(&dst_addr, &info_list->addr, sizeof(dst_addr));
    coap_free_address_info(info_list);

    /* Convert provided uri into CoAP options */
    if (coap_uri_into_options(&uri, &dst_addr, &optlist, 1, uri_path, sizeof(uri_path)) < 0) {
        wm_log_error("Failed to create options for URI %s", server_uri);
        goto clean_up;
    }
    /* This is to keep the test suites happy */
    coap_print_ip_addr(&dst_addr, tmpbuf, sizeof(tmpbuf));
#ifdef CONFIG_EXAMPLE_COAP_MBEDTLS_PKI
    if (uri.scheme == COAP_URI_SCHEME_COAPS || uri.scheme == COAP_URI_SCHEME_COAPS_TCP ||
        uri.scheme == COAP_URI_SCHEME_COAPS_WS) {
        session = coap_start_pki_session(ctx, &dst_addr, &uri, proto);
    } else
#endif
    {
        session = coap_new_client_session(ctx, NULL, &dst_addr, proto);
    }
    if (!session) {
        wm_log_error("coap_new_client_session() failed");
        goto clean_up;
    }

    while (1) {
        request = coap_new_pdu(coap_is_mcast(&dst_addr) ? COAP_MESSAGE_NON : COAP_MESSAGE_CON, COAP_REQUEST_CODE_GET, session);
        if (!request) {
            wm_log_error("coap_new_pdu() failed");
            goto clean_up;
        }
        /* Add in an unique token */
        coap_session_new_token(session, &tokenlength, token);
        coap_add_token(request, tokenlength, token);

        coap_add_optlist_pdu(request, &optlist);

        resp_wait = 1;
        coap_show_pdu(COAP_LOG_INFO, request);
        coap_send(session, request);

        wait_ms = COAP_DEFAULT_TIME_SEC * 1000;
        while (resp_wait) {
            int result = coap_io_process(ctx, wait_ms > 1000 ? 1000 : wait_ms);

            if (result >= 0) {
                if (result >= wait_ms) {
                    wm_log_debug("No response from server");
                    break;
                } else {
                    wait_ms -= result;
                }
            }
        }
        for (int countdown = 10; countdown >= 0; countdown--) {
            wm_log_debug("%d... ", countdown);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        wm_log_info("Starting again!");
    }
clean_up:
    if (optlist) {
        coap_delete_optlist(optlist);
        optlist = NULL;
    }

    if (session) {
        coap_session_release(session);
    }

    if (ctx) {
        coap_free_context(ctx);
    }
    coap_cleanup();

    wm_log_info("Finished");
    vTaskDelete(NULL);
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
    xTaskCreate(coap_example_client, "coap-client", 2048, NULL, 9, NULL);
    return 0;
}
