/**
 * @file wm_hal_sdh.h
 *
 * @brief sd/sdio host hardware abstraction layer header file
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

#ifndef __WM_HAL_SDH_H__
#define __WM_HAL_SDH_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_hal_dma.h"
#include "wm_ll_sdh.h"
#include "wm_hal_rcc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HAL_SDH_Macros WM HAL_SDH Macros
 * @brief WinnerMicro HAL_SDH Macros
 */

/**
 * @addtogroup WM_HAL_SDH_Macros
 * @{
 */

#define WM_HAL_SDH_MALLOC                              malloc /**< Allocates memory for SD/SDIO operations. */
#define WM_HAL_SDH_FREE                                free   /**< Frees memory used for SD/SDIO operations. */
/**
 * @brief Sets a target memory area to zero.
 *
 * @param[in] target Pointer to the target memory area.
 * @param[in] size The size of the memory area to set to zero.
 */
#define WM_HAL_SDH_MEM_ZERO(target, size)              memset(target, 0, size)

#define WM_HAL_SDH_CMD_RSP_BUF_LEN                     (4)    /**< The length of the command response buffer. */
#define WM_HAL_SDH_CMD_NUM_BASE                        (0x40) /**< The base value for the command number. */

#define WM_HAL_SDH_WAIT_SINGLE_BLOCK_TRAN_DONE_TIMEOUT (10) /**< The timeout value for single block transfer completion. */
#define WM_HAL_SDH_WAIT_MULTI_BLOCKS_TRAN_DONE_TIMEOUT (50) /**< The timeout value for multiple blocks transfer completion. */

#define WM_HAL_SDH_MMC_TIMEOUTCNT \
    0xFF //Time=Scale*bit[7:0],the Scale is defined by the register mmc_io_mbctl bits [7:6] or [5:4].
#define WM_HAL_SDH_SPI_TIMEOUTCNT \
    0x00 //Time=Scale*bit[7:0],the Scale is defined by the register mmc_io_mbctl bits [7:6] or [5:4].

#define WM_HAL_SDH_CLK_1M              (1000000) /**< Clock frequency 1MHz */
#define WM_HAL_SDH_CLR_MMC_INT_ALL     (0x7FF)   /**< Clear all MMC interrupts, clr_mmc_int[8:0] */
#define WM_HAL_SDH_BUF_CTRL_PIPELINE_1 (0x01)    /**< Pipeline setting 1 */
#define WM_HAL_SDH_BUF_CTRL_PIPELINE_8 (0x08)    /**< Pipeline setting 8 */

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SDH_Enumerations WM HAL_SDH Enumerations
 * @brief WinnerMicro HAL_SDH Enumerations
 */

/**
 * @addtogroup WM_HAL_SDH_Enumerations
 * @{
 */

/**
 * @brief SD/SDIO command numbers.
 *
 * This enumeration defines the standard command numbers used in SD/SDIO
 * communication protocols.
 */
typedef enum {
    WM_HAL_SDH_CMD_NUM_0  = 0,  /**< Command number 0 */
    WM_HAL_SDH_CMD_NUM_2  = 2,  /**< Command number 2 */
    WM_HAL_SDH_CMD_NUM_3  = 3,  /**< Command number 3 */
    WM_HAL_SDH_CMD_NUM_5  = 5,  /**< Command number 5 */
    WM_HAL_SDH_CMD_NUM_6  = 6,  /**< Command number 6 */
    WM_HAL_SDH_CMD_NUM_7  = 7,  /**< Command number 7 */
    WM_HAL_SDH_CMD_NUM_8  = 8,  /**< Command number 8 */
    WM_HAL_SDH_CMD_NUM_9  = 9,  /**< Command number 9 */
    WM_HAL_SDH_CMD_NUM_10 = 10, /**< Command number 10 */
    WM_HAL_SDH_CMD_NUM_11 = 11, /**< Command number 11 */
    WM_HAL_SDH_CMD_NUM_12 = 12, /**< Command number 12 */
    WM_HAL_SDH_CMD_NUM_13 = 13, /**< Command number 13 */
    WM_HAL_SDH_CMD_NUM_16 = 16, /**< Command number 16 */
    WM_HAL_SDH_CMD_NUM_17 = 17, /**< Command number 17 */
    WM_HAL_SDH_CMD_NUM_18 = 18, /**< Command number 18 */
    WM_HAL_SDH_CMD_NUM_22 = 22, /**< Command number 22 */
    WM_HAL_SDH_CMD_NUM_23 = 23, /**< Command number 23 */
    WM_HAL_SDH_CMD_NUM_24 = 24, /**< Command number 24 */
    WM_HAL_SDH_CMD_NUM_25 = 25, /**< Command number 25 */
    WM_HAL_SDH_CMD_NUM_41 = 41, /**< Command number 41 */
    WM_HAL_SDH_CMD_NUM_52 = 52, /**< Command number 52 */
    WM_HAL_SDH_CMD_NUM_53 = 53, /**< Command number 53 */
    WM_HAL_SDH_CMD_NUM_55 = 55  /**< Command number 55 */
} wm_hal_sdh_cmd_num_t;

