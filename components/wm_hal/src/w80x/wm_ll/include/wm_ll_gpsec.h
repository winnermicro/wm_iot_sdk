/**
 * @file wm_ll_gpsec.h
 *
 * @brief LL_GPSEC Module
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

#ifndef __WM_LL_GPSEC_H__
#define __WM_LL_GPSEC_H__

#include "wm_attr.h"
#include "wm_gpsec_reg_struct.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_LL_GPSEC_Functions WM LL_GPSEC Functions
 * @brief WinnerMicro LL_GPSEC Functions
 */

/**
 * @addtogroup WM_LL_GPSEC_Functions
 * @{
 */

/**
 * @brief       Set source address
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   source address
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_src_address(wm_gpsec_reg_t *reg, void *address)
{
    reg->src_addr = (uint32_t)address;
}

/**
 * @brief       Get source address
 * @param[in]   reg Pointer to gpsec register structure
 * @return      source address
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_src_address(wm_gpsec_reg_t *reg)
{
    return reg->src_addr;
}

/**
 * @brief       Set destination address
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   destination address
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_dest_address(wm_gpsec_reg_t *reg, void *address)
{
    reg->dest_addr = (uint32_t)address;
}

/**
 * @brief       Get destination address
 * @param[in]   reg Pointer to gpsec register structure
 * @return      destination address
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_dest_address(wm_gpsec_reg_t *reg)
{
    return reg->dest_addr;
}

/**
 * @brief       reset config register
 * @param[in]   reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_reset_config_reg(wm_gpsec_reg_t *reg)
{
    reg->config.val = 0;
}

/**
 * @brief       Set the total bytes to perform encryption or decryption
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   the total bytes
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_total_bytes(wm_gpsec_reg_t *reg, uint16_t bytes)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->config, total_bytes, bytes);
}

/**
 * @brief       Get the total bytes to perform encryption or decryption
 * @param[in]   reg Pointer to gpsec register structure
 * @return      the total bytes
 */
ATTRIBUTE_INLINE uint16_t wm_ll_gpsec_get_total_bytes(wm_gpsec_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->config, total_bytes);
}

/**
 * @brief       Set gpsec cypher mode
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   cypher mode, type of  @arg wm_gpsec_cypher_mode_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_cypher_mode(wm_gpsec_reg_t *reg, wm_gpsec_cypher_mode_t mode)
{
    reg->config.cypher_mode = mode;
}

/**
 * @brief       Get gpsec cypher mode
 * @param[in]   reg Pointer to gpsec register structure
 * @return      cypher mode, type of  @arg wm_gpsec_cypher_mode_t
 */
ATTRIBUTE_INLINE wm_gpsec_cypher_mode_t wm_ll_gpsec_get_cypher_mode(wm_gpsec_reg_t *reg)
{
    return reg->config.cypher_mode;
}

/**
 * @brief          Mask encryption/decryption completion interrupt
 * @param[in]      reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_mask_cmplt_irq(wm_gpsec_reg_t *reg)
{
    reg->config.int_mask = 1;
}

/**
 * @brief          Unmask encryption/decryption completion interrupt
 * @param[in]      reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_unmask_cmplt_irq(wm_gpsec_reg_t *reg)
{
    reg->config.int_mask = 0;
}

/**
 * @brief          Get encryption/decryption completion interrupt mask configuration
 * @param[in]      reg Pointer to gpsec register structure
 * @return         interrupt mask configuration
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_irq_mask(wm_gpsec_reg_t *reg)
{
    return reg->config.int_mask;
}

/**
 * @brief       Select encrypt or decrypt
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   sel type of  @arg wm_gpsec_encrypt_decrypt_sel_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_encrypt_or_decrypt(wm_gpsec_reg_t *reg, wm_gpsec_encrypt_decrypt_sel_t sel)
{
    reg->config.encrypt_decrypt_sel = sel;
}

/**
 * @brief       Get encrypt or decrypt selection
 * @param[in]   reg Pointer to gpsec register structure
 * @return      sel type of  @arg wm_gpsec_encrypt_decrypt_sel_t
 */
