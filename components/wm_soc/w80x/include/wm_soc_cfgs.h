/**
 * @file wm_soc_cfgs.h
 *
 * @brief Storage for differentiated hardware configurations specific to
 * a particular SoC (System on Chip).
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

#ifndef __SOC_CFGS_H__
#define __SOC_CFGS_H__

#include "wmsdk_config.h"
#include "wm_reg_op.h"

#if CONFIG_BUILD_TYPE_W800
#include "wm_w800_cfgs.h"
#elif CONFIG_BUILD_TYPE_W801S
#include "wm_w801s_cfgs.h"
#elif CONFIG_BUILD_TYPE_W802
#include "wm_w802_cfgs.h"
#elif CONFIG_BUILD_TYPE_W803
#include "wm_w803_cfgs.h"
#elif CONFIG_BUILD_TYPE_W805
#include "wm_w805_cfgs.h"
#elif CONFIG_BUILD_TYPE_W806
#include "wm_w806_cfgs.h"
#else
#error "The configuration of SOC was not found"
#endif
/**
 * @defgroup WM_SOC_CFG_TYPEs WM SOC Types
 * @brief WinnerMicro WM_SOC_CFG_TYPEs
 */

/**
 * @addtogroup WM_SOC_CFG_TYPEs
 * @{
 */

/******************************************************************/
/*                  RCC HW Capability   (Start)                   */
/******************************************************************/
#define WM_PLL_CLOCK           480
#define UNIT_MHZ               1000000

#define WM_APB_CLOCK           (40000000) /* 40MHz */

/* Store reboot reason by RAM's Last Word */
#define WM_REBOOT_PARA_ADDRESS (0x20047EFC)

#define WM_RCC_I2C_GATE_EN     WM_BIT(0)
#define WM_RCC_UART0_GATE_EN   WM_BIT(1)
#define WM_RCC_UART1_GATE_EN   WM_BIT(2)
#define WM_RCC_UART2_GATE_EN   WM_BIT(3)
#define WM_RCC_UART3_GATE_EN   WM_BIT(4)
#define WM_RCC_UART4_GATE_EN   WM_BIT(5)
#define WM_RCC_UART5_GATE_EN   WM_BIT(6)
#define WM_RCC_LS_SPI_GATE_EN  WM_BIT(7)
#define WM_RCC_DMA_GATE_EN     WM_BIT(8)
#define WM_RCC_RF_CFG_GATE_EN  WM_BIT(9)
#define WM_RCC_TIMER_GATE_EN   WM_BIT(10)
#define WM_RCC_GPIO_GATE_EN    WM_BIT(11)
#define WM_RCC_SD_ADC_GATE_EN  WM_BIT(12)
#define WM_RCC_PWM_GATE_EN     WM_BIT(13)
#define WM_RCC_LCD_GATE_EN     WM_BIT(14)
#define WM_RCC_I2S_GATE_EN     WM_BIT(15)
#define WM_RCC_RSA_GATE_EN     WM_BIT(16)
#define WM_RCC_GPSEC_GATE_EN   WM_BIT(17)
#define WM_RCC_SDIOM_GATE_EN   WM_BIT(18)
#define WM_RCC_QSRAM_GATE_EN   WM_BIT(19)
#define WM_RCC_BT_GATE_EN      WM_BIT(20)
#define WM_RCC_TOUCH_GATE_EN   WM_BIT(21)
#define WM_RCC_CLOCK_ALL       0x3fffff

#define WM_RCC_BBP_RST         WM_BIT(0)
#define WM_RCC_MAC_RST         WM_BIT(1)
#define WM_RCC_SEC_RST         WM_BIT(2)
#define WM_RCC_SDIO_AHB_RST    WM_BIT(3)
#define WM_RCC_DMA_RST         WM_BIT(4)
#define WM_RCC_MEM_MNG_RST     WM_BIT(5)
#define WM_RCC_APB_RST         WM_BIT(6)
#define WM_RCC_BUS1_RST        WM_BIT(7)
#define WM_RCC_BUS2_RST        WM_BIT(8)
#define WM_RCC_I2C_RST         WM_BIT(9)
#define WM_RCC_UART0_RST       WM_BIT(10)
#define WM_RCC_UART1_RST       WM_BIT(11)
#define WM_RCC_UART2_RST       WM_BIT(12)
#define WM_RCC_UART3_RST       WM_BIT(13)
#define WM_RCC_UART4_RST       WM_BIT(14)
#define WM_RCC_UART5_RST       WM_BIT(15)
#define WM_RCC_SPIM_RST        WM_BIT(16)
#define WM_RCC_SPIS_RST        WM_BIT(17)
#define WM_RCC_RF_CFG_RST      WM_BIT(18)
#define WM_RCC_GPIO_RST        WM_BIT(19)
#define WM_RCC_TIMER_RST       WM_BIT(20)
#define WM_RCC_SAR_ADC_RST     WM_BIT(21)
#define WM_RCC_PWM_RST         WM_BIT(22)
#define WM_RCC_LCD_RST         WM_BIT(23)
#define WM_RCC_I2S_RST         WM_BIT(24)
#define WM_RCC_RSA_RST         WM_BIT(25)
#define WM_RCC_GPSEC_RST       WM_BIT(26)
#define WM_RCC_SDIOM_RST       WM_BIT(27)
#define WM_RCC_QSPI_RAM_RST    WM_BIT(28)
#define WM_RCC_BT_RST          WM_BIT(29)
#define WM_RCC_FLASH_RST       WM_BIT(30)
#define WM_RCC_TOUCH_RST       WM_BIT(31)

typedef enum {
    WM_RCC_TYPE_PERIPHERAL = 0,
    WM_RCC_TYPE_WLAN,
    WM_RCC_TYPE_CPU,
    WM_RCC_TYPE_SD_ADC,
    WM_RCC_TYPE_QFLASH,
    WM_RCC_TYPE_GPSEC,
    WM_RCC_TYPE_RSA,
    WM_RCC_TYPE_APB,
    WM_RCC_TYPE_MAX,
} wm_rcc_type_t;
/******************************************************************/
/*                  RCC HW Capability   (End)                     */
/******************************************************************/

/******************************************************************/
/*                  PMU HW Capability   (Start)                   */
/******************************************************************/

typedef enum {
    WM_PMU_CLOCK_SRC_32K = 0, /**< Using internal 32K crystal oscillator  */
    WM_PMU_CLOCK_SRC_40M_DIV, /**< Using external 40M crystal oscillator by frequency division */
} wm_pmu_clock_source_t;

#define WM_PMU_TIME0_MIN_TIME (1 * 1000)     /**< PMU timer0 min sleep time (millisecond) */
#define WM_PMU_TIME0_MAX_TIME (65535 * 1000) /**< PMU timer0 max sleep time (millisecond) */
#define WM_PMU_TIME1_MIN_TIME (1)            /**< PMU timer1 min sleep time (millisecond) */
#define WM_PMU_TIME1_MAX_TIME (65535)        /**< PMU timer1 max sleep time (millisecond) */

