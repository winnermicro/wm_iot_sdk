/* main.c - Application main entry point */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <settings/settings.h>
#include <sys/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

#include <bluetooth/mesh/model_cli.h>
#include <bluetooth/mesh/model_srv.h>
#include <bluetooth/mesh/model_util.h>

#include "wm_cli.h"
#include "wm_utils.h"
#include "wm_drv_gpio.h"

#define LOG_TAG "mesh_onoff"
#include <logging/log.h>

#define WM_MESH_LED_FLAG_BIT_OFF   0x00
#define WM_MESH_LED_FLAG_BIT_GREEN 0x01
#define WM_MESH_LED_FLAG_BIT_RED   0x02
#define WM_MESH_LED_FLAG_BIT_BLUE  0x04

static uint16_t first_elem_addr_within_node = 0;
static bool attention_is_on                 = false;

static void mesh_light_update(uint8_t flag);
static int gen_onoff_cli_pub_update(struct bt_mesh_model *mod);
static int gen_onoff_srv_pub_update(struct bt_mesh_model *mod);

int model_gen_onoff_get(struct bt_mesh_model *model, bool *state);
int model_gen_onoff_set(struct bt_mesh_model *model, bool state);
int model_gen_onoff_cli_state_notify(struct bt_mesh_model *model, bool state);

static void attention_on(struct bt_mesh_model *mod)
{
    wm_log_info("attention_on");
    attention_is_on = true;
}

static void attention_off(struct bt_mesh_model *mod)
{
    wm_log_info("attention_off");
    attention_is_on = false;
}

static const struct bt_mesh_health_srv_cb health_cb = {
    .attn_on  = attention_on,
    .attn_off = attention_off,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_cb,
};

static struct bt_mesh_gen_model_cli gen_onoff_cli = {
    .notify = model_gen_onoff_cli_state_notify,
};

BT_MESH_MODEL_PUB_DEFINE(gen_onoff_cli_pub, gen_onoff_cli_pub_update, 4);

static struct bt_mesh_gen_onoff_srv gen_onoff_srv = {
    .get = model_gen_onoff_get,
    .set = model_gen_onoff_set,
};
BT_MESH_MODEL_PUB_DEFINE(gen_onoff_srv_pub, gen_onoff_srv_pub_update, 2 + 2);

BT_MESH_HEALTH_PUB_DEFINE(health_pub, 0);

static struct bt_mesh_model node_primary_models[] = { BT_MESH_MODEL_CFG_SRV, BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
                                                      BT_MESH_MODEL_GEN_ONOFF_SRV(&gen_onoff_srv, &gen_onoff_srv_pub) };

static struct bt_mesh_model node_second_models[] = { BT_MESH_MODEL_GEN_ONOFF_CLI(&gen_onoff_cli, &gen_onoff_cli_pub) };

static struct bt_mesh_elem node_elements[] = {
    BT_MESH_ELEM(0, node_primary_models, BT_MESH_MODEL_NONE),
    BT_MESH_ELEM(1, node_second_models, BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
    .cid        = BT_COMP_ID_LF,
    .elem       = node_elements,
    .elem_count = ARRAY_SIZE(node_elements),
};

static struct k_work_delayable prov_work;

static void prov_work_timeout(struct k_work *work)
{
    (void)work;
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
}

/* Provisioning */

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
    wm_log_info("OOB Number: %u", number);
    return 0;
}

static void prov_complete(uint16_t net_idx, uint16_t addr)
{
    wm_log_info("prov_complete: addr 0x%04x", addr);
    first_elem_addr_within_node = addr;
}

static void prov_reset(void)
{
    first_elem_addr_within_node = 0;
    k_work_reschedule(&prov_work, K_MSEC(3000));
}

static void prov_input_num(int argc, char *argv[])
{
    int err = 0;
    uint32_t val;

    val = wm_atoi(argv[1], &err);
    if (err) {
        wm_log_error("Unable to parse input string argument");
        return;
    }

    err = bt_mesh_input_number(val);
    if (err) {
        wm_log_error("Numeric input failed (err %d)", err);
    }
}

static void prov_input_str(int argc, char *argv[])
{
    int err = bt_mesh_input_string(argv[1]);

    if (err) {
        wm_log_error("String input failed (err %d)", err);
    }
}

static uint8_t dev_uuid[16] = { 0x12, 0x34 };

static const struct bt_mesh_prov prov = {
    .uuid           = dev_uuid,
    .output_size    = 4,
    .output_actions = BT_MESH_DISPLAY_NUMBER,
    .output_number  = output_number,
    .complete       = prov_complete,
    .reset          = prov_reset,
};

static uint8_t transaction_id = 0;

