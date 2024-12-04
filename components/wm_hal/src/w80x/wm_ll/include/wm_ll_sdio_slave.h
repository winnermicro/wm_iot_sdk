/**
 * @file wm_ll_sdio_slave.h
 *
 * @brief sdio slave low level operation
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

#ifndef __WM_LL_SDIO_SLAVE_H__
#define __WM_LL_SDIO_SLAVE_H__

#include "wm_attr.h"
#include "wm_types.h"
#include "wm_reg_op.h"
#include "wm_sdio_slave_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          get cis f0 offset
 * @param[in]      sdio slave hardware ahb register base address
 * @return         cis f0 offset
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_cisf0_addr(wm_sdio_slave_reg_t *reg)
{
    return reg->cis_f0_addr;
}

/**
 * @brief          set cis f0 offset
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      cis f0 offset
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_cisf0_addr(wm_sdio_slave_reg_t *reg, uint32_t oft)
{
    reg->cis_f0_addr = oft;
}

/**
 * @brief          get cis f1 offset
 * @param[in]      sdio slave hardware ahb register base address
 * @return         cis f1 offset
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_cisf1_addr(wm_sdio_slave_reg_t *reg)
{
    return reg->cis_f1_addr;
}

/**
 * @brief          set cis f1 offset
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      cis f1 offset
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_cisf1_addr(wm_sdio_slave_reg_t *reg, uint32_t oft)
{
    reg->cis_f1_addr = oft;
}

/**
 * @brief          get ahb transfer cnt
 * @param[in]      sdio slave hardware ahb register base address
 * @return         ahb xfer cnt
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_ahb_xfer_cnt(wm_sdio_slave_reg_t *reg)
{
    return reg->ahb_xfer.ahb_xcnt;
}

/**
 * @brief          set ahb transfer cnt
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      ahb xfer cnt
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_ahb_xfer_cnt(wm_sdio_slave_reg_t *reg, uint32_t cnt)
{
    reg->ahb_xfer.ahb_xcnt = cnt;
}

/**
 * @brief          get sdio transfer cnt
 * @param[in]      sdio slave hardware ahb register base address
 * @return         sdio xfer cnt
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_sdio_xfer_cnt(wm_sdio_slave_reg_t *reg)
{
    return reg->sdio_xfer.sdio_xcnt;
}

/**
 * @brief          get cccr version
 * @param[in]      sdio slave hardware ahb register base address
 * @return         cccr version
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_ccr_version(wm_sdio_slave_reg_t *reg)
{
    return reg->cia.cccr_revision;
}

/**
 * @brief          set cccr version
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      cccr version
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_ccr_version(wm_sdio_slave_reg_t *reg, uint32_t version)
{
    reg->cia.cccr_revision = version;
}

/**
 * @brief          get sdio version
 * @param[in]      sdio slave hardware ahb register base address
 * @return         sdio version
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_sdio_version(wm_sdio_slave_reg_t *reg)
{
    return reg->cia.sdio_revision;
}

/**
 * @brief          set sdio version
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      sdio version
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_sdio_version(wm_sdio_slave_reg_t *reg, uint32_t version)
{
    reg->cia.sdio_revision = version;
}

/**
 * @brief          get sd spec version
 * @param[in]      sdio slave hardware ahb register base address
 * @return         sd version id
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_sd_version(wm_sdio_slave_reg_t *reg)
{
    return reg->cia.sd_revision;
}

/**
 * @brief          set sd spec version
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      sd version id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_sd_version(wm_sdio_slave_reg_t *reg, uint32_t version)
{
    reg->cia.sd_revision = version;
}

/**
 * @brief          get io device interface code
 * @param[in]      sdio slave hardware ahb register base address
 * @return         interface code
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_iodev_code(wm_sdio_slave_reg_t *reg)
{
    return reg->cia.io_code;
}

/**
 * @brief          set io device interface code
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      interface code id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_iodev_code(wm_sdio_slave_reg_t *reg, uint32_t code)
{
    reg->cia.io_code = code;
}

/**
 * @brief          get csa support
 * @param[in]      sdio slave hardware ahb register base address
 * @return         csa support
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_csa_sup(wm_sdio_slave_reg_t *reg)
{
    return reg->cia.csa_sup;
}

/**
 * @brief          set csa support
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      csa support
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_csa_sup(wm_sdio_slave_reg_t *reg, uint32_t csa_sup)
{
    reg->cia.csa_sup = csa_sup;
}

/**
 * @brief          get extend io device interface code
 * @param[in]      sdio slave hardware ahb register base address
 * @return         extend interface code
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_ex_iodev_code(wm_sdio_slave_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->cia, ex_io_code);
}

/**
 * @brief          set extend io device interface code
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      extend interface code id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_ex_iodev_code(wm_sdio_slave_reg_t *reg, uint32_t code)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->cia, ex_io_code, code);
}

/**
 * @brief          get sps
 * @param[in]      sdio slave hardware ahb register base address
 * @return         sps
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_sps(wm_sdio_slave_reg_t *reg)
{
    return reg->cia.sps;
}

/**
 * @brief          set sps
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      sps control
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_sps(wm_sdio_slave_reg_t *reg, uint32_t sps)
{
    reg->cia.sps = sps;
}

/**
 * @brief          get high speed
 * @param[in]      sdio slave hardware ahb register base address
 * @return         high speed control
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_shs(wm_sdio_slave_reg_t *reg)
{
    return reg->cia.shs;
}

/**
 * @brief          set high speed
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      high speed control
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_shs(wm_sdio_slave_reg_t *reg, uint32_t hs)
{
    reg->cia.shs = hs;
}

/**
 * @brief          get f1 ready
 * @param[in]      sdio slave hardware ahb register base address
 * @return         f1 ready
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_f1_ready(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.fn1_ready;
}

/**
 * @brief          set f1 ready
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      f1 ready
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_f1_ready(wm_sdio_slave_reg_t *reg, uint32_t ready)
{
    reg->pr.fn1_ready = ready;
}

/**
 * @brief          get f1 read data ready
 * @param[in]      sdio slave hardware ahb register base address
 * @return         f1 read data ready
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_f1_rd_ready(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.fn1_read_ready;
}

/**
 * @brief          set f1 read data ready
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      f1 read data ready
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_f1_rd_ready(wm_sdio_slave_reg_t *reg, uint32_t ready)
{
    reg->pr.fn1_read_ready = ready;
}

/**
 * @brief          get scsi
 * @param[in]      sdio slave hardware ahb register base address
 * @return         scsi
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_scsi(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.scsi;
}

/**
 * @brief          set scsi
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      scsi
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_scsi(wm_sdio_slave_reg_t *reg, uint32_t scsi)
{
    reg->pr.scsi = scsi;
}

/**
 * @brief          get sdc
 * @param[in]      sdio slave hardware ahb register base address
 * @return         sdc
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_sdc(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.sdc;
}

/**
 * @brief          set sdc
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      sdc
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_sdc(wm_sdio_slave_reg_t *reg, uint32_t sdc)
{
    reg->pr.sdc = sdc;
}

/**
 * @brief          get smb
 * @param[in]      sdio slave hardware ahb register base address
 * @return         smb
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_smb(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.smb;
}

/**
 * @brief          set smb
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      smb
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_smb(wm_sdio_slave_reg_t *reg, uint32_t smb)
{
    reg->pr.smb = smb;
}

/**
 * @brief          get srw
 * @param[in]      sdio slave hardware ahb register base address
 * @return         srw
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_srw(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.srw;
}

/**
 * @brief          set srw
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      srw
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_srw(wm_sdio_slave_reg_t *reg, uint32_t srw)
{
    reg->pr.srw = srw;
}

/**
 * @brief          get sbs
 * @param[in]      sdio slave hardware ahb register base address
 * @return         sbs
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_sbs(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.sbs;
}

/**
 * @brief          set sbs
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      sbs
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_sbs(wm_sdio_slave_reg_t *reg, uint32_t sbs)
{
    reg->pr.sbs = sbs;
}

/**
 * @brief          get s4mi
 * @param[in]      sdio slave hardware ahb register base address
 * @return         s4mi
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_s4mi(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.s4mi;
}

/**
 * @brief          set s4mi
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      s4mi
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_s4mi(wm_sdio_slave_reg_t *reg, uint32_t s4mi)
{
    reg->pr.s4mi = s4mi;
}

/**
 * @brief          get lsc
 * @param[in]      sdio slave hardware ahb register base address
 * @return         lsc
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_lsc(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.lsc;
}

/**
 * @brief          set lsc
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      lsc
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_lsc(wm_sdio_slave_reg_t *reg, uint32_t lsc)
{
    reg->pr.lsc = lsc;
}

/**
 * @brief          get bls4
 * @param[in]      sdio slave hardware ahb register base address
 * @return         bls4
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_bls4(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.bls4;
}
/**
 * @brief          set bls4
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      bls4
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_bls4(wm_sdio_slave_reg_t *reg, uint32_t bls4)
{
    reg->pr.bls4 = bls4;
}

/**
 * @brief          get card ready
 * @param[in]      sdio slave hardware ahb register base address
 * @return         card ready
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_card_ready(wm_sdio_slave_reg_t *reg)
{
    return reg->pr.card_ready;
}

/**
 * @brief          set card ready
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      card ready
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_card_ready(wm_sdio_slave_reg_t *reg, uint32_t card_ready)
{
    reg->pr.card_ready = card_ready;
}

/**
 * @brief          get voltage
 * @param[in]      sdio slave hardware ahb register base address
 * @return         card voltage
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_card_volatage(wm_sdio_slave_reg_t *reg)
{
    return reg->ocr.work_vo;
}

/**
 * @brief          set voltage
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      card voltage
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_card_volatage(wm_sdio_slave_reg_t *reg, uint32_t vol)
{
    reg->ocr.work_vo = WM_BIT(vol);
}

/**
 * @brief          get card cd status
 * @param[in]      sdio slave hardware ahb register base address
 * @retrun         card cd status
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_slave_get_cds(wm_sdio_slave_reg_t *reg)
{
    return reg->cd_state.sdd3;
}

/**
 * @brief          set card cd status
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      card cd status
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_cds(wm_sdio_slave_reg_t *reg, uint32_t cds)
{
    reg->cd_state.sdd3 = cds;
}

/**
 * @brief          get fn1 enable
 * @param[in]      sdio slave hardware ahb register base address
 * @return         fn1 enable
 */
ATTRIBUTE_INLINE bool wm_ll_sdio_slave_get_f1_en(wm_sdio_slave_reg_t *reg)
{
    return !!(reg->f1_en.fn1_en);
}

/**
 * @brief          f1 enable disable control
 * @param[in]      sdio slave hardware ahb register base address
 * @param[in]      fn1 enable
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_slave_set_f1_en(wm_sdio_slave_reg_t *reg, bool en)
{
    reg->f1_en.fn1_en = en;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_SDIO_SLAVE_H__ */
