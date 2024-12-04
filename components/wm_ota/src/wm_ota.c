#include <string.h>
#include "wm_ota.h"
#include "wm_ota_ops.h"
#include "wm_osal.h"

#define LOG_TAG "ota"
#include "wm_log.h"

static void wm_ota_update_state(wm_ota_ctx_t *ota_ctx, wm_ota_status_t status, uint32_t progress)
{
    // Update the OTA state and progress in the context.
    ota_ctx->ota_state.status   = status;
    ota_ctx->ota_state.progress = progress;
    if (ota_ctx->ota_state_cb != NULL && ota_ctx->ota_state_cb != 0) {
        ota_ctx->ota_state_cb(&ota_ctx->ota_state);
    }
    // wm_log_debug("Download progress:%d-%d%%", ota_ctx->ota_state.status, ota_ctx->ota_state.progress);
}

int wm_ota_init(wm_ota_ctx_t *ota_ctx)
{
    int ret = WM_ERR_SUCCESS;

    // Check for invalid parameters or uninitialized callbacks.
    if (ota_ctx == NULL || ota_ctx->ota_session.ota_connect_cb == NULL || ota_ctx->ota_session.ota_get_firmware_cb == NULL ||
        ota_ctx->ota_session.ota_close_cb == NULL || ota_ctx->ota_session.ota_connect_cb == 0 ||
        ota_ctx->ota_session.ota_get_firmware_cb == 0 || ota_ctx->ota_session.ota_close_cb == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_ota_ops_get_ota_partition(&ota_ctx->ota_ops_ctx);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    // Begin the OTA process with an unknown image size.
    ret = wm_ota_ops_begin(&ota_ctx->ota_ops_ctx, OTA_SIZE_UNKNOWN);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    return ret;
}

static int ota_start(wm_ota_ctx_t *ota_ctx, bool reboot)
{
    int ret                     = WM_ERR_SUCCESS;
    uint8_t *ota_buffer         = NULL;
    int reconnect_cnt           = 0;
    uint32_t progress           = 0;
    uint32_t next_read_size     = CONFIG_OTA_BLOCK_SIZE;
    uint32_t got                = 0;
    uint32_t new_progress       = 0;
    uint32_t ota_img_total_size = 0;

    // Notify the start of the connection.
    wm_ota_update_state(ota_ctx, WM_OTA_STATUS_CONNECTION_START, progress);

    // Attempt to connect to the server, with a maximum number of retries.
    do {
        ota_ctx->ota_conn_ret = ota_ctx->ota_session.ota_connect_cb(&ota_ctx->handle, ota_ctx->wrote_offset);
        if (ota_ctx->ota_conn_ret != WM_ERR_SUCCESS) {
            wm_os_internal_time_delay_ms(50);
        }
    } while ((ota_ctx->ota_conn_ret != WM_ERR_SUCCESS) && (reconnect_cnt++ < CONFIG_OTA_SOCKET_RECONNECT_TIMES));

    if (ota_ctx->ota_conn_ret != WM_ERR_SUCCESS) {
        wm_ota_update_state(ota_ctx, WM_OTA_STATUS_CONNECTION_FAILED, progress);
        wm_log_error("OTA connect failed: %d", ota_ctx->ota_conn_ret);
        return WM_ERR_OTA_CONNECTION_FAILED;
    } else {
        wm_ota_update_state(ota_ctx, WM_OTA_STATUS_CONNECTED, progress);
    }

    wm_ota_update_state(ota_ctx, WM_OTA_STATUS_DOWNLOAD_START, progress);
    // Allocate memory for the OTA buffer.
    ota_buffer = (uint8_t *)wm_os_internal_malloc(CONFIG_OTA_BLOCK_SIZE + 1); // +1 for http
    if (ota_buffer == NULL) {
        wm_ota_update_state(ota_ctx, WM_OTA_STATUS_ABORT, progress);
        return WM_ERR_NO_MEM;
    }
    memset(ota_buffer, 0, CONFIG_OTA_BLOCK_SIZE);
    wm_log_debug("OTA block size: %d", CONFIG_OTA_BLOCK_SIZE);

    // Download and write firmware in a loop until complete or an error occurs.
    while (1) {
        // Get firmware data from the server.
        ret = ota_ctx->ota_session.ota_get_firmware_cb(ota_ctx->handle, 0, ota_buffer, next_read_size, &got);
        if (ret != WM_ERR_SUCCESS) {
            wm_ota_update_state(ota_ctx, WM_OTA_STATUS_DOWNLOAD_FAILED, progress);
            wm_log_error("OTA get firmware failed: %d", ret);
            break;
        } else if (got == 0) {
            wm_os_internal_time_delay_ms(10);
            continue;
        }
        // Write the received data to the firmware.
        ret = wm_ota_ops_write(&ota_ctx->ota_ops_ctx, ota_buffer, got);
        if (ret != WM_ERR_SUCCESS) {
            wm_ota_update_state(ota_ctx, WM_OTA_STATUS_ABORT, progress);
            wm_log_error("OTA write failed: %d", ret);
            break;
        }

        // Update progress and check if the download is complete.
        if (!ota_img_total_size) {
            ota_img_total_size = ota_ctx->ota_ops_ctx.ota_header.img_len + OTA_FW_HEADER_SIZE_IN_BIN;
        }
        ota_ctx->wrote_offset += got;
        new_progress = ota_ctx->wrote_offset * 100 / ota_img_total_size;
        if (progress != new_progress) {
            printf(".");
            progress = new_progress;
            wm_ota_update_state(ota_ctx, WM_OTA_STATUS_DOWNLOAD_START, progress);
        }
        if ((ota_img_total_size - ota_ctx->wrote_offset) >= CONFIG_OTA_BLOCK_SIZE) {
            next_read_size = CONFIG_OTA_BLOCK_SIZE;
        } else {
            next_read_size = ota_img_total_size - ota_ctx->wrote_offset;
        }
        // wm_log_debug("OTA wrote offset: %d(%d), got: %d", ota_ctx->wrote_offset, ota_img_total_size, got);
        if (ota_ctx->wrote_offset >= ota_img_total_size) {
            printf(".\n");
            progress = 100;
            ret      = wm_ota_ops_end(&ota_ctx->ota_ops_ctx);
            if (ret != WM_ERR_SUCCESS) {
                wm_ota_update_state(ota_ctx, WM_OTA_STATUS_ABORT, progress);
                wm_log_error("OTA end failed: %d", ret);
                break;
            }

            wm_ota_update_state(ota_ctx, WM_OTA_STATUS_DOWNLOADED, progress);
            if (reboot) {
                wm_ota_update_state(ota_ctx, WM_OTA_STATUS_UPGRADE_START, progress);
                wm_os_internal_time_delay_ms(10);
                ret = wm_ota_ops_reboot();
                if (ret != WM_ERR_SUCCESS) {
                    wm_ota_update_state(ota_ctx, WM_OTA_STATUS_UPGRADE_FAILED, progress);
                }
            }
            break;
        }
        wm_os_internal_time_delay_ms(1);
    }

    wm_os_internal_free(ota_buffer);
    ota_buffer = NULL;

    return ret;
}

int wm_ota_start(wm_ota_ctx_t *ota_ctx, bool reboot)
{
    int ret = WM_ERR_SUCCESS;

    wm_ota_ops_ctx_t ota_ops_ctx_zero = { 0 };
    uint32_t start_time               = wm_os_internal_get_time_ms();

    if (ota_ctx == NULL || ota_ctx->ota_session.ota_connect_cb == NULL || ota_ctx->ota_session.ota_get_firmware_cb == NULL ||
        ota_ctx->ota_session.ota_close_cb == NULL || ota_ctx->ota_session.ota_connect_cb == 0 ||
        ota_ctx->ota_session.ota_get_firmware_cb == 0 || ota_ctx->ota_session.ota_close_cb == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (memcmp(&ota_ctx->ota_ops_ctx, &ota_ops_ctx_zero, sizeof(wm_ota_ops_ctx_t)) == 0) {
        wm_ota_update_state(ota_ctx, WM_OTA_STATUS_ABORT, 0);
        return WM_ERR_OTA_NO_INIT;
    }

    wm_ota_update_state(ota_ctx, WM_OTA_STATUS_START, 0);

    // Retry starting OTA within the configured timeout.
    do {
        ret = ota_start(ota_ctx, reboot);
    } while ((ret == WM_ERR_OTA_TIMEOUT) && ((wm_os_internal_get_time_ms() - start_time) < CONFIG_OTA_RETRY_TIMEOUT));

    return ret;
}

int wm_ota_stop(wm_ota_ctx_t *ota_ctx)
{
    int ret = WM_ERR_SUCCESS;

    wm_ota_ops_ctx_t ota_ops_ctx_zero = { 0 };

    if (ota_ctx == NULL || ota_ctx->ota_session.ota_connect_cb == NULL || ota_ctx->ota_session.ota_get_firmware_cb == NULL ||
        ota_ctx->ota_session.ota_close_cb == NULL || ota_ctx->ota_session.ota_connect_cb == 0 ||
        ota_ctx->ota_session.ota_get_firmware_cb == 0 || ota_ctx->ota_session.ota_close_cb == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (memcmp(&ota_ctx->ota_ops_ctx, &ota_ops_ctx_zero, sizeof(wm_ota_ops_ctx_t)) == 0) {
        wm_ota_update_state(ota_ctx, WM_OTA_STATUS_ABORT, 0);
        return WM_ERR_OTA_NO_INIT;
    }

    // Stop the OTA operation and clean up resources.
    if (ota_ctx->ota_conn_ret == WM_ERR_SUCCESS && ota_ctx->ota_session.ota_close_cb) {
        ota_ctx->ota_session.ota_close_cb(ota_ctx->handle);
    }

    ota_ctx->handle       = 0;
    ota_ctx->ota_conn_ret = -1;

    return ret;
}

int wm_ota_deinit(wm_ota_ctx_t *ota_ctx)
{
    int ret = WM_ERR_SUCCESS;

    wm_ota_ops_ctx_t ota_ops_ctx_zero = { 0 };

    if (ota_ctx == NULL || ota_ctx->ota_session.ota_connect_cb == NULL || ota_ctx->ota_session.ota_get_firmware_cb == NULL ||
        ota_ctx->ota_session.ota_close_cb == NULL || ota_ctx->ota_session.ota_connect_cb == 0 ||
        ota_ctx->ota_session.ota_get_firmware_cb == 0 || ota_ctx->ota_session.ota_close_cb == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (memcmp(&ota_ctx->ota_ops_ctx, &ota_ops_ctx_zero, sizeof(wm_ota_ops_ctx_t)) == 0) {
        wm_ota_update_state(ota_ctx, WM_OTA_STATUS_ABORT, 0);
        return WM_ERR_OTA_NO_INIT;
    }

    // Deinitialize the OTA context and clean up.
    wm_ota_stop(ota_ctx);
    memset(ota_ctx, 0, sizeof(wm_ota_ctx_t));

    return ret;
}
