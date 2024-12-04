/**
 * @file wm_dt.c
 *
 * @brief DT Module
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

#include <string.h>
#include "wm_list.h"
#include "wm_osal.h"
#include "wm_utils.h"
#include "wm_dt.h"
#include "wm_dt_op.h"
#include "wm_dt_hw.h"

#define LOG_TAG "dt"
#include "wm_log.h"

#define WM_DT_LIST_LOCK(lock, ret)                                                       \
    do {                                                                                 \
        if (!lock) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                  \
                wm_log_error("mutex create failed");                                     \
                return ret;                                                              \
            }                                                                            \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            wm_log_error("mutex acquire failed");                                        \
            return ret;                                                                  \
        }                                                                                \
    } while (0);

#define WM_DT_LIST_UNLOCK(lock, ret)         \
    do {                                     \
        if (!lock) {                         \
            wm_log_warn("mutex not create"); \
            return ret;                      \
        }                                    \
        wm_os_internal_mutex_release(lock);  \
    } while (0);

#define WM_DT_LIST_LOCK_GOTO(lock, ret)                                                  \
    do {                                                                                 \
        if (!lock) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                  \
                wm_log_error("mutex create failed");                                     \
                goto err;                                                                \
            }                                                                            \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            wm_log_error("mutex acquire failed");                                        \
            goto err;                                                                    \
        }                                                                                \
    } while (0);

#define WM_DT_LIST_UNLOCK_GOTO(lock, ret)    \
    do {                                     \
        if (!lock) {                         \
            wm_log_warn("mutex not create"); \
            goto err;                        \
        }                                    \
        wm_os_internal_mutex_release(lock);  \
    } while (0);

struct wm_dt_device_t {
    struct dl_list node;
    wm_device_t *dev;
};

struct wm_dt_device_array_t {
    uint8_t priority;
    struct wm_dt_device_t *dev;
};

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    /* more... */
} dt_device_hw_t;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    /* more... */
} dt_device_ops_t;

static DEFINE_DL_LIST(g_dt_dev_list);

static wm_os_mutex_t *g_dt_dev_mutex = NULL;

static char *g_dt_table_name                     = NULL;
static uint32_t g_dt_entry_count                 = 0;
static struct wm_dt_table_entry *g_dt_table_addr = NULL;

extern const unsigned int wm_dt_table_start;
extern const unsigned int wm_dt_table_end;

#define DT_CHECK_DEFAULT_TABLE_ENTRY(return_value)                                                    \
    if (!g_dt_table_addr || !g_dt_entry_count) {                                                      \
        if (!g_dt_table_name) {                                                                       \
            /* empty name or default select an dt table by random */                                  \
            table_entry = table_index->table_addr;                                                    \
        } else {                                                                                      \
            /* select an dt table by match name */                                                    \
            for (i = 0; i < ((unsigned int)(&wm_dt_table_end) - (unsigned int)(&wm_dt_table_start)) / \
                                sizeof(struct wm_dt_table_index);                                     \
                 i++) {                                                                               \
                if (!strcmp(g_dt_table_name, table_index->table_name)) {                              \
                    table_entry = table_index->table_addr;                                            \
                    break;                                                                            \
                }                                                                                     \
                table_index++;                                                                        \
            }                                                                                         \
        }                                                                                             \
                                                                                                      \
        if (!table_entry || !table_index->table_addr || !table_index->entry_count) {                  \
            wm_log_error("not found dt table %s", g_dt_table_name ? g_dt_table_name : "");            \
            return return_value;                                                                      \
        }                                                                                             \
                                                                                                      \
        /* save valid dt table addr in the ram */                                                     \
        g_dt_entry_count = table_index->entry_count;                                                  \
        g_dt_table_addr  = table_index->table_addr;                                                   \
    }

