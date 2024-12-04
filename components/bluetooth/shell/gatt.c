/** @file
 * @brief Bluetooth GATT shell functions
 *
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/byteorder.h>
#include <sys/util.h>
#include <kernel.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_cli.h"
#include "bt.h"

static void print_data_hex(const uint8_t *data, uint8_t len)
{
	if (len == 0)
		return;

	for (int16_t i = 0; i <= len; i++) {
		printf("%02x", data[i]);
	}
    printf("\n");
}

#if defined(CONFIG_BT_GATT_CLIENT) || defined(CONFIG_BT_GATT_DYNAMIC_DB)
extern uint8_t selected_id;

static struct write_stats {
	uint32_t count;
	uint32_t len;
	uint32_t total;
	uint32_t rate;
} write_stats;

static void update_write_stats(uint16_t len)
{
	static uint32_t ms_stamp = 0;
	uint32_t delta;

	delta = k_uptime_get_32() - ms_stamp;

	if (!delta) {
		delta = 1;
	}

	write_stats.count++;
	write_stats.total += len;

	/* if last data rx-ed was greater than 1 second in the past,
	 * reset the metrics.
	 */
	if (delta > 1000) {
		write_stats.len = 0U;
		write_stats.rate = 0U;
		ms_stamp = k_uptime_get_32();
	} else {
		write_stats.len += len;
		write_stats.rate = ((uint64_t)write_stats.len << 3) *
			1000U / delta;
	}
}

#if defined(CONFIG_BT_EATT)
#define SET_CHAN_OPT_ANY(params) (params).chan_opt = BT_ATT_CHAN_OPT_NONE
#else
#define SET_CHAN_OPT_ANY(params)
#endif /* CONFIG_BT_EATT */

static void print_write_stats(void)
{
	printf("Write #%u: %u bytes (%u bps)\n",
		    write_stats.count, write_stats.total, write_stats.rate);
}
#endif /* CONFIG_BT_GATT_CLIENT || CONFIG_BT_GATT_DYNAMIC_DB */

#if defined(CONFIG_BT_GATT_CLIENT)
static void reset_write_stats(void)
{
	memset(&write_stats, 0, sizeof(write_stats));
}

/* This variable is write-locked when `(exchange_params.func != NULL)`.
 * Must be zero-initialized when unlocked.
 */
static struct bt_gatt_exchange_params exchange_params;

static void exchange_func(struct bt_conn *conn, uint8_t err,
			  struct bt_gatt_exchange_params *params)
{
	printf("Exchange %s\n", err == 0U ? "successful" :
		    "failed");

	/* Release global `exchange_params`. */
	__ASSERT_NO_MSG(params == &exchange_params);
	(void)memset(params, 0, sizeof(*params));
}

static void cmd_exchange_mtu(int argc, char *argv[])
{
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	if (exchange_params.func) {
		printf("Shell command busy. A previous invocation is in progress.\n");
		return ;
	}

	exchange_params.func = exchange_func;

	err = bt_gatt_exchange_mtu(default_conn, &exchange_params);
	if (err) {
		/* Release global `exchange_params`. */
		exchange_params.func = NULL;
	}

	if (err == -EALREADY) {
		printf("Already exchanged\n");
	} else if (err) {
		printf("Exchange failed (err %d)\n", err);
	} else {
		printf("Exchange pending\n");
	}

	return ;
}

static struct bt_gatt_discover_params discover_params;
static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);

static void print_chrc_props(uint8_t properties)
{
	printf("Properties: ");

	if (properties & BT_GATT_CHRC_BROADCAST) {
		printf("[bcast]\n");
	}

	if (properties & BT_GATT_CHRC_READ) {
		printf("[read]\n");
	}

	if (properties & BT_GATT_CHRC_WRITE) {
		printf("[write]\n");
	}

	if (properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
		printf("[write w/w rsp]\n");
	}

	if (properties & BT_GATT_CHRC_NOTIFY) {
		printf("[notify]\n");
	}

	if (properties & BT_GATT_CHRC_INDICATE) {
		printf("[indicate]\n");
	}

	if (properties & BT_GATT_CHRC_AUTH) {
		printf("[auth]\n");
	}

	if (properties & BT_GATT_CHRC_EXT_PROP) {
		printf("[ext prop]\n");
	}

	printf("\n");
}

