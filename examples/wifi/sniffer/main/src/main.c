#include <stdio.h>
#include <string.h>
#include "wm_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LOG_TAG "sniffer_example"
#include "wm_log.h"

#define SNIFFER_DEFAULT_CHANNEL           (6)
#define SNIFFER_PAYLOAD_FCS_LEN           (4)
#define SNIFFER_PROCESS_PACKET_TIMEOUT_MS (100)

static uint32_t exit_count = 0;
static QueueHandle_t work_queue;

typedef struct {
    void *payload;
    uint32_t length;
    uint32_t seconds;
    uint32_t microseconds;
} sniffer_packet_info_t;

static void queue_packet(void *recv_packet, sniffer_packet_info_t *packet_info)
{
    /* Copy a packet from Link Layer driver and queue the copy to be processed by sniffer task */
    void *packet_to_queue = malloc(packet_info->length);
    if (packet_to_queue) {
        memcpy(packet_to_queue, recv_packet, packet_info->length);
        packet_info->payload = packet_to_queue;
        if (work_queue) {
            /* send packet_info */
            if (xQueueSend(work_queue, packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS)) != pdTRUE) {
                wm_log_error("sniffer work queue full");
                free(packet_info->payload);
            }
        }
    } else {
        wm_log_error("No enough memory for promiscuous packet");
    }
}

static void wifi_sniffer_cb(wm_wifi_promiscuous_pkt_t *buf, wm_wifi_promiscuous_pkt_type_t type)
{
    sniffer_packet_info_t packet_info;
    wm_wifi_promiscuous_pkt_t *sniffer = (wm_wifi_promiscuous_pkt_t *)buf;
    /* prepare packet_info */
    packet_info.seconds      = sniffer->rx_ctrl.timestamp / 1000000U;
    packet_info.microseconds = sniffer->rx_ctrl.timestamp % 1000000U;
    packet_info.length       = sniffer->rx_ctrl.sig_len;

    /* For now, the sniffer only dumps the length of the MISC type frame */
    if (type != WM_WIFI_PKT_MISC && !sniffer->rx_ctrl.rx_state) {
        packet_info.length -= SNIFFER_PAYLOAD_FCS_LEN;
        queue_packet(sniffer->payload, &packet_info);
    }
}

static void sniffer_task(void *parameters)
{
    sniffer_packet_info_t packet_info;

    while (1) {
        if (exit_count > 100)
            break;

        /* receive packet info from queue */
        if (xQueueReceive(work_queue, &packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS)) != pdTRUE) {
            continue;
        }

        wm_log_info("rx packet %p, len %u, time %u.%u", packet_info.payload, packet_info.length, packet_info.seconds,
                    packet_info.microseconds);
        free(packet_info.payload);

        exit_count++;
    }

    wm_wifi_set_promiscuous(false);

    /* make sure to free all resources in the left items */
    UBaseType_t left_items = uxQueueMessagesWaiting(work_queue);
    while (left_items--) {
        xQueueReceive(work_queue, &packet_info, pdMS_TO_TICKS(SNIFFER_PROCESS_PACKET_TIMEOUT_MS));
        free(packet_info.payload);
    }
    vQueueDelete(work_queue);
    work_queue = NULL;

    wm_log_info("stop promiscuous ok");

    vTaskDelete(NULL);
}

static void do_sniffer(void)
{
    wm_wifi_promiscuous_filter_t wifi_filter;

    wm_wifi_init();

    work_queue = xQueueCreate(128, sizeof(sniffer_packet_info_t));
    xTaskCreate(sniffer_task, "snifferT", 4096, NULL, 2, NULL);

    wm_wifi_set_channel(SNIFFER_DEFAULT_CHANNEL, WM_WIFI_SECOND_CHAN_NONE);
    wifi_filter.filter_mask = WM_WIFI_PROMIS_FILTER_MASK_ALL;
    wm_wifi_set_promiscuous_filter(&wifi_filter);
    wm_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
    wm_wifi_set_promiscuous(true);

    wm_log_info("start WiFi promiscuous ok");
}

int main(void)
{
    do_sniffer();

    return 0;
}
