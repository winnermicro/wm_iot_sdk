/** @file
 * @brief Bluetooth shell module
 *
 * Provide some Bluetooth shell commands that can be useful to applications.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/byteorder.h>
#include <kernel.h>

#include <settings/settings.h>

#include <bluetooth/hci.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_cli.h"
#include "wm_component.h"
#include "wm_utils.h"

#include "bt.h"

#define CREDITS			10
#define DATA_MTU		(23 * CREDITS)

#define L2CAP_POLICY_NONE		0x00
#define L2CAP_POLICY_ALLOWLIST		0x01
#define L2CAP_POLICY_16BYTE_KEY		0x02

NET_BUF_POOL_FIXED_DEFINE(data_tx_pool, 1,
			  BT_L2CAP_SDU_BUF_SIZE(DATA_MTU), 8, NULL);
NET_BUF_POOL_FIXED_DEFINE(data_rx_pool, 1, DATA_MTU, 8, NULL);

static uint8_t l2cap_policy;
static struct bt_conn *l2cap_allowlist[CONFIG_BT_MAX_CONN];

static uint32_t l2cap_rate;
static uint32_t l2cap_recv_delay_ms;
static K_FIFO_DEFINE(l2cap_recv_fifo);
struct l2ch {
	struct k_work_delayable recv_work;
	struct bt_l2cap_le_chan ch;
};
#define L2CH_CHAN(_chan) CONTAINER_OF(_chan, struct l2ch, ch.chan)
#define L2CH_WORK(_work) CONTAINER_OF(k_work_delayable_from_work(_work), \
				      struct l2ch, recv_work)
#define L2CAP_CHAN(_chan) _chan->ch.chan

static bool metrics;

static void print_data_hex(const uint8_t *data, uint8_t len)
{
	if (len == 0)
		return;

	for (int16_t i = 0; i <= len; i++) {
		printf("%02x", data[i]);
	}
    printf("\n");
}

static int l2cap_recv_metrics(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
	static uint32_t len;
	static uint32_t ms_stamp;
	uint32_t delta;

	delta = wm_os_internal_get_time_ms() - ms_stamp;

	/* if last data rx-ed was greater than 1 second in the past,
	 * reset the metrics.
	 */
	if (delta > MSEC_PER_SEC) {
		len = 0U;
		l2cap_rate = 0U;
		ms_stamp = wm_os_internal_get_time_ms();
	} else {
		len += buf->len;
		l2cap_rate = ((uint64_t)len << 3) * MSEC_PER_SEC;
	}

	return 0;
}

static void l2cap_recv_cb(struct k_work *work)
{
	struct l2ch *c = L2CH_WORK(work);
	struct net_buf *buf;

	while ((buf = net_buf_get(&l2cap_recv_fifo, K_NO_WAIT))) {
		printf("Confirming reception\n");
		bt_l2cap_chan_recv_complete(&c->ch.chan, buf);
	}
}

static int l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
	struct l2ch *l2ch = L2CH_CHAN(chan);

	if (metrics) {
		return l2cap_recv_metrics(chan, buf);
	}

	printf("Incoming data channel %p len %u\n", chan,
		    buf->len);

	if (buf->len) {
		print_data_hex(buf->data, buf->len);
	}

	if (l2cap_recv_delay_ms > 0) {
		/* Submit work only if queue is empty */
		if (k_fifo_is_empty(&l2cap_recv_fifo)) {
			printf("Delaying response in %u ms...\n",
				    l2cap_recv_delay_ms);
		}

		net_buf_put(&l2cap_recv_fifo, buf);
		k_work_schedule(&l2ch->recv_work, K_MSEC(l2cap_recv_delay_ms));

		return -EINPROGRESS;
	}

	return 0;
}

static void l2cap_sent(struct bt_l2cap_chan *chan)
{
	printf("Outgoing data channel %p transmitted\n", chan);
}

static void l2cap_status(struct bt_l2cap_chan *chan, atomic_t *status)
{
	printf("Channel %p status %u\n", chan, (uint32_t)*status);
}

static void l2cap_connected(struct bt_l2cap_chan *chan)
{
	struct l2ch *c = L2CH_CHAN(chan);

	k_work_init_delayable(&c->recv_work, l2cap_recv_cb);

	printf("Channel %p connected\n", chan);
}