static uint8_t discover_func(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params)
{
	struct bt_gatt_service_val *gatt_service;
	struct bt_gatt_chrc *gatt_chrc;
	struct bt_gatt_include *gatt_include;
	char str[BT_UUID_STR_LEN];

	if (!attr) {
		printf("Discover complete\n");
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	switch (params->type) {
	case BT_GATT_DISCOVER_SECONDARY:
	case BT_GATT_DISCOVER_PRIMARY:
		gatt_service = attr->user_data;
		bt_uuid_to_str(gatt_service->uuid, str, sizeof(str));
		printf("Service %s found: start handle %x, "
			    "end_handle %x\n", str, attr->handle,
			    gatt_service->end_handle);
		break;
	case BT_GATT_DISCOVER_CHARACTERISTIC:
		gatt_chrc = attr->user_data;
		bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
		printf("Characteristic %s found: handle %x\n",
			    str, attr->handle);
		print_chrc_props(gatt_chrc->properties);
		break;
	case BT_GATT_DISCOVER_INCLUDE:
		gatt_include = attr->user_data;
		bt_uuid_to_str(gatt_include->uuid, str, sizeof(str));
		printf("Include %s found: handle %x, start %x, "
			    "end %x\n", str, attr->handle,
			    gatt_include->start_handle,
			    gatt_include->end_handle);
		break;
	default:
		bt_uuid_to_str(attr->uuid, str, sizeof(str));
		printf("Descriptor %s found: handle %x\n", str,
			    attr->handle);
		break;
	}

	return BT_GATT_ITER_CONTINUE;
}

static void cmd_discover(int argc, char *argv[])
{
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	if (discover_params.func) {
		printf("Discover ongoing\n");
		return ;
	}

	discover_params.func = discover_func;
	discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	SET_CHAN_OPT_ANY(discover_params);

	if (argc > 1) {
		/* Only set the UUID if the value is valid (non zero) */
		uuid.val = strtoul(argv[1], NULL, 16);
		if (uuid.val) {
			discover_params.uuid = &uuid.uuid;
		}
	}

	if (argc > 2) {
		discover_params.start_handle = strtoul(argv[2], NULL, 16);
		if (argc > 3) {
			discover_params.end_handle = strtoul(argv[3], NULL, 16);
		}
	}

	if (!strcmp(argv[0], "discover")) {
		discover_params.type = BT_GATT_DISCOVER_ATTRIBUTE;
	} else if (!strcmp(argv[0], "discover-secondary")) {
		discover_params.type = BT_GATT_DISCOVER_SECONDARY;
	} else if (!strcmp(argv[0], "discover-include")) {
		discover_params.type = BT_GATT_DISCOVER_INCLUDE;
	} else if (!strcmp(argv[0], "discover-characteristic")) {
		discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
	} else if (!strcmp(argv[0], "discover-descriptor")) {
		discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
	} else {
		discover_params.type = BT_GATT_DISCOVER_PRIMARY;
	}

	err = bt_gatt_discover(default_conn, &discover_params);
	if (err) {
		printf("Discover failed (err %d)\n", err);
	} else {
		printf("Discover pending\n");
	}

	return ;
}

static struct bt_gatt_read_params read_params;

static uint8_t read_func(struct bt_conn *conn, uint8_t err,
			 struct bt_gatt_read_params *params,
			 const void *data, uint16_t length)
{
	printf("Read complete: err 0x%02x length %u\n", err, length);

	if (!data) {
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	} else {
		print_data_hex( data, length);
	}

	return BT_GATT_ITER_CONTINUE;
}

static void cmd_read(int argc, char *argv[])
{
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	if (read_params.func) {
		printf("Read ongoing\n");
		return ;
	}

	read_params.func = read_func;
	read_params.handle_count = 1;
	read_params.single.handle = strtoul(argv[1], NULL, 16);
	read_params.single.offset = 0U;
	SET_CHAN_OPT_ANY(read_params);

	if (argc > 2) {
		read_params.single.offset = strtoul(argv[2], NULL, 16);
	}

	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		printf("Read failed (err %d)\n", err);
	} else {
		printf("Read pending\n");
	}

	return ;
}

static void cmd_mread(int argc, char *argv[])
{
	uint16_t h[8];
	size_t i;
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	if (read_params.func) {
		printf("Read ongoing\n");
		return ;
	}

	if ((argc - 1) >  ARRAY_SIZE(h)) {
		printf("Enter max %zu handle items to read\n",
			    ARRAY_SIZE(h));
		return ;
	}

	for (i = 0; i < argc - 1; i++) {
		h[i] = strtoul(argv[i + 1], NULL, 16);
	}

	read_params.func = read_func;
	read_params.handle_count = i;
	read_params.multiple.handles = h;
	read_params.multiple.variable = true;
	SET_CHAN_OPT_ANY(read_params);

	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		printf("GATT multiple read request failed (err %d)\n",
			    err);
	}

	return ;
}

