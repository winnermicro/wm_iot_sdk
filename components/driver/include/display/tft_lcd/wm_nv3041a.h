/**
 * @file wm_nv3041a.h
 *
 * @brief LCD TFT NV3041A Module
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

#ifndef __WM_DRV_NV3041A_H__
#define __WM_DRV_NV3041A_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LCD command list which is strictly matchs with LCD's datasheet */
/* Algorithm */
#define NV3041_LCD_CMD_NOP          0x00 /**< Empty Command, It does not have any effect on the NV3041A */
#define NV3041_LCD_CMD_RD_SYSID     0x04 /**< Read 24-bit System ID which the lcd driver version defined by display supplier */
#define NV3041_LCD_CMD_RD_STATE     0x09 /**< Read display state (Booster Vol,Row/Col Addr Order,Row/Col exchange, rgb order) */
#define NV3041_LCD_CMD_RD_DISP      0x0A /**< Read power mode (Booster Vol,idle, partial,sleep, display on/off...) */
#define NV3041_LCD_CMD_RD_MADCTL    0x0B /**< Read MADCTL (page/ addr order, page/col order, rgb/bgr order...) */
#define NV3041_LCD_CMD_RD_IM        0x0D /**< Read image mode (like scolling on/off, inversion on/off) */
#define NV3041_LCD_CMD_RD_SM        0x0E /**< Read signal mode (like tearing effect line on/off,  mode.) */

/* Display Control */
#define NV3041_LCD_CMD_SLPIN        0x10 /**< Enter sleep in (DC,oscillator,pannel scanning is stopped, memory keeps) */
#define NV3041_LCD_CMD_SLPOUT       0x11 /**< Turns off sleep mode */
#define NV3041_LCD_CMD_PTLON        0x12 /**< Turns on partial mode, which described by teh partial area command (0x30) */
#define NV3041_LCD_CMD_NORMAL       0x13 /**< Exit partial and scrolling display mode */
#define NV3041_LCD_CMD_WR_SYSID     0x14 /**< Write 24-bibt System ID */

/* Function Control*/
#define NV3041_LCD_CMD_INVOFF       0x20 /**< Recover from display inversion mode */
#define NV3041_LCD_CMD_INVON        0x21 /**< Eneter into display inversion mode */
#define NV3041_LCD_CMD_DISPOFF      0x28 /**< Enter into display off mode (disable frame buffer output, and insert blank page) */
#define NV3041_LCD_CMD_DISPON       0x29 /**< Recover from display off mode (enable frame buffer output) */
#define NV3041_LCD_CMD_COL_ADR      0x2A /**< Set column address (column access start point, end point) */
#define NV3041_LCD_CMD_ROW_ADR      0x2B /**< Set row address (row access start point, end point) */
#define NV3041_LCD_CMD_RAMRW        0x2C /**< Enable GRAM Read/Write */

#define NV3041_LCD_CMD_PTL_ADR      0x30 /**< Define the partial mode's display area (PSL:start row, PEL:end row) */
#define NV3041_LCD_CMD_SCROLL_ADR   0x33 /**< Define the vertical scrolling area (top fixed area, verical scrolling area) */
#define NV3041_LCD_CMD_TEOFF        0x34 /**< Turns off tearing effect output signel from TE signal line */
#define NV3041_LCD_CMD_TEON         0x35 /**< Turns on tearing effect output signel */
#define NV3041_LCD_CMD_MADCTL       0x36 /**< Define read/write scanning direction of frame memory */
#define NV3041_LCD_CMD_VSCSAD       0x37 /**< Define Vertical scroll start address (used together with cmd 0x33) */
#define NV3041_LCD_CMD_IDMOFF       0x38 /**< Recover from IDLE mode on */
#define NV3041_LCD_CMD_IDMON        0x39 /**< Enter into IDLE mode on (8 color depth is displayed) */
#define NV3041_LCD_CMD_COLMOD       0x3A /**< Defines the format of RGB picture data (rgb 666,565) */