static void l2cap_disconnected(struct bt_l2cap_chan *chan)
{
	struct l2ch *c = L2CH_CHAN(chan);

	k_work_deinit_delayable(&c->recv_work);

	printf("Channel %p disconnected\n", chan);
}

static struct net_buf *l2cap_alloc_buf(struct bt_l2cap_chan *chan)
{
	/* print if metrics is disabled */
	if (!metrics) {
		printf("Channel %p requires buffer\n", chan);
	}

	return net_buf_alloc(&data_rx_pool, K_FOREVER);
}

static const struct bt_l2cap_chan_ops l2cap_ops = {
	.alloc_buf	= l2cap_alloc_buf,
	.recv		= l2cap_recv,
	.sent		= l2cap_sent,
	.status		= l2cap_status,
	.connected	= l2cap_connected,
	.disconnected	= l2cap_disconnected,
};

static struct l2ch l2ch_chan = {
	.ch.chan.ops	= &l2cap_ops,
	.ch.rx.mtu	= DATA_MTU,
};

static void l2cap_allowlist_remove(struct bt_conn *conn, uint8_t reason)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(l2cap_allowlist); i++) {
		if (l2cap_allowlist[i] == conn) {
			bt_conn_unref(l2cap_allowlist[i]);
			l2cap_allowlist[i] = NULL;
		}
	}
}

BT_CONN_CB_DEFINE(l2cap_conn_callbacks) = {
	.disconnected = l2cap_allowlist_remove,
};

static int l2cap_accept_policy(struct bt_conn *conn)
{
	int i;

	if (l2cap_policy == L2CAP_POLICY_16BYTE_KEY) {
		uint8_t enc_key_size = bt_conn_enc_key_size(conn);

		if (enc_key_size && enc_key_size < BT_ENC_KEY_SIZE_MAX) {
			return -EPERM;
		}
	} else if (l2cap_policy == L2CAP_POLICY_ALLOWLIST) {
		for (i = 0; i < ARRAY_SIZE(l2cap_allowlist); i++) {
			if (l2cap_allowlist[i] == conn) {
				return 0;
			}
		}

		return -EACCES;
	}

	return 0;
}

static int l2cap_accept(struct bt_conn *conn, struct bt_l2cap_server *server,
			struct bt_l2cap_chan **chan)
{
	int err;

	printf("Incoming conn %p\n", conn);

	err = l2cap_accept_policy(conn);
	if (err < 0) {
		return err;
	}

	if (l2ch_chan.ch.chan.conn) {
		printf("No channels available\n");
		return -ENOMEM;
	}

	*chan = &l2ch_chan.ch.chan;

	return 0;
}

static struct bt_l2cap_server server = {
	.accept		= l2cap_accept,
};

static int cmd_register(int argc, char *argv[])
{
	const char *policy;

	if (server.psm) {
		printf("Already registered\n");
		return -ENOEXEC;
	}

	server.psm = strtoul(argv[1], NULL, 16);

	if (argc > 2) {
		server.sec_level = strtoul(argv[2], NULL, 10);
	}

	if (argc > 3) {
		policy = argv[3];

		if (!strcmp(policy, "allowlist")) {
			l2cap_policy = L2CAP_POLICY_ALLOWLIST;
		} else if (!strcmp(policy, "16byte_key")) {
			l2cap_policy = L2CAP_POLICY_16BYTE_KEY;
		} else {
			return -EINVAL;
		}
	}

	if (bt_l2cap_server_register(&server) < 0) {
		printf("Unable to register psm\n");
		server.psm = 0U;
		return -ENOEXEC;
	} else {
		printf("L2CAP psm %u sec_level %u registered\n",
			    server.psm, server.sec_level);
	}

	return 0;
}