ATTRIBUTE_INLINE wm_gpsec_encrypt_decrypt_sel_t wm_ll_gpsec_get_encrypt_or_decrypt(wm_gpsec_reg_t *reg)
{
    return reg->config.encrypt_decrypt_sel;
}

/**
 * @brief       Select algorithm type sub-mode
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   sel type of  @arg wm_gpsec_sub_mode_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_sub_mode(wm_gpsec_reg_t *reg, wm_gpsec_sub_mode_t sel)
{
    reg->config.sub_mode = sel;
}

/**
 * @brief       Get algorithm type sub-mode selection
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_sub_mode_t
 */
ATTRIBUTE_INLINE wm_gpsec_sub_mode_t wm_ll_gpsec_get_sub_mode(wm_gpsec_reg_t *reg)
{
    return reg->config.sub_mode;
}

/**
 * @brief       Select CRC algorithm type
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   sel type of  @arg wm_gpsec_crc_type_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_crc_type(wm_gpsec_reg_t *reg, wm_gpsec_crc_type_t sel)
{
    reg->config.sub_mode = sel;
}

/**
 * @brief       Get CRC algorithm type selection
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_crc_type_t
 */
ATTRIBUTE_INLINE wm_gpsec_crc_type_t wm_ll_gpsec_get_crc_type(wm_gpsec_reg_t *reg)
{
    return reg->config.sub_mode;
}

/**
 * @brief       Set CRC check sum reverse or not
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   set type of  @arg wm_gpsec_crc_reverse_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_crc_reverse(wm_gpsec_reg_t *reg, wm_gpsec_crc_reverse_t set)
{
    reg->config.crc_reverse = set;
}

/**
 * @brief       Get CRC check sum reverse or not
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_crc_reverse_t
 */
ATTRIBUTE_INLINE wm_gpsec_crc_reverse_t wm_ll_gpsec_get_crc_reverse(wm_gpsec_reg_t *reg)
{
    return reg->config.crc_reverse;
}

/**
 * @brief          Soft reset RC4 algorithm
 * @param[in]      reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_rc4_soft_reset(wm_gpsec_reg_t *reg)
{
    reg->config.rc4_soft_reset = 1;
}

/**
 * @brief          Get RC4 algorithm soft reset bit
 * @param[in]      reg Pointer to gpsec register structure
 * @return         RC4 algorithm soft reset bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_rc4_soft_reset(wm_gpsec_reg_t *reg)
{
    return reg->config.rc4_soft_reset;
}

/**
 * @brief          Soft reset AES algorithm
 * @param[in]      reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_aes_soft_reset(wm_gpsec_reg_t *reg)
{
    reg->config.aes_soft_reset = 1;
}

/**
 * @brief          Get AES algorithm soft reset bit
 * @param[in]      reg Pointer to gpsec register structure
 * @return         AES algorithm soft reset bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_aes_soft_reset(wm_gpsec_reg_t *reg)
{
    return reg->config.aes_soft_reset;
}

/**
 * @brief          Soft reset DES algorithm
 * @param[in]      reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_des_soft_reset(wm_gpsec_reg_t *reg)
{
    reg->config.des_soft_reset = 1;
}

/**
 * @brief          Get DES algorithm soft reset bit
 * @param[in]      reg Pointer to gpsec register structure
 * @return         DES algorithm soft reset bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_des_soft_reset(wm_gpsec_reg_t *reg)
{
    return reg->config.des_soft_reset;
}

/**
 * @brief       Set pseudo random number bit 
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   set type of  @arg wm_gpsec_rng_bits_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_prng_bit_length(wm_gpsec_reg_t *reg, wm_gpsec_prng_bits_t set)
{
    reg->config.prng_bits_length_sel = set;
}

/**
 * @brief       Get pseudo random number bit
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_rng_bits_t
 */
