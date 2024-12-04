/**
 * @file wm_gpsec_reg_struct.h
 *
 * @brief GPSEC_REG_STRUCT Module
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

#ifndef __WM_GPSEC_REG_STRUCT_H__
#define __WM_GPSEC_REG_STRUCT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief gpsec configuratoin register layout
 */
typedef union {
    struct {
        uint32_t total_bytes : 16; /* [15:0] The total number of bytes that require encryption and decryption operations.*/
        uint32_t cypher_mode : 3;  /* [18:16] Cryptographic Algorithm Type
                                              0: RSV
                                              1: RC4
                                              2: SHA1
                                              3: AES
                                              4: DES
                                              5: 3DES
                                              6: CRC
                                              7: MD5    */
        uint32_t int_mask    : 1;  /* [19] 0: unmask encryption/decryption completion interrupt
                                           1: mask encryption/decryption completion interrupt    */
        uint32_t encrypt_decrypt_sel : 1; /* [20] Selection of encryption or decryption Mode for RC4/AES/DES/3DES Algorithms 
                                              0: Encryption
                                              1: Decryption*/
        uint32_t sub_mode            : 2; /* [22:21] Algorithm type sub-mode selection:
                                              0: ECB mode for DES/AES cryptographic algorithms, CRC8 mode for CRC algorithm
                                              1: CBC mode for DES/AES cryptographic algorithms, CRC16_0 mode for CRC algorithm
                                              2: CTR mode for AES cryptographic algorithm, CRC16_1 mode for CRC algorithm
                                              3: MAC mode for AES cryptographic algorithm, CRC32 mode for CRC algorithm    */
        uint32_t crc_reverse         : 2; /* [24:23] CRC check sum and input reverse configuration 
                                              0: CRC check sum and input not reverse 
                                              1: CRC check sum reverse 
                                              2: CRC input reverse 
                                              3: CRC check sum and input reverse */
        uint32_t rc4_soft_reset : 1; /* [25] RC4 soft reset, after reset completion, the hardware automatically clears to 0. 
                                          0: No soft reset is generated and the current state remains unchanged.
                                          1: The encryption algorithm is soft-reset to its initial state by software.*/
        uint32_t aes_soft_reset : 1; /* [26] AES soft reset, after reset completion, the hardware automatically clears to 0. 
                                          0: No soft reset is generated and the current state remains unchanged.
                                          1: The encryption algorithm is soft-reset to its initial state by software.*/
        uint32_t des_soft_reset : 1; /* [27] DES soft reset, after reset completion, the hardware automatically clears to 0. 
                                          0: No soft reset is generated and the current state remains unchanged.
                                          1: The encryption algorithm is soft-reset to its initial state by software.*/
        uint32_t prng_bits_length_sel  : 1; /* [28] Control the number of bits generated for a pseudo random number. 
                                              0: 16 bits
                                              1: 32 bits*/
        uint32_t prng_load_seed_method : 1; /* [29] Hardware automatically clears to zero. 
                                                   fixme: Hardware clear this bit to zero or the seed?
                                               0: The pseudo random number generator will default to using zero as the seed 
                                                  and generate a random number with the corresponding number of bits.
                                               1: After the seed is loaded, the random number generation will start.*/
        uint32_t prng_start            : 1; /* [30] Set 1 to start the pseudo random number generation.*/
        uint32_t rc4_128_256_sel       : 1; /* [31] 0: RC4 encryption/decryption is performed with a block length of 128 bits.
                                                    1: RC4 encryption/decryption is performed with a block length of 256 bits.*/
    };
    uint32_t val;
} wm_gpsec_cfg_t;

/**
 * @brief gpsec control register layout
 */
typedef union {
    struct {
        uint32_t sec_start    : 1;  /* [0] Set 1 to start encryption/decryption, 
                                       after completing the specified number of bytes for encryption/decryption operations, 
                                       the hardware automatically resets to zero*/
        uint32_t sec_stop     : 1;  /* [1] Set 1 to stop the currently ongoing encryption/decryption operation.*/
        uint32_t crc_fifo_clr : 1;  /* [2] Set 1 to clear crc fifo fixme */
        uint32_t reserved_3   : 29; /* [31:3] Reserved */
    };
    uint32_t val;
} wm_gpsec_ctrl_t;

