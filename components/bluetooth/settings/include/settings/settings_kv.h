/*
 * Copyright (c) 2019 Laczen
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SETTINGS_NVS_H_
#define __SETTINGS_NVS_H_

#include <settings/settings.h>
#include "wm_nvs.h"
#ifdef __cplusplus
extern "C" {
#endif

/* In the NVS backend, each setting is stored in two NVS entries:
 *	1. setting's name
 *	2. setting's value
 *
 * The NVS entry ID for the setting's value is determined implicitly based on
 * the ID of the NVS entry for the setting's name, once that is found. The
 * difference between name and value ID is constant and equal to
 * NVS_NAME_ID_OFFSET.
 *
 * Setting's name entries start from NVS_NAMECNT_ID + 1. The entry at
 * NVS_NAMECNT_ID is used to store the largest name ID in use.
 *
 * Deleted records will not be found, only the last record will be
 * read.
 */


struct settings_nvs_kv {
	struct settings_store cf_store;
    const char *cf_key;

};

/* register nvs to be a source of settings */
int settings_nvs_kv_src(struct settings_nvs_kv *cf);

/* register nvs to be the destination of settings */
int settings_nvs_kv_dst(struct settings_nvs_kv *cf);

/* Initialize a nvs backend. */
int settings_nvs_kv_backend_init(struct settings_nvs_kv *cf);


#ifdef __cplusplus
}
#endif

#endif /* __SETTINGS_NVS_H_ */
