/*
 * Copyright (c) 2018 WinnerMicro Semiconductor ASA
 *
 */

/** @file
 *  @brief WinnerMicro WiFi provision over BLE sample
 */
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/byteorder.h>
#include <kernel.h>
#include <bluetooth/bluetooth.h>

#include "wps_svc.h"
#include "wm_event.h"
#include "wm_wifi.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"

#include "wm_wifi.h"
#include "wm_utils.h"

#define LOG_TAG "wifi_prov"
#include "wm_log.h"

#define ENC_FLAG_VALID_BIT      (0x01 << 7)
#define ACK_FLAG_VALID_BIT      (0x01 << 6)
#define MRE_FLAG_VALID_BIT      (0x01 << 5)

#define DEFAULT_SEND_FLAG       0x00

#define CONFIG_STA_CMD          0x0A
#define CONFIG_SOFTAP_CMD       0x0B
#define CONFIG_STA_SOFTAP_CMD   0x0C
#define CONFIG_INQUIRY_CMD      0x0D
#define CONFIG_WIFI_SCAN_CMD    0x0E
#define CONFIG_KEY_EXCHANGE_CMD 0x0F

#define ACK_TO_APP              0x90
#define ACK_FROM_APP            0x10
#define AES_BLOCK_SIZE          0x10

/*opcode, seq, flag, frag_seq*/
#define PROTO_HEADER_LENGTH     4

#define WPS_STACKSIZE           2048
#define WPS_PRIORITY            10
#define PROTO_BUF_SIZE          256
#define WIFI_JOIN_MAXIMUM_RETRY 4

//#define NVS_STORE_AFTER_WIFI_JOIN

typedef struct proto_data {
    uint8_t single[PROTO_BUF_SIZE];
    uint8_t data[PROTO_BUF_SIZE];
    uint16_t len;
    uint16_t out_len;
    uint8_t txrx; //0, IN; 1, OUT
} proto_data_t;

typedef struct proto_header {
    uint8_t opcode;
    uint8_t seq;
    uint8_t flag;
    uint8_t seqno;
    uint8_t data[];
} __attribute__((__packed__)) proto_data_header_t;

static struct k_queue proto_queue;
static struct k_sem proto_sem;
static struct k_thread wps_txrx_thread_data;
static uint8_t tx_seq = 0, rx_seq = 0;
static uint8_t proto_cmd_buffer[PROTO_BUF_SIZE];
static uint8_t proto_cmd_buffer_offset         = 0;
static void *p_ciphercontext_aes               = NULL;
static int s_retry_num                         = 0;
static struct bt_conn *local_conn              = NULL;
static bool s_already_connected_before_connect = false;

static struct k_work_delayable delay_worker;
static void delay_work_handler(struct k_work *work);
static void wps_send_error_status(uint8_t opcode, uint8_t error_code);
static void wps_send_response(uint8_t opcode, uint8_t flag, const uint8_t *payload, uint16_t payload_length);
extern void *bt_aes_ctx_init(const unsigned char *key, size_t len);
extern int bt_aes_encrypt(void *ctx, const uint8_t *src_ptr, int length, uint8_t *dest_ptr);
extern int bt_aes_decrypt(void *ctx, uint8_t *src_ptr, int length, uint8_t *dest_ptr);
extern int bt_rsa_encrypt(uint8_t *pub_key, int pubkey_size, uint8_t *src_ptr, int length, uint8_t *dest_ptr, int dest_len);

