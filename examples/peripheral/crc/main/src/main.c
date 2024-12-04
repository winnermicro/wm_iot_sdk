#define LOG_TAG "crc_example"
#include "wm_log.h"

#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wm_drv_crc.h"

static int crc_hard_demo(void)
{
    uint32_t crckey = 0xFFFFFFFF;
    uint8_t *in     = NULL;
    int i;
    uint32_t crcvalue = 0;
    wm_drv_crc_cfg_t ctx;
    wm_device_t *dev;

    /*initialize crc*/
    dev = wm_drv_crc_init("crc");
    if (dev == NULL) {
        wm_log_error("init fail.");
        goto OUT;
    }

    in = malloc(1024);
    if (in == NULL) {
        wm_log_error("malloc err");
        goto OUT;
    }

    for (i = 0; i < 1024; i++) {
        in[i] = i; //rand();
    }

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC8, WM_GPSEC_CRC_REVERSE_NONE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC8 normal value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC8, WM_GPSEC_CRC_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC8 INPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC8, WM_GPSEC_CRC_OUT_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC8 OUTPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC8, WM_GPSEC_CRC_OUT_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC8 OUT_IN_REVERSE value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_MODBUS, WM_GPSEC_CRC_REVERSE_NONE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_MODBUS normal value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_MODBUS, WM_GPSEC_CRC_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_MODBUS INPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_MODBUS, WM_GPSEC_CRC_OUT_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_MODBUS OUTPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_MODBUS, WM_GPSEC_CRC_OUT_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_MODBUS OUT_IN_REVERSE value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_CCITT, WM_GPSEC_CRC_REVERSE_NONE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_CCITT normal value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_CCITT, WM_GPSEC_CRC_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_CCITT INPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_CCITT, WM_GPSEC_CRC_OUT_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_CCITT OUTPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC16_CCITT, WM_GPSEC_CRC_OUT_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC16_CCITT OUT_IN_REVERSE value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC32, WM_GPSEC_CRC_REVERSE_NONE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC32 normal value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC32, WM_GPSEC_CRC_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC32 INPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC32, WM_GPSEC_CRC_OUT_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC32 OUTPUT_REFLECT value:0x%08X", crcvalue);

    wm_drv_crc_cfg(dev, &ctx, crckey, WM_GPSEC_CRC32, WM_GPSEC_CRC_OUT_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_update(dev, &ctx, in, 1024);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    wm_log_info("WM_GPSEC_CRC32 OUT_IN_REVERSE value:0x%08X", crcvalue);

OUT:
    if (in != NULL)
        free(in);
    wm_drv_crc_deinit(dev);
    return 0;
}

int main(void)
{
    wm_log_info("crc demo.");

    crc_hard_demo();

    while (1) {
        wm_log_info("crc demo.");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}
