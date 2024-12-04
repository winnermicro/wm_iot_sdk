/**
 * @file wm_sdh_reg_struct.h
 *
 * @brief SD/SDIO host register struct
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

#ifndef __WM_SDH_REG_STRUCT_H__
#define __WM_SDH_REG_STRUCT_H__

#include "wm_types.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SD/SDIO host mmc control register
 */
typedef union {
    struct {
        uint32_t port_mode           : 1; /**< [0] RW - Port mode selection - 1 bit
                                            *   '1': MMC mode
                                            *   '0': SPI mode */
        uint32_t sig_mode            : 1; /**< [1] RW - Signal mode selection - 1 bit
                                            *   '1': Automatically select transmission mode
                                            *   '0': Use mmc_port register selection */
        uint32_t sdio_drive_mode_sel : 1; /**< [2] RW - SDIO/MMC/SDIO interface drive mode selection - 1 bit
                                            *   '1' : open_DrainMode
                                            *   '0': Push-Pull Mode */
        uint32_t sdio_port_clock_sel : 3; /**< [5:3] RW - SDIO/MMC/SDIO port clock rate selection - 3 bits
                                            *   Refer to Table 2 */
        uint32_t sdio_transfer_mode  : 1; /**< [6] RW - SD/MMC/SDIO transfer mode - 1 bit
                                            *   '1': High-Speed Mode
                                            *   '0' : Low-Speed Mode */
        uint32_t sdio_bus_width      : 1; /**< [7] RW - SD/MMC/SDIO interface bus width - 1 bit
                                            *   '1' : 4 bits
                                            *   '0' : 1 bit */
        uint32_t sdio_mode_en        : 1; /**< [8] RW - SDIO mode enable - 1 bit
                                            *   '1': SDIO
                                            *   '0' : SD/MMC */
        uint32_t sdio_int_en         : 1; /**< [9] RW - SDIO interrupt enable - 1 bit
                                            *   '1': SDIO interrupt enable
                                            *   '0': SDIO interrupt is turned off */
        uint32_t sdio_read_wait_en   : 1; /**< [10] RW - SDIO read wait enable - 1 bit
                                            *   '1': Enable SDIO read waiting
                                            *   '0': Turn off SDIO read waiting */

        uint32_t reserved_11         : 21; /**< [31:11] RO - reserved */
    };
    uint32_t val;
} wm_sdh_mmc_ctrl_reg_t;

/**
 * @brief SD/SDIO host mmc IO control register
 */
typedef union {
    struct {
        uint32_t auto_data_trans     : 1; /**< [0] RW - Automatic data transfer - 1 bit
                                                         *   '1': Enable automatic data transfer
                                                         *   '0': Disable automatic data transfer
                                                         *   This bit will be automatically cleared when data transfer is complete */
        uint32_t data_dir            : 1; /**< [1] RW - Data transfer direction - 1 bit
                                                         *   '1': Read data
                                                         *   '0': Write data */
        uint32_t auto_8_null_clk_gen : 1; /**< [2] RW - Automatic 8 null clock generation - 1 bit
                                                         *   '1': Enable automatic generation of 8 null clocks after response/command or single block data
                                                         *   '0': Disable automatic generation of 8 null clocks */
        uint32_t cmd_sel             : 1; /**< [3] RW - Command select - 1 bit
                                                         *   '1': Receive response
                                                         *   '0': Send command */
        uint32_t cid_csd_read        : 1; /**< [4] RW - CID/CSD read - 1 bit
                                                         *   '1': Designed for CID and CSD read. When sending CID or CSD command, the SD/MMC/SDIO card device will reply with 136-bit CID or 11 CSD data on the CMD line. When this bit is set to 1, CID or CSD data will be stored in the command buffer area [135:8]. */
        uint32_t port_clk_gen        : 1; /**< [5] RW - Port clock generation - 1 bit
                                                         *   '1': Generate 8 null clocks on SD/MMC/SDIO port clock line
                                                         *   '0': Receive response or send command based on bit 3 setting */
        uint32_t auto_resp_recv      : 1; /**< [6] RW - Automatic response receive - 1 bit
                                                         *   '1': Enable automatic response receive after command
                                                         *   '0': Disable automatic response receive */
        uint32_t
            auto_null_clk_gen_resp_cmd_trans : 1; /**< [7] RW - Automatic null clock generation/response/command transfer - 1 bit
                                                         *   '1': Enable automatic null clock generation/response/command transfer
                                                         *   '0': Disable automatic null clock generation/response/command transfer
                                                         *   Generates 8 null clocks, receives response, or transfers command based on bits 5 and 3 setting. This bit is automatically cleared when transfer is complete. */
        uint32_t cmd_attr       : 1;              /**< [8] RW - Command attribute - 1 bit
                                                         *   '1': Marks that there are data blocks after the current command
                                                         *   '0': Marks that there are no data blocks and command response after the current command */
        uint32_t cmd_abort_flag : 1;              /**< [9] RW - Command abort flag - 1 bit
                                                         *   '1': Marks the current command as cmd12 / IO Abort
                                                         *   '0': Marks the current command as not cmd12 / IO Abort */
        uint32_t reserved_10    : 21;             /**< [31:10] RO - Reserved - 21 bits */
    };
    uint32_t val;
} wm_sdh_mmc_io_t;

