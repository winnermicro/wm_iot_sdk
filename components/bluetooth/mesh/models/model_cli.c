 /*
  * Copyright (c) 2017 Intel Corporation
  *
  * SPDX-License-Identifier: Apache-2.0
  */


#include <kernel.h>
#include <stdbool.h>
#include <errno.h>

#include <net/buf.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/model_cli.h>
#include <bluetooth/mesh/model_util.h>

#define LOG_LEVEL CONFIG_BT_MESH_ACCESS_LOG_LEVEL
#define LOG_TAG "mesh_model"
#include <logging/log.h>

static struct bt_mesh_gen_model_cli *gen_onoff_cli;
static struct bt_mesh_gen_model_cli *gen_level_cli;

static uint8_t transaction_id = 0;

struct gen_onoff_param {
    uint8_t *state;
};

struct gen_level_param {
    int16_t *level;
};

static int gen_onoff_status(struct bt_mesh_model *model,
			     struct bt_mesh_msg_ctx *ctx,
			     struct net_buf_simple *buf)
{
	struct bt_mesh_gen_model_cli *cli = model->user_data;
	struct gen_onoff_param *param;
	uint8_t state;

	LOG_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

    state = net_buf_simple_pull_u8(buf);

    if(cli->notify) {
        cli->notify(model, (bool)state);
    }

	if (cli->op_pending != OP_GEN_ONOFF_STATUS) {
		LOG_WRN("Unexpected Generic OnOff Status message");
		//latency response message, 3s;
		//return -ENOENT;
	}

	param = cli->op_param;

	if (param && param->state) {
		*param->state = state;
	}

	LOG_DBG("state: %d", state);

	k_sem_give(&cli->op_sync);

	return 0;
}

static int gen_level_status(struct bt_mesh_model *model,
			     struct bt_mesh_msg_ctx *ctx,
			     struct net_buf_simple *buf)
{
	struct bt_mesh_gen_model_cli *cli = model->user_data;
	struct gen_level_param *param;
	int16_t level;

	LOG_DBG("net_idx 0x%04x app_idx 0x%04x src 0x%04x len %u: %s",
	       ctx->net_idx, ctx->app_idx, ctx->addr, buf->len,
	       bt_hex(buf->data, buf->len));

	if (cli->op_pending != OP_GEN_LEVEL_STATUS) {
		LOG_WRN("Unexpected Generic LEVEL Status message");
		return -EINVAL;
	}

	param = cli->op_param;

	level = net_buf_simple_pull_le16(buf);
	if (param->level) {
		*param->level = level;
	}

	LOG_DBG("level: %d", level);

	k_sem_give(&cli->op_sync);

	return 0;
}

const struct bt_mesh_model_op gen_onoff_cli_op[] = {
	{ OP_GEN_ONOFF_STATUS, 1, gen_onoff_status },
	BT_MESH_MODEL_OP_END,
};

static int onoff_cli_init(struct bt_mesh_model *model)
{
	LOG_DBG("");

	if (!model->user_data) {
		LOG_ERR("No Generic OnOff Client context provided");
		return -EINVAL;
	}

	gen_onoff_cli = model->user_data;
	gen_onoff_cli->model = model;
	k_sem_init(&gen_onoff_cli->op_sync, 0, 1);

	return 0;
}

const struct bt_mesh_model_cb bt_mesh_gen_onoff_cli_cb = {
	.init = onoff_cli_init,
};

const struct bt_mesh_model_op gen_level_cli_op[] = {
	{ OP_GEN_LEVEL_STATUS, 2, gen_level_status },
	BT_MESH_MODEL_OP_END,
};

static int level_cli_init(struct bt_mesh_model *model)
{
	LOG_DBG("");

	if (!model->user_data) {
		LOG_ERR("No Generic Level Client context provided");
		return -EINVAL;
	}

	gen_level_cli = model->user_data;
	gen_level_cli->model = model;

	k_sem_init(&gen_level_cli->op_sync, 0, 1);

	return 0;
}

const struct bt_mesh_model_cb bt_mesh_gen_level_cli_cb = {
	.init = level_cli_init,
};