static void delay_work_handler(struct k_work *work)
{
    (void)work;

    if (local_conn) {
        bt_conn_disconnect(local_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
}

static void wps_handler_sta_disconnected(wm_event_group_t group, int event, void *data, void *priv)
{
    wm_wifi_event_data_t *evt_data = (wm_wifi_event_data_t *)data;

    if (event == WM_EVENT_WIFI_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_JOIN_MAXIMUM_RETRY) {
            if (s_already_connected_before_connect) {
                s_already_connected_before_connect = false;
            } else {
                wm_wifi_connect();
                s_retry_num++;
                wm_log_info("retry to connect to the AP (%d)", s_retry_num);
            }
        } else {
            wps_send_error_status(CONFIG_STA_CMD,
                                  evt_data->sta_disconnected_info.reason + 0x10); //mapping to protocol error code
        }
    }
}

static void wps_handler_sta_got_ip(wm_event_group_t group, int event, wm_lwip_event_data_t *data, void *priv)
{
    uint8_t resp[17] = { 0x00 };

    uint8_t resp_length = 0;

    if (event == WM_EVENT_WIFI_STA_GOT_IP) {
        char ip[16];
        ipaddr_ntoa_r((ip_addr_t *)&data->sta_got_ip_info.ip, ip, sizeof(ip));
        wm_log_info("got ip: %s", ip);

        resp[resp_length++] = 0x81; //type id;
        resp[resp_length++] = 0x01; //length;
        resp[resp_length++] = 0x00; //variable value;
        resp[resp_length++] = 0x82; //type id;
        resp[resp_length++] = 0x04; //length;
        memcpy(resp + resp_length, &data->sta_got_ip_info.ip, 4);
        resp_length += 4;
        resp[resp_length++] = 0x83; //type id;
        resp[resp_length++] = 0x06; //length;

        if (wm_sys_get_mac_addr(WM_MAC_TYPE_STA, resp + resp_length, WM_MAC_ADDR_LEN) != WM_ERR_SUCCESS) {
            wm_log_error("Get mac addr fail");
        }

        resp_length += WM_MAC_ADDR_LEN;

        wps_send_response(CONFIG_STA_CMD + 0x80, ENC_FLAG_VALID_BIT, resp, resp_length);
        k_work_schedule(&delay_worker, K_MSEC(600));
    }

    if (event == WM_EVENT_WIFI_STA_LOST_IP) {
        //wps_send_error_status(CONFIG_STA_CMD, 0x03);
    }
}

static int wps_wifi_join(wm_wifi_config_t *wifi_cfg)
{
    int err;
    s_retry_num = 0;
    wm_wifi_ap_record_t ap;

    wm_log_info("connecting to ap ssid:%s password:%s", wifi_cfg->sta.ssid, wifi_cfg->sta.password);

    err = wm_wifi_sta_get_ap_info(&ap);
    if (err == WM_ERR_SUCCESS) {
        s_already_connected_before_connect = true;
    } else {
        s_already_connected_before_connect = false;
    }

    err = 0;

#ifdef NVS_STORE_AFTER_WIFI_JOIN
    wm_wifi_change_storage_type(WM_WIFI_STORAGE_RAM);
#endif

    if (!err)
        err = wm_wifi_set_config(WM_WIFI_IF_STA, wifi_cfg);

    if (!err)
        err = wm_wifi_connect();

    if (!err) {
        wm_log_info("wifi_init_sta finished.");
    } else {
        wm_log_error("wifi_init_sta failed.");
    }

    return err;
}

static void wps_send_ack(uint8_t payload)
{
    proto_data_t *ack = NULL;

    ack = (proto_data_t *)malloc(sizeof(proto_data_t));
    if (ack == NULL) {
        wm_log_error("No free memory to alloc ack");
        return;
    }
    memset(ack, 0, sizeof(proto_data_t));
    ack->data[0] = ACK_TO_APP;
    ack->data[1] = 0x00; //tx_seq++;
    ack->data[2] = 0x00; //flag;
    ack->data[3] = 0x00; //fragment seq;
    ack->data[4] = payload;
    ack->len     = 1; //tx packet tag;

    /**No ack needed, No encryption need, Just send to app*/
    ack->txrx = 1;
    k_queue_append(&proto_queue, ack);
}

static void wps_send_response(uint8_t opcode, uint8_t flag, const uint8_t *payload, uint16_t payload_length)
{
    int err;
    proto_data_t *resp    = NULL;
    uint8_t *temp_payload = NULL;

    resp = (proto_data_t *)malloc(sizeof(proto_data_t));
    if (resp == NULL) {
        wm_log_error("No free memory to alloc response");
        return;
    }
    memset(resp, 0, sizeof(proto_data_t));

    if (flag & ENC_FLAG_VALID_BIT) {
        temp_payload = (uint8_t *)malloc(payload_length + AES_BLOCK_SIZE);
        if (temp_payload == NULL) {
            wm_log_error("No free memory to alloc aes enc payload");
            free(resp);
            return;
        }
        err            = bt_aes_encrypt(p_ciphercontext_aes, payload, payload_length, temp_payload);
        payload_length = err;
    } else {
        temp_payload = (uint8_t *)payload;
    }

    resp->data[0] = opcode;
    resp->data[1] = 0x00; //tx_seq++;
    resp->data[2] = flag; //flag;
    resp->data[3] = 0x00; //fragment seq;
    memcpy(&resp->data[4], temp_payload, payload_length);
    resp->len  = payload_length;
    resp->txrx = 1; //tx packet tag;

    err = k_queue_append(&proto_queue, resp);
    if (err != 0) {
        wm_log_error("put response to queue failed");
        free(resp);
    }

    if (temp_payload && temp_payload != payload) {
        free(temp_payload);
    }
}

static void wps_send_error_status(uint8_t opcode, uint8_t error_code)
{
    uint8_t resp[3];
    resp[0] = 0x81;
    resp[1] = 0x01;
    resp[2] = error_code;
    wps_send_response(opcode + 0x80, DEFAULT_SEND_FLAG, resp, sizeof(resp));
    wm_log_info("send app error code %d", error_code);
    k_work_schedule(&delay_worker, K_MSEC(1000));
}

static void key_exchange_process(const uint8_t *payload, int length)
{
#define TLV_TYPE_PUB_KEY 0x09
    int i = 0;
    uint8_t priv_key[16];
    uint8_t resp[133] = { 0x81, 0x01, 0x00, 0x87, 128, 0x00 };

    //payload format: 0x87, 0x10, public key;
    if (payload[0] != TLV_TYPE_PUB_KEY) {
        wm_log_error("%s,%d, payload format error", __FUNCTION__, __LINE__);
        return;
    }

    //generate aes private key
    for (i = 0; i < 16; i++) {
        priv_key[i] = rand() % 0xFF;
    }

    //exchange aes key with app
    if (bt_rsa_encrypt((uint8_t *)payload + 2, payload[1], priv_key, 16, resp + 5, 128)) {
        wps_send_error_status(CONFIG_KEY_EXCHANGE_CMD, 0x05);
        return;
    }

    //regenerate aes context, free it first if allocated already
    if (p_ciphercontext_aes) {
        free(p_ciphercontext_aes);
        p_ciphercontext_aes = NULL;
    }
    p_ciphercontext_aes = bt_aes_ctx_init(priv_key, sizeof(priv_key));
    if (p_ciphercontext_aes == NULL) {
        wm_log_error("bt aes ctx init failed");
        wps_send_error_status(CONFIG_KEY_EXCHANGE_CMD, 0x01);
        return;
    }

    wps_send_response(CONFIG_KEY_EXCHANGE_CMD + 0x80, DEFAULT_SEND_FLAG, resp, sizeof(resp));
}

static int wlan_config_process(const uint8_t *payload, int length)
{
#define AP_SSID     0x01
#define AP_PASSWORD 0x02
#define AP_BSSID    0x03

    int offset         = 0;
    uint8_t cmd_type   = payload[offset++];
    uint8_t cmd_length = payload[offset++];
    int wifi_join      = 0;
    wm_wifi_config_t wifi_cfg;

    memset(&wifi_cfg, 0, sizeof(wifi_cfg));
    while (offset < length) {
        switch (cmd_type) {
            case AP_SSID:
                if (cmd_length > sizeof(wifi_cfg.sta.ssid)) {
                    break;
                };
                memcpy(wifi_cfg.sta.ssid, &payload[offset], cmd_length);
                wm_log_dump(WM_LOG_LEVEL_DEBUG, "SSID:", 16, wifi_cfg.sta.ssid, cmd_length);
                wifi_join = 1;
                break;
            case AP_PASSWORD:
                if (cmd_length > sizeof(wifi_cfg.sta.password)) {
                    break;
                }
                memcpy(wifi_cfg.sta.password, &payload[offset], cmd_length);
                wm_log_dump(WM_LOG_LEVEL_DEBUG, "PASS:", 16, wifi_cfg.sta.password, cmd_length);
                wifi_join = 1;
                break;
            case AP_BSSID:
                if (cmd_length != sizeof(wifi_cfg.sta.bssid)) {
                    break;
                }
                memcpy(wifi_cfg.sta.bssid, &payload[offset], cmd_length);
                wifi_cfg.sta.bssid_set = true;
                wm_log_dump(WM_LOG_LEVEL_DEBUG, "BSSID:", 16, wifi_cfg.sta.bssid, cmd_length);
                wifi_join = 1;
                break;
            default:
                break;
        }
        offset += cmd_length;
        if (offset >= length) {
            break;
        }
        cmd_type   = payload[offset++];
        cmd_length = payload[offset++];
    }

    if (wifi_join) {
        wifi_join = wps_wifi_join(&wifi_cfg);
        if (wifi_join == 0) {
            return 0;
        }
    }

    wps_send_error_status(CONFIG_STA_CMD, 0x01);

    return -1;
}

static int wlan_enter_soft_ap_mode(const uint8_t *payload, int length)
{
    return 0;
}

static int wlan_enter_soft_ap_and_sta_mode(const uint8_t *payload, int length)
{
    return 0;
}

static int get_ap_list(const uint8_t *payload, int length)
{
    return 0;
}

static int wps_cmd_process(const uint8_t *ptr, int length)
{
    /* ptr = opcode + payload */
    switch (ptr[0]) {
        case CONFIG_STA_CMD:
            wlan_config_process(ptr + 1, length - 1);
            break;

        case CONFIG_SOFTAP_CMD:
            wlan_enter_soft_ap_mode(ptr + 1, length - 1);
            break;

        case CONFIG_STA_SOFTAP_CMD:
            wlan_enter_soft_ap_and_sta_mode(ptr + 1, length - 1);
            break;

        case CONFIG_WIFI_SCAN_CMD:
            get_ap_list(ptr + 1, length - 1);
            break;

        case CONFIG_KEY_EXCHANGE_CMD:
            key_exchange_process(ptr + 1, length - 1);
            break;
        default:
            break;
    }

    return 0;
}

static void wps_proto_rx_process(const proto_data_t *cmd)
{
    int err;
    proto_data_header_t *header = (proto_data_header_t *)cmd->data;

    if (header->seq != rx_seq) {
        wm_log_error("Invalid rx seq %d expected seq %d", header->seq, rx_seq);
        return;
    }
    rx_seq++;

    if (header->flag & ACK_FLAG_VALID_BIT) {
        wps_send_ack(header->seq);
    }

    if (cmd->data[cmd->len - 1] != wm_get_crc8((uint8_t *)&cmd->data[0], cmd->len - 1)) {
        wps_send_error_status(header->opcode, 0x01);
        return;
    }

    if (header->seqno == 0) {
        proto_cmd_buffer[0] = header->opcode;
        memcpy(&proto_cmd_buffer[1], &cmd->data[PROTO_HEADER_LENGTH], cmd->len - PROTO_HEADER_LENGTH - 1);
        proto_cmd_buffer_offset = cmd->len - PROTO_HEADER_LENGTH;
    } else {
        if ((proto_cmd_buffer_offset + cmd->len - PROTO_HEADER_LENGTH - 1) > sizeof(proto_cmd_buffer)) {
            wps_send_error_status(header->opcode, 0x01);
            return;
        }
        memcpy(&proto_cmd_buffer[proto_cmd_buffer_offset], &cmd->data[PROTO_HEADER_LENGTH], cmd->len - PROTO_HEADER_LENGTH - 1);
        proto_cmd_buffer_offset += (cmd->len - PROTO_HEADER_LENGTH - 1);
    }

    if (header->flag & MRE_FLAG_VALID_BIT) {
        ///fragmented payload, contine to receive
    } else {
        if (header->flag & ENC_FLAG_VALID_BIT) {
            err = bt_aes_decrypt(p_ciphercontext_aes, &proto_cmd_buffer[1], proto_cmd_buffer_offset - 1, &proto_cmd_buffer[1]);
            if (err < 0) {
                wm_log_error("bt aes decrypt failed");
                wps_send_error_status(header->opcode, 0x07);
                return;
            }
        }
        wps_cmd_process(&proto_cmd_buffer[0], proto_cmd_buffer_offset);
    }
}

static int wps_proto_tx_process(proto_data_t *cmd)
{
    int offset        = 0;
    int err           = 0;
    int ret           = 0;
    int left_len      = 0;
    uint16_t frag_mtu = 15;

    frag_mtu = bt_gatt_get_mtu(local_conn);

    if (frag_mtu == 0) {
        return ret;
    }

    frag_mtu = frag_mtu - 3 - 5;

    if (cmd->len <= frag_mtu) {
        cmd->single[0] = cmd->data[0];
        cmd->single[1] = tx_seq++;
        cmd->single[2] = cmd->data[2];
        cmd->single[3] = 0x00;
        offset         = 4;
        memcpy(&cmd->single[offset], &cmd->data[offset], cmd->len);
        offset += cmd->len;
        cmd->single[offset] = wm_get_crc8(cmd->single, offset);
        offset += 1;
        ///wm_log_dump(WM_LOG_LEVEL_INFO, "sent out", 16, cmd->single, offset);
        err = bt_wps_send(cmd->single, offset);
    } else {
        cmd->single[0] = cmd->data[0];
        cmd->single[1] = tx_seq++;
        cmd->single[2] = cmd->data[2];
        cmd->single[3] = cmd->out_len / frag_mtu;
        offset         = 4;

        left_len = cmd->len - cmd->out_len;
        if (left_len > frag_mtu) {
            cmd->single[2] |= MRE_FLAG_VALID_BIT;
            left_len = frag_mtu;
            ret      = 1;
        } else {
            ret = 0;
        }

        memcpy(&cmd->single[offset], &cmd->data[offset + cmd->out_len], left_len);
        offset += left_len;
        cmd->single[offset] = wm_get_crc8(cmd->single, offset);
        offset += 1;
        ///wm_log_dump(WM_LOG_LEVEL_INFO, "sent out", 16, cmd->single, offset);
        err = bt_wps_send(cmd->single, offset);
        cmd->out_len += left_len;
    }

    if (err == 0) {
        err = k_sem_take(&proto_sem, K_MSEC(300));
        if (err) {
            cmd->out_len -= left_len;
            ret = 1;
            tx_seq--;
            wm_log_warn("send to app timeout, ret %d", err);
        }
    } else {
        if (err != -ENOTCONN) {
            cmd->out_len -= left_len;
            ret = 1; //send error, resend;
            tx_seq--;
            wm_log_warn("send to app failed, ret %d", err);
        }
    }

    if (ret) {
        err = k_queue_prepend(&proto_queue, cmd);

        if (err) {
            free(cmd);
            wm_log_error("put to queue, err %d", err);
        }
    }

    return ret;
}
static void wps_txrx_thread(void *arg)
{
    int ret;
    proto_data_t *msg = NULL;

    while (1) {
        msg = k_queue_get(&proto_queue, K_FOREVER);
        if (msg) {
            if (msg->txrx == 0) {
                wps_proto_rx_process(msg);
            } else {
                ret = wps_proto_tx_process(msg);
                if (ret) {
                    continue;
                }
            }
            free(msg);
        }
    }
}

static void bt_sent_cb(struct bt_conn *conn)
{
    k_sem_give(&proto_sem);
}

static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
    int err;
    char addr[BT_ADDR_LE_STR_LEN] = { 0 };

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, ARRAY_SIZE(addr));
    wm_log_debug("Received data from: %s", addr);

    proto_data_t *cmd = (proto_data_t *)malloc(sizeof(proto_data_t));
    if (cmd == NULL) {
        wm_log_error("No free memory to alloc proto data");
        return;
    }
    memset(cmd, 0, sizeof(proto_data_t));
    if (len > sizeof(cmd->data)) {
        wm_log_warn("illegal payload length");
        free(cmd);
        return;
    }
    memcpy(cmd->data, data, len);
    cmd->len  = len;
    cmd->txrx = 0;

    err = k_queue_append(&proto_queue, cmd);
    if (err != 0) {
        wm_log_error("rx cmd put to queue failed");
        free(cmd);
    }

    return;
}
static uint8_t rejoin_counter = 0;
static void bt_connected_cb(struct bt_conn *conn, uint8_t err)
{
    tx_seq     = 0;
    rx_seq     = 0;
    local_conn = bt_conn_ref(conn);
    wm_event_add_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, wps_handler_sta_disconnected, NULL);
    wm_event_add_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)wps_handler_sta_got_ip, NULL);
    wm_log_info("wps service connected[%d]", rejoin_counter++);
}
static void bt_disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    proto_data_t *msg = NULL;

    while ((msg = k_queue_get(&proto_queue, K_NO_WAIT)) != NULL) {
        free(msg);
    }
    if (p_ciphercontext_aes) {
        free(p_ciphercontext_aes);
        p_ciphercontext_aes = NULL;
    }

    wm_event_remove_callback(WM_WIFI_EV, WM_EVENT_ANY_TYPE, wps_handler_sta_disconnected, NULL);
    wm_event_remove_callback(WM_LWIP_EV, WM_EVENT_ANY_TYPE, (wm_event_callback)wps_handler_sta_got_ip, NULL);
    bt_conn_unref(local_conn);
    local_conn = NULL;
    wm_log_info("wps service disconnected, reason code 0x%02x", reason);

