/**
 * @file wm_utils.c
 *
 * @brief Utils Module
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
#include <ctype.h>
#include <time.h>
#include <string.h>
#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_debug.h"
#include "wm_osal.h"
#include "wm_regs_base.h"
#include "wm_hal_wdt.h"
#include "wm_drv_rtc.h"

#if CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS
#if CONFIG_COMPONENT_EVENT_ENABLED
#include "wm_event.h"
#endif
#endif

#include "wm_utils.h"

#if CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS

#if CONFIG_COMPONENT_EVENT_ENABLED
WM_EVENT_DEFINE_GROUP(WM_UTILS_EV);
#endif

extern void vTaskGetRunTimeStats(char *pcWriteBuffer);

static int wm_get_task_usage(char *task_name)
{
    char *buf        = NULL;
    char *percentstr = NULL;
    uint8_t percent  = 0;

    if (task_name) {
        buf = wm_os_internal_malloc(2048);
        if (NULL == buf)
            return WM_ERR_FAILED;
        memset(buf, 0x00, 2048);
        vTaskGetRunTimeStats(buf);
        percentstr = strstr(buf, task_name);
        if (percentstr) {
            percentstr = strchr(percentstr, '%');
            if (percentstr) {
                *percentstr = '\0';
                percentstr -= 3;
                percent = atoi(percentstr);
                wm_os_internal_free(buf);
                if (percent > 100)
                    return WM_ERR_FAILED;
                return percent;
            }
        }
        wm_os_internal_free(buf);
        buf = NULL;
    }

    return WM_ERR_FAILED;
}

static void wm_report_cpu_useage(void *ptmr, void *parg)
{
#if CONFIG_COMPONENT_EVENT_ENABLED
    wm_utils_event_t event = { 0 };
#endif
    int percent = 0;

    percent = wm_get_task_usage("IDLE");
    if (WM_ERR_FAILED != percent) {
#if CONFIG_COMPONENT_EVENT_ENABLED
        event.cpu_usage = (100 - percent);
        wm_event_send(WM_UTILS_EV, WM_UTILS_EVENT_CPU_USAGE, &event, sizeof(event));
#endif // CONFIG_COMPONENT_EVENT_ENABLED
    }
}
#endif // CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS

/*
*********************************************************************************************************
*                                       cpu usage
*
* Description: This function to set cpu usage duration
* Arguments  :
*
* @param time_ms : The time of being counted, if 0, the statistical time will be calculated from the last time it was not 0
*
* Returns    : WM_ERR_SUCCESS or WM_ERR_FAILED
*********************************************************************************************************
*/
int wm_set_cpu_usage_duration(uint32_t time_ms)
{
#if CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS
    int err                     = 0;
    static wm_os_timer_t *timer = NULL;

    if (time_ms) {
        if (!timer) {
            if (WM_OS_STATUS_SUCCESS !=
                (err = wm_os_internal_timer_create_ms(&timer, wm_report_cpu_useage, NULL, time_ms, false, "cpuusage"))) {
                return WM_ERR_FAILED;
            }
        }

        wm_os_internal_set_critical();
        wm_os_internal_timer_change_ms(timer, time_ms);
        extern void uxTaskClearAllRunTime(void);
        uxTaskClearAllRunTime();
        wm_os_internal_release_critical();
    } else {
        wm_report_cpu_useage(NULL, NULL);
    }

    return WM_ERR_SUCCESS;
#else
    return WM_ERR_FAILED;
#endif //CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS
}

int wm_atohex(char ch)
{
    int hex;

    hex = -1;

    if ((ch >= '0') && (ch <= '9')) {
        hex = ch - '0';
    } else if ((ch >= 'a') && (ch <= 'f')) {
        hex = ch - 'a' + 0xa;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        hex = ch - 'A' + 0xa;
    }

    return hex;
}

int wm_hex2byte(const char hex[2])
{
    int a, b;

    a = wm_atohex(*hex);
    if (a < 0) {
        return -1;
    }

    b = wm_atohex(*(hex + 1));
    if (b < 0) {
        return -1;
    }

    return (a << 4) | b;
}