static int gen_onoff_cli_pub_update(struct bt_mesh_model *mod)
{
    static bool general_onoff_state = false;
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET       BT_MESH_MODEL_OP_2(0x82, 0x02)

    wm_log_info("gen off client publish update to 0x%04x onoff 0x%04x", gen_onoff_cli_pub.addr, general_onoff_state);

    bt_mesh_model_msg_init(gen_onoff_cli_pub.msg, BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);
    net_buf_simple_add_u8(gen_onoff_cli_pub.msg, general_onoff_state);
    net_buf_simple_add_u8(gen_onoff_cli_pub.msg, transaction_id++);
    {
        general_onoff_state = !general_onoff_state;
    }

    return 0;
}

static bool onoff_value              = false;
static uint32_t server_onoff_counter = 0;
static uint32_t onoff_time_ms        = 0;
int model_gen_onoff_get(struct bt_mesh_model *model, bool *state)
{
    *state = onoff_value;
    return 0;
}
int model_gen_onoff_set(struct bt_mesh_model *model, bool state)
{
    server_onoff_counter++;

    wm_log_info("onoff set state %u [%d]", state, server_onoff_counter);
    onoff_value   = state;
    onoff_time_ms = k_uptime_get();

    if (onoff_value) {
        mesh_light_update(WM_MESH_LED_FLAG_BIT_RED | WM_MESH_LED_FLAG_BIT_BLUE | WM_MESH_LED_FLAG_BIT_GREEN);

    } else {
        mesh_light_update(WM_MESH_LED_FLAG_BIT_OFF);
    }
    return 0;
}

static int gen_onoff_srv_pub_update(struct bt_mesh_model *mod)
{
    int err;
    struct net_buf_simple *msg = mod->pub->msg;

    bt_mesh_model_msg_init(msg, OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, onoff_value);
    err = bt_mesh_model_publish(mod);
    if (err) {
        wm_log_error("bt_mesh_model_publish err %d", err);
    }

    return 0;
}

int model_gen_onoff_cli_state_notify(struct bt_mesh_model *model, bool state)
{
    wm_log_debug("onoff client recv state %u", state);
    return 0;
}

static uint32_t mesh_ctrl_counter      = 0;
static uint32_t mesh_ctrl_err_counter  = 0;
static uint32_t mesh_ctrl_max_delay_ms = 0;
static void mesh_client_ctrl(int argc, char *argv[])
{
    int err = 0;
    uint32_t dest;
    uint32_t val;
    uint32_t ack_need  = 0;
    uint32_t ack_delay = 0;
    uint8_t state;

    dest = wm_atoi(argv[1], &err);
    if (err) {
        wm_log_error("invalid dest address");
        return;
    }

    val = wm_atoi(argv[2], &err);
    if (err) {
        wm_log_error("invalid on off value");
        return;
    }

    ack_need = wm_atoi(argv[3], &err);
    if (err) {
        wm_log_error("invalid on off value");
        return;
    }

    mesh_ctrl_counter++;
    wm_log_info("onoff ctrl dest %d val %d", dest, val);
    ack_delay = k_uptime_get();

    if (ack_need) {
        err = bt_mesh_gen_onoff_set(0, dest, 0, val, &state);
    } else {
        err = bt_mesh_gen_onoff_set(0, dest, 0, val, NULL);
    }

    if (err) {
        wm_log_error("Failed to send Generic OnOff Set (err %d)\r\n", err);
        mesh_ctrl_err_counter++;
    } else {
        if (ack_need) {
            ack_delay = k_uptime_delta(&ack_delay);
            if (ack_delay > mesh_ctrl_max_delay_ms) {
                mesh_ctrl_max_delay_ms = ack_delay;
            }
            wm_log_info("onoff state %d [%d,%d] max_delay %d ms\r\n", state, mesh_ctrl_counter, mesh_ctrl_err_counter,
                        mesh_ctrl_max_delay_ms);
        } else {
            wm_log_info("onoff state %d [%d,%d]\r\n", val, mesh_ctrl_counter, mesh_ctrl_err_counter);
        }
    }
}

static void bt_ready(int err)
{
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
        return;
    }

    wm_log_info("Bluetooth initialized");
    err = bt_mesh_init(&prov, &comp);
    if (err) {
        wm_log_error("Initializing mesh failed (err %d)", err);
        return;
    }

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    /* This will be a no-op if settings_load() loaded provisioning info */
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

    wm_log_info("Mesh initialized");
}

#define MESH_LIGHT_GPIO_GREEN WM_GPIO_NUM_16
#define MESH_LIGHT_GPIO_RED   WM_GPIO_NUM_17
#define MESH_LIGHT_GPIO_BLUE  WM_GPIO_NUM_18

