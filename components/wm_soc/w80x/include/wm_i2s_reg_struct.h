/**
 * @file wm_i2s_reg_struct.h
 *
 * @brief i2s registers struct Module
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

#ifndef __WM_I2S_REG_STRUCT_H__
#define __WM_I2S_REG_STRUCT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief i2s control register
 */
typedef union {
    struct {
        uint32_t i2sen            : 1; /* [0] 0: disable i2s, 1: enable i2s */
        uint32_t txen             : 1; /* [1] 0: disable tx, 1: enable tx */
        uint32_t rxen             : 1; /* [2] 0: disable rx, 1: enable rx */
        uint32_t mute             : 1; /* [3] 0: Transmit data from the shift register in normal operation mode,
                                              1: Set the transmission data to 0 to mute the sound */
        uint32_t wdwidth          : 2; /* [5:4] Set the transmission word length
                                                0: 8bit
                                                1: 16bit
                                                2: 24bit
                                                3: 32bit */
        uint32_t reserved_6       : 2; /* [7:6] Reserved */
        uint32_t tx_clk_phase_sel : 1; /* [8] select tx clock phase
                                              0: default mode
                                              1: invert mode*/
        uint32_t txth             : 3; /* [11:9] txfifo threshold
                                                3’h0: set txfifo threshold as 0 word
                                                3’h1：set txfifo threshold as 1 words
                                                3’h2：set txfifo threshold as 2 words
                                                3’h3：set txfifo threshold as 3 words
                                                3’h4：set txfifo threshold as 4 words
                                                3’h5：set txfifo threshold as 5 words
                                                3’h6：set txfifo threshold as 6 words
                                                3’h7：set txfifo threshold as 7 words
                                                Note: When the number of existing words in the TXFIFO is equal to or less than the value of TXTH,
                                                the TXTHIF bit will be set.
                                                At this point, based on the configuration, you can choose to trigger either the TXDMA or the I2S interrupt.*/
        uint32_t rxth             : 3; /* [14:12] rxfifo threshold
                                                3’h0: set rxfifo threshold as 0 word
                                                3’h1：set rxfifo threshold as 1 words
                                                3’h2：set rxfifo threshold as 2 words
                                                3’h3：set rxfifo threshold as 3 words
                                                3’h4：set rxfifo threshold as 4 words
                                                3’h5：set rxfifo threshold as 5 words
                                                3’h6：set rxfifo threshold as 6 words
                                                3’h7：set rxfifo threshold as 7 words
                                                Note: When the number of existing words in the RXFIFO is equal to or greater than the value of RXTH,
                                                the RXTHIF bit will be set.
                                                At this point, based on the configuration, you can choose to trigger either the RXDMA or the I2S interrupt.*/
        uint32_t rx_clk_phase_sel : 1; /* [15] select rx clock phase
                                               0: default mode
                                               1: invert mode*/
        uint32_t rzcen            : 1; /* [16] 0: disable right audio channel zero-crossing detection
                                               1: enable right audio channel zero-crossing detection    */
        uint32_t lzcen            : 1; /* [17] 0: disable left audio channel zero-crossing detection
                                               1: enable left audio channel zero-crossing detection    */
        uint32_t txclr            : 1; /* [18] clear to 0 have no effect, set 1 to clear txfifo
                                               Note: Writing 1 to this bit clears the TXFIFO, which is automatically done by the hardware.
                                               Reading this bit will always return 0.*/
        uint32_t rxclr            : 1; /* [19] clear to 0 have no effect, set 1 to clear rxfifo
                                               Note: Writing 1 to this bit clears the RXFIFO, which is automatically done by the hardware.
                                               Reading this bit will always return 0.*/
        uint32_t txdmaen          : 1; /* [20] 0: disable DMA tx request
                                               1: enable DMA tx request
                                               Note: When the transmission DMA request is enabled and the number of words in the TXFIFO is less than TXTH,
                                               the I2S controller will issue a transmission request to the DMA and continue until the TXFIFO is full,
                                               at which point the DMA transmission will stop.*/
        uint32_t rxdmaen          : 1; /* [21] 0: disable DMA rx request
                                               1: enable DMA rx request
                                               Note: When the transmission DMA request is enabled and the number of words in the RXFIFO is equal to or greater than RXTH,
                                               the I2S controller will issue a transmission request to the DMA and continue until the RXFIFO is empty,
                                               at which point the DMA transmission will stop.*/
        uint32_t mono_stereo      : 1; /* [22] 0: Data is transmitted in stereo format,
                                               1: Data is transmitted in mono format */
        uint32_t rxlch            : 1; /* [23] 0: select right audio channel reception,
                                               1: select left audio channel reception
                                               Note: This bit is only valid when the mono_stereo mode is selected for mono sound transmission.*/
        uint32_t format           : 2; /* [25:24] select data format
                                                0: i2s data format
                                                1: MSB Justified data format
                                                2: PCM A audio data format
                                                3: PCM B audio data format        */
        uint32_t timeout_en       : 1; /* [26] set 1 to enable timeout counter
                                               When this bit is set to 1 and the transmission process is forcibly stopped by the master device,
                                               no reception completion (RXDONE) interrupt will occur.*/
        uint32_t duplex_en        : 1; /* [27] 0: disable duplex
                                               1: enable duplex */
        uint32_t master_slave_sel : 1; /* [28] 0:master, 1:slave */
        uint32_t reserved_29      : 3; /* [31:29] Reserved */
    };
    uint32_t val;
} wm_i2s_control_t;

