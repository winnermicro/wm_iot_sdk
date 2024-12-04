#ifndef __WM_LOG_H__
#define __WM_LOG_H__

#include "wmsdk_config.h"
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_LOG_Enumerations WM LOG Enumerations
 * @brief WinnerMicro LOG Enumerations
 */

/**
 * @addtogroup WM_LOG_Enumerations
 * @{
 */

/**
 * @enum wm_log_level_t
 * @brief Description of log level
 */
typedef enum {
    WM_LOG_LEVEL_NONE,    /**< No log output        */
    WM_LOG_LEVEL_ERROR,   /**< Critical errors      */
    WM_LOG_LEVEL_WARN,    /**< warnings             */
    WM_LOG_LEVEL_INFO,    /**< Information messages */
    WM_LOG_LEVEL_DEBUG,   /**< Debug informations   */
    WM_LOG_LEVEL_VERBOSE, /**< Extra information which is not necessary */
} wm_log_level_t;

/**
 * @}
 */

/**
 * @defgroup WM_LOG_MACROs WM LOG Macros
 * @brief WinnerMicro LOG Macros
 */

/**
 * @addtogroup WM_LOG_MACROs
 * @{
 */

/**
 * @brief Log colors define
 */
#if CONFIG_LOG_USE_COLOR
#define WM_LOG_COLOR_BLACK   "30"
#define WM_LOG_COLOR_RED     "31"
#define WM_LOG_COLOR_GREEN   "32"
#define WM_LOG_COLOR_YELLOW  "33"
#define WM_LOG_COLOR_BLUE    "34"
#define WM_LOG_COLOR_MAGENTA "35"
#define WM_LOG_COLOR_CYAN    "36"
#define WM_LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define WM_LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define WM_LOG_UNDERLINE     (COLOR) "\033[4;" COLOR "m"
#define WM_LOG_BLINK(COLOR)  "\033[1;" COLOR "m"
#define WM_LOG_COLOR_END     "\033[0m"
#define WM_LOG_COLOR_E       WM_LOG_COLOR(WM_LOG_COLOR_RED)
#define WM_LOG_COLOR_W       WM_LOG_COLOR(WM_LOG_COLOR_YELLOW)
#define WM_LOG_COLOR_I       WM_LOG_COLOR(WM_LOG_COLOR_CYAN)
#define WM_LOG_COLOR_D       WM_LOG_COLOR(WM_LOG_COLOR_GREEN)
#define WM_LOG_COLOR_V       WM_LOG_COLOR(WM_LOG_COLOR_BLUE)
#else //CONFIG_LOG_USE_COLOR
#define WM_LOG_COLOR_E
#define WM_LOG_COLOR_W
#define WM_LOG_COLOR_I
#define WM_LOG_COLOR_D
#define WM_LOG_COLOR_V
#define WM_LOG_COLOR_END
#endif //CONFIG_LOG_USE_COLOR

/**
 * @brief macro for formating the log line
 */
#define LOG_FMT(letter, fmt) WM_LOG_COLOR_##letter "[" #letter "] (%u) %s: " fmt WM_LOG_COLOR_END "\r\n"

/**
 * @brief macro for get time stamp
 */
#define WM_GET_MS(...)       wm_os_internal_get_time_ms()

/**
 * @brief Log tag name, used to distinguish different logs.
 *        To override the default LOG_TAG defined in wm_log. h, you need to define your own LOG_TAG
 *        before the. c or. cpp file includes wm_log.h
 * @warning Do not define LOG_TAG in the header file for external use, or the definition may be repeated.
 */
#if !defined(LOG_TAG)
#define LOG_TAG "NO_TAG"
#endif

/**
 * @brief macro for log wm_log_error
 */
#if CONFIG_LOG_DEFAULT_LEVEL
#define wm_log_error(fmt, ...) wm_log_printf(WM_LOG_LEVEL_ERROR, LOG_TAG, LOG_FMT(E, fmt), WM_GET_MS(), LOG_TAG, ##__VA_ARGS__)
#else
#define wm_log_error(fmt, ...) ((void)0);
#endif

/**
 * @brief macro for log wm_log_warn
 */
