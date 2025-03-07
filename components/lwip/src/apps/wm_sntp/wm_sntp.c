#include "wmsdk_config.h"
#include "lwip/apps/wm_sntp.h"
#include "wm_osal.h"
#include "lwip/sockets.h"
#include "wm_event.h"
#include "wm_utils.h"
#include "wm_log.h"

WM_EVENT_DEFINE_GROUP(WM_SNTP_EV);

typedef struct {
    wm_sntp_patam_t time;                      /** sntp time */
    char *server[CONFIG_LWIP_MAX_NTP_SERVERS]; /** sntp server domain name */
    wm_os_sem_t *sem;                          /** sntp semaphore */
    bool is_sync;                              /** is the NTP clock synchronized */
} wm_sntp_t;

static wm_sntp_t s_wm_sntp = { 0 };

static void get_time_event_porc(wm_event_group_t group, int event, void *data, void *priv)
{
    if (event == WM_LWIP_EVENT_SNTP_GOT_TIME) {
        s_wm_sntp.time = *((wm_sntp_patam_t *)data);

        if (s_wm_sntp.sem) {
            wm_os_internal_sem_release(s_wm_sntp.sem);
        }
        s_wm_sntp.is_sync = true;
    }
}

static bool wm_network_is_ready(void)
{
    wm_ip_addr_t ip      = { 0 };
    wm_ip_addr_t netmask = { 0 };
    wm_ip_addr_t gw      = { 0 };

    wm_netif_get_ipaddr(WM_NETIF_TYPE_WIFI_STA, &ip, &netmask, &gw);

    return ip.u_addr.ip4.addr ? true : false;
}

void wm_sntp_start(void)
{
    ip_addr_t temp_addr = { 0 };
    ip_addr_t is_addr   = { 0 };

    if (wm_network_is_ready() == false) {
        wm_log_error("network is not ready, NTP start fail!");
        return;
    }

    if (s_wm_sntp.sem == NULL) {
        wm_os_internal_sem_create(&s_wm_sntp.sem, 0);
    }

#if CONFIG_LWIP_MAX_NTP_SERVERS >= 1
    memset(&temp_addr, 0, sizeof(temp_addr));
    memset(&is_addr, 0, sizeof(is_addr));
    temp_addr = wm_sntp_getserver(0);
    wm_sntp_setservername(0, NULL);
    if (wm_sntp_getservername(0) == NULL && ip_addr_isany_val(temp_addr)) {
        if (!ipaddr_aton(CONFIG_WM_NTP_SERVER1, &is_addr)) {
            wm_sntp_setservername(0, CONFIG_WM_NTP_SERVER1);
        } else {
            wm_sntp_setserver(0, &is_addr);
        }
    }
#endif

#if CONFIG_LWIP_MAX_NTP_SERVERS >= 2
    memset(&temp_addr, 0, sizeof(temp_addr));
    memset(&is_addr, 0, sizeof(is_addr));
    temp_addr = wm_sntp_getserver(1);
    wm_sntp_setservername(1, NULL);
    if (wm_sntp_getservername(1) == NULL && ip_addr_isany_val(temp_addr)) {
        if (!ipaddr_aton(CONFIG_WM_NTP_SERVER2, &is_addr)) {
            wm_sntp_setservername(1, CONFIG_WM_NTP_SERVER2);
        } else {
            wm_sntp_setserver(1, &is_addr);
        }
    }
#endif

#if CONFIG_LWIP_MAX_NTP_SERVERS >= 3
    memset(&temp_addr, 0, sizeof(temp_addr));
    memset(&is_addr, 0, sizeof(is_addr));
    temp_addr = wm_sntp_getserver(2);
    wm_sntp_setservername(2, NULL);
    if (wm_sntp_getservername(2) == NULL && ip_addr_isany_val(temp_addr)) {
        if (!ipaddr_aton(CONFIG_WM_NTP_SERVER3, &is_addr)) {
            wm_sntp_setservername(2, CONFIG_WM_NTP_SERVER3);
        } else {
            wm_sntp_setserver(2, &is_addr);
        }
    }
#endif
    wm_event_add_callback(WM_SNTP_EV, WM_EVENT_ANY_TYPE, get_time_event_porc, NULL);
    sntp_init();
}

void wm_sntp_stop(void)
{
    wm_event_remove_callback(WM_SNTP_EV, WM_EVENT_ANY_TYPE, get_time_event_porc, NULL);
    sntp_stop();

    for (int i = 0; i < CONFIG_LWIP_MAX_NTP_SERVERS; i++) {
        if (s_wm_sntp.server[i] != NULL) {
            free(s_wm_sntp.server[i]);
            s_wm_sntp.server[i] = NULL;
        }
    }

    if (s_wm_sntp.sem) {
        wm_os_internal_sem_delete(s_wm_sntp.sem);
        s_wm_sntp.sem = NULL;
    }
    s_wm_sntp.is_sync = false;
}

int wm_sntp_setserver(u8_t idx, const ip_addr_t *addr)
{
    if (idx >= CONFIG_LWIP_MAX_NTP_SERVERS) {
        wm_log_info("Idx failed!");
        return WM_ERR_INVALID_PARAM;
    }

    sntp_setserver(idx, addr);
    return WM_ERR_SUCCESS;
}

const ip_addr_t wm_sntp_getserver(u8_t idx)
{
    ip_addr_t addr = { 0 };

    memcpy(&addr, sntp_getserver(idx), sizeof(ip_addr_t));

    return addr;
}

int wm_sntp_setservername(u8_t idx, const char *server)
{
    if (idx >= CONFIG_LWIP_MAX_NTP_SERVERS) {
        wm_log_info("Idx failed!");
        return WM_ERR_INVALID_PARAM;
    }

    if (s_wm_sntp.server[idx] != NULL) {
        free(s_wm_sntp.server[idx]);
        s_wm_sntp.server[idx] = NULL;
    }
    if (server == NULL) {
        sntp_setservername(idx, NULL);
        return WM_ERR_SUCCESS;
    }

    s_wm_sntp.server[idx] = strdup(server);
    wm_sntp_setserver(idx, NULL);
    sntp_setservername(idx, s_wm_sntp.server[idx]);
    return WM_ERR_SUCCESS;
}

const char *wm_sntp_getservername(u8_t idx)
{
    const char *result = sntp_getservername(idx);
    return result;
}

struct tm *wm_get_time_sync(int tick_wait)
{
    if (s_wm_sntp.is_sync) {
        time_t tim = time(NULL);
        return localtime(&tim);
    }

    if (s_wm_sntp.sem == NULL) {
        wm_log_error("sntp is not start!");
        return NULL;
    }

    if (wm_network_is_ready() == false) {
        wm_log_error("network anomaly!");
        return NULL;
    }

    if (wm_os_internal_sem_acquire(s_wm_sntp.sem, tick_wait) != WM_OS_STATUS_SUCCESS) {
        wm_log_error("get time failed!");
        return NULL;
    }

    return localtime((const time_t *)&s_wm_sntp.time.sec);
}
