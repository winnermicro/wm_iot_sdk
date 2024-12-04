#include <string.h>
#include "wmsdk_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_ota.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define OTA_TASK_STACK (4096)
#define OTA_TASK_PRIO  (configMAX_PRIORITIES - 10)

static TaskHandle_t ota_task_handle = NULL;

extern const uint8_t ota_img_start[] asm("__binary_main_src_atcmd_ota_img");
extern const uint32_t ota_img_length asm("__binary_main_src_atcmd_ota_img_length");

static int ota_get_img_data(uint8_t *data, size_t *size)
{
    int ret                    = WM_ERR_SUCCESS;
    static uint32_t img_offset = 0;

    if (ota_img_length - img_offset >= CONFIG_OTA_BLOCK_SIZE) {
        *size = CONFIG_OTA_BLOCK_SIZE;
    } else {
        *size = ota_img_length - img_offset;
    }

    if (*size > 0) {
        memcpy(data, &ota_img_start[img_offset], *size);
        img_offset += *size;
    } else {
        return WM_ERR_FAILED;
    }

    return ret;
}

static void ota_task(void *arg)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_ota_ops_ctx_t ota_ops_ctx = { 0 };
    uint8_t *ota_buffer          = NULL;
    uint32_t size                = 0;
    uint32_t ota_img_total_size  = 0;
    uint32_t wrote_offset        = 0;
    uint32_t progress            = 0;
    uint32_t new_progress        = 0;

    ota_buffer = (uint8_t *)malloc(CONFIG_OTA_BLOCK_SIZE);
    if (!ota_buffer) {
        wm_log_error("memory allocation failed");
        goto exit;
    }
    memset(ota_buffer, 0, CONFIG_OTA_BLOCK_SIZE);

    ret = wm_ota_ops_get_ota_partition(&ota_ops_ctx);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("OTA get partition failed: %d", ret);
        goto exit;
    }

    /*
     * Determine whether a full erase of the OTA partition is necessary based on the image_size value:
     * - 0 or OTA_SIZE_UNKNOWN: Perform a full erase of the 'app_ota' partition.
     * - OTA_WITH_SEQUENTIAL_WRITES: No need to erase now; the erase will occur during the subsequent write process.
     * - Actual image size: Erase the 'app_ota' partition to match the actual size of the image.
     */
    ret = wm_ota_ops_begin(&ota_ops_ctx, OTA_SIZE_UNKNOWN);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("OTA begin failed: %d", ret);
        goto exit;
    }

    while (1) {
        ret = ota_get_img_data(ota_buffer, &size);
        if (ret != WM_ERR_SUCCESS && size > 0) {
            wm_log_error("Get img failed: %d", ret);
            goto exit;
        }

        ret = wm_ota_ops_write(&ota_ops_ctx, ota_buffer, size);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("OTA write failed: %d", ret);
            goto exit;
        }

        if (!ota_img_total_size) {
            ota_img_total_size = ota_ops_ctx.ota_header.img_len + OTA_FW_HEADER_SIZE_IN_BIN;
        }

        wrote_offset += size;
        new_progress = wrote_offset * 100 / ota_img_total_size;
        if (progress != new_progress) {
            printf(".");
        }

        if (wrote_offset >= ota_img_total_size) {
            printf("\n");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    ret = wm_ota_ops_end(&ota_ops_ctx);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("OTA end failed: %d", ret);
        goto exit;
    }

    wm_log_info("This example succeeds in running.");
    ret = wm_ota_ops_reboot();
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("OTA reboot failed: %d", ret);
        goto exit;
    }

exit:

    if (ota_buffer) {
        free(ota_buffer);
        ota_buffer = NULL;
    }

    vTaskDelete(NULL);
}

int main(void)
{
    int err = xTaskCreate(ota_task, "ota_task", OTA_TASK_STACK, NULL, OTA_TASK_PRIO, &ota_task_handle);
    if (err != pdTRUE || ota_task_handle == NULL) {
        wm_log_error("ota_task creation failed.");
    }
    return 0;
}