int wm_hex_str_to_array(const char *str, uint8_t array[], int array_len)
{
    int hex;
    uint8_t tmp;
    uint8_t *des;

    des = array;

    while (array_len > 0) {
        if (':' == *str || '-' == *str) {
            str++;
            continue;
        }

        hex = wm_atohex(*str++);
        if (hex < 0) {
            return -1;
        } else {
            tmp = (hex << 4) & 0xf0;
        }

        hex = wm_atohex(*str++);
        if (hex < 0) {
            return -1;
        } else {
            tmp = tmp | (hex & 0x0f);
        }

        *des++ = (uint8_t)tmp;

        array_len--;
    }

    return ((*str == 0) ? 0 : -1);
}

int wm_atoi(const char *s, int *err)
{
    int n   = 0;
    int neg = 0;

    while (isspace((unsigned char)*s) != 0) {
        s++;
    }
    switch (*s) {
        case '-':
            neg = 1;
            s++;
            break; /* artifact to quiet coverity warning */
        case '+':
            s++;
            break;
        default:
            /* Add an empty default with break, this is a defensive programming.
             * Static analysis tool won't raise a violation if default is empty,
             * but has that comment.
             */
            break;
    }

    if (isdigit((unsigned char)*s) == 0) {
        if (err) {
            *err = 1;
        }
        return 0;
    }

    /* Compute n as a negative number to avoid overflow on INT_MIN */
    while (isdigit((unsigned char)*s) != 0) {
        n = 10 * n - (*s++ - '0');
    }

    if (err) {
        *err = 0;
    }

    return neg ? n : -n;
}

void *memrchr(const void *s, int c, size_t n)
{
    register const char *t    = s;
    register const char *last = 0;
    int i;

    t = t + n;
    for (i = n; i; --i) {
        if (*t == c) {
            last = t;
            break;
        }
        t--;
    }
    return (void *)last; /* man, what an utterly b0rken prototype */
}

char *strdup(const char *s)
{
    size_t len;
    char *d;

    len = strlen(s);
    d   = malloc(len + 1);

    if (d == NULL) {
        return NULL;
    }

    memcpy(d, s, len);
    d[len] = '\0';

    return d;
}

ATTRIBUTE_WEAK char *strcasestr(const char *str1, const char *str2)
{
    char *cp = (char *)str1;
    char *s1, *s2;

    if (!*str2)
        return (char *)str1;

    while (*cp) {
        s1 = cp;
        s2 = (char *)str2;

        while (*s1 && *s2 && !(tolower((int)*s1) - tolower((int)*s2)))
            s1++, s2++;
        if (!*s2)
            return cp;
        cp++;
    }

    return NULL;
}

ATTRIBUTE_WEAK int strcasecmp(const char *s1, const char *s2)
{
    char a, b;
    while (*s1 && *s2) {
        a = *s1++;
        b = *s2++;

        if (a == b) {
            continue;
        }

        if (a >= 'a' && a <= 'z') {
            a -= 'a' - 'A';
        }
        if (b >= 'a' && b <= 'z') {
            b -= 'a' - 'A';
        }

        if (a != b) {
            return 1;
        }
    }

    return 0;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    const unsigned char *u1 = (const unsigned char *)s1;
    const unsigned char *u2 = (const unsigned char *)s2;
    int result;

    for (; n != 0; n--) {
        result = tolower(*u1) - tolower(*u2);
        if (result)
            return result;
        if (*u1 == 0)
            return 0;
        u1++;
        u2++;
    }
    return 0;
}

size_t strlcpy(char *dest, const char *src, size_t len)
{
    size_t ret = strlen(src);

    if (len != 0) {
        if (ret < len) {
            strcpy(dest, src);
        } else {
            strncpy(dest, src, len - 1);
            dest[len - 1] = 0;
        }
    }
    return ret;
}

