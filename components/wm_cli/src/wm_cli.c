#include <string.h>
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_task_config.h"
#include "wm_osal.h"
#include "wm_drv_uart.h"
#include "wm_component.h"
#include "letter_shell.h"

#define LOG_TAG "cli"
#include "wm_log.h"

static wm_device_t *g_wm_cli_uart;
static SHELL_TypeDef g_wm_cli_shell;

static char wm_cli_get_char(void)
{
    char ch;

    while (wm_drv_uart_read(g_wm_cli_uart, (uint8_t *)&ch, 1, (uint32_t)-1) != 1) {
        wm_os_internal_time_delay(1);
    }

    return ch;
}

static int wm_cli_uart_init(void)
{
    int ret = WM_ERR_SUCCESS;

    wm_device_t *dev = wm_dt_get_device_by_name(CONFIG_CLI_UART_DEVICE);
    if (!dev || (dev && (WM_DEV_ST_INITED != dev->state))) {
        g_wm_cli_uart = wm_drv_uart_init(
            CONFIG_CLI_UART_DEVICE,
            (CONFIG_CLI_LINE_SIZE + 1) > WM_DRV_UART_RX_MIN_BUF_SIZE ? (CONFIG_CLI_LINE_SIZE + 1) : WM_DRV_UART_RX_MIN_BUF_SIZE,
            (CONFIG_CLI_LINE_SIZE + 1) > WM_DRV_UART_TX_MIN_BUF_SIZE ? (CONFIG_CLI_LINE_SIZE + 1) :
                                                                       WM_DRV_UART_TX_MIN_BUF_SIZE);
        if (!g_wm_cli_uart) {
            wm_log_error("cli %s init failed.", CONFIG_CLI_UART_DEVICE);
            return WM_ERR_FAILED;
        }
    } else if (dev && (WM_DEV_ST_INITED == dev->state)) {
        g_wm_cli_uart = dev;
    }

    return ret;
}

static void wm_cli_write(const char ch)
{
    wm_drv_uart_write(g_wm_cli_uart, (uint8_t *)&ch, 1);
}

static void wm_cli_task(void *data)
{
    char ch;

    while (1) {
        ch = wm_cli_get_char();

        shellInput(&g_wm_cli_shell, ch);
    }
}

int wm_cli_printf(const char *fmt, ...)
{
    size_t len;
    va_list args;
    char buf[CONFIG_CLI_LINE_SIZE];

    if (!fmt)
        return WM_ERR_INVALID_PARAM;

    if (!g_wm_cli_uart)
        return WM_ERR_NO_INITED;

    va_start(args, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    if (len < CONFIG_CLI_LINE_SIZE) {
        len = wm_drv_uart_write(g_wm_cli_uart, (uint8_t *)buf, len);
    } else {
        char *ptr = wm_os_internal_malloc(len + 2);
        if (ptr) {
            len = vsnprintf(ptr, len + 1, fmt, args);
            len = wm_drv_uart_write(g_wm_cli_uart, (uint8_t *)ptr, len);
            wm_os_internal_free(ptr);
        } else {
            len = 0;
        }
    }
    va_end(args);

    return len;
}

int wm_cli_init(void)
{
    wm_os_status_t err;

    err = wm_cli_uart_init();
    if (err)
        return err;

    memset(&g_wm_cli_shell, 0, sizeof(g_wm_cli_shell));
    g_wm_cli_shell.write = wm_cli_write;
    shellInit(&g_wm_cli_shell);

    err = wm_os_internal_task_create(NULL, "wm_cli", wm_cli_task, NULL, WM_TASK_CLI_STACK, WM_TASK_CLI_PRIO, 0);
    if (WM_OS_STATUS_SUCCESS != err) {
        wm_log_error("cli init task failed");
    }

    return WM_OS_STATUS_SUCCESS == err ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}
WM_COMPONEN_INIT_3(wm_cli_init);