/**
 * @brief SD/SDIO port mode.
 *
 * This enumeration defines the operating mode of the SD/SDIO port.
 */
typedef enum {
    WM_HAL_SDH_PORT_MODE_SPI = 0x00, /**< SPI mode */
    WM_HAL_SDH_PORT_MODE_MMC = 0x01, /**< MMC mode */
    WM_HAL_SDH_PORT_MODE_MAX
} wm_hal_sdh_port_mode_t;

/**
 * @brief SD/SDIO transfer mode.
 *
 * This enumeration defines the speed mode for data transfers in SD/SDIO operations.
 */
typedef enum {
    WM_HAL_SDH_XFER_MODE_SPEED_LOW  = 0x00, /**< Low-Speed Mode */
    WM_HAL_SDH_XFER_MODE_SPEED_HIGH = 0x01, /**< High-Speed Mode */
    WM_HAL_SDH_XFER_MODE_MAX
} wm_hal_sdh_xfer_mode_t;

/**
 * @brief SD/SDIO mode.
 *
 * This enumeration defines the mode of operation for the SD/SDIO device.
 */
typedef enum {
    WM_HAL_SDH_SD_MODE_SDMMC = 0x00, /**< SDMMC Mode */
    WM_HAL_SDH_SD_MODE_SDIO  = 0x01, /**< SDIO Mode */
    WM_HAL_SDH_SD_MODE_MAX
} wm_hal_sdh_sd_mode_t;

/**
 * @brief SPI transfer mode.
 *
 * This enumeration defines the transfer mode for SPI operations.
 */
typedef enum {
    WM_HAL_SDH_SPI_TX = 0x01, /**< SPI TX mode */
    WM_HAL_SDH_SPI_RX = 0x02, /**< SPI RX mode */
    WM_HAL_SDH_SPI_MAX,
} wm_hal_sdh_spi_xfer_mode_t;

/**
 * @brief SDMMC transfer mode.
 *
 * This enumeration defines the transfer mode for SDMMC operations.
 */
typedef enum {
    WM_HAL_SDH_SDMMC_TX = 0x01, /**< SDMMC TX mode */
    WM_HAL_SDH_SDMMC_RX = 0x02, /**< SDMMC RX mode */
    WM_HAL_SDH_SDMMC_MAX,
} wm_hal_sdh_sdmmc_xfer_mode_t;

/**
 * @brief SD/SDIO enable/disable status.
 *
 * This enumeration defines the enable/disable status for various SD/SDIO operations.
 */
typedef enum {
    WM_HAL_SDH_DISABLE = 0x00, /**< Disable */
    WM_HAL_SDH_ENABLE  = 0x01, /**< Enable */
    WM_HAL_SDH_MAX
} wm_hal_sdh_enable_t;

/**
 * @brief SD/SDIO port clock selection.
 *
 * This enumeration defines the clock selection for the SD/SDIO port.
 */
