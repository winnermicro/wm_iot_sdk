#ifndef __WM_DEBUG_H__
#define __WM_DEBUG_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DEBUG_Enumerations WM DEBUG Enumerations
 * @brief WinnerMicro DEBUG Enumerations
 */

/**
 * @addtogroup WM_DEBUG_Enumerations
 * @{
 */

/**
 * @brief Enumeration for the reason of system reboot.
 *
 * This enumeration lists the different reasons that could cause the system to reboot.
 * The reasons include power on or reset button, chip standby, exception reset,
 * watchdog timeout, user active reset, and over-the-air firmware update.
 */
typedef enum {
    WM_REBOOT_REASON_POWER_ON    = 0, /**< power on or reset button */
    WM_REBOOT_REASON_STANDBY     = 1, /**< chip standby */
    WM_REBOOT_REASON_EXCEPTION   = 2, /**< exception reset */
    WM_REBOOT_REASON_WDG_TIMEOUT = 3, /**< watchdog timeout */
    WM_REBOOT_REASON_ACTIVE      = 4, /**< user active reset */
    WM_REBOOT_REASON_OTA         = 5, /**< reboot for over-the-air firmware update */
    WM_REBOOT_REASON_MAX              // No comment needed for the max value
} wm_reboot_reason_t;

/**
 * @}
 */

/**
 * @defgroup WM_DEBUG_Unions WM DEBUG Unions
 * @brief WinnerMicro DEBUG Unions
 */

/**
 * @addtogroup WM_DEBUG_Unions
 * @{
 */

/**
 * @brief Union for reboot parameters.
 *
 * This union provides a way to access the reboot parameters either as a single 32-bit unsigned integer
 * or by individual bitfields. It encapsulates the reason for the reboot, a flag to indicate if the reboot log is enabled,
 * and reserved bits for future extensions.
 */
typedef union {
    uint32_t val; /**< The raw 32-bit unsigned integer value representing the reboot parameters */
    struct {
        uint32_t reboot_reason : 8;  /**< [7:0] The code indicating why the system was rebooted */
        uint32_t reserved      : 24; /**< [31:8] 24-bit field reserved for future use. */
    };
} wm_reboot_para_t;

/**
 * @}
 */

/**
 * @defgroup WM_DEBUG_Functions WM DEBUG Functions
 * @brief WinnerMicro DEBUG Functions
 */

/**
 * @addtogroup WM_DEBUG_Functions
 * @{
 */

/**
  * @brief     Set reboot reason code
  *
  * @param     reason : reason code
  *
  * @return  None
  */
void wm_set_reboot_reason(wm_reboot_reason_t reason);

/**
  * @brief  Get reboot reason code
  *
  * @return wm_reboot_reason_t
  */
wm_reboot_reason_t wm_get_reboot_reason(void);

/**
  * @brief     Wait command from uart and output randump to uart
  *
  * @attention The function will not return after execution
  *
  * @param     regs : register list
  * @param     regs_len : register list length
  *
  * @return  None
  */
void wm_output_ramdump(unsigned int *regs, unsigned int regs_len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DEBUG_H__ */