/**
 * @brief i2s interrupt mask register layout
 */
typedef union {
    struct {
        uint32_t rxudimask   : 1;  /* [0]    0: unmask rxfifo underflow interrupt
                                             1: mask rxfifo underflow interrupt
                                             Note: When the RXFIFO underflow interrupt is unmask and RXUDIF is detected as 1,
                                             an underflow interrupt will be generated.*/
        uint32_t rxovimask   : 1;  /* [1]    0: unmask rxfifo overflow interrupt
                                             1: mask rxfifo overflow interrupt
                                             Note: When the RXFIFO overflow interrupt is unmask and RXOVIF is detected as 1,
                                             an overflow interrupt will be generated.*/
        uint32_t rxthimask   : 1;  /* [2]    0: unmask rxfifo threshold interrupt
                                             1: mask rxfifo threshold interrupt
                                             When the RXFIFO threshold interrupt is unmask and the number of data items in the RXFIFO is equal to or greater
                                             than the threshold value, an RX interrupt will be generated.*/
        uint32_t rxdonemask  : 1;  /* [3]    0: unmask rxdone interrupt
                                             1: mask rxdone interrupt
                                             When the rx done interrupt is unmask and the reception process is completed,
                                             a rx done interrupt will be generated.*/
        uint32_t txudimask   : 1;  /* [4]    0: unmask txfifo underflow interrupt
                                             1: mask txfifo underflow interrupt
                                             Note: When the TXFIFO underflow interrupt is unmask and TXUDIF is detected as 1,
                                             an underflow interrupt will be generated.*/
        uint32_t txovimask   : 1;  /* [5]    0: unmask txfifo overflow interrupt
                                             1: mask txfifo overflow interrupt
                                             Note: When interrupts are unmask, and the TXFIFO is full, if the CPU attempts to write data into the TXFIFO again,
                                             the TXOVIF flag will be set.*/
        uint32_t txthimask   : 1;  /* [6]    0: unmask txfifo threshold interrupt
                                             1: mask txfifo threshold interrupt
                                             When the interrupt is unmask and the number of data items in the TXFIFO is equal to or less than TXTH,
                                             an interrupt will be generated.*/
        uint32_t txdonemask  : 1;  /* [7]    0: unmask txdone interrupt
                                             1: mask txdone interrupt
                                             When the interrupt is unmask and the TXFIFO is empty, an interrupt will be generated.*/
        uint32_t rzcimask    : 1;  /* [8]    0: unmask Right Channel Zero-Crossing Interrupt
                                             1: mask Right Channel Zero-Crossing Interrupt
                                             When the interrupt is unmask and a zero-crossing is detected on the right channel, an interrupt will be generated.*/
        uint32_t lzcimask    : 1;  /* [9]    0: unmask Left Channel Zero-Crossing Interrupt
                                             1: mask Left Channel Zero-Crossing Interrupt
                                             When the interrupt is unmask and a zero-crossing is detected on the left channel, an interrupt will be generated.*/
        uint32_t reserved_10 : 22; /* [31:10] Reserved */
    };
    uint32_t val;
} wm_i2s_imask_t;

/**
 * @brief i2s interrupt flag register layout
 */
