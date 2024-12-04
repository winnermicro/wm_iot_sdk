#ifndef WM_DRV_RCC_H
#define WM_DRV_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_hal_rcc.h"
#include "wm_soc_cfgs.h"
#include "wm_osal.h"
#include "wm_dt.h"

/**
 * @defgroup WM_RCC_Structs WM RCC Structs
 * @brief WinnerMicro RCC Structs
 */

/**
 * @addtogroup WM_RCC_Structs
 * @{
 */

typedef struct {
    wm_os_mutex_t *lock;
    wm_hal_clock_dev_t hal_dev;
} wm_drv_clock_ctx_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_RCC_APIs WM DRV RCC APIs
 * @brief WinnerMicro DRV RCC APIs
 */

/**
 * @addtogroup WM_DRV_RCC_APIs
 * @{
 */

/**
 * @brief Set the rcc clock enable
 *
 * @param[in] dev  clock device pointer
 * @param[in] module_type config the indicate module clock enable, please refer to #wm_rcc_clock_enable_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 *
 *
 */
int wm_drv_clock_enable(wm_device_t *dev, uint32_t module_type);

/**
 * @brief Set the rcc clock disable
 * @param[in] dev  clock device pointer
 * @param[in] module_type config the indicate module clock disable, please refer to #wm_rcc_clock_enable_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 *
 */
int wm_drv_clock_disable(wm_device_t *dev, uint32_t module_type);

/**
 * @brief Set the rcc clock reset
 * @param[in] dev  clock device pointer
 * @param[in] module_type config the indicate module clock reset, please refer to #wm_rcc_clk_rst_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 *
 *
 */
int wm_drv_clock_reset(wm_device_t *dev, uint32_t module_type);

/**
  * @brief     Initialize clock driver
  *
  * @param dev_name  clock device name
  *
  * @return
  *    - clock device pointer: succeed
  *    - NULL : failed
  */
wm_device_t *wm_drv_clock_init(char *dev_name);

/**
  * @brief     deinit flash driver
  *
  * @param dev  clock driver device
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_clock_deinit(wm_device_t *dev);

/**
 * @brief Set the rcc clock
 *
 * @param[in] dev  clock device pointer
 * @param[in] module_type config the indicate module clock, please refer to #wm_rcc_type_t
 * @param[in] MHz the clock setting, unit: MHz
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 *
 * @note The Periphal/WLAN/CPU clock should be a divider of PLL clock(480MHz), others value is invalid;
 *       The SD ADC clock should be a divider of 40MHz;
 *       The wlan clock should be 160MHz to work normal;
 *       The CPU clock should above 40MHz to make wifi work, and maxmum clock is 240MHz;
 *       The QFLASH clock should be 40MHz or 80MHz;
 *       The GPSEC clock should be 80MHz or 160MHz;
 *       The RSA clock should be 80MHz or 160MHz;
 *       No need to config APB clock, used for query;
 */
int wm_drv_rcc_config_clock(wm_device_t *dev, wm_rcc_type_t module_type, uint16_t MHz);

/**
 * @brief Get the rcc clock
 *
 * @param[in] dev  clock device pointer
 * @param[in] module_type config the clock module, please refer to #wm_rcc_type_t
 *
 * @return
 *    - module clock: succeed
 *    - others: failed
 */
int wm_drv_rcc_get_config_clock(wm_device_t *dev, wm_rcc_type_t module_type);

/**
 * @brief Set the I2S clock of RCC
 *
 * @param[in] dev  clock device pointer
 * @param[in] extal_en true if enable clock from extal crystal, otherwise use internal PLL
 * @param[in] mclk_en true if enable mclk function, otherwise disable
 * @param[in] mclk_div divider value for mclk, range [2, 64]
 * @param[in] bclk_div divider value for bclk, range [1, 1024]
 *
 * @return
 *    - module clock: succeed
 *    - others: failed
 */
int32_t wm_drv_rcc_set_i2s_clock(wm_device_t *dev, bool extal_en, bool mclk_en, uint32_t mclk_div, uint32_t bclk_div);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* WM_DRV_RCC_H */