static void cmd_read_uuid(int argc, char *argv[])
{
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	if (read_params.func) {
		printf("Read ongoing\n");
		return ;
	}

	read_params.func = read_func;
	read_params.handle_count = 0;
	read_params.by_uuid.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	read_params.by_uuid.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	SET_CHAN_OPT_ANY(read_params);

	if (argc > 1) {
		uuid.val = strtoul(argv[1], NULL, 16);
		if (uuid.val) {
			read_params.by_uuid.uuid = &uuid.uuid;
		}
	}

	if (argc > 2) {
		read_params.by_uuid.start_handle = strtoul(argv[2], NULL, 16);
		if (argc > 3) {
			read_params.by_uuid.end_handle = strtoul(argv[3],
								 NULL, 16);
		}
	}

	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		printf("Read failed (err %d)\n", err);
	} else {
		printf("Read pending\n");
	}
	return ;
}

static struct bt_gatt_write_params write_params;
static uint8_t gatt_write_buf[BT_ATT_MAX_ATTRIBUTE_LEN];

static void write_func(struct bt_conn *conn, uint8_t err,
		       struct bt_gatt_write_params *params)
{
	printf("Write complete: err 0x%02x\n", err);

	(void)memset(&write_params, 0, sizeof(write_params));
}

static void cmd_write(int argc, char *argv[])
{
	int err;
	uint16_t handle, offset;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	if (write_params.func) {
		printf("Write ongoing\n");
		return ;
	}

	handle = strtoul(argv[1], NULL, 16);
	offset = strtoul(argv[2], NULL, 16);

	write_params.length = hex2bin(argv[3], strlen(argv[3]),
				      gatt_write_buf, sizeof(gatt_write_buf));
	if (write_params.length == 0) {
		printf("No data set\n");
		return ;
	}

	write_params.data = gatt_write_buf;
	write_params.handle = handle;
	write_params.offset = offset;
	write_params.func = write_func;
	SET_CHAN_OPT_ANY(write_params);

	err = bt_gatt_write(default_conn, &write_params);
	if (err) {
		write_params.func = NULL;
		printf("Write failed (err %d)\n", err);
	} else {
		printf("Write pending\n");
	}

	return ;
}

static void write_without_rsp_cb(struct bt_conn *conn, void *user_data)
{
	uint16_t len = POINTER_TO_UINT(user_data);

	update_write_stats(len);

	print_write_stats();
}

static void cmd_write_without_rsp(
				int argc, char *argv[])
{
	uint16_t handle;
	uint16_t repeat;
	int err;
	uint16_t len;
	bool sign;
	bt_gatt_complete_func_t func = NULL;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	sign = !strcmp(argv[0], "signed-write");
	if (!sign) {
		if (!strcmp(argv[0], "write-without-response-cb")) {
			func = write_without_rsp_cb;
			reset_write_stats();
		}
	}

	handle = strtoul(argv[1], NULL, 16);
	gatt_write_buf[0] = strtoul(argv[2], NULL, 16);
	len = 1U;

	if (argc > 3) {
		int i;

		len = MIN(strtoul(argv[3], NULL, 16), sizeof(gatt_write_buf));

		for (i = 1; i < len; i++) {
			gatt_write_buf[i] = gatt_write_buf[0];
		}
	}

	repeat = 0U;
	err = 0;

	if (argc > 4) {
		repeat = strtoul(argv[4], NULL, 16);
	}

	if (!repeat) {
		repeat = 1U;
	}

	while (repeat--) {
		err = bt_gatt_write_without_response_cb(default_conn, handle,
							gatt_write_buf, len,
							sign, func,
							UINT_TO_POINTER(len));
		if (err) {
			break;
		}

		k_yield();

	}

	printf("Write Complete (err %d)\n", err);
	return ;
}

static struct bt_gatt_subscribe_params subscribe_params;

static uint8_t notify_func(struct bt_conn *conn,
			struct bt_gatt_subscribe_params *params,
			const void *data, uint16_t length)
{
	if (!data) {
		printf("Unsubscribed\n");
		params->value_handle = 0U;
		return BT_GATT_ITER_STOP;
	}

	printf("Notification: value_handle %u, length %u\n",
		    params->value_handle, length);
	print_data_hex( data, length);

	return BT_GATT_ITER_CONTINUE;
}

