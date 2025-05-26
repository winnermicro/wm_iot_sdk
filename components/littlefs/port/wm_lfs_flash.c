/**
 * @file wm_lfs_flash.c
 *
 * @brief LittleFS Flash Block Device Implementation
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "wm_lfs_flash.h"
#include "wm_error.h"
#include "wm_partition_table.h"
#include "wm_drv_flash.h"
#if CONFIG_LITTLEFS_THREADSAFE_ENABLE
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#endif
#include "wm_log.h"

/**
 * @brief Configuration structure for flash block device
 */
typedef struct {
    /** Flash device name */
    char *dev_name;

    /** Start address in flash */
    uint32_t start_addr;

    /** Total size of flash area */
    uint32_t total_size;

    /** Minimum size of a read operation in bytes */
    lfs_size_t read_size;

    /** Minimum size of a program operation in bytes */
    lfs_size_t prog_size;

    /** Size of an erase operation in bytes */
    lfs_size_t erase_size;

    /** Number of erase blocks on the device */
    lfs_size_t block_count;
} lfs_flash_config_t;

/**
 * @brief State structure for flash block device
 */
typedef struct {
    /** Flash device handle */
    wm_device_t *dev;

    /** Flash configuration */
    const lfs_flash_config_t *cfg;
#if CONFIG_LITTLEFS_THREADSAFE_ENABLE
    /** Mutex for thread safety */
    SemaphoreHandle_t mutex;
#endif
} lfs_flash_t;