#ifdef NVS_STORE_AFTER_WIFI_JOIN
    wm_wifi_set_storage(WM_WIFI_STORAGE_FLASH);
#endif
}

static struct bt_svc_cb wps_cb = {
    .connected    = bt_connected_cb,
    .disconnected = bt_disconnected_cb,
    .received     = bt_receive_cb,
    .sent         = bt_sent_cb,
};
static bool wps_init_already = false;

int wps_app_init(void)
{
    int err;

    if (wps_init_already) {
        return 0;
    }

    err = wm_wifi_init();
    if (err) {
        wm_log_error("Failed to initialize wifi (err: %d)", err);
        return err;
    }

    err = bt_wps_init(&wps_cb);
    if (err) {
        wm_log_error("Failed to initialize WPS service (err: %d)", err);
        wm_wifi_deinit();
        return err;
    }

    k_queue_init(&proto_queue);
    k_sem_init(&proto_sem, 0, 1);
    k_work_init_delayable(&delay_worker, delay_work_handler);

    /* RX thread */
    k_thread_create(&wps_txrx_thread_data, NULL, WPS_STACKSIZE, wps_txrx_thread, NULL, K_PRIO_COOP(WPS_PRIORITY), 0, K_NO_WAIT);
    k_thread_name_set(&wps_txrx_thread_data, "WPS RX");
    wps_init_already = true;

    return 0;
}

int wps_app_uninit(void)
{
    if (!wps_init_already) {
        return 0;
    }

    bt_wps_uninit();
    k_queue_deinit(&proto_queue);
    k_sem_deinit(&proto_sem);
    k_thread_delete(&wps_txrx_thread_data);
    wps_init_already = false;
    k_work_deinit_delayable(&delay_worker);
    wm_wifi_deinit();

    return 0;
}

bool wps_app_is_ready(void)
{
    return (wps_init_already == 1);
}
