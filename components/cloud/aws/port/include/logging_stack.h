/**
 * @file logging.h
 *
 * @brief AWS Cloud Log porting layer
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

//"logging_stack.h".

#ifndef __LOGGING_H_
#define __LOGGING_H_

#if !defined(LOG_TAG)
#if defined(LIBRARY_LOG_NAME)
#define LOG_TAG LIBRARY_LOG_NAME
#else
#define LOG_TAG "aws"
#endif
#endif

#include "wm_log.h"

#define LogError(message) wm_log_error message
#define LogWarn(message)  wm_log_warn message
#define LogInfo(message)  wm_log_info message
#define LogDebug(message) wm_log_debug message
#define LogRaw(message)   wm_log_raw message

#endif /* ifndef __LOGGING_H_ */