size_t strnlen(const char *s, size_t maxlen)
{
    const char *sc;
    for (sc = s; maxlen != 0 && *sc != '\0'; maxlen--, ++sc)
        ;
    return sc - s;
}

/** because RTC not supporting ms, adding s_time_offset_s compensates time */
static time_t s_time_offset_s = 0;

static int timestamp_conversion_to_tm(const time_t *tim_p, struct tm *tm);

/** because time needs to be obtained from RTC, so redefine gettimeofday */
#if __GNUC__ >= 13
int gettimeofday(struct timeval *tv, void *tz)
#else
int gettimeofday(struct timeval *tv, struct timezone *tz)
#endif
{
    uint32_t current_ms = wm_os_internal_get_time_ms();

#if CONFIG_COMPONENT_DRIVER_RTC_ENABLED
    if (!s_time_offset_s) {
        struct tm tm = { 0 };
        wm_device_t *rtc_dev;

        (void)tz;

        if (tv == NULL) {
            return -1;
        }

        rtc_dev = wm_drv_rtc_init("rtc");
        if (rtc_dev == NULL) {
            rtc_dev = wm_dt_get_device_by_name("rtc");
        }

        if (rtc_dev == NULL) {
            return -1;
        }

        if (wm_drv_rtc_get_time(rtc_dev, &tm) != WM_ERR_SUCCESS) {
            return -1;
        }

        s_time_offset_s = mktime(&tm) - current_ms / 1000;
    }

#endif

    tv->tv_sec  = s_time_offset_s + (current_ms / 1000);
    tv->tv_usec = (current_ms % 1000) * 1000;

    return 0;
}

#if __GNUC__ >= 13
int settimeofday(struct timeval *tv, void *tz)
#else
int settimeofday(const struct timeval *tv, const struct timezone *tz)
#endif
{
#if CONFIG_COMPONENT_DRIVER_RTC_ENABLED
    struct tm tm_ptr;
    time_t time;
    wm_device_t *rtc_dev;

    (void)tz;

    if (tv == NULL) {
        return -1;
    }

    time = tv->tv_sec;

    if (timestamp_conversion_to_tm(&time, &tm_ptr)) {
        return -1;
    }

    rtc_dev = wm_drv_rtc_init("rtc");
    if (rtc_dev == NULL) {
        rtc_dev = wm_dt_get_device_by_name("rtc");
    }

    if (rtc_dev == NULL) {
        return -1;
    }

    if (wm_drv_rtc_set_time(rtc_dev, &tm_ptr) != WM_ERR_SUCCESS) {
        return -1;
    }

#endif

    s_time_offset_s = (tv->tv_sec) - wm_os_internal_get_time_ms() / 1000;

    return 0;
}

time_t time(time_t *t)
{
    time_t __t;
    struct timeval tv;

    if (gettimeofday(&tv, NULL)) {
        return 0;
    }

    __t = tv.tv_sec;

    if (t) {
        *t = __t;
    }

    return __t;
}

/* seconds per day */
#define SPD 24 * 60 * 60
/* days per month -- nonleap! */
static const short __spm[13] = {
    0,
    (31),
    (31 + 28),
    (31 + 28 + 31),
    (31 + 28 + 31 + 30),
    (31 + 28 + 31 + 30 + 31),
    (31 + 28 + 31 + 30 + 31 + 30),
    (31 + 28 + 31 + 30 + 31 + 30 + 31),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31),
};
static int __isleap(int year)
{
    /* every fourth year is a leap year except for century years that are
     * not divisible by 400. */
    /*  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); */
    return (!(year % 4) && ((year % 100) || !(year % 400)));
}
struct tm *gmtime_r(const time_t *timep, struct tm *r)
{
    time_t i;
    time_t work = *timep % (SPD);

    if (timep == NULL || r == NULL) {
        return NULL;
    }

    memset(r, 0, sizeof(struct tm));

