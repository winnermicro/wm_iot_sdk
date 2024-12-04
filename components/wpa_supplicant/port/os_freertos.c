/*
 * wpa_supplicant/hostapd / Empty OS specific functions
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file can be used as a starting point when adding a new OS target. The
 * functions here do not really work as-is since they are just empty or only
 * return an error value. os_internal.c can be used as another starting point
 * or reference since it has example implementation of many of these functions.
 */

#include "includes.h"

#include "os.h"

#include <stdio.h>
//#include <stdarg.h>
#include <string.h>
#include "wm_utils.h"
#include "wm_osal.h"
#include "wm_drv_rng.h"

void os_sleep(os_time_t sec, os_time_t usec)
{
    uint32_t time_ms = sec * 1000 + usec / 1000;
    wm_os_internal_time_delay_ms(time_ms);
}

int os_get_time(struct os_time *t)
{
    uint32_t time_ms = wm_os_internal_get_time_ms();
    t->sec           = time_ms / 1000;
    t->usec          = (time_ms % 1000) * 1000;
    return 0;
}

int os_get_reltime(struct os_reltime *t)
{
    uint32_t time_ms = wm_os_internal_get_time_ms();
    t->sec           = time_ms / 1000;
    t->usec          = (time_ms % 1000) * 1000;
    return 0;
}

int os_mktime(int year, int month, int day, int hour, int min, int sec, os_time_t *t)
{
    return -1;
}

int os_gmtime(os_time_t t, struct os_tm *tm)
{
    return -1;
}

int os_daemonize(const char *pid_file)
{
    return -1;
}

void os_daemonize_terminate(const char *pid_file)
{
}

int os_get_random(unsigned char *buf, size_t len)
{
    return wm_drv_fill_random(buf, len);
}

unsigned long os_random(void)
{
    unsigned long ret = 0;
    os_get_random((unsigned char *)&ret, sizeof(ret));
    return ret;
}

char *os_rel2abs_path(const char *rel_path)
{
    return NULL; /* strdup(rel_path) can be used here */
}

int os_program_init(void)
{
    return 0;
}

void os_program_deinit(void)
{
}

int os_setenv(const char *name, const char *value, int overwrite)
{
    return -1;
}

int os_unsetenv(const char *name)
{
    return -1;
}

char *os_readfile(const char *name, size_t *len)
{
    return NULL;
}

int os_fdatasync(FILE *stream)
{
    return 0;
}

#ifdef CONFIG_HEAP_USE_TRACING
void *os_zalloc_tracing(size_t size, const char *file, int line)
{
    void *ptr = wm_os_internal_malloc_tracing(size, file, line);
    if (ptr)
        os_memset(ptr, 0, size);
    return ptr;
}

void *os_calloc_tracing(size_t nmemb, size_t size, const char *file, int line)
{
    if (size && nmemb > (~(size_t)0) / size)
        return NULL;
    return os_zalloc_tracing(nmemb * size, file, line);
}
#else
void *os_zalloc_tracing(size_t size, const char *file, int line)
{
    void *ptr = os_malloc(size);
    if (ptr)
        os_memset(ptr, 0, size);
    return ptr;
}

void *os_calloc_tracing(size_t nmemb, size_t size, const char *file, int line)
{
    if (size && nmemb > (~(size_t)0) / size)
        return NULL;
    return os_zalloc_tracing(nmemb * size, file, line);
}

void *os_zalloc(size_t size)
{
    void *ptr = os_malloc(size);
    if (ptr)
        os_memset(ptr, 0, size);
    return ptr;
}
#endif

void *os_memdup(const void *src, size_t n)
{
    void *r = os_malloc(n);

    if (r && src)
        os_memcpy(r, src, n);
    return r;
}

#ifdef OS_NO_C_LIB_DEFINES
#ifndef CONFIG_HEAP_USE_TRACING
void *os_malloc(size_t size)
{
    return wm_os_internal_malloc(size);
}

void *os_realloc(void *ptr, size_t size)
{
    return wm_os_internal_realloc(ptr, size);
}
#endif

void os_free(void *ptr)
{
    wm_os_internal_free(ptr);
}

void *os_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

void *os_memmove(void *dest, const void *src, size_t n)
{
    return memmove(dest, src, n);
}

void *os_memset(void *s, int c, size_t n)
{
    return memset(s, c, n);
}

int os_memcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}

char *os_strdup(const char *s)
{
#if 0
	size_t len;
	char *d;
	len = os_strlen(s);
	d = os_malloc(len + 1);
	if (d == NULL)
		return NULL;
	os_memcpy(d, s, len);
	d[len] = '\0';
	return d;
#else
    return strdup(s);
#endif
}

size_t os_strlen(const char *s)
{
    return strlen(s);
}

int os_strcasecmp(const char *s1, const char *s2)
{
#if 0
	char a, b;
	while (*s1 && *s2) {
		a = *s1++;
		b = *s2++;

		if (a == b)
			continue;

		if (a >= 'a' && a <= 'z')
			a -= 'a' - 'A';
		if (b >= 'a' && b <= 'z')
			b -= 'a' - 'A';

		if (a != b)
			return 1;
	}

	return 0;
#else
    return strcasecmp(s1, s2);
#endif
}

int os_strncasecmp(const char *s1, const char *s2, size_t n)
{
    char a, b;
    while (*s1 && *s2 && n) {
        a = *s1++;
        b = *s2++;
        n--;

        if (a == b)
            continue;

        if (a >= 'a' && a <= 'z')
            a -= 'a' - 'A';
        if (b >= 'a' && b <= 'z')
            b -= 'a' - 'A';

        if (a != b)
            return 1;
    }

    return 0;
}

char *os_strchr(const char *s, int c)
{
    return strchr(s, c);
}

char *os_strrchr(const char *s, int c)
{
    return strrchr(s, c);
}

int os_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

int os_strncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1, s2, n);
}

size_t os_strlcpy(char *dest, const char *src, size_t size)
{
    const char *s = src;
    size_t left   = size;

    if (left) {
        /* Copy string up to the maximum size of the dest buffer */
        while (--left != 0) {
            if ((*dest++ = *s++) == '\0')
                break;
        }
    }

    if (left == 0) {
        /* Not enough room for the string; force NUL-termination */
        if (size != 0)
            *dest = '\0';
        while (*s++)
            ; /* determine total src string length */
    }

    return s - src - 1;
}

char *os_strstr(const char *haystack, const char *needle)
{
    return strstr(haystack, needle);
}

int os_snprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = vsnprintf(str, size, format, ap);
    va_end(ap);
    if (size > 0)
        str[size - 1] = '\0';
    return ret;
}
#endif /* OS_NO_C_LIB_DEFINES */

int os_exec(const char *program, const char *arg, int wait_completion)
{
    return -1;
}

int os_memcmp_const(const void *a, const void *b, size_t len)
{
    const unsigned char *aa = a;
    const unsigned char *bb = b;
    size_t i;
    unsigned char res;

    for (res = 0, i = 0; i < len; i++)
        res |= aa[i] ^ bb[i];

    return res;
}
