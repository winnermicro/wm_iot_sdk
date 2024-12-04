/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dfu.h"

#include <stdlib.h>
#include <kernel.h>
#include <sys/byteorder.h>
#include <sys/util.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/shell.h>
///#include <dfu/mcuboot.h>
///#include <storage/flash_map.h>

#include "utils.h"
#include "blob.h"
#include "../dfu_slot.h"

struct mcuboot_img_sem_ver {
	uint8_t major;
	uint8_t minor;
	uint16_t revision;
	uint32_t build_num;
};

/***************************************************************************************************
 * Implementation of models' instances
 **************************************************************************************************/

extern const struct shell *bt_mesh_shell_ctx_shell;

#if defined(CONFIG_BT_MESH_SHELL_DFU_CLI)

static void dfu_cli_ended(struct bt_mesh_dfu_cli *cli,
			  enum bt_mesh_dfu_status reason)
{
	printf( "DFU ended: %u", reason);
}

static void dfu_cli_applied(struct bt_mesh_dfu_cli *cli)
{
	printf( "DFU applied.");
}

static void dfu_cli_lost_target(struct bt_mesh_dfu_cli *cli,
				struct bt_mesh_dfu_target *target)
{
	printf( "DFU target lost: 0x%04x\n", target->blob.addr);
}

static void dfu_cli_confirmed(struct bt_mesh_dfu_cli *cli)
{
	printf( "DFU confirmed\n");
}

const struct bt_mesh_dfu_cli_cb dfu_cli_cb = {
	.ended = dfu_cli_ended,
	.applied = dfu_cli_applied,
	.lost_target = dfu_cli_lost_target,
	.confirmed = dfu_cli_confirmed,
};

struct bt_mesh_dfu_cli bt_mesh_shell_dfu_cli = BT_MESH_DFU_CLI_INIT(&dfu_cli_cb);

#endif /* CONFIG_BT_MESH_SHELL_DFU_CLI */

#if defined(CONFIG_BT_MESH_SHELL_DFU_SRV)

struct shell_dfu_fwid {
	uint8_t type;
	struct mcuboot_img_sem_ver ver;
};

static struct bt_mesh_dfu_img dfu_imgs[] = { {
	.fwid = &((struct shell_dfu_fwid){ 0x01, { 1, 0, 0, 0 } }),
	.fwid_len = sizeof(struct shell_dfu_fwid),
} };

static int dfu_meta_check(struct bt_mesh_dfu_srv *srv,
			  const struct bt_mesh_dfu_img *img,
			  struct net_buf_simple *metadata,
			  enum bt_mesh_dfu_effect *effect)
{
	return 0;
}

static int dfu_start(struct bt_mesh_dfu_srv *srv,
		     const struct bt_mesh_dfu_img *img,
		     struct net_buf_simple *metadata,
		     const struct bt_mesh_blob_io **io)
{
	printf( "DFU setup\n");

	*io = bt_mesh_shell_blob_io;

	return 0;
}

static void dfu_end(struct bt_mesh_dfu_srv *srv, const struct bt_mesh_dfu_img *img, bool success)
{
	if (!success) {
		printf( "DFU failed\n");
		return;
	}

	if (!bt_mesh_shell_blob_valid) {
		bt_mesh_dfu_srv_rejected(srv);
		return;
	}

	bt_mesh_dfu_srv_verified(srv);
}

static int dfu_apply(struct bt_mesh_dfu_srv *srv,
		     const struct bt_mesh_dfu_img *img)
{
	if (!bt_mesh_shell_blob_valid) {
		return -EINVAL;
	}

	printf( "Applying DFU transfer...\n");

	return 0;
}

static const struct bt_mesh_dfu_srv_cb dfu_handlers = {
	.check = dfu_meta_check,
	.start = dfu_start,
	.end = dfu_end,
	.apply = dfu_apply,
};

struct bt_mesh_dfu_srv bt_mesh_shell_dfu_srv =
	BT_MESH_DFU_SRV_INIT(&dfu_handlers, dfu_imgs, ARRAY_SIZE(dfu_imgs));

#endif /* CONFIG_BT_MESH_SHELL_DFU_SRV */

