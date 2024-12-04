/*
 * Copyright (c) 2023 WinnerMicro
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <settings/settings.h>
#include "settings/settings_kv.h"
#include "settings_priv.h"
#include "wm_error.h"
#include "wm_nvs.h"
#include "wm_osal.h"
#define LOG_TAG "settings_kv"
#include <logging/log.h>
#include "wm_key_config.h"

struct settings_nvs_kv_key_read_fn_arg {
	const char *cf_key;
};

static sys_slist_t settings_key_list;

typedef struct{
    char *key;
    size_t key_len;

    sys_snode_t node;
    /**< Linked list node info for module internal usage. */
} key_node_t;

static int settings_nvs_kv_load(struct settings_store *cs,
                 const struct settings_load_arg *arg);
static int settings_nvs_kv_save(struct settings_store *cs, const char *name,
                 const char *value, size_t val_len);
static void *settings_nvs_kv_storage_get(struct settings_store *cs);

static struct settings_store_itf settings_nvs_kv_itf = {
    .csi_load = settings_nvs_kv_load,
    .csi_save = settings_nvs_kv_save,
    .csi_storage_get = settings_nvs_kv_storage_get
};

static size_t settings_nvs_kv_read_fn(void *back_end, void *data, size_t len)
{
    int ret;
    size_t rc = len;
    struct settings_nvs_kv_key_read_fn_arg *rd_fn_arg;
    wm_nvs_handle_t handle = NULL;

    rd_fn_arg = (struct settings_nvs_kv_key_read_fn_arg *)back_end;

    if(wm_nvs_open(WM_NVS_DEF_PARTITION,WM_GROUP_BLE,WM_NVS_OP_READ_WRITE,&handle) != WM_ERR_SUCCESS){
        return 0;
    }

    ret = wm_nvs_get_binary(handle,rd_fn_arg->cf_key, data, &rc);

    wm_nvs_close(handle);

    if(ret == WM_ERR_SUCCESS)
    {
        return rc;
    }else
    {
        LOG_WRN("nvs error when reading key[%s]-value", rd_fn_arg->cf_key);
    }

    return 0;
}

int settings_nvs_kv_src(struct settings_nvs_kv *cf)
{
    cf->cf_store.cs_itf = &settings_nvs_kv_itf;
    settings_src_register(&cf->cf_store);

    return 0;
}

int settings_nvs_kv_dst(struct settings_nvs_kv *cf)
{
    cf->cf_store.cs_itf = &settings_nvs_kv_itf;
    settings_dst_register(&cf->cf_store);

    return 0;
}

static void settings_loading_key(const char *key, size_t key_len)
{
    key_node_t *key_node = (key_node_t *)wm_os_internal_malloc(sizeof(key_node_t));

    if(key_node)
    {
        key_node->key = (char*)wm_os_internal_malloc(key_len+1);
        if(key_node->key)
        {
            memcpy(key_node->key, key, key_len);
            key_node->key[key_len] = '\0';
            key_node->key_len = key_len;
            LOG_DBG("Found key[%d] %s in nvs", key_len, key_node->key);
            sys_slist_append(&settings_key_list, &key_node->node);
        }else
        {
            wm_os_internal_free(key_node);
        }
    }else
    {
        LOG_WRN("No free memory to store loaded key name");
    }
}
static int settings_nvs_kv_load(struct settings_store *cs,
                 const struct settings_load_arg *arg)
{
    int ret = 0;
    key_node_t *key, *key_next;
    struct settings_nvs_kv_key_read_fn_arg read_fn_arg;
    wm_nvs_iterator_t iterator;
    wm_nvs_entry_t entry_info;
    sys_slist_init(&settings_key_list);
    //wm_nvs_print_key(settings_loading_key);

    wm_nvs_entry_find(WM_NVS_DEF_PARTITION,WM_GROUP_BLE,WM_NVS_TYPE_ANY,&iterator);
    while(iterator){
        wm_nvs_entry_info(iterator,&entry_info);
        settings_loading_key(entry_info.key,strlen(entry_info.key));
        wm_nvs_entry_next(&iterator);
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&settings_key_list, key, node)
    {
        read_fn_arg.cf_key = key->key;
        ret = settings_call_set_handler(
            key->key, key->key_len,
            settings_nvs_kv_read_fn, &read_fn_arg,
            (void *)arg);
        if (ret)
        {
            break;
        }
    }

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&settings_key_list, key, key_next, node)
    {
        if(key)
        {
            sys_slist_remove(&settings_key_list, NULL, &key->node);
            wm_os_internal_free(key->key);
            wm_os_internal_free(key);
        }
    }

    return 0;
}

static int settings_nvs_kv_save(struct settings_store *cs, const char *name,
                    const char *value, size_t val_len)
{
    int ret;
    bool delete;
    uint8_t kv_value[128];
    size_t kv_value_len = sizeof(kv_value);
    wm_nvs_handle_t handle = NULL;

    if (!name)
    {
        return -EINVAL;
    }

    LOG_DBG("settings_nvs_kv_save %s(%d), value length %d", name, strlen(name), val_len);

    if(wm_nvs_open(WM_NVS_DEF_PARTITION,WM_GROUP_BLE,WM_NVS_OP_READ_WRITE,&handle) != WM_ERR_SUCCESS){
        return WM_ERR_FAILED;
    }
    /* Find out if we are doing a delete */
    delete = ((value == NULL) || (val_len == 0));

    if(delete)
    {
        ret = wm_nvs_get_binary(handle,name, kv_value, &kv_value_len);
        if(ret == WM_ERR_SUCCESS)
        {
            wm_nvs_del_key(handle,name);
            LOG_DBG("remove %s from kv", name);
        }

        wm_nvs_close(handle);
        return 0;
     }

    ret = wm_nvs_set_binary(handle,name, value, val_len);
    if(ret != WM_ERR_SUCCESS)
    {
        LOG_ERR("settings_nvs_kv_save %s, value length %d", name, val_len);
    }

    wm_nvs_close(handle);

    return ret;
}

/* Initialize the nvs backend. */
int settings_nvs_kv_backend_init(struct settings_nvs_kv *cf)
{
    return 0;
}

int settings_backend_init(void)
{
    int rc;
    static struct settings_nvs_kv default_settings_nvs_kv;

    rc = settings_nvs_kv_backend_init(&default_settings_nvs_kv);
    if (rc) 
    {
        return rc;
    }

    rc = settings_nvs_kv_src(&default_settings_nvs_kv);

    if (rc) 
    {
        return rc;
    }

    rc = settings_nvs_kv_dst(&default_settings_nvs_kv);

    return rc;
}

static void *settings_nvs_kv_storage_get(struct settings_store *cs)
{
    struct settings_nvs_kv *cf = CONTAINER_OF(cs, struct settings_nvs_kv, cf_store);

    return &cf->cf_key;
}
