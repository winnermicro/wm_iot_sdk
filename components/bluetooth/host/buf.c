/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/buf.h>
#include <bluetooth/l2cap.h>

#include "hci_core.h"
#include "conn_internal.h"
#include "iso_internal.h"

#include <bluetooth/hci.h>

#define LOG_TAG "bt_buf"
#include <logging/log.h>

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_ISO)
#define MAX_EVENT_COUNT CONFIG_BT_MAX_CONN + CONFIG_BT_ISO_MAX_CHAN
#else
#define MAX_EVENT_COUNT CONFIG_BT_MAX_CONN
#endif /* CONFIG_BT_ISO */
#elif defined(CONFIG_BT_ISO)
#define MAX_EVENT_COUNT CONFIG_BT_ISO_MAX_CHAN
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_CONN) || defined(CONFIG_BT_ISO)
#define NUM_COMLETE_EVENT_SIZE BT_BUF_EVT_SIZE(                        \
	sizeof(struct bt_hci_cp_host_num_completed_packets) +          \
	MAX_EVENT_COUNT * sizeof(struct bt_hci_handle_count))
/* Dedicated pool for HCI_Number_of_Completed_Packets. This event is always
 * consumed synchronously by bt_recv_prio() so a single buffer is enough.
 * Having a dedicated pool for it ensures that exhaustion of the RX pool
 * cannot block the delivery of this priority event.
 */
NET_BUF_POOL_FIXED_DEFINE(num_complete_pool, 1, NUM_COMLETE_EVENT_SIZE, 8, NULL);
#endif /* CONFIG_BT_CONN || CONFIG_BT_ISO */

NET_BUF_POOL_FIXED_DEFINE(discardable_pool, CONFIG_BT_BUF_EVT_DISCARDABLE_COUNT,
			  BT_BUF_EVT_SIZE(CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE), 8,
			  NULL);

#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
NET_BUF_POOL_DEFINE(acl_in_pool, CONFIG_BT_BUF_ACL_RX_COUNT,
		    BT_BUF_ACL_SIZE(CONFIG_BT_BUF_ACL_RX_SIZE),
		    MAX(sizeof(struct bt_buf_data), CONFIG_BT_CONN_TX_USER_DATA_SIZE),
		    bt_hci_host_num_completed_packets);

NET_BUF_POOL_FIXED_DEFINE(evt_pool, CONFIG_BT_BUF_EVT_RX_COUNT,
			  BT_BUF_EVT_RX_SIZE, 8,
			  NULL);
#else
NET_BUF_POOL_FIXED_DEFINE(hci_rx_pool, BT_BUF_RX_COUNT,
			  BT_BUF_RX_SIZE, CONFIG_BT_CONN_TX_USER_DATA_SIZE,
			  NULL);
#endif /* CONFIG_BT_HCI_ACL_FLOW_CONTROL */

struct net_buf *bt_buf_get_rx(enum bt_buf_type type, k_timeout_t timeout)
{
	struct net_buf *buf;

	__ASSERT(type == BT_BUF_EVT || type == BT_BUF_ACL_IN ||
		 type == BT_BUF_ISO_IN, "Invalid buffer type requested");

	if ((IS_ENABLED(CONFIG_BT_ISO_UNICAST) ||
	     IS_ENABLED(CONFIG_BT_ISO_SYNC_RECEIVER)) &&
	     type == BT_BUF_ISO_IN) {
		return bt_iso_get_rx(timeout);
	}

#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
	if (type == BT_BUF_EVT) {
		buf = net_buf_alloc(&evt_pool, timeout);
	} else {
		buf = net_buf_alloc(&acl_in_pool, timeout);
	}
#else
	buf = net_buf_alloc(&hci_rx_pool, timeout);
#endif

	if (buf) {
		net_buf_reserve(buf, BT_BUF_RESERVE);
		bt_buf_set_type(buf, type);
	}

	return buf;
}

struct net_buf *bt_buf_get_cmd_complete(k_timeout_t timeout)
{
	struct net_buf *buf;

	buf = (struct net_buf *)atomic_ptr_clear((atomic_ptr_t *)&bt_dev.sent_cmd);
	if (buf) {
		bt_buf_set_type(buf, BT_BUF_EVT);
		buf->len = 0U;
		net_buf_reserve(buf, BT_BUF_RESERVE);

		return buf;
	}

	return bt_buf_get_rx(BT_BUF_EVT, timeout);
}

struct net_buf *bt_buf_get_evt(uint8_t evt, bool discardable,
			       k_timeout_t timeout)
{
	switch (evt) {
#if defined(CONFIG_BT_CONN) || defined(CONFIG_BT_ISO)
	case BT_HCI_EVT_NUM_COMPLETED_PACKETS:
		{
			struct net_buf *buf;

			buf = net_buf_alloc(&num_complete_pool, timeout);
			if (buf) {
				net_buf_reserve(buf, BT_BUF_RESERVE);
				bt_buf_set_type(buf, BT_BUF_EVT);
			}

			return buf;
		}
#endif /* CONFIG_BT_CONN || CONFIG_BT_ISO */
	case BT_HCI_EVT_CMD_COMPLETE:
	case BT_HCI_EVT_CMD_STATUS:
		return bt_buf_get_cmd_complete(timeout);
	default:
		if (discardable) {
			struct net_buf *buf;

			buf = net_buf_alloc(&discardable_pool, timeout);
			if (buf) {
				net_buf_reserve(buf, BT_BUF_RESERVE);
				bt_buf_set_type(buf, BT_BUF_EVT);
			}

			return buf;
		}

		return bt_buf_get_rx(BT_BUF_EVT, timeout);
	}
}