    r->tm_sec = work % 60;
    work /= 60;
    r->tm_min  = work % 60;
    r->tm_hour = work / 60;
    work       = *timep / (SPD);
    r->tm_wday = (4 + work) % 7;
    for (i = 1970;; ++i) {
        time_t k = __isleap(i) ? 366 : 365;
        if (work >= k)
            work -= k;
        else
            break;
    }
    r->tm_year = i - 1900;
    r->tm_yday = work;

    r->tm_mday = 1;
    if (__isleap(i) && (work > 58)) {
        if (work == 59)
            r->tm_mday = 2; /* 29.2. */
        work -= 1;
    }

    for (i = 11; i && (__spm[i] > work); --i)
        ;

    r->tm_mon = i;
    r->tm_mday += work - __spm[i];

    r->tm_isdst = 0;
    return r;
}
struct tm *localtime_r(const time_t *t, struct tm *r)
{
    time_t local_tz;

    local_tz = *t + 8 * 3600;
    return gmtime_r(&local_tz, r);
}

/** save time zone parameters*/
static char *s_event_tz = NULL;

char *getenv(const char *name)
{
    if (strcmp(name, "TZ") == 0) {
        return s_event_tz;
    }
    return NULL;
}

static const uint8_t crc8_tbl[256] = {
    0x00, 0x91, 0xe3, 0x72, 0x07, 0x96, 0xe4, 0x75, 0x0e, 0x9f, 0xed, 0x7c, 0x09, 0x98, 0xea, 0x7b, 0x1c, 0x8d, 0xff, 0x6e,
    0x1b, 0x8a, 0xf8, 0x69, 0x12, 0x83, 0xf1, 0x60, 0x15, 0x84, 0xf6, 0x67, 0x38, 0xa9, 0xdb, 0x4a, 0x3f, 0xae, 0xdc, 0x4d,
    0x36, 0xa7, 0xd5, 0x44, 0x31, 0xa0, 0xd2, 0x43, 0x24, 0xb5, 0xc7, 0x56, 0x23, 0xb2, 0xc0, 0x51, 0x2a, 0xbb, 0xc9, 0x58,
    0x2d, 0xbc, 0xce, 0x5f, 0x70, 0xe1, 0x93, 0x02, 0x77, 0xe6, 0x94, 0x05, 0x7e, 0xef, 0x9d, 0x0c, 0x79, 0xe8, 0x9a, 0x0b,
    0x6c, 0xfd, 0x8f, 0x1e, 0x6b, 0xfa, 0x88, 0x19, 0x62, 0xf3, 0x81, 0x10, 0x65, 0xf4, 0x86, 0x17, 0x48, 0xd9, 0xab, 0x3a,
    0x4f, 0xde, 0xac, 0x3d, 0x46, 0xd7, 0xa5, 0x34, 0x41, 0xd0, 0xa2, 0x33, 0x54, 0xc5, 0xb7, 0x26, 0x53, 0xc2, 0xb0, 0x21,
    0x5a, 0xcb, 0xb9, 0x28, 0x5d, 0xcc, 0xbe, 0x2f, 0xe0, 0x71, 0x03, 0x92, 0xe7, 0x76, 0x04, 0x95, 0xee, 0x7f, 0x0d, 0x9c,
    0xe9, 0x78, 0x0a, 0x9b, 0xfc, 0x6d, 0x1f, 0x8e, 0xfb, 0x6a, 0x18, 0x89, 0xf2, 0x63, 0x11, 0x80, 0xf5, 0x64, 0x16, 0x87,
    0xd8, 0x49, 0x3b, 0xaa, 0xdf, 0x4e, 0x3c, 0xad, 0xd6, 0x47, 0x35, 0xa4, 0xd1, 0x40, 0x32, 0xa3, 0xc4, 0x55, 0x27, 0xb6,
    0xc3, 0x52, 0x20, 0xb1, 0xca, 0x5b, 0x29, 0xb8, 0xcd, 0x5c, 0x2e, 0xbf, 0x90, 0x01, 0x73, 0xe2, 0x97, 0x06, 0x74, 0xe5,
    0x9e, 0x0f, 0x7d, 0xec, 0x99, 0x08, 0x7a, 0xeb, 0x8c, 0x1d, 0x6f, 0xfe, 0x8b, 0x1a, 0x68, 0xf9, 0x82, 0x13, 0x61, 0xf0,
    0x85, 0x14, 0x66, 0xf7, 0xa8, 0x39, 0x4b, 0xda, 0xaf, 0x3e, 0x4c, 0xdd, 0xa6, 0x37, 0x45, 0xd4, 0xa1, 0x30, 0x42, 0xd3,
    0xb4, 0x25, 0x57, 0xc6, 0xb3, 0x22, 0x50, 0xc1, 0xba, 0x2b, 0x59, 0xc8, 0xbd, 0x2c, 0x5e, 0xcf
};