typedef enum {
    WM_HAL_SDH_PORT_CLK_SEL_1_2  = 0x00, /**< 1/2 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_1_4  = 0x01, /**< 1/4 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_1_6  = 0x02, /**< 1/6 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_1_8  = 0x03, /**< 1/8 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_1_10 = 0x04, /**< 1/10 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_1_12 = 0x05, /**< 1/12 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_1_14 = 0x06, /**< 1/14 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_1_16 = 0x07, /**< 1/16 base clock */
    WM_HAL_SDH_PORT_CLK_SEL_MAX
} wm_hal_sdh_port_clk_sel_t;

/**
 * @brief SD/SDIO driver mode.
 *
 * This enumeration defines the driver mode for the SD/SDIO pins.
 */
typedef enum {
    WM_HAL_SDH_DRIVER_MODE_PUSH_PULL  = 0x00, /**< Push-Pull Mode */
    WM_HAL_SDH_DRIVER_MODE_OPEN_DRAIN = 0x01, /**< Open-Drain Mode */
    WM_HAL_SDH_DRIVER_MODE_MAX,
} wm_hal_sdh_driver_mode_t;

/**
 * @brief SD/SDIO signal mode selection.
 *
 * This enumeration defines the signal mode selection for the SD/SDIO interface.
 */
typedef enum {
    WM_HAL_SDH_SIGNAL_MODE_MANUAL = 0x00, /**< Manual signal mode selection */
    WM_HAL_SDH_SIGNAL_MODE_AUTO   = 0x01, /**< Automatic signal mode selection */
    WM_HAL_SDH_SIGNAL_MODE_MAX,
} wm_hal_sdh_signal_mode_t;

/**
 * @brief Command 12 I/O abort flag.
 *
 * This enumeration defines the status of Command 12, which can be used to abort I/O.
 */
typedef enum {
    WM_HAL_SDH_CMD12_IO_NOT_ABORT = 0x00, /**< Current command is not a cmd12/IO abort */
    WM_HAL_SDH_CMD12_IO_ABORT     = 0x01, /**< Current command is a cmd12/IO abort */
    WM_HAL_SDH_CMD12_IO_ABORT_MAX,
} wm_hal_sdh_cmd12_io_abort_flag_t;

/**
 * @brief Command attribute types.
 *
 * This enumeration specifies whether a command is followed by a data block or not.
 */
typedef enum {
    WM_HAL_SDH_CMD_ATTRIBUTE_NO_DATA      = 0x00, /**< No data block follows the command */
    WM_HAL_SDH_CMD_ATTRIBUTE_DATA_FOLLOWS = 0x01, /**< Data block follows the command */
    WM_HAL_SDH_CMD_ATTRIBUTE_MAX,
} wm_hal_sdh_cmd_attribute_t;

/**
 * @brief Auto-generated 8 clocks selection.
 *
 * This enumeration defines the conditions under which 8 auto-generated clocks are selected.
 */
typedef enum {
    WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_RESPONSE_OR_CMD = 0x00, /**< Select based on bit3 for response or command */
    WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_NULL            = 0x01, /**< Generate 8 NULL clocks */
    WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_MAX,
} wm_hal_sdh_auto_gen_8_clk_sel_t;

/**
 * @brief Auto-generated 8 clocks selection type.
 *
 * This enumeration specifies the type of selection for the auto-generated 8 clocks.
 */
typedef enum {
    WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_TYPE_SEND_CMD        = 0x00, /**< Send command */
    WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_TYPE_RECIVE_RESPONSE = 0x01, /**< Receive response */
    WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_TYPE_MAX,
} wm_hal_sdh_auto_gen_8_clk_sel_type_t;

/**
 * @brief Data transfer direction.
 *
 * This enumeration defines the direction of data transfer in SD/SDIO operations.
 */
typedef enum {
    WM_HAL_SDH_DATA_XFER_DIR_WRITE = 0x00, /**< Write data */
    WM_HAL_SDH_DATA_XFER_DIR_READ  = 0x01, /**< Read data */
    WM_HAL_SDH_DATA_XFER_DIR_MAX,
} wm_hal_sdh_data_xfer_dir_t;

/**
 * @brief Data CRC selection.
 *
 * This enumeration specifies which data line the CRC value is taken from.
 */
