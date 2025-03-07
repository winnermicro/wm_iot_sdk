/**
 * @file aws_mbedtls_port.c
 *
 * @brief AWS mbedTLS port layer
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <string.h>
#include "aws_mbedtls_port.h"

/* mbedTLS transport header. */
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_ciphersuites.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/hmac_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/timing.h"
#include "mbedtls/base64.h"

#define LOG_TAG "aws_mbedtls"
#include "wm_log.h"

/**
 * @brief Structure to using mbedTLS
 */
typedef struct {
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
} aws_mbedtls_t;

void mbedtls_disconnect_from_server(struct NetworkContext *pNetworkContext)
{
    aws_mbedtls_t *aws_mbedtls = (aws_mbedtls_t *)pNetworkContext->handle;

    if (aws_mbedtls == NULL) {
        return;
    }

    pNetworkContext->handle = NULL;

    mbedtls_net_free(&(aws_mbedtls->server_fd));
    mbedtls_entropy_free(&(aws_mbedtls->entropy));
    mbedtls_ctr_drbg_free(&(aws_mbedtls->ctr_drbg));

    mbedtls_ssl_free(&(aws_mbedtls->ssl));
    mbedtls_ssl_config_free(&(aws_mbedtls->conf));

    mbedtls_x509_crt_free(&(aws_mbedtls->cacert));
    mbedtls_x509_crt_free(&(aws_mbedtls->clicert));
    mbedtls_pk_free(&(aws_mbedtls->pkey));

    free(aws_mbedtls);
}

