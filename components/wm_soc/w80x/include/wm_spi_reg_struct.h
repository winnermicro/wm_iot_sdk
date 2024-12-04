/**
 * @file wm_spi_reg_struct.h
 *
 * @brief spi registers struct Module
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

#ifndef __WM_SPI_REG_STRUCT_H__
#define __WM_SPI_REG_STRUCT_H__

#include "wm_types.h"
#include "wm_regs_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_INT_TIMEOUT       (1U << 7)
#define SPI_INT_TRANSFER_DONE (1U << 6)
#define SPI_INT_RX_OVERRUN    (1U << 5)
#define SPI_INT_RX_UNDERRUN   (1U << 4)
#define SPI_INT_TX_OVERRUN    (1U << 3)
#define SPI_INT_TX_UNDERRUN   (1U << 2)
#define SPI_INT_RX_FIFO_RDY   (1U << 1)
#define SPI_INT_TX_FIFO_RDY   (1U << 0)

/**
 * @brief channel configuration register layout
 * This structure is used to channel configuration
 */
typedef union {
    struct {
        uint32_t start          : 1;  /*[0] 0: stop SPI, 1: start SPI*/
        uint32_t cs_out         : 1;  /*[1] 0: spi_cs out contrl by HW  1:spi_cs out contrl by SW */
        uint32_t chip_sel       : 1;  /*[2] 0: SPI_CS invalid singanl is 0  1:SPI_CS invalid singanl is 1*/
        uint32_t tx_rx_len      : 16; /*[18:3]  tx/rx length, unit is bit*/
        uint32_t tx_ch_on       : 1;  /*[19] 0: tx channel off, 1: tx channel on*/
        uint32_t rx_ch_on       : 1;  /*[20] 0: rx channel off, 1: rx channel on*/
        uint32_t continue_mode  : 1;  /*[21] 0: not continue, 1: contune*/
        uint32_t clr_fifo       : 1;  /*[22] 0: not clear FIFO, 1: clear FIFO*/
        uint32_t rx_invalid_bit : 8;  /*[30:23] the invalid bit begin the recveie*/
        uint32_t reserved_31    : 1;  /*[31] Reserved */
    };
    uint32_t val;
} wm_spi_ch_cfg_t;

/**
 * @brief SPI configuration register layout
 * This structure is used to SPI configuration
 */
typedef union {
    struct {
        uint32_t cpol          : 1;  /*[0] 0: sck idle is 0,  1: sck idle is 1*/
        uint32_t cpha          : 1;  /*[1] 0: tramsmit mode as A,  1: transmit mode as B*/
        uint32_t role          : 1;  /*[2] 0: slave,  1: master*/
        uint32_t big_edian     : 1;  /*[3] for tx 0: small ednian,  1: big endian*/
        uint32_t frame_delay   : 3;  /*[6:4] delay time between trasmit frame*/
        uint32_t spi_out_delay : 2;  /*[8:7] spi data out delay compare with SCK*/
        uint32_t cs_steup_time : 3;  /*[11:9] steup time before transfer*/
        uint32_t hold_time     : 3;  /*[14:12] hold time after transmit done*/
        uint32_t reserved_15   : 1;  /*[15] Reserved */
        uint32_t tx_driven     : 1;  /*[16] 0: drivern when spi_cs valid,  1: always be drivern*/
        uint32_t fram_format   : 2;  /*[18:17] 00:motorola, 01：TI,  10：microwire, 11:rsv*/
        uint32_t reserved_19   : 13; /*[31:19] Reserved */
    };
    uint32_t val;
} wm_spi_cfg_t;

/**
 * @brief SPI clock configuration register layout
 * This structure is used to config frequency division coefficient
 */
typedef union {
    struct {
        uint32_t divider     : 16; /*[15:0]divder,  Fsck =Fapb_clk / ( 2 x (Divider +1))*/
        uint32_t reserved_16 : 16; /*[31:16]reserved*/
    };
    uint32_t val;
} wm_spi_clk_cfg_t;

/**
 * @brief SPI mode configuration register layout
 * This structure is used to config transmit mode
 */
