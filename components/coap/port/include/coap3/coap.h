/*
 * coap.h -- main header file for CoAP stack of libcoap
 *
 * Copyright (C) 2010-2012,2015-2023 Olaf Bergmann <bergmann@tzi.org>
 *               2015 Carsten Schoenert <c.schoenert@t-online.de>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

/**
 * @file coap.h
 * @brief Primary include file
 */

#ifndef COAP_H_
#define COAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_IPV4
#define COAP_IPV4_SUPPORT 1
#else /* ! _LWIP_IPV4 */
struct sockaddr_in {
    u8_t            sin_len;
    sa_family_t     sin_family;
    in_port_t       sin_port;
    struct in_addr  sin_addr;
};
#endif /* ! LWIP_IPV4 */

#if LWIP_IPV6
#define COAP_IPV6_SUPPORT 1
#else /* ! LWIP_IPV6 */
struct sockaddr_in6 {
    u8_t            sin6_len;
    sa_family_t     sin6_family;
    in_port_t       sin6_port;
    u32_t           sin6_flowinfo;
    struct in_addr  sin6_addr;
    u32_t           sin6_scope_id;
};
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 40
#endif /* INET6_ADDRSTRLEN */
#endif /* ! LWIP_IPV6 */

#include "coap3/libcoap.h"

#include "coap3/coap_forward_decls.h"
#include "coap3/coap_address.h"
#include "coap3/coap_async.h"
#include "coap3/coap_block.h"
#include "coap3/coap_cache.h"
#include "coap3/coap_debug.h"
#include "coap3/coap_dtls.h"
#include "coap3/coap_encode.h"
#include "coap3/coap_event.h"
#include "coap3/coap_io.h"
#include "coap3/coap_mem.h"
#include "coap3/coap_net.h"
#include "coap3/coap_option.h"
#include "coap3/coap_oscore.h"
#include "coap3/coap_pdu.h"
#include "coap3/coap_prng.h"
#include "coap3/coap_resource.h"
#include "coap3/coap_str.h"
#include "coap3/coap_subscribe.h"
#include "coap3/coap_time.h"
#include "coap3/coap_uri.h"
#include "coap3/coap_ws.h"

#ifdef __cplusplus
}
#endif

#endif /* COAP_H_ */
