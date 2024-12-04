/**
 * @file wm_dma_reg_struct.h
 *
 * @brief dma register struct
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

#ifndef __WM_DMA_REG_STRUCT_H__
#define __WM_DMA_REG_STRUCT_H__

#include <stdint.h>
#include "wm_regs_base.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dma interupt mask register
 */
typedef union {
    struct {
        uint32_t ch0_burst_done : 1;  /* [0] channel 0 burst done */
        uint32_t ch0_xfer_done  : 1;  /* [1] channel 0 transfer done */
        uint32_t ch1_burst_done : 1;  /* [2] channel 1 burst done */
        uint32_t ch1_xfer_done  : 1;  /* [3] channel 1 transfer done */
        uint32_t ch2_burst_done : 1;  /* [4] channel 2 burst done */
        uint32_t ch2_xfer_done  : 1;  /* [5] channel 2 transfer done */
        uint32_t ch3_burst_done : 1;  /* [6] channel 3 burst done */
        uint32_t ch3_xfer_done  : 1;  /* [7] channel 3 transfer done */
        uint32_t ch4_burst_done : 1;  /* [8] channel 4 burst done */
        uint32_t ch4_xfer_done  : 1;  /* [9] channel 4 transfer done */
        uint32_t ch5_burst_done : 1;  /* [10] channel 5 burst done */
        uint32_t ch5_xfer_done  : 1;  /* [11] channel 5 transfer done */
        uint32_t ch6_burst_done : 1;  /* [12] channel 6 burst done */
        uint32_t ch6_xfer_done  : 1;  /* [13] channel 6 transfer done */
        uint32_t ch7_burst_done : 1;  /* [14] channel 7 burst done */
        uint32_t ch7_xfer_done  : 1;  /* [15] channel 7 transfer done */
        uint32_t reserved_16    : 16; /* [31:16] reserved  */
    };
    uint32_t val;
} wm_dma_int_mask_t;

/**
 * @brief dma interupt status register
 */
typedef union {
    struct {
        uint32_t ch0_burst_done : 1;  /* [0] channel 0 burst done, write 1 clear */
        uint32_t ch0_xfer_done  : 1;  /* [1] channel 0 transfer done, write 1 clear */
        uint32_t ch1_burst_done : 1;  /* [2] channel 1 burst done, write 1 clear */
        uint32_t ch1_xfer_done  : 1;  /* [3] channel 1 transfer done, write 1 clear */
        uint32_t ch2_burst_done : 1;  /* [4] channel 2 burst done, write 1 clear */
        uint32_t ch2_xfer_done  : 1;  /* [5] channel 2 transfer done, write 1 clear */
        uint32_t ch3_burst_done : 1;  /* [6] channel 3 burst done, write 1 clear */
        uint32_t ch3_xfer_done  : 1;  /* [7] channel 3 transfer done, write 1 clear */
        uint32_t ch4_burst_done : 1;  /* [8] channel 4 burst done, write 1 clear */
        uint32_t ch4_xfer_done  : 1;  /* [9] channel 4 transfer done, write 1 clear */
        uint32_t ch5_burst_done : 1;  /* [10] channel 5 burst done, write 1 clear */
        uint32_t ch5_xfer_done  : 1;  /* [11] channel 5 transfer done, write 1 clear */
        uint32_t ch6_burst_done : 1;  /* [12] channel 6 burst done, write 1 clear */
        uint32_t ch6_xfer_done  : 1;  /* [13] channel 6 transfer done, write 1 clear */
        uint32_t ch7_burst_done : 1;  /* [14] channel 7 burst done, write 1 clear */
        uint32_t ch7_xfer_done  : 1;  /* [15] channel 7 transfer done, write 1 clear */
        uint32_t reserved_16    : 16; /* [31:16] reserved */
    };
    uint32_t val;
} wm_dma_int_status_t;

/**
 * @brief dma uart channel select register
 */
typedef union {
    struct {
        uint32_t uart_ch_sel : 3;  /* [2:0] dma channel select */
        uint32_t reserved_3  : 5;  /* [7:3] reserved */
        uint32_t req_clear   : 16; /* [23:8] clear dam req if write bit 1 */
        uint32_t reserved_24 : 8;  /* [31:24] reserved */
    };
    uint32_t val;
} wm_dma_uart_ch_t;

/**
 * @brief dma wrap size register
 */
typedef union {
    struct {
        uint16_t src_addr_len  : 16; /* [15:0] src addr len */
        uint16_t dest_addr_len : 16; /* [31:16] dest addr len */
    };
    uint32_t val;
} wm_dma_wrap_size_t;

/**
 * @brief dma channel control register
 */