void bt_mesh_shell_dfu_cmds_init(void)
{
#if defined(CONFIG_BT_MESH_SHELL_DFU_SRV) && defined(CONFIG_BOOTLOADER_MCUBOOT)
///  TODO 
#if 0
	struct mcuboot_img_header img_header;

	int err = boot_read_bank_header(FIXED_PARTITION_ID(slot0_partition),
					&img_header, sizeof(img_header));
	if (!err) {
		struct shell_dfu_fwid *fwid =
			(struct shell_dfu_fwid *)dfu_imgs[0].fwid;

		fwid->ver = img_header.h.v1.sem_ver;

		boot_write_img_confirmed();
	}
#endif
#endif
}

/***************************************************************************************************
 * Shell Commands
 **************************************************************************************************/

#if defined(CONFIG_BT_MESH_SHELL_DFU_METADATA)

NET_BUF_SIMPLE_DEFINE_STATIC(dfu_comp_data, BT_MESH_TX_SDU_MAX);

static void cmd_dfu_comp_clear(int argc, char *argv[])
{
	net_buf_simple_reset(&dfu_comp_data);
}

static void cmd_dfu_comp_add(int argc, char *argv[])
{
	struct net_buf_simple_state state;
	int err = 0;

	if (argc < 6) {
		return -EINVAL;
	}

	if (net_buf_simple_tailroom(&dfu_comp_data) < 10) {
		printf("Buffer is too small: %u\n",
			    net_buf_simple_tailroom(&dfu_comp_data));
		return -EMSGSIZE;
	}

	net_buf_simple_save(&dfu_comp_data, &state);

	for (size_t i = 1; i <= 5; i++) {
		net_buf_simple_add_le16(&dfu_comp_data, wm_atoi(argv[i],  &err););
	}

	if (err) {
		net_buf_simple_restore(&dfu_comp_data, &state);
		printf("Unable to parse input string argument\n");
		return err;
	}

	return 0;
}

static void cmd_dfu_comp_elem_add(int argc, char *argv[])
{
	uint8_t sig_model_count;
	uint8_t vnd_model_count;
	struct net_buf_simple_state state;
	int err = 0;

	if (argc < 5) {
		return ;
	}

	net_buf_simple_save(&dfu_comp_data, &state);

	sig_model_count = wm_atoi(argv[2], &err);
	vnd_model_count = wm_atoi(argv[3], &err);

	if (argc < 4 + sig_model_count + vnd_model_count * 2) {
		return ;
	}

	if (net_buf_simple_tailroom(&dfu_comp_data) < 4 + sig_model_count * 2 +
	    vnd_model_count * 4) {
		printf("Buffer is too small: %u\n",
			    net_buf_simple_tailroom(&dfu_comp_data));
		return ;
	}

	net_buf_simple_add_le16(&dfu_comp_data, wm_atoi(argv[1],  &err););
	net_buf_simple_add_u8(&dfu_comp_data, sig_model_count);
	net_buf_simple_add_u8(&dfu_comp_data, vnd_model_count);

	for (size_t i = 0; i < sig_model_count; i++) {
		net_buf_simple_add_le16(&dfu_comp_data, wm_atoi(argv[4 + i],  &err););
	}

	for (size_t i = 0; i < vnd_model_count; i++) {
		size_t arg_i = 4 + sig_model_count + i * 2;

		net_buf_simple_add_le16(&dfu_comp_data, wm_atoi(argv[arg_i],  &err););
		net_buf_simple_add_le16(&dfu_comp_data, wm_atoi(argv[arg_i + 1],  &err););
	}

	if (err) {
		net_buf_simple_restore(&dfu_comp_data, &state);
		printf("Unable to parse input string argument\n");
	}

}