static void cmd_subscribe(int argc, char *argv[])
{
	int err;

	if (subscribe_params.value_handle) {
		printf("Cannot subscribe: subscription to %x "
			    "already exists\n", subscribe_params.value_handle);
		return ;
	}

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	subscribe_params.ccc_handle = strtoul(argv[1], NULL, 16);
	subscribe_params.value_handle = strtoul(argv[2], NULL, 16);
	subscribe_params.value = BT_GATT_CCC_NOTIFY;
	subscribe_params.notify = notify_func;
	SET_CHAN_OPT_ANY(subscribe_params);

#if defined(CONFIG_BT_GATT_AUTO_DISCOVER_CCC)
	if (subscribe_params.ccc_handle == 0) {
		static struct bt_gatt_discover_params disc_params;

		subscribe_params.disc_params = &disc_params;
		subscribe_params.end_handle = 0xFFFF;
	}
#endif /* CONFIG_BT_GATT_AUTO_DISCOVER_CCC */


	if (argc > 3 && !strcmp(argv[3], "ind")) {
		subscribe_params.value = BT_GATT_CCC_INDICATE;
	}

	err = bt_gatt_subscribe(default_conn, &subscribe_params);
	if (err) {
		subscribe_params.value_handle = 0U;
		printf("Subscribe failed (err %d)\n", err);
	} else {
		printf("Subscribed\n");
	}

	return ;
}

static void cmd_resubscribe(int argc,
				char *argv[])
{
	bt_addr_le_t addr;
	int err;

	if (subscribe_params.value_handle) {
		printf("Cannot resubscribe: subscription to %x"
			    " already exists\n", subscribe_params.value_handle);
		return ;
	}

	err = bt_addr_le_from_str(argv[1], argv[2], &addr);
	if (err) {
		printf("Invalid peer address (err %d)\n", err);
		return ;
	}

	subscribe_params.ccc_handle = strtoul(argv[3], NULL, 16);
	subscribe_params.value_handle = strtoul(argv[4], NULL, 16);
	subscribe_params.value = BT_GATT_CCC_NOTIFY;
	subscribe_params.notify = notify_func;
	SET_CHAN_OPT_ANY(subscribe_params);

	if (argc > 5 && !strcmp(argv[5], "ind")) {
		subscribe_params.value = BT_GATT_CCC_INDICATE;
	}

	err = bt_gatt_resubscribe(selected_id, &addr, &subscribe_params);
	if (err) {
		subscribe_params.value_handle = 0U;
		printf("Resubscribe failed (err %d)\n", err);
	} else {
		printf("Resubscribed\n");
	}

	return ;
}

static void cmd_unsubscribe(
			  int argc, char *argv[])
{
	int err;

	if (!default_conn) {
		printf("Not connected\n");
		return ;
	}

	if (!subscribe_params.value_handle) {
		printf("No subscription found\n");
		return ;
	}

	err = bt_gatt_unsubscribe(default_conn, &subscribe_params);
	if (err) {
		printf("Unsubscribe failed (err %d)\n", err);
	} else {
		printf("Unsubscribe success\n");
	}

	return ;
}
#endif /* CONFIG_BT_GATT_CLIENT */

static struct db_stats {
	uint16_t svc_count;
	uint16_t attr_count;
	uint16_t chrc_count;
	uint16_t ccc_count;
} stats;

static uint8_t print_attr(const struct bt_gatt_attr *attr, uint16_t handle,
			  void *user_data)
{
	char str[BT_UUID_STR_LEN];

	stats.attr_count++;

	if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_PRIMARY) ||
	    !bt_uuid_cmp(attr->uuid, BT_UUID_GATT_SECONDARY)) {
		stats.svc_count++;
	}

	if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC)) {
		stats.chrc_count++;
	}

	if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC) &&
	    attr->write == bt_gatt_attr_write_ccc) {
		stats.ccc_count++;
	}

	bt_uuid_to_str(attr->uuid, str, sizeof(str));
	printf("attr %p handle 0x%04x uuid %s perm 0x%02x\n",
		    attr, handle, str, attr->perm);

	return BT_GATT_ITER_CONTINUE;
}

static void cmd_show_db(int argc, char *argv[])
{
	struct bt_uuid_16 uuid16;
	size_t total_len;

	memset(&stats, 0, sizeof(stats));

	if (argc > 1) {
		uint16_t num_matches = 0;

		uuid16.uuid.type = BT_UUID_TYPE_16;
		uuid16.val = strtoul(argv[1], NULL, 16);

		if (argc > 2) {
			num_matches = strtoul(argv[2], NULL, 10);
		}

		bt_gatt_foreach_attr_type(0x0001, 0xffff, &uuid16.uuid, NULL,
					  num_matches, print_attr,
					  (void *)NULL);
		return ;
	}

	bt_gatt_foreach_attr(0x0001, 0xffff, print_attr, (void *)NULL);

	if (!stats.attr_count) {
		printf("No attribute found\n");
		return ;
	}

	total_len = stats.svc_count * sizeof(struct bt_gatt_service);
	total_len += stats.chrc_count * sizeof(struct bt_gatt_chrc);
	total_len += stats.attr_count * sizeof(struct bt_gatt_attr);
	total_len += stats.ccc_count * sizeof(struct _bt_gatt_ccc);

	printf("=================================================\n");
	printf("Total: %u services %u attributes (%zu bytes)\n",
		    stats.svc_count, stats.attr_count, total_len);

	return ;
}

