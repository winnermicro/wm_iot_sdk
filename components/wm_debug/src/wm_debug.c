#include <string.h>

#include "wmsdk_config.h"
#include "wm_utils.h"
#include "wm_reg_op.h"
#include "wm_debug.h"
#include "wm_hal_uart.h"
#include "wm_dt_hw.h"
#include "wm_dt.h"

#if CONFIG_BUILD_TYPE_W800 || CONFIG_BUILD_TYPE_W801 || CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || \
    CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W805 || CONFIG_BUILD_TYPE_W806
/* Store reboot reason by RAM's Last Word */
#define WM_REBOOT_PARA_ADDRESS (0x20047EFC)
#endif

void wm_set_reboot_reason(wm_reboot_reason_t reason)
{
#if CONFIG_BUILD_TYPE_W800 || CONFIG_BUILD_TYPE_W801 || CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || \
    CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W805 || CONFIG_BUILD_TYPE_W806
    wm_reboot_para_t para = { 0 };
    para.val              = WM_REG32_READ(WM_REBOOT_PARA_ADDRESS);
    para.reboot_reason    = (uint8_t)reason;
    WM_REG32_WRITE(WM_REBOOT_PARA_ADDRESS, para.val);
#endif
}

wm_reboot_reason_t wm_get_reboot_reason(void)
{
    wm_reboot_para_t para = { 0 };

    para.val = WM_REG32_READ(WM_REBOOT_PARA_ADDRESS);

    if (para.reboot_reason >= WM_REBOOT_REASON_MAX) {
        return WM_REBOOT_REASON_POWER_ON;
    } else {
        return para.reboot_reason;
    }
}

void wm_set_boot_log_level(wm_boot_log_level_t level)
{
#if CONFIG_BUILD_TYPE_W800 || CONFIG_BUILD_TYPE_W801 || CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || \
    CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W805 || CONFIG_BUILD_TYPE_W806
    wm_reboot_para_t para = { 0 };
    para.val              = WM_REG32_READ(WM_REBOOT_PARA_ADDRESS);
    para.boot_log_level   = level;
    WM_REG32_WRITE(WM_REBOOT_PARA_ADDRESS, para.val);
#endif
}

static void uart_ramdump(wm_hal_uart_dev_t *hal_uart, unsigned int *regs, unsigned int regs_len)
{
    /**
     * In Exception, interrupt is disabled. the OS can no longer be scheduled,
     * Only the polling interface can be used. driver api use OS and interrupt,
     * it will not work, so use hal polling api.
     */
    wm_hal_uart_tx_polling(hal_uart, (uint8_t *)regs, regs_len);
#if CONFIG_BUILD_TYPE_W800 || CONFIG_BUILD_TYPE_W801 || CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || \
    CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W805 || CONFIG_BUILD_TYPE_W806
    /**
      * w800 addr layout:
      * sram : 0x20000000 ~ 0x20047FFF
      * psram: 0x30000000 ~ 0x307FFFFF
      * reg  : 0x40000000 ~ 0x4003BFFF
      */
    wm_hal_uart_tx_polling(hal_uart, (uint8_t *)(uint32_t *)0x20000000, 0x48000);
#endif
}

void wm_output_ramdump(unsigned int *regs, unsigned int regs_len)
{
    char buf[16]  = { 0 };
    uint8_t index = 0;

    extern wm_device_t *g_log_uart;
    wm_hal_uart_dev_t hal_uart = { .reg = (wm_uart_reg_t *)(((wm_dt_hw_uart_t *)g_log_uart->hw)->reg_base) };

    while (1) {
        /**
         * In Exception, interrupt is disabled. the OS can no longer be scheduled,
         * Only the polling interface can be used. driver api use OS and interrupt,
         * it will not work, so use hal polling api.
         */
        buf[index] = (char)wm_hal_uart_getchar(&hal_uart);
        index++;

        if (index >= sizeof(buf)) {
            buf[sizeof(buf) - 1] = '\0';
            index                = 0;
        }

        if (strstr(buf, "ramdump")) {
            uart_ramdump(&hal_uart, regs, regs_len);
            memset(buf, 0, sizeof(buf));
        } else if (strstr(buf, "reboot")) {
            //wm_set_reboot_reason(WM_REBOOT_REASON_EXCEPTION);
            wm_system_reboot();
            wm_set_reboot_reason(WM_REBOOT_REASON_EXCEPTION);
            memset(buf, 0, sizeof(buf));
            while (1)
                ;
        }
    }
}
