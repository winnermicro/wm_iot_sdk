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

#define LOG_TAG "coap_srv"
#include "wm_log.h"

#ifndef CONFIG_COAP_SERVER_SUPPORT
#error COAP_SERVER_SUPPORT needs to be enabled
#endif /* COAP_CLIENT_SUPPORT */

#ifndef CONFIG_EXAMPLE_WIFI_PASSWORD
#define CONFIG_EXAMPLE_WIFI_PASSWORD NULL
#endif

#define COAP_DEFAULT_TIME_SEC 60

#ifdef CONFIG_EXAMPLE_COAP_MBEDTLS_PKI
extern const uint8_t mbedtls_demos_ca_cert[] asm("__binary_main_certs_ca_cert_rsa_sha256_pem");
extern const uint32_t mbedtls_demos_ca_cert_len asm("__binary_main_certs_ca_cert_rsa_sha256_pem_length");
/* This is taken from tests/data_files/srv_cert_rsa_sha256.pem. */
extern const uint8_t mbedtls_demos_srv_cert[] asm("__binary_main_certs_srv_cert_rsa_sha256_pem");
extern const uint32_t mbedtls_demos_srv_cert_len asm("__binary_main_certs_srv_cert_rsa_sha256_pem_length");
/* This is taken from tests/data_files/srv-rsa.key. */
extern const uint8_t mbedtls_demos_srv_key[] asm("__binary_main_certs_srv_key_rsa_pem");
extern const uint32_t mbedtls_demos_srv_key_len asm("__binary_main_certs_srv_key_rsa_pem_length");
#endif

static char resp_data[100];
static int resp_data_len = 0;

#define INITIAL_DATA "Hello World!"

static void hnd_coap_get(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request,
                         const coap_string_t *query, coap_pdu_t *response)
{
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
    coap_add_data_large_response(resource, session, request, response, query, COAP_MEDIATYPE_TEXT_PLAIN, 60, 0,
                                 (size_t)resp_data_len, (const u_char *)resp_data, NULL, NULL);
}

static void hnd_coap_put(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request,
                         const coap_string_t *query, coap_pdu_t *response)
{
    size_t size;
    size_t offset;
    size_t total;
    const unsigned char *data;

    coap_resource_notify_observers(resource, NULL);

    if (strcmp(resp_data, INITIAL_DATA) == 0) {
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_CREATED);
    } else {
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
    }

    /* coap_get_data_large() sets size to 0 on error */
    (void)coap_get_data_large(request, &size, &data, &offset, &total);

    if (size == 0) { /* re-init */
        snprintf(resp_data, sizeof(resp_data), INITIAL_DATA);
        resp_data_len = strlen(resp_data);
    } else {
        resp_data_len = size > sizeof(resp_data) ? sizeof(resp_data) : size;
        memcpy(resp_data, data, resp_data_len);
    }
}

static void hnd_coap_delete(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request,
                            const coap_string_t *query, coap_pdu_t *response)
{
    coap_resource_notify_observers(resource, NULL);
    snprintf(resp_data, sizeof(resp_data), INITIAL_DATA);
    resp_data_len = strlen(resp_data);
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_DELETED);
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

