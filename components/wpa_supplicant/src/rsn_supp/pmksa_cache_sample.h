#ifndef PMKSA_CACHE_SAMPLE_H
#define PMKSA_CACHE_SAMPLE_H

#include "config_ssid.h"
#include "wpa_supplicant_i.h"

void pmksa_cache_sample_init(void);

void pmksa_cache_sample_save(struct wpa_supplicant *wpa_s, const u8 *pmk, const u8 *pmkid);

void pmksa_cache_sample_update(struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid);

void pmksa_cache_sample_remove(void);

u8  *pmksa_cache_sample_pmkid(void);

#endif /* PMKSA_CACHE_SAMPLE_H */
