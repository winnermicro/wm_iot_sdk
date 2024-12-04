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

#ifndef __WM_HAL_SPI_MASTER__
#define __WM_HAL_SPI_MASTER__

#include "wm_spi_reg_struct.h"
#include "wm_ll_spi.h"
#include "wm_irq.h"
#include "wm_hal_dma.h"
#include "wm_hal_rcc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**    default configuration data.    */
#define WM_SPIM_DEFAULT_CLOCK (2 * 1000000)  /** default clock rate is 2MHz.*/
#define WM_SPIM_MAX_CLOCK     (20 * 1000000) /** W800 max clock is 20MHz */
#define WM_SPIM_MIN_CLOCK     (10000)        /** if less this value, maybe lost data in transceive */

typedef enum {
    SPIM_FORMAT_MOTO = 0, //motorola
    SPIM_FORMAT_TI,       //TI
    SPIM_FORMAT_MICR,     //microwire
    SPIM_FORMAT_MAX       //RSV
} wm_spim_format_e;

typedef enum {
    SPI_MODE_0 = 0, //CPOL=0, CPHA=0
    SPI_MODE_1,     //CPOL=0, CPHA=1
    SPI_MODE_2,     //CPOL=1, CPHA=0
    SPI_MODE_3,     //CPOL=1, CPHA=1
    SPI_MODE_MAX
} wm_spi_trans_mode_e; //only valid for motorola frame format

typedef enum {
    SPIM_CMD_FRAM_FORMAT = 0,  // reference enum: wm_spim_format_e
    SPIM_CMD_RX_INVALID_BIT,   //how many bits will be dropped after start recevie data
    SPIM_CMD_DATA_OUT_DELAY,   //base on APB clock[0, 3]
    SPIM_CMD_FRMAE_INTERVAL,   //base on SPI clock[0, 7]
    SPIM_CMD_BIG_ENDIAN,       //0: transmit low byte first, 1: transmit high byte first
    SPIM_CMD_CS_CTRL_BY_SW,    //0: control by SW, 1: control by HW
    SPIM_CMD_CS_INVALID_VALUE, //0: SPI_CS invaid singal is 0,  1: SPI_CS invaid singal is 1
    SPIM_CMD_FREQ,             //set SPI frequency
    SPIM_CMD_MODE,             //reference enum: wm_spi_trans_mode_e
    SPIM_CMD_MAX
} spim_cmd_e;

typedef struct {
    bool cs_active;           //0: SPI_CS invalid singal is 0,  1: SPI_CS invalid singal is 1
    uint32_t clock;           //unit is HZ
    uint8_t cs_control_by_sw; //0: SPI_CS conotrol by HW, 1: control by SW
} hal_spim_config_t;

typedef struct {
    bool big_endian; // if set, transmit high address byte first
    uint8_t irq_no;
    uint8_t tx_dma_channel;
    uint8_t rx_dma_channel;
    uint8_t cmd_len;  /**< [input] command length, unit is byte */
    uint8_t addr_len; /**< [input] address length, unit is byte */
    bool cmd_small_endian;
    bool addr_small_endian;
    uint8_t dummy_bits; /**< [input] dummy length, unit is bit, must set SPI_TRANS_DUMMY_BITS flag if want to use this member */
    uint16_t cmd;       /**< [input] tx command data, if you want to use this member must set "cmd_len" at the same time */
    uint32_t addr;      /**< [input] tx address data, if you want to use this member must set "addr_len at the same time */
    uint32_t register_base;
    wm_hal_dma_dev_t *dma_hal_dev;
    wm_hal_dma_desc_t tx_dma_cfg;
    wm_hal_dma_desc_t rx_dma_cfg;
    wm_hal_clock_dev_t *rcc_hal_dev;
    void *priv;
} wm_hal_spim_dev_t;

typedef void (*wm_hal_spim_xfer_callback_t)(void *user_data, int succ);

/**
  * @brief     Initialize SPI master HAL layer
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] config SPI config
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  */
int wm_hal_spim_init(wm_hal_spim_dev_t *dev, hal_spim_config_t *config);

/**
  * @brief     deinitialize SPI master HAL layer
  *
  * @param[in] dev SPI master HAL layer pointer
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  */
int wm_hal_spim_deinit(wm_hal_spim_dev_t *dev);