/* Porch Setting */
#define NV3041_LCD_CMD_MACTL_USR    0x40 /**< Make “XOR” logic with MADCTR(0x36),in case of default 36H val are not expectation */
#define NV3041_LCD_CMD_BUS_WD       0x41 /**< Define the bus width */
#define NV3041_LCD_CMD_QSPI_DCTL    0x43 /**< Define the QSPI interface configuration (dummy, bgr ...) */
#define NV3041_LCD_CMD_FSM_VBP      0x44 /**< FSM VBP */
#define NV3041_LCD_CMD_FSM_VFP      0x45 /**< FSM VFP */
#define NV3041_LCD_CMD_FSM_HBP_ODD  0x46 /**< FSM HBP ODD  */
#define NV3041_LCD_CMD_FSM_HFP_ODD  0x47 /**< FSM HFP ODD  */
#define NV3041_LCD_CMD_FSM_HBP_EVEN 0x48 /**< FSM HBP EVEN  */
#define NV3041_LCD_CMD_FSM_HFP_EVEN 0x49 /**< FSM HFP EVEN  */
#define NV3041_LCD_CMD_SCAN_VRES    0x4A /**< SCAN VRES  */
#define NV3041_LCD_CMD_SCAN_HRES    0x4B /**< SCAN HRES  */
#define NV3041_LCD_CMD_RGB_HBP_ODD  0x4C /**< RGB HBP ODD  */
#define NV3041_LCD_CMD_RGB_HFP_ODD  0x4D /**< RGB HFP ODD  */
#define NV3041_LCD_CMD_RGB_HBP_EVEN 0x4E /**< RGB HBP EVEN  */
#define NV3041_LCD_CMD_RGB_HFP_EVEN 0x4F /**< RGB HFP EVEN  */

//TODO: more commands
/* Gate/Source Setting */
/* Power Trimming */
/* Gamma P Selection */
/* Gamma N Selection */
/* Pump Control */
/* 0xE1~0xE8 (RGB Control, INTF, DVDD, ESD, TE, PCLK)*/
/* OTP Control Setting*/

/* ROTATION Register Value */
#define NV3041A_CFG_MADCTL_MY       0x80
#define NV3041A_CFG_MADCTL_MX       0x40
#define NV3041A_CFG_MADCTL_MV       0x20
#define NV3041A_CFG_MADCTL_ML       0x10
#define NV3041A_CFG_MADCTL_RGB      0x00

/* The command table definiation, which using TLV format 
   the command header is defined by LCD_CMD_SEQ_HEADER_LEN
   len = cmd content + data content */