ATTRIBUTE_INLINE wm_gpsec_prng_bits_t wm_ll_gpsec_get_rng_bit_length(wm_gpsec_reg_t *reg)
{
    return reg->config.prng_bits_length_sel;
}

/**
 * @brief       Set prng load seed method
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   set type of  @arg wm_gpsec_rng_load_seed_method_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_prng_load_seed_method(wm_gpsec_reg_t *reg, wm_gpsec_prng_load_seed_method_t set)
{
    reg->config.prng_load_seed_method = set;
}

/**
 * @brief       Get prng load seed method
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_rng_load_seed_method_t
 */
ATTRIBUTE_INLINE wm_gpsec_prng_load_seed_method_t wm_ll_gpsec_get_prng_load_seed_method(wm_gpsec_reg_t *reg)
{
    return reg->config.prng_load_seed_method;
}

/**
 * @brief       Set prng start
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   start
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_prng_start(wm_gpsec_reg_t *reg, uint32_t start)
{
    reg->config.prng_start = start;
}

/**
 * @brief       Get prng start
 * @param[in]   reg Pointer to gpsec register structure
 * @return      prng start bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_prng_start(wm_gpsec_reg_t *reg)
{
    return reg->config.prng_start;
}

/**
 * @brief       Set rc4 block length
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   length,type of  @arg wm_gpsec_rc4_block_length_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_rc4_block_length(wm_gpsec_reg_t *reg, wm_gpsec_rc4_block_length_t length)
{
    reg->config.rc4_128_256_sel = length;
}

/**
 * @brief       Get rc4 block length
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_rc4_block_length_t
 */
ATTRIBUTE_INLINE wm_gpsec_rc4_block_length_t wm_ll_gpsec_get_rc4_block_length(wm_gpsec_reg_t *reg)
{
    return reg->config.rc4_128_256_sel;
}