/**
  * @brief SPI transmit data by polling, API return until TX done, synchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] tx_buf tx buf pointer
  * @param[in] tx_len tx length
  * @param[in] timeout_ms max wait time
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  */
int wm_hal_spim_tx_polling(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint32_t timeout_ms);

/**
  * @brief SPI transmit data by interrupt， asynchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] tx_buf tx buf pointer
  * @param[in] tx_len tx length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: callback will be trigger after HAL layer transmit done
  */
int wm_hal_spim_tx_it(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len);

/**
  * @brief SPI transmit data by DMA, asynchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] tx_buf tx buf pointer
  * @param[in] tx_len tx length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: callback will be trigger after HAL layer transmit done
  */
int wm_hal_spim_tx_dma(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len);

/**
  * @brief SPI receive data by polling, API return until receive done, synchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] rx_buf rx buf pointer
  * @param[in] rx_len rx length
  * @param[in] timeout_ms max wait time
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  */
int wm_hal_spim_rx_polling(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len, uint32_t timeout_ms);

/**
  * @brief SPI receive data by interrupt， asynchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] rx_buf rx buf pointer
  * @param[in] rx_len rx length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: callback will be trigger after HAL layer receive done
  */
int wm_hal_spim_rx_it(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len);

/**
  * @brief SPI receive data by DMA, asynchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] rx_buf rx buf pointer
  * @param[in] rx_len rx length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: callback will be trigger after HAL layer receive done
  */
int wm_hal_spim_rx_dma(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len);

/**
  * @brief SPI transceive data by polling, synchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] tx_buf tx buf pointer
  * @param[in] tx_len tx length
  * @param[in] rx_buf rx buf pointer
  * @param[in] rx_len rx length
  * @param[in] timeout_ms max wait time for transceive
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: tx buf and buf cannot as null at the same time
  */
int wm_hal_spim_tx_rx_polling(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len,
                              uint32_t timeout_ms);

/**
  * @brief SPI transceive data by interrupt, asynchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] tx_buf tx buf pointer
  * @param[in] tx_len tx length
  * @param[in] rx_buf rx buf pointer
  * @param[in] rx_len rx length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: 1:tx buf and buf cannot as null at the same time
  *     2:callback will be trigger after HAL layer receive done
  */
int wm_hal_spim_tx_rx_it(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);

/**
  * @brief SPI transceive data by DMA, asynchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] tx_buf tx buf pointer
  * @param[in] tx_len tx length
  * @param[in] rx_buf rx buf pointer
  * @param[in] rx_len rx length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: 1:tx buf and buf cannot as null at the same time
  *     2:callback will be trigger after HAL layer receive done
  */
int wm_hal_spim_tx_rx_dma(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);

/**
  * @brief SPI transceive data by DMA, synchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] tx_buf tx buf pointer
  * @param[in] tx_len tx length
  * @param[in] rx_buf rx buf pointer
  * @param[in] rx_len rx length
  * @param[in] timeout_ms max wait time for transceive
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  * Note: 1:tx buf and buf cannot as null at the same time
  */
int wm_hal_spim_tx_rx_dma_sync(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len,
                               uint32_t timeout_ms);

/**
  * @brief register transceive done callback for synchronization API
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] spim_callback call back function
  * @param[in] user_data the argument for spim_callback
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  */
int wm_hal_spim_register_xfer_done_callback(wm_hal_spim_dev_t *dev, wm_hal_spim_xfer_callback_t spim_callback, void *user_data);

/**
  * @brief config SPI for current transceive
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] cmd config flag
  * @param[in] value config value
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  */
int wm_hal_spim_set_cmds(wm_hal_spim_dev_t *dev, spim_cmd_e cmd, uint32_t value);

/**
  * @brief get current SPI config
  *
  * @param[in] dev SPI master HAL layer pointer
  * @param[in] cmd config flag
  * @param[out] value config value
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  *
  */
int wm_hal_spim_get_cmd_value(wm_hal_spim_dev_t *dev, spim_cmd_e cmd, uint16_t *value);

#ifdef __cplusplus
}
#endif

#endif