/**
 * @brief SD/SDIO host byte count control register
 */
typedef union {
    struct {
        uint32_t byte_count  : 16; /**< [15:0] RW - Byte count for data transfer - 16 bits
                                    *   Represents the number of bytes to be transferred */
        uint32_t reserved_16 : 16; /**< [31:16] RO - Reserved - 21 bits */
    };
    uint32_t val;
} wm_sdh_mmc_bytecntl_t;

/**
 * @brief SD/SDIO host block count control register
 */
typedef union {
    struct {
        uint32_t block_count : 16; /**< [15:0] RO - Block count for multi-block transfer - 16 bits
                                     *   Represents the number of blocks that have been completed during multi-block transfer */
        uint32_t reserved_16 : 16; /**< [31:16] RO - Reserved - 21 bits */
    };
    uint32_t val;
} wm_sdh_mmc_tr_blockcnt_t;

/**
 * @brief SD/SDIO host CRC control register
 */
typedef union {
    struct {
        uint32_t dat_crc_err      : 1;  /**< [0] RO - Data CRC error - 1 bit */
        uint32_t cmd_crc_err      : 1;  /**< [1] RO - Command CRC error - 1 bit */
        uint32_t dat_crc_sel      : 2;  /**< [3:2] RW - Data CRC selection - 2 bits
                                           *   Refer to Table 4 */
        uint32_t read_before_resp : 1;  /**< [4] RW - Read before response in multi-block - 1 bit
                                           *   '1': Enable
                                           *   '0': Disable */
        uint32_t auto_crc_check   : 1;  /**< [5] RW - Enable automatic CRC check - 1 bit
                                           *   '1': Enable. When crc_status != 3'b010, a CRC status interrupt will be generated, write data transfer will be interrupted by stop command, and mmc_io[0] or mmc_io_mbctl[2:0] will be cleared.
                                           *   '0': Disable */
        uint32_t data_line_crc_en : 1;  /**< [6] RW - Data line CRC function enable - 1 bit
                                           *   '1': Enable
                                           *   '0': Disable */
        uint32_t cmd_line_crc_en  : 1;  /**< [7] RW - Command line CRC function enable - 1 bit
                                           *   '1': Enable
                                           *   '0': Disable */
        uint32_t reserved_16      : 24; /**< [31:8] RO - Reserved - 24 bits */
    };
    uint32_t val;
} wm_sdh_mmc_crcctl_t;

/**
 * @brief SD/SDIO host command CRC register
 */
typedef union {
    struct {
        uint32_t cmd_crc_value : 7;  /**< [6:0] RO - Command CRC register value - 7 bits */
        uint32_t reserved_7    : 25; /**< [31:7] RO - Reserved - 25 bits */
    };
    uint32_t val;
} wm_sdh_cmd_crc_t;

/**
 * @brief SD/SDIO host DAT CRC low register
 */