static void cmd_dfu_comp_hash_get(int argc, char *argv[])
{
	uint8_t key[16] = {};
	uint32_t hash;
	int err;

	if (dfu_comp_data.len < 14) {
		printf("Composition data is not set\n");
		return ;
	}

	if (argc > 1) {
		hex2bin(argv[1], strlen(argv[1]), key, sizeof(key));
	}

	printf("Composition data to be hashed:\n");
	printf("\tCID: 0x%04x\n", sys_get_le16(&dfu_comp_data.data[0]));
	printf("\tPID: 0x%04x\n", sys_get_le16(&dfu_comp_data.data[2]));
	printf("\tVID: 0x%04x\n", sys_get_le16(&dfu_comp_data.data[4]));
	printf("\tCPRL: %u\n", sys_get_le16(&dfu_comp_data.data[6]));
	printf("\tFeatures: 0x%x\n", sys_get_le16(&dfu_comp_data.data[8]));

	for (size_t i = 10; i < dfu_comp_data.len - 4;) {
		uint8_t sig_model_count = dfu_comp_data.data[i + 2];
		uint8_t vnd_model_count = dfu_comp_data.data[i + 3];

		printf("\tElem: %u\n", sys_get_le16(&dfu_comp_data.data[i]));
		printf("\t\tNumS: %u", sig_model_count);
		printf("\t\tNumV: %u", vnd_model_count);

		for (size_t j = 0; j < sig_model_count; j++) {
			printf("\t\tSIG Model ID: 0x%04x",
				    sys_get_le16(&dfu_comp_data.data[i + 4 + j * 2]));
		}

		for (size_t j = 0; j < vnd_model_count; j++) {
			size_t arg_i = i + 4 + sig_model_count * 2 + j * 4;

			printf("\t\tVnd Company ID: 0x%04x, Model ID: 0x%04x",
				    sys_get_le16(&dfu_comp_data.data[arg_i]),
				    sys_get_le16(&dfu_comp_data.data[arg_i + 2]));
		}

		i += 4 + sig_model_count * 2 + vnd_model_count * 4;
	}

	err = bt_mesh_dfu_metadata_comp_hash_get(&dfu_comp_data, key, &hash);
	if (err) {
		printf("Failed to compute composition data hash: %d\n", err);
		return ;
	}

	printf("Composition data hash: 0x%04x\n", hash);

}