static inline int dt_init_device(wm_device_t *dev)
{
    int ret              = WM_ERR_NO_SUPPORT;
    dt_device_ops_t *ops = NULL;
    dt_device_hw_t *hw   = NULL;

    if (dev) {
        hw = (dt_device_hw_t *)dev->hw;
        if (!hw) {
            return WM_ERR_FAILED;
        }

        /* script sorted priority */
        if (!hw->init_cfg.init_level) /* 0-app, 1-system */
            return WM_ERR_SUCCESS;

        ops = (dt_device_ops_t *)dev->ops;
        if (!ops) {
            return WM_ERR_FAILED;
        }

        if (ops->init) {
            ret = ops->init(dev);
            if (ret) {
                wm_log_error("auto init device %s failed", dev->name);
            } else {
                wm_log_debug("auto init device %s success", dev->name);
            }
        }
    }

    return ret;
}

static inline struct wm_dt_device_t *dt_get_device(const char *device_name)
{
    struct wm_dt_device_t *dev = NULL;

    WM_DT_LIST_LOCK(g_dt_dev_mutex, NULL);
    dl_list_for_each(dev, &g_dt_dev_list, struct wm_dt_device_t, node)
    {
        if (!strcmp((char *)dev->dev->name, device_name)) {
            WM_DT_LIST_UNLOCK(g_dt_dev_mutex, NULL);
            return dev;
        }
    }
    WM_DT_LIST_UNLOCK(g_dt_dev_mutex, NULL);

    return NULL;
}

static inline struct wm_dt_device_t *dt_add_device(struct wm_dt_table_entry *table_entry, bool add2list)
{
    struct wm_dt_device_t *dev = NULL;

    dev = wm_os_internal_malloc(sizeof(struct wm_dt_device_t));
    if (!dev) {
        wm_log_error("mem not enough");
        return NULL;
    }

    memset(dev, 0, sizeof(struct wm_dt_device_t));
    dl_list_init(&dev->node);

    dev->dev = wm_os_internal_malloc(sizeof(wm_device_t));
    if (!dev->dev) {
        wm_log_error("mem not enough");
        wm_os_internal_free(dev);
        return NULL;
    }

    memset(dev->dev, 0, sizeof(wm_device_t));

    dev->dev->name = table_entry->dev_name;
    dev->dev->hw   = table_entry->hw_addr;
    dev->dev->ops  = table_entry->ops_addr;

    /* 5. add device to ram list */
    if (add2list) {
        WM_DT_LIST_LOCK(g_dt_dev_mutex, NULL);
        dl_list_add(&g_dt_dev_list, &dev->node);
        WM_DT_LIST_UNLOCK(g_dt_dev_mutex, NULL);
    }

    wm_log_debug("add device %s", table_entry->dev_name);

    return dev;
}

static int dt_compare_priority_callback(const void *a, const void *b)
{
    struct wm_dt_device_array_t *deva = (struct wm_dt_device_array_t *)a;
    struct wm_dt_device_array_t *devb = (struct wm_dt_device_array_t *)b;

    if (deva->priority > devb->priority)
        return -1;
    else if (deva->priority < devb->priority)
        return 1;
    else
        return 0;
}

wm_device_t *wm_dt_get_device_by_name(const char *device_name)
{
    struct wm_dt_device_t *dev = NULL;
    uint32_t i;
    struct wm_dt_table_entry *table_entry = NULL;
    struct wm_dt_table_index *table_index = (struct wm_dt_table_index *)(&wm_dt_table_start);

    if (!device_name) {
        return NULL;
    }

    /* 1. get from ram list */
    WM_DT_LIST_LOCK(g_dt_dev_mutex, NULL);
    dl_list_for_each(dev, &g_dt_dev_list, struct wm_dt_device_t, node)
    {
        if (!strcmp((char *)dev->dev->name, device_name)) {
            WM_DT_LIST_UNLOCK(g_dt_dev_mutex, NULL);
            return dev->dev;
        }
    }
    WM_DT_LIST_UNLOCK(g_dt_dev_mutex, NULL);

    /* 2. find valid dt table */
    DT_CHECK_DEFAULT_TABLE_ENTRY(NULL);

    /* 3. find dev info in the dt table */
    table_entry = NULL;
    for (i = 0; i < g_dt_entry_count; i++) {
        if (!strcmp(g_dt_table_addr[i].dev_name, device_name)) {
            table_entry = &g_dt_table_addr[i];
            break;
        }
    }

    if (!table_entry) {
        wm_log_error("not found device %s in the dt table %s", device_name, g_dt_table_name ? g_dt_table_name : "");
        return NULL;
    }

    /* 4. alloc device struct and fill hw and ops addr */
    dev = dt_add_device(table_entry, true);

    return dev ? dev->dev : NULL;
}

