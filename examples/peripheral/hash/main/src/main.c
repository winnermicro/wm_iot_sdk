#define LOG_TAG "hash_example"
#include "wm_log.h"

#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wm_drv_hash.h"

static int md5_hard_demo(void)
{
    uint8_t md5hash[16];
    uint8_t *md5in1     = (uint8_t *)"abacdefghgjklmno1234";
    uint8_t *md5in2     = (uint8_t *)"1234567890123456abcd";
    uint8_t md5real[16] = {
        0xD6, 0xEB, 0xD1, 0x1B, 0x0D, 0x42, 0x84, 0xE8, 0x52, 0x74, 0xA6, 0xA8, 0x2A, 0x3B, 0x2A, 0x12,
    };

    wm_device_t *dev;
    wm_drv_hash_cfg_t ctx;

    /*initialize hash*/
    dev = wm_drv_hash_init("hash");
    if (dev == NULL) {
        wm_log_error("init fail.");
        return 0;
    }

    wm_drv_md5_start(dev, &ctx);
    wm_drv_md5_update(dev, &ctx, md5in1, 20);
    wm_drv_md5_update(dev, &ctx, md5in2, 20);
    wm_drv_md5_final(dev, &ctx, md5hash);

    wm_drv_hash_deinit(dev);

    if (memcmp(md5hash, md5real, 16)) {
        wm_log_info("md5 test fail");
    } else {
        wm_log_info("md5 test success");
    }

    return 0;
}

static int sha1_hard_demo(void)
{
    uint8_t sha1hash[20];
    uint8_t *sha1in1     = (uint8_t *)"abacdefghgjklmno1234";
    uint8_t *sha1in2     = (uint8_t *)"1234567890123456abcd";
    uint8_t sha1real[20] = {
        0xA9, 0xED, 0x29, 0x56, 0x83, 0x6A, 0x59, 0x22, 0x27, 0x70, 0x87, 0x52, 0xDC, 0xB5, 0x1D, 0x2B, 0xBA, 0x93, 0x40, 0xA6,
    };

    wm_device_t *dev;
    wm_drv_hash_cfg_t ctx;

    /*initialize hash*/
    dev = wm_drv_hash_init("hash");
    if (dev == NULL) {
        wm_log_error("init fail.");
        return 0;
    }

    wm_drv_sha1_start(dev, &ctx);
    wm_drv_sha1_update(dev, &ctx, sha1in1, 20);
    wm_drv_sha1_update(dev, &ctx, sha1in2, 20);
    wm_drv_sha1_final(dev, &ctx, sha1hash);

    wm_drv_hash_deinit(dev);

    if (memcmp(sha1hash, sha1real, 20)) {
        wm_log_info("sha1 test fail");
    } else {
        wm_log_info("sha1 test success");
    }

    return 0;
}

int main(void)
{
    wm_log_info("hash demo.");

    md5_hard_demo();

    sha1_hard_demo();
    while (1) {
        wm_log_info("hash demo.");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}
