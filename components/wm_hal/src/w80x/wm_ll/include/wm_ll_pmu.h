/**
 * @file wm_ll_pmu.h
 *
 * @brief ll pmu Module
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

#ifndef __WM_LL_PMU_H__
#define __WM_LL_PMU_H__

#include "wm_attr.h"
#include "wm_pmu_reg_struct.h"
#include "wm_reg_op.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the pmu standby enable
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] stdby_en the standby state setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_standby_enable(wm_pmu_reg_t *pmu_dev, bool stdby_en)
{
    pmu_dev->pmu_ctrl.stdby_en = stdby_en;
}

/**
 * @brief Set the pmu sleep enable
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] stdby_en the sleep state setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_sleep_enable(wm_pmu_reg_t *pmu_dev, bool sleep_en)
{
    pmu_dev->pmu_ctrl.sleep_en = sleep_en;
}

/**
 * @brief Set the pmu io sleep enable
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] sleep_en the io sleep state setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_io_sleep_enable(wm_pmu_reg_t *pmu_dev, bool io_sleep_en)
{
    pmu_dev->pmu_ctrl.io_sleep_en = io_sleep_en;
}

/**
 * @brief Set the pmu 32K calibration enable
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] cal_en the 32K calibration setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_32k_calibration_enable(wm_pmu_reg_t *pmu_dev, bool cal_en)
{
    pmu_dev->pmu_ctrl.cal32k_en = cal_en;
}

/**
 * @brief Set the pmu 32K bypass
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] bypass_en the bypass 32K
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_32k_bypass(wm_pmu_reg_t *pmu_dev, bool bypass_en)
{
    pmu_dev->pmu_ctrl.bypass_32k = bypass_en;
}

/**
 * @brief Get the pmu 32K bypass
 * @param[in] pmu_dev the pmu register structure
 *
 * @return the pmu is bypass 32K?
 *
 */
ATTRIBUTE_INLINE bool wm_ll_pmu_get_32k_bypass(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_ctrl.bypass_32k;
}

/**
 * @brief Set the pmu DLDO core voltage select
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] dldo_sel 0: DBG, 1: ABG
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_dldo_core_sel(wm_pmu_reg_t *pmu_dev, bool dldo_sel)
{
    pmu_dev->pmu_ctrl.mux_bg = dldo_sel;
}

/**
 * @brief Set the minimum io pull low keep time to trigger wakeup
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] wake_thres the minimum keep time, uint: 128ms
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_wake_thres_time(wm_pmu_reg_t *pmu_dev, uint8_t wake_thres)
{
    pmu_dev->pmu_ctrl.wake_thres_level = wake_thres;
}

/**
 * @brief Get the pmu control setting
 * @param[in] pmu_dev the pmu register structure
 *
 * @return the pmu ctrl register setting
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_pmu_ctrl(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_ctrl.val;
}

/**
 * @brief Set the timer0 enable setting
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] timer0_en enable timer0
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_timer0_enable(wm_pmu_reg_t *pmu_dev, bool timer0_en)
{
    pmu_dev->pmu_timer0.timer0_en = timer0_en;
}

/**
 * @brief Get the timer0 enable setting
 * @param[in] pmu_dev the pmu register structure
 *
 * @return the timer0 enable setting
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_timer0_enable(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_timer0.timer0_en;
}

/**
 * @brief Set the timer0 value
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] timer0_val timer0 value
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_timer0_value(wm_pmu_reg_t *pmu_dev, uint16_t timer0_val)
{
    WM_LL_REG_FILED_COPY_WRTIE(pmu_dev->pmu_timer0, timer0_val, timer0_val);
}

/**
 * @brief Get the timer0 value
 * @param[in] pmu_dev the pmu register structure
 *
 * @return the timer0 value setting
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_timer0_value(wm_pmu_reg_t *pmu_dev)
{
    return WM_LL_REG_FILED_COPY_READ(pmu_dev->pmu_timer0, timer0_val);
}

/**
 * @brief Set the timer1 setting
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] listen_interval listen interval setting
 *
 * @param[in] timer1_en timer1 enable setting
 *
 * @param[in] timer1_mcs beacon recieve valid
 *
 * @param[in] pre_tbtt pre TBTT setting
 *
 * @param[in] tbtt_timer TBTT timer setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_timer1_ctrl(wm_pmu_reg_t *pmu_dev, uint16_t listen_interval, bool timer1_en,
                                                bool timer1_mcs, uint8_t pre_tbtt, uint8_t tbtt_timer)
{
    wm_pmu_timer1_t fake_pmu_timer1 = { .listen_interval = listen_interval,
                                        .timer1_en       = timer1_en,
                                        .timer1_mcs      = timer1_mcs,
                                        .pre_tbtt_timer  = pre_tbtt,
                                        .tbtt_timer      = tbtt_timer };

    /** limitation: writing whole register is faster than domain write */

    pmu_dev->pmu_timer1.val = fake_pmu_timer1.val;
}