/******************************************************************/
/*                  PMU HW Capability   (End)                     */
/******************************************************************/

/******************************************************************/
/*                  Timer HW Capability   (Start)                 */
/******************************************************************/

typedef enum {
    WM_TIMER_ID_0 = 0,
    WM_TIMER_ID_1,
    WM_TIMER_ID_2,
    WM_TIMER_ID_3,
    WM_TIMER_ID_4,
    WM_TIMER_ID_5,
    WM_TIMER_ID_MAX
} wm_timer_id_t;

/******************************************************************/
/*                  Timer HW Capability   (End)                   */
/******************************************************************/

/******************************************************************/
/*                  DMA HW Capability   (Start)                   */
/******************************************************************/
/// dma channel 7 transfer done
#define WM_DMA_CH7_XFER_DONE      ((uint32_t)0x00008000)
/// dma channel 7 transfer done pos
#define WM_DMA_CH7_XFER_DONE_POS  (15)
/// dma channel 7 burst done
#define WM_DMA_CH7_BURST_DONE     ((uint32_t)0x00004000)
/// dma channel 7 burst done pos
#define WM_DMA_CH7_BURST_DONE_POS (14)
/// dma channel 6 transfer done
#define WM_DMA_CH6_XFER_DONE      ((uint32_t)0x00002000)
/// dma channel 6 transfer done pos
#define WM_DMA_CH6_XFER_DONE_POS  (13)
/// dma channel 6 burst done
#define WM_DMA_CH6_BURST_DONE     ((uint32_t)0x00001000)
/// dma channel 6 burst done pos
#define WM_DMA_CH6_BURST_DONE_POS (12)
/// dma channel 5 transfer done
#define WM_DMA_CH5_XFER_DONE      ((uint32_t)0x00000800)
/// dma channel 5 transfer done pos
#define WM_DMA_CH5_XFER_DONE_POS  (11)
/// dma channel 5 burst done
#define WM_DMA_CH5_BURST_DONE     ((uint32_t)0x00000400)
/// dma channel 5 burst done pos
#define WM_DMA_CH5_BURST_DONE_POS (10)
/// dma channel 4 transfer done
#define WM_DMA_CH4_XFER_DONE      ((uint32_t)0x00000200)
/// dma channel 4 transfer done pos
#define WM_DMA_CH4_XFER_DONE_POS  (9)
/// dma channel 4 burst done
#define WM_DMA_CH4_BURST_DONE     ((uint32_t)0x00000100)
/// dma channel 4 burst done pos
#define WM_DMA_CH4_BURST_DONE_POS (8)
/// dma channel 3 transfer done
#define WM_DMA_CH3_XFER_DONE      ((uint32_t)0x00000080)
/// dma channel 3 transfer done pos
#define WM_DMA_CH3_XFER_DONE_POS  (7)
/// dma channel 3 burst done
#define WM_DMA_CH3_BURST_DONE     ((uint32_t)0x00000040)
/// dma channel 3 burst done pos
#define WM_DMA_CH3_BURST_DONE_POS (6)
/// dma channel 2 transfer done
#define WM_DMA_CH2_XFER_DONE      ((uint32_t)0x00000020)
/// dma channel 2 transfer done pos
#define WM_DMA_CH2_XFER_DONE_POS  (5)
/// dma channel 2 burst done
#define WM_DMA_CH2_BURST_DONE     ((uint32_t)0x00000010)
/// dma channel 2 burst done pos
#define WM_DMA_CH2_BURST_DONE_POS (4)
/// dma channel 1 transfer done
#define WM_DMA_CH1_XFER_DONE      ((uint32_t)0x00000008)
/// dma channel 1 transfer done pos
#define WM_DMA_CH1_XFER_DONE_POS  (3)
/// dma channel 1 burst done
#define WM_DMA_CH1_BURST_DONE     ((uint32_t)0x00000004)
/// dma channel 1 burst done pos
#define WM_DMA_CH1_BURST_DONE_POS (2)
/// dma channel 0 transfer done
#define WM_DMA_CH0_XFER_DONE      ((uint32_t)0x00000002)
/// dma channel 0 transfer done pos
#define WM_DMA_CH0_XFER_DONE_POS  (1)
/// dma channel 0 burst done
#define WM_DMA_CH0_BURST_DONE     ((uint32_t)0x00000001)
/// dma channel 0 burst done pos
#define WM_DMA_CH0_BURST_DONE_POS (0)
/// dma all ch interrupt mask
#define WM_DMA_CH07_INT_MAP                                                                                                \
    (WM_DMA_CH0_BURST_DONE | WM_DMA_CH0_XFER_DONE | WM_DMA_CH1_BURST_DONE | WM_DMA_CH1_XFER_DONE | WM_DMA_CH2_BURST_DONE | \
     WM_DMA_CH2_XFER_DONE | WM_DMA_CH3_BURST_DONE | WM_DMA_CH3_XFER_DONE | WM_DMA_CH4_BURST_DONE | WM_DMA_CH4_XFER_DONE |  \
     WM_DMA_CH5_BURST_DONE | WM_DMA_CH5_XFER_DONE | WM_DMA_CH6_BURST_DONE | WM_DMA_CH6_XFER_DONE | WM_DMA_CH7_BURST_DONE | \
     WM_DMA_CH7_XFER_DONE)
/// dma desc list valid bit
#define WM_DMA_LIST_VLD (0x1f)
/**
 * @brief W800 DMA channel enum
 */
typedef enum wm_dma_ch_e {
    WM_DMA_CH_0 = 0x0,
    WM_DMA_CH_1,
    WM_DMA_CH_2,
    WM_DMA_CH_3,
    WM_DMA_CH_4,
    WM_DMA_CH_5,
    WM_DMA_CH_6,
    WM_DMA_CH_7,
    WM_DMA_CH_MAX,
} wm_dma_ch_t;

/**
 * @brief dma ch interrupt id
 */
typedef enum wm_dma_ch_irq_e {
    WM_DMA_CH_IRQ_0 = WM_DMA_CH_0,
    WM_DMA_CH_IRQ_1,
    WM_DMA_CH_IRQ_2,
    WM_DMA_CH_IRQ_3,
    WM_DMA_CH_IRQ_4_7,
    WM_DMA_CH_IRQ_MAX
} wm_dma_ch_irq_t;

/**
 * @brief dma uart dma channel
 */
typedef enum wm_dma_ch_uart_sel_e {
    WM_DMA_CH_UART0_SEL = 0x0,
    WM_DMA_CH_UART1_SEL,
    WM_DMA_CH_UART2_7816_SEL,
    WM_DMA_CH_UART3_SEL,
    WM_DMA_CH_UART4_SEL,
    WM_DMA_CH_UART5_SEL,
    WM_DMA_CH_UART_SEL_MAX
} wm_dma_ch_uart_sel_t;

/**
 * @brief dma request source
 */