typedef union {
    struct {
        uint32_t tx_dma_on     : 1;  /*[0] 0: use DMA,  1: not use DMA*/
        uint32_t rx_dma_on     : 1;  /*[1] 0: use DMA,  1: not use DMA*/
        uint32_t tx_trig_level : 3;  /*[4:2] tx use dma threshold */
        uint32_t reserved_5    : 1;  /*reseved */
        uint32_t rx_trig_level : 3;  /*[8:6] rx use dma threshold */
        uint32_t reserved_9    : 23; /*[31:9] reserved*/
    };
    uint32_t val;
} wm_spi_mode_cfg_t;

/**
 * @brief SPI interrupt configuration register layout
 * This structure is used to disable/enable interrupt
 */
typedef union {
    struct {
        uint32_t tx_fifo_rdy_en : 1;  /*[0] 0: enable interrupt when tx FIFO ready, 1:disable interrupt when TX ready*/
        uint32_t rx_fifo_rdy_en : 1;  /*[1] 0: enable interrupt has data in RX FIFO,  1:disable interrupt has data in RX FIFO*/
        uint32_t tx_underrun_en : 1;  /*[2] 0: enable tx underflow interrupt,  1:disable tx underflow interrupt*/
        uint32_t tx_overrun_en  : 1;  /*[3] 0: enable tx overflow interrupt,  1:disable tx overflow interrupt*/
        uint32_t rx_underrun_en : 1;  /*[4] 0: enable rx underflow interrupt,  1:disable rx underflow interrupt*/
        uint32_t rx_overrun_en  : 1;  /*[5] 0: enable rx overflow interrupt,  1:disable rx overflow interrupt*/
        uint32_t spi_done_en    : 1;  /*[6] 0: enable tx/rx done interrupt,  1:disable tx/rx done interrupt*/
        uint32_t timeout_en     : 1;  /*[7] 0: enable spi_timeout interrupt,  1:disable spi_timeout interrupt*/
        uint32_t reserved_8     : 24; /*[31:8] reserved*/
    };
    uint32_t val;
} wm_spi_int_mask_cfg_t;

/**
 * @brief SPI interrupt status register layout
 * This structure is used to get the interrupt source
 */
typedef union {
    struct {
        uint32_t tx_fifo_rdy : 1;  /*[0] 0: TX FIFO not ready  1: TX FIFO is ready  */
        uint32_t rx_fifo_rdy : 1;  /*[1] 0: RX FIFO not ready  1: RX FIFO is ready  */
        uint32_t tx_underrun : 1;  /*[2] 0: not Tx FIFO underflow   1: Tx FIFO underflow */
        uint32_t tx_overrun  : 1;  /*[3] 0: not Tx FIFO overflow   1: Tx FIFO overflow */
        uint32_t rx_underrun : 1;  /*[4] 0: not Tx FIFO underflow   1: Tx FIFO underflow */
        uint32_t rx_overrun  : 1;  /*[5] 0: not Tx FIFO overflow   1: Tx FIFO overflow */
        uint32_t spi_done    : 1;  /*[6] 0: tx/rx not done, : 1:tx/rx done*/
        uint32_t timeout     : 1;  /*[7] 0: rx fifo has not ending data, : 1:rx fifo has ending data*/
        uint32_t reserved_8  : 24; /*[31:8] reserved*/
    };
    uint32_t val;
} wm_spi_int_status_t;

/**
 * @brief SPI status register layout
 * This structure is used to list SPI status at communication
 */
typedef union {
    struct {
        uint32_t tx_fifo_level : 6;  /*[5:0] transmit size in tx FIFO, uint is byte*/
        uint32_t rx_fifo_level : 6;  /*[11:6] receive size in rx FIFO, uint is byte*/
        uint32_t is_busy       : 1;  /*[12]0: idle,  1: tx/rx ongoing*/
        uint32_t reserved_13   : 19; /*[31:13] reserved*/
    };
    uint32_t val;
} wm_spi_status_t;

/**
 * @brief SPI timeout configuration register layout
 * This structure is used to config timeout value when communicate
 */
