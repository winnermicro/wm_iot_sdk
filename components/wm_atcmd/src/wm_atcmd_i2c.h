/**
 * @file wm_atcmd_i2c.h
 *
 * @brief Default I2C AT Command
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

#ifndef __WM_ATCMD_I2C_H__
#define __WM_ATCMD_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_I2C_ENABLE

#define WM_ATCMD_I2C_INIT_USAGE                         \
    "AT+DRVI2CINIT=<num>,<scl_io>,<sda_io>,<clock>\r\n" \
    "example:AT+DRVI2CINIT=0,1,4,400000\r\n"

#define WM_ATCMD_I2C_RD_USAGE                  \
    "AT+DRVI2CRD=<num>,<address>,<length>\r\n" \
    "example:AT+DRVI2CRD=0,0x52,1\r\n"

#define WM_ATCMD_I2C_WRDATA_USAGE                  \
    "AT+DRVI2CWRDATA=<num>,<address>,<length>\r\n" \
    "example:AT+DRVI2CWRDATA=0,0x52,1\r\n"

#define WM_ATCMD_I2C_WRDBYTES_USAGE                        \
    "AT+DRVI2CWRBYTES=<num>,<address>,<length>,<data>\r\n" \
    "example:AT+DRVI2CWRBYTES=0,0x52,1,0x01\r\n"

int wm_atcmd_i2c_init(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_i2c_rd(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_i2c_wrdata(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_i2c_wrbytes(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+DRVI2CINIT", wm_atcmd_i2c_init, WM_ATCMD_I2C_INIT_USAGE);
WM_ATCMD_DEFINE("AT+DRVI2CRD", wm_atcmd_i2c_rd, WM_ATCMD_I2C_RD_USAGE);
WM_ATCMD_DEFINE("AT+DRVI2CWRDATA", wm_atcmd_i2c_wrdata, WM_ATCMD_I2C_WRDATA_USAGE);
WM_ATCMD_DEFINE("AT+DRVI2CWRBYTES", wm_atcmd_i2c_wrbytes, WM_ATCMD_I2C_WRDBYTES_USAGE);

#endif //CONFIG_WM_ATCMD_I2C_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_I2C_H__ */