typedef union {
    struct {
        uint32_t dat_crc_low : 8;  /**< [7:0] RO - DAT CRC low register value - 8 bits */
        uint32_t reserved_8  : 24; /**< [31:8] RO - Reserved - 24 bits */
    };
    uint32_t val;
} wm_sdh_dat_crcl_t;

/**
 * @brief SD/SDIO host DAT CRC high register
 */
typedef union {
    struct {
        uint32_t dat_crc_high : 8;  /**< [7:0] RO - DAT CRC high register value - 8 bits */
        uint32_t reserved_8   : 24; /**< [31:8] RO - Reserved - 24 bits */
    };
    uint32_t val;
} wm_sdh_dat_crch_t;

/**
 * @brief SD/SDIO host MM port control register
 */
typedef union {
    struct {
        uint32_t ncr_timeout_val  : 4;  /**< [3:0] RW - Ncr timeout count value - 4 bits
                                         *   (SD/MMC/SDIO clock cycles) */
        uint32_t auto_ncr_timeout : 1;  /**< [4] RW - Automatic Ncr timeout check function - 1 bit
                                         *   '1': Enable automatic Ncr timeout check
                                         *   '0': Disable automatic Ncr timeout check */
        uint32_t port_data_signal : 1;  /**< [5] RW - SD/MMC/SDIO port data line signal - 1 bit
                                         *   '1': Enabled
                                         *   '0': Disabled */
        uint32_t port_cmd_signal  : 1;  /**< [6] RW - SD/MMC/SDIO port command line signal - 1 bit
                                         *   '1': Enabled
                                         *   '0': Disabled */
        uint32_t port_clk_signal  : 1;  /**< [7] RW - SD/MMC/SDIO port clock line signal - 1 bit
                                         *   '1': Enabled
                                         *   '0': Disabled */
        uint32_t reserved_8       : 24; /**< [31:8] RO - Reserved - 24 bits */
    };
    uint32_t val;
} wm_sdh_mm_port_t;

/**
 * @brief SD/SDIO host interrupt mask register
 */
typedef union {
    struct {
        uint32_t cmd_complete_int_mask         : 1;  /**< [0] RW - Command transfer complete interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t data_complete_int_mask        : 1;  /**< [1] RW - Data transfer complete interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t data_crc_err_int_mask         : 1;  /**< [2] RW - Data CRC error interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t cmd_crc_err_int_mask          : 1;  /**< [3] RW - Command CRC error interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t multi_block_tran_int_mask     : 1;  /**< [4] RW - Multi-block transfer complete interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t multi_block_timeout_int_mask  : 1;  /**< [5] RW - Multi-block timeout interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t cmd_resp_ncr_timeout_int_mask : 1;  /**< [6] RW - Command and response Ncr timeout interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t crc_status_token_err_int_mask : 1;  /**< [7] RW - CRC status token error interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t sdio_data1_int_mask           : 1;  /**< [8] RW - SDIO data line 1 interrupt mask - 1 bit
                                                      *   '1': Unmasked
                                                      *   '0': Masked */
        uint32_t reserved_9                    : 23; /**< [31:9] RO - Reserved - 23 bits */
    };
    uint32_t val;
} wm_sdh_mmc_int_mask_t;

/**
 * @brief SD/SDIO host interrupt clear register
 */
