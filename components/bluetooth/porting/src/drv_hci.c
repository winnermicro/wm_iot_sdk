
#include <bluetooth/hci.h>
#include <drv_hci.h>
#include <sys/byteorder.h>

#include "wm_component.h"
#include "bluetooth/controller.h"
#include "wm_pm.h"
#include "wm_drv_pmu.h"
#define LOG_LEVEL CONFIG_BT_HCI_DRIVER_LOG_LEVEL
#define LOG_TAG "bt_hci_drv"
#include <logging/log.h>

#define HCI_CMD                 0x01
#define HCI_ACL                 0x02
#define HCI_SCO                 0x03
#define HCI_EVT                 0x04
#define HCI_ISO                 0x05

#define HCI_BT_HCI_TIMEOUT K_MSEC(2000)

#if !defined(CONFIG_BT_TX_POWER_LEVEL_INDEX)
#define CONFIG_BT_TX_POWER_LEVEL_INDEX   (2)
#endif

static K_SEM_DEFINE(hci_wait_available_sem, 0, 1);
static volatile uint32_t hci_available_pkts_buffer = 0;
static struct k_mutex k_controller_mutex;

static bool is_hci_event_discardable(const uint8_t *evt_data)
{
	uint8_t evt_type = evt_data[0];

	switch (evt_type) {
#if defined(CONFIG_BT_BREDR)
	case BT_HCI_EVT_INQUIRY_RESULT_WITH_RSSI:
	case BT_HCI_EVT_EXTENDED_INQUIRY_RESULT:
		return true;
#endif
	case BT_HCI_EVT_LE_META_EVENT: {
		uint8_t subevt_type = evt_data[sizeof(struct bt_hci_evt_hdr)];

		switch (subevt_type) {
		case BT_HCI_EVT_LE_ADVERTISING_REPORT:
			return true;
		default:
			return false;
		}
	}
	default:
		return false;
	}
}

static struct net_buf *bt_evt_recv(uint8_t *data, size_t remaining)
{
	bool discardable = false;
	struct bt_hci_evt_hdr hdr;
	struct net_buf *buf;
	size_t buf_tailroom;

	if (remaining < sizeof(hdr)) {
		LOG_ERR("Not enough data for event header");
		return NULL;
	}

	discardable = is_hci_event_discardable(data);

	memcpy((void *)&hdr, data, sizeof(hdr));
	data += sizeof(hdr);
	remaining -= sizeof(hdr);

	if (remaining != hdr.len) {
		LOG_ERR("Event payload length is not correct");
		return NULL;
	}
	LOG_DBG("len %u", hdr.len);
	buf = bt_buf_get_evt(hdr.evt, discardable, K_NO_WAIT);
	if (!buf) {
		if (discardable) {
			LOG_DBG("Discardable buffer pool full, ignoring event");
		} else {
			LOG_WRN("No available event buffers!");
		}
		return buf;
	}

	net_buf_add_mem(buf, &hdr, sizeof(hdr));

	buf_tailroom = net_buf_tailroom(buf);
	if (buf_tailroom < remaining) {
		LOG_ERR("Not enough space in buffer %zu/%zu", remaining, buf_tailroom);
		net_buf_unref(buf);
		return NULL;
	}

	net_buf_add_mem(buf, data, remaining);

	return buf;
}

static struct net_buf *bt_acl_recv(uint8_t *data, size_t remaining)
{
	struct bt_hci_acl_hdr hdr;
	struct net_buf *buf;
	size_t buf_tailroom;

	if (remaining < sizeof(hdr)) {
		LOG_ERR("Not enough data for ACL header");
		return NULL;
	}

	buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_NO_WAIT);
	if (buf) {
		memcpy((void *)&hdr, data, sizeof(hdr));
		data += sizeof(hdr);
		remaining -= sizeof(hdr);

		net_buf_add_mem(buf, &hdr, sizeof(hdr));
	} else {
		LOG_ERR("No available ACL buffers!");
		return NULL;
	}

	if (remaining != sys_le16_to_cpu(hdr.len)) {
		LOG_ERR("ACL payload length is not correct");
		net_buf_unref(buf);
		return NULL;
	}

	buf_tailroom = net_buf_tailroom(buf);
	if (buf_tailroom < remaining) {
		LOG_ERR("Not enough space in buffer %zu/%zu", remaining, buf_tailroom);
		net_buf_unref(buf);
		return NULL;
	}

	LOG_DBG("len %u", remaining);
	net_buf_add_mem(buf, data, remaining);

	return buf;
}

