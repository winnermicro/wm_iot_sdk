#include "wmsdk_config.h"
#include <stdio.h>
#include <csi_config.h>
#include "wm_utils.h"
#include "wm_dt.h"
#include "wm_hal_uart.h"
#include "wm_drv_uart.h"
#include "wm_dt_hw.h"
#include "wm_osal.h"

#define WM_PRINTF_MAX_STR_LEN (16 * 1024)

extern wm_device_t *g_log_uart;

/*C++ global object section*/
extern int __dtor_end__;
extern int __ctor_end__;
extern int __ctor_start__;
typedef void (*func_ptr)(void);

/**
 * @brief C++ Global Object Initialization
 */
__attribute__((weak)) void cxx_system_init(void)
{
    func_ptr *p;
    for (p = (func_ptr *)((unsigned long)&__ctor_end__ - sizeof(unsigned long)); p >= (func_ptr *)&__ctor_start__; p--) {
        (*p)();
    }
}

/**
 * @brief uart output, for libc printf
 */
static void wm_put_data(int is_direct,const char *data, int len)
{
    if (!g_log_uart) {
        return;
    }

    if (g_log_uart->state != WM_DEV_ST_INITED || is_direct) {
        wm_hal_uart_dev_t dev = {
            .reg = (wm_uart_reg_t *)(((wm_dt_hw_uart_t *)g_log_uart->hw)->reg_base),
        };

        /*write to uart fifo directly*/
        wm_hal_uart_tx_polling(&dev, (const uint8_t *)data, len);

    } else {
        /*use uart driver to write*/
        wm_drv_uart_write(g_log_uart, (const uint8_t *)data, len);
    }
}

/**
 * @brief vprintf by direct or not
 */
static int wm_vprintf_by_type(int is_direct,const char *fmt, va_list args)
{
    int len;
    char buf[CONFIG_LOG_FORMAT_BUF_SIZE];

    len = vsnprintf(buf, sizeof(buf), fmt, args);
    if (len < sizeof(buf)) {
        wm_put_data(is_direct,buf, len);
    } else {
        if(len <= WM_PRINTF_MAX_STR_LEN){
            char *ptr = malloc(len + 2);
            if (ptr) {
                len = vsnprintf(ptr, len + 1, fmt, args);
                wm_put_data(is_direct,ptr, len);
                free(ptr);
            } else {
                len = 0;
            }
        }
    }
    return len;
}

/**
 * @brief overwrite the function vprintf in libc
 */
int vprintf(const char *fmt, va_list args)
{
    return wm_vprintf_by_type(0,fmt,args);
}

/**
 * @brief overwrite the printf function in libc
 */
int printf(const char *fmt, ...)
{
    int len = 0;
    va_list list;
    va_start(list, fmt);
    if (fmt) {
        len = wm_vprintf_by_type(0,fmt, list);
    }
    va_end(list);
    return len;
}

/**
 * @brief overwrite the printf function in libc
 */
int wm_printf_direct(const char *fmt, ...)
{
    int len = 0;
    va_list list;
    va_start(list, fmt);
    if (fmt) {
        len = wm_vprintf_by_type(1,fmt, list);
    }
    va_end(list);
    return len;
}

/**
 * @brief overwrite the fprintf function in libc
 */
int fprintf(FILE *fp, const char *fmt, ...)
{
    int len = 0;

    (void)fp;

    va_list list;
    va_start(list, fmt);
    if (fmt) {
        len = vprintf(fmt, list);
    }
    va_end(list);
    return len;
}

/**
 * @brief overwrite the puts function in libc
 */
int puts(const char *str)
{
    int len = strlen(str);
    wm_put_data(0,str, len);
    return len;
}

/**
 * @brief overwrite the fputs function in libc
 */
int fputs(const char *str, FILE *fp)
{
    int len;

    (void)fp;
    len = strlen(str);
    wm_put_data(0,str, len);

    return len;
}

/**
 * @brief overwrite the putchar function in libc
 */
int putchar(int ch)
{
    char c = (char)ch;
    wm_put_data(0,&c, 1);
    return ch;
}

/**
 * @brief overwrite the fputc function in libc
 */
int fputc(int ch, FILE *fp)
{
    (void)fp;
    char byte = (char)ch;
    wm_put_data(0,&byte, 1);

    return ch;
}

/**
 * @brief overwrite the fgetc function in libc
 */
int fgetc(FILE *fp)
{
    (void)fp;
    return 0;
}

/**
 * @brief overwrite the __assert_fail function in libc, use for assert
 */
__attribute__((weak)) void __assert_fail(const char *file, int line, const char *func, const char *failedexpr)
{
    wm_printf_direct("assertion \"%s\" failed: file \"%s\", line %d%s%s\r\n", failedexpr, file, line, func ? ", function: " : "",
           func ? func : "");
    asm("trap 0");
    while (1) {
        ;
    }
}