typedef union {
    struct {
        uint32_t dma_start  : 1;  /* [0] start curren channel dma,auto clear to 0 if stoped */
        uint32_t dma_stop   : 1;  /* [1] stop dma , stop after curren burst,then clear dma_on */
        uint32_t reserved_2 : 30; /* [31:2] reserved */
    };
    uint32_t val;
} wm_dma_ch_ctrl_t;

/**
 * @brief dma mode select register
 */
typedef union {
    struct {
        uint32_t dma_mode      : 1;  /* [0] 0: soft, 1: harware */
        uint32_t chain_mode    : 1;  /* [1] 0: normal, 1: chain */
        uint32_t dma_sel       : 4;  /* [5-2] 16 dma req select
                                              4’d0：uart rx dma req
                                              4’d1：uart tx dma req
                                              4’d2：pwm_cap0_req
                                              4’d3：pwm_cap1_req
                                              4’d4：LS_SPI rx dma req
                                              4’d5：LS_SPI tx dma req
                                              4’d6：SD_ADC chnl0 req
                                              4’d7：SD_ADC chnl1 req
                                              4’d8：SD_ADC chnl2 req
                                              4’d9：SD_ADC chnl3 req
                                              4’d10: I2S RX req
                                              4’d11: I2S TX req
                                              4’d12: SDIO_HOST req */
        uint32_t chain_link_en : 1;  /* [6] chain link enable */
        uint32_t reserved_7    : 25; /* [31:7] reserved */
    };
    uint32_t val;
} wm_dma_mode_t;

/**
 * @brief dma stream control register
 */
typedef union {
    struct {
        uint32_t auto_reload   : 1; /* [0] fter completing the current DMA transfer, automatically perform the next DMA transfer
                                        according to the current DMA configuration */
        uint32_t src_addr_inc  : 2; /* [2:1] src size
                                           2'h0: The source address remains unchanged after each operation
                                           2'h1: The source address is automatically accumulated after each operation
                                           2'h2: reserved
                                           2'h3: Loop operation, after each operation, the source address is automatically accumulated,
                                                 and when it reaches the defined loop boundary, it jumps to the loop starting address */
        uint32_t dest_addr_inc : 2; /* [4:3] data size
                                           2'h0: The destination address remains unchanged after each operation
                                           2'h1: The destination address is automatically accumulated after each operation
                                           2'h2: reserved
                                           2'h3: Loop operation, the destination address is automatically accumulated after each operation,
                                                 and when it reaches the defined loop boundary, it jumps to the loop Ring start address */
        uint32_t data_uint     : 2; /* [6:5] data uint
                                            2’h0：byte
                                            2’h1：half_word
                                            2’h2：word
                                            2’h3：reserve  */
        uint32_t burst_size    : 1; /* [7] burst size
                                           1’b0：burst is 1
                                           1’b1：burst is 4  */
        uint32_t total_bytes   : 16; /* [23:8] total bytes */
        uint32_t reserved_24   : 8;  /* [31:24] reserved */
    };
    uint32_t val;
} wm_dma_ctrl_t;

/**
 * @brief dma status register
 */
typedef union {
    struct {
        uint16_t xfer_cnt    : 16; /* [15:0]The current number of bytes transmitted. Every time the DMA is restarted
                                            (ch on set to 1), clear 0 and restart counting */
        uint16_t reserved_16 : 16; /* [31:16] reserved */
    };
    uint32_t val;
} wm_dma_status_t;

/**
 * @brief dma ch register
 */
typedef struct {
    volatile uint32_t src_addr;
    volatile uint32_t dest_addr;
    volatile uint32_t src_warp_addr;
    volatile uint32_t dest_warp_addr;
    volatile wm_dma_wrap_size_t warp_size;
    volatile wm_dma_ch_ctrl_t ch_ctrl;
    volatile wm_dma_mode_t mode;
    volatile wm_dma_ctrl_t ctrl;
    volatile wm_dma_status_t status;
    volatile uint32_t desc_addr;
    volatile uint32_t current_dest_addr;
    volatile uint32_t reserved;
} wm_dma_ch_reg_t;

/**
 * @brief dma register layout
 */
typedef struct {
    volatile wm_dma_int_mask_t int_msk;             /* WM_DMA_BASE_ADDR + 0x0000 */
    volatile wm_dma_int_status_t int_sts;           /* WM_DMA_BASE_ADDR + 0x0004 */
    volatile wm_dma_uart_ch_t uart_ch;              /* WM_DMA_BASE_ADDR + 0x0008 */
    volatile uint32_t reserved;                     /* WM_DMA_BASE_ADDR + 0x000C */
    volatile wm_dma_ch_reg_t dma_ch[WM_DMA_CH_MAX]; /* WM_DMA_BASE_ADDR + 0x0010 */
} wm_dma_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_DMA_REG_STRUCT_H__ */