/**
 * @brief Get the timer1 setting
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_timer1_ctrl(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_timer1.val;
}

/**
 * @brief Get the timer0 ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 * @return timer0 isr flag
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_timer0_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_isr.timer0_isr_flag;
}

/**
 * @brief Clear the timer0 ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_clr_timer0_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    wm_pmu_isr_t fake_pmu_isr = { .timer0_isr_flag = 1 };

    /** HW limitation: Can only write this bit */
    pmu_dev->pmu_isr.val = fake_pmu_isr.val;
}

/**
 * @brief Get the timer1 ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 * @return timer1 isr flag
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_timer1_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_isr.timer1_isr_flag;
}

/**
 * @brief Clear the timer1 ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_clr_timer1_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    wm_pmu_isr_t fake_pmu_isr = { .timer1_isr_flag = 1 };

    /** HW limitation: Can only write this bit */
    pmu_dev->pmu_isr.val = fake_pmu_isr.val;
}

/**
 * @brief Get io wake ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 * @return io wake isr flag
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_io_wake_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_isr.io_wake_isr_flag;
}

/**
 * @brief Clear io wake ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_clr_io_wake_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    wm_pmu_isr_t fake_pmu_isr = { .io_wake_isr_flag = 1 };

    /** HW limitation: Can only write this bit */
    pmu_dev->pmu_isr.val = fake_pmu_isr.val;
}

/**
 * @brief Get rtc ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 * @return rtc isr flag
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_rtc_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_isr.rtc_isr_flag;
}

/**
 * @brief Clear rtc ISR flag
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_clr_rtc_isr_flag(wm_pmu_reg_t *pmu_dev)
{
    wm_pmu_isr_t fake_pmu_isr = { .rtc_isr_flag = 1 };

    /** HW limitation: Can only write this bit */
    pmu_dev->pmu_isr.val = fake_pmu_isr.val;
}

/**
 * @brief Get sleep status
 * @param[in] pmu_dev the pmu register structure
 *
 * @return sleep status
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_sleep_status(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_isr.last_sleep_state;
}

/**
 * @brief Clear standby status
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_clr_standby_status(wm_pmu_reg_t *pmu_dev)
{
    wm_pmu_isr_t fake_pmu_isr = { .standby_status = 1 };

    /** HW limitation: Can only write this bit */
    pmu_dev->pmu_isr.val = fake_pmu_isr.val;
}

/**
 * @brief Get standby status
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_pmu_get_standby_status(wm_pmu_reg_t *pmu_dev)
{
    return pmu_dev->pmu_isr.standby_status;
}

/**
 * @brief Clear sleep status
 * @param[in] pmu_dev the pmu register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_clr_sleep_status(wm_pmu_reg_t *pmu_dev)
{
    wm_pmu_isr_t fake_pmu_isr = { .last_sleep_state = 1 };

    /** HW limitation: Can only write this bit */
    pmu_dev->pmu_isr.val = fake_pmu_isr.val;
}

/**
 * @brief Set the XTAL 40M enable
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] xtal_40m_en enable XTAL 40M single
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_wlan_xtal_40m_enable(wm_pmu_reg_t *pmu_dev, bool xtal_40m_en)
{
    pmu_dev->wlan_stts.xtal_40m_en = xtal_40m_en;
}

/**
 * @brief Set the XTAL_BUF_BG_MUX to ABG
 * @param[in] pmu_dev the pmu register structure
 *
 * @param[in] bg_mux 1: choose ABG, 0: choose DBG
 *
 */
ATTRIBUTE_INLINE void wm_ll_pmu_set_xtal_buf_bg_mux(wm_pmu_reg_t *pmu_dev, bool bg_mux)
{
    pmu_dev->pmu_bk.xtal_buf_bg_mux = bg_mux;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_PMU_H__ */
