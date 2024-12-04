/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     trap_c.c
 * @brief    source file for the trap process
 * @version  V1.0
 * @date     12. December 2017
 ******************************************************************************/
#include "wmsdk_config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <csi_config.h>
#ifdef CONFIG_XT804_TRAP_OUTPUT_RAMDUMP
#include "wm_debug.h"
#endif

/* Use in local range, so it is not added to the header file
   Use wm_printf_direct because iterrupt is no work now
*/
extern int wm_printf_direct(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

void trap_c(unsigned int *regs)
{
    int i;
    unsigned int vec = 0;

    asm volatile("mfcr    %0, psr \n"
                 "lsri    %0, 16 \n"
                 "sextb   %0 \n"
                 : "=r"(vec)
                 :);

    wm_printf_direct("\r\n");
    wm_printf_direct("CPU Exception : %u", vec);
    wm_printf_direct("\r\n");

#ifdef CONFIG_XT804_TRAP_PRINT_STR
    wm_printf_direct("Exception name: ");
    switch (vec) {
        case 1:
        {
            wm_printf_direct("Unaligned access");
            break;
        }
        case 2:
        {
            wm_printf_direct("Access error");
            break;
        }
        case 3:
        {
            wm_printf_direct("Divide by zero");
            break;
        }
        case 4:
        {
            wm_printf_direct("Illegal instruction");
            break;
        }
        case 5:
        {
            wm_printf_direct("Privilege violation");
            break;
        }
        case 6:
        {
            wm_printf_direct("tracking");
            break;
        }
        case 7:
        {
            wm_printf_direct("Break point");
            break;
        }
        case 8:
        {
            wm_printf_direct("Unrecoverable error");
            break;
        }
        case 9:
        {
            wm_printf_direct("Idly");
            break;
        }
        case 16:
        case 17:
        case 18:
        case 19:
        {
            wm_printf_direct("Trap instruction");
            break;
        }
        case 22:
        {
            wm_printf_direct("TSPEND");
            break;
        }
        case 30:
        {
            wm_printf_direct("Floating-point arithmetic");
            break;
        }
        default:
        {
            wm_printf_direct("Other");
            break;
        }
    }
    wm_printf_direct("\r\n");
#endif

    wm_printf_direct("\r\n");

    for (i = 0; i < 16; i++) {
        wm_printf_direct("r%-2d: 0x%08x\t", i, regs[i]);

        if ((i % 4) == 3) {
            wm_printf_direct("\r\n");
        }
    }

    wm_printf_direct("\r\n");
    wm_printf_direct("psr: 0x%08x\r\n", regs[16]);
    wm_printf_direct("pc : 0x%08x\r\n", regs[17]);

    wm_printf_direct("\r\n");
    wm_printf_direct("Show pc info by run: %saddr2line -p -f -e ./build/%s.elf -a 0x%08x 0x%08x\r\n", CONFIG_TOOLCHAIN_PREFIX,
           CONFIG_PROJECT_NAME, regs[17], regs[15]);

#ifdef CONFIG_XT804_TRAP_OUTPUT_RAMDUMP
    wm_output_ramdump(regs, 72); ////4 * 18
#endif

    while (1)
        ;
}

