/**
 * @file wm_rsa_reg_struct.h
 *
 * @brief RSA_REG_STRUCT Module
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

#ifndef __WM_RSA_REG_STRUCT_H__
#define __WM_RSA_REG_STRUCT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief rsa control register layout
 */
typedef union {
    struct {
        uint32_t modmul_cmplt : 1;  /* [0] Modular multiplication completion flag, active high. 
                                          Set to '1' by hardware, cleared to '0' by software. Writing '1' by software is invalid. */
        uint32_t reserved_1   : 1;  /* [1] Reserved */
        uint32_t mode_sel     : 2;  /* [3:2] Mode Selection for Modular Multiplication:
                                            0: X = D*D mod M
                                            1: D = X*Y mod M
                                            2: X = D*Y mod M
                                            3: D = X*X mod M */
        uint32_t reset        : 1;  /* [4] Software reset function, active high. Writing a "1" to initiates a soft reset, 
                                          and after the reset is complete, the hardware automatically clears it to "0".
                                          1. Set the parameters MC and N to 0.
                                          2. After initiating modular multiplication (by setting bit5 to 1), 
                                             setting this bit to "1" will terminate the current operation 
                                             (when bit0 goes high, indicating that the soft reset command has been executed and the operation has been terminated). 
                                             However, the partially completed results in the internal data buffers (X, Y, M, D) will be preserved. */
        uint32_t start        : 1;  /* [5] Modular Multiplication Start Control Bit. 
                                          Writing "1" by software initiates the modular multiplication operation, 
                                          and after the operation is completed, the hardware automatically clears it to "0". */
        uint32_t reserved_6   : 26; /* [31:6] Reserved */
    };
    uint32_t val;
} wm_rsa_ctrl_t;

/**
 * @brief rsa mc register layout
 */
typedef union {
    struct {
        uint32_t mc; /* Reset value is 0. Write only. Read value is 0. */
    };
    uint32_t val;
} wm_rsa_mc_t;

/**
 * @brief rsa n register layout
 */
typedef union {
    struct {
        uint32_t n : 7; /* [6:0] If calling a 1024-bit modular multiplication operation, it is necessary to set n = 32. 
                                 When writing to this register, the low 7 bits are taken as valid data, and when reading, the high 25 bits will be 0s. 
                                 The reset value is 0. */
        uint32_t reserved_7 : 25; /* [31:7] Reserved */
    };
    uint32_t val;
} wm_rsa_n_t;

/**
 * @brief rsa register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile uint32_t xbuf[64];      /* WM_RSA_BASE_ADDR */
    volatile uint32_t ybuf[64];      /* WM_RSA_BASE_ADDR + 0x100 */
    volatile uint32_t mbuf[64];      /* WM_RSA_BASE_ADDR + 0x200 */
    volatile uint32_t dbuf[64];      /* WM_RSA_BASE_ADDR + 0x300 */
    volatile wm_rsa_ctrl_t rsa_ctrl; /* WM_RSA_BASE_ADDR + 0x400 */
    volatile wm_rsa_mc_t rsa_mc;     /* WM_RSA_BASE_ADDR + 0x404 */
    volatile wm_rsa_n_t rsa_n;       /* WM_RSA_BASE_ADDR + 0x408 */
} wm_rsa_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_RSA_REG_STRUCT_H__ */
