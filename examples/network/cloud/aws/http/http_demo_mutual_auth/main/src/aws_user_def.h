/**
 * @file aws_user_def.h
 *
 * @brief AWS user define file
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

#ifndef __AWS_USER_DEF_H__
#define __AWS_USER_DEF_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AWS_MQTT_CLI_TASK_STACK_SIZE (1024) /** mqtt command line task stack size */
#define AWS_MQTT_CLI_TASK_PRIORITY   (5)    /** mqtt command line task priority */

extern const char aws_root_ca1[] asm("__binary_main_cert_AmazonRootCA1_pem"); /** root CA */

#if USE_INTERNAL_CERT
extern const char aws_client_cert[] asm("__binary_main_cert_internal_certificate_pem_crt"); /** device certificate */
extern const char aws_client_cert_key[] asm("__binary_main_cert_internal_private_pem_key"); /** device private_pem */
#else
extern const char aws_client_cert[] asm("__binary_main_cert_common_certificate_pem_crt"); /** device certificate */
extern const char aws_client_cert_key[] asm("__binary_main_cert_common_private_pem_key"); /** device private_pem */
#endif

/**
 * @brief Details of the MQTT broker to connect to.
 *
 * This is the Thing's Rest API Endpoint for AWS IoT.
 *
 * @note Your AWS IoT Core endpoint can be found in the AWS IoT console under
 * Settings/Custom Endpoint, or using the describe-endpoint API.
 *
 * #define AWS_IOT_ENDPOINT               "...insert here..."
 */
#define AWS_IOT_ENDPOINT "a2crv3ycvx4d9c-ats.iot.ap-south-1.amazonaws.com"

#ifdef __cplusplus
}
#endif

#endif /* __AWS_USER_DEF_H__ */
