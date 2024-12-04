#ifndef __WM_SOC_H__
#define __WM_SOC_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function provides a delay in milliseconds.
 *
 * This function suspends the execution of the current task for the specified number of milliseconds.
 * It uses the system timer to measure the time elapsed.
 *
 * @param[in] ms The number of milliseconds to delay.
 *
 * @return This function does not return a value.
 *
 * @note This function is implemented as a macro and calls the underlying function \ref sys_msleep.
 *
 * @see sys_msleep
 */
void mdelay(uint32_t ms);

/**
 * @brief This function provides a delay in microseconds.
 *
 * This function suspends the execution of the current task for the specified number of microseconds.
 * It uses the system timer to measure the time elapsed.
 *
 * @param[in] us The number of microseconds to delay.
 *
 * @return This function does not return a value.
 *
 * @note This function is implemented as a macro and calls the underlying function \ref sys_usleep.
 *
 * @see sys_usleep
 */
void udelay(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif /* __WM_SOC_H__ */