static void coap_example_server(void *p)
{
    coap_context_t *ctx       = NULL;
    coap_resource_t *resource = NULL;
    int have_ep               = 0;
    uint32_t scheme_hint_bits;
    char tmpbuf[INET6_ADDRSTRLEN];

    /* Initialize libcoap library */
    coap_startup();

    /* Set up the CoAP logging */
    coap_set_log_handler(coap_log_handler);
    coap_set_log_level(COAP_MAX_LOGGING_LEVEL);
    coap_dtls_set_log_level(COAP_MAX_LOGGING_LEVEL);

    snprintf(resp_data, sizeof(resp_data), INITIAL_DATA);
    resp_data_len = strlen(resp_data);

    while (1) {
        unsigned wait_ms;
        coap_addr_info_t *info      = NULL;
        coap_addr_info_t *info_list = NULL;

        ctx = coap_new_context(NULL);
        if (!ctx) {
            wm_log_error("coap_new_context() failed");
            goto clean_up;
        }
        coap_context_set_block_mode(ctx, COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY);
        coap_context_set_max_idle_sessions(ctx, 20);

#ifdef CONFIG_EXAMPLE_COAP_MBEDTLS_PKI
        if (coap_dtls_is_supported()) {
            wm_log_debug("DTLS Already Ready \r\n");
            coap_dtls_pki_t dtls_pki                     = { 0 };
            dtls_pki.version                             = COAP_DTLS_PKI_SETUP_VERSION;
            dtls_pki.verify_peer_cert                    = 0;
            dtls_pki.check_common_ca                     = 0;
            dtls_pki.allow_self_signed                   = 1;
            dtls_pki.allow_expired_certs                 = 1;
            dtls_pki.cert_chain_validation               = 1;
            dtls_pki.cert_chain_verify_depth             = 2;
            dtls_pki.check_cert_revocation               = 1;
            dtls_pki.allow_no_crl                        = 1;
            dtls_pki.allow_expired_crl                   = 1;
            dtls_pki.allow_bad_md_hash                   = 1;
            dtls_pki.allow_short_rsa_length              = 1;
            dtls_pki.validate_cn_call_back               = NULL;
            dtls_pki.cn_call_back_arg                    = NULL;
            dtls_pki.validate_sni_call_back              = NULL;
            dtls_pki.sni_call_back_arg                   = NULL;
            dtls_pki.pki_key.key_type                    = COAP_PKI_KEY_PEM_BUF;
            dtls_pki.pki_key.key.pem_buf.public_cert     = mbedtls_demos_srv_cert;
            dtls_pki.pki_key.key.pem_buf.public_cert_len = mbedtls_demos_srv_cert_len;
            dtls_pki.pki_key.key.pem_buf.private_key     = mbedtls_demos_srv_key;
            dtls_pki.pki_key.key.pem_buf.private_key_len = mbedtls_demos_srv_key_len;
            dtls_pki.pki_key.key.pem_buf.ca_cert         = mbedtls_demos_ca_cert;
            dtls_pki.pki_key.key.pem_buf.ca_cert_len     = mbedtls_demos_ca_cert_len;
            coap_context_set_pki(ctx, &dtls_pki);
        }
#endif
        /* set up the CoAP server socket(s) */
#ifdef CONFIG_EXAMPLE_COAP_MBEDTLS_PKI
        scheme_hint_bits = coap_get_available_scheme_hint_bits(1, 0, 0);
#else
        scheme_hint_bits = coap_get_available_scheme_hint_bits(0, 0, 0);
#endif
        info_list = coap_resolve_address_info(coap_make_str_const("0.0.0.0"), COAP_DEFAULT_PORT, COAPS_DEFAULT_PORT, 0, 0, 0,
                                              scheme_hint_bits, COAP_RESOLVE_TYPE_LOCAL);
        if (info_list == NULL) {
            wm_log_error("coap_resolve_address_info() failed");
            goto clean_up;
        }

        for (info = info_list; info != NULL; info = info->next) {
            coap_endpoint_t *ep;

            coap_print_ip_addr(&info->addr, tmpbuf, sizeof(tmpbuf));
            ep = coap_new_endpoint(ctx, &info->addr, info->proto);
            if (!ep) {
                wm_log_error("cannot create endpoint for proto %u", info->proto);
            } else {
                have_ep = 1;
            }
        }
        coap_free_address_info(info_list);
        if (!have_ep) {
            wm_log_error("No endpoints available");
            goto clean_up;
        }

        resource = coap_resource_init(NULL, 0);
        if (!resource) {
            wm_log_error("coap_resource_init() failed");
            goto clean_up;
        }
        coap_register_handler(resource, COAP_REQUEST_GET, hnd_coap_get);
        coap_register_handler(resource, COAP_REQUEST_PUT, hnd_coap_put);
        coap_register_handler(resource, COAP_REQUEST_DELETE, hnd_coap_delete);
        /* We possibly want to Observe the GETs */
        coap_resource_set_get_observable(resource, 1);
        coap_add_resource(ctx, resource);

        wait_ms = COAP_DEFAULT_TIME_SEC * 1000;

        while (1) {
            int result = coap_io_process(ctx, wait_ms);
            if (result < 0) {
                break;
            } else if (result && (unsigned)result < wait_ms) {
                /* decrement if there is a result wait time returned */
                wait_ms -= result;
            }

            if (result) {
                /* result must have been >= wait_ms, so reset wait_ms */
                wait_ms = COAP_RESOURCE_CHECK_TIME * 1000;
            }
        }
    }
clean_up:
    coap_free_context(ctx);
    coap_cleanup();

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
    xTaskCreate(coap_example_server, "coap-server", 2048, NULL, 9, NULL);
    return 0;
}
