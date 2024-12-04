/**
 * @file wm_utils.h
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

#ifndef __WM_UTILS_H__
#define __WM_UTILS_H__

#include <ctype.h>
#include <time.h>
#include "wm_types.h"

#if CONFIG_COMPONENT_EVENT_ENABLED
#include "wm_event.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_UTILS_Macros WM UTILS Macros
 * @brief WinnerMicro UTILS Macros
 */

/**
 * @addtogroup WM_UTILS_Macros
 * @{
 */

#define WM_MAC_ADDR_LEN (6) /**< MAC Address Len*/
#define WM_DEFAULT_BASE_MAC_ADDRESS        \
    {                                      \
        0x00, 0x25, 0x08, 0x09, 0x01, 0x0F \
    } /**< Default MAC Addr*/

#if (CONFIG_COMPONENT_EVENT_ENABLED && CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS)
WM_EVENT_DECLARE_GROUP(WM_UTILS_EV); /**< declare utils event group as WM_UTILS_EV*/
#endif

#define WM_DATA_ALIGN_TYPE_INVALID (0x0) /**< Initial value for the data alignment flag */
#define WM_DATA_ALIGN_TYPE_DMA     (0x1) /**< Data is aligned and suitable for DMA transmission */
#define WM_DATA_ALIGN_TYPE_CPU     (0x2) /**< Data is unaligned and can be transmitted with CPU.*/

/**
 * @}
 */

/**
 * @defgroup WM_UTILS_Enumerations WM UTILS Enumerations
 * @brief WinnerMicro UTILS Enumerations
 */

/**
 * @addtogroup WM_UTILS_Enumerations
 * @{
 */

#if (CONFIG_COMPONENT_EVENT_ENABLED && CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS)
/**
 * @brief Enumerations of CPU Usage Event Type
 */
enum {
    WM_UTILS_EVENT_CPU_USAGE = 0, /**< cpu usage event type */
};
#endif

/**
 * @brief Enumerations of MAC address type
 */
typedef enum {
    WM_MAC_TYPE_STA = 0, /**< MAC Type: WIFI STA */
    WM_MAC_TYPE_SAP,     /**< MAC Type: WIFI SoftAP */
    WM_MAC_TYPE_BT,      /**< MAC Type: BT/BLE */
    WM_MAC_TYPE_MAX      /**< The Max Number of MAC Type */
} wm_mac_type_t;

/**
 * @brief Structure to hold data transmit management information
 *
 * This structure is used to manage the data transfer parameters such as
 * the memory address, size of the data, and the flag indicating the
 * transmit method (DMA or CPU).
 */
typedef struct {
    uint8_t flag;  /**< Flag indicating the transmit method:
                         - WM_DATA_ALIGN_TYPE_INVALID
                         - WM_DATA_ALIGN_TYPE_DMA
                         - WM_DATA_ALIGN_TYPE_CPU */
    uint8_t *addr; /**< Pointer to the memory address where the data is located */
    uint32_t size; /**< Size of the data block to be transmited */
} wm_data_align_t;

#if (CONFIG_COMPONENT_EVENT_ENABLED && CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS)
/** @brief utils event data.
 *
 * use function wm_event_send to send utils event data
 * process utils event data in user callback
 *
 */
typedef struct {
    int cpu_usage; /**< cpu_usage */
} wm_utils_event_t;
#endif

/**
 * @}
 */

/**
 * @defgroup WM_UTILS_APIs WM UTILS APIs
 * @brief WinnerMicro UTILS APIs
 */

/**
 * @addtogroup WM_UTILS_APIs
 * @{
 */

/**
 * @brief system reboot
 *
 * This function reboot the system.
 */
void wm_system_reboot(void);

/**
 * @brief          This function to set cpu usage duration
 *
 * @param[in]      time_ms : The time of being counted, if 0, the statistical time will be 
 *                           calculated from the last time it was not 0
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other failed
 */
int wm_set_cpu_usage_duration(uint32_t time_ms);

/**
 * @brief  Converts the hex string to a byte array
 *
 * @param[in]  str   hex string
 * @param[out] array hex array
 * @param[in]  array_len hex array length
 *
 * @return
 *         - WM_ERR_SUCCESS on success
 *         - other failed
 */
int wm_hex_str_to_array(const char *str, uint8_t array[], int array_len);

/**
 * @brief convert double hex character to byte
 *
 * @param[in] hex double hex character
 * @return
 *      - >= 0 : the result convert by character
 *      - -1  : fail
 */