int mbedtls_connect_to_server(struct NetworkContext *pNetworkContext)
{
    int ret          = EXIT_FAILURE;
    const char *pers = "mqtt_demo_mutual_auth";
    char port[8];
    aws_mbedtls_t *aws_mbedtls = NULL;

    mbedtls_disconnect_from_server(pNetworkContext); /** clean mbedtls connect */

    aws_mbedtls = calloc(1, sizeof(aws_mbedtls_t));
    if (aws_mbedtls == NULL) {
        wm_log_error(" failed\n  ! out of memory \n");
        return EXIT_FAILURE;
    }

    pNetworkContext->handle = aws_mbedtls;

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init(&(aws_mbedtls->server_fd));
    mbedtls_ssl_init(&(aws_mbedtls->ssl));
    mbedtls_ssl_config_init(&(aws_mbedtls->conf));
    mbedtls_x509_crt_init(&(aws_mbedtls->cacert));
    mbedtls_x509_crt_init(&(aws_mbedtls->clicert));
    mbedtls_pk_init(&(aws_mbedtls->pkey));
    mbedtls_ctr_drbg_init(&(aws_mbedtls->ctr_drbg));
    mbedtls_entropy_init(&(aws_mbedtls->entropy));

    wm_log_debug("\n  . Seeding the random number generator...");

    if ((ret = mbedtls_ctr_drbg_seed(&(aws_mbedtls->ctr_drbg), mbedtls_entropy_func, &(aws_mbedtls->entropy),
                                     (const unsigned char *)pers, strlen(pers))) != 0) {
        wm_log_error(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto exit;
    }

    wm_log_debug(" ok\n");

    /* ALPN is required when communicating to AWS IoT Core over port 443 through HTTP. */
    if (pNetworkContext->port == 443 && pNetworkContext->alpn_protocols) {
        const char *alpn_protocols[2] = { pNetworkContext->alpn_protocols, NULL };

        ret = mbedtls_ssl_conf_alpn_protocols(&(aws_mbedtls->conf), alpn_protocols);

        if (ret != 0) {
            wm_log_error(" failed\n  ! mbedtls_ssl_conf_alpn_protocols returned %d", ret);
            goto exit;
        }
    }

    /*
     * 1. Initialize certificates
     */

    if (pNetworkContext->root_CA) {
        wm_log_debug("  . Loading the CA root certificate ...");

        ret = mbedtls_x509_crt_parse(&(aws_mbedtls->cacert), (const unsigned char *)pNetworkContext->root_CA,
                                     strlen(pNetworkContext->root_CA) + 1);
        if (ret < 0) {
            wm_log_error(" root CA failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int)-ret);
            goto exit;
        }
        wm_log_debug(" ok (%d skipped)\n", ret);
    }

    if (pNetworkContext->client_cert && pNetworkContext->client_key) {
        ret = mbedtls_x509_crt_parse(&(aws_mbedtls->clicert), (const unsigned char *)pNetworkContext->client_cert,
                                     strlen(pNetworkContext->client_cert) + 1);
        if (ret < 0) {
            wm_log_error(" client cert failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int)-ret);
            goto exit;
        }
        wm_log_debug(" ok (%d skipped)\n", ret);

        ret = mbedtls_pk_parse_key(&(aws_mbedtls->pkey), (const unsigned char *)pNetworkContext->client_key,
                                   strlen(pNetworkContext->client_key) + 1, NULL, 0, // NULL, NULL);
                                   mbedtls_ctr_drbg_random, &(aws_mbedtls->ctr_drbg));
        if (ret < 0) {
            wm_log_error(" client key failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int)-ret);
            goto exit;
        }
        wm_log_debug(" ok (%d skipped)\n", ret);
    }

    /*
     * 2. Start the connection
     */
    wm_log_debug("  . Connecting to tcp/%s/%d...", pNetworkContext->host_name, pNetworkContext->port);

    snprintf(port, sizeof(port), "%d", pNetworkContext->port);
    if ((ret = mbedtls_net_connect(&(aws_mbedtls->server_fd), pNetworkContext->host_name, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        wm_log_error(" failed\n  ! mbedtls_net_connect returned -0x%x\n\n", -ret);
        goto exit;
    }

    wm_log_debug(" ok\n");

    /*
     * 3. Setup stuff
     */
    wm_log_debug("  . Setting up the SSL/TLS structure...");

    if ((ret = mbedtls_ssl_config_defaults(&(aws_mbedtls->conf), MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        wm_log_error(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    wm_log_debug(" ok\n");

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode(&(aws_mbedtls->conf), MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&(aws_mbedtls->conf), &(aws_mbedtls->cacert), NULL);

    if ((ret = mbedtls_ssl_conf_own_cert(&(aws_mbedtls->conf), &(aws_mbedtls->clicert), &(aws_mbedtls->pkey))) != 0) {
        wm_log_error(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_conf_rng(&(aws_mbedtls->conf), mbedtls_ctr_drbg_random, &(aws_mbedtls->ctr_drbg));

    if ((ret = mbedtls_ssl_setup(&(aws_mbedtls->ssl), &(aws_mbedtls->conf))) != 0) {
        wm_log_error(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = mbedtls_ssl_set_hostname(&(aws_mbedtls->ssl), pNetworkContext->host_name)) != 0) {
        wm_log_error(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto exit;
    }

    if (pNetworkContext->recv_timeout_ms) {
        mbedtls_ssl_conf_read_timeout(&(aws_mbedtls->conf), pNetworkContext->recv_timeout_ms);
    }
    mbedtls_ssl_set_bio(&(aws_mbedtls->ssl), &(aws_mbedtls->server_fd), mbedtls_net_send, NULL, mbedtls_net_recv_timeout);

    /*
     * 4. Handshake
     */
    wm_log_debug("  . Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(&(aws_mbedtls->ssl))) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            wm_log_error(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", (unsigned int)-ret);
            goto exit;
        }
    }

    wm_log_debug(" ok\n");

    return EXIT_SUCCESS;

exit:
    mbedtls_disconnect_from_server(pNetworkContext);
    return EXIT_FAILURE;
}

int32_t mbedtls_send(struct NetworkContext *pNetworkContext, const void *pBuffer, size_t bytesToSend)
{
    int ret                    = 0;
    aws_mbedtls_t *aws_mbedtls = (aws_mbedtls_t *)pNetworkContext->handle;

    if (aws_mbedtls == NULL) {
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = mbedtls_ssl_write(&(aws_mbedtls->ssl), pBuffer, bytesToSend);

    if ((ret == MBEDTLS_ERR_SSL_TIMEOUT) || (ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
        /* Mark these set of errors as a timeout. The libraries may retry send on these errors. */
        ret = 0;
    } else if (ret < 0) {
        wm_log_error("mbedtls_ssl_write failed: -0x%x(errno:%d).", ret, errno);
    } else {
        /* Empty else marker. */
    }

    return ret;
}

int32_t mbedtls_recv(struct NetworkContext *pNetworkContext, void *pBuffer, size_t bytesToRecv)
{
    int ret;
    aws_mbedtls_t *aws_mbedtls = (aws_mbedtls_t *)pNetworkContext->handle;

    if (aws_mbedtls == NULL) {
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    memset(pBuffer, 0, bytesToRecv);

    ret = mbedtls_ssl_read(&(aws_mbedtls->ssl), pBuffer, bytesToRecv);

    if (ret < 0) {
        if (ret == MBEDTLS_ERR_SSL_TIMEOUT || ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            return 0; /** No available data */
        }

        wm_log_error("mbedtls_ssl_read failed: -0x%x(errno:%d).", -ret, errno);
        return -1;
    }

    wm_log_debug("mbedtls_ssl_read recv %d bytes", ret);

    return ret;
}