typedef enum {
    WM_HAL_SDH_DAT_CRC_SEL_DAT0 = 0x00, /**< Data CRC value on DAT0 line */
    WM_HAL_SDH_DAT_CRC_SEL_DAT1 = 0x01, /**< Data CRC value on DAT1 line */
    WM_HAL_SDH_DAT_CRC_SEL_DAT2 = 0x02, /**< Data CRC value on DAT2 line */
    WM_HAL_SDH_DAT_CRC_SEL_DAT3 = 0x03, /**< Data CRC value on DAT3 line */
    WM_HAL_SDH_DAT_CRC_SEL_MAX,
} wm_hal_sdh_dat_crc_sel_t;

/**
 * @brief NAC timeout interrupt scale.
 *
 * This enumeration defines the scale for the NAC timeout interrupt.
 */
typedef enum {
    WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_1_US   = 0x00, /**< 1 Microsecond */
    WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_100_US = 0x01, /**< 100 Microseconds */
    WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_10_MS  = 0x02, /**< 10 Milliseconds */
    WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_1_S    = 0x03, /**< 1 Second */
    WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_MAX,
} wm_hal_sdh_nac_timeout_int_scale_t;

/**
 * @brief Busy timeout interrupt scale.
 *
 * This enumeration defines the scale for the busy timeout interrupt.
 */
typedef enum {
    WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_1_US   = 0x00, /**< 1 Microsecond */
    WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_100_US = 0x01, /**< 100 Microseconds */
    WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_10_MS  = 0x02, /**< 10 Milliseconds */
    WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_1_S    = 0x03, /**< 1 Second */
    WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_MAX,
} wm_hal_sdh_busy_timeout_int_scale_t;

/**
 * @brief Port clock polarity settings.
 *
 * This enumeration defines the polarity settings for the port clock.
 */
typedef enum {
    WM_HAL_SDH_PORT_CLK_POL_0 = 0x00, /**< Send on falling clock edge, sample on rising edge */
    WM_HAL_SDH_PORT_CLK_POL_1 = 0x01, /**< Send on rising clock edge, sample on falling edge */
    WM_HAL_SDH_PORT_CLK_POL_MAX,
} wm_hal_sdh_port_clk_pol_t;

/**
 * @brief Data block transfer direction for multi-block transfers.
 *
 * This enumeration specifies the direction of data transfer for multi-block operations.
 */
typedef enum {
    WM_HAL_SDH_DATA_BLOCK_XFER_DIR_WRITE = 0x00, /**< Multi data block transfer direction is write data */
    WM_HAL_SDH_DATA_BLOCK_XFER_DIR_READ  = 0x01, /**< Multi data block transfer direction is read data */
    WM_HAL_SDH_DATA_BLOCK_XFER_DIR_MAX,
} wm_hal_sdh_data_block_xfer_dir_t;

/**
 * @brief Command buffer IDs for SD/SDIO commands.
 *
 * This enumeration defines the buffer IDs for storing command data.
 */
typedef enum {
    WM_HAL_SDH_CMD_BUF_ID_0  = 0x00,
    WM_HAL_SDH_CMD_BUF_ID_1  = 0x01,
    WM_HAL_SDH_CMD_BUF_ID_2  = 0x02,
    WM_HAL_SDH_CMD_BUF_ID_3  = 0x03,
    WM_HAL_SDH_CMD_BUF_ID_4  = 0x04,
    WM_HAL_SDH_CMD_BUF_ID_5  = 0x05,
    WM_HAL_SDH_CMD_BUF_ID_6  = 0x06,
    WM_HAL_SDH_CMD_BUF_ID_7  = 0x07,
    WM_HAL_SDH_CMD_BUF_ID_8  = 0x08,
    WM_HAL_SDH_CMD_BUF_ID_9  = 0x09,
    WM_HAL_SDH_CMD_BUF_ID_10 = 0x0A,
    WM_HAL_SDH_CMD_BUF_ID_11 = 0x0B,
    WM_HAL_SDH_CMD_BUF_ID_12 = 0x0C,
    WM_HAL_SDH_CMD_BUF_ID_13 = 0x0D,
    WM_HAL_SDH_CMD_BUF_ID_14 = 0x0E,
    WM_HAL_SDH_CMD_BUF_ID_15 = 0x0F,
    WM_HAL_SDH_CMD_BUF_ID_MAX,
} wm_hal_sdh_cmd_buf_id_t;

