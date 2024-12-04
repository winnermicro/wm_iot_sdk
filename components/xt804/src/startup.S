/**
 * @file startup.S
 *
 * @brief startup file for W80X
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

#include <csi_config.h>

.section .vectors
    .align  10
    .globl  __Vectors
    .type   __Vectors, @object
__Vectors:
    .long   reset_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   tspend_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler
    .long   default_handler

    /* External interrupts */
    .long   irq_interrupt_entry         /*  0:  SDIO         */
    .long   irq_interrupt_entry         /*  1:  MAC          */
    .long   irq_interrupt_entry         /*  2:  RF Cfg       */
    .long   irq_interrupt_entry         /*  3:  SEC          */
    .long   irq_interrupt_entry         /*  4:  DMA_Channel0       */
    .long   irq_interrupt_entry         /*  5:  DMA_Channel1       */
    .long   irq_interrupt_entry         /*  6:  DMA_Channel2       */
    .long   irq_interrupt_entry         /*  7:  DMA_Channel3       */
    .long   irq_interrupt_entry         /*  8:  DMA_Channel4_7     */
    .long   irq_interrupt_entry         /*  9:  DMA_BRUST          */
    .long   irq_interrupt_entry         /*  10: IIC         */
    .long   irq_interrupt_entry         /*  11: SD ADC      */
    .long   irq_interrupt_entry         /*  12: LS SPI      */
    .long   irq_interrupt_entry         /*  13: HS SPI      */
    .long   irq_interrupt_entry         /*  14: GPIOA       */
    .long   irq_interrupt_entry         /*  15: GPIOB       */
    .long   irq_interrupt_entry         /*  16: UART0       */
    .long   irq_interrupt_entry         /*  17: UART1       */
    .long   irq_interrupt_entry         /*  18: UART2&7816  */
    .long   irq_interrupt_entry         /*  19: USART3_5    */
    .long   irq_interrupt_entry         /*  20: BLE         */
    .long   irq_interrupt_entry         /*  21: BT          */
    .long   irq_interrupt_entry         /*  22: PWM         */
    .long   irq_interrupt_entry         /*  23: I2S         */
    .long   irq_interrupt_entry         /*  24: SDIO HOST   */
    .long   irq_interrupt_entry         /*  25:  CoreTIM      */
    .long   irq_interrupt_entry         /*  26:  RSA          */
    .long   irq_interrupt_entry         /*  27:  GPSEC        */
    .long   irq_interrupt_entry         /*  28:  Flash        */
    .long   irq_interrupt_entry         /*  29:  PMU          */
    .long   irq_interrupt_entry         /*  30:  Timer0_5     */
    .long   irq_interrupt_entry         /*  31:  Watch dog    */

    .size   __Vectors, . - __Vectors

    .text
    .align  2
_start:
    .text
    .align  2
    .globl  reset_handler
    .type   reset_handler, %function
reset_handler:
#ifdef CONFIG_KERNEL_NONE
    lrw     r0, 0xe0000200
#else
    lrw     r0, 0x80000200
    mtcr    r0, psr
#endif
    mtcr    r0, psr

    lrw     r0, g_top_irqstack
    mov     sp, r0

/*
 *	move __Vectors to irq_vectors
 */
    lrw     r1, __Vectors
    lrw     r2, __vdata_start__
    lrw     r3, __vdata_end__

    subu    r3, r2
    cmpnei  r3, 0
    bf      .L_loopv0_done

.L_loopv0:
    ldw     r0, (r1, 0)
    stw     r0, (r2, 0)
    addi    r1, 4
    addi    r2, 4
    subi    r3, 4
    cmpnei  r3, 0
    bt      .L_loopv0

.L_loopv0_done:

/*
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    lrw     r1, __erodata
    lrw     r2, __data_start__
    lrw     r3, __data_end__

    subu    r3, r2
    cmpnei  r3, 0
    bf      .L_loop0_done

.L_loop0:
    ldw     r0, (r1, 0)
    stw     r0, (r2, 0)
    addi    r1, 4
    addi    r2, 4
    subi    r3, 4
    cmpnei  r3, 0
    bt      .L_loop0

.L_loop0_done:

/*
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
 #if 1
    lrw     r1, __bss_start__
    lrw     r2, __bss_end__

    movi    r0, 0

    subu    r2, r1
    cmpnei  r2, 0
    bf      .L_loop1_done

.L_loop1:
    stw     r0, (r1, 0)
    addi    r1, 4
    subi    r2, 4
    cmpnei  r2, 0
    bt      .L_loop1
.L_loop1_done:
#endif

#ifndef __NO_SYSTEM_INIT
    jbsr    cpu_init
#endif

#ifndef __NO_BOARD_INIT
    jbsr    soc_init
#endif

    jbsr    wm_start
    .size   reset_handler, . - reset_handler

__exit:
    br      __exit

.section .bss

    .align  2
    .globl  g_intstackalloc
    .global g_intstackbase
    .global g_top_irqstack
g_intstackalloc:
g_intstackbase:
    .space CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