typedef enum wm_dma_req_sel_e {
    WM_DMA_UART_RX_REQ = 0x0,
    WM_DMA_UART_TX_REQ,
    WM_DMA_PWM_CAP0_REQ,
    WM_DMA_PWM_CAP1_REQ,
    WM_DMA_LSPI_RX_REQ,
    WM_DMA_LSPI_TX_REQ,
    WM_DMA_ADC_CH0_REQ,
    WM_DMA_ADC_CH1_REQ,
    WM_DMA_ADC_CH2_REQ,
    WM_DMA_ADC_CH3_REQ,
    WM_DMA_I2S_RX_REQ,
    WM_DMA_I2S_TX_REQ,
    WM_DMA_SDIOH_REQ,
    WM_DMA_REQ_MAX
} wm_dma_req_sel_t;

/**
 * @brief dma ch interrupt type
 */
typedef enum wm_dma_int_type_e {
    /* burst done */
    WM_DMA_INT_BURST_DONE = 0x0,
    /* transfer done  */
    WM_DMA_INT_XFER_DONE,
    WM_DMA_INT_MAX
} wm_dma_int_type_t;

/**
 * @brief dma mode
 */
typedef enum wm_dma_op_mode_e {
    /* m2m */
    WM_DMA_SW_MODE = 0x0,
    /* m2p or p2m */
    WM_DMA_HW_MODE,
    WM_DMA_MODE_MAX
} wm_dma_op_mode_t;

/**
 * @brief dma chain mode
 */
typedef enum wm_dma_chain_mode_e {
    WM_DMA_CHAIN_MODE_NORMAL = 0x0,
    WM_DMA_CHAIN_MODE_LIST,
    WM_DMA_CHAIN_MODE_MAX
} wm_dma_chain_mode_t;

/**
 * @brief dma auto reload control
 */
typedef enum wm_dma_reload_e {
    WM_DMA_RELOAD_DISABLE = 0x0,
    WM_DMA_RELOAD_ENABLE,
    WM_DMA_RELOAD_MAX
} wm_dma_reload_t;

/**
 * @brief dma ch interrupt control
 */
typedef enum wm_dma_ch_en_ctrl_e {
    WM_DMA_CH_EN_CTRL_DISABLE = 0x0,
    WM_DMA_CH_EN_CTRL_ENABLE,
    WM_DMA_CH_EN_CTRL_MAX
} wm_dmach_en_ctrl_t;

/**
 * @brief dma warp mode control
 */
typedef enum wm_dma_warp_mode_ctrl_e {
    WM_DMA_WARP_CTRL_DISABLE = 0x0,
    WM_DMA_WARP_CTRL_ENABLE,
    WM_DMA_WARP_CTRL_MAX
} wm_dma_warp_mode_ctrl_t;

/**
 * @brief dma chain mode control
 */
typedef enum wm_dma_chain_mode_ctrl_e {
    WM_DMA_CHAIN_MODE_DISABLE = 0x0,
    WM_DMA_CHAIN_MODE_ENABLE,
    WM_DMA_CHAIN_MODE_CTRL_MAX
} wm_dma_chain_mode_ctrl_t;

/**
 * @brief dma addr inc mode
 */
typedef enum wm_dma_addr_mode_e {
    WM_DMA_ADDR_FIXED = 0x0,
    WM_DMA_ADDR_INC,
    WM_DMA_ADDR_RSVD,
    WM_DMA_ADDR_WARP,
    WM_DMA_ADDR_MAX
} wm_dma_addr_mode_t;

/**
 * @brief dma data xfer uint
 */
typedef enum wm_dma_xfer_uint_e {
    WM_DMA_XFER_UNIT_BYTE = 0x0,
    WM_DMA_XFER_UNIT_HALF_WORD,
    WM_DMA_XFER_UNIT_WORD,
    WM_DMA_XFER_UINT_MAX
} wm_dma_xfer_uint_t;

/**
 * @brief dma data burst uint
 */
typedef enum wm_dma_burst_uint_e {
    WM_DMA_BURST_1UNIT = 0x0,
    WM_DMA_BURST_4UNIT,
    WM_DMA_BURST_UNIT_MAX
} wm_dma_burst_uint_t;

/**
 * @brief dma start/stop
 */
typedef enum wm_dma_ch_control_e {
    WM_DMA_START_EN = 0x1,
    WM_DMA_STOP_EN  = WM_DMA_START_EN,
    WM_DMA_CNTL_EN_MAX
} wm_dma_ch_control_t;

/**
 * @brief dma running status
 */
typedef enum wm_dma_running_sts_e {
    WM_DMA_IDLE = 0x1,
    WM_DMA_RUNNIG,
    WM_DMA_STS_MAX
} wm_dma_running_sts_t;

/* @brief dma status info
 */
typedef struct wm_dma_sts_info_s {
    wm_dma_running_sts_t sts;
    uint32_t xfer_cnt;
} wm_dma_sts_info_t;
/******************************************************************/
/*                  DMA HW Capability   (End)                     */
/******************************************************************/

/******************************************************************/
/*                  SD/SDIO Host HW Capability   (Start)          */
/******************************************************************/

#define WM_SDH_DATA_BUF_BYTES (512)
#define WM_SDH_BLOCK_SIZE     (512)

/**
 * @brief sd/sdio host bus width
 */
typedef enum {
    WM_SDH_BUS_WIDTH_1BIT  = 0x00, // 1 bit Data Width
    WM_SDH_BUS_WIDTH_4BITS = 0x01, // 4 bits Data Width
    WM_SDH_BUS_WIDTH_MAX
} wm_sdh_bus_width_t;

/******************************************************************/
/*                  SD/SDIO Host HW Capability   (End)            */
/******************************************************************/

/******************************************************************/
/*                  I2C HW Capability   (Start)                   */
/******************************************************************/

/**
 * @enum wm_i2c_port_t
 * @brief i2c device port
 */
typedef enum {
    WM_I2C_PORT_0 = 0x0,
    WM_I2C_PORT_NUM,
} wm_i2c_port_t;

/**
  * @enum  wm_i2c_speed_t
  * @brief i2c transfer speed mode
  */
typedef enum {
    WM_I2C_SPEED_STANDARD = 100000, /**< 100KHz */
    WM_I2C_SPEED_FAST     = 400000, /**< 400KHz */
    WM_I2C_SPEED_MAX      = 400000, /**< 400KHz */
} wm_i2c_speed_t;
/******************************************************************/
/*                  I2C HW Capability   (End)                     */
/******************************************************************/

/******************************************************************/
/*                  UART HW Capability   (Start)                  */
/******************************************************************/

#define WM_UART_HW_FIFO_NUM 32

/**
 * @enum  wm_uart_port_t;
 * @brief uart port
 */
typedef enum {
    WM_UART_PORT_0 = 0x0,
    WM_UART_PORT_1,
    WM_UART_PORT_2,
    WM_UART_PORT_3,
    WM_UART_PORT_4,
    WM_UART_PORT_5,
    WM_UART_PORT_NUM,
} wm_uart_port_t;