/**
 * @brief Buffer access direction for SD/SDIO data transfers.
 *
 * This enumeration defines the direction of access for the data buffer.
 */
typedef enum {
    WM_HAL_SDH_BUF_ACCESS_DIR_READ  = 0x00, /**< Buffer access direction is read */
    WM_HAL_SDH_BUF_ACCESS_DIR_WRITE = 0x01, /**< Buffer access direction is write */
    WM_HAL_SDH_BUF_ACCESS_DIR_MAX,
} wm_hal_sdh_buf_access_dir_t;

/**
 * @brief Interrupt status types for SD/SDIO host controller.
 *
 * This enumeration defines the various interrupt status types that can occur in the SD/SDIO host controller.
 */
typedef enum {
    WM_HAL_SDH_INT_CMD_DONE            = 0x00, /**< Command done interrupt status */
    WM_HAL_SDH_INT_DATA_DONE           = 0x01, /**< Data done interrupt status */
    WM_HAL_SDH_INT_DATA_CRC_ERR        = 0x02, /**< Data CRC error interrupt status */
    WM_HAL_SDH_INT_CMD_CRC_ERR         = 0x03, /**< Command CRC error interrupt status */
    WM_HAL_SDH_INT_MB_DATA_DONE        = 0x04, /**< Multi-block data done interrupt status */
    WM_HAL_SDH_INT_MB_DATA_TIMEOUT     = 0x05, /**< Multi-block data timeout interrupt status */
    WM_HAL_SDH_INT_CMD_RSP_NCR_TIMEOUT = 0x06, /**< Command response non-critical timeout interrupt status */
    WM_HAL_SDH_INT_CRC_TOKEN_ERR       = 0x07, /**< CRC token error interrupt status */
    WM_HAL_SDH_INT_DATA_LINE_1         = 0x08, /**< Data line 1 interrupt status */
    WM_HAL_SDH_INT_STS_MAX,
} wm_hal_sdh_int_sts_t;

/**
 * @brief Event types for SD/SDIO host controller operations.
 *
 * This enumeration defines the types of events that can be generated by the SD/SDIO host controller.
 */
typedef enum {
    WM_HAL_SDH_EVT_READ_DONE  = 0x01, /**< Event indicating read operation completed successfully */
    WM_HAL_SDH_EVT_READ_ERR   = 0x02, /**< Event indicating an error occurred during read operation */
    WM_HAL_SDH_EVT_WRITE_DONE = 0x03, /**< Event indicating write operation completed successfully */
    WM_HAL_SDH_EVT_WRITE_ERR  = 0x04, /**< Event indicating an error occurred during write operation */
    WM_HAL_SDH_EVT_MAX                /**< Maximum event type value */
} wm_hal_sdh_evt_type_t;

/**
 * @brief Structure representing an SD/SDIO host controller event.
 *
 * This structure defines the event generated by the SD/SDIO host controller, including the type of event
 * and a flag indicating whether the event is related to DMA (Direct Memory Access).
 */
typedef struct {
    wm_hal_sdh_evt_type_t type; /**< The type of event generated by the SD/SDIO host controller */
    bool is_dma_evt;            /**< Flag indicating if the event is related to DMA operations */
} wm_hal_sdh_evt_t;

/**
 * @brief Command response types.
 *
 * This enumeration specifies the type of response expected from an SD/SDIO command.
 */
typedef enum {
    WM_HAL_SDH_CMD_NO_RSP   = 0x00, /**< Command does not expect a response */
    WM_HAL_SDH_CMD_WAIT_RSP = 0x01, /**< Command expects a response */
    WM_HAL_SDH_CMD_RSP_MAX,         /**< Maximum command response type value */
} wm_hal_sdh_cmd_rsp_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SDH_Type_Definitions WM HAL_SDH Type Definitions
 * @brief WinnerMicro HAL_SDH Type Definitions
 */