void bt_buf_init(void)
{
#if defined(CONFIG_BT_CONN) || defined(CONFIG_BT_ISO)
    net_buf_pool_init(&num_complete_pool, "num_complete_pool", 1, NUM_COMLETE_EVENT_SIZE, 8, NULL);
#endif /* CONFIG_BT_CONN || CONFIG_BT_ISO */

    net_buf_pool_init(&discardable_pool, "discardable_pool", CONFIG_BT_BUF_EVT_DISCARDABLE_COUNT,
			  BT_BUF_EVT_SIZE(CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE), 8,
			  NULL);

#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
    net_buf_pool_init(&acl_in_pool,"acl_in_pool", CONFIG_BT_BUF_ACL_RX_COUNT,
                BT_BUF_ACL_SIZE(CONFIG_BT_BUF_ACL_RX_SIZE),
                MAX(sizeof(struct bt_buf_data), CONFIG_BT_CONN_TX_USER_DATA_SIZE),
                bt_hci_host_num_completed_packets);

    net_buf_pool_init(&evt_pool,"evt_pool", CONFIG_BT_BUF_EVT_RX_COUNT,
			  BT_BUF_EVT_RX_SIZE, 8,
			  NULL);
#else
    net_buf_pool_init(&hci_rx_pool, "hci_rx_pool", BT_BUF_RX_COUNT,
			  BT_BUF_RX_SIZE, CONFIG_BT_CONN_TX_USER_DATA_SIZE,
			  NULL);

#endif /* CONFIG_BT_HCI_ACL_FLOW_CONTROL */
}

void bt_buf_check_usage()
{
#if defined(CONFIG_NET_BUF_POOL_USAGE)

    struct net_buf_pool *pool;

#if defined(CONFIG_BT_CONN) || defined(CONFIG_BT_ISO)
        pool = &num_complete_pool;
        LOG_ERR("%s id is %d count %d avail %d", pool->name, pool_id(pool), pool->buf_count, pool->avail_count);
#endif /* CONFIG_BT_CONN || CONFIG_BT_ISO */
        pool = &discardable_pool;
        LOG_ERR("%s id is %d count %d avail %d", pool->name, pool_id(pool), pool->buf_count, pool->avail_count);

#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
        pool = &acl_in_pool;
        LOG_ERR("%s id is %d count %d avail %d", pool->name, pool_id(pool), pool->buf_count, pool->avail_count);

        pool = &evt_pool;
        LOG_ERR("%s id is %d count %d avail %d", pool->name, pool_id(pool), pool->buf_count, pool->avail_count);

#else
        pool = &hci_rx_pool;
        LOG_ERR("%s id is %d count %d avail %d", pool->name, pool_id(pool), pool->buf_count, pool->avail_count);

#endif /* CONFIG_BT_HCI_ACL_FLOW_CONTROL */
#endif

}
void bt_buf_deinit(void)
{
#if defined(CONFIG_BT_CONN) || defined(CONFIG_BT_ISO)
        net_buf_pool_deinit(&num_complete_pool);
#endif /* CONFIG_BT_CONN || CONFIG_BT_ISO */
    
        net_buf_pool_deinit(&discardable_pool);
    
#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
        net_buf_pool_deinit(&acl_in_pool);
        net_buf_pool_deinit(&evt_pool);
#else
        net_buf_pool_deinit(&hci_rx_pool);
    
#endif /* CONFIG_BT_HCI_ACL_FLOW_CONTROL */

}


#ifdef ZTEST_UNITTEST
#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
struct net_buf_pool *bt_buf_get_evt_pool(void)
{
	return &evt_pool;
}

struct net_buf_pool *bt_buf_get_acl_in_pool(void)
{
	return &acl_in_pool;
}
#else
struct net_buf_pool *bt_buf_get_hci_rx_pool(void)
{
	return &hci_rx_pool;
}
#endif /* CONFIG_BT_HCI_ACL_FLOW_CONTROL */

#if defined(CONFIG_BT_BUF_EVT_DISCARDABLE_COUNT)
struct net_buf_pool *bt_buf_get_discardable_pool(void)
{
	return &discardable_pool;
}
#endif /* CONFIG_BT_BUF_EVT_DISCARDABLE_COUNT */

#if defined(CONFIG_BT_CONN) || defined(CONFIG_BT_ISO)
struct net_buf_pool *bt_buf_get_num_complete_pool(void)
{
	return &num_complete_pool;
}
#endif /* CONFIG_BT_CONN || CONFIG_BT_ISO */
#endif /* ZTEST_UNITTEST */