int wm_hex2byte(const char hex[2]);

/**
 * @brief convert  hex character to integer
 *
 * @param[in] ch hex character
 * @return
 *      - >= 0 : the result convert by character
 *      - -1  : fail
 */
int wm_atohex(char ch);

/**
 * @brief convert string to integer
 *
 * @param[in] s the string to be checked
 * @param[out] err error, 0: no error, 1: has error
 * @return
 *      - integer : the result convert by string
 */
int wm_atoi(const char *s, int *err);

/**
 * @brief calculate crc8
 *
 * @param[in] ptr A pointer to the data buffer to compute the CRC
 * @param[in] len The length of the data buffer in bytes
 * @return
 *      - true : exist double quotation marks
 *      - false : not exist double quotation marks
 */
uint8_t wm_get_crc8(uint8_t *ptr, uint32_t len);

/**
 * @brief Check whether there are double quotation marks at both ends of the string
 *
 * @param[in] str the string to be checked
 * @return
 *      - true : exist double quotation marks
 *      - false : not exist double quotation marks
 */
bool wm_isstr(const char *str);

/**
 * @brief transfer a num string to int value
 *
 * @param[in] str the string indicates a hex value or digital value
 * @param[out] value the pointer indicates digital value
 * @return
 *      - true : translate success
 *      - false : translate failure
 */
int wm_str2int(const char *str, int *value);

/**
 * @brief Check the string is a valid hex string or digital string
 *
 * @param[in] str the string to be checked
 * @return
 *      - true : is a string indicates a hex value or digital value
 *      - false : is not a hex value or digital value
 */
bool wm_isnumstr(const char *str);

/**
 * @brief Check the string is a valid digital string
 *
 * @param[in] str the string to be checked
 * @return
 *      - true : is a digital string
 *      - false : is not a digital string
 */
bool wm_isdigitstr(const char *str);

/**
 * @brief Check the string is a valid decimal string
 *
 * @param[in] str the string to be checked
 * @return
 *      - true : is a decimal string
 *      - false : is not a decimal string
 */
bool wm_isdecimalstr(const char *str);

/**
 * @brief Check the string is a valid hex string
 *
 * @param[in] str the string to be checked
 * @return
 *      - true : is a hex string
 *      - false : is not a hex string
 */
bool wm_ishexstr(const char *str);

/**
 * @brief Remove double quotes at both ends of the args
 *
 * @param[in] argc Number of arguments
 * @param[in,out] argv the arguments array
 */
void wm_remove_quotation_mark(int argc, char **argv);

/**
 * @brief Segments the data buffer based on 4-byte alignment requirements.
 *
 * This function aligns the buffer's starting address and length to 4-byte boundaries.
 * It categorizes the buffer into segments suitable for DMA and CPU transfers accordingly.
 *
 * @param[in] addr The pointer to the source address of the data buffer.
 * @param[in] size The total size of the data to be transferred.
 * @param[in,out] table The pointer to the table where the alignment information will be stored.
 *                      The table must have exactly 3 elements.
 *
 * @return WM_ERR_SUCCESS on success, or an error code on failure.
 *
 * @note The `table` must have exactly 3 elements to accommodate the maximum of three segments:
 *       - The first segment for CPU transfer (if necessary).
 *       - The second segment for DMA transfer.
 *       - The third segment for any remaining CPU transfer.
 *
 * @note Examples:
 * Example A: Input addr=0x10000000, size=103
 * - The output table will be:
 *   - table[0]: flag=1 (DMA), addr=0x10000004, size=100 (100 bytes for DMA transfer)
 *   - table[1]: flag=2 (CPU), addr=0x10000104, size=3   (remaining 3 bytes for CPU transfer)
 *   - table[2]: flag=0 (Invalid), addr=NULL, size=0
 *
 * Example B: Input addr=0x10000001, size=103
 * - The output table will be:
 *   - table[0]: flag=2 (CPU), addr=0x10000001, size=3   (first 3 bytes for CPU transfer)
 *   - table[1]: flag=1 (DMA), addr=0x10000004, size=100 (next 100 bytes for DMA transfer)
 *   - table[2]: flag=0 (Invalid), addr=NULL, size=0
 *
 * Example C: Input addr=0x10000002, size=103
 * - The output table will be:
 *   - table[0]: flag=2 (CPU), addr=0x10000002, size=2   (first 2 bytes for CPU transfer)
 *   - table[1]: flag=1 (DMA), addr=0x10000004, size=100 (next 100 bytes for DMA transfer)
 *   - table[2]: flag=2 (CPU), addr=0x10000108, size=1   (last 1 byte for CPU transfer)
 */