typedef union {
    struct {
        uint32_t cmd_complete_int_clr         : 1;  /**< [0] RW - Command transfer complete interrupt clear - 1 bit
                                                     *   Write: Clear command transfer complete interrupt
                                                     *   Read: Command transfer complete interrupt status */
        uint32_t data_complete_int_clr        : 1;  /**< [1] RW - Data transfer complete interrupt clear - 1 bit
                                                     *   Write: Clear data transfer complete interrupt
                                                     *   Read: Data transfer complete interrupt status */
        uint32_t data_crc_err_int_clr         : 1;  /**< [2] RW - Data CRC error interrupt clear - 1 bit
                                                     *   Write: Clear data CRC error interrupt
                                                     *   Read: Data CRC error interrupt status */
        uint32_t cmd_crc_err_int_clr          : 1;  /**< [3] RW - Command CRC error interrupt clear - 1 bit
                                                     *   Write: Clear command CRC error interrupt
                                                     *   Read: Command CRC error interrupt status */
        uint32_t multi_block_tran_int_clr     : 1;  /**< [4] RW - Multi-block transfer complete interrupt clear - 1 bit
                                                     *   Write: Clear multi-block transfer complete interrupt
                                                     *   Read: Multi-block transfer complete interrupt status */
        uint32_t multi_block_timeout_int_clr  : 1;  /**< [5] RW - Multi-block timeout interrupt clear - 1 bit
                                                     *   Write: Clear multi-block timeout interrupt
                                                     *   Read: Multi-block timeout interrupt status */
        uint32_t cmd_resp_ncr_timeout_int_clr : 1;  /**< [6] RW - Command and response Ncr timeout interrupt clear - 1 bit
                                                     *   Write: Clear command and response Ncr timeout interrupt
                                                     *   Read: Command and response Ncr timeout interrupt status */
        uint32_t crc_status_token_err_int_clr : 1;  /**< [7] RW - CRC status token error interrupt clear - 1 bit
                                                     *   Write: Clear CRC status token error interrupt
                                                     *   Read: CRC status token error interrupt status */
        uint32_t clr_sdio_data1_int           : 1;  /**< [8] RW - Clear SDIO data line 1 interrupt - 1 bit */
        uint32_t reserved_9                   : 23; /**< [31:9] RO - Reserved - 23 bits */
    };
    uint32_t val;
} wm_sdh_clr_mmc_int_t;

/**
 * @brief SD/SDIO host card selection register
 */
typedef union {
    struct {
        uint32_t time_base_coefficient : 6;  /**< [5:0] RW - SD/MMC/SDIO time base coefficient - 6 bits
                                              *   Use this register to establish a 1MHz clock */
        uint32_t enable_card_clk       : 1;  /**< [6] RW - Enable SD/MMC/SDIO card device clock line - 1 bit
                                              *   '1': Enable
                                              *   '0': Disable */
        uint32_t controller_enable     : 1;  /**< [7] RW - Enable SD/MMC/SDIO controller - 1 bit
                                              *   '1': Enable
                                              *   '0': Disable */
        uint32_t reserved_8            : 24; /**< [31:8] RO - Reserved - 24 bits */
    };
    uint32_t val;
} wm_sdh_mmc_cardsel_t;

/**
 * @brief SD/SDIO host signal register
 */
typedef union {
    struct {
        uint32_t dat0_signal : 1;  /**< [0] RW - SD/MMC/SDIO port DAT0 data signal - 1 bit
                                    *   '1': Enabled
                                    *   '0': Disabled */
        uint32_t dat1_signal : 1;  /**< [1] RW - SD/MMC/SDIO port DAT1 data signal - 1 bit
                                    *   '1': Enabled
                                    *   '0': Disabled */
        uint32_t dat2_signal : 1;  /**< [2] RW - SD/MMC/SDIO port DAT2 data signal - 1 bit
                                    *   '1': Enabled
                                    *   '0': Disabled */
        uint32_t dat3_signal : 1;  /**< [3] RW - SD/MMC/SDIO port DAT3 data signal - 1 bit
                                    *   '1': Enabled
                                    *   '0': Disabled */
        uint32_t crc_status  : 3;  /**< [6:4] RW - CRC status [2:0] when writing data CRC status token - 3 bits
                                    *   3'b111 */
        uint32_t cmd_signal  : 1;  /**< [7] RW - SD/MMC/SDIO port CMD line signal - 1 bit
                                    *   When reading this register, the SDIO controller will generate a clock pulse on the clock line.
                                    *   The status on the CMD line will be stored in this register on the rising edge of the clock. */
        uint32_t reserved_8  : 23; /**< [31:8] RO - Reserved - 24 bits */
    };
    uint32_t val;
} wm_sdh_mmc_sig_t;

/**
 * @brief SD/SDIO host IO and multi-block control register
 */
