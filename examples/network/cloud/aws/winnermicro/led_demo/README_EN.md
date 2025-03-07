# AWS Winnermicro LED Demo

## Overview

This example is developed by referring to the official AWS shadow example. It mainly demonstrates how a device can access the AWS platform through the MQTTS protocol. By subscribing to shadow - related topics, the device can report the status of its LED and simulated temperature sensor, and execute commands issued by the platform. The specific functions are as follows:

1. The device successfully connects to the AWS server through the MQTTS protocol.
2. The device publishes an empty message to AWS with the topic "$aws/things/THING_NAME/shadow/delete/accepted". Here, THING_NAME represents the item name, and the meaning of THING_NAME mentioned below is the same. This operation is aimed at deleting the shadow device to prepare for subsequent operations.
3. The device publishes a message to AWS with the topic "$aws/things/THING_NAME/shadow/update", which is used to create a shadow device. The message is assembled by the aws_device_update function. Users can customize the message content by modifying or expanding the s_dev_attr variable.
4. The device continuously monitors messages from the AWS platform. After receiving a message, it parses the message and realizes the processing ability of the messages issued by the platform through the function pointers filled in the s_dev_attr by the user.

## Requirements

1. First, you need to complete the item registration operation on the AWS platform. For detailed steps, please refer to: [AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. Configure the root CA certificate. Its storage path is "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. Configure the device certificate. The path is "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. Configure the device certificate key. The path is "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. Device access address: Defined as AWS_IOT_ENDPOINT in aws_user_def.h, and its value is the "iot:Data-ATS" domain name in the AWS console.

## Building and Flashing

Example Location：`examples/network/cloud/aws/winnermicro/led_demo`

compile, burn, and more, see：[Quick Start Guide](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Device Usage Instructions

Run this demo using the following command lines:
1. wifi init: Initialize WiFi.
2. wifi connect ssid password: Replace "ssid" and "password" with the actual available wireless network name and password to connect to an available hotspot.
3. ntp start: Start the network clock.
4. aws_led: Start the AWS demo.

## Remote Control

Users can use the MQTT.fx tool to achieve remote control. The specific steps are as follows:
1. Configure server information:
    + Broker Address: The same as the AWS_IOT_ENDPOINT address, Broker Port: 8883.
2. Configure SSL/TLS: Select SSL/TLS and enable this option. At the same time, configure the following file paths:
    + CA File: wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem
    + Client Certificate File: wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt
    + Client Key File: wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key
3. Subscribe to the topic: Subscribe: $aws/things/THING_NAME/shadow/update/accepted. After subscribing to this topic, you can receive the messages reported by the device.
4. Publish messages: Publish: $aws/things/THING_NAME/shadow/update. Publishing messages can pass commands to the device. The message format is as follows. LED supports "blue", "green", and "red". Data format: {"state":{"reported":{"led":"blue"}}}

## Running Result

Upon successful execution, it will output the following logs

```
W800:
 ===================================================
 |       Steps to connect to AWS                   |
 |                                                 |
 |  1. Print 'help' to gain overview of commands   |
 |  2. Configure device to station                 |
 |  3. Setup WiFi connection                       |
 |  4. Run aws to test AWS function                |
 |                                                 |
 ===================================================
wifi init
W800:
W800:
W800:
W800: wifi connect 61test 12345678
connecting, please wait...
sta connected
W800: ntp start
W800: ntp
NTP is running:
0st sntp server: ntp.sjtu.edu.cn
1st sntp server: cn.ntp.org.cn
2st sntp server: us.pool.ntp.org
Beijing time: 2025-02-10 14:20:07
W800: aws_led
W800:
W800: [I] (28435) SHADOW_DEMO: Establishing a TLS session to a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com:8883.
[I] (30909) MQTT: MQTT connection established with the broker.
[I] (30909) SHADOW_DEMO: MQTT connection successfully established with broker.
[I] (30909) SHADOW_DEMO: An MQTT session with broker is re-established. Resending unacked publishes.
[I] (30912) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/delete/accepted to broker.
[I] (31502) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[I] (31506) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/delete/rejected to broker.
[I] (32200) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[I] (32203) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/update/delta to broker.
[I] (32833) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[I] (32836) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/update/accepted to broker.
[I] (33549) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[I] (33552) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/update/rejected to broker.
[I] (34511) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[I] (34512) SHADOW_DEMO: Published Topic: $aws/things/wm_demo_1/shadow/delete
[I] (34512) SHADOW_DEMO: Published payload:
[I] (34514) SHADOW_DEMO: PUBLISH sent for topic $aws/things/wm_demo_1/shadow/delete to broker with packet ID 6.
[I] (35299) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (35299) MQTT: State record updated. New state=MQTTPublishDone.
[I] (35299) SHADOW_DEMO: PUBACK received for packet id 6.
[I] (35299) SHADOW_DEMO: Cleaned up outgoing publish packet with packet id 6.
[I] (35300) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (35301) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (35301) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/delete/accepted.
[I] (35301) SHADOW: Received an MQTT incoming publish on /delete/accepted topic.
[I] (45302) SHADOW: Demo iteration 0 is successful.
[I] (45302) SHADOW_DEMO: Published Topic: $aws/things/wm_demo_1/shadow/update
[I] (45302) SHADOW_DEMO: Published payload: {
        "state":        {
                "reported":     {
                        "led":  "red",
                        "temperature_sensor":   "0.0"
                }
        },
        "clientToken":  "654718"
}
[I] (45306) SHADOW_DEMO: PUBLISH sent for topic $aws/things/wm_demo_1/shadow/update to broker with packet ID 7.
[I] (45958) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (45958) MQTT: State record updated. New state=MQTTPublishDone.
[I] (45958) SHADOW_DEMO: PUBACK received for packet id 7.
[I] (45958) SHADOW_DEMO: Cleaned up outgoing publish packet with packet id 7.
[I] (45961) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (45962) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (45962) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/update/accepted.
[I] (45962) SHADOW: /update/accepted json payload:{"state":{"reported":{"led":"red","temperature_sensor":"0.0"}},"metadata":{"reported":{"led":{"timestamp":1739168442},"temperature_sensor":{"timestamp":1739168442}}},"version":69,"timestamp":1739168442,"clientToken":"654718"}.
[W] (45963) aws_device: not found temperature_sensor set_fun
[I] (62940) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (62940) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (62940) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/update/accepted.
[I] (62940) SHADOW: /update/accepted json payload:{"state":{"reported":{"led":"green"}},"metadata":{"reported":{"led":{"timestamp":1739168459}}},"version":70,"timestamp":1739168459}.
[I] (66288) temp_sensor: user_key pressed
[I] (72953) SHADOW_DEMO: Published Topic: $aws/things/wm_demo_1/shadow/update
[I] (72953) SHADOW_DEMO: Published payload: {
        "state":        {
                "reported":     {
                        "temperature_sensor":   "5.0"
                }
        },
        "clientToken":  "675704"
}
[I] (72960) SHADOW_DEMO: PUBLISH sent for topic $aws/things/wm_demo_1/shadow/update to broker with packet ID 8.
[I] (73568) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (73569) MQTT: State record updated. New state=MQTTPublishDone.
[I] (73569) SHADOW_DEMO: PUBACK received for packet id 8.
[I] (73569) SHADOW_DEMO: Cleaned up outgoing publish packet with packet id 8.
[I] (73629) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (73630) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (73630) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/update/accepted.
[I] (73630) SHADOW: /update/accepted json payload:{"state":{"reported":{"temperature_sensor":"5.0"}},"metadata":{"reported":{"temperature_sensor":{"timestamp":1739168470}}},"version":71,"timestamp":1739168470,"clientToken":"675704"}.
[W] (73631) aws_device: not found temperature_sensor set_fun
```