/**
 * @brief       Set crypto start
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   start
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_operation_start(wm_gpsec_reg_t *reg, uint32_t start)
{
    reg->ctrl.sec_start = start;
}

/**
 * @brief       Get gpsec start
 * @param[in]   reg Pointer to gpsec register structure
 * @return      gpsec start bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_crypto_start(wm_gpsec_reg_t *reg)
{
    return reg->ctrl.sec_start;
}

/**
 * @brief       Set crypto stop
 * @param[in]   reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_stop_crypto(wm_gpsec_reg_t *reg)
{
    reg->ctrl.sec_stop = 1;
}

/**
 * @brief       Get crypto stop
 * @param[in]   reg Pointer to gpsec register structure
 * @return      crypto stop bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_crypto_stop(wm_gpsec_reg_t *reg)
{
    return reg->ctrl.sec_stop;
}

/**
 * @brief       Clear crc fifo
 * @param[in]   reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_clear_crc_fifo(wm_gpsec_reg_t *reg)
{
    reg->ctrl.crc_fifo_clr = 1;
}

/**
 * @brief       Get the number of bytes completed for encryption/decryption
 * @param[in]   reg Pointer to gpsec register structure
 * @return      the number of bytes completed for encryption/decryption
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_cmplt_bytes(wm_gpsec_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->status, transfer_cnt);
}

/**
 * @brief          Get gpsec Encryption/Decryption completion interrupt src
 * @param[in]      reg Pointer to gpsec register structure.
 * @retval         gpsec interrupt src
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_irq_flag(wm_gpsec_reg_t *reg)
{
    return reg->status.int_flag;
}

/**
 * @brief          Clear gpsec Encryption/Decryption completion interrupt
 * @param[in]      reg Pointer to gpsec register structure.
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_clear_irq_flag(wm_gpsec_reg_t *reg)
{
    reg->status.int_flag = 1;
}

/**
 * @brief Write the key to the CRYP peripheral
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @param[in] key key to be written to CRYP peripheral
 * @param[in] word_len word number of the key
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_key(wm_gpsec_reg_t *reg, void *key, uint32_t word_len)
{
    uint32_t *key_words    = (uint32_t *)key;
    uint32_t *reg_addr_buf = (uint32_t *)&(reg->key00_seed);

    for (uint32_t i = 0; i < word_len && i < 6; i++) {
        reg_addr_buf[i] = key_words[i];
    }
    if (word_len == 8) {
        reg->key30 = key_words[6];
        reg->key31 = key_words[7];
    }
}

/**
 * @brief Read the key from the CRYP peripheral
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @param[out] key key to be read from the CRYP peripheral
 * @param[in] word_len word number of the key
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_get_key(wm_gpsec_reg_t *reg, void *key, uint32_t word_len)
{
    uint32_t *key_words    = (uint32_t *)key;
    uint32_t *reg_addr_buf = (uint32_t *)&(reg->key00_seed);

    for (uint32_t i = 0; i < word_len && i < 6; i++) {
        key_words[i] = reg_addr_buf[i];
    }
    if (word_len == 8) {
        key_words[6] = reg->key30;
        key_words[7] = reg->key31;
    }
}

/**
 * @brief Write iv to the CRYP peripheral
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @param[in] iv iv to be written to CRYP peripheral
 * @param[in] word_len word number of the iv
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_iv(wm_gpsec_reg_t *reg, void *iv, uint32_t word_len)
{
    uint32_t *iv_words = (uint32_t *)iv;

    if (word_len >= 2) {
        reg->iv00 = iv_words[0];
        reg->iv01 = iv_words[1];
    }
    if (word_len == 4) {
        reg->key20_iv10 = iv_words[2];
        reg->key21_iv11 = iv_words[3];
    }
}

/**
 * @brief Read iv from the CRYP peripheral
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @param[out] iv iv to be read from the CRYP peripheral
 * @param[in] word_len word number of the iv
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_get_iv(wm_gpsec_reg_t *reg, void *iv, uint32_t word_len)
{
    uint32_t *iv_words = (uint32_t *)iv;

    if (word_len >= 2) {
        iv_words[0] = reg->iv00;
        iv_words[1] = reg->iv01;
    }
    if (word_len == 4) {
        iv_words[2] = reg->key20_iv10;
        iv_words[3] = reg->key21_iv11;
    }
}

/**
 * @brief Write the message digest to the SHA engine
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @param[in] digest_state Message digest to be written to SHA engine
 * @param[in] digest_word_len Word number of the message digest
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_digest(wm_gpsec_reg_t *reg, void *digest_state, uint32_t word_len)
{
    uint32_t *digest_state_words = (uint32_t *)digest_state;
    uint32_t *reg_addr_buf       = (uint32_t *)&(reg->sha1_md5_digest0);

    //fixme memcpy maybe better
    for (uint32_t i = 0; i < word_len; i++) {
        reg_addr_buf[i] = digest_state_words[i];
    }
}

/**
 * @brief Read the message digest from the SHA engine
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @param[out] digest_state Buffer that message digest will be written to
 * @param[in] digest_word_len Word number of the message digest
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_get_digest(wm_gpsec_reg_t *reg, void *digest_state, uint32_t word_len)
{
    uint32_t *digest_state_words = (uint32_t *)digest_state;
    uint32_t *reg_addr_buf       = (uint32_t *)&(reg->sha1_md5_digest0);

    for (uint32_t i = 0; i < word_len; i++) {
        digest_state_words[i] = reg_addr_buf[i];
    }
}

/**
 * @brief Set CRC input data
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @param[in] value CRC input data
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_crc_value(wm_gpsec_reg_t *reg, uint32_t value)
{
    reg->sha1_digest4_crc_in_out = value;
}

/**
 * @brief Get CRC output data
 *
 * @param[in] reg Pointer to gpsec register structure.
 * @return    CRC result
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_crc_value(wm_gpsec_reg_t *reg)
{
    return reg->sha1_digest4_crc_in_out;
}

/**
 * @brief       Set the seed for generating pseudo-random numbers
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   seed
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_prng_seed(wm_gpsec_reg_t *reg, uint32_t seed)
{
    reg->key00_seed = seed;
}

/**
 * @brief       Get the seed for generating pseudo-random numbers
 * @param[in]   reg Pointer to gpsec register structure
 * @return      seed
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_prng_seed(wm_gpsec_reg_t *reg)
{
    return reg->key00_seed;
}

/**
 * @brief       Get rng result
 * @param[in]   reg Pointer to gpsec register structure
 * @return      rng result
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_rng(wm_gpsec_reg_t *reg)
{
    return reg->rng_result;
}

/**
 * @brief       Set trng start
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   start
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_trng_start(wm_gpsec_reg_t *reg, uint32_t start)
{
    reg->trng_cr.trng_en = start;
}

/**
 * @brief       Get trng start bit
 * @param[in]   reg Pointer to gpsec register structure
 * @return      trng start bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_trng_start(wm_gpsec_reg_t *reg)
{
    return reg->trng_cr.trng_en;
}

/**
 * @brief       Select result register display
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   sel,type of  @arg wm_gpsec_result_select_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_rng_result_select(wm_gpsec_reg_t *reg, wm_gpsec_result_select_t sel)
{
    reg->trng_cr.trng_sel = sel;
}

/**
 * @brief       Get result register display selection
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_result_select_t
 */
