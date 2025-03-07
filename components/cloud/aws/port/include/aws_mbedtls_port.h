/**
 * @file aws_mbedtls_port.h
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

#ifndef __AWS_MBED_TLS_PORT_H__
#define __AWS_MBED_TLS_PORT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ALPN_NUM (5)

/**
 * @brief Structure to using network context
 */
struct NetworkContext {
    const char *host_name;      /**< @brief Server host name. */
    uint16_t port;              /**< @brief Server port in host-order. */
    const char *root_CA;        /**< @brief Trusted server root certificate bytes. */
    const char *client_cert;    /**< @brief Client certificate bytes. */
    const char *client_key;     /**< @brief Client certificate's private key bytes. */
    const char *alpn_protocols; /**< @brief Client alpn protocols. */
    uint32_t recv_timeout_ms;   /**< @brief Recv timeout by milliseconds.*/
    void *handle;               /**< @brief Network Handle.*/
};

void mbedtls_disconnect_from_server(struct NetworkContext *pNetworkContext);

int mbedtls_connect_to_server(struct NetworkContext *pNetworkContext);

int32_t mbedtls_send(struct NetworkContext *pNetworkContext, const void *pBuffer, size_t bytesToSend);

int32_t mbedtls_recv(struct NetworkContext *pNetworkContext, void *pBuffer, size_t bytesToRecv);

#ifdef __cplusplus
}
#endif

#endif /* __AWS_MBED_TLS_PORT_H__ */