static int cli_wait(struct bt_mesh_gen_model_cli *cli, void *param, uint32_t op)
{
	int err;

	LOG_DBG("");

	cli->op_param = param;
	cli->op_pending = op;

	err = k_sem_take(&cli->op_sync, K_SECONDS(5));

	cli->op_pending = 0;
	cli->op_param = NULL;

	return err;
}

int bt_mesh_gen_onoff_get(uint16_t net_idx, uint16_t addr, uint16_t app_idx,
			  uint8_t *state)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GEN_ONOFF_GET, 4);
	bt_mesh_model_msg_init(&buf, OP_GEN_ONOFF_GET);

	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = app_idx,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	struct gen_onoff_param param = {
		.state = state,
	};
	int err;

	err = bt_mesh_model_send(gen_onoff_cli->model, &ctx, &buf, NULL, NULL);
	if (err) {
		LOG_ERR("model_send() failed (err %d)", err);
		goto done;
	}

	err = cli_wait(gen_onoff_cli, &param, OP_GEN_ONOFF_STATUS);
done:
	return err;
}

int bt_mesh_gen_onoff_set(uint16_t net_idx, uint16_t addr, uint16_t app_idx,
			  uint8_t val, uint8_t *state)
{
    BT_MESH_MODEL_BUF_DEFINE(buf, OP_GEN_ONOFF_SET, 6);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = app_idx,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	struct gen_onoff_param param = {
		.state = state,
	};
	int err;

	if (state) {
		bt_mesh_model_msg_init(&buf, OP_GEN_ONOFF_SET);
	} else {
		bt_mesh_model_msg_init(&buf, OP_GEN_ONOFF_SET_UNACK);
	}

	net_buf_simple_add_u8(&buf, val);
	net_buf_simple_add_u8(&buf, transaction_id);

	err = bt_mesh_model_send(gen_onoff_cli->model, &ctx, &buf, NULL, NULL);
	if (err) {
		LOG_ERR("model_send() failed (err %d)", err);
		goto done;
	}

	if (!state) {
		goto done;
	}

	err = cli_wait(gen_onoff_cli, &param, OP_GEN_ONOFF_STATUS);
done:
	if (err == 0) {
		transaction_id++;
	}
	return err;
}

int bt_mesh_gen_level_get(uint16_t net_idx, uint16_t addr, uint16_t app_idx,
			  int16_t *level)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GEN_LEVEL_GET, 4);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = app_idx,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	struct gen_level_param param = {
		.level = level,
	};
	int err;

	bt_mesh_model_msg_init(&buf, OP_GEN_LEVEL_GET);

	err = bt_mesh_model_send(gen_level_cli->model, &ctx, &buf, NULL, NULL);
	if (err) {
		LOG_ERR("model_send() failed (err %d)", err);
		goto done;
	}

	err = cli_wait(gen_level_cli, &param, OP_GEN_LEVEL_STATUS);
done:
	return err;
}

int bt_mesh_gen_level_set(uint16_t net_idx, uint16_t addr, uint16_t app_idx,
			  int16_t val, int16_t *state)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GEN_LEVEL_SET, 7);
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = net_idx,
		.app_idx = app_idx,
		.addr = addr,
		.send_ttl = BT_MESH_TTL_DEFAULT,
	};
	struct gen_level_param param = {
		.level = state,
	};
	int err;

	if (state) {
		bt_mesh_model_msg_init(&buf, OP_GEN_LEVEL_SET);
	} else {
		bt_mesh_model_msg_init(&buf, OP_GEN_LEVEL_SET_UNACK);
	}

	net_buf_simple_add_le16(&buf, val);
	net_buf_simple_add_u8(&buf, transaction_id);

	err = bt_mesh_model_send(gen_level_cli->model, &ctx, &buf, NULL, NULL);
	if (err) {
		LOG_ERR("model_send() failed (err %d)", err);
		goto done;
	}

	if (!state) {
		goto done;
	}

	err = cli_wait(gen_level_cli, &param, OP_GEN_LEVEL_STATUS);
done:
	if (err == 0) {
		transaction_id++;
	}
	return err;
}