ATTRIBUTE_INLINE wm_gpsec_result_select_t wm_ll_gpsec_get_rng_result_select(wm_gpsec_reg_t *reg)
{
    return reg->trng_cr.trng_sel;
}

/**
 * @brief       Set TRNG digital post-processing
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   set,type of  @arg wm_gpsec_trng_dig_process_bypass_t
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_trng_dig_post_process(wm_gpsec_reg_t *reg, wm_gpsec_trng_dig_process_bypass_t set)
{
    reg->trng_cr.trng_dig_bypass = set;
}

/**
 * @brief       Get TRNG digital post-processing
 * @param[in]   reg Pointer to gpsec register structure
 * @return      type of  @arg wm_gpsec_trng_dig_process_bypass_t
 */
ATTRIBUTE_INLINE wm_gpsec_trng_dig_process_bypass_t wm_ll_gpsec_get_trng_dig_post_process(wm_gpsec_reg_t *reg)
{
    return reg->trng_cr.trng_dig_bypass;
}

/**
 * @brief       Set TRNG module charge pump configuration
 * @param[in]   reg Pointer to gpsec register structure
 * @param[in]   set, charge pump configuration
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_set_trng_charge_pump(wm_gpsec_reg_t *reg, uint32_t set)
{
    reg->trng_cr.trng_charge_pump = set;
}

/**
 * @brief       Get TRNG module charge pump configuration
 * @param[in]   reg Pointer to gpsec register structure
 * @return      charge pump configuration
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_trng_charge_pump(wm_gpsec_reg_t *reg)
{
    return reg->trng_cr.trng_charge_pump;
}

/**
 * @brief          Mask trng interrupt
 * @param[in]      reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_mask_trng_irq(wm_gpsec_reg_t *reg)
{
    reg->trng_cr.trng_int_mask = 1;
}

/**
 * @brief          Unmask trng interrupt
 * @param[in]      reg Pointer to gpsec register structure
 */
ATTRIBUTE_INLINE void wm_ll_gpsec_unmask_trng_irq(wm_gpsec_reg_t *reg)
{
    reg->trng_cr.trng_int_mask = 0;
}

/**
 * @brief          Get trng interrupt mask configuration
 * @param[in]      reg Pointer to gpsec register structure
 * @return         interrupt mask configuration
 */
ATTRIBUTE_INLINE uint32_t wm_ll_gpsec_get_trng_mask(wm_gpsec_reg_t *reg)
{
    return reg->trng_cr.trng_int_mask;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_GPSEC_H__ */