#if CONFIG_LOG_DEFAULT_LEVEL
#define wm_log_warn(fmt, ...) wm_log_printf(WM_LOG_LEVEL_WARN, LOG_TAG, LOG_FMT(W, fmt), WM_GET_MS(), LOG_TAG, ##__VA_ARGS__)
#else
#define wm_log_warn(fmt, ...) ((void)0);
#endif

/**
 * @brief macro for log wm_log_info
 */
#if CONFIG_LOG_DEFAULT_LEVEL
#define wm_log_info(fmt, ...) wm_log_printf(WM_LOG_LEVEL_INFO, LOG_TAG, LOG_FMT(I, fmt), WM_GET_MS(), LOG_TAG, ##__VA_ARGS__)
#else
#define wm_log_info(fmt, ...) ((void)0);
#endif

/**
 * @brief macro for log wm_log_debug
 */
#if CONFIG_LOG_DEFAULT_LEVEL
#define wm_log_debug(fmt, ...) wm_log_printf(WM_LOG_LEVEL_DEBUG, LOG_TAG, LOG_FMT(D, fmt), WM_GET_MS(), LOG_TAG, ##__VA_ARGS__)
#else
#define wm_log_debug(fmt, ...) ((void)0);
#endif

/**
 * @brief macro for log wm_log_verbose
 */
