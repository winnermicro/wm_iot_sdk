/**
 * @file wm_clock.c
 *
 * @brief clock porting layer
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

#include <time.h>
#include "FreeRTOS_POSIX.h"

/* Platform clock include. */
#include "clock.h"

#define MICROSECONDS_PER_MILLISECOND (1000L) /**< @brief Nanoseconds per millisecond. */
#define MILLISECONDS_PER_SECOND      (1000L) /**< @brief Milliseconds per second. */

uint32_t Clock_GetTimeMs(void)
{
    int64_t timeMs;
    struct timeval tv;
    /* Get current time. */
    (void)gettimeofday(&tv, NULL);

    /* Calculate the milliseconds from timespec. */
    timeMs = (tv.tv_sec * MILLISECONDS_PER_SECOND) + (tv.tv_usec / MICROSECONDS_PER_MILLISECOND);

    /* Libraries need only the lower 32 bits of the time in milliseconds, since
     * this function is used only for calculating the time difference.
     * Also, the possible overflows of this time value are handled by the
     * libraries. */
    return (uint32_t)timeMs;
}

void Clock_SleepMs(uint32_t sleepTimeMs)
{
    vTaskDelay(pdMS_TO_TICKS(sleepTimeMs) ? pdMS_TO_TICKS(sleepTimeMs) : 1);
}
