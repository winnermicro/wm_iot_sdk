/**
 * @file wm_irq.h
 *
 * @brief Irq Module
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
#ifndef __WM_IRQ_H__
#define __WM_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WM_IRQ_SYS_INTR_NUM 32 /*!< System internal interrupt num                        */

typedef enum {
    /******  W800 specific Interrupt Numbers *********************************************************/
    WM_IRQ_SDIO           = 0,  /*!< SDIO Receiver/Transmit/Command Receiver/SDIO Command Transmit Interrupt */
    WM_IRQ_MAC            = 1,  /*!< MAC Interrupt                                        */
    WM_IRQ_RF_CFG         = 2,  /*!< RF_CFG Interrupt                                     */
    WM_IRQ_SEC            = 3,  /*!< SEC RX/TX_MNGT/TX_DATA Interrupt                     */
    WM_IRQ_DMA_Channel0   = 4,  /*!< DMA  Channel 0 global Interrupt                      */
    WM_IRQ_DMA_Channel1   = 5,  /*!< DMA  Channel 1 global Interrupt                      */
    WM_IRQ_DMA_Channel2   = 6,  /*!< DMA  Channel 2 global Interrupt                      */
    WM_IRQ_DMA_Channel3   = 7,  /*!< DMA  Channel 3 global Interrupt                      */
    WM_IRQ_DMA_Channel4_7 = 8,  /*!< DMA  Channel 4~7 global Interrupt                    */
    WM_IRQ_DMA_BRUST      = 9,  /*!< DMA Burst global Interrupt                           */
    WM_IRQ_I2C            = 10, /*!< I2C Interrupts                                       */
    WM_IRQ_ADC            = 11, /*!< ADC Convert Interrupts                               */
    WM_IRQ_SPI_LS         = 12, /*!< Low Speed SPI Interrupt                              */
    WM_IRQ_SPI_HS         = 13, /*!< High Speed SPI Interrupt                             */
    WM_IRQ_GPIOA          = 14, /*!< GPIO A Interrupt                                     */
    WM_IRQ_GPIOB          = 15, /*!< GPIO B Interrupt                                     */
    WM_IRQ_UART0          = 16, /*!< UART0 Interrupts                                     */
    WM_IRQ_UART1          = 17, /*!< UART1 Interrupt                                      */
    WM_IRQ_TOUCH          = 18, /*!< Touch Sensor Iterrupt                                */
    WM_IRQ_UART24         = 19, /*!< UART2/3/4/5 Interrupt                                */
    WM_IRQ_BLE            = 20, /*!< BLE Interrupt                                        */
    WM_IRQ_BT             = 21, /*!< BT Interrupt                                         */
    WM_IRQ_PWM            = 22, /*!< PWM Interrupt                                        */
    WM_IRQ_I2S            = 23, /*!< I2S  Interrupts                                      */
    WM_IRQ_SIDO_HOST      = 24, /*!< SDIO Host Interrupts                                 */
    WM_IRQ_SYS_TICK       = 25, /*!< SYS Tick Interrupts                                  */
    WM_IRQ_RSA            = 26, /*!< RSA Interrupts                                       */
    WM_IRQ_CRYPTION       = 27, /*!< GPSEC Interrupts                                     */
    WM_IRQ_FLASH          = 28, /*!< Flash Interrupt                                      */
    WM_IRQ_PMU            = 29, /*!< PMU Interrupt                                        */
    WM_IRQ_TIMER          = 30, /*!< Timer0~5 Trigger                                     */
    WM_IRQ_WDG            = 31, /*!< Watch Dog Interrupt                                  */
    WM_IRQ_MAX,                 /*!< Max Interrupt Num                                    */
} wm_irq_no_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_IRQ_H__ */
