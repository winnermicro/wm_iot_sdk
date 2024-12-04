/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_LOGGING_LOG_H_
#define ZEPHYR_INCLUDE_LOGGING_LOG_H_

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "bt"
#include "wm_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logging
 * @defgroup logging Logging
 * @{
 * @}
 */

/**
 * @brief Logger API
 * @defgroup log_api Logging API
 * @ingroup logger
 * @{
 */

/**
 * @brief Writes an ERROR level message to the log.
 *
 * @details It's meant to report severe errors, such as those from which it's
 * not possible to recover.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#if CONFIG_BT_PRINT_DEBUG_LEVEL >= 1
#define LOG_ERR(...) wm_log_error(__VA_ARGS__)
//#define LOG_ERR printf

#else
#define LOG_ERR(...) ((void)0)
#endif


/**
 * @brief Writes a WARNING level message to the log.
 *
 * @details It's meant to register messages related to unusual situations that
 * are not necessarily errors.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#if CONFIG_BT_PRINT_DEBUG_LEVEL >= 2
#define LOG_WRN(...) wm_log_warn(__VA_ARGS__)
//#define LOG_WRN printf

#else
#define LOG_WRN(...) ((void)0)
#endif

/**
 * @brief Writes an INFO level message to the log.
 *
 * @details It's meant to write generic user oriented messages.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#if CONFIG_BT_PRINT_DEBUG_LEVEL >= 3
#define LOG_INF(...) wm_log_info(__VA_ARGS__)
//#define LOG_INF printf

#else
#define LOG_INF(...) ((void)0)
#endif

/**
 * @brief Writes a DEBUG level message to the log.
 *
 * @details It's meant to write developer oriented information.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#if CONFIG_BT_PRINT_DEBUG_LEVEL >= 4
#define LOG_DBG(...) wm_log_debug(__VA_ARGS__)
#else
#define LOG_DBG(...) ((void)0)
#endif

/**
 * @brief Writes a VERBOSE level message to the log.
 *
 * @details It's meant to write developer oriented information.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#if CONFIG_BT_PRINT_DEBUG_LEVEL >= 5
#define LOG_VERB(...) wm_log_verbose(__VA_ARGS__)
#else
#define LOG_VERB(...) ((void)0)
#endif

/**
 * @brief Unconditionally print raw log message.
 *
 * The result is same as if printk was used but it goes through logging
 * infrastructure thus utilizes logging mode, e.g. deferred mode.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */

#if CONFIG_BT_PRINT_DEBUG_LEVEL >= 4
#define LOG_HEXDUMP_DBG(name, buf, size) wm_log_dump(WM_LOG_LEVEL_DEBUG,name, 16, buf, size)
#else
#define LOG_HEXDUMP_DBG(...) ((void)0)
#endif

#define LOG_PRINTK(...) wm_log_error(__VA_ARGS__)

#define LOG_MODULE_REGISTER(...)

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_LOGGING_LOG_H_ */