/**
 * @addtogroup WM_HAL_SDH_Type_Definitions
 * @{
 */

/**
 * @brief SD/SDIO Host Controller Callback Function Type Definition.
 *
 * This typedef defines the prototype of the callback function that can be registered
 * with the SD/SDIO host controller to handle asynchronous events such as completion
 * of data transfers or occurrence of errors. The callback function is called with
 * a pointer to the HAL device, a pointer to an event structure indicating the type
 * of event, and a pointer to user-defined data.
 *
 * @param[in] hal_dev A pointer to the SD/SDIO host controller HAL device structure.
 * @param[in] evt A pointer to the event structure that contains information about the event.
 * @param[in] user_data A pointer to user-defined data that was passed when registering the callback.
 *
 * @return none
 */
typedef void (*wm_hal_sdh_callback_t)(void *hal_dev, wm_hal_sdh_evt_t *evt, void *user_data);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SDH_Structures WM HAL_SDH Structures
 * @brief WinnerMicro HAL_SDH Structures
 */

/**
 * @addtogroup WM_HAL_SDH_Structures
 * @{
 */

/**
 * @brief SD/SDIO command structure.
 *
 * This structure defines the attributes of an SD/SDIO command, including the command number,
 * argument, response type, response buffer, response length, timeout value, and I/O flags.
 */
typedef struct {
    uint32_t cmd_id;                              /**< SD/SDIO command number */
    uint32_t cmd_arg;                             /**< Command argument */
    wm_hal_sdh_cmd_rsp_t cmd_rsp_type;            /**< Type of response expected from the command */
    uint32_t cmd_rsp[WM_HAL_SDH_CMD_RSP_BUF_LEN]; /**< Buffer to store the command response */
    uint32_t cmd_rsp_len;                         /**< Length of the response buffer */
    uint32_t cmd_timeout;                         /**< Timeout value for the command */
    uint32_t cmd_io;                              /**< I/O flags for the command */
} wm_hal_sdh_cmd_t;

/**
 * @brief SD/SDIO data descriptor structure.
 *
 * This structure is used to describe the data to be transferred via SD/SDIO, including
 * the data buffer, data length, timeout value, and a pointer to the next data descriptor.
 */
typedef struct wm_hal_sdh_data_desc_s {
    uint8_t *buf;                        /**< Pointer to the data buffer */
    uint32_t len;                        /**< Length of the data to be transferred */
    uint32_t timeout;                    /**< Timeout value for the data transfer */
    struct wm_hal_sdh_data_desc_s *next; /**< Pointer to the next data descriptor in a linked list */
} wm_hal_sdh_data_desc_t;

/**
 * @brief SD/SDIO data transfer structure.
 *
 * This structure represents the data to be transmitted (TX) or received (RX) and includes
 * the data descriptor for each direction.
 */
typedef struct {
    wm_hal_sdh_data_desc_t tx; /**< Data descriptor for transmit direction */
    wm_hal_sdh_data_desc_t rx; /**< Data descriptor for receive direction */
} wm_hal_sdh_data_t;

/**
 * @brief SDIO transfer descriptor structure.
 *
 * This structure defines the parameters for an SDIO transfer, including the transfer mode,
 * SD command, and data information.
 */
typedef struct {
    wm_hal_sdh_sdmmc_xfer_mode_t mode; /**< SDIO transfer mode */
    wm_hal_sdh_cmd_t sd_cmd;           /**< SD command for the transfer */
    wm_hal_sdh_data_t sd_data;         /**< Data information for the transfer */
} wm_hal_sdh_sdio_xfer_desc_t;

/**
 * @brief SPI transfer descriptor structure.
 *
 * This structure defines the parameters for an SPI transfer, including the transfer mode
 * and data information.
 */
typedef struct {
    wm_hal_sdh_spi_xfer_mode_t mode; /**< SPI transfer mode */
    wm_hal_sdh_data_t spi_data;      /**< Data information for the transfer */
} wm_hal_sdh_spi_xfer_desc_t;

