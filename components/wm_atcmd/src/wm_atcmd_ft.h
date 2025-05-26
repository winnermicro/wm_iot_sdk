/**
 * @file wm_atcmd_ft.h
 *
 * @brief FT AT Command
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

#ifndef __WM_ATCMD_FT_H__
#define __WM_ATCMD_FT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_FT_ENABLE

int wm_atcmd_ft_plus(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_e(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_z(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_qver(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_qmac(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_mac(wm_at_type_e type, int argc, char **argv);

int wm_atcmd_ft_regr(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_regw(wm_at_type_e type, int argc, char **argv);

int wm_atcmd_ft_rfr(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_rfw(wm_at_type_e type, int argc, char **argv);

int wm_atcmd_ft_lpchl(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_lptstr(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_lptstp(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_lprstr(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_lprstp(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_lprstt(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_lptpd(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_txg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_txgi(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_width(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_freq(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_txiq(wm_at_type_e type, int argc, char **argv);

int wm_atcmd_ft_wbgr(wm_at_type_e type, int argc, char **argv);

int wm_atcmd_ft_wscan(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_wjoin(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_wleav(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_ssid(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_key(wm_at_type_e type, int argc, char **argv);

int wm_atcmd_ft_bten(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_bttest(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_btdes(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ft_bletps(wm_at_type_e type, int argc, char **argv);

int wm_atcmd_ft_iotest(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+", wm_atcmd_ft_plus, NULL);
WM_ATCMD_DEFINE("AT+E", wm_atcmd_ft_e, NULL);
WM_ATCMD_DEFINE("AT+Z", wm_atcmd_ft_z, NULL);
WM_ATCMD_DEFINE("AT+QVER", wm_atcmd_ft_qver, NULL);
WM_ATCMD_DEFINE("AT+QMAC", wm_atcmd_ft_qmac, NULL);
WM_ATCMD_DEFINE("AT+&MAC", wm_atcmd_ft_mac, NULL);

WM_ATCMD_DEFINE("AT+&REGR", wm_atcmd_ft_regr, NULL);
WM_ATCMD_DEFINE("AT+&REGW", wm_atcmd_ft_regw, NULL);

WM_ATCMD_DEFINE("AT+&RFR", wm_atcmd_ft_rfr, NULL);
WM_ATCMD_DEFINE("AT+&RFW", wm_atcmd_ft_rfw, NULL);

WM_ATCMD_DEFINE("AT+&LPCHL", wm_atcmd_ft_lpchl, NULL);
WM_ATCMD_DEFINE("AT+&LPTSTR", wm_atcmd_ft_lptstr, NULL);
WM_ATCMD_DEFINE("AT+&LPTSTP", wm_atcmd_ft_lptstp, NULL);
WM_ATCMD_DEFINE("AT+&LPRSTR", wm_atcmd_ft_lprstr, NULL);
WM_ATCMD_DEFINE("AT+&LPRSTP", wm_atcmd_ft_lprstp, NULL);
WM_ATCMD_DEFINE("AT+&LPRSTT", wm_atcmd_ft_lprstt, NULL);
WM_ATCMD_DEFINE("AT+&LPTPD", wm_atcmd_ft_lptpd, NULL);
WM_ATCMD_DEFINE("AT+&TXG", wm_atcmd_ft_txg, NULL);
WM_ATCMD_DEFINE("AT+&TXGI", wm_atcmd_ft_txgi, NULL);
WM_ATCMD_DEFINE("AT+WIDTH", wm_atcmd_ft_width, NULL);
WM_ATCMD_DEFINE("AT+FREQ", wm_atcmd_ft_freq, NULL);
WM_ATCMD_DEFINE("AT+TXIQ", wm_atcmd_ft_txiq, NULL);

WM_ATCMD_DEFINE("AT+WBGR", wm_atcmd_ft_wbgr, NULL);

WM_ATCMD_DEFINE("AT+WSCAN", wm_atcmd_ft_wscan, NULL);
WM_ATCMD_DEFINE("AT+WJOIN", wm_atcmd_ft_wjoin, NULL);
WM_ATCMD_DEFINE("AT+WLEAV", wm_atcmd_ft_wleav, NULL);
WM_ATCMD_DEFINE("AT+SSID", wm_atcmd_ft_ssid, NULL);
WM_ATCMD_DEFINE("AT+KEY", wm_atcmd_ft_key, NULL);

WM_ATCMD_DEFINE("AT+BTEN", wm_atcmd_ft_bten, NULL);
WM_ATCMD_DEFINE("AT+BTTEST", wm_atcmd_ft_bttest, NULL);
WM_ATCMD_DEFINE("AT+BTDES", wm_atcmd_ft_btdes, NULL);
WM_ATCMD_DEFINE("AT+BLETPS", wm_atcmd_ft_bletps, NULL);

WM_ATCMD_DEFINE("AT+IOTEST", wm_atcmd_ft_iotest, NULL);

#endif //CONFIG_WM_ATCMD_FT_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_FT_H__ */