static void cmd_dfu_metadata_encode(int argc, char *argv[])
{
	char md_str[2 * CONFIG_BT_MESH_DFU_METADATA_MAXLEN + 1];
	uint8_t user_data[CONFIG_BT_MESH_DFU_METADATA_MAXLEN - 18];
	struct bt_mesh_dfu_metadata md;
	size_t len;
	int err = 0;

	NET_BUF_SIMPLE_DEFINE(buf, CONFIG_BT_MESH_DFU_METADATA_MAXLEN);

	if (argc < 9) {
		return ;
	}

	md.fw_ver.major = wm_atoi(argv[1], &err);
	md.fw_ver.minor = wm_atoi(argv[2], &err);
	md.fw_ver.revision = wm_atoi(argv[3], &err);
	md.fw_ver.build_num = wm_atoi(argv[4], &err);
	md.fw_size = wm_atoi(argv[5], &err);
	md.fw_core_type = wm_atoi(argv[6], &err);
	md.comp_hash = wm_atoi(argv[7], &err);
	md.elems = wm_atoi(argv[8], &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (argc > 9) {
		if (sizeof(user_data) < strlen(argv[9]) / 2) {
			printf("User data is too big.\n");
			return ;
		}

		md.user_data_len = hex2bin(argv[9], strlen(argv[9]), user_data, sizeof(user_data));
		md.user_data = user_data;
	} else {
		md.user_data_len = 0;
	}

	printf("Metadata to be encoded:\n");
	printf("\tVersion: %u.%u.%u+%u\n", md.fw_ver.major, md.fw_ver.minor,
		    md.fw_ver.revision, md.fw_ver.build_num);
	printf("\tSize: %u\n", md.fw_size);
	printf("\tCore Type: 0x%x\n", md.fw_core_type);
	printf("\tComposition data hash: 0x%x\n", md.comp_hash);
	printf("\tElements: %u\n", md.elems);

	if (argc > 9) {
		printf("\tUser data: %s\n", argv[10]);
	}

	printf("\tUser data length: %u\n", md.user_data_len);

	err = bt_mesh_dfu_metadata_encode(&md, &buf);
	if (err) {
		printf("Failed to encode metadata: %d\n", err);
		return err;
	}

	len = bin2hex(buf.data, buf.len, md_str, sizeof(md_str));
	md_str[len] = '\0';
	printf("Encoded metadata: %s\n", md_str);

}

#endif /* CONFIG_BT_MESH_SHELL_DFU_METADATA */

#if defined(CONFIG_BT_MESH_SHELL_DFU_SLOT)

static void cmd_dfu_slot_add(int argc, char *argv[])
{
	struct bt_mesh_dfu_slot *slot;
	size_t size;
	uint8_t fwid[CONFIG_BT_MESH_DFU_FWID_MAXLEN];
	size_t fwid_len = 0;
	uint8_t metadata[CONFIG_BT_MESH_DFU_METADATA_MAXLEN];
	size_t metadata_len = 0;
	int err = 0;

	size = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	printf("Adding slot (size: %u)\n", size);
	slot = bt_mesh_dfu_slot_reserve();

	if (!slot) {
		printf("Failed to reserve slot.\n");
		return ;
	}

	fwid_len = hex2bin(argv[2], strlen(argv[2]), fwid,
			   sizeof(fwid));
	bt_mesh_dfu_slot_fwid_set(slot, fwid, fwid_len);

	if (argc > 3) {
		metadata_len = hex2bin(argv[3], strlen(argv[3]), metadata,
				       sizeof(metadata));
	}

	bt_mesh_dfu_slot_info_set(slot, size, metadata, metadata_len);

	err = bt_mesh_dfu_slot_commit(slot);
	if (err) {
		printf("Failed to commit slot: %d\n", err);
		bt_mesh_dfu_slot_release(slot);
		return ;
	}

	printf("Slot added. Index: %u", bt_mesh_dfu_slot_img_idx_get(slot));
}

static void cmd_dfu_slot_del(int argc, char *argv[])
{
	const struct bt_mesh_dfu_slot *slot;
	uint8_t idx;
	int err = 0;

	idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	slot = bt_mesh_dfu_slot_at(idx);
	if (!slot) {
		printf("No slot at %u", idx);
		return ;
	}

	err = bt_mesh_dfu_slot_del(slot);
	if (err) {
		printf("Failed deleting slot %u (err: %d)\n", idx,
			    err);
		return ;
	}

	printf("Slot %u deleted.\n", idx);
}

static void cmd_dfu_slot_del_all(int argc, char *argv[])
{
	bt_mesh_dfu_slot_del_all();
	printf("All slots deleted.\n");
}

static void slot_info_print(  const struct bt_mesh_dfu_slot *slot,
			    const uint8_t *idx)
{
	char fwid[2 * CONFIG_BT_MESH_DFU_FWID_MAXLEN + 1];
	char metadata[2 * CONFIG_BT_MESH_DFU_METADATA_MAXLEN + 1];
	size_t len;

	len = bin2hex(slot->fwid, slot->fwid_len, fwid, sizeof(fwid));
	fwid[len] = '\0';
	len = bin2hex(slot->metadata, slot->metadata_len, metadata,
		      sizeof(metadata));
	metadata[len] = '\0';

	if (idx != NULL) {
		printf("Slot %u:\n", *idx);
	} else {
		printf("Slot:\n");
	}
	printf("\tSize:     %u bytes\n", slot->size);
	printf("\tFWID:     %s\n", fwid);
	printf("\tMetadata: %s\n", metadata);
}

static void cmd_dfu_slot_get(int argc, char *argv[])
{
	const struct bt_mesh_dfu_slot *slot;
	uint8_t idx;
	int err = 0;

	idx = wm_atoi(argv[1], &err);
	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	slot = bt_mesh_dfu_slot_at(idx);
	if (!slot) {
		printf("No slot at %u\n", idx);
		return ;
	}

	slot_info_print(slot, &idx);
}

#endif /* defined(CONFIG_BT_MESH_SHELL_DFU_SLOT) */

#if defined(CONFIG_BT_MESH_SHELL_DFU_CLI)

static struct bt_mesh_model *mod_cli;

static struct {
	struct bt_mesh_dfu_target targets[32];
	struct bt_mesh_blob_target_pull pull[32];
	size_t target_cnt;
	struct bt_mesh_blob_cli_inputs inputs;
} dfu_tx;

static void dfu_tx_prepare(void)
{
	sys_slist_init(&dfu_tx.inputs.targets);

	for (size_t i = 0; i < dfu_tx.target_cnt; i++) {
		/* Reset target context. */
		uint16_t addr = dfu_tx.targets[i].blob.addr;

		memset(&dfu_tx.targets[i].blob, 0, sizeof(struct bt_mesh_blob_target));
		memset(&dfu_tx.pull[i], 0, sizeof(struct bt_mesh_blob_target_pull));
		dfu_tx.targets[i].blob.addr = addr;
		dfu_tx.targets[i].blob.pull = &dfu_tx.pull[i];

		sys_slist_append(&dfu_tx.inputs.targets, &dfu_tx.targets[i].blob.n);
	}
}

static void cmd_dfu_target(int argc, char *argv[])
{
	uint8_t img_idx;
	uint16_t addr;
	int err = 0;

	addr = wm_atoi(argv[1], &err);
	img_idx = wm_atoi(argv[2], &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (dfu_tx.target_cnt == ARRAY_SIZE(dfu_tx.targets)) {
		printf("No room.\n");
		return ;
	}

	for (size_t i = 0; i < dfu_tx.target_cnt; i++) {
		if (dfu_tx.targets[i].blob.addr == addr) {
			printf("Target 0x%04x already exists\n", addr);
			return ;
		}
	}

	dfu_tx.targets[dfu_tx.target_cnt].blob.addr = addr;
	dfu_tx.targets[dfu_tx.target_cnt].img_idx = img_idx;
	sys_slist_append(&dfu_tx.inputs.targets, &dfu_tx.targets[dfu_tx.target_cnt].blob.n);
	dfu_tx.target_cnt++;

	printf("Added target 0x%04x\n", addr);

}

static void cmd_dfu_targets_reset(int argc, char *argv[])
{
	dfu_tx_prepare();
}

static void cmd_dfu_target_state(int argc, char *argv[])
{
	struct bt_mesh_dfu_target_status rsp;
	struct bt_mesh_msg_ctx ctx = {
		.send_ttl = BT_MESH_TTL_DEFAULT,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.addr = bt_mesh_shell_target_ctx.dst,
		.app_idx = bt_mesh_shell_target_ctx.app_idx,
	};
	int err;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	err = bt_mesh_dfu_cli_status_get((struct bt_mesh_dfu_cli *)mod_cli->user_data,
					 &ctx, &rsp);
	if (err) {
		printf("Failed getting target status (err: %d)\n",
			    err);
		return ;
	}

	printf("Target 0x%04x:\n", bt_mesh_shell_target_ctx.dst);
	printf("\tStatus:     %u\n", rsp.status);
	printf("\tPhase:      %u\n", rsp.phase);
	if (rsp.phase != BT_MESH_DFU_PHASE_IDLE) {
		printf("\tEffect:       %u\n", rsp.effect);
		printf("\tImg Idx:      %u\n", rsp.img_idx);
		printf("\tTTL:          %u\n", rsp.ttl);
		printf("\tTimeout base: %u\n", rsp.timeout_base);
	}

}

static enum bt_mesh_dfu_iter dfu_img_cb(struct bt_mesh_dfu_cli *cli,
					struct bt_mesh_msg_ctx *ctx,
					uint8_t idx, uint8_t total,
					const struct bt_mesh_dfu_img *img,
					void *cb_data)
{
	char fwid[2 * CONFIG_BT_MESH_DFU_FWID_MAXLEN + 1];
	size_t len;

	len = bin2hex(img->fwid, img->fwid_len, fwid, sizeof(fwid));
	fwid[len] = '\0';

	printf( "Image %u:\n", idx);
	printf( "\tFWID: %s\n", fwid);
	if (img->uri) {
		printf( "\tURI:  %s\n", img->uri);
	}

	return BT_MESH_DFU_ITER_CONTINUE;
}

static void cmd_dfu_target_imgs(int argc, char *argv[])
{
	struct bt_mesh_msg_ctx ctx = {
		.send_ttl = BT_MESH_TTL_DEFAULT,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.addr = bt_mesh_shell_target_ctx.dst,
		.app_idx = bt_mesh_shell_target_ctx.app_idx,
	};
	uint8_t img_cnt = 0xff;
	int err = 0;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	if (argc == 2) {
		img_cnt = wm_atoi(argv[1], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return err;
		}
	}

	printf("Requesting DFU images in 0x%04x\n", bt_mesh_shell_target_ctx.dst);

	err = bt_mesh_dfu_cli_imgs_get((struct bt_mesh_dfu_cli *)mod_cli->user_data,
				       &ctx, dfu_img_cb, NULL, img_cnt);
	if (err) {
		printf("Request failed (err: %d)\n", err);
	}
}

static void cmd_dfu_target_check(int argc, char *argv[])
{
	struct bt_mesh_dfu_metadata_status rsp;
	const struct bt_mesh_dfu_slot *slot;
	struct bt_mesh_msg_ctx ctx = {
		.send_ttl = BT_MESH_TTL_DEFAULT,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.addr = bt_mesh_shell_target_ctx.dst,
		.app_idx = bt_mesh_shell_target_ctx.app_idx,
	};
	uint8_t slot_idx, img_idx;
	int err = 0;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	slot_idx = wm_atoi(argv[1], &err);
	img_idx = wm_atoi(argv[2], &err);

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	slot = bt_mesh_dfu_slot_at(slot_idx);
	if (!slot) {
		printf("No image in slot %u\n", slot_idx);
		return ;
	}

	err = bt_mesh_dfu_cli_metadata_check((struct bt_mesh_dfu_cli *)mod_cli->user_data,
					     &ctx, img_idx, slot, &rsp);
	if (err) {
		printf("Metadata check failed. err: %d\n", err);
		return ;
	}

	printf("Slot %u check for 0x%04x image %u:\n", slot_idx,
		    bt_mesh_shell_target_ctx.dst, img_idx);
	printf("\tStatus: %u\n", rsp.status);
	printf("\tEffect: 0x%x\n", rsp.effect);

}

static void cmd_dfu_send(int argc, char *argv[])
{
	struct bt_mesh_dfu_cli_xfer_blob_params blob_params;
	struct bt_mesh_dfu_cli_xfer xfer = { 0 };
	uint8_t slot_idx;
	uint16_t group;
	int err = 0;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	slot_idx = wm_atoi(argv[1], &err);
	if (argc > 2) {
		group = wm_atoi(argv[2], &err);
	} else {
		group = BT_MESH_ADDR_UNASSIGNED;
	}

	if (argc > 3) {
		xfer.mode = wm_atoi(argv[3], &err);
	} else {
		xfer.mode = BT_MESH_BLOB_XFER_MODE_PUSH;
	}

	if (argc > 5) {
		blob_params.block_size_log = wm_atoi(argv[4], &err);
		blob_params.chunk_size = wm_atoi(argv[5], &err);
		xfer.blob_params = &blob_params;
	} else {
		xfer.blob_params = NULL;
	}

	if (err) {
		printf("Unable to parse input string argument\n");
		return ;
	}

	if (!dfu_tx.target_cnt) {
		printf("No targets.\n");
		return ;
	}

	xfer.slot = bt_mesh_dfu_slot_at(slot_idx);
	if (!xfer.slot) {
		printf("No image in slot %u\n", slot_idx);
		return ;
	}

	printf("Starting DFU from slot %u (%u targets)\n", slot_idx,
		    dfu_tx.target_cnt);

	dfu_tx.inputs.group = group;
	dfu_tx.inputs.app_idx = bt_mesh_shell_target_ctx.app_idx;
	dfu_tx.inputs.ttl = BT_MESH_TTL_DEFAULT;

	err = bt_mesh_dfu_cli_send((struct bt_mesh_dfu_cli *)mod_cli->user_data,
				   &dfu_tx.inputs, bt_mesh_shell_blob_io, &xfer);
	if (err) {
		printf("Failed (err: %d)\n", err);
	}
}

static void cmd_dfu_tx_cancel(int argc, char *argv[])
{
	struct bt_mesh_msg_ctx ctx = {
		.send_ttl = BT_MESH_TTL_DEFAULT,
		.net_idx = bt_mesh_shell_target_ctx.net_idx,
		.addr = bt_mesh_shell_target_ctx.dst,
		.app_idx = bt_mesh_shell_target_ctx.app_idx,
	};
	int err = 0;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	if (argc == 2) {
		ctx.addr = wm_atoi(argv[1], &err);
		if (err) {
			printf("Unable to parse input string argument\n");
			return ;
		}
		printf("Cancelling DFU for 0x%04x\n", ctx.addr);
	} else {
		printf("Cancelling DFU\n");
	}

	err = bt_mesh_dfu_cli_cancel((struct bt_mesh_dfu_cli *)mod_cli->user_data,
				     (argc == 2) ? &ctx : NULL);
	if (err) {
		printf("Failed (err: %d)\n", err);
	}
}

static void cmd_dfu_apply(int argc, char *argv[])
{
	int err;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	printf("Applying DFU\n");

	err = bt_mesh_dfu_cli_apply((struct bt_mesh_dfu_cli *)mod_cli->user_data);
	if (err) {
		printf("Failed (err: %d)\n", err);
	}
}

static void cmd_dfu_confirm(int argc, char *argv[])
{
	int err;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	printf("Confirming DFU\n");

	err = bt_mesh_dfu_cli_confirm((struct bt_mesh_dfu_cli *)mod_cli->user_data);
	if (err) {
		printf("Failed (err: %d)\n", err);
	}
}

static void cmd_dfu_suspend(int argc, char *argv[])
{
	int err;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	printf("Suspending DFU\n");

	err = bt_mesh_dfu_cli_suspend((struct bt_mesh_dfu_cli *)mod_cli->user_data);
	if (err) {
		printf("Failed (err: %d)\n", err);
	}

}

static void cmd_dfu_resume(int argc, char *argv[])
{
	int err;

	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}

	printf("Resuming DFU\n");

	err = bt_mesh_dfu_cli_resume((struct bt_mesh_dfu_cli *)mod_cli->user_data);
	if (err) {
		printf("Failed (err: %d)\n", err);
	}
}

static void cmd_dfu_tx_progress(int argc, char *argv[])
{
	if (!mod_cli && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		return ;
	}
	printf("DFU progress: %u %%\n",bt_mesh_dfu_cli_progress((struct bt_mesh_dfu_cli *)mod_cli->user_data));
}

#endif /* CONFIG_BT_MESH_SHELL_DFU_CLI */

#if defined(CONFIG_BT_MESH_SHELL_DFU_SRV)

static struct bt_mesh_model *mod_srv;
static void cmd_dfu_applied(int argc, char *argv[])
{
	if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV, &mod_srv)) {
		return ;
	}
	bt_mesh_dfu_srv_applied((struct bt_mesh_dfu_srv *)mod_srv->user_data);
}

