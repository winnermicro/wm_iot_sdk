/*
 * Copyright (c) 2018 WinnerMicro Semiconductor ASA
 *
 */

/** @file
 *  @brief WinnerMicro WiFi provision by BLE sample
 */
#include <kernel.h>
extern int wps_app_init(void);

int main(void)
{
    wps_app_init();

    for (;;) {
        k_sleep(K_MSEC(1000));
    }
}
