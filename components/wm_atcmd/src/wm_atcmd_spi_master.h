/**
 * @file wm_atcmd_spi_master.h
 *
 * @brief Default spi master AT Command
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

#ifndef __WM_ATCMD_SPI_MASTER_H__
#define __WM_ATCMD_SPI_MASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_SPIM_ENABLE

#define WM_ATCMD_SPIM_CONF_GPIO_USAGE                  \
    "AT+DRVSPIMCONFGPIO=<mosi>,<miso>,<sclk>,<cs>\r\n" \
    "example:AT+DRVSPIMCONFGPIO=21,16,17,20\r\n"

#define WM_ATCMD_SPIM_INIT_USAGE        \
    "AT+DRVSPIMINIT=<clock>,<mode>\r\n" \
    "example:AT+DRVSPIMINIT=2,1\r\n"

#define WM_ATCMD_SPIM_RD_USAGE    \
    "AT+DRVSPIMRD=<data_len>\r\n" \
    "example:AT+DRVSPIMRD=10\r\n"

#define WM_ATCMD_SPIM_WR_USAGE    \
    "AT+DRVSPIMWR=<data_len>\r\n" \
    "example:AT+DRVSPIMWR=100\r\n"

int wm_atcmd_spim_config_gpio(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_spim_init(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_spim_rd(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_spim_wr(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+DRVSPIMCONFGPIO", wm_atcmd_spim_config_gpio, WM_ATCMD_SPIM_CONF_GPIO_USAGE);
WM_ATCMD_DEFINE("AT+DRVSPIMINIT", wm_atcmd_spim_init, WM_ATCMD_SPIM_INIT_USAGE);
WM_ATCMD_DEFINE("AT+DRVSPIMRD", wm_atcmd_spim_rd, WM_ATCMD_SPIM_RD_USAGE);
WM_ATCMD_DEFINE("AT+DRVSPIMWR", wm_atcmd_spim_wr, WM_ATCMD_SPIM_WR_USAGE);

#endif //CONFIG_WM_ATCMD_SPIM_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_SPI_MASTER_H__ */
