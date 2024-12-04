#include <string.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_dt.h"
#include "wm_drv_flash.h"
#include "wm_drv_hash.h"
#include "wm_partition_table.h"

#define LOG_TAG "ptb"
#include "wm_log.h"

#define WM_PARTITION_TABLE_MAGIC_NUMBER 0xA1F2F39F

#define WM_PARTITION_OPT_MD5_CHKSUM     0x1
#define WM_PARTITION_OPT_SIGNATURE      0x2

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t count;
    uint32_t option;
} wm_partition_header_t;

/*

                                partition table
----------------------------------------------------------------------------
|                                magic(4byte)                              |
|--------------------------------------------------------------------------|
|              version(2byte)          |             count(2byte)          |
|--------------------------------------------------------------------------|
|                                option(4byte)                             |
|--------------------------------------------------------------------------|
|   name(16byte)   |   offset(4byte)   |   size(4byte)   |   flag(4byte)   |
|--------------------------------------------------------------------------|
|                              md5-checksum(16byte)                        |
|--------------------------------------------------------------------------|
|                         ......(option: signature...)                     |
|--------------------------------------------------------------------------|

*/

static bool g_partition_table_verified  = false;
static uint32_t g_partition_table_addr  = 0x0;
static uint32_t g_partition_table_count = 0;

int wm_partition_table_init(void)
{
    int ret   = WM_ERR_FAILED;
    char *buf = NULL;
    uint8_t md5_checksum[16];
    wm_device_t *flash_dev = NULL;
    wm_drv_flash_info_t fls_info;
    wm_partition_header_t partition_header = { 0 };

    flash_dev = wm_dt_get_device_by_name("iflash");
    if (flash_dev) {
        memset(&fls_info, 0, sizeof(fls_info));
        ret = wm_drv_flash_get_device_info(flash_dev, &fls_info);
    }

    if (!ret) {
        g_partition_table_addr = CONFIG_WM_PARTITION_TABLE_OFFSET;
        wm_log_debug("partition table base 0x%x", g_partition_table_addr);

        memset(&partition_header, 0, sizeof(partition_header));
        ret = wm_drv_flash_read(flash_dev, g_partition_table_addr, (uint8_t *)&partition_header, sizeof(partition_header));
    }

    if (!ret) {
        if (WM_PARTITION_TABLE_MAGIC_NUMBER != partition_header.magic) {
            wm_log_error("partition table magic 0x%x error", partition_header.magic);
            return WM_ERR_FAILED;
        }

        //TODO: check version

        if ((sizeof(partition_header) + partition_header.count * sizeof(wm_partition_item_t) + sizeof(md5_checksum)) >
            fls_info.sector_size) {
            wm_log_error("partition table count too many %u", partition_header.count);
            return WM_ERR_FAILED;
        }

        buf = wm_os_internal_malloc(sizeof(partition_header) + partition_header.count * sizeof(wm_partition_item_t) +
                                    sizeof(md5_checksum));
        if (!buf)
            return WM_ERR_NO_MEM;

        ret = wm_drv_flash_read(flash_dev, g_partition_table_addr + sizeof(partition_header),
                                (uint8_t *)(buf + sizeof(partition_header)),
                                partition_header.count * sizeof(wm_partition_item_t) + sizeof(md5_checksum));
    }

    if (!ret) {
        memcpy(buf, &partition_header, sizeof(partition_header));

        ret = wm_drv_md5(buf, sizeof(partition_header) + partition_header.count * sizeof(wm_partition_item_t), md5_checksum);

        if (ret || memcmp(buf + sizeof(partition_header) + partition_header.count * sizeof(wm_partition_item_t), md5_checksum,
                          sizeof(md5_checksum))) {
            wm_log_error("partition table md5 checksum error");
            if (buf)
                wm_os_internal_free(buf);
            return WM_ERR_FAILED;
        }

        // Check if any partition exceeds flash size
        wm_partition_item_t *partition_item = (wm_partition_item_t *)(buf + sizeof(partition_header));
        for (uint16_t i = 0; i < partition_header.count; i++) {
            if (partition_item->offset + partition_item->size > fls_info.flash_size) {
                wm_log_error("partition %s exceeds flash size", partition_item->name);
                if (buf) {
                    wm_os_internal_free(buf);
                    buf = NULL;
                }
                return WM_ERR_FAILED;
            }
            partition_item++;
        }

        g_partition_table_count    = partition_header.count;
        g_partition_table_verified = true;
    }

    if (buf)
        wm_os_internal_free(buf);

    return ret;
}

void wm_partition_table_print(void)
{
    uint8_t *buf = NULL;
    int ret      = WM_ERR_FAILED;
    wm_partition_item_t *partition_item;
    wm_device_t *flash_dev = NULL;

    flash_dev = wm_dt_get_device_by_name("iflash");

    if (!g_partition_table_verified)
        return;

    buf = wm_os_internal_malloc(g_partition_table_count * sizeof(wm_partition_item_t));
    if (!buf)
        return;

    ret = wm_drv_flash_read(flash_dev, g_partition_table_addr + sizeof(wm_partition_header_t), buf,
                            g_partition_table_count * sizeof(wm_partition_item_t));

    if (!ret) {
        partition_item = (wm_partition_item_t *)buf;
        wm_log_raw(WM_LOG_LEVEL_ERROR, "partition table info:\r\n");
        wm_log_raw(WM_LOG_LEVEL_ERROR, "  %-16s %6s %8s %10s\r\n", "name", "offset", "size", "flag");
        for (uint8_t i = 0; i < g_partition_table_count; i++) {
            wm_log_raw(WM_LOG_LEVEL_ERROR, "  %-16s 0x%08x 0x%08x 0x%02x\r\n", partition_item->name,
                       (unsigned int)partition_item->offset, (unsigned int)partition_item->size,
                       (unsigned int)partition_item->flag);
            partition_item++;
        }
    }

    wm_os_internal_free(buf);
}

int wm_partition_table_find(const char *name, wm_partition_item_t *partition)
{
    int ret                                 = WM_ERR_FAILED;
    wm_partition_item_t *partition_item     = NULL;
    wm_partition_item_t *partition_item_src = NULL;
    wm_device_t *flash_dev                  = NULL;

    flash_dev = wm_dt_get_device_by_name("iflash");

    if (!name || !partition)
        return WM_ERR_INVALID_PARAM;

    if (!g_partition_table_verified)
        return WM_ERR_NO_INITED;

    partition_item = wm_os_internal_malloc(g_partition_table_count * sizeof(wm_partition_item_t));
    if (!partition_item)
        return WM_ERR_NO_MEM;

    partition_item_src = partition_item;
    ret = wm_drv_flash_read(flash_dev, g_partition_table_addr + sizeof(wm_partition_header_t), (uint8_t *)partition_item,
                            g_partition_table_count * sizeof(wm_partition_item_t));

    if (!ret) {
        ret = WM_ERR_NOT_FOUND;
        for (uint8_t i = 0; i < g_partition_table_count; i++) {
            if (!strcmp(name, partition_item->name)) {
                memcpy(partition, partition_item, sizeof(wm_partition_item_t));
                ret = WM_ERR_SUCCESS;
                break;
            }
            partition_item++;
        }
    }

    if (partition_item_src)
        wm_os_internal_free(partition_item_src);

    return ret;
}