int wm_data_align_parser(uint8_t *addr, int32_t size, wm_data_align_t table[3]);

/**
 * @brief The following API prototypes are same with libc, but they are not in the libc library
 * on W80X platform, so they are implemented in wm_utils.c
 */
void *memrchr(const void *s, int c, size_t n);
char *strdup(const char *s);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
size_t strlcpy(char *dest, const char *src, size_t len);
size_t strnlen(const char *s, size_t maxlen);

/**
 * @brief Convert struct tm to seconds since 1970.
 *
 * @param[in] timeptr struct tm pointer, including year, month, day, hour, minute, second and etc.
 *
 * @return
 *      - The number of seconds since 1970.
 */
time_t mktime(struct tm *timeptr);

/**
 * @brief Convert seconds since 1970 to struct tm.
 *
 * @param[in] tim_p pointer of seconds since 1970.
 *
 * @return
  *     - NULL:  Failed
  *     - others: struct tm pointer
 */
struct tm *localtime(const time_t *tim_p);

/**
 * @brief Set time and date
 *
 * @param[in] tv Pointer of struct timeval, include seconds and microseconds since 1970.
 * @param[in] tz Time zone related information has been discarded
 *
 * @return
  *     - 0:  success
  *     - others: failure
 */
#if __GNUC__ >= 13
int settimeofday(struct timeval *tv, void *tz);
#else
int settimeofday(const struct timeval *tv, const struct timezone *tz);
#endif

/**
 * @brief  Set environment variable
 *
 * @param[in] name Name of the variable.
 * @param[in] value Value to set to the variable
 * @param[in] overwrite Whether existing variable should be overwritten
 *
 * @return
  *     - 0:  success
  *     - others: failure
 */
int setenv(const char *name, const char *value, int overwrite);

/**
  * @brief  Get the MAC address from storage according to the mac_type. 
  * 
  * The main MAC address load flow in this api as follows:
  * First, obtain the MAC address from NVStore according to the mac_type.
  * Second, if the MAC in NVStore is invalid, try to get the raw MAC from FT region(from flash or  
  * OTP depending no the chip design).
  * Third, since the FT region may only have two MAC addresses (WIFI mac, BT MAC), a coversion
  * needs to be done according to the actual usage of STA, SoftAP and BT.
  * Fouth, if the FT mac is invalid, use a default MAC address and then a conversion according to
  * the actual usage of STA, SoftAP, and BT.
  * 
  * @param[in] mac_type  Indicates the type of the MAC address,refer to @ref wm_mac_type_t
  * @param[in,out] mac_addr A pointer to a MAC address buffer that represents a six-byte length 
  *                         hexadecimal value
  * @param[in] mac_addr_len The length of the MAC address buffer, which should always be equal 
  *                         to @ref WM_MAC_ADDR_LEN
  * 
  * @attention  This is a weak API that provides a reference design for MAC address management. It
  * is allowed to replace this function with a new one in a user's project according to their own 
  * MAC strategy. For example, implement the same function in main.c without needing to declare it
  * again. The linker will use the new implementation.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_sys_get_mac_addr(wm_mac_type_t mac_type, uint8_t *mac_addr, uint8_t mac_addr_len);

/**
  * @brief  Set the MAC address into storage according to the mac_type. 
  * 
  * The main MAC address store flow in this api as follows:
  * First, write the MAC address to NVStore according to the mac_type.
  * Second, if NVStore is not enabled, return direclty.
  * 
  * @param[in] mac_type Indicates the type of the MAC address,refer to @ref wm_mac_type_t
  * @param[in] mac_addr A pointer to a MAC address buffer that represents a six-byte length 
  *                     hexadecimal value
  * @param[in] mac_addr_len The length of the MAC address buffer, which should always be equal 
  *                     to @ref WM_MAC_ADDR_LEN

  * @attention  This is a weak API that provides a reference design for MAC address management. 
  * It is allowed to replace this function with a new one in a user's project according to their
  * own MAC strategy. For example, implement the same function in main.c without needing to 
  * declare it again. The linker will use the new implementation.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_sys_set_mac_addr(wm_mac_type_t mac_type, uint8_t *mac_addr, uint8_t mac_addr_len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_UTILS_H__ */
