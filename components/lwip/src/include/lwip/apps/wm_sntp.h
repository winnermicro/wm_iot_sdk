#ifndef __WM_SNTP_H__
#define __WM_SNTP_H__

#include "lwip/apps/sntp.h"
#include "wm_netif.h"

/**
 * @brief          This function enables the NTP service
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void wm_sntp_start(void);

/**
 * @brief          This function is used to shut down the NTP service
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void wm_sntp_stop(void);

/**
 * @brief          This function is used to set the NTP server domain name
 *
 * @param[in]      idx            the index of the NTP server
 * @param[in]      *server        Domain name of the NTP server to set
 *
 * @return
 *                 - WM_ERR_SUCCESS: succeed
 *                 - others: failed
 *
 * @note           When setting SNTP domain name, the server IP address will be modified or cleared
 */
int wm_sntp_setservername(u8_t idx, const char *server);

/**
 * @brief          This function is used to get the NTP server domain name of the index
 *
 * @param[in]      idx           the index of the NTP server
 *
 * @return         Domain name of the indexed NTP server or NULL
 *
 * @note           None
 */
const char *wm_sntp_getservername(u8_t idx);

/**
 * @brief          This function is used to set the NTP server address
 *
 * @param[in]      idx           the index of the NTP server
 * @param[in]      *addr         IP address of the indexed NTP server
 *
 * @return
 *                 - WM_ERR_SUCCESS: succeed
 *                 - others: failed
 *
 * @note           None
 */
int wm_sntp_setserver(u8_t idx, const ip_addr_t *addr);

/**
 * @brief          This function is used to get the NTP server address of the index
 *
 * @param[in]      idx           the index of the NTP server
 *
 * @return         IP address of the indexed NTP server or NULL
 *
 * @note           None
 */
const ip_addr_t wm_sntp_getserver(u8_t idx);

/**
 * @brief          This function is used to get Beijing time
 *
 * @param[in]      tick_wait      max waiting time by tick
 *
 * @return         Beijing time
 *
 * @note           None
 */
struct tm *wm_get_time_sync(int tick_wait);

#endif /* __WM_SNTP_H__ */