/**
 * @brief gpsec status register layout
 */
typedef union {
    struct {
        uint32_t transfer_cnt : 16; /* [15:0] The current number of bytes completed for encryption/decryption. 
                                              It resets to 0 and starts counting from the beginning each time encryption/decryption is restarted.*/
        uint32_t int_flag     : 1;  /* [16] 0: No Encryption/Decryption completion interrupt occurred.
                                            1: An Encryption/Decryption completion interrupt has occurred.
                                            Write 1 to clear the flag.*/
        uint32_t reserved_17  : 15; /* [31:17] Reserved */
    };
    uint32_t val;
} wm_gpsec_sts_t;

/**
 * @brief trng control register layout
 */
typedef union {
    struct {
        uint32_t trng_en          : 1;  /* [0] Set 1 to start trng */
        uint32_t trng_sel         : 1;  /* [1] rng result selection.
                                           0: Result register displays the result of the Pseudo-Random Number Generator (PRNG);
                                           1: Result register displays the result of the True Random Number Generator (TRNG).*/
        uint32_t trng_dig_bypass  : 1;  /* [2] TRNG digital post-processing module bypass or not:
                                           0: The TRNG module performs post-processing.
                                           1: The TRNG module bypasses post-processing. */
        uint32_t trng_charge_pump : 3;  /* [5:3] TRNG module charge pump fixme */
        uint32_t trng_int_mask    : 1;  /* [6]  0: unmask trng interrupt
                                               1: mask trng interrupt    */
        uint32_t reserved_7       : 25; /* [31:7] Reserved */
    };
    uint32_t val;
} wm_trng_ctrl_t;

/**
 * @brief gpsec register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile uint32_t src_addr;                /* WM_GPSEC_BASE_ADDR */
    volatile uint32_t dest_addr;               /* WM_GPSEC_BASE_ADDR + 0x004 */
    volatile wm_gpsec_cfg_t config;            /* WM_GPSEC_BASE_ADDR + 0x008 */
    volatile wm_gpsec_ctrl_t ctrl;             /* WM_GPSEC_BASE_ADDR + 0x00C */
    volatile uint32_t key00_seed;              /* WM_GPSEC_BASE_ADDR + 0x010 */
    volatile uint32_t key01;                   /* WM_GPSEC_BASE_ADDR + 0x014 */
    volatile uint32_t key10;                   /* WM_GPSEC_BASE_ADDR + 0x018 */
    volatile uint32_t key11;                   /* WM_GPSEC_BASE_ADDR + 0x01C */
    volatile uint32_t key20_iv10;              /* WM_GPSEC_BASE_ADDR + 0x020 */
    volatile uint32_t key21_iv11;              /* WM_GPSEC_BASE_ADDR + 0x024 */
    volatile uint32_t iv00;                    /* WM_GPSEC_BASE_ADDR + 0x028 */
    volatile uint32_t iv01;                    /* WM_GPSEC_BASE_ADDR + 0x02C */
    volatile wm_gpsec_sts_t status;            /* WM_GPSEC_BASE_ADDR + 0x030 */
    volatile uint32_t sha1_md5_digest0;        /* WM_GPSEC_BASE_ADDR + 0x034 */
    volatile uint32_t sha1_md5_digest1;        /* WM_GPSEC_BASE_ADDR + 0x038 */
    volatile uint32_t sha1_md5_digest2;        /* WM_GPSEC_BASE_ADDR + 0x03C */
    volatile uint32_t sha1_md5_digest3;        /* WM_GPSEC_BASE_ADDR + 0x040 */
    volatile uint32_t sha1_digest4_crc_in_out; /* WM_GPSEC_BASE_ADDR + 0x044 */
    volatile uint32_t rng_result;              /* WM_GPSEC_BASE_ADDR + 0x048 */
    volatile uint32_t key30;                   /* WM_GPSEC_BASE_ADDR + 0x04C */
    volatile uint32_t key31;                   /* WM_GPSEC_BASE_ADDR + 0x050 */
    volatile wm_trng_ctrl_t trng_cr;           /* WM_GPSEC_BASE_ADDR + 0x054 */
} wm_gpsec_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_GPSEC_REG_STRUCT_H__ */
