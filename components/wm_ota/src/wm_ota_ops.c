#include <string.h>
#include "wm_ota_ops.h"
#include "wm_partition_table.h"
#include "wm_drv_flash.h"
#include "wm_hal_wdt.h"
#include "wm_osal.h"
#include "wm_utils.h"
#include "wm_debug.h"

#define LOG_TAG "ota_ops"
#include "wm_log.h"

#define OTA_FLASH_BASE_ADDR        (0x8000000)  /**< Base address of the flash memory */
#define OTA_FLASH_SECTOR_SIZE      (0x1000)     /**< Flash sector size: 4096 bytes */
#define OTA_MAGIC_NO               (0xA0FFFF9F) /**< OTA firmware header magic number */
#define OTA_APP_RUN_ADDRESS_OFFSET (0x400)      /**< Run address offset for the application */

static int wm_ota_ops_check_header(wm_ota_ops_ctx_t *wm_ota_ops_ctx, const uint8_t *data, size_t size)
{
    int ret             = WM_ERR_SUCCESS;
    uint32_t crc32      = 0;
    char sw_version[16] = { 0 };

    // Check if the context, data, or size is invalid
    if (wm_ota_ops_ctx == NULL || data == NULL || size < OTA_FW_HEADER_SIZE_IN_BIN) {
        return WM_ERR_INVALID_PARAM;
    }

    // wm_log_dump("ota_header:", 8, (void *)data, sizeof(wm_ota_header_t) + 16);

    // Copy the OTA header from the data buffer to the context structure.
    memcpy(&wm_ota_ops_ctx->ota_header, data, sizeof(wm_ota_header_t));
    wm_log_debug("magic_no: %08X", wm_ota_ops_ctx->ota_header.magic_no);
    wm_log_debug("img_attr: %08X", wm_ota_ops_ctx->ota_header.img_attr.val);
    wm_log_debug("img_addr: %08X", wm_ota_ops_ctx->ota_header.img_addr);
    wm_log_debug("img_len: %08X", wm_ota_ops_ctx->ota_header.img_len);
    wm_log_debug("img_header_addr: %08X", wm_ota_ops_ctx->ota_header.img_header_addr);
    wm_log_debug("upgrade_img_addr: %08X", wm_ota_ops_ctx->ota_header.upgrade_img_addr);
    wm_log_debug("org_checksum: %08X", wm_ota_ops_ctx->ota_header.org_checksum);
    wm_log_debug("upd_no: %08X", wm_ota_ops_ctx->ota_header.upd_no);
    wm_log_debug("ver: %s", wm_ota_ops_ctx->ota_header.ver);
    wm_log_debug("log_level: %08X", wm_ota_ops_ctx->ota_header.log_level);
    wm_log_debug("skip_esc_check: %08X", wm_ota_ops_ctx->ota_header.skip_esc_check);
    wm_log_debug("skip_upgrade_check: %08X", wm_ota_ops_ctx->ota_header.skip_upgrade_check);
    wm_log_debug("skip_integrity_check: %08X", wm_ota_ops_ctx->ota_header.skip_integrity_check);
    wm_log_debug("_reserved0: %08X", wm_ota_ops_ctx->ota_header._reserved0);
    wm_log_debug("_reserved1: %08X", wm_ota_ops_ctx->ota_header._reserved1);
    wm_log_debug("next: %08X", (uint32_t)wm_ota_ops_ctx->ota_header.next);
    wm_log_debug("hd_checksum: %08X", wm_ota_ops_ctx->ota_header.hd_checksum);

    // Check if the magic code in the header matches the expected value.
    if (wm_ota_ops_ctx->ota_header.magic_no != OTA_MAGIC_NO) {
        return WM_ERR_OTA_HEADER_INVALID;
    }

    // Check if the image type is valid
    if (wm_ota_ops_ctx->ota_header.img_attr.bits.img_type == 0 || wm_ota_ops_ctx->ota_header.img_attr.bits.img_type == 0x0E) {
        return WM_ERR_OTA_HEADER_INVALID;
    }

    // Check if gzip is enabled
    if (wm_ota_ops_ctx->ota_header.img_attr.bits.gzip_enable != 0) {
        return WM_ERR_OTA_HEADER_INVALID;
    }

    // Check if the compression type is valid
    if (wm_ota_ops_ctx->ota_header.img_attr.bits.compress_type >= WM_OTA_OPS_COMPRESS_MAX) {
        return WM_ERR_OTA_HEADER_INVALID;
    }

    // Check if the image address is valid
    if ((wm_ota_ops_ctx->ota_header.img_addr !=
         OTA_FLASH_BASE_ADDR + wm_ota_ops_ctx->app_ota_addr + OTA_APP_RUN_ADDRESS_OFFSET) &&
        (wm_ota_ops_ctx->ota_header.img_addr != OTA_FLASH_BASE_ADDR + wm_ota_ops_ctx->app_addr + OTA_APP_RUN_ADDRESS_OFFSET)) {
        return WM_ERR_OTA_HEADER_INVALID;
    }

    // Check if the firmware size plus the header size exceeds the application partition size.
    if (wm_ota_ops_ctx->ota_header.img_len > (wm_ota_ops_ctx->app_ota_size - OTA_FW_HEADER_SIZE_IN_BIN)) {
        return WM_ERR_OTA_FW_OVERFLOW;
    }

    // Check if the image header address is valid
    if ((wm_ota_ops_ctx->ota_header.img_header_addr != OTA_FLASH_BASE_ADDR + wm_ota_ops_ctx->app_ota_addr) &&
        (wm_ota_ops_ctx->ota_header.img_header_addr != OTA_FLASH_BASE_ADDR + wm_ota_ops_ctx->app_addr)) {
        return WM_ERR_OTA_HEADER_INVALID;
    }

    // Check if the upgrade image address is 0
    if (wm_ota_ops_ctx->ota_header.upgrade_img_addr != 0 &&
        wm_ota_ops_ctx->ota_header.upgrade_img_addr != OTA_FLASH_BASE_ADDR + wm_ota_ops_ctx->app_ota_addr) {
        return WM_ERR_OTA_HEADER_INVALID;
    }

    // Check if the current software version is the same as the version in the OTA header
    ret = wm_ota_ops_get_version(sw_version);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }
    if (!strcmp((const char *)wm_ota_ops_ctx->ota_header.ver, sw_version)) {
        wm_log_info("ota version is the same");
        // return WM_ERR_OTA_SAME_VERSION;
    }

    // Calculate the CRC32 for the OTA header and compare it with the checksum in the header.
    crc32 = wm_drv_crc32_reverse(&wm_ota_ops_ctx->ota_header, sizeof(wm_ota_header_t) - 0x04);
    if (wm_ota_ops_ctx->ota_header.hd_checksum != crc32) {
        wm_log_error("ota crc error,hd_checksum=0x%08X,crc32=0x%08X", (unsigned int)wm_ota_ops_ctx->ota_header.hd_checksum,
                     (unsigned int)crc32);
        return WM_ERR_OTA_CRC_ERROR;
    }

    return ret;
}