typedef union {
    struct {
        uint32_t auto_multi_block_tran : 1; /**< [0] RW - Set SD/MMC/SDIO port auto multiple block data transfer - 1 bit
                                                 *   '1': Enable
                                                 *   '0': Disable
                                                 *   Setting this bit to 1 (when mmc_io[7:6] == 11) will trigger a multiple block data transfer for SD/MMC/SDIO port.
                                                 *   The number of data blocks is set in the mmc_blockcnt register. When data transfer completes, this bit will automatically clear. */
        uint32_t multi_block_direction : 1; /**< [1] RW - Select multiple block data transfer direction - 1 bit
                                                 *   '1': Read data
                                                 *   '0': Write data */
        uint32_t
            auto_cmd_multi_tran : 1; /**< [2] RW - Set SD/MMC/SDIO port full automatic command and multiple block transfer - 1 bit
                                                 *   '1': Enable
                                                 *   '0': Disable
                                                 *   Setting this bit to 1 (when mmc_io[7:6] == 11) will trigger a command, response, 8 null clocks, and multiple block transfer for SD/MMC/SDIO.
                                                 *   When data transfer completes, this bit will automatically clear. */
        uint32_t port_clock_polarity : 1;  /**< [3] RW - SD/MMC/SDIO port clock line polarity - 1 bit
                                                 *   '1': Clock is sent on falling edge and sampled on rising edge
                                                 *   '0': Clock is sent on rising edge and sampled on falling edge */
        uint32_t busy_timeout_scale  : 2;  /**< [5:4] RW - SD/MMC/SDIO Busy timeout scale selection - 2 bits
                                                 *   See Appendix 2 - Table 6 */
        uint32_t nac_timeout_scale   : 2;  /**< [7:6] RW - SD/MMC/SDIO NAC timeout scale selection - 2 bits
                                                 *   See Appendix 2 - Table 5 */
        uint32_t reserved_8          : 23; /**< [31:8] RO - Reserved - 24 bits */
    };
    uint32_t val;
} wm_sdh_mmc_io_mbctl_t;

/**
 * @brief SD/SDIO host data block count register
 */
typedef union {
    struct {
        uint32_t cnt         : 16; /**< [15:0] RW - Data block count - 16 bits */
        uint32_t reserved_16 : 16; /**< [31:16] RO - reserved */
    };
    uint32_t val;
} wm_sdh_blockcnt_t;

/**
 * @brief SD/SDIO host data transfer timeout count register
 */
typedef union {
    struct {
        uint32_t timeout_cnt : 8;  /**< [7:0] RW - Data transfer timeout count - 8 bits
                                     *   Time = Scale * bits [7:0] */
        uint32_t reserved_8  : 24; /**< [31:8] RO - reserved */
    };
    uint32_t val;
} wm_sdh_timeoutcnt_t;

/**
 * @brief SD/SDIO host command buffer byte
 */
typedef union {
    struct {
        uint32_t byte       : 8;  /**< [7:0] RW - Command buffer byte 1 - 8 bits
                                    *   Mapped to command line bit [23:16] */
        uint32_t reserved_8 : 24; /**< [31:8] RO - reserved */
    };
    uint32_t val;
} wm_sdh_cmd_buf_t;

/**
 * @brief SD/SDIO host buffer control register
 */