static void cmd_dfu_rx_cancel(int argc, char *argv[])
{
	if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV, &mod_srv)) {
		return ;
	}
	bt_mesh_dfu_srv_cancel((struct bt_mesh_dfu_srv *)mod_srv->user_data);
}

static void cmd_dfu_rx_progress(int argc, char *argv[])
{
	if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV, &mod_srv)) {
		return ;
	}
	printf("DFU progress: %u %%\n",bt_mesh_dfu_srv_progress((struct bt_mesh_dfu_srv *)mod_srv->user_data));
}

#endif /* CONFIG_BT_MESH_SHELL_DFU_SRV */


#if defined(CONFIG_BT_MESH_SHELL_DFU_METADATA)
    WM_CLI_CMD_DEFINE(mesh_models_dfu_metadata_comp_clear, cmd_dfu_comp_clear, clear dfu comp data, mesh_models_dfu_metadata_comp_clear -- mesh_models_dfu_metadata_comp_clear cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_metadata_comp_add, cmd_dfu_comp_add, add dfu comp data, mesh_models_dfu_metadata_comp_add <CID> <ProductID> <VendorID> <Crpl> <Features> -- mesh_models_dfu_metadata_comp_add cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_metadata_comp_elem_add, cmd_dfu_comp_elem_add, add dfu comp element data, mesh_models_dfu_metadata_comp_elem_add <Loc> <NumS> <NumV> {<SigMID>|<VndCID> <VndMID>}... -- mesh_models_dfu_metadata_comp_elem_add cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_metadata_comp_hash_get, cmd_dfu_comp_hash_get, display dfu image hash of comp data, mesh_models_dfu_metadata_comp_hash_get [<Key>] -- mesh_models_dfu_metadata_comp_hash_get cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_metadata_encode, cmd_dfu_metadata_encode, encode image hash of comp data, mesh_models_dfu_metadata_encode <Major> <Minor> <Rev> <BuildNum> <Size> <CoreType> <Hash> <Elems> [<UserData>] -- mesh_models_dfu_metadata_encode cmd);