/**
 * @enum  wm_uart_baudrate_t
 * @brief uart baudrate, Only these enumeration values are supported
 */
typedef enum {
    WM_UART_BAUDRATE_B600     = 600,     /*< baudrate 600        */
    WM_UART_BAUDRATE_B1200    = 1200,    /*< baudrate 1200       */
    WM_UART_BAUDRATE_B1800    = 1800,    /*< baudrate 1800       */
    WM_UART_BAUDRATE_B2400    = 2400,    /*< baudrate 2400       */
    WM_UART_BAUDRATE_B4800    = 4800,    /*< baudrate 4800       */
    WM_UART_BAUDRATE_B9600    = 9600,    /*< baudrate 9600       */
    WM_UART_BAUDRATE_B19200   = 19200,   /*< baudrate 19200      */
    WM_UART_BAUDRATE_B38400   = 38400,   /*< baudrate 38400      */
    WM_UART_BAUDRATE_B57600   = 57600,   /*< baudrate 57600      */
    WM_UART_BAUDRATE_B115200  = 115200,  /*< baudrate 115200     */
    WM_UART_BAUDRATE_B230400  = 230400,  /*< baudrate 230400     */
    WM_UART_BAUDRATE_B460800  = 460800,  /*< baudrate 460800     */
    WM_UART_BAUDRATE_B921600  = 921600,  /*< baudrate 921600     */
    WM_UART_BAUDRATE_B1000000 = 1000000, /*< baudrate 1000000    */
    WM_UART_BAUDRATE_B1250000 = 1250000, /*< baudrate 1250000    */
    WM_UART_BAUDRATE_B1500000 = 1500000, /*< baudrate 1500000    */
    WM_UART_BAUDRATE_B2000000 = 2000000  /*< baudrate 2000000    */
} wm_uart_baudrate_t;

/**
 * @enum wm_uart_data_bits_t
 * @brief uart data bits
 */
typedef enum {
    WM_UART_DATA_BIT_8 = 3, /*< 8 bits    */
    WM_UART_DATA_BIT_MAX,
} wm_uart_data_bits_t;

/**
 * @enum wm_uart_stop_bits_t
 * @brief uart stop bits
 */
typedef enum {
    WM_UART_STOP_BIT_1 = 0, /*< 1 bit    */
    WM_UART_STOP_BIT_2,     /*< 2 bits   */
    WM_UART_STOP_BIT_MAX,
} wm_uart_stop_bits_t;

/**
 * @enum wm_uart_parity_t
 * @brief uart parity check
 */
typedef enum {
    WM_UART_PARITY_NONE = 0, /*< disable     */
    WM_UART_PARITY_EVEN,     /*< even check  */
    WM_UART_PARITY_ODD,      /*< odd check   */
    WM_UART_PARITY_MAX,
} wm_uart_parity_t;

/**
 * @enum wm_uart_flowctrl_t
 * @brief uart flow control type
 */
typedef enum {
    WM_UART_FLOW_CTRL_DISABLE = 0x00, /*< disable              */
    WM_UART_FLOW_CTRL_RTS     = 0x01, /*< RX flow control      */
    WM_UART_FLOW_CTRL_CTS     = 0x02, /*< TX flow control      */
    WM_UART_FLOW_CTRL_RTS_CTS = 0x03, /*< RX,TX flow control   */
    WM_UART_FLOW_CTRL_MAX
} wm_uart_flowctrl_t;

/******************************************************************/
/*                  UART HW Capability   (End)                    */
/******************************************************************/

/******************************************************************/
/*                  7816 HW Capability   (start)                  */
/******************************************************************/

/**
 * @enum wm_7816_protocol_t
 * @brief 7816 protocol
 */
typedef enum {
    WM_7816_T0 = 0, /*< T0 protocol   */
    WM_7816_T1 = 1, /*< T1 protocol   */
} wm_7816_protocol_t;

/**
 * @enum wm_7816_bits_order_t
 * @brief 7816 transfer bits order
 */
typedef enum {
    WM_7816_BITS_LSB = 0, /*< 0: LSB, bits 0 transfer fisrt */
    WM_7816_BITS_MSB = 1, /*< 1: MSB, bits 7 transfer fisrt */
} wm_7816_bits_order_t;

/**
 * @enum wm_7816_stop_bits_t
 * @brief 7816 stop bits
 */
typedef enum {
    WM_7816_STOP_BITS_HALF = 0,     /*< 0.5 bits     */
    WM_7816_STOP_BITS_ONE_AND_HALF, /*< 1.5 bits     */
    WM_7816_STOP_BITS_MAX,
} wm_7816_stop_bits_t;

/******************************************************************/
/*                  7816 HW Capability   (End)                    */
/******************************************************************/

/******************************************************************/
/*                  GPIO HW Capability   (Start)                   */
/******************************************************************/

#include "wm_gpio_reg_struct.h"

#define WM_GPIO_MAX_NUM_BY_ALL_SOC 48
typedef struct {
    wm_gpio_reg_t *hw;
} wm_gpio_map_t;

typedef struct {
    uint16_t pin_num  : 6;
    uint16_t pin_mux  : 3;
    uint16_t dir      : 1;
    uint16_t pullmode : 2;
} wm_io_mux_attr_t;

#define WM_GPIO_PIN_VALID(pin_no) ((pin_no >= WM_GPIO_NUM_START && pin_no < WM_GPIO_NUM_MAX) && g_gpio_map[pin_no].hw)

typedef enum {
    WM_GPIO_DIR_INPUT,
    WM_GPIO_DIR_OUTPUT
} wm_gpio_dir_t;

typedef enum {
    WM_GPIO_FLOAT,
    WM_GPIO_PULL_UP,
    WM_GPIO_PULL_DOWN
} wm_gpio_pupd_t;

typedef enum {
    WM_GPIO_IRQ_TRIG_FALLING_EDGE = 1,
    WM_GPIO_IRQ_TRIG_RISING_EDGE,
    WM_GPIO_IRQ_TRIG_DOUBLE_EDGE,
    WM_GPIO_IRQ_TRIG_LOW_LEVEL,
    WM_GPIO_IRQ_TRIG_HIGH_LEVEL
} wm_gpio_intr_mode_t;

typedef enum {
    WM_GPIO_IOMUX_FUN1 = 1,
    WM_GPIO_IOMUX_FUN2,
    WM_GPIO_IOMUX_FUN3,
    WM_GPIO_IOMUX_FUN4,
    WM_GPIO_IOMUX_FUN5, // for GPIO
    WM_GPIO_IOMUX_FUN6, // if set ,gpio need to be configured to WM_GPIO_DIR_INPUT and WM_GPIO_FLOAT
    WM_GPIO_IOMUX_FUN7  // if set ,gpio need to be configured to WM_GPIO_DIR_INPUT and WM_GPIO_FLOAT
} wm_gpio_pin_mux_t;