int wm_dt_set_device_table_name(const char *default_name)
{
    char *new_name = NULL;
    bool dt_used;
    uint32_t i;
    bool dt_config_exist                  = false;
    struct wm_dt_table_index *table_index = (struct wm_dt_table_index *)(&wm_dt_table_start);

    if (!default_name)
        return WM_ERR_INVALID_PARAM;

    WM_DT_LIST_LOCK(g_dt_dev_mutex, WM_ERR_FAILED);
    dt_used = dl_list_len(&g_dt_dev_list) > 0 ? true : false;
    WM_DT_LIST_UNLOCK(g_dt_dev_mutex, WM_ERR_FAILED);

    if (dt_used)
        return WM_ERR_NOT_ALLOWED;

    for (i = 0; i < ((unsigned int)(&wm_dt_table_end) - (unsigned int)(&wm_dt_table_start)) / sizeof(struct wm_dt_table_index);
         i++) {
        if (!strcmp(default_name, table_index->table_name)) {
            dt_config_exist = true;
            break;
        }
        table_index++;
    }

    if (!dt_config_exist)
        return WM_ERR_NOT_FOUND;

    new_name = strdup(default_name);
    if (!new_name)
        return WM_ERR_NO_MEM;

    if (g_dt_table_name)
        free(g_dt_table_name);

    g_dt_table_name = new_name;

    return WM_ERR_SUCCESS;
}

int wm_dt_free_device_by_name(const char *device_name)
{
    bool found                  = false;
    struct wm_dt_device_t *dev  = NULL;
    struct wm_dt_device_t *next = NULL;

    if (!device_name)
        return WM_ERR_INVALID_PARAM;

    WM_DT_LIST_LOCK(g_dt_dev_mutex, WM_ERR_TIMEOUT);

    dl_list_for_each_safe(dev, next, &g_dt_dev_list, struct wm_dt_device_t, node)
    {
        if (!strcmp((char *)dev->dev->name, device_name)) {
            dl_list_del(&dev->node);
            WM_DT_LIST_UNLOCK(g_dt_dev_mutex, WM_ERR_FAILED);
            found = true;
            break;
        }
    }

    if (found) {
        wm_os_internal_free(dev->dev);
        wm_os_internal_free(dev);
    } else {
        WM_DT_LIST_UNLOCK(g_dt_dev_mutex, WM_ERR_FAILED);
    }

    return found ? WM_ERR_SUCCESS : WM_ERR_NOT_FOUND;
}

int wm_dt_free_device(wm_device_t *device)
{
    bool found                  = false;
    struct wm_dt_device_t *dev  = NULL;
    struct wm_dt_device_t *next = NULL;

    if (!device)
        return WM_ERR_INVALID_PARAM;

    WM_DT_LIST_LOCK(g_dt_dev_mutex, WM_ERR_TIMEOUT);

    dl_list_for_each_safe(dev, next, &g_dt_dev_list, struct wm_dt_device_t, node)
    {
        if (device == dev->dev) {
            dl_list_del(&dev->node);
            WM_DT_LIST_UNLOCK(g_dt_dev_mutex, WM_ERR_FAILED);
            found = true;
            break;
        }
    }

    if (found) {
        wm_os_internal_free(dev->dev);
        wm_os_internal_free(dev);
    } else {
        WM_DT_LIST_UNLOCK(g_dt_dev_mutex, WM_ERR_FAILED);
    }

    return found ? WM_ERR_SUCCESS : WM_ERR_NOT_FOUND;
}

int wm_dt_free_all_device(void)
{
    struct wm_dt_device_t *dev  = NULL;
    struct wm_dt_device_t *next = NULL;

    WM_DT_LIST_LOCK(g_dt_dev_mutex, WM_ERR_TIMEOUT);

    dl_list_for_each_safe(dev, next, &g_dt_dev_list, struct wm_dt_device_t, node)
    {
        dl_list_del(&dev->node);

        wm_os_internal_free(dev->dev);
        wm_os_internal_free(dev);
    }

    WM_DT_LIST_UNLOCK(g_dt_dev_mutex, WM_ERR_FAILED);

    return WM_ERR_SUCCESS;
}

