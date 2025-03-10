/* Copyright (c) 2022 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

/* Helper for printk parameters to convert from binary to hex.
 * We declare multiple buffers so the helper can be used multiple times
 * in a single printk call.
 */

#include <stddef.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>
//#include <kernel.h>
//#include <sys/util.h>
//#include <types.h>

const char *bt_hex(const void *buf, size_t len)
{
	static const char hex[] = "0123456789abcdef";
	static char str[129];
	const uint8_t *b = buf;
	size_t i;

	len = MIN(len, (sizeof(str) - 1) / 2);

	for (i = 0; i < len; i++) {
		str[i * 2] = hex[b[i] >> 4];
		str[i * 2 + 1] = hex[b[i] & 0xf];
	}

	str[i * 2] = '\0';

	return str;
}

const char *bt_addr_str(const bt_addr_t *addr)
{
	static char str[BT_ADDR_STR_LEN];

	bt_addr_to_str(addr, str, sizeof(str));

	return str;
}

const char *bt_addr_le_str(const bt_addr_le_t *addr)
{
	static char str[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(addr, str, sizeof(str));

	return str;
}

const char *bt_uuid_str(const struct bt_uuid *uuid)
{
	static char str[BT_UUID_STR_LEN];

	bt_uuid_to_str(uuid, str, sizeof(str));

	return str;
}