#if !CONFIG_BUILD_TYPE_W800 && !CONFIG_BUILD_TYPE_W803
#define WM_IOMUX_ATTR_NUM 22
#else
#define WM_IOMUX_ATTR_NUM 15
#endif

extern const wm_io_mux_attr_t g_iomux_attr_table[WM_IOMUX_ATTR_NUM];
extern const wm_gpio_map_t g_gpio_map[WM_GPIO_MAX_NUM_BY_ALL_SOC];

#define WM_GPIO_DIR_DEFAULT       WM_GPIO_DIR_INPUT

/******************************************************************/
/*                  GPIO HW Capability   (End)                   */
/******************************************************************/

/******************************************************************/
/*                  PWM HW Capability   (start)                   */
/******************************************************************/
/// default output pwm period cycle set to 200 times pwm clocks
#define WM_PWM_PERIOD_DEFAULT     (200 - 1)
/// default output pwm duty cycle set to 40 times pwm clocks
#define WM_PWM_DUTY_CYCLE_DEFAULT (40 - 1)
/// default output pwm frequency set to 10kHz for 40MHz main clock and 200 pwm period cycle
#define WM_PWM_CLKDIV_DEFAULT     (20)
/// max period value can be set PWM module register, final value will incremented by 1
#define WM_PWM_MAX_PERIOD         (255)
/// min period value can be set PWM module registerk0k, final value will be incremented by 1
#define WM_PWM_MIN_PERIOD         (0)
/// max clock divider value can be set PWM module register, final value will incremented by 1
#define WM_PWM_MAX_CLKDIV         (65535)
/// min clock divider value can be set PWM module register, final value will incremented by 1
#define WM_PWM_MIN_CLKDIV         (1)
/// the max clock divider for deadtime
#define WM_PWM_MAX_DT_CLKDIV      (3)

/**
 * @brief    PWM channel support
 */
enum wm_pwm_channels {
    WM_PWM_CHANNEL_0 = 0,
    WM_PWM_CHANNEL_1,
    WM_PWM_CHANNEL_2,
    WM_PWM_CHANNEL_3,
    WM_PWM_CHANNEL_4,

    WM_PWM_CHANNEL_MAX,
};

/**
 * @brief     PWM work mode supported by W800
 */
enum wm_pwm_mode {
    WM_PWM_OUT_INDPT,   /**< independent output mode */
    WM_PWM_OUT_2SYNC,   /**< two channel sync output mode */
    WM_PWM_OUT_ALLSYNC, /**< all channel sync output mode */
    WM_PWM_OUT_MC,      /**< complement output mode */
    WM_PWM_OUT_BRAKE,   /**< brake output mode */
    WM_PWM_IN_CAP,      /**< capture for input mode */
    WM_PWM_MODE_MAX
};

/**
 * @brief     PWM counter type supported by W800
 */
enum wm_pwm_cnt_type {
    WM_PWM_CNT_TYPE_INC,    /**< Edge-aligned mode counter counting mode is increment, for capture mode */
    WM_PWM_CNT_TYPE_DEC,    /**< Edge-aligned mode counter counting mode is decrement, for PWM mode) */
    WM_PWM_CNT_TYPE_CENTER, /**< center-aligned mode, for PWM mode */

    WM_PWM_CNT_TYPE_MAX
};

/**
 * @brief PIN state setting for which support capture mode
 * This configure item will be set matched with the output/capture mode
 */
enum wm_pwm_out_state {
    WM_PWM_OUT_EN_STATE_OUT, /**< output state for this PWM IO */
    WM_PWM_OUT_EN_STATE_TRI, /**< tristate state for this PWM IO */
};

/**
 * @brief     PWM interrupt type for capture mode supported by W800
 */
enum wm_pwm_cap_int_type {
    WM_PWM_CAP_INT_RISING,          /**< Use rising interrupt */
    WM_PWM_CAP_INT_FALLING,         /**< Use falling edge interrupt */
    WM_PWM_CAP_INT_RIGSING_FALLING, /**< Use both rising and falling edge interrupt */
    WM_PWM_CAP_INT_DMA,             /**< Use DMA Request Interrupt */
};

/******************************************************************/
/*                  PWM HW Capability   (End)                    */
/******************************************************************/

/******************************************************************/
/*                  Touch Sensor HW Capability   (Start)                   */
/******************************************************************/

#define WM_TOUCH_THRESHOLD_MAX 0x7f
#define WM_TOUCH_NUM_MAX       16

/******************************************************************/
/*                  TOUCH HW Capability   (End)                   */
/******************************************************************/

/******************************************************************/
/*                SEG_LCD HW Capability   (Start)                 */
/******************************************************************/

typedef enum {
    WM_SEG_LCD_DUTY_SEL_STATIC = 0, /**< Static duty */
    WM_SEG_LCD_DUTY_SEL_1_2,        /**< 1/2 duty cycle */
    WM_SEG_LCD_DUTY_SEL_1_3,        /**< 1/3 duty cycle */
    WM_SEG_LCD_DUTY_SEL_1_4,        /**< 1/4 duty cycle */
    WM_SEG_LCD_DUTY_SEL_1_5,        /**< 1/5 duty cycle */
    WM_SEG_LCD_DUTY_SEL_1_6,        /**< 1/6 duty cycle */
    WM_SEG_LCD_DUTY_SEL_1_7,        /**< 1/7 duty cycle */
    WM_SEG_LCD_DUTY_SEL_1_8,        /**< 1/8 duty cycle */
    WM_SEG_LCD_DUTY_SEL_MAX
} wm_seg_lcd_duty_sel_t;

typedef enum {
    WM_SEG_LCD_VLCD_CC_2_7V = 0, /**< 2.7V output supply voltage */
    WM_SEG_LCD_VLCD_CC_2_9V,     /**< 2.9V output supply voltage */
    WM_SEG_LCD_VLCD_CC_3_1V,     /**< 3.1V output supply voltage */
    WM_SEG_LCD_VLCD_CC_3_3V,     /**< 3.3V output supply voltage */
    WM_SEG_LCD_VLCD_CC_MAX
} wm_seg_lcd_vlcd_cc_t;

typedef enum {
    WM_SEG_LCD_BIAS_1_4 = 0, /**< 1/4 internal bias */
    WM_SEG_LCD_BIAS_1_2,     /**< 1/2 internal bias */
    WM_SEG_LCD_BIAS_1_3,     /**< 1/3 internal bias */
    WM_SEG_LCD_BIAS_STATIC,  /**< Static bias */
    WM_SEG_LCD_BIAS_MAX
} wm_seg_lcd_bias_t;