int wm_dt_dump_all_device(void)
{
    struct wm_dt_device_t *dev = NULL;

    wm_log_raw(WM_LOG_LEVEL_INFO, "%-32s %s\n", "device name", "state");

    WM_DT_LIST_LOCK(g_dt_dev_mutex, WM_ERR_TIMEOUT);

    dl_list_for_each(dev, &g_dt_dev_list, struct wm_dt_device_t, node)
    {
        wm_log_raw(WM_LOG_LEVEL_INFO, "%-32s %s\n", dev->dev->name, dev->dev->state == WM_DEV_ST_INITED ? "inited" : "uninit");
    }

    WM_DT_LIST_UNLOCK(g_dt_dev_mutex, WM_ERR_FAILED);

    return WM_ERR_SUCCESS;
}

int wm_dt_auto_init_device(void)
{
    int ret                     = WM_ERR_FAILED;
    struct wm_dt_device_t *dev  = NULL;
    struct wm_dt_device_t *next = NULL;
    uint32_t i;
    dt_device_hw_t *hw      = NULL;
    int dev_cnt             = 0;
    struct dl_list dev_list = DL_LIST_HEAD_INIT(dev_list);
    struct wm_dt_device_array_t *array;
    struct wm_dt_table_entry *table_entry = NULL;
    struct wm_dt_table_index *table_index = (struct wm_dt_table_index *)(&wm_dt_table_start);

    DT_CHECK_DEFAULT_TABLE_ENTRY(WM_ERR_NOT_FOUND);

    table_entry = NULL;
    for (i = 0; i < g_dt_entry_count; i++) {
        table_entry = &g_dt_table_addr[i];
        dev         = dt_get_device(table_entry->dev_name);

        if (!dev || (dev && (WM_DEV_ST_INITED != dev->dev->state))) {
            hw = (dt_device_hw_t *)table_entry->hw_addr;
            if (!hw->init_cfg.init_level)
                continue;

            if (!dev) {
                dev = dt_add_device(table_entry, false);
                if (!dev)
                    continue;
            } else {
                WM_DT_LIST_LOCK_GOTO(g_dt_dev_mutex, WM_ERR_NO_MEM);
                dl_list_del(&dev->node);
                WM_DT_LIST_UNLOCK_GOTO(g_dt_dev_mutex, WM_ERR_NO_MEM);
            }

            dl_list_add(&dev_list, &dev->node);
        }
    }

    dev_cnt = dl_list_len(&dev_list);
    if (!dev_cnt)
        return WM_ERR_SUCCESS;

    array = wm_os_internal_malloc(dev_cnt * sizeof(*array));
    if (!array) {
        ret = WM_ERR_NO_MEM;
        goto err;
    }

    i = 0;
    dl_list_for_each(dev, &dev_list, struct wm_dt_device_t, node)
    {
        hw                = (dt_device_hw_t *)dev->dev->hw;
        array[i].priority = hw->init_cfg.init_priority;
        array[i++].dev    = dev;
    }

    qsort(array, dev_cnt, sizeof(*array), dt_compare_priority_callback);

    for (i = 0; i < dev_cnt; i++) {
        dev = array[i].dev;
        dt_init_device(dev->dev);

        WM_DT_LIST_LOCK_GOTO(g_dt_dev_mutex, WM_ERR_NO_MEM);
        dl_list_add(&g_dt_dev_list, &dev->node);
        WM_DT_LIST_UNLOCK_GOTO(g_dt_dev_mutex, WM_ERR_NO_MEM);
    }

    wm_os_internal_free(array);

    return WM_ERR_SUCCESS;

err:
    dl_list_for_each_safe(dev, next, &dev_list, struct wm_dt_device_t, node)
    {
        dl_list_del(&dev->node);
        wm_os_internal_free(dev);
    }

    return ret;
}
