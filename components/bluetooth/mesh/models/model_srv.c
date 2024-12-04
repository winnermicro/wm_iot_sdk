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
#include <bluetooth/mesh/model_srv.h>
#include <bluetooth/mesh/model_util.h>


#define LOG_LEVEL CONFIG_BT_MESH_ACCESS_LOG_LEVEL
#define LOG_TAG "mesh_model"
#include <logging/log.h>

static struct bt_mesh_gen_onoff_srv *gen_onoff_srv;
static struct bt_mesh_gen_level_srv *gen_level_srv;
static struct bt_mesh_light_lightness_srv *light_lightness_srv;

static onoff_t onoff;

static int gen_onoff_status_send(struct bt_mesh_model *model,
			     struct bt_mesh_msg_ctx *ctx)
{
	int err;
	uint32_t remaining;

	struct bt_mesh_gen_onoff_srv *cb = model->user_data;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GEN_ONOFF_STATUS, 3);
	bt_mesh_model_msg_init(&buf, OP_GEN_ONOFF_STATUS);

	remaining = k_ticks_to_ms_floor32(
			    k_work_delayable_remaining_get(&onoff.work)) +
		    onoff.transition_time;

	if (cb && cb->get) {
		cb->get(model, &onoff.val);
	}

	/* Check using remaining time instead of "work pending" to make the
	 * onoff status send the right value on instant transitions. As the
	 * work item is executed in a lower priority than the mesh message
	 * handler, the work will be pending even on instant transitions.
	 */
	if (remaining) {
		net_buf_simple_add_u8(&buf, !onoff.val);
		net_buf_simple_add_u8(&buf, onoff.val);
		net_buf_simple_add_u8(&buf, model_time_encode(remaining));
	} else {
		net_buf_simple_add_u8(&buf, onoff.val);
	}

	err = bt_mesh_model_send(model, ctx, &buf, NULL, NULL);
	if (err) {
		LOG_ERR("Send status failed");
	}

	return err;
}

 static void gen_onoff_timeout(struct k_work *work)
 {

    if (onoff.transition_time) {
        /* Start transition.
        *
        * The LED should be on as long as the transition is in
        * progress, regardless of the target value, according to the
        * Bluetooth Mesh Model specification, section 3.1.1.
        */
        k_work_reschedule(&onoff.work, K_MSEC(onoff.transition_time));
        onoff.transition_time = 0;
        return;
    }
 }

static int gen_onoff_get(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	LOG_DBG("");

	return gen_onoff_status_send(model, ctx);
}

static int gen_onoff_set_unack(struct bt_mesh_model *model,
				struct bt_mesh_msg_ctx *ctx,
				struct net_buf_simple *buf)
{
	struct net_buf_simple *msg = model->pub->msg;
	struct bt_mesh_gen_onoff_srv *cb = model->user_data;
	uint8_t val = net_buf_simple_pull_u8(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);
	int32_t trans = 0;
	int32_t delay = 0;
    int err;

	if (buf->len) {
		trans = model_time_decode(net_buf_simple_pull_u8(buf));
		delay = net_buf_simple_pull_u8(buf) * 5;
	}

	/* Only perform change if the message wasn't a duplicate and the
	 * value is different.
	 */
	if (tid == onoff.tid && ctx->addr == onoff.src) {
		/* Duplicate */
		return 0;
	}

	if (val == onoff.val) {
		/* No change */
		return 0;
	}

	if (cb && cb->set) {
		cb->set(model, (bool)val);
	}

	onoff.tid = tid;
	onoff.src = ctx->addr;
	onoff.val = val;
	onoff.transition_time = trans;

	/* Schedule the next action to happen on the delay, and keep
	 * transition time stored, so it can be applied in the timeout.
	 */
	k_work_reschedule(&onoff.work, K_MSEC(delay));

    if(model->pub && model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        bt_mesh_model_msg_init(msg,OP_GEN_ONOFF_STATUS);
        net_buf_simple_add_u8(msg, val);
        err = bt_mesh_model_publish(model);
        if (err) {
            LOG_WRN("bt_mesh_model_publish err %d\n", err);
        }
    }


	return 0;

}

static int gen_onoff_set(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	LOG_DBG("");

	gen_onoff_set_unack(model, ctx, buf);
	return gen_onoff_status_send(model, ctx);
}

static int gen_level_status(struct bt_mesh_model *model,
			     struct bt_mesh_msg_ctx *ctx)
{
	int err;
	struct bt_mesh_gen_level_srv *cb = model->user_data;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GEN_LEVEL_STATUS, 4);
	bt_mesh_model_msg_init(&buf, OP_GEN_LEVEL_STATUS);

	int16_t level;

	if (cb && cb->get) {
		cb->get(model, &level);
	}
    net_buf_simple_add_le16(&buf, level);

	LOG_DBG("level: %d", level);

	err = bt_mesh_model_send(model, ctx, &buf, NULL, NULL);
	if (err) {
		LOG_ERR("Send status failed");
	}

	return err;
}