#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
/* Custom Service Variables */

static struct bt_uuid_128 vnd_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0));

static struct bt_uuid_128 vnd_auth_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2));

static const struct bt_uuid_128 vnd_long_uuid1 = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef3));

static const struct bt_uuid_128 vnd_long_uuid2 = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x12340, 0x5678cefaadde));

static uint8_t vnd_value[] = { 'V', 'e', 'n', 'd', 'o', 'r' };

static struct bt_uuid_128 vnd1_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x12340, 0x56789abcdef4));

static const struct bt_uuid_128 vnd1_echo_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x12340, 0x56789abcdef5));

static uint8_t echo_enabled;

static void vnd1_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	echo_enabled = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

static ssize_t write_vnd1(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			  const void *buf, uint16_t len, uint16_t offset,
			  uint8_t flags)
{
	if (echo_enabled) {
		printf("Echo attr len %u\n", len);
		bt_gatt_notify(conn, attr, buf, len);
	}

	return len;
}

static ssize_t read_vnd(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 strlen(value));
}

static ssize_t write_vnd(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset,
			 uint8_t flags)
{
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(vnd_value)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);

	return len;
}

#define MAX_DATA 30
static uint8_t vnd_long_value1[MAX_DATA] = { 'V', 'e', 'n', 'd', 'o', 'r' };
static uint8_t vnd_long_value2[MAX_DATA] = { 'S', 't', 'r', 'i', 'n', 'g' };

static ssize_t read_long_vnd(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr, void *buf,
			     uint16_t len, uint16_t offset)
{
	uint8_t *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(vnd_long_value1));
}

static ssize_t write_long_vnd(struct bt_conn *conn,
			      const struct bt_gatt_attr *attr, const void *buf,
			      uint16_t len, uint16_t offset, uint8_t flags)
{
	uint8_t *value = attr->user_data;

	if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
		return 0;
	}

	if (offset + len > sizeof(vnd_long_value1)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	/* Copy to buffer */
	memcpy(value + offset, buf, len);

	return len;
}

static struct bt_gatt_attr vnd_attrs[] = {
	/* Vendor Primary Service Declaration */
	BT_GATT_PRIMARY_SERVICE(&vnd_uuid),

	BT_GATT_CHARACTERISTIC(&vnd_auth_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ_AUTHEN |
			       BT_GATT_PERM_WRITE_AUTHEN,
			       read_vnd, write_vnd, vnd_value),

	BT_GATT_CHARACTERISTIC(&vnd_long_uuid1.uuid, BT_GATT_CHRC_READ |
			       BT_GATT_CHRC_WRITE | BT_GATT_CHRC_EXT_PROP,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE |
			       BT_GATT_PERM_PREPARE_WRITE,
			       read_long_vnd, write_long_vnd,
			       &vnd_long_value1),

	BT_GATT_CHARACTERISTIC(&vnd_long_uuid2.uuid, BT_GATT_CHRC_READ |
			       BT_GATT_CHRC_WRITE | BT_GATT_CHRC_EXT_PROP,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE |
			       BT_GATT_PERM_PREPARE_WRITE,
			       read_long_vnd, write_long_vnd,
			       &vnd_long_value2),
};

static struct bt_gatt_service vnd_svc = BT_GATT_SERVICE(vnd_attrs);

static struct bt_gatt_attr vnd1_attrs[] = {
	/* Vendor Primary Service Declaration */
	BT_GATT_PRIMARY_SERVICE(&vnd1_uuid),