static int lfs_flash_create(const struct lfs_config *cfg, const lfs_flash_config_t *fcfg)
{
    wm_log_debug("lfs_flash_create(%p {.context=%p, "
                 ".read=%p, .prog=%p, .erase=%p, .sync=%p}, "
                 "%p {.dev_name=%s, .start_addr=0x%x, "
                 ".total_size=0x%x, .read_size=%" PRIu32 ", "
                 ".prog_size=%" PRIu32 ", .erase_size=%" PRIu32 ", "
                 ".block_count=%" PRIu32 "})",
                 (void *)cfg, cfg->context, (void *)(uintptr_t)cfg->read, (void *)(uintptr_t)cfg->prog,
                 (void *)(uintptr_t)cfg->erase, (void *)(uintptr_t)cfg->sync, (void *)fcfg, fcfg->dev_name, fcfg->start_addr,
                 fcfg->total_size, fcfg->read_size, fcfg->prog_size, fcfg->erase_size, fcfg->block_count);

    lfs_flash_t *flash = cfg->context;
    flash->cfg         = fcfg;

#if CONFIG_LITTLEFS_THREADSAFE_ENABLE
    // Create mutex for thread safety
    flash->mutex = xSemaphoreCreateMutex();
    if (flash->mutex == NULL) {
        wm_log_error("Failed to create flash mutex");
        return LFS_ERR_NOMEM;
    }
#endif

    // Initialize flash device
    flash->dev = wm_dt_get_device_by_name(fcfg->dev_name);
    if (!flash->dev || flash->dev->state != WM_DEV_ST_INITED) {
        flash->dev = wm_drv_flash_init(fcfg->dev_name);
        if (flash->dev == NULL) {
#if CONFIG_LITTLEFS_THREADSAFE_ENABLE
            vSemaphoreDelete(flash->mutex); // Clean up mutex on failure
#endif
            return LFS_ERR_IO;
        }
    }

    wm_log_debug("lfs_flash_create -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}

static int lfs_flash_destroy(const struct lfs_config *cfg)
{
    wm_log_debug("lfs_flash_destroy(%p)", (void *)cfg);

#if CONFIG_LITTLEFS_THREADSAFE_ENABLE
    lfs_flash_t *flash = cfg->context;
    if (flash->mutex != NULL) {
        vSemaphoreDelete(flash->mutex);
        flash->mutex = NULL;
    }
#endif

    wm_log_debug("lfs_flash_destroy -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}

static int lfs_flash_read(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    lfs_flash_t *flash = cfg->context;
    uint32_t addr      = flash->cfg->start_addr + block * flash->cfg->erase_size + off;

    wm_log_debug("lfs_flash_read(%p, 0x%" PRIx32 ", %" PRIu32 ", %p, %" PRIu32 ")", (void *)cfg, block, off, buffer, size);

    // Check if read is valid
    if (block >= flash->cfg->block_count) {
        wm_log_error("Invalid block number for read: block=0x%" PRIx32 ", block_count=%" PRIu32, block,
                     flash->cfg->block_count);
        return LFS_ERR_INVAL;
    }
    if (off % flash->cfg->read_size != 0) {
        wm_log_error("Invalid read offset: offset=%" PRIu32 ", read_size=%" PRIu32, off, flash->cfg->read_size);
        return LFS_ERR_INVAL;
    }
    if (size % flash->cfg->read_size != 0) {
        wm_log_error("Invalid read size: size=%" PRIu32 ", read_size=%" PRIu32, size, flash->cfg->read_size);
        return LFS_ERR_INVAL;
    }
    if (off + size > flash->cfg->erase_size) {
        wm_log_error("Read operation exceeds block boundary: offset=%" PRIu32 ", size=%" PRIu32 ", erase_size=%" PRIu32, off,
                     size, flash->cfg->erase_size);
        return LFS_ERR_INVAL;
    }

    int ret = wm_drv_flash_read(flash->dev, addr, buffer, size);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Flash read failed at addr=0x%x, size=%" PRIu32 ", ret=%d", addr, size, ret);
        return LFS_ERR_IO;
    }

    wm_log_debug("lfs_flash_read -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}

static int lfs_flash_prog(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    lfs_flash_t *flash = cfg->context;
    uint32_t addr      = flash->cfg->start_addr + block * flash->cfg->erase_size + off;

    wm_log_debug("lfs_flash_prog(%p, 0x%" PRIx32 ", %" PRIu32 ", %p, %" PRIu32 ")", (void *)cfg, block, off, buffer, size);

    // Check if write is valid
    if (block >= flash->cfg->block_count) {
        wm_log_error("Invalid block number for write: block=0x%" PRIx32 ", block_count=%" PRIu32, block,
                     flash->cfg->block_count);
        return LFS_ERR_INVAL;
    }
    if (off % flash->cfg->prog_size != 0) {
        wm_log_error("Invalid write offset: offset=%" PRIu32 ", prog_size=%" PRIu32, off, flash->cfg->prog_size);
        return LFS_ERR_INVAL;
    }
    if (size % flash->cfg->prog_size != 0) {
        wm_log_error("Invalid write size: size=%" PRIu32 ", prog_size=%" PRIu32, size, flash->cfg->prog_size);
        return LFS_ERR_INVAL;
    }
    if (off + size > flash->cfg->erase_size) {
        wm_log_error("Write operation exceeds block boundary: offset=%" PRIu32 ", size=%" PRIu32 ", erase_size=%" PRIu32, off,
                     size, flash->cfg->erase_size);
        return LFS_ERR_INVAL;
    }

    // Cast away const for the flash driver API
    int ret = wm_drv_flash_write(flash->dev, addr, (uint8_t *)buffer, size);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Flash write failed at addr=0x%x, size=%" PRIu32 ", ret=%d", addr, size, ret);
        return LFS_ERR_IO;
    }

    wm_log_debug("lfs_flash_prog -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}

static int lfs_flash_erase(const struct lfs_config *cfg, lfs_block_t block)
{
    lfs_flash_t *flash = cfg->context;
    uint32_t addr      = flash->cfg->start_addr + block * flash->cfg->erase_size;

    wm_log_debug("lfs_flash_erase(%p, 0x%" PRIx32 ")", (void *)cfg, block);

    // Check if erase is valid
    if (block >= flash->cfg->block_count) {
        wm_log_error("Invalid block number for erase: block=0x%" PRIx32 ", block_count=%" PRIu32, block,
                     flash->cfg->block_count);
        return LFS_ERR_INVAL;
    }

    int ret = wm_drv_flash_erase_region(flash->dev, addr, flash->cfg->erase_size);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Flash erase failed at addr=0x%x, size=%" PRIu32 ", ret=%d", addr, flash->cfg->erase_size, ret);
        return LFS_ERR_IO;
    }

    wm_log_debug("lfs_flash_erase -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}

static int lfs_flash_sync(const struct lfs_config *cfg)
{
    wm_log_debug("lfs_flash_sync(%p)", (void *)cfg);
    // Nothing to do for now
    wm_log_debug("lfs_flash_sync -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}

#if CONFIG_LITTLEFS_THREADSAFE_ENABLE
static int lfs_flash_lock(const struct lfs_config *cfg)
{
    wm_log_debug("lfs_flash_lock(%p)", (void *)cfg);

    lfs_flash_t *flash = cfg->context;
    if (xSemaphoreTake(flash->mutex, portMAX_DELAY) != pdTRUE) {
        wm_log_error("Failed to take flash mutex");
        return LFS_ERR_IO;
    }

    wm_log_debug("lfs_flash_lock -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}

static int lfs_flash_unlock(const struct lfs_config *cfg)
{
    wm_log_debug("lfs_flash_unlock(%p)", (void *)cfg);

    lfs_flash_t *flash = cfg->context;
    if (xSemaphoreGive(flash->mutex) != pdTRUE) {
        wm_log_error("Failed to give flash mutex");
        return LFS_ERR_IO;
    }

    wm_log_debug("lfs_flash_unlock -> %d", LFS_ERR_OK);
    return LFS_ERR_OK;
}
#endif

struct lfs_config *lfs_flash_init(wm_lfs_flash_type_t flash_type)
{
    lfs_flash_t *flash       = NULL;
    struct lfs_config *cfg   = NULL;
    lfs_flash_config_t *fcfg = NULL;

    // Allocate memory for flash context
    flash = malloc(sizeof(lfs_flash_t));
    if (!flash) {
        return NULL;
    }
    memset(flash, 0, sizeof(lfs_flash_t));

    // Allocate memory for lfs config
    cfg = malloc(sizeof(struct lfs_config));
    if (!cfg) {
        free(flash);
        return NULL;
    }
    memset(cfg, 0, sizeof(struct lfs_config));

    // Allocate memory for flash config
    fcfg = malloc(sizeof(lfs_flash_config_t));
    if (!fcfg) {
        free(flash);
        free(cfg);
        return NULL;
    }
    memset(fcfg, 0, sizeof(lfs_flash_config_t));

    // Setup flash config based on flash type
#if CONFIG_LITTLEFS_INTERNAL_FLASH_DISK_ENABLE
    wm_partition_item_t partition;

    if (flash_type == WM_LFS_FLASH_TYPE_INTERNAL) {
        fcfg->dev_name = "iflash";
        // Get partition info from partition table
        if (wm_partition_table_find(CONFIG_LITTLEFS_INTERNAL_FLASH_PARTITION_NAME, &partition) != WM_ERR_SUCCESS) {
            wm_log_error("Failed to find partition %s", CONFIG_LITTLEFS_INTERNAL_FLASH_PARTITION_NAME);
            goto cleanup;
        }
        fcfg->start_addr = partition.offset;
        fcfg->total_size = partition.size;
    }
#endif
#if CONFIG_LITTLEFS_EXTERNAL_FLASH_DISK_ENABLE
    if (flash_type == WM_LFS_FLASH_TYPE_EXTERNAL) {
        fcfg->dev_name   = CONFIG_LITTLEFS_EXTERNAL_FLASH_DEVICE_NAME;
        fcfg->start_addr = CONFIG_LITTLEFS_EXTERNAL_FLASH_START_ADDRESS;
        fcfg->total_size = CONFIG_LITTLEFS_EXTERNAL_FLASH_SIZE;
    }
#endif

    // Setup common flash config parameters
    fcfg->read_size   = CONFIG_LITTLEFS_READ_SIZE;
    fcfg->prog_size   = CONFIG_LITTLEFS_PROG_SIZE;
    fcfg->erase_size  = CONFIG_LITTLEFS_ERASE_SIZE;
    fcfg->block_count = fcfg->total_size / fcfg->erase_size;

    // Setup lfs config
    cfg->context = flash;
    cfg->read    = lfs_flash_read;
    cfg->prog    = lfs_flash_prog;
    cfg->erase   = lfs_flash_erase;
    cfg->sync    = lfs_flash_sync;
#if CONFIG_LITTLEFS_THREADSAFE_ENABLE
    cfg->lock   = lfs_flash_lock;
    cfg->unlock = lfs_flash_unlock;
#endif
    cfg->read_size        = fcfg->read_size;
    cfg->prog_size        = fcfg->prog_size;
    cfg->block_size       = fcfg->erase_size;
    cfg->block_count      = fcfg->block_count;
    cfg->block_cycles     = 500;
    cfg->cache_size       = fcfg->prog_size;
    cfg->lookahead_size   = 16;
    cfg->read_buffer      = NULL;
    cfg->prog_buffer      = NULL;
    cfg->lookahead_buffer = NULL;
    cfg->name_max         = 0;
    cfg->file_max         = 0;
    cfg->attr_max         = 0;

    // Initialize flash device using lfs_flash_create
    if (lfs_flash_create(cfg, fcfg) != LFS_ERR_OK) {
        wm_log_error("Failed to create flash device");
        goto cleanup;
    }

    return cfg;

cleanup:
    if (flash) {
        free(flash);
    }
    if (cfg) {
        free(cfg);
    }
    if (fcfg) {
        free(fcfg);
    }
    return NULL;
}

int lfs_flash_deinit(const struct lfs_config *cfg)
{
    if (!cfg) {
        wm_log_error("Invalid lfs config pointer");
        return LFS_ERR_INVAL;
    }

    lfs_flash_t *flash = cfg->context;
    if (!flash) {
        wm_log_error("Invalid flash context in lfs config");
        free((void *)cfg);
        return LFS_ERR_INVAL;
    }

    const lfs_flash_config_t *fcfg = flash->cfg;
    if (!fcfg) {
        wm_log_error("Invalid flash config in flash context");
        free(flash);
        free((void *)cfg);
        return LFS_ERR_INVAL;
    }

    // Free flash resources
    lfs_flash_destroy(cfg);

    // Free allocated memory in reverse order of allocation
    free((void *)fcfg);
    free(flash);
    free((void *)cfg);

    return LFS_ERR_OK;
}