typedef union {
    struct {
        uint32_t time_out_value : 31; /*[30: 0] after timeout, request CPU move data from RX FIFO */
        uint32_t timer_en       : 1;  /*[31] 0: disable timer, 1: enable timer*/
    };
    uint32_t val;
} wm_spi_time_out_cfg_t;

/**
 * @brief SPI transmit data register layout
 * This structure is used to store trasmit data (TX FIFO)
 */
typedef union {
    struct {
        uint32_t address; /*[31:0] the address write data to TX FIFO*/
    };
    uint32_t val;
} wm_spi_tx_data_t;

/**
 * @brief SPI transmit mode configuration register layout
 * This structure is used to transmit mode configuration
 */
typedef union {
    struct {
        uint32_t micro_ctrl_len : 6; /*[5:0] command length in microwire mode*/
        uint32_t reserved_6     : 2; /*[7:6]reserved*/
        uint32_t micro_dat_len  : 6; /*[13:8] transer data length in burst mode*/
        uint32_t reserved_14    : 2; /*[15:14]reserved*/
        uint32_t micro_burst    : 1; /*[16] 1: tx not use burst in microwire mode,  1: tx use burst in microwire mode*/
        uint32_t reserved_17    : 7; /*[23:17]reserved*/
        uint32_t ti_blk_len     : 6; /*[29:24] transmit length at each block*/
        uint32_t reserved       : 2; /*[31:30]reserved*/
    };
    uint32_t val;
} wm_spi_trans_mode_cfg_t;

/**
 * @brief SPI transmit data length register layout
 * This structure is used to store slave trasmit data length
 */
typedef union {
    struct {
        uint32_t rx_valid_len : 16; /*[15:0] RX length in slave*/
        uint32_t tx_valid_len : 16; /*[31:16]TX length in slave*/
    };
    uint32_t val;
} wm_spi_slv_xmit_len_t;

/**
 * @brief SPI reserve register layout
 * This SPI reserve register
 */
typedef union {
    struct {
        uint32_t reserved; /*reserved*/
    };
    uint32_t val;
} wm_spi_rsv_t;

/**
 * @brief SPI receive data register layout
 * This structure is used to store recevie data(RX FIFO)
 */
typedef union {
    struct {
        uint32_t rx_addr; //window address from RX FIFO;
    };
    uint32_t val;
} wm_spi_rx_data_t;

/**
 * @brief SPI register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile wm_spi_ch_cfg_t ch_cfg;             /*WM_SPI_BASE_ADDR*/
    volatile wm_spi_cfg_t cfg;                   /*WM_SPI_BASE_ADDR + 0x004*/
    volatile wm_spi_clk_cfg_t clk_cfg;           /*WM_SPI_BASE_ADDR + 0x008*/
    volatile wm_spi_mode_cfg_t mode_cfg;         /*WM_SPI_BASE_ADDR + 0x00C*/
    volatile wm_spi_int_mask_cfg_t int_mask;     /*WM_SPI_BASE_ADDR + 0x010*/
    volatile wm_spi_int_status_t int_status;     /*WM_SPI_BASE_ADDR + 0x014*/
    volatile wm_spi_status_t status;             /*WM_SPI_BASE_ADDR + 0x018*/
    volatile wm_spi_time_out_cfg_t timeout_cfg;  /*WM_SPI_BASE_ADDR + 0x01C*/
    volatile wm_spi_tx_data_t tx_data;           /*WM_SPI_BASE_ADDR + 0x020*/
    volatile wm_spi_trans_mode_cfg_t trans_mode; /*WM_SPI_BASE_ADDR + 0x024*/
    volatile wm_spi_slv_xmit_len_t slv_xmit_len; /*WM_SPI_BASE_ADDR + 0x028*/
    volatile wm_spi_rsv_t rsv;                   /*WM_SPI_BASE_ADDR + 0x02C*/
    volatile wm_spi_rx_data_t rx_data;           /*WM_SPI_BASE_ADDR + 0x030*/
} wm_spi_reg_t;

#ifdef __cplusplus
}
#endif
#endif /* __WM_SPI_REG_STRUCT_H__ */