#if CONFIG_LOG_DEFAULT_LEVEL
#define wm_log_verbose(fmt, ...) \
    wm_log_printf(WM_LOG_LEVEL_VERBOSE, LOG_TAG, LOG_FMT(V, fmt), WM_GET_MS(), LOG_TAG, ##__VA_ARGS__)
#else
#define wm_log_verbose(fmt, ...) ((void)0);
#endif

/**
 * @brief macro for log raw data with output level speicified
 */
#if CONFIG_LOG_DEFAULT_LEVEL
#define wm_log_raw(level, fmt, ...) wm_log_printf(level, LOG_TAG, fmt, ##__VA_ARGS__)
#else
#define wm_log_raw(level, fmt, ...) ((void)0)
#endif

/**
 * @brief macro for log raw date with level WM_LOG_LEVEL_ERROR.
 * @note No TAG, timestamp, level, color and other information will be output.
 */
#define wm_log_raw_error(fmt, ...)   wm_log_raw(WM_LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

/**
 * @brief macro for log raw date with level WM_LOG_LEVEL_WARN
 */
#define wm_log_raw_warn(fmt, ...)    wm_log_raw(WM_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/**
 * @brief macro for log raw date with level WM_LOG_LEVEL_INFO
 */
#define wm_log_raw_info(fmt, ...)    wm_log_raw(WM_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/**
 * @brief macro for log raw date with level WM_LOG_LEVEL_DEBUG
 */
#define wm_log_raw_debug(fmt, ...)   wm_log_raw(WM_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

/**
 * @brief macro for log raw date with level WM_LOG_LEVEL_VERBOSE
 */
#define wm_log_raw_verbose(fmt, ...) wm_log_raw(WM_LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)

/**
 * @brief macro for dump data as hex format with output level speicified
 *
 * The dump log likes this:
 * [E] (565762) at_log: dump [dump_name]
 * 0x0000   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  ................
 * 0x0010   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  ................
 * 0x0020   20 21 22 23 24 25 26 27  28 29 2a 2b 2c 2d 2e 2f   !"# %&'()*+,-./
 * 0x0030   30 31 32 33 34 35 36 37  38 39 3a 3b 3c 3d 3e 3f  0123456789:;<=>?
 * 0x0040   40 41
 *
 * @param[in]  level  log level
 * @param[in]  name   the log item name, used to distinguish other dump data
 * @param[in]  width  the columns of the format data each line
 * @param[in]  buf  data buffer need show
 * @param[in]  size  data size
 *
 * @note The recommended width is 16
 */
#if CONFIG_LOG_DEFAULT_LEVEL
#define wm_log_dump(level, name, width, buf, size) wm_log_hex_dump(level, LOG_TAG, name, width, buf, size)
#else
#define wm_log_dump(level, name, width, buf, size) ((void)0)
#endif

/**
 * @brief macro for log dump binary data as hex format with level WM_LOG_LEVEL_ERROR
 */
#define wm_log_dump_error(name, buf, size)   wm_log_dump(WM_LOG_LEVEL_ERROR, name, 16, buf, size)

/**
 * @brief macro for log dump binary data as hex format with level WM_LOG_LEVEL_WARN
 */
#define wm_log_dump_warn(name, buf, size)    wm_log_dump(WM_LOG_LEVEL_WARN, name, 16, buf, size)

/**
 * @brief macro for log dump binary data as hex format with level WM_LOG_LEVEL_INFO
 */
#define wm_log_dump_info(name, buf, size)    wm_log_dump(WM_LOG_LEVEL_INFO, name, 16, buf, size)

/**
 * @brief macro for log dump binary data as hex format with level WM_LOG_LEVEL_DEBUG
 */
#define wm_log_dump_debug(name, buf, size)   wm_log_dump(WM_LOG_LEVEL_DEBUG, name, 16, buf, size)

/**
 * @brief macro for log dump binary data as hex format with level WM_LOG_LEVEL_VERBOSE
 */
#define wm_log_dump_verbose(name, buf, size) wm_log_dump(WM_LOG_LEVEL_VERBOSE, name, 16, buf, size)

/**
 * @}
 */

extern uint32_t wm_os_internal_get_time_ms(void);

/**
 * @brief  custom vprintf type, the protoype is same to standard vprintf in libc
 *
 */
typedef int (*wm_log_vprintf_t)(const char *, va_list);

/**
 * @defgroup WM_LOG_APIs WM LOG APIs
 * @brief WinnerMicro LOG APIs
 */

/**
 * @addtogroup WM_LOG_APIs
 * @{
 */

/**
 * @brief  log printf function
 *
 * @param[in]  level  log level
 * @param[in]  tag   the log tag
 * @param[in]  format format
 *
 * @return
 *       size of format size
 * @note The maximum log length is 16K. If you need to output more data, you can use wm_log_dump_xxx.
 */
int wm_log_printf(wm_log_level_t level, const char *tag, const char *format,
                  ...) /** @cond */ __attribute__((format(printf, 3, 4))) /** @endcond */;

/**
 * @brief          Output log information in hexadecimal and display its ascii code string
 *
 * @param[in]      level     log level
 * @param[in]      tag       log TAG
 * @param[in]      name      name for hex object, it will show on log header
 * @param[in]      width     hex number for every line, such as: 16, 32
 * @param[in]      data      hex buffer
 * @param[in]      data_len  buffer size
 * @return
 *    - WM_ERR_SUCCESS : success
 *    - WM_ERR_INVALID_PARAM : invalid param
 */
int wm_log_hex_dump(wm_log_level_t level, const char *tag, const char *name, uint8_t width, const void *data, size_t data_len);

/**
 * @brief          format output log by arguments list
 *
 * @param[in]      level     log level
 * @param[in]      tag       log TAG
 * @param[in]      format    format parameter
 * @param[in]      args     Variable length parameter list
 * @return
 *    - size of format size
 */
int wm_log_vprintf(wm_log_level_t level, const char *tag, const char *format, va_list args);

/**
 * @brief  Set log level
 *
 * @param[in] level   log level
 *
 * @return
 *    - WM_ERR_SUCCESS : success
 */
int wm_log_set_level(wm_log_level_t level);

/**
 * @brief  Get log level
 *
 * @return  log level
 */
wm_log_level_t wm_log_get_level(void);

/**
 * @brief  set custom vprintf
 *
 * @param[in]  user_vprintf user defined vprintf function
 *
 * @return
 *         - WM_ERR_SUCCESS
 */
int wm_log_set_vprintf(wm_log_vprintf_t user_vprintf);

/**
 * @brief  printf directly, output log to uart hardware fifo directly
 *
 * @param[in]  fmt printf format
 *
 * @return
 *         - WM_ERR_SUCCESS
 * @note The output content is not limited by LOG level, it likes printf.
 *  It is recommended to use only in scenarios where interrupts have been
 *  turned off or the OS cannot be scheduled.
 */
int wm_printf_direct(const char *fmt, ...) /** @cond */ __attribute__((format(printf, 1, 2))) /** @endcond */;

/**
  * @brief  Initialize log module
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  * @note This API must be called before all other log API can be called
  */
int wm_log_init(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* end of __WM_LOG_H__ */