typedef union {
    struct {
        uint32_t data_full_sig     : 1;  /**< [0] RO - Data buffer full signal - 1 bit
                                          *   '1': Full */
        uint32_t data_empty_sig    : 1;  /**< [1] RO - Data buffer empty signal - 1 bit
                                          *   '1': Empty */
        uint32_t data_pipeline_set : 8;  /**< [9:2] RW - Data buffer data pipeline setting; only valid when buf_ctl[10]=1
                                          *   Note: Data buffer depth is 128 words, do not configure this register larger than 127. */
        uint32_t dma_hw_intf_en    : 1;  /**< [10] RW - DMA hardware interface enable - 1 bit
                                          *   '1': Enable DMA interface
                                          *   '0': AHB interface accesses data buffer;
                                          *   When using the DMA interface, this bit will be automatically reset after data transmission is completed (single data block transmission or multiple data block transmission) */
        uint32_t buf_access_dir    : 1;  /**< [11] RW - Set buffer access direction - 1 bit
                                          *   '1': Write
                                          *   '0': Read */
        uint32_t fifo_status_mask  : 1;  /**< [12] RW - FIFO status signal mask configuration - 1 bit
                                          *   '1': Activate
                                          *   '0': Default
                                          *   Data FIFO status signal, active high;
                                          *   Mask FIFO full signal when reading card device;
                                          *   Mask FIFO empty signal when writing card device */
        uint32_t reserved_13       : 1;  /**< [13] RW - Reserved - 1 bit */
        uint32_t dma_req_mask      : 1;  /**< [14] RW - DMA request mask - 1 bit
                                          *   '0': Not masked
                                          *   '1': Masked
                                          *   Note: Configure this register before enabling DMA; configuring this register after enabling DMA has no effect */
        uint32_t data_flush_en     : 1;  /**< [15] RW - Data buffer flush enable - 1 bit
                                          *   '1': Trigger data buffer flush
                                          *   '0': Hold
                                          *   After writing '1', this register automatically clears after one clock */
        uint32_t reserved_16       : 16; /**< [31:16] RO - reserved */
    };
    uint32_t val;
} wm_sdh_buf_ctrl_t;

/**
 * @brief SD/SDIO host register layout
 */
typedef struct {
    volatile wm_sdh_mmc_ctrl_reg_t mmc_ctrl;                /**< WM_SDH_BASE_ADDR + 0x0000 */
    volatile wm_sdh_mmc_io_t mmc_io;                        /**< WM_SDH_BASE_ADDR + 0x0004 */
    volatile wm_sdh_mmc_bytecntl_t mmc_bytecntl;            /**< WM_SDH_BASE_ADDR + 0x0008 */
    volatile wm_sdh_mmc_tr_blockcnt_t mmc_tr_blockcnt;      /**< WM_SDH_BASE_ADDR + 0x000C */
    volatile wm_sdh_mmc_crcctl_t mmc_crcctl;                /**< WM_SDH_BASE_ADDR + 0x0010 */
    volatile wm_sdh_cmd_crc_t cmd_crc;                      /**< WM_SDH_BASE_ADDR + 0x0014 */
    volatile wm_sdh_dat_crcl_t dat_crcl;                    /**< WM_SDH_BASE_ADDR + 0x0018 */
    volatile wm_sdh_dat_crch_t dat_crch;                    /**< WM_SDH_BASE_ADDR + 0x001C */
    volatile wm_sdh_mm_port_t mm_port;                      /**< WM_SDH_BASE_ADDR + 0x0020 */
    volatile wm_sdh_mmc_int_mask_t mmc_int_mask;            /**< WM_SDH_BASE_ADDR + 0x0024 */
    volatile wm_sdh_clr_mmc_int_t clr_mmc_int;              /**< WM_SDH_BASE_ADDR + 0x0028 */
    volatile wm_sdh_mmc_cardsel_t mmc_cardsel;              /**< WM_SDH_BASE_ADDR + 0x002C */
    volatile wm_sdh_mmc_sig_t mmc_sig;                      /**< WM_SDH_BASE_ADDR + 0x0030 */
    volatile wm_sdh_mmc_io_mbctl_t mmc_io_mbctl;            /**< WM_SDH_BASE_ADDR + 0x0034 */
    volatile wm_sdh_blockcnt_t blockcnt;                    /**< WM_SDH_BASE_ADDR + 0x0038 */
    volatile wm_sdh_timeoutcnt_t timeoutcnt;                /**< WM_SDH_BASE_ADDR + 0x003C */
    volatile wm_sdh_cmd_buf_t cmd_buf[16];                  /**< WM_SDH_BASE_ADDR + 0x0040 */
    volatile wm_sdh_buf_ctrl_t buf_ctrl;                    /**< WM_SDH_BASE_ADDR + 0x0080 */
    uint32_t reserved[31];                                  /**< WM_SDH_BASE_ADDR + 0x0084 */
    volatile uint32_t data_buf[WM_SDH_DATA_BUF_BYTES >> 2]; /**< WM_SDH_BASE_ADDR + 0x0100 */
} wm_sdh_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_SDH_REG_STRUCT_H__ */