#if defined(CONFIG_BT_L2CAP_ECRED)
static int cmd_ecred_reconfigure(int argc, char *argv[])
{
	struct bt_l2cap_chan *l2cap_ecred_chans[] = { &l2ch_chan.ch.chan, NULL };
	uint16_t mtu;
	int err = 0;

	if (!default_conn) {
		printf("Not connected\n");
		return -ENOEXEC;
	}

	if (!l2ch_chan.ch.chan.conn) {
		printf("Channel not connected\n");
		return -ENOEXEC;
	}

    mtu = strtoul(argv[1], NULL, 10);
	if (mtu<=0) {
		printf("Unable to parse MTU (%s)\n", argv[1]);

		return -ENOEXEC;
	}

	err = bt_l2cap_ecred_chan_reconfigure(l2cap_ecred_chans, mtu);
	if (err < 0) {
		printf("Unable to reconfigure channel (err %d)\n", err);
	} else {
		printf("L2CAP reconfiguration pending\n");
	}

	return err;
}

static int cmd_ecred_connect(int argc, char *argv[])
{
	struct bt_l2cap_chan *l2cap_ecred_chans[] = { &l2ch_chan.ch.chan, NULL };
	uint16_t psm;
	int err = 0;

	if (!default_conn) {
		printf("Not connected\n");

		return -ENOEXEC;
	}

	if (l2ch_chan.ch.chan.conn) {
		printf("Channel already in use\n");

		return -ENOEXEC;
	}

	psm = strtoul(argv[1], NULL, 16);
	if (psm == 0) {
		printf("Unable to parse PSM (%s)\n", argv[1]);

		return err;
	}

	if (argc > 2) {
		int sec;

		sec = strtoul(argv[2], NULL, 10);
		if (err) {
			printf("Unable to parse security level (err %d)\n", err);

			return err;
		}


		l2ch_chan.ch.required_sec_level = sec;
	}

	err = bt_l2cap_ecred_chan_connect(default_conn, l2cap_ecred_chans, psm);
	if (err < 0) {
		printf("Unable to connect to psm %u (err %d)\n", psm,
			    err);
	} else {
		printf("L2CAP connection pending\n");
	}

	return err;
}
#endif /* CONFIG_BT_L2CAP_ECRED */

static int cmd_connect(size_t argc, char *argv[])
{
	uint16_t psm;
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return -ENOEXEC;
	}

	if (l2ch_chan.ch.chan.conn) {
		printf("Channel already in use\n");
		return -ENOEXEC;
	}

	psm = strtoul(argv[1], NULL, 16);

	if (argc > 2) {
		int sec;

		sec = *argv[2] - '0';

		l2ch_chan.ch.required_sec_level = sec;
	}

	err = bt_l2cap_chan_connect(default_conn, &l2ch_chan.ch.chan, psm);
	if (err < 0) {
		printf("Unable to connect to psm %u (err %d)\n", psm,
			    err);
	} else {
		printf("L2CAP connection pending\n");
	}

	return err;
}

static int cmd_disconnect(int argc, char *argv[])
{
	int err;

	err = bt_l2cap_chan_disconnect(&l2ch_chan.ch.chan);
	if (err) {
		printf("Unable to disconnect: %u\n", -err);
	}

	return err;
}

static int cmd_send(int argc, char *argv[])
{
	static uint8_t buf_data[DATA_MTU] = { [0 ... (DATA_MTU - 1)] = 0xff };
	int ret, len = DATA_MTU, count = 1;
	struct net_buf *buf;

	if (argc > 1) {
		count = strtoul(argv[1], NULL, 10);
	}

	if (argc > 2) {
		len = strtoul(argv[2], NULL, 10);
		if (len > DATA_MTU) {
			shell_print(sh,
				    "Length exceeds TX MTU for the channel\n");
			return -ENOEXEC;
		}
	}

	len = MIN(l2ch_chan.ch.tx.mtu, len);

	while (count--) {
		printf("Rem %d\n", count);
		buf = net_buf_alloc(&data_tx_pool, K_SECONDS(2));
		if (!buf) {
			if (l2ch_chan.ch.state != BT_L2CAP_CONNECTED) {
				printf("Channel disconnected, stopping TX\n");

				return -EAGAIN;
			}
			printf("Allocation timeout, stopping TX\n");

			return -EAGAIN;
		}
		net_buf_reserve(buf, BT_L2CAP_SDU_CHAN_SEND_RESERVE);

		net_buf_add_mem(buf, buf_data, len);
		ret = bt_l2cap_chan_send(&l2ch_chan.ch.chan, buf);
		if (ret < 0) {
			printf("Unable to send: %d\n", -ret);
			net_buf_unref(buf);
			return -ENOEXEC;
		}
	}

	return 0;
}