uint8_t wm_get_crc8(uint8_t *ptr, uint32_t len)
{
    uint8_t crc8;
    uint8_t data;

    crc8 = 0;
    while (len-- != 0) {
        data = *ptr++;
        crc8 = crc8_tbl[crc8 ^ data];
    }

    return crc8;
}

bool wm_isdigitstr(const char *str)
{
    int len = strlen(str);
    int i   = 0;

    for (i = 0; i < len; i++) {
        if (!(isdigit(str[i]))) {
            return false;
        }
    }

    return true;
}

bool wm_isdecimalstr(const char *str)
{
    int len          = strlen(str);
    int decimalCount = 0;

    if (len == 0 || str[0] == '.') {
        return false;
    }

    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)str[i]) && str[i] != '.') {
            return false;
        }
        if (str[i] == '.') {
            decimalCount++;
        }
        if (decimalCount > 1) {
            return false;
        }
    }

    if (decimalCount == 0) {
        return false;
    }

    return true;
}

bool wm_isstr(const char *str)
{
    int len = strlen(str);

    if (len >= 2) {
        if (str[0] == '"' && str[len - 1] == '"') {
            return true;
        }
    }

    return false;
}

bool wm_ishexstr(const char *str)
{
    if (str == NULL) {
        return false;
    }

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    while (*str != '\0') {
        if (!isxdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }

    return true;
}

bool wm_isnumstr(const char *str)
{
    if (wm_isdigitstr(str) || wm_ishexstr(str)) {
        return true;
    } else {
        return false;
    }
}

int wm_str2int(const char *str, int *value)
{
    int ret = WM_ERR_FAILED;

    if (str) {
        if (wm_isnumstr(str) == false) {
            goto exit;
        }
        *value = strtol(str, NULL, 0);

        ret = WM_ERR_SUCCESS;
    }

exit:
    return ret;
}
void wm_remove_quotation_mark(int argc, char **argv)
{
    char *tmp;
    int len;

    for (int i = 0; i < argc; i++) {
        tmp = argv[i];
        len = strlen(argv[i]);
        if (tmp[0] == '"' || tmp[len - 1] == '"') {
            memmove(tmp, tmp + 1, strlen(tmp) - 1);
            tmp[len - 2] = '\0';
        }
    }
}

void wm_system_reboot(void)
{
    wm_os_internal_set_critical();
    wm_hal_wdt_dev_t dev = { 0 };
    dev.register_base    = WM_WDG_BASE_ADDR;

    wm_set_reboot_reason(WM_REBOOT_REASON_ACTIVE);
    wm_hal_wdt_set_counter_value(&dev, 0x100);
    //while(1);
}

int wm_data_align_parser(uint8_t *addr, int32_t size, wm_data_align_t table[3])
{
    uint32_t unaligned_part = (uintptr_t)addr & 0x03;
    uint32_t remaining_size = size;
    int id                  = 0;

    if (addr == NULL || size <= 0 || table == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    unaligned_part = (unaligned_part == 0) ? 0 : (4 - unaligned_part);

    memset(table, 0x00, sizeof(wm_data_align_t) * 3);

    // the unalign start address , or total size<4, should transfer by cpu
    if (unaligned_part > 0 || (size < 4)) {
        table[id].flag = WM_DATA_ALIGN_TYPE_CPU;
        table[id].addr = addr;

        if (((unaligned_part > 0) && (size < 4 + unaligned_part)) || (size < 4)) {
            //case1: addr=0x1000, size=3  or  case2:addr=0x1001, size=6
            table[id].size = size;
        } else {
            table[id].size = unaligned_part;
        }
        remaining_size -= table[id].size;
        id++;
    }

    if (remaining_size >= 4) {
        // the align start address
        table[id].flag = WM_DATA_ALIGN_TYPE_DMA;
        table[id].addr = addr + unaligned_part;
        table[id].size = remaining_size & ~0x03;
        remaining_size -= table[id].size;
        id++;
    }

    // the unalign size
    if (remaining_size > 0 && id >= 1) {
        table[id].flag = WM_DATA_ALIGN_TYPE_CPU;
        table[id].addr = table[id - 1].addr + table[id - 1].size;
        table[id].size = remaining_size;
    }

    return WM_ERR_SUCCESS;
}

/** This function is implemented incorrectly in the C-SKY library */
time_t mktime(struct tm *timeptr)
{
    int mon  = timeptr->tm_mon;
    int year = timeptr->tm_year + 1900;

    if (timeptr->tm_mon >= 12) {
        year += mon / 12;
        mon = mon % 12;
    } else if (timeptr->tm_mon < 0) {
        mon  = 0 - mon;
        year = year - (mon / 12) - 1;
        mon  = 12 - (mon % 12);
    }

    mon++;
    /* 1..12 -> 11,12,1..10 */
    if (0 >= (mon -= 2)) {
        mon += 12; /* Puts Feb last since it has leap day */
        year -= 1;
    }

    return ((((int)(year / 4 - year / 100 + year / 400 + 367 * mon / 12 + timeptr->tm_mday) + year * 365 - 719499) * 24 +
             timeptr->tm_hour /* now have hours */
             ) * 60 +
            timeptr->tm_min /* now have minutes */
            ) * 60 +
           timeptr->tm_sec; /* finally seconds */
}

static int timestamp_conversion_to_tm(const time_t *tim_p, struct tm *tm)
{
#define WM_DAY_SEC               (24 * 60 * 60)     /* secs in a day */
#define WM_YEAR_SEC              (365 * WM_DAY_SEC) /* secs in a year */
#define WM_BASE_DOW              4                  /* 01-01-70 was a Thursday */
#define WM_LEAP_YEAR_ADJUST      17                 /* Leap years 1900 - 1970 */

/*
 * Macro to determine if a given year, expressed as the number of years since
 * 1900, is a leap year.
 */
#define WM_IS_LEAP_YEAR(y)       (((y % 4 == 0) && (y % 100 != 0)) || ((y + 1900) % 400 == 0))

/*
 * Number of leap years from 1970 up to, but not including, the specified year
 * (expressed as the number of years since 1900).
 */
#define WM_ELAPSED_LEAP_YEARS(y) (((y - 1) / 4) - ((y - 1) / 100) + ((y + 299) / 400) - WM_LEAP_YEAR_ADJUST)

    int _lpdays[]   = { -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
    int _days[]     = { -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364 };
    int caltim      = 0;
    int tmptim      = 0;
    time_t time_bak = 0;
    int islpyr      = 0; /* is-current-year-a-leap-year flag */
    int *mdays;          /* pointer to days or lpdays */

    memset(tm, 0x00, sizeof(struct tm));

    if (tim_p == NULL || *tim_p < 0 || *tim_p > 0x7fffd27f) {
        return -1; /* Time range exceeds the limit */
    }

    time_bak = *tim_p;

    caltim = time_bak;
    /* Determine the years since 1900. Start by ignoring leap years. */
    tmptim = (caltim / WM_YEAR_SEC) + 70;
    caltim -= (tmptim - 70) * WM_YEAR_SEC;
    /* Correct for elapsed leap years */
    caltim -= (WM_ELAPSED_LEAP_YEARS(tmptim) * WM_DAY_SEC);
    /*
     * If we have underflowed the __time64_t range (i.e., if caltim < 0),
     * back up one year, adjusting the correction if necessary.
     */
    if (caltim < 0) {
        caltim += WM_YEAR_SEC;
        tmptim--;
        if (WM_IS_LEAP_YEAR(tmptim)) {
            caltim += WM_DAY_SEC;
            islpyr++;
        }
    } else {
        if (WM_IS_LEAP_YEAR(tmptim))
            islpyr++;
    }

    /*
     * tmptim now holds the value for tm_year. caltim now holds the
     * number of elapsed seconds since the beginning of that year.
     */
    tm->tm_year = tmptim;

    /*
     * Determine days since January 1 (0 - 365). This is the tm_yday value.
     * Leave caltim with number of elapsed seconds in that day.
     */
    tm->tm_yday = caltim / WM_DAY_SEC;
    caltim -= tm->tm_yday * WM_DAY_SEC;

    /* Determine months since January (0 - 11) and day of month (1 - 31) */
    if (islpyr) {
        mdays = _lpdays;
    } else {
        mdays = _days;
    }

    for (tmptim = 1; mdays[tmptim] < tm->tm_yday; tmptim++)
        ;

    tm->tm_mon  = --tmptim;
    tm->tm_mday = tm->tm_yday - mdays[tmptim];

    /* Determine days since Sunday (0 - 6) */
    tm->tm_wday = ((time_bak / WM_DAY_SEC) + WM_BASE_DOW) % 7;
    tm->tm_hour = caltim / 3600;
    caltim -= tm->tm_hour * 3600;
    tm->tm_min = caltim / 60;
    tm->tm_sec = caltim - (tm->tm_min) * 60;

    return 0;
}

static bool tz_to_utc_time(const char *value, int *time_s)
{
    const char *min_str = NULL;
    int len             = strlen(value);
    int hour            = 0;
    int min             = 0;
    int sec             = 0;

    if (memcmp(value, "UTC", 3)) {
        return false;
    }

    if (value[3] != '+' && value[3] != '-' && (value[3] < '0' || value[3] > '9')) {
        return false;
    }

    for (int i = 4; i < len; i++) {
        if (value[i] < '0' || value[i] > '9') {
            if (value[i] == ':') {
                if (min_str != NULL) {
                    return false;
                }

                min_str = &value[i + 1];
            } else {
                return false;
            }
        }
    }
    hour = atoi(value + 3);

    if (min_str) {
        min = atoi(min_str);
        if (min > 59) {
            return false;
        }

        if (hour < 0) {
            min *= -1;
        }
    }

    sec = hour * 60 * 60 + min * 60;

    /** rang is UTC-12:00 - UTC+14:00*/
    if (sec < -43200 || sec > 50400) {
        return false;
    }

    if (time_s != NULL) {
        *time_s = sec;
    }

    return true;
}

/** This function is implemented incorrectly in the C-SKY library */
struct tm *localtime(const time_t *tim_p)
{
    static struct tm s_tm = { 0 };
    time_t time_bak       = 0;
    int time_s;

    if (tim_p == NULL) {
        return NULL;
    }

    if (tz_to_utc_time(getenv("TZ"), &time_s)) {
        /** time zone offset */
        time_bak = time_s;
    }
    time_bak += (*tim_p);

    if (timestamp_conversion_to_tm(&time_bak, &s_tm)) {
        return NULL; /** Time range exceeds the limit */
    }

    return &s_tm;
}

int setenv(const char *name, const char *value, int overwrite)
{
    if (strcmp(name, "TZ") == 0) {
        /** set time zone */
        if (!tz_to_utc_time(value, NULL)) {
            return -1;
        }

        if (s_event_tz) {
            if (!overwrite) {
                return -1;
            }
            free(s_event_tz);
        }

        s_event_tz = strdup(value);
        if (s_event_tz == NULL) {
            return -1;
        }

        return 0;
    }

    return -1;
}
