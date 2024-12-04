#include "wmsdk_config.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "wm_log.h"
#include "wm_osal.h"

/*runtime global log level*/
static uint8_t g_level = CONFIG_LOG_DEFAULT_LEVEL;

/*vprintf for log , default use the standard vprintf in libc, can be overwritten by users*/
static wm_log_vprintf_t g_log_vprintf = vprintf;

int wm_log_vprintf(wm_log_level_t level, const char *tag, const char *format, va_list args)
{
    int ret = 0;
    (void)tag;

    if (format && level <= g_level && g_log_vprintf) {
        ret = (g_log_vprintf)(format, args);
    }
    return ret;
}

int wm_log_printf(wm_log_level_t level, const char *tag, const char *format, ...)
{
    int len = 0;
    va_list list;
    va_start(list, format);
    if (format) {
        len = wm_log_vprintf(level, tag, format, list);
    }
    va_end(list);
    return len;
}

int wm_log_hex_dump(wm_log_level_t level, const char *tag, const char *name, uint8_t width, const void *data, size_t data_len)
{
    if (!(level >= 0 && level < WM_LOG_LEVEL_VERBOSE && tag && name && width > 0 && width <= 32 && data && data_len > 0)) {
        return WM_ERR_INVALID_PARAM;
    }

    if (level > g_level) {
        return WM_ERR_SUCCESS;
    }

    const char *ptr_line;
    const char level_char[] = { 'N', 'E', 'W', 'I', 'D', 'V' };
    char line_buf[10 + 9 + 1 + width * 4]; /*10 bytes address, 9 bytes space, 4 * witdh character*/
    char *p_line;
    int line_size;
    uint32_t count = 0;
    int32_t len    = data_len;

    wm_log_printf(level, tag, "[%c] (%u) %s: dump [%s]\r\n", level_char[level], wm_os_internal_get_time_ms(), tag, name);

    while (len > 0) {
        if (len > width) {
            line_size = width;
        } else {
            line_size = len;
        }

        /*current line*/
        ptr_line = data;
        p_line   = line_buf;

        /*add address*/
        p_line += sprintf(p_line, "0x%04x ", count);

        /*dump hex*/
        for (int i = 0; i < width; i++) {
            if ((i & 7) == 0) {
                p_line += sprintf(p_line, " ");
            }
            if (i < line_size) {
                p_line += sprintf(p_line, " %02x", (uint8_t)ptr_line[i]);
            } else {
                p_line += sprintf(p_line, "   ");
            }
        }

        /*dump characters*/
        p_line += sprintf(p_line, "  ");
        for (int i = 0; i < line_size; i++) {
            if (isprint((int)ptr_line[i])) {
                p_line += sprintf(p_line, "%c", ptr_line[i]);
            } else {
                p_line += sprintf(p_line, ".");
            }
        }

        /*output line*/
        wm_log_printf(level, name, "%s\r\n", line_buf);

        data = (char *)data + line_size;
        len -= line_size;
        count += line_size;
    }

    return WM_ERR_SUCCESS;
}

int wm_log_set_vprintf(wm_log_vprintf_t user_vprintf)
{
    g_log_vprintf = user_vprintf;
    return WM_ERR_SUCCESS;
}

int wm_log_set_level(wm_log_level_t level)
{
    if (level >= WM_LOG_LEVEL_NONE && level <= WM_LOG_LEVEL_VERBOSE) {
        g_level = level;
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

wm_log_level_t wm_log_get_level(void)
{
    return (wm_log_level_t)g_level;
}

int wm_log_init(void)
{
    return WM_ERR_SUCCESS;
}