static int cmd_recv(int argc, char *argv[])
{
	if (argc > 1) {
		l2cap_recv_delay_ms = strtoul(argv[1], NULL, 10);
	} else {
		printf("l2cap receive delay: %u ms\n",
			    l2cap_recv_delay_ms);
	}

	return 0;
}

static int cmd_metrics(int argc, char *argv[])
{
	const char *action;

	if (argc < 2) {
		printf("l2cap rate: %u bps.\n", l2cap_rate);

		return 0;
	}

	action = argv[1];

	if (!strcmp(action, "on")) {
		metrics = true;
	} else if (!strcmp(action, "off")) {
		metrics = false;
	} else {
		printf("btl2capmetrics <value on, off>\n");
		return 0;
	}

	printf("l2cap metrics %s.\n", action);
	return 0;
}

static int cmd_allowlist_add(int argc, char *argv[])
{
	int i;

	if (!default_conn) {
		printf("Not connected\n");
		return 0;
	}

	for (i = 0; i < ARRAY_SIZE(l2cap_allowlist); i++) {
		if (l2cap_allowlist[i] == NULL) {
			l2cap_allowlist[i] = bt_conn_ref(default_conn);
			return 0;
		}
	}

	return -ENOMEM;
}

static int cmd_allowlist_remove(int argc, char *argv[])
{
	if (!default_conn) {
		printf("Not connected\n");
		return 0;
	}

	l2cap_allowlist_remove(default_conn, 0);

	return 0;
}


WM_CLI_CMD_DEFINE(btl2capconnect, cmd_connect, l2cap connect cmd, btl2capconnect <psm> [sec_level] -- btl2capconnect cmd);
WM_CLI_CMD_DEFINE(btl2capdisconnect, cmd_disconnect, l2cap disconnect cmd, btl2capdisconnect -- btl2capconnect cmd);
WM_CLI_CMD_DEFINE(btl2capmetrics, cmd_metrics, l2cap metrics cmd, btl2capmetrics <value on, off> -- btl2capmetrics cmd);
WM_CLI_CMD_DEFINE(btl2caprecv, cmd_recv, l2cap recv cmd, btl2caprecv [delay (in milliseconds)] -- btl2caprecv cmd);
WM_CLI_CMD_DEFINE(btl2capregister, cmd_register, l2cap register psm cmd, btl2capregister <psm> [sec_level] [policy: allowlist, 16byte_key]-- btl2capregister cmd);
WM_CLI_CMD_DEFINE(btl2capsend, cmd_send, l2cap send cmd, btl2capsend [number of packets] [length of packet(s)]-- btl2capsend cmd);
WM_CLI_CMD_DEFINE(btl2capallowlistadd, cmd_allowlist_add, l2cap add allowlist cmd, btl2capallowlistadd -- btl2capallowlistadd cmd);
WM_CLI_CMD_DEFINE(btl2capallowlistremove, cmd_allowlist_remove, l2cap remove allowlist cmd, btl2capallowlistremove -- btl2capallowlistremove cmd);
#if defined(CONFIG_BT_L2CAP_ECRED)
WM_CLI_CMD_DEFINE(btl2capecredconnect, cmd_ecred_connect, l2cap ecred connect cmd, btl2capecredconnect <psm (hex)> [sec_level (dec)]-- btl2capecredconnect cmd);
WM_CLI_CMD_DEFINE(btl2capecredreconfigure, cmd_ecred_reconfigure, l2cap ecred reconfigure cmd, btl2capecredreconfigure <mtu (dec)>-- btl2capecredreconfigure cmd);
#endif

static int bt_l2cap_shell_buffer_init(void)
{
    net_buf_pool_init(&data_tx_pool, "shell_data_tx_pool" 1,
                  BT_L2CAP_SDU_BUF_SIZE(DATA_MTU), 8, NULL);
    net_buf_pool_init(&data_rx_pool, "shell_data_rx_pool" 1, DATA_MTU, 8, NULL);

    k_fifo_init(&l2cap_recv_fifo);

}

WM_COMPONEN_INIT_2(bt_l2cap_shell_buffer_init);