static void mesh_light_init(void)
{
    int err;

    wm_drv_gpio_init("gpio");

    if (0 != (err = wm_drv_gpio_iomux_func_sel(MESH_LIGHT_GPIO_GREEN, WM_GPIO_IOMUX_FUN5)) ||
        0 != (err = wm_drv_gpio_set_dir(MESH_LIGHT_GPIO_GREEN, WM_GPIO_DIR_OUTPUT)) ||
        0 != (err = wm_drv_gpio_data_set(MESH_LIGHT_GPIO_GREEN))) {
        wm_log_error("MESH_LIGHT_GPIO_GREEN cfg err %d", err);
        return;
    }

    if (0 != (err = wm_drv_gpio_iomux_func_sel(MESH_LIGHT_GPIO_RED, WM_GPIO_IOMUX_FUN5)) ||
        0 != (err = wm_drv_gpio_set_dir(MESH_LIGHT_GPIO_RED, WM_GPIO_DIR_OUTPUT)) ||
        0 != (err = wm_drv_gpio_data_set(MESH_LIGHT_GPIO_RED))) {
        wm_log_error("MESH_LIGHT_GPIO_RED cfg err %d", err);
        return;
    }

    if (0 != (err = wm_drv_gpio_iomux_func_sel(MESH_LIGHT_GPIO_BLUE, WM_GPIO_IOMUX_FUN5)) ||
        0 != (err = wm_drv_gpio_set_dir(MESH_LIGHT_GPIO_BLUE, WM_GPIO_DIR_OUTPUT)) ||
        0 != (err = wm_drv_gpio_data_set(MESH_LIGHT_GPIO_BLUE))) {
        wm_log_error("MESH_LIGHT_GPIO_BLUE cfg err %d", err);
        return;
    }
}

void mesh_light_update(uint8_t flag)
{
    if (flag & WM_MESH_LED_FLAG_BIT_GREEN) {
        wm_drv_gpio_data_reset(MESH_LIGHT_GPIO_GREEN);
    } else {
        wm_drv_gpio_data_set(MESH_LIGHT_GPIO_GREEN);
    }

    if (flag & WM_MESH_LED_FLAG_BIT_RED) {
        wm_drv_gpio_data_reset(MESH_LIGHT_GPIO_RED);
    } else {
        wm_drv_gpio_data_set(MESH_LIGHT_GPIO_RED);
    }

    if (flag & WM_MESH_LED_FLAG_BIT_BLUE) {
        wm_drv_gpio_data_reset(MESH_LIGHT_GPIO_BLUE);
    } else {
        wm_drv_gpio_data_set(MESH_LIGHT_GPIO_BLUE);
    }
}

static uint32_t time_elapsed(uint32_t last_on_ms)
{
    uint32_t diff_ms    = 0;
    uint32_t curr_ticks = k_uptime_get();

    if (curr_ticks >= last_on_ms) {
        diff_ms = curr_ticks - last_on_ms;
    } else {
        diff_ms = curr_ticks + (0xFFFFFFFF - last_on_ms);
    }

    return diff_ms;
}

int main(void)
{
    int err = -1;

    wm_log_info("Mesh node demo running");

    if (wm_sys_get_mac_addr(WM_MAC_TYPE_BT, &dev_uuid[10], BT_ADDR_SIZE) != WM_ERR_SUCCESS) {
        wm_log_error("Get mac addr fail");
    }

    mesh_light_init();

    k_work_init_delayable(&prov_work, prov_work_timeout);

    /* Initialize the Bluetooth Subsystem */
    err = bt_enable(bt_ready);
    if (err) {
        wm_log_error("Bluetooth init failed (err %d)", err);
    }

    while (1) {
        if (attention_is_on) {
            mesh_light_update(WM_MESH_LED_FLAG_BIT_GREEN);
            k_sleep(K_MSEC(200));
            mesh_light_update(WM_MESH_LED_FLAG_BIT_OFF);
            k_sleep(K_MSEC(200));
        }

        /*blue led blink indicates system is running when mesh light is idle*/
        if (!onoff_value && (time_elapsed(onoff_time_ms) > 10000) && first_elem_addr_within_node) {
            mesh_light_update(WM_MESH_LED_FLAG_BIT_BLUE);
            k_sleep(K_MSEC(1000));
            if (!onoff_value) {
                mesh_light_update(WM_MESH_LED_FLAG_BIT_OFF);
            }
            k_sleep(K_MSEC(1000));
        }

        /*blue led blink quickly indicates this node is waiting for provision*/
        if (first_elem_addr_within_node == 0 && !attention_is_on) {
            mesh_light_update(WM_MESH_LED_FLAG_BIT_BLUE);
            k_sleep(K_MSEC(500));
            mesh_light_update(WM_MESH_LED_FLAG_BIT_OFF);
            k_sleep(K_MSEC(500));
        }
    };

    return 0;
}

WM_CLI_CMD_DEFINE(mesh_prov_input_num, prov_input_num, input number for provision auth, mesh_prov_input_num <Number> --mesh_prov_input_num cmd);
WM_CLI_CMD_DEFINE(mesh_prov_input_str, prov_input_str, input string for provision auth, mesh_prov_input_str <String> --mesh_prov_input_str cmd);
WM_CLI_CMD_DEFINE(mesh_ctrl, mesh_client_ctrl, send switch cmd to a node, mesh_ctrl<dest><value><ack> --mesh_ctrl cmd);