int wm_ota_ops_get_ota_partition(wm_ota_ops_ctx_t *wm_ota_ops_ctx)
{
    int ret                       = WM_ERR_SUCCESS;
    wm_partition_item_t partition = { 0 };

    if (wm_ota_ops_ctx == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    // Find the application partition and store its information in the context.
    ret = wm_partition_table_find("app", &partition);
    if (ret == WM_ERR_SUCCESS) {
        wm_ota_ops_ctx->app_addr = partition.offset;
        wm_ota_ops_ctx->app_size = partition.size;
        wm_log_debug("ota partition offset=0x%08x,size=0x%x", partition.offset, partition.size);
    } else {
        return WM_ERR_FAILED;
    }

    // Find the OTA partition and store its information in the context.
    ret = wm_partition_table_find("app_ota", &partition);
    if (ret == WM_ERR_SUCCESS) {
        wm_ota_ops_ctx->app_ota_addr = partition.offset;
        wm_ota_ops_ctx->app_ota_size = partition.size;
        wm_log_debug("app_ota partition offset=0x%08x,size=0x%x", partition.offset, partition.size);
    } else {
        return WM_ERR_FAILED;
    }

    return ret;
}

int wm_ota_ops_begin(wm_ota_ops_ctx_t *wm_ota_ops_ctx, size_t image_size)
{
    int ret                     = WM_ERR_SUCCESS;
    uint32_t aligned_erase_size = 0;
    wm_device_t *flash_dev      = NULL;

    if (wm_ota_ops_ctx == NULL || !wm_ota_ops_ctx->app_ota_addr) {
        return WM_ERR_INVALID_PARAM;
    }
    wm_ota_ops_ctx->wrote_addr = wm_ota_ops_ctx->app_ota_addr;

    flash_dev = wm_dt_get_device_by_name("iflash");

    // Erase the last 4K flash of the app_ota partition
    ret = wm_drv_flash_erase_region(
        flash_dev, wm_ota_ops_ctx->app_ota_addr + wm_ota_ops_ctx->app_ota_size - OTA_FLASH_SECTOR_SIZE, OTA_FLASH_SECTOR_SIZE);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    // Determine if a full erase is needed based on the image size and erase the flash if necessary.
    if (image_size != OTA_WITH_SEQUENTIAL_WRITES) {
        if ((image_size == 0) || (image_size == OTA_SIZE_UNKNOWN)) {
            ret = wm_drv_flash_erase_region(flash_dev, wm_ota_ops_ctx->app_ota_addr, wm_ota_ops_ctx->app_ota_size);
        } else {
            if (image_size > wm_ota_ops_ctx->app_ota_size) {
                return WM_ERR_OTA_FW_OVERFLOW;
            }
            aligned_erase_size = (image_size + OTA_FLASH_SECTOR_SIZE - 1) & ~(OTA_FLASH_SECTOR_SIZE - 1);
            ret                = wm_drv_flash_erase_region(flash_dev, wm_ota_ops_ctx->app_ota_addr, aligned_erase_size);
        }

        if (ret != WM_ERR_SUCCESS) {
            wm_ota_ops_ctx->need_erase = 1;
        } else {
            wm_ota_ops_ctx->need_erase = 0;
        }
    } else {
        wm_ota_ops_ctx->need_erase = 1;
    }

    return ret;
}

int wm_ota_ops_write(wm_ota_ops_ctx_t *wm_ota_ops_ctx, const uint8_t *data, uint32_t size)
{
    int ret                = WM_ERR_SUCCESS;
    uint32_t first_sector  = 0;
    uint32_t last_sector   = 0;
    wm_device_t *flash_dev = NULL;

    if (wm_ota_ops_ctx == NULL || data == NULL || size == 0 || !wm_ota_ops_ctx->app_ota_addr) {
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_ota_ops_ctx->wrote_addr < wm_ota_ops_ctx->app_ota_addr ||
        size > (wm_ota_ops_ctx->app_ota_addr + wm_ota_ops_ctx->app_ota_size - wm_ota_ops_ctx->wrote_addr)) {
        return WM_ERR_NOT_ALLOWED;
    }

    flash_dev = wm_dt_get_device_by_name("iflash");

    // If a sector erase is needed, calculate the sector range and perform the erase.
    if (wm_ota_ops_ctx->need_erase) {
        first_sector = wm_ota_ops_ctx->wrote_addr / OTA_FLASH_SECTOR_SIZE;
        last_sector  = (wm_ota_ops_ctx->wrote_addr + size) / OTA_FLASH_SECTOR_SIZE;

        if ((wm_ota_ops_ctx->wrote_addr % OTA_FLASH_SECTOR_SIZE) == 0) {
            if (size % OTA_FLASH_SECTOR_SIZE == 0) {
                ret = wm_drv_flash_erase_region(flash_dev, wm_ota_ops_ctx->wrote_addr,
                                                (last_sector - first_sector) * OTA_FLASH_SECTOR_SIZE);
            } else {
                ret = wm_drv_flash_erase_region(flash_dev, wm_ota_ops_ctx->wrote_addr,
                                                ((last_sector - first_sector) + 1) * OTA_FLASH_SECTOR_SIZE);
            }
        } else if (first_sector != last_sector) {
            ret = wm_drv_flash_erase_region(flash_dev, (first_sector + 1) * OTA_FLASH_SECTOR_SIZE,
                                            (last_sector - first_sector) * OTA_FLASH_SECTOR_SIZE);
        }
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    wm_device_t *crc_dev;
    crc_dev = wm_drv_crc_init("crc");
    // Verify the header of the OTA image if it's the first write operation.
    if (wm_ota_ops_ctx->app_ota_addr == wm_ota_ops_ctx->wrote_addr) {
        ret = wm_ota_ops_check_header(wm_ota_ops_ctx, data, size);
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }

        // Initialize CRC context and update it with the image data excluding the header.
        wm_drv_crc_cfg(crc_dev, &wm_ota_ops_ctx->crc_ctx, 0xFFFFFFFF, WM_GPSEC_CRC32, WM_GPSEC_CRC_OUT_IN_REVERSE);
        if (size - OTA_FW_HEADER_SIZE_IN_BIN > 0) {
            wm_drv_crc_update(crc_dev, &wm_ota_ops_ctx->crc_ctx, (unsigned char *)(data + OTA_FW_HEADER_SIZE_IN_BIN),
                              size - OTA_FW_HEADER_SIZE_IN_BIN);
        }
    } else {
        // Update CRC context with the current data chunk.
        wm_drv_crc_update(crc_dev, &wm_ota_ops_ctx->crc_ctx, (unsigned char *)data, size);
    }

    // Write the data to the flash memory.
    //TODO: need to replace to wm_drv_flash_write() in future as it has some abnormal currently
    ret = wm_drv_flash_write_with_erase(flash_dev, wm_ota_ops_ctx->wrote_addr, (void *)data, size);
    if (ret == WM_ERR_SUCCESS) {
        wm_ota_ops_ctx->wrote_addr += size; // Update the write address after successful write.
    }

    return ret;
}

int wm_ota_ops_end(wm_ota_ops_ctx_t *wm_ota_ops_ctx)
{
    int ret        = WM_ERR_SUCCESS;
    uint32_t crc32 = 0;
    wm_device_t *crc_dev;
    crc_dev = wm_drv_crc_init("crc");

    if (wm_ota_ops_ctx == NULL || wm_ota_ops_ctx->wrote_addr <= wm_ota_ops_ctx->app_ota_addr) {
        return WM_ERR_INVALID_PARAM;
    }

    // Finalize the CRC calculation and retrieve the result.
    wm_drv_crc_final(crc_dev, &wm_ota_ops_ctx->crc_ctx, &crc32);

    // Verify the calculated CRC32 against the original checksum in the OTA header.
    if (wm_ota_ops_ctx->ota_header.org_checksum != crc32) {
        wm_log_debug("ota crc error,org_checksum=0x%08X,crc32=0x%08X", wm_ota_ops_ctx->ota_header.org_checksum, crc32);
        return WM_ERR_OTA_CRC_ERROR;
    }

    return ret;
}

/**
 * @brief Reboot the system to apply the OTA update.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - Other error codes based on the underlying system reset operation
 */
int wm_ota_ops_reboot(void)
{
    int ret = WM_ERR_SUCCESS;

    // Trigger a system reset to reboot the device.
    wm_log_info("ota reboot, please wait for the restart...");
    wm_os_internal_time_delay_ms(20);

    wm_os_internal_set_critical();
    wm_hal_wdt_dev_t dev = { 0 };
    dev.register_base    = WM_WDG_BASE_ADDR;

    wm_set_reboot_reason(WM_REBOOT_REASON_OTA);
    wm_hal_wdt_set_counter_value(&dev, 0x100);

    return ret;
}

int wm_ota_ops_abort(wm_ota_ops_ctx_t *wm_ota_ops_ctx)
{
    int ret = WM_ERR_SUCCESS;

    if (wm_ota_ops_ctx == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    // Clear the OTA context structure to reset the state.
    memset(wm_ota_ops_ctx, 0, sizeof(wm_ota_ops_ctx_t));

    return ret;
}

int wm_ota_ops_get_version(char *version)
{
    int ret = WM_ERR_SUCCESS;

    if (version == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    strcpy(version, CONFIG_BUILD_VERSION);

    return ret;
}