/**
 * @brief SD/SDIO transfer descriptor structure.
 *
 * This structure is a union of SDIO and SPI transfer descriptors, allowing it to represent
 * either type of transfer.
 */
typedef struct {
    uint32_t port_mode; /**< Port mode, either SDIO or SPI */
    union {
        wm_hal_sdh_sdio_xfer_desc_t sdio_xfer_desc; /**< SDIO transfer descriptor */
        wm_hal_sdh_spi_xfer_desc_t spi_xfer_desc;   /**< SPI transfer descriptor */
    };
} wm_hal_sdh_xfer_desc_t;

/**
 * @brief SD/SDIO callback information structure.
 *
 * This structure holds the callback function and private data associated with
 * SD/SDIO transfer completion or error events.
 */
typedef struct wm_hal_sdh_cb_info_s {
    wm_hal_sdh_callback_t cb; /**< Pointer to the callback function */
    void *cb_priv;            /**< Pointer to private data passed to the callback function */
} wm_hal_sdh_cb_info_t;

/**
 * @brief SD/SDIO context structure.
 *
 * This structure contains the context information for SD/SDIO transfers, including
 * callback information for transmit and receive operations, and a pointer to the
 * receive data descriptor.
 */
typedef struct {
    wm_hal_sdh_cb_info_t tx_cb;         /**< Callback information for transmit operations */
    wm_hal_sdh_cb_info_t rx_cb;         /**< Callback information for receive operations */
    wm_hal_sdh_data_desc_t *sd_rx_data; /**< Pointer to the receive data descriptor */
} wm_hal_sdh_ctx_t;

/**
 * @brief SD/SDIO configuration structure.
 *
 * This structure defines the configuration for the SD/SDIO host controller, including
 * the clock frequency, transfer mode, bus width, port mode, SD mode, and reserved bits.
 */
typedef struct {
    uint32_t clock_hz; /**< Clock frequency for the SD/SDIO host controller */
    struct {
        wm_hal_sdh_xfer_mode_t xfer_mode : 2;  /**< Transfer mode of the SD/SDIO host controller */
        wm_sdh_bus_width_t bus_width     : 2;  /**< Bus width of the SD/SDIO host controller */
        wm_hal_sdh_port_mode_t port_mode : 2;  /**< Port mode of the SD/SDIO host controller */
        wm_hal_sdh_sd_mode_t sd_mode     : 2;  /**< SD mode of the SD/SDIO host controller */
        uint32_t reserved                : 24; /**< Reserved bits for future use */
    };
} wm_hal_sdh_cfg_t;

/**
 * @brief SD/SDIO host controller device structure.
 *
 * This structure represents the SD/SDIO host controller device and includes the register
 * base address, interrupt number, interrupt priority, high frequency clock, configuration,
 * DMA channel, pointers to DMA and clock devices, and the context information.
 */
typedef struct {
    wm_sdh_reg_t *reg_base;          /**< Register base address of the SD/SDIO host controller */
    wm_irq_no_t irq_num;             /**< Interrupt number of the SD/SDIO host controller */
    uint8_t irq_priority;            /**< Interrupt priority of the SD/SDIO host controller */
    uint32_t fhclk;                  /**< High frequency clock for the SD/SDIO host controller */
    wm_hal_sdh_cfg_t sdh_cfg;        /**< Configuration of the SD/SDIO host controller */
    uint8_t dma_ch;                  /**< DMA channel used for SD/SDIO transfers */
    wm_hal_dma_dev_t *dma_hal_dev;   /**< Pointer to the DMA device used for SD/SDIO transfers */
    wm_hal_clock_dev_t *rcc_hal_dev; /**< Pointer to the clock device used for SD/SDIO transfers */
    wm_hal_sdh_ctx_t ctx;            /**< Context information for SD/SDIO transfers */
} wm_hal_sdh_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SDH_Functions WM HAL_SDH Functions
 * @brief WinnerMicro HAL_SDH Functions
 */

/**
 * @addtogroup WM_HAL_SDH_Functions
 * @{
 */

