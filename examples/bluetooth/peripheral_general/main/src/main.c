/* main.c - Application main entry point */

/*
 * Copyright (c) 2023 WinnerMicro Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include "wmsdk_config.h"

extern void run_notify_sample();
extern void run_indicate_sample();

int main(void)
{
#if defined(CONFIG_INDICATE_EXAMPLE)
    run_indicate_sample();
#else
    run_notify_sample();
#endif

    return 0;
}