const uint8_t nv3041a_init_seq[] = {
    /*  len , delay(ms), cmd_type,  cmd, data ... */
    //0x03, 0x00, LCD_CMD_TYPE_16BIT, 0x12, 0x34, 0xBE, // Example:16bit command "0x3412" be used
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xFF, 0xA5, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xE7, 0x10, // TE_output_en
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x35, 0x01, // TE_interface_en 01
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x36, 0x00, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x3A, 0x01, // color mode, 01:RGB565，00:RGB666
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x40, 0x01, // 01:IPS/00:TN
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x44, 0x15, // VBP 21
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x45, 0x15, // VFP 21
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x7D, 0x03, // vdds_trim[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC1, 0xBB, // avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0] 0xbb 88 a2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC2, 0x05, // vgl_clp_en vgl_clp[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC3, 0x10, // vgl_clp_en vgl_clp[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC6, 0x3E, // avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0] 35
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC7, 0x25, // mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0] 2e
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC8, 0x11, // VGL_CLK_sel
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x7A, 0x5F, // user_vgsp  4f:0.8V 3f:1.04V 5f
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x6F, 0x44, // user_gvdd  1C:5.61 5f 53 2a 3a
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x78, 0x70, // user_gvcl  50:-3.22 75 58 66
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC9, 0x00, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x67, 0x21, //

    // gate_ed
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x51, 0x0A, // gate_st_o[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x52, 0x76, // gate_ed_o[7:0] 76
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x53, 0x0A, // gate_st_e[7:0] 76
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x54, 0x76, // gate_ed_e[7:0]
    // sorce
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x46, 0x0A, // fsm_hbp_o[5:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x47, 0x2A, // fsm_hfp_o[5:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x48, 0x0A, // fsm_hbp_e[5:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x49, 0x1A, // fsm_hfp_e[5:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x56, 0x43, // src_ld_wd[1:0] src_ld_st[5:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x57, 0x42, // pn_cs_en src_cs_st[5:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x58, 0x3C, // src_cs_p_wd[6:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x59, 0x64, // src_cs_n_wd[6:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x5A, 0x41, // src_pchg_st_o[6:0] 41
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x5B, 0x3C, // src_pchg_wd_o[6:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x5C, 0x02, // src_pchg_st_e[6:0] 02
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x5D, 0x3C, // src_pchg_wd_e[6:0] 3c
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x5E, 0x1F, // src_pol_sw[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x60, 0x80, // src_op_st_o[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x61, 0x3F, // src_op_st_e[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x62, 0x21, // src_op_ed_o[9:8] src_op_ed_e[9:8]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x63, 0x07, // src_op_ed_o[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x64, 0xE0, // src_op_ed_e[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x65, 0x02, // chopper
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xCA, 0x20, // avdd_mux_st_o[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xCB, 0x52, // avdd_mux_ed_o[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xCC, 0x10, // avdd_mux_st_e[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xCD, 0x42, // avdd_mux_ed_e[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xD0, 0x20, // avcl_mux_st_o[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xD1, 0x52, // avcl_mux_ed_o[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xD2, 0x10, // avcl_mux_st_e[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xD3, 0x42, // avcl_mux_ed_e[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xD4, 0x0A, // vgh_mux_st[7:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xD5, 0x32, // vgh_mux_ed[7:0]

    // test mode
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xF8, 0x03, //-
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xF9, 0x20, //-

    // gammma  weihuan pianguangpian 0913
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x80, 0x00, // am_vrp0
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA0, 0x00, // gam_VRN0
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x81, 0x05, // gam_vrp1
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA1, 0x05, // gam_VRN1
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x82, 0x04, // gam_vrp2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA2, 0x03, // gam_VRN2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x86, 0x25, // gam_prp0
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA6, 0x1C, // gam_PRN0
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x87, 0x2A, // gam_prp1
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA7, 0x2A, // gam_PRN1
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x83, 0x1D, // gam_vrp3
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA3, 0x1D, // gam_VRN3
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x84, 0x1E, // gam_vrp4
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA4, 0x1E, // gam_VRN4
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x85, 0x3F, // gam_vrp5
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA5, 0x3F, // gam_VRN5
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x88, 0x0B, // gam_pkp0
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA8, 0x0B, // gam_PKN0
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x89, 0x14, // gam_pkp1
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xA9, 0x13, // gam_PKN1
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8A, 0x1A, // gam_pkp2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xAA, 0x1A, // gam_PKN2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8B, 0x0A, // gam_PKP3
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xAB, 0x0A, // gam_PKN3
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8C, 0x1C, // gam_PKP4
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xAC, 0x0C, // gam_PKN4
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8D, 0x1F, // gam_PKP5
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xAD, 0x0B, // gam_PKN5
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8E, 0x1F, // gam_PKP6
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xAE, 0x0A, // gam_PKN6
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8F, 0x1F, // gam_PKP7
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xAF, 0x07, // gam_PKN7
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x90, 0x06, // gam_PKP8
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xB0, 0x06, // gam_PKN8
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x91, 0x0D, // gam_PKP9
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xB1, 0x0D, // gam_PKN9
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x92, 0x17, // gam_PKP10
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xB2, 0x17, // gam_PKN10
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xFF, 0x00, //
    0x01, 0x74, LCD_CMD_TYPE_8BIT, 0x11,       //
    0x01, 0x74, LCD_CMD_TYPE_8BIT, 0x29,       //

    0, // END bytes
};

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_NV3041A_H__ */