typedef enum {
    WM_SEG_LCD_COM0 = 0, /**< COM0 line */
    WM_SEG_LCD_COM1,     /**< COM1 line */
    WM_SEG_LCD_COM2,     /**< COM2 line */
    WM_SEG_LCD_COM3,     /**< COM3 line */
    WM_SEG_LCD_COM4,     /**< COM4 line */
    WM_SEG_LCD_COM5,     /**< COM5 line */
    WM_SEG_LCD_COM6,     /**< COM6 line */
    WM_SEG_LCD_COM7,     /**< COM7 line */
    WM_SEG_LCD_COM_MAX
} wm_seg_lcd_com_id_t;

typedef enum {
    WM_SEG_LCD_SEG1 = 1, /**< SEG1 line */
    WM_SEG_LCD_SEG2,     /**< SEG2 line */
    WM_SEG_LCD_SEG3,     /**< SEG3 line */
    WM_SEG_LCD_SEG4,     /**< SEG4 line */
    WM_SEG_LCD_SEG5,     /**< SEG5 line */
    WM_SEG_LCD_SEG6,     /**< SEG6 line */
    WM_SEG_LCD_SEG7,     /**< SEG7 line */
    WM_SEG_LCD_SEG8,     /**< SEG8 line */
    WM_SEG_LCD_SEG9,     /**< SEG9 line */
    WM_SEG_LCD_SEG10,    /**< SEG10 line */
    WM_SEG_LCD_SEG11,    /**< SEG11 line */
    WM_SEG_LCD_SEG12,    /**< SEG12 line */
    WM_SEG_LCD_SEG13,    /**< SEG13 line */
    WM_SEG_LCD_SEG14,    /**< SEG14 line */
    WM_SEG_LCD_SEG15,    /**< SEG15 line */
    WM_SEG_LCD_SEG16,    /**< SEG16 line */
    WM_SEG_LCD_SEG17,    /**< SEG17 line */
    WM_SEG_LCD_SEG18,    /**< SEG18 line */
    WM_SEG_LCD_SEG19,    /**< SEG19 line */
    WM_SEG_LCD_SEG20,    /**< SEG20 line */
    WM_SEG_LCD_SEG21,    /**< SEG21 line */
    WM_SEG_LCD_SEG22,    /**< SEG22 line */
    WM_SEG_LCD_SEG23,    /**< SEG23 line */
    WM_SEG_LCD_SEG24,    /**< SEG24 line */
    WM_SEG_LCD_SEG25,    /**< SEG25 line */
    WM_SEG_LCD_SEG26,    /**< SEG26 line */
    WM_SEG_LCD_SEG27,    /**< SEG27 line */
    WM_SEG_LCD_SEG28,    /**< SEG28 line */
    WM_SEG_LCD_SEG29,    /**< SEG29 line */
    WM_SEG_LCD_SEG30,    /**< SEG30 line */
    WM_SEG_LCD_SEG31,    /**< SEG31 line */
    WM_SEG_LCD_SEG_MAX
} wm_seg_lcd_seg_id_t;

typedef enum {
    WM_SEG_LCD_DRIVE_STRENGTH_LOW = 0, /**< Low output drive strength */
    WM_SEG_LCD_DRIVE_STRENGTH_HIGH,    /**< High output drive strength */
    WM_SEG_LCD_DRIVE_STRENGTH_MAX
} wm_seg_lcd_hd_t;

/******************************************************************/
/*                 SEG_LCD HW Capability   (End)                  */
/******************************************************************/

/******************************************************************/
/*                  ADC HW Capability   (Start)                   */
/******************************************************************/
#define WM_ADC_RESULT_MASK (0x3FFFF)

/*
    WM_ADC_GAIN1_LEVEL_0 * WM_ADC_GAIN2_LEVEL_0  = 1
    WM_ADC_GAIN1_LEVEL_0 * WM_ADC_GAIN2_LEVEL_1  = 2
    WM_ADC_GAIN1_LEVEL_0 * WM_ADC_GAIN2_LEVEL_2  = 3
    WM_ADC_GAIN1_LEVEL_0 * WM_ADC_GAIN2_LEVEL_3  = 4

    WM_ADC_GAIN1_LEVEL_1 * WM_ADC_GAIN2_LEVEL_0  = 16
    WM_ADC_GAIN1_LEVEL_1 * WM_ADC_GAIN2_LEVEL_1  = 32
    WM_ADC_GAIN1_LEVEL_1 * WM_ADC_GAIN2_LEVEL_2  = 48
    WM_ADC_GAIN1_LEVEL_1 * WM_ADC_GAIN2_LEVEL_3  = 64

    WM_ADC_GAIN1_LEVEL_2 * WM_ADC_GAIN2_LEVEL_0  = 32
    WM_ADC_GAIN1_LEVEL_2 * WM_ADC_GAIN2_LEVEL_1  = 64
    WM_ADC_GAIN1_LEVEL_2 * WM_ADC_GAIN2_LEVEL_2  = 96
    WM_ADC_GAIN1_LEVEL_2 * WM_ADC_GAIN2_LEVEL_3  = 128

    WM_ADC_GAIN1_LEVEL_3 * WM_ADC_GAIN2_LEVEL_0  = 64
    WM_ADC_GAIN1_LEVEL_3 * WM_ADC_GAIN2_LEVEL_1  = 128
    WM_ADC_GAIN1_LEVEL_3 * WM_ADC_GAIN2_LEVEL_2  = 192
    WM_ADC_GAIN1_LEVEL_3 * WM_ADC_GAIN2_LEVEL_3  = 256

    WM_ADC_GAIN1_LEVEL_4 * WM_ADC_GAIN2_LEVEL_0  = 128
    WM_ADC_GAIN1_LEVEL_4 * WM_ADC_GAIN2_LEVEL_1  = 256
    WM_ADC_GAIN1_LEVEL_4 * WM_ADC_GAIN2_LEVEL_2  = 384
    WM_ADC_GAIN1_LEVEL_4 * WM_ADC_GAIN2_LEVEL_3  = 512

    WM_ADC_GAIN1_LEVEL_5 * WM_ADC_GAIN2_LEVEL_0  = 256
    WM_ADC_GAIN1_LEVEL_5 * WM_ADC_GAIN2_LEVEL_1  = 512
    WM_ADC_GAIN1_LEVEL_5 * WM_ADC_GAIN2_LEVEL_2  = 768
    WM_ADC_GAIN1_LEVEL_5 * WM_ADC_GAIN2_LEVEL_3  = 1024

*/
typedef enum {
    WM_ADC_GAIN1_LEVEL_0,
    WM_ADC_GAIN1_LEVEL_1,
    WM_ADC_GAIN1_LEVEL_2,
    WM_ADC_GAIN1_LEVEL_3,
    WM_ADC_GAIN1_LEVEL_4,
    WM_ADC_GAIN1_LEVEL_5,
} wm_adc_gain1_level_t;

typedef enum {
    WM_ADC_GAIN2_LEVEL_0,
    WM_ADC_GAIN2_LEVEL_1,
    WM_ADC_GAIN2_LEVEL_2,
    WM_ADC_GAIN2_LEVEL_3,
} wm_adc_gain2_level_t;