#endif

#if defined(CONFIG_BT_MESH_SHELL_DFU_SLOT)
    WM_CLI_CMD_DEFINE(mesh_models_dfu_slot_add, cmd_dfu_slot_add, add virtual dfu image slot, mesh_models_dfu_slot_add <Size> <FwID> [<Metadata>] -- mesh_models_dfu_slot_add cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_slot_del, cmd_dfu_slot_del, remove a virtual dfu image slot, mesh_models_dfu_slot_del <SlotIdx> -- mesh_models_dfu_slot_del cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_slot_del_all, cmd_dfu_slot_del_all, remove all virtual dfu image slot, mesh_models_dfu_slot_del_all -- mesh_models_dfu_slot_del_all cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_slot_get, cmd_dfu_slot_get, retrieve a virtual dfu image slot, mesh_models_dfu_slot_get <SlotIdx> -- mesh_models_dfu_slot_get cmd);
#endif

#if defined(CONFIG_BT_MESH_SHELL_DFU_CLI)
    BT_MESH_SHELL_MDL_INSTANCE_CMDS(cli_instance_cmds, BT_MESH_MODEL_ID_DFU_CLI, mod_cli);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_target, cmd_dfu_target, add dfu client target address, mesh_models_dfu_cli_target <Addr> <ImgIdx> -- mesh_models_dfu_cli_target cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_target_reset, cmd_dfu_targets_reset, reset dfu client targets, mesh_models_dfu_cli_target_reset  -- mesh_models_dfu_cli_target_reset cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_target_stat, cmd_dfu_target_state, stat dfu client targets, mesh_models_dfu_cli_target_stat  -- mesh_models_dfu_cli_target_stat cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_target_imgs, cmd_dfu_target_imgs, rquest dfu images, mesh_models_dfu_cli_target_imgs [<MaxCount>] -- mesh_models_dfu_cli_target_imgs cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_target_check, cmd_dfu_target_check, check dfu image, mesh_models_dfu_cli_target_check <SlotIdx> <TargetImgIdx> -- mesh_models_dfu_cli_target_check cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_send, cmd_dfu_send, send dfu image, mesh_models_dfu_cli_send <SlotIdx>  [<Group> [<Mode(push, pull)> [<BlockSizeLog> <ChunkSize>]]] -- mesh_models_dfu_cli_send cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_cancel, cmd_dfu_tx_cancel, cancel tx dfu image, mesh_models_dfu_cli_cancel [<Addr>] -- mesh_models_dfu_cli_cancel cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_apply, cmd_dfu_apply, apply tx dfu image, mesh_models_dfu_cli_apply -- mesh_models_dfu_cli_apply cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_confirm, cmd_dfu_confirm, confirm tx dfu image, mesh_models_dfu_cli_confirm -- mesh_models_dfu_cli_cancel cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_suspend, cmd_dfu_suspend, suspend tx dfu image, mesh_models_dfu_cli_suspend -- mesh_models_dfu_cli_suspend cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_resume, cmd_dfu_resume, resume tx dfu image, mesh_models_dfu_cli_resume -- mesh_models_dfu_cli_resume cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_progress, cmd_dfu_tx_progress, display tx dfu progress, mesh_models_dfu_cli_progress -- mesh_models_dfu_cli_progress cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_set, cmd_cli_instance_cmds_set, set dfu instance, mesh_models_dfu_cli_set <ElemIdx> -- mesh_models_dfu_cli_set cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_cli_get_all, cmd_cli_instance_cmds_get_all, get dfu instance, mesh_models_dfu_cli_get_all -- mesh_models_dfu_cli_get_all cmd);
#endif

#if defined(CONFIG_BT_MESH_SHELL_DFU_SRV)
    BT_MESH_SHELL_MDL_INSTANCE_CMDS(srv_instance_cmds, BT_MESH_MODEL_ID_DFU_SRV, mod_srv);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_srv_applied, cmd_dfu_applied, apply dfu server parameter, mesh_models_dfu_srv_applied  -- mesh_models_dfu_srv_applied cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_srv_rx_cancel, cmd_dfu_rx_cancel, cancel dfu server parameter, mesh_models_dfu_srv_rx_cancel  -- mesh_models_dfu_srv_rx_cancel cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_srv_progress, cmd_dfu_rx_progress, display dfu progress, mesh_models_dfu_srv_progress  -- mesh_models_dfu_srv_progress cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_srv_set, cmd_srv_instance_cmds_set, set dfu instance, mesh_models_dfu_srv_set <ElemIdx> -- mesh_models_dfu_srv_set cmd);
    WM_CLI_CMD_DEFINE(mesh_models_dfu_srv_get_all, cmd_srv_instance_cmds_get_all, get dfu instance, mesh_models_dfu_srv_get_all -- mesh_models_dfu_srv_get_all cmd);
#endif