typedef union {
    struct {
        uint32_t rxudif      : 1;  /* [0]    0: The rxfifo has not generated an underflow flag.
                                             1: The rxfifo has generated an underflow flag.
                                             Note: Write 1 to clear the underflow flag.*/
        uint32_t rxovif      : 1;  /* [1]    0: The rxfifo has not generated an overflow flag.
                                             1: The rxfifo has generated an overflow flag.
                                             Note: Write 1 to clear the overflow flag.*/
        uint32_t rxthif      : 1;  /* [2]    0: The number of words in the RXFIFO is less than the threshold.
                                             1: The number of words in the RXFIFO is equal to or greater than the threshold.
                                             Note: When the number of words in the RXFIFO is equal to or greater than the threshold set by RXTH,
                                             this bit will be set to 1. It will remain 1 until the data in the RXFIFO is read out and the value of RXCNT becomes less than RXTH,
                                             at which point it will revert back to 0.*/
        uint32_t rxdoneif    : 1;  /* [3]    0: The current reception is incomplete.
                                             1: The current reception is complete.
                                             Note: Write 1 to clear the flag.*/
        uint32_t txudif      : 1;  /* [4]    0: The txfifo has not generated an underflow flag.
                                             1: The txfifo has generated an underflow flag.
                                             Note: Write 1 to clear the underflow flag.*/
        uint32_t txovif      : 1;  /* [5]    0: The txfifo has not generated an overflow flag.
                                             1: The txfifo has generated an overflow flag.
                                             Note: Write 1 to clear the overflow flag.*/
        uint32_t txthif      : 1;  /* [6]    0: The number of words in the TXFIFO is greater than the threshold.
                                             1: The number of words in the RXFIFO is equal to or less than the threshold.
                                             Note: When the number of words in the TXFIFO (TXCNT) is equal to or less than the threshold set by TXTH,
                                             this bit will be set to 1. It will remain 1 until data is written to the TXFIFO and the value of TXCNT becomes greater than TXTH,
                                             at which point it will revert back to 0.*/
        uint32_t txdoneif    : 1;  /* [7]    0: The current transmission is incomplete.
                                             1: The current transmission is complete.
                                             Note: Write 1 to clear the flag.*/
        uint32_t rzcif       : 1;  /* [8]    0: No right channel zero-crossing detected
                                             1: Detected right channel zero-crossing
                                             Note: This bit indicates a change in the sign bit of the next sample data on the right channel or that all data bits are zero.
                                             Write 1 to clear the flag.*/
        uint32_t lzcif       : 1;  /* [9]    0: No left channel zero-crossing detected
                                             1: Detected left channel zero-crossing
                                             Note: This bit indicates a change in the sign bit of the next sample data on the left channel or that all data bits are zero.
                                             Write 1 to clear the flag.*/
        uint32_t i2sif       : 1;  /* [10]    0: No I2S flag occurred.
                                              1: An flag has occurred
                                              Note: This bit will be set whenever there is an flag on either RX or TX.*/
        uint32_t rxif        : 1;  /* [11]    0: No I2S receive flag occurred.
                                              1: An I2S receive flag has occurred.*/
        uint32_t txif        : 1;  /* [12]    0: No I2S transmission flag occurred.
                                              1: An I2S transmission flag has occurred.*/
        uint32_t reserved_13 : 19; /* [31:13] Reserved */
    };
    uint32_t val;
} wm_i2s_int_flag_t;

/**
 * @brief i2s fifo status register layout
 */
typedef union {
    struct {
        uint32_t rxcnt       : 4;  /* [3:0] the number of words currently in the RXFIFO
                                            0: No data
                                            1: 1 word
                                            ...
                                            8: 8 words    */
        uint32_t txcnt       : 4;  /* [7:4] the number of words currently in the TXFIFO
                                            0: No data
                                            1: 1 word
                                            ...
                                            8: 8 words    */
        uint32_t validbyte   : 2;  /* [9:8] the number of valid bytes in the last word of RXFIFO, after reception is completed
                                            0: 4 bytes are valid
                                            1: 1 byte is valid
                                            2: 2 bytes are valid
                                            3: 3 bytes are valid    */
        uint32_t reserved_10 : 22; /* [31:10] Reserved */
    };
    uint32_t val;
} wm_i2s_status_t;

/**
 * @brief i2s tx data register layout
          The I2S has a built-in 8-word-length FIFO buffer to store data pending transmission.
          Every time a word is written into the TXFIFO, the number of words in the TXFIFO increases by one.
          The I2S controller automatically sends out the words that entered the TXFIFO first.
 */
typedef union {
    struct {
        uint32_t txfifo;
    };
    uint32_t val;
} wm_i2s_tx_t;

/**
 * @brief i2s rx data register layout
          The I2S has a built-in 8-word-length FIFO for storing received data.
          Every time a word is read from the RXFIFO, the number of words in the RXFIFO decreases by one.
 */
typedef union {
    struct {
        uint32_t rxfifo;
    };
    uint32_t val;
} wm_i2s_rx_t;

/**
 * @brief i2s register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile wm_i2s_control_t control;   /* WM_I2S_REG_BASE_ADDR */
    volatile wm_i2s_imask_t imask;       /* WM_I2S_REG_BASE_ADDR + 0x004 */
    volatile wm_i2s_int_flag_t int_flag; /* WM_I2S_REG_BASE_ADDR + 0x008 */
    volatile wm_i2s_status_t status;     /* WM_I2S_REG_BASE_ADDR + 0x00C */
    volatile wm_i2s_tx_t tx;             /* WM_I2S_REG_BASE_ADDR + 0x010 */
    volatile wm_i2s_rx_t rx;             /* WM_I2S_REG_BASE_ADDR + 0x014 */
} wm_i2s_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_I2S_REG_STRUCT_H__ */