typedef enum {
    WM_ADC_CHANNEL_0              = 0, //WM_GPIO_NUM_1
    WM_ADC_CHANNEL_1              = 1, //WM_GPIO_NUM_4
    WM_ADC_CHANNEL_2              = 2, //WM_GPIO_NUM_3
    WM_ADC_CHANNEL_3              = 3, //WM_GPIO_NUM_2
    WM_ADC_CHANNEL_0_1_DIFF_INPUT = 8, //WM_GPIO_NUM_1 && WM_GPIO_NUM_4
    WM_ADC_CHANNEL_2_3_DIFF_INPUT = 9, //WM_GPIO_NUM_3 && WM_GPIO_NUM_2
    WM_ADC_CHANNEL_MAX,
} wm_adc_channel_t;

typedef enum {
    WM_ADC_CHANNEL_TEMP   = 0xC,
    WM_ADC_CHANNEL_VOLT   = 0xD,
    WM_ADC_CHANNEL_OFFSET = 0xE,
} wm_adc_internal_channel_t;

typedef enum {
    WM_ADC_INTR_TYPE_ADC = 0,
    WM_ADC_INTR_TYPE_DMA, // NOT SUPPORT
    WM_ADC_INTR_TYPE_COMP,
    WM_ADC_INTR_TYPE_MAX
} wm_adc_intr_type_t;

#define WM_ADC_MAX_CHANNEL_COUNT 6

/******************************************************************/
/*                  ADC HW Capability   (End)                   */
/******************************************************************/

/******************************************************************/
/*                  GPSEC HW Capability   (Start)                   */
/******************************************************************/

/**
 * @brief     crc max process bytes one time for W80x
 */
#define W800_CRC_MAX_CALC_BYTES  65532

/**
 * @brief     gpsec cypher mode
 */
typedef enum {
    WM_GPSEC_CYPHER_MODE_RSV = 0, /*< RSV */
    WM_GPSEC_CYPHER_MODE_RC4,     /*< RC4 */
    WM_GPSEC_CYPHER_MODE_SHA1,    /*< SHA1 */
    WM_GPSEC_CYPHER_MODE_AES,     /*< AES */
    WM_GPSEC_CYPHER_MODE_DES,     /*< DES */
    WM_GPSEC_CYPHER_MODE_3DES,    /*< 3DES */
    WM_GPSEC_CYPHER_MODE_CRC,     /*< CRC */
    WM_GPSEC_CYPHER_MODE_MD5,     /*< MD5 */
} wm_gpsec_cypher_mode_t;

/**
 * @brief     gpsec select encrypt or decrypt
 */
typedef enum {
    WM_GPSEC_ENCRYPT = 0, /*< encrypt */
    WM_GPSEC_DECRYPT,     /*< decrypt */
} wm_gpsec_encrypt_decrypt_sel_t;

/**
 * @brief     Algorithm type sub-mode selection
 */
typedef enum {
    WM_GPSEC_ECB = 0, /*< ECB mode for DES/AES*/
    WM_GPSEC_CBC,     /*< CBC mode for DES/AES*/
    WM_GPSEC_CTR,     /*< CTR mode for AES*/
    WM_GPSEC_MAC,     /*< MAC mode for AES*/
} wm_gpsec_sub_mode_t;

/**
 * @brief     CRC type selection
 */
typedef enum {
    WM_GPSEC_CRC8 = 0,     /*< 8 bits CRC */
    WM_GPSEC_CRC16_MODBUS, /*< 16 bits Modbus CRC*/
    WM_GPSEC_CRC16_CCITT,  /*< 16 bits CCITT CRC*/
    WM_GPSEC_CRC32,        /*< 32 bits CRC*/
} wm_gpsec_crc_type_t;

/**
 * @brief    gpsec crc output and input data reverse
 */
typedef enum {
    WM_GPSEC_CRC_REVERSE_NONE = 0, /*< crc output and input not reverse */
    WM_GPSEC_CRC_OUT_REVERSE,      /*< crc output reverse */
    WM_GPSEC_CRC_IN_REVERSE,       /*< crc input reverse */
    WM_GPSEC_CRC_OUT_IN_REVERSE,   /*< crc output and input reverse */
} wm_gpsec_crc_reverse_t;

/**
 * @brief    pseudo random number bits
 */
typedef enum {
    WM_GPSEC_RNG_16BIT = 0, /*< 16bit */
    WM_GPSEC_RNG_32BIT,     /*< 32bit */
} wm_gpsec_prng_bits_t;

/**
 * @brief    prng load seed method
 */
typedef enum {
    WM_GPSEC_RNG_USE_0 = 0, /*< default to using zero as the seed */
    WM_GPSEC_RNG_LOAD,      /*< need load */
} wm_gpsec_prng_load_seed_method_t;

/**
 * @brief    block length in RC4 encryption/decryption
 */
typedef enum {
    WM_GPSEC_RC4_128BIT = 0, /*< 128bits block length in RC4 encryption/decryption */
    WM_GPSEC_RC4_256BIT,     /*< 256bits block length in RC4 encryption/decryption */
} wm_gpsec_rc4_block_length_t;

/**
 * @brief    rng result selection
 */
typedef enum {
    WM_GPSEC_RESULT_DISP_PRNG = 0, /*< Result register displays prng */
    WM_GPSEC_RESULT_DISP_TRNG,     /*< Result register displays trng */
} wm_gpsec_result_select_t;

/**
 * @brief    TRNG digital post-processing module bypass
 */
typedef enum {
    WM_GPSEC_TRNG_DIG = 0,    /*< TRNG digital post-process */
    WM_GPSEC_TRNG_DIG_BYPASS, /*< TRNG digital post-process bypass */
} wm_gpsec_trng_dig_process_bypass_t;

/******************************************************************/
/*                  GPSEC HW Capability   (End)                   */
/******************************************************************/

/******************************************************************/
/*                  I2S HW Capability   (Start)                   */
/******************************************************************/

/// min divider value support for I2S mclk
#define WM_I2S_MCLK_DIV_MIN 2
#define WM_I2S_MCLK_DIV_MAX 63
#define WM_I2S_BCLK_DIV_MIN 1
#define WM_I2S_BCLK_DIV_MAX 1023

/**
 * @brief    i2s select master or slave
 */
enum wm_i2s_mode {
    WM_I2S_MODE_MASTER,
    WM_I2S_MODE_SLAVE,
    WM_I2S_MODE_MAX
};
enum wm_i2s_dir {
    WM_I2S_DIR_IN,
    WM_I2S_DIR_OUT,
    WM_I2S_DIR_INOUT,
    WM_I2S_DIR_MAX
};

/**
 * @brief The standard of I2S to use
 */
enum wm_i2s_std {
    WM_I2S_STD_I2S,
    WM_I2S_STD_MSB,
    WM_I2S_STD_PCMA,
    WM_I2S_STD_PCMB,
    WM_I2S_STD_MAX
};