	BT_GATT_CHARACTERISTIC(&vnd1_echo_uuid.uuid,
			       BT_GATT_CHRC_WRITE_WITHOUT_RESP |
			       BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_WRITE, NULL, write_vnd1, NULL),
	BT_GATT_CCC(vnd1_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service vnd1_svc = BT_GATT_SERVICE(vnd1_attrs);

static void cmd_register_test_svc(
				 int argc, char *argv[])
{
	char str[BT_UUID_STR_LEN];
	int err;

	bt_uuid_to_str(&vnd_uuid.uuid, str, sizeof(str));
	err = bt_gatt_service_register(&vnd_svc);
	if (!err) {
		printf("Registered test vendor service %s\n", str);
	} else {
		printf("Failed to register test vendor service %s (%d)\n", str, err);
	}

	bt_uuid_to_str(&vnd1_uuid.uuid, str, sizeof(str));
	err = bt_gatt_service_register(&vnd1_svc);
	if (!err) {
		printf("Registered test vendor service %s\n", str);
	} else {
		printf("Failed to register test vendor service %s (%d)\n", str, err);
	}

	return ;
}

static void cmd_unregister_test_svc(
				   int argc, char *argv[])
{
	char str[BT_UUID_STR_LEN];
	int err;

	bt_uuid_to_str(&vnd_uuid.uuid, str, sizeof(str));
	err = bt_gatt_service_unregister(&vnd_svc);
	if (!err) {
		printf("Unregistered test vendor service %s\n", str);
	} else {
		printf("Failed to unregister test vendor service %s (%d)\n", str, err);
	}

	bt_uuid_to_str(&vnd1_uuid.uuid, str, sizeof(str));
	err = bt_gatt_service_unregister(&vnd1_svc);
	if (!err) {
		printf("Unregistered test vendor service %s\n", str);
	} else {
		printf("Failed to unregister test vendor service %s (%d)\n", str, err);
	}

	return ;
}

static uint8_t found_attr(const struct bt_gatt_attr *attr, uint16_t handle,
			  void *user_data)
{
	const struct bt_gatt_attr **found = user_data;

	*found = attr;

	return BT_GATT_ITER_STOP;
}

static const struct bt_gatt_attr *find_attr(uint16_t handle)
{
	const struct bt_gatt_attr *attr = NULL;

	bt_gatt_foreach_attr(handle, handle, found_attr, &attr);

	return attr;
}

static void cmd_notify(int argc, char *argv[])
{
	const struct bt_gatt_attr *attr;
	int err;
	size_t data_len;
	unsigned long handle;
	static char data[BT_ATT_MAX_ATTRIBUTE_LEN];

	const char *arg_handle = argv[1];
	const char *arg_data = argv[2];
	size_t arg_data_len = strlen(arg_data);

	err = 0;
	handle = strtoul(arg_handle, NULL, 16);
	if (handle == 0) {
		printf("Handle '%s': Not a valid hex number.\n", arg_handle);
		return ;
	}

	if (!IN_RANGE(handle, BT_ATT_FIRST_ATTRIBUTE_HANDLE, BT_ATT_LAST_ATTRIBUTE_HANDLE)) {
		printf("Handle 0x%lx: Impossible value.\n", handle);
		return ;
	}

	if ((arg_data_len / 2) > BT_ATT_MAX_ATTRIBUTE_LEN) {
		printf("Data: Size exceeds legal attribute size.\n");
		return ;
	}

	data_len = hex2bin(arg_data, arg_data_len, data, sizeof(data));
	if (data_len == 0 && arg_data_len != 0) {
		printf("Data: Bad hex.\n");
		return ;
	}

	attr = find_attr(handle);
	if (!attr) {
		printf("Handle 0x%lx: Local attribute not found.\n", handle);
		return ;
	}

	err = bt_gatt_notify(NULL, attr, data, data_len);
	if (err) {
		printf("bt_gatt_notify errno %d (%s)\n", -err, strerror(-err));
	}

	return ;
}

#if defined(CONFIG_BT_GATT_NOTIFY_MULTIPLE)
static void notify_cb(struct bt_conn *conn, void *user_data)
{
	(void)user_data;

	printf("Nofication sent to conn %p\n", conn);
}
static void cmd_notify_mult(int argc, char *argv[])
{
	const size_t max_cnt = CONFIG_BT_L2CAP_TX_BUF_COUNT;
	struct bt_gatt_notify_params params[max_cnt];
	const size_t min_cnt = 1U;
	unsigned long data;
	unsigned long cnt;
	uint16_t cnt_u16;
	int err = 0;

	if (!default_conn) {
		printf("Not connected.\n");

		return ;
	}

	if (!echo_enabled) {
		printf("No clients have enabled notifications for the vnd1_echo CCC.\n");

		return ;
	}

	cnt = strtoul(argv[1], NULL, 10);
	if (cnt == 0) {
		printf("Invalid count parameter: %s\n", argv[1]);

		return ;
	}

	if (!IN_RANGE(cnt, min_cnt, max_cnt)) {
		printf("Invalid count value %lu (range %zu to %zu)\n",
			    cnt, min_cnt, max_cnt);

		return ;
	}

	cnt_u16 = (uint16_t)cnt;

	if (argc > 2) {
		data = strtoul(argv[2], NULL, 16);
        ///Illegal hexstring will force to 00;
		///if (err != 0) {
		///	printf("Invalid data parameter: %s\n", argv[1]);
        ///
		///	return ;
		///}
	}

	(void)memset(params, 0, sizeof(params));

	for (uint16_t i = 0U; i < cnt_u16; i++) {
		params[i].uuid = 0;
		params[i].attr = vnd1_attrs;
		params[i].data = &data;
		params[i].len = sizeof(data);
		params[i].func = notify_cb;
		params[i].user_data = (void *)NULL;
	}

	err = bt_gatt_notify_multiple(default_conn, cnt_u16, params);
	if (err != 0) {
		printf("bt_gatt_notify_multiple failed: %d\n", err);
	} else {
		printf("Send %u notifications\n", cnt_u16);
	}

	return ;
}
#endif /* CONFIG_BT_GATT_NOTIFY_MULTIPLE */

static struct bt_uuid_128 met_svc_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcde01));
static const struct bt_uuid_128 met_char_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcde02));

