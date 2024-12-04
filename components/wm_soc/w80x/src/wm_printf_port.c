/**
 * @file wm_printf_port.c
 *
 * @brief printf port
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

#include "wmsdk_config.h"
#include "wm_soc_cfgs.h"

#include "wm_dt.h"
#include "wm_dt_hw.h"
#include "wm_hal_gpio.h"
#include "wm_ll_uart.h"

wm_device_t *g_log_uart = NULL;
/*
At early stage, uart driver was not initialized.
Init it directly
*/
void wm_log_port_init(void)
{
    int i;
    unsigned int bandrate;
    unsigned int bd;

    wm_dt_hw_uart_t *hw;
    wm_uart_reg_t *uart_reg;

    g_log_uart = wm_dt_get_device_by_name(CONFIG_LOG_PRINT_UART_DEVICE);
    if (g_log_uart) {
        hw       = g_log_uart->hw;
        uart_reg = (wm_uart_reg_t *)(hw->reg_base);

        /*pin config*/
        for (i = 0; i < hw->pin_cfg_count; i++) {
            wm_hal_gpio_iomux_func_sel(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux);
            wm_hal_gpio_set_pullmode(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_pupd);
            wm_hal_gpio_set_dir(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_dir);
        }

        /*uart config*/
        bandrate = hw->uart_cfg.baudrate;

        bd = (WM_APB_CLOCK / (16 * bandrate) - 1) | (((WM_APB_CLOCK % (bandrate * 16)) * 16 / (bandrate * 16)) << 16);

        wm_ll_uart_set_baudrate(uart_reg, bd);
        wm_ll_uart_set_data_bits(uart_reg, hw->uart_cfg.data_bits);
        wm_ll_uart_set_stop_bits(uart_reg, hw->uart_cfg.stop_bits);
        wm_ll_uart_set_flow_ctrl(uart_reg, 0);
        wm_ll_uart_reset_tx_fifo(uart_reg);
        wm_ll_uart_set_dma_ctrl(uart_reg, 0);
        wm_ll_uart_enable_afc(uart_reg, 0);
        wm_ll_uart_enable_tx(uart_reg, 1);
    }
}