static struct net_buf *bt_iso_recv(uint8_t *data, size_t remaining)
{
	struct bt_hci_iso_hdr hdr;
	struct net_buf *buf;
	size_t buf_tailroom;

	if (remaining < sizeof(hdr)) {
		LOG_ERR("Not enough data for ISO header");
		return NULL;
	}

	buf = bt_buf_get_rx(BT_BUF_ISO_IN, K_NO_WAIT);
	if (buf) {
		memcpy((void *)&hdr, data, sizeof(hdr));
		data += sizeof(hdr);
		remaining -= sizeof(hdr);

		net_buf_add_mem(buf, &hdr, sizeof(hdr));
	} else {
		LOG_ERR("No available ISO buffers!");
		return NULL;
	}

	if (remaining != bt_iso_hdr_len(sys_le16_to_cpu(hdr.len))) {
		LOG_ERR("ISO payload length is not correct");
		net_buf_unref(buf);
		return NULL;
	}

	buf_tailroom = net_buf_tailroom(buf);
	if (buf_tailroom < remaining) {
		LOG_ERR("Not enough space in buffer %zu/%zu", remaining, buf_tailroom);
		net_buf_unref(buf);
		return NULL;
	}

	LOG_DBG("len %zu", remaining);
	net_buf_add_mem(buf, data, remaining);

	return buf;
}

static void hci_host_rcv_pkt(uint8_t *data, uint16_t len)
{
	uint8_t pkt_indicator;
	struct net_buf *buf = NULL;
	size_t remaining = len;

	//LOG_HEXDUMP_DBG(data, len, "host packet data:");

	pkt_indicator = *data++;
	remaining -= sizeof(pkt_indicator);

	switch (pkt_indicator) {
	case HCI_EVT:
		buf = bt_evt_recv(data, remaining);
		break;

	case HCI_ACL:
		buf = bt_acl_recv(data, remaining);
		break;

	case HCI_SCO:
		buf = bt_iso_recv(data, remaining);
		break;

	default:
		LOG_ERR("Unknown HCI type %u", pkt_indicator);
            return;
	}

	if (buf) {
		LOG_DBG("Calling bt_recv(%p)", buf);

        if (IS_ENABLED(CONFIG_BT_RECV_BLOCKING)) {
            if ((pkt_indicator == HCI_EVT) && ((data[0] == BT_HCI_EVT_CMD_COMPLETE) || (data[0] == BT_HCI_EVT_CMD_STATUS))) {
                bt_recv_prio(buf);
                return;
            }
        }

		bt_pre_recv(buf);
	}
}

static void hci_controller_rcv_pkt_ready(int available_counter)
{
	hci_available_pkts_buffer = available_counter;
}

static wm_bt_host_if_t vuart_hci_cb = {
	.notify_controller_avaiable_hci_buffer = hci_controller_rcv_pkt_ready,
	.notify_host_recv_h4 = hci_host_rcv_pkt,
};

static int drv_bt_send(struct net_buf *buf)
{
	int err = 0, wait_resend_counter = 0;
	uint8_t pkt_indicator;

	LOG_DBG("buf %p type %u len %u", buf, bt_buf_get_type(buf), buf->len);

	switch (bt_buf_get_type(buf)) {
	case BT_BUF_ACL_OUT:
		pkt_indicator = HCI_ACL;
		break;
	case BT_BUF_CMD:
		pkt_indicator = HCI_CMD;
		break;
	case BT_BUF_ISO_OUT:
		pkt_indicator = HCI_ISO;
		break;
	default:
		LOG_WRN("Unknown type %u", bt_buf_get_type(buf));
		goto done;
	}
	net_buf_push_u8(buf, pkt_indicator);
	LOG_HEXDUMP_DBG(buf->data, buf->len, "Final HCI buffer:");
    if (pkt_indicator != HCI_CMD) {
        while (!wm_bt_host_check_send_available()) {
            wm_os_internal_time_delay(1);
            wait_resend_counter++;
            if (wait_resend_counter > 1000) {
                LOG_WRN("Controller ACL buffer hungry...\r\n");
                break;
            }
        }
    }
    k_mutex_lock(6, &k_controller_mutex, K_FOREVER);
    wm_bt_host_send_packet(buf->data, buf->len);
    k_mutex_unlock(&k_controller_mutex);

#if 0
    //if(hci_available_pkts_buffer>0)
    if(wm_bt_host_check_send_available())
    {
        wm_bt_host_send_packet(buf->data, buf->len);
    }else {
        LOG_ERR("Send packet timeout error");
    	if (k_sem_take(&hci_wait_available_sem, HCI_BT_HCI_TIMEOUT) == 0) {
            wm_bt_host_send_packet(buf->data, buf->len);
    	} else {
    		LOG_ERR("Send packet timeout error");
    		err = -ETIMEDOUT;
    	}
    }
#endif

done:
	net_buf_unref(buf);

	return err;
}

