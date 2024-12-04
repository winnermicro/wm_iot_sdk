/**
 * @file wm_gpio_map.c
 *
 * @brief Gpio Map Module
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

#include "wm_soc_cfgs.h"
#include "wm_regs_base.h"

const wm_gpio_map_t g_gpio_map[WM_GPIO_MAX_NUM_BY_ALL_SOC] = {
    {
#if CONFIG_BUILD_TYPE_W800 || CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W803 || \
    CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_0*/
#else
        NULL
#endif
    },
    { (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR }, /*WM_GPIO_NUM_1*/
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_2*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_3*/
#else
        NULL
#endif
    },
    { (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR }, /*WM_GPIO_NUM_4*/
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_5*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_6*/
#else
        NULL
#endif
    },
    { (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR }, /*WM_GPIO_NUM_7*/
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_8*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_9*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_10*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_11*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_12*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_13*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_14*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOA_BASE_ADDR /*WM_GPIO_NUM_15*/
#else
        NULL
#endif
    },
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_16*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_17*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_18*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_19*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_20*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_21*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_22*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_23*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_24*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_25*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_26*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_27*/
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_28*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_29*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_30*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_31*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_32*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_33*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_34*/
#else
        NULL
#endif
    },
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_35*/
    { (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR }, /*WM_GPIO_NUM_36*/
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_37*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_38*/
#else
        NULL
#endif
    },
    {
        NULL /*WM_GPIO_NUM_39*/
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_40*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_41*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_42*/
#else
        NULL
#endif
    },
    {
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_43*/
#else
        NULL
#endif
    },
    {
        NULL /*WM_GPIO_NUM_44*/
    },
    {
#if CONFIG_BUILD_TYPE_W802
        (wm_gpio_reg_t *)WM_GPIOB_BASE_ADDR /*WM_GPIO_NUM_45*/
#else
        NULL
#endif
    },
    {
        NULL /*WM_GPIO_NUM_46*/
    },
    {
        NULL /*WM_GPIO_NUM_47*/
    },
};

const wm_io_mux_attr_t g_iomux_attr_table[WM_IOMUX_ATTR_NUM] = {
    { WM_GPIO_NUM_1,  WM_GPIO_IOMUX_FUN2, WM_GPIO_DIR_OUTPUT, WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO1_FUNC_I2C_SCL
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
    { WM_GPIO_NUM_3,  WM_GPIO_IOMUX_FUN2, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO3_FUNC_UART2_RX
#endif
    { WM_GPIO_NUM_4,  WM_GPIO_IOMUX_FUN2, WM_GPIO_DIR_OUTPUT, WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO4_FUNC_I2C_SDA
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
    { WM_GPIO_NUM_6,  WM_GPIO_IOMUX_FUN1, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO6_FUNC_UART3_RX
#endif
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
    { WM_GPIO_NUM_9,  WM_GPIO_IOMUX_FUN2, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO9_FUNC_UART4_RX
    { WM_GPIO_NUM_9,  WM_GPIO_IOMUX_FUN3, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO9_FUNC_UART5_RX
#endif
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W806
    { WM_GPIO_NUM_13, WM_GPIO_IOMUX_FUN2, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO13_FUNC_UART5_RX
#endif
    { WM_GPIO_NUM_17, WM_GPIO_IOMUX_FUN3, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO17_FUNC_UART3_RX
    { WM_GPIO_NUM_19, WM_GPIO_IOMUX_FUN3, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO19_FUNC_UART2_RX
    { WM_GPIO_NUM_21, WM_GPIO_IOMUX_FUN3, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO21_FUNC_UART4_RX
    { WM_GPIO_NUM_22, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO22_FUNC_SDIO_CK
    { WM_GPIO_NUM_23, WM_GPIO_IOMUX_FUN1, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO23_FUNC_UART1_RX
    { WM_GPIO_NUM_23, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO23_FUNC_SDIO_CMD
    { WM_GPIO_NUM_24, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO24_FUNC_SDIO_D0
    { WM_GPIO_NUM_25, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO25_FUNC_SDIO_D1
    { WM_GPIO_NUM_26, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO26_FUNC_SDIO_D2
    { WM_GPIO_NUM_27, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO27_FUNC_SDIO_D3
#if CONFIG_BUILD_TYPE_W801S || CONFIG_BUILD_TYPE_W802 || CONFIG_BUILD_TYPE_W806
    { WM_GPIO_NUM_32, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO32_FUNC_UART1_RX
    { WM_GPIO_NUM_33, WM_GPIO_IOMUX_FUN1, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO33_FUNC_UART5_RX
#endif
    { WM_GPIO_NUM_35, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO35_FUNC_I2C_SDA
    { WM_GPIO_NUM_36, WM_GPIO_IOMUX_FUN1, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO36_FUNC_UART0_RX
    { WM_GPIO_NUM_36, WM_GPIO_IOMUX_FUN4, WM_GPIO_DIR_INPUT,  WM_GPIO_PULL_UP }, // WM_IO_MUX_GPIO36_FUNC_I2C_SCL
};