/**
 * @brief The format of frame
 * @warning For 24 bit sampling, it is necessary to expand 24 bits to 32 bits before sending,
 *      and all the high 8 bits are 0. After receiving, it is necessary to remove the high 8 bits
 *      and restore them to 24 bits.
 */
enum wm_i2s_bits {
    WM_I2S_BITS_8BIT  = 8,
    WM_I2S_BITS_16BIT = 16,
    WM_I2S_BITS_24BIT = 24,
    WM_I2S_BITS_32BIT = 32,
    WM_I2S_BITS_MAX
};

/**
 * @brief The type of channel
 */
enum wm_i2s_chan_type {
    WM_I2S_CHAN_TYPE_MONO_LEFT  = (1 << 0),
    WM_I2S_CHAN_TYPE_MONO_RIGHT = (1 << 1),
    WM_I2S_CHAN_TYPE_STEREO     = (WM_I2S_CHAN_TYPE_MONO_LEFT | WM_I2S_CHAN_TYPE_MONO_RIGHT),

    WM_I2S_CHAN_TYPE_MAX,
};

/**
 * @brief Using the I2S interface with GPIO pins
 */
enum wm_i2s_pins {
    WM_I2S_PIN_MCLK,
    WM_I2S_PIN_BCLK,
    WM_I2S_PIN_LRCLK,
    WM_I2S_PIN_OUT,
    WM_I2S_PIN_IN,
    WM_I2S_PIN_MAX
};

/**
 * @brief The method of data displacement
 */
enum wm_i2s_xfer_type {
    WM_I2S_XFER_DMA, /**< xfer data with DMA by default */
    WM_I2S_XFER_MAX
};

/**
 * @brief    i2s interrupt mask
 */
typedef enum {
    WM_I2S_RXFIFO_UNDERFLOW_IRQ_MASK   = (1 << 0), /*< [0] rxfifo underflow interrupt mask */
    WM_I2S_RXFIFO_OVERFLOW_IRQ_MASK    = (1 << 1), /*< [1] rxfifo overflow interrupt mask */
    WM_I2S_RXFIFO_THRESHOLD_IRQ_MASK   = (1 << 2), /*< [2] rxfifo threshold interrupt mask */
    WM_I2S_RXDONE_IRQ_MASK             = (1 << 3), /*< [3] rxdone interrupt mask */
    WM_I2S_TXFIFO_UNDERFLOW_IRQ_MASK   = (1 << 4), /*< [4] txfifo underflow interrupt mask */
    WM_I2S_TXFIFO_OVERFLOW_IRQ_MASK    = (1 << 5), /*< [5] txfifo overflow interrupt mask */
    WM_I2S_TXFIFO_THRESHOLD_IRQ_MASK   = (1 << 6), /*< [6] txfifo threshold interrupt mask */
    WM_I2S_TXDONE_IRQ_MASK             = (1 << 7), /*< [7] txdone interrupt mask */
    WM_I2S_RIGHT_ZERO_CROSS_IRQ_MASK   = (1 << 8), /*< [8] Right Channel Zero-Crossing Interrupt mask */
    WM_LL_I2S_LEFT_ZERO_CROSS_IRQ_MASK = (1 << 9), /*< [9] Left Channel Zero-Crossing Interrupt mask */
} wm_i2s_irq_mask_t;

/**
 * @brief    i2s interrupt flag
 */
typedef enum {
    WM_I2S_RXFIFO_UNDERFLOW_FLAG = (1 << 0),  /*< [0] rxfifo underflow flag */
    WM_I2S_RXFIFO_OVERFLOW_FLAG  = (1 << 1),  /*< [1] rxfifo overflow flag */
    WM_I2S_RXFIFO_THRESHOLD_FLAG = (1 << 2),  /*< [2] rxfifo threshold flag */
    WM_I2S_RXDONE_FLAG           = (1 << 3),  /*< [3] rxdone flag */
    WM_I2S_TXFIFO_UNDERFLOW_FLAG = (1 << 4),  /*< [4] txfifo underflow flag */
    WM_I2S_TXFIFO_OVERFLOW_FLAG  = (1 << 5),  /*< [5] txfifo overflow flag */
    WM_I2S_TXFIFO_THRESHOLD_FLAG = (1 << 6),  /*< [6] txfifo threshold flag */
    WM_I2S_TXDONE_FLAG           = (1 << 7),  /*< [7] txdone flag */
    WM_I2S_RIGHT_ZERO_CROSS_FLAG = (1 << 8),  /*< [8] Right Channel Zero-Crossing flag */
    WM_I2S_LEFT_ZERO_CROSS_FLAG  = (1 << 9),  /*< [9] Left Channel Zero-Crossing flag */
    WM_I2S_RX_OR_TX_FLAG         = (1 << 10), /*< [10] Rx or Tx Interrupt flag */
    WM_I2S_RECEIVE_FLAG          = (1 << 11), /*< [11] receive flag */
    WM_I2S_TRANSMISSION_FLAG     = (1 << 12), /*< [12] transmission flag */
} wm_i2s_irq_flag_t;

/**
 * @brief     clear i2s interrupt flag
 */
typedef enum {
    WM_I2S_RXFIFO_UNDERFLOW_FLAG_CLEAR   = (1 << 0), /*< [0] clear rxfifo underflow flag */
    WM_I2S_RXFIFO_OVERFLOW_FLAG_CLEAR    = (1 << 1), /*< [1] clear rxfifo overflow flag */
    WM_I2S_RXDONE_FLAG_CLEAR             = (1 << 3), /*< [3] clear rxdone flag */
    WM_I2S_TXFIFO_UNDERFLOW_FLAG_CLEAR   = (1 << 4), /*< [4] clear txfifo underflow flag */
    WM_I2S_TXFIFO_OVERFLOW_FLAG_CLEAR    = (1 << 5), /*< [5] clear txfifo overflow flag */
    WM_I2S_TXDONE_FLAG_CLEAR             = (1 << 7), /*< [7] clear txdone flag */
    WM_I2S_RIGHT_ZERO_CROSS_FLAG_CLEAR   = (1 << 8), /*< [8] clear Right Channel Zero-Crossing flag */
    WM_LL_I2S_LEFT_ZERO_CROSS_FLAG_CLEAR = (1 << 9), /*< [9] clear Left Channel Zero-Crossing flag */
} wm_i2s_irq_flag_clear_t;

#define I2S_INT_MASK    (0x3ff)  /*!< I2S all interrupt mask bitmap */
#define I2S_INT_GET_SRC (0x1fff) /*!< get I2S all interrupt src bitmap */
#define I2S_INT_CLS_SRC (0x3ff)  /*!< clear I2S all interrupt src bitmap */

/******************************************************************/
/*                  I2S HW Capability   (End)                     */
/******************************************************************/

/******************************************************************/
/*                  XXX HW Capability   (Start)                   */
/******************************************************************/
/******************************************************************/
/*                  XXX HW Capability   (End)                     */
/******************************************************************/

/**
 * @}
 */

#endif /* __SOC_CFGS_H__ */
