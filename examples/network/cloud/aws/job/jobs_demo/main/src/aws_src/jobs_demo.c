/*
 * AWS IoT Device SDK for Embedded C 202211.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * This demonstration downloads files from URLs present in job documents
 * received from the AWS IoT Jobs service. It shows the use of the jobs
 * library with the Mosquitto client MQTT library for communicating with the
 * AWS IoT Jobs service.  More details are available in the usage function
 * in this file.  Note: This demo focuses on use of the jobs library;
 * a thorough explanation of libmosquitto is beyond the scope of the demo.
 */

/* C standard includes. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "wm_types.h"
#include "wm_utils.h"

#include "mqtt_operations.h"
#include "logging_stack.h"
#include "demo_config.h"
#include "jobs.h"
#include "core_json.h"

#include "FreeRTOS_POSIX.h"
#include "aws_mbedtls_port.h"
/*-----------------------------------------------------------*/

/**
 * @brief MQTT server port number.
 *
 * AWS IoT Core uses this port for MQTT over TLS.
 */
#define DEFAULT_MQTT_PORT              (8883)

#define TRANSPORT_SEND_RECV_TIMEOUT_MS (10000U)

/*-----------------------------------------------------------*/

/**
 * @brief The several states of execution.
 */
typedef enum {
    None = 0, /* no current job */
    Ready,    /* job document received and parsed */
    Running,  /* download in progress */
    Cancel,   /* cancel due to failed update */
} runStatus_t;

/**
 * @brief All runtime parameters and state.
 */
typedef struct {
    /* thing name */
    const char *name;
    size_t nameLength;
    /* connection parameters */
    struct NetworkContext networkContext;

    /* flags */
    bool runOnce;
} handle_t;

/*-----------------------------------------------------------*/

/**
 * @brief Populate a handle with default values.
 *
 * @param[in] p runtime state handle
 */
void initHandle(handle_t *p);

/**
 * @brief Subscribe to a Jobs topic.
 *
 * @param[in] h runtime state handle
 * @param[in] api the desired Jobs topic
 *
 * @return true if the broker granted the subscription;
 * false otherwise
 */
static bool subscribe(handle_t *h, JobsTopic_t api);

/**
 * @brief Publish a status update for a job ID to the Jobs service.
 *
 * @param[in] h runtime state handle
 * @param[in] jobid the job ID
 * @param[in] jobidLength size of the job ID string
 * @param[in] report body of the update
 *
 * @return true if libmosquitto accepted the publish message;
 * false otherwise
 *
 * @note This does not call mosquitto_loop(); it expects main() to do so.
 */
static bool sendUpdate(handle_t *h, const char *jobid, size_t jobidLength, const char *report);

/**
 * @brief Read job ID and URL from a JSON job document.
 *
 * @param[in] h runtime state handle
 * @param[in] message an MQTT publish message
 *
 * @return true if values were found and copied to the handle;
 * false otherwise
 */
static bool parseJob(handle_t *h, const MQTTPublishInfo_t *message);

/**
 * @brief The libmosquitto callback for a received publish message.
 *
 * @param[in] message an MQTT publish message
 * @param[in] packetIdentifier packet identifier
 *
 * This checks if a message corresponds to a Jobs API, and transitions
 * runtime state based on the API and current state.
 */
static void on_message(MQTTPublishInfo_t *message, uint16_t packetIdentifier);

/**
 * @brief Publish a request to the Jobs service to describe the next job.
 *
 * @param[in] h runtime state handle
 *
 * @return true if libmosquitto accepted the publish message;
 * false otherwise
 *
 * @note This does not call mosquitto_loop(); it expects main() to do so.
 */
static bool sendDescribeNext(handle_t *h);

/**
 * @brief Format a JSON status message.
 *
 * @param[in] x one of "IN_PROGRESS", "SUCCEEDED", or "FAILED"
 */
#define makeReport_(x) "{\"status\":\"" x "\"}"

static handle_t h__, *job_handle = &h__;
/*-----------------------------------------------------------*/

void initHandle(handle_t *p)
{
    assert(p != NULL);

    handle_t h = { 0 };

    h.networkContext.host_name       = AWS_IOT_ENDPOINT;
    h.networkContext.port            = DEFAULT_MQTT_PORT;
    h.networkContext.root_CA         = aws_root_ca1;
    h.networkContext.client_cert     = aws_client_cert;
    h.networkContext.client_key      = aws_client_cert_key;
    h.networkContext.recv_timeout_ms = TRANSPORT_SEND_RECV_TIMEOUT_MS;

    h.name       = THING_NAME;
    h.nameLength = strlen(THING_NAME);

    h.runOnce = false;

    *p = h;
}

/*-----------------------------------------------------------*/

static bool subscribe(handle_t *h, JobsTopic_t api)
{
    bool ret;
    char topic[JOBS_API_MAX_LENGTH(JOBS_THINGNAME_MAX_LENGTH)];
    JobsStatus_t jobs_ret;

    assert(h != NULL);

    /* populate the topic buffer with the specified API for use
     * in a subscription request */
    jobs_ret = Jobs_GetTopic(topic, sizeof(topic), h->name, h->nameLength, api, NULL);
    assert(jobs_ret == JobsSuccess);
    (void)jobs_ret;

    ret = SubscribeToTopic(topic, strlen(topic));

    return ret;
}