static uint8_t met_char_value[BT_ATT_MAX_ATTRIBUTE_LEN] = {
	'M', 'e', 't', 'r', 'i', 'c', 's' };

static ssize_t read_met(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;
	uint16_t value_len;

	value_len = MIN(strlen(value), BT_ATT_MAX_ATTRIBUTE_LEN);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 value_len);
}

static ssize_t write_met(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset,
			 uint8_t flags)
{
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(met_char_value)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);

	update_write_stats(len);

	return len;
}

static struct bt_gatt_attr met_attrs[] = {
	BT_GATT_PRIMARY_SERVICE(&met_svc_uuid),

	BT_GATT_CHARACTERISTIC(&met_char_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_met, write_met, met_char_value),
};

static struct bt_gatt_service met_svc = BT_GATT_SERVICE(met_attrs);

static void cmd_metrics(int argc, char *argv[])
{
	int err = 0;

	if (argc < 2) {
		print_write_stats();
		return ;
	}

	if (!strcmp(argv[1], "on")) {
		printf("Registering GATT metrics test Service.\n");
		err = bt_gatt_service_register(&met_svc);
	} else if (!strcmp(argv[1], "off")) {
		printf("Unregistering GATT metrics test Service.\n");
		err = bt_gatt_service_unregister(&met_svc);
	} else {
		printf("Incorrect value: %s\n", argv[1]);
		return ;
	}

	if (!err) {
		printf("GATT write cmd metrics %s.\n", argv[1]);
	}

	return ;
}
#endif /* CONFIG_BT_GATT_DYNAMIC_DB */

static uint8_t get_cb(const struct bt_gatt_attr *attr, uint16_t handle,
		      void *user_data)
{
	uint8_t buf[256];
	ssize_t ret;
	char str[BT_UUID_STR_LEN];

	bt_uuid_to_str(attr->uuid, str, sizeof(str));
	printf("attr %p uuid %s perm 0x%02x\n", attr, str,
		    attr->perm);

	if (!attr->read) {
		return BT_GATT_ITER_CONTINUE;
	}

	ret = attr->read(NULL, attr, (void *)buf, sizeof(buf), 0);
	if (ret < 0) {
		printf("Failed to read: %zd\n", ret);
		return BT_GATT_ITER_STOP;
	}

	print_data_hex(buf, ret);

	return BT_GATT_ITER_CONTINUE;
}

static void cmd_get(int argc, char *argv[])
{
	uint16_t start, end;

	start = strtoul(argv[1], NULL, 16);
	end = start;

	if (argc > 2) {
		end = strtoul(argv[2], NULL, 16);
	}

	bt_gatt_foreach_attr(start, end, get_cb, (void *)NULL);

	return ;
}

struct set_data {
	size_t argc;
	char **argv;
	int err;
};