#if defined(CONFIG_BT_LOW_POWER_SYSTEM)

//bluetooth controller callback
static void bt_sleep_enter(uint32_t duration_ms)
{
    wm_pm_lock_release();
}

static void bt_sleep_exit(void)
{
    wm_pm_lock_acquire();
}
#endif

void bt_lowpower_enable(void)
{
    wm_bt_ctrl_sleep(true);
}

void bt_lowpower_disable(void)
{
    wm_bt_ctrl_sleep(false);
}

static int bt_ble_init(void)
{
    wm_bt_hci_if_t hci_if;
    wm_bt_status_t status;

    k_mutex_init(&k_controller_mutex);

    status = wm_bt_ctrl_if_register(&vuart_hci_cb);

    if (status != WM_BT_STATUS_SUCCESS) {
        LOG_ERR("tls_bt_ctrl_if_register error");
        return WM_BT_STATUS_CTRL_ENABLE_FAILED;
    }

    hci_if.uart_index = 0xFF;
    status = wm_bt_ctrl_enable(&hci_if, 0);

    if ((status != WM_BT_STATUS_SUCCESS) && (status != WM_BT_STATUS_DONE)) {
        LOG_ERR("tls_bt_ctrl_enable error");
        return WM_BT_STATUS_CTRL_ENABLE_FAILED;
    }
    wm_ble_set_tx_power(WM_BLE_PWR_TYPE_DEFAULT, CONFIG_BT_TX_POWER_LEVEL_INDEX);

#if defined(CONFIG_BT_LOW_POWER_CONTROLLER) || defined(CONFIG_BT_LOW_POWER_SYSTEM)
            wm_device_t *pmu = wm_dt_get_device_by_name("pmu");
            if(!pmu) {
                LOG_WRN("get device pmu failed");
            } else {
                int ret = wm_drv_pmu_set_clock_source(pmu, WM_PMU_CLOCK_SRC_40M_DIV);
                
                if(ret == WM_ERR_SUCCESS) {
#if defined(CONFIG_BT_LOW_POWER_SYSTEM)
                    wm_bt_register_sleep_callback(bt_sleep_enter, bt_sleep_exit);
#endif
                    //wm_bt_ctrl_sleep(true);
                }
            }

#endif

    return 0;
}

static int drv_bt_close(void)
{
    wm_bt_ctrl_disable();
    wm_bt_ctrl_if_unregister();
#if defined(CONFIG_BT_LOW_POWER_SYSTEM)
    wm_bt_register_sleep_callback(NULL, NULL);
#endif
    k_mutex_deinit(&k_controller_mutex);

    LOG_DBG("HCI BT closed");

    return 0;
}
static int drv_bt_open(void)
{
	int err;
	err = bt_ble_init();
	if (err) {
		return err;
	}

	LOG_DBG("HCI BT started");

	return 0;
}

static const struct bt_hci_driver drv = {
	.name           = "BT W800",
	.open           = drv_bt_open,
	.send           = drv_bt_send,
	.close          = drv_bt_close,
	.bus            = BT_HCI_DRIVER_BUS_IPM,
#if defined(CONFIG_BT_DRIVER_QUIRK_NO_AUTO_DLE)
	.quirks         = BT_QUIRK_NO_AUTO_DLE,
#endif
};

static int bt_hci_init(void)
{
	bt_hci_driver_register(&drv);

	return 0;
}

WM_COMPONEN_INIT_2(bt_hci_init);