static int gen_level_get(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	LOG_DBG("");

	return gen_level_status(model, ctx);
}

static int gen_level_set_unack(struct bt_mesh_model *model,
				struct bt_mesh_msg_ctx *ctx,
				struct net_buf_simple *buf) {
	struct bt_mesh_gen_level_srv *cb = model->user_data;
	int16_t level;

	level = (int16_t) net_buf_simple_pull_le16(buf);
	LOG_DBG("level: %d", level);

	if (cb && cb->set) {
		return cb->set(model, level);
	}

	return 0;
}

static int gen_level_set(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	gen_level_set_unack(model, ctx, buf);
	return gen_level_status(model, ctx);
}

static int light_lightness_status(struct bt_mesh_model *model,
			     struct bt_mesh_msg_ctx *ctx)
{
	int err;
	struct bt_mesh_light_lightness_srv *cb = model->user_data;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_STATUS, 4);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_STATUS);

	int16_t lightness;

	if (cb && cb->get) {
		cb->get(model, &lightness);
	}

	LOG_DBG("lightness: %d", lightness);

    net_buf_simple_add_le16(&buf, lightness);

	err = bt_mesh_model_send(model, ctx, &buf, NULL, NULL);
	if (err) {
		LOG_ERR("Send status failed");
	}

	return err;
}

static int light_lightness_get(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	LOG_DBG("");

	return light_lightness_status(model, ctx);
}

static int light_lightness_set_unack(struct bt_mesh_model *model,
				struct bt_mesh_msg_ctx *ctx,
				struct net_buf_simple *buf) {
	struct bt_mesh_light_lightness_srv *cb = model->user_data;
	int16_t lightness;

	lightness = (int16_t) net_buf_simple_pull_le16(buf);
	LOG_DBG("lightness: %d", lightness);

	if (cb && cb->set) {
		return cb->set(model, lightness);
	}

	return 0;
}

static int light_lightness_set(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
	light_lightness_set_unack(model, ctx, buf);
	return light_lightness_status(model, ctx);
}

const struct bt_mesh_model_op gen_onoff_srv_op[] = {
	{ OP_GEN_ONOFF_GET, 		0, gen_onoff_get },
	{ OP_GEN_ONOFF_SET, 		2, gen_onoff_set },
	{ OP_GEN_ONOFF_SET_UNACK, 	2, gen_onoff_set_unack },
	BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_level_srv_op[] = {
	{ OP_GEN_LEVEL_GET, 		0, gen_level_get },
	{ OP_GEN_LEVEL_SET, 		3, gen_level_set },
	{ OP_GEN_LEVEL_SET_UNACK, 	3, gen_level_set_unack },
	BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op light_lightness_srv_op[] = {
	{ OP_LIGHT_LIGHTNESS_GET, 		0, light_lightness_get },
	{ OP_LIGHT_LIGHTNESS_SET, 		3, light_lightness_set },
	{ OP_LIGHT_LIGHTNESS_SET_UNACK, 	3, light_lightness_set_unack },
	BT_MESH_MODEL_OP_END,
};

static int onoff_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_gen_onoff_srv *cfg = model->user_data;

	LOG_DBG("");

	if (!cfg) {
		LOG_ERR("No Generic OnOff Server context provided");
		return -EINVAL;
	}

	cfg->model = model;
	gen_onoff_srv = cfg;
    memset(&onoff, 0, sizeof(onoff));
    k_work_init_delayable(&onoff.work, gen_onoff_timeout);

	return 0;
}

static int onoff_srv_deinit(struct bt_mesh_model *model)
{
	LOG_DBG("");

    memset(&onoff, 0, sizeof(onoff));
    k_work_deinit_delayable(&onoff.work);
    gen_onoff_srv = NULL;

    return 0;
}


const struct bt_mesh_model_cb gen_onoff_srv_cb = {
	.init = onoff_srv_init,
    .deinit = onoff_srv_deinit
};

static int level_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_gen_level_srv *cfg = model->user_data;

	LOG_DBG("");

	if (!cfg) {
		LOG_ERR("No Generic Level Server context provided");
		return -EINVAL;
	}

	cfg->model = model;
	gen_level_srv = cfg;

	return 0;
}

const struct bt_mesh_model_cb gen_level_srv_cb = {
	.init = level_srv_init,
};

static int lightness_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_light_lightness_srv *cfg = model->user_data;

	LOG_DBG("");

	if (!cfg) {
		LOG_ERR("No Light Lightness Server context provided");
		return -EINVAL;
	}

	cfg->model = model;
	light_lightness_srv = cfg;

	return 0;
}

const struct bt_mesh_model_cb light_lightness_srv_cb = {
	.init = lightness_srv_init,
};