static uint8_t set_cb(const struct bt_gatt_attr *attr, uint16_t handle,
		      void *user_data)
{
	struct set_data *data = user_data;
	uint8_t buf[256];
	size_t i;
	ssize_t ret;

	if (!attr->write) {
		printf("Write not supported\n");
		data->err = -ENOENT;
		return BT_GATT_ITER_CONTINUE;
	}

	for (i = 0; i < data->argc; i++) {
		buf[i] = strtoul(data->argv[i], NULL, 16);
	}

	ret = attr->write(NULL, attr, (void *)buf, i, 0, 0);
	if (ret < 0) {
		data->err = ret;
		printf("Failed to write: %zd\n", ret);
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

static void cmd_set(int argc, char *argv[])
{
	uint16_t handle;
	struct set_data data;

	handle = strtoul(argv[1], NULL, 16);

	data.argc = argc - 2;
	data.argv = argv + 2;
	data.err = 0;

	bt_gatt_foreach_attr(handle, handle, set_cb, &data);

	if (data.err < 0) {
		return ;
	}

	bt_gatt_foreach_attr(handle, handle, get_cb, (void *)NULL);

	return ;
}

void cmd_att_mtu(int argc, char *argv[])
{
	uint16_t mtu;

	if (default_conn) {
		mtu = bt_gatt_get_mtu(default_conn);
		printf("MTU size: %u", mtu);
	} else {
		printf("No default connection\n");
	}
}


#if defined(CONFIG_BT_GATT_CLIENT)
WM_CLI_CMD_DEFINE(btdiscover, cmd_discover, discover service cmd, btdiscover [UUID] [start handle] [end handle] -- btdiscover cmd);
WM_CLI_CMD_DEFINE(btdiscovercharacter, cmd_discover, discover characteristic cmd, btdiscovercharacter [UUID] [start handle] [end handle] -- btdiscovercharacter cmd);
WM_CLI_CMD_DEFINE(btdiscoverdescriptor, cmd_discover, discover descriptor cmd, btdiscoverdescriptor [UUID] [start handle] [end handle] -- btdiscoverdescriptor cmd);
WM_CLI_CMD_DEFINE(btdiscoverinclude, cmd_discover, discover include service cmd, btdiscoverinclude [UUID] [start handle] [end handle] -- btdiscoverinclude cmd);
WM_CLI_CMD_DEFINE(btdiscoverprimary, cmd_discover, discover primary service cmd, btdiscoverprimary [UUID] [start handle] [end handle] -- btdiscoverprimary cmd);
WM_CLI_CMD_DEFINE(btdiscoversecondary, cmd_discover, discover secondary service cmd, btdiscoversecondary [UUID] [start handle] [end handle] -- btdiscoversecondary cmd);

WM_CLI_CMD_DEFINE(btexchangemtu, cmd_exchange_mtu, exchange ATT mtu cmd, btexchangemtu -- btexchangemtu cmd);
WM_CLI_CMD_DEFINE(btread, cmd_read, read character or descriptor by handle cmd, btread <handle> [offset] -- btread cmd);
WM_CLI_CMD_DEFINE(btreaduuid, cmd_read_uuid, read character or descriptor by uuid cmd, btreaduuid <UUID> [start handle] [end handle] -- btreaduuid cmd);
WM_CLI_CMD_DEFINE(btreadmultiple, cmd_mread, read multiple by handle cmd, btreadmultiple <handle 1> <handle 2> ... -- btreadmultiple cmd);
WM_CLI_CMD_DEFINE(btsignedwrite, cmd_write_without_rsp, signed write cmd, btsignedwrite <handle> <data> [length] [repeat] -- btsignedwrite cmd);
WM_CLI_CMD_DEFINE(btsubscribe, cmd_subscribe, subscribe notification or indication cmd, btsubscribe <CCC handle> <value handle> [ind] -- btsubscribe cmd);
WM_CLI_CMD_DEFINE(btresubscribe, cmd_resubscribe, resubscribe notification or indication cmd, btresubscribe <CCC handle> -- btresubscribe cmd);
WM_CLI_CMD_DEFINE(btwrite, cmd_write, write cmd, btwrite <handle> <offset> <data> -- btwrite cmd);
WM_CLI_CMD_DEFINE(btwritewithoutrsp, cmd_write_without_rsp, write without response cmd, btwritewithoutrsp <handle> <data> [length] [repeat] -- btwritewithoutrsp cmd);
WM_CLI_CMD_DEFINE(btunsubscribe, cmd_unsubscribe, unsubscribe notification or indication cmd, btunsubscribe -- btunsubscribe cmd);
#endif /* CONFIG_BT_GATT_CLIENT */
WM_CLI_CMD_DEFINE(btget, cmd_get, read remote handle list cmd, btget <start handle> [end handle] -- btget cmd);
WM_CLI_CMD_DEFINE(btset, cmd_set, write by handle cmd, btset <handle> [data...] -- btset cmd);
WM_CLI_CMD_DEFINE(btshowdb, cmd_show_db, show gatt database cmd, btshowdb [uuid] [num_matches] -- btshowdb cmd);
WM_CLI_CMD_DEFINE(btattmtu, cmd_att_mtu, read mtu of default connection cmd, btattmtu -- btattmtu cmd);
#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
WM_CLI_CMD_DEFINE(btmetrics, cmd_metrics, show throughtput cmd, btmetrics [value: on, off] -- btmetrics cmd);
WM_CLI_CMD_DEFINE(btregister, cmd_register_test_svc, register test service cmd, btregister -- btregister cmd);
WM_CLI_CMD_DEFINE(btunregister, cmd_unregister_test_svc, unregister test service cmd, btunregister -- btunregister cmd);
WM_CLI_CMD_DEFINE(btnotify, cmd_notify, notify write cmd, btnotify <handle> <data> -- btnotify cmd);

#if defined(CONFIG_BT_GATT_NOTIFY_MULTIPLE)
WM_CLI_CMD_DEFINE(btnotifymult, cmd_notify_mult, multiply notify write cmd, btnotifymult <count [data]> -- btnotifymult cmd);
#endif /* CONFIG_BT_GATT_NOTIFY_MULTIPLE */
#endif /* CONFIG_BT_GATT_DYNAMIC_DB */