/*-----------------------------------------------------------*/
static bool sendUpdate(handle_t *h, const char *jobid, size_t jobidLength, const char *report)
{
    JobsStatus_t jobs_ret;
    char topic[JOBS_API_MAX_LENGTH(JOBS_THINGNAME_MAX_LENGTH)];

    assert(h != NULL);
    assert((jobid != NULL) && (jobidLength > 0));
    assert(report != NULL);

    /* populate the topic buffer for an UpdateJobExecution request */
    jobs_ret = Jobs_Update(topic, sizeof(topic), h->name, h->nameLength, jobid, jobidLength, NULL);
    assert(jobs_ret == JobsSuccess);
    (void)jobs_ret;

    if (!PublishToTopic(topic, strlen(topic), report, strlen(report))) {
        wm_log_error("sendUpdate fail");
        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

static int prvProcessJobDocument(handle_t *h, const char *jobId, size_t jobIdLength, const char *jobDocument,
                                 size_t jobDocumentLength)
{
    JSONStatus_t json_ret;
    char *action        = NULL;
    size_t actionLength = 0;

    json_ret = JSON_Search((char *)jobDocument, jobDocumentLength, "steps[0].action.name", //.action.name
                           (sizeof("steps[0].action.name") - 1), &action, &actionLength);

    if (json_ret == JSONSuccess) {
        if (!strncasecmp(action, "Reboot", actionLength)) {
            sendUpdate(h, jobId, jobIdLength, makeReport_("SUCCEEDED"));

            wm_log_info("recv Reboot, system is about to restart");

            vTaskDelay(1000);
            wm_system_reboot();
        } else {
            wm_log_info("ignore action: %.*s", actionLength, action);
        }
    } else {
        wm_log_info("no action");
    }
    return true;
}

static bool parseJob(handle_t *h, const MQTTPublishInfo_t *message)
{
    JSONStatus_t json_ret;
    int ret     = false;
    char *jobid = NULL, *jobDocument = NULL;
    size_t jobidLength = 0, jobDocumentLength = 0;

    assert(h != NULL);
    assert(message != NULL);
    assert((message->pPayload != NULL) && (message->payloadLength > 0));

    json_ret = JSON_Validate(message->pPayload, message->payloadLength);

    if (json_ret != JSONSuccess) {
        wm_log_error("invalid job document");
    } else {
        json_ret = JSON_Search((char *)message->pPayload, message->payloadLength, "execution.jobId",
                               (sizeof("execution.jobId") - 1), &jobid, &jobidLength);
    }

    if (json_ret == JSONSuccess) {
        json_ret = JSON_Search((char *)message->pPayload, message->payloadLength, "execution.jobDocument",
                               (sizeof("execution.jobDocument") - 1), &jobDocument, &jobDocumentLength);
    } else {
        wm_log_error("invalid job document");
    }

    if (json_ret == JSONSuccess) {
        ret = prvProcessJobDocument(h, jobid, jobidLength, jobDocument, jobDocumentLength);
    }

    if (ret) {
        sendUpdate(h, jobid, jobidLength, makeReport_("SUCCEEDED"));
    } else if (jobid != NULL && jobidLength) {
        sendUpdate(h, jobid, jobidLength, makeReport_("FAILED"));
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void on_message(MQTTPublishInfo_t *message, uint16_t packetIdentifier)
{
    handle_t *h = job_handle;
    JobsStatus_t ret;
    JobsTopic_t api;
    char *jobid;
    uint16_t jobidLength;

    assert(h != NULL);
    assert(message->pTopicName != NULL);

    /* identify a Jobs API and a job ID represented in a topic buffer */
    ret = Jobs_MatchTopic((char *)message->pTopicName, message->topicNameLength, h->name, h->nameLength, &api, &jobid,
                          &jobidLength);

    assert(ret != JobsBadParameter);
    (void)ret;

    switch (api) {
        /* a job has been added or the current job was canceled */
        case JobsNextJobChanged:
        /* response to a request to describe the next job */
        case JobsDescribeSuccess:
        case JobsStartNextSuccess:

            if (!parseJob(h, message)) {
                wm_log_info("unexpected message, topic: %.*s", message->topicNameLength, message->pTopicName);
            }

            break;

        default:
            wm_log_info("ignore topic: %.*s", message->topicNameLength, message->pTopicName);
            break;
    }
}

/*-----------------------------------------------------------*/

static bool sendDescribeNext(handle_t *h)
{
    bool ret = true;
    JobsStatus_t jobs_ret;
    char topic[JOBS_API_MAX_LENGTH(JOBS_THINGNAME_MAX_LENGTH)];

    assert(h != NULL);

    /* populate the topic buffer for a DescribeJobExecution request */
    jobs_ret = Jobs_StartNext(topic, sizeof(topic), h->name, h->nameLength, NULL);

    assert(jobs_ret == JobsSuccess);
    (void)jobs_ret;

    ret = PublishToTopic(topic, strlen(topic), NULL, 0);

    if (!ret) {
        wm_log_error("PublishToTopic fail");
    }

    return ret;
}

/*-----------------------------------------------------------*/

int jobs_demo_main(int argc, char *argv[])
{
    handle_t *h = job_handle;

    initHandle(job_handle);
    do {
        if (EstablishMqttSession(on_message, &h->networkContext) == false) {
            wm_log_error("connect error");
            return EXIT_FAILURE;
        }

        if (subscribe(h, JobsNextJobChanged) == false) {
            wm_log_error("fatal error");
            return EXIT_FAILURE;
        }

        wm_log_info("requesting first job");

        if (sendDescribeNext(h) == false) {
            wm_log_error("fatal error");
            return EXIT_FAILURE;
        }

        while (1) {
            if (!ProcessLoopWithTimeout(1000)) {
                wm_log_error("ProcessLoopWithTimeout error, retry!");
                break;
            }

            vTaskDelay(500U);
        }

        DisconnectMqttSession(&h->networkContext);
    } while (h->runOnce == false);

    return EXIT_SUCCESS;
}