/**
 * @brief Initializes the SD/SDIO host controller.
 *
 * This function sets up the SD/SDIO host controller with default configurations.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_sdh_init(wm_hal_sdh_dev_t *dev);

/**
 * @brief Deinitializes the SD/SDIO host controller.
 *
 * This function disables the SD/SDIO host controller and releases the resources.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_sdh_deinit(wm_hal_sdh_dev_t *dev);

/**
 * @brief Sets the bus width for the SD/SDIO host controller.
 *
 * This function configures the bus width used for data transfers.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 * @param[in] width The bus width to be set.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the bus width setting process
 */
int wm_hal_sdh_set_bus_width(wm_hal_sdh_dev_t *dev, wm_sdh_bus_width_t width);

/**
 * @brief Sets the clock frequency for the SD/SDIO host controller.
 *
 * This function configures the clock frequency for the SD/SDIO bus.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 * @param[in] clock The clock frequency in Hertz.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the clock setting process
 */
int wm_hal_sdh_set_clock(wm_hal_sdh_dev_t *dev, uint32_t clock);

/**
 * @brief Performs a data transfer in polling mode.
 *
 * This function transfers data to or from the SD/SDIO device using polling mode.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 * @param[in,out] xfer_desc A pointer to the wm_hal_sdh_xfer_desc_t structure describing the transfer.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - WM_ERR_TIMEOUT: timeout occurred during transfer
 *    - Other error codes based on the polling transfer process
 */
int wm_hal_sdh_xfer_polling(wm_hal_sdh_dev_t *dev, wm_hal_sdh_xfer_desc_t *xfer_desc);

/**
 * @brief Performs a data transfer using DMA.
 *
 * This function transfers data to or from the SD/SDIO device using DMA.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 * @param[in,out] xfer_desc A pointer to the wm_hal_sdh_xfer_desc_t structure describing the transfer.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - WM_ERR_TIMEOUT: timeout occurred during DMA transfer
 *    - Other error codes based on the DMA transfer process
 */
int wm_hal_sdh_xfer_dma(wm_hal_sdh_dev_t *dev, wm_hal_sdh_xfer_desc_t *xfer_desc);

/**
 * @brief Performs a data transfer using interrupt mode.
 *
 * This function transfers data to or from the SD/SDIO device using interrupt mode.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 * @param[in,out] xfer_desc A pointer to the wm_hal_sdh_xfer_desc_t structure describing the transfer.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - WM_ERR_TIMEOUT: timeout occurred during interrupt-driven transfer
 *    - Other error codes based on the interrupt-driven transfer process
 */
int wm_hal_sdh_xfer_it(wm_hal_sdh_dev_t *dev, wm_hal_sdh_xfer_desc_t *xfer_desc);

/**
 * @brief Registers a callback function for transmit completion events.
 *
 * This function allows the user to register a callback function that will be called
 * when a transmit operation completes.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 * @param[in] xfer_cb A pointer to the callback function.
 * @param[in] user_priv A pointer to user-defined data that will be passed to the callback function.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the registration process
 */
int wm_hal_sdh_tx_register_callback(wm_hal_sdh_dev_t *dev, wm_hal_sdh_callback_t xfer_cb, void *user_priv);

/**
 * @brief Unregisters the callback function for transmit completion events.
 *
 * This function removes the previously registered callback function for transmit operations.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the unregistration process
 */
int wm_hal_sdh_tx_unregister_callback(wm_hal_sdh_dev_t *dev);

/**
 * @brief Registers a callback function for receive completion events.
 *
 * This function allows the user to register a callback function that will be called
 * when a receive operation completes.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 * @param[in] xfer_cb A pointer to the callback function.
 * @param[in] user_priv A pointer to user-defined data that will be passed to the callback function.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the registration process
 */
int wm_hal_sdh_rx_register_callback(wm_hal_sdh_dev_t *dev, wm_hal_sdh_callback_t xfer_cb, void *user_priv);

/**
 * @brief Unregisters the callback function for receive completion events.
 *
 * This function removes the previously registered callback function for receive operations.
 *
 * @param[in] dev A pointer to the wm_hal_sdh_dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the unregistration process
 */
int wm_hal_sdh_rx_unregister_callback(wm_hal_sdh_dev_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_SDH_H__ */
