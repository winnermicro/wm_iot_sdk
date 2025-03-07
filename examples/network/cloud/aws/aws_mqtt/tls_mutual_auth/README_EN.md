# AWS MQTT Mutual Auth Demo

## Overview

This example is developed based on the official AWS example and is mainly used to demonstrate how devices can access the AWS platform through the MQTTS protocol. The specific functions are as follows:

1. The device successfully accesses the AWS server via the MQTTS protocol.
2. The device subscribes to a topic named MQTT_EXAMPLE_TOPIC on the server. This topic parameter can be flexibly defined in the mqtt_demo_mutual_auth.c file to meet the requirements of different application scenarios.
3. The device publishes the MQTT_EXAMPLE_MESSAGE message to the MQTT_EXAMPLE_TOPIC, realizing information interaction between the device and the server.

## Requirements

1. First, you need to complete the item registration operation on the AWS platform. For detailed steps, please refer to: [AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. Configure the root CA certificate. Its storage path is "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. Configure the device certificate. The path is "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. Configure the device certificate key. The path is "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. Device access address: Defined as AWS_IOT_ENDPOINT in aws_user_def.h, and its value is the "iot:Data-ATS" domain name in the AWS console.

## Building and Flashing

Example Location：`examples/network/cloud/aws/aws_mqtt/tls_mutual_auth`

compile, burn, and more, see：[Quick Start Guide](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Device Usage Instructions

Run this demo using the following command lines:
1. wifi init: Initialize WiFi.
2. wifi connect ssid password: Replace "ssid" and "password" with the actual available wireless network name and password to connect to an available hotspot.
3. ntp start: Start the network clock.
4. aws_mqtt: Start the AWS demo.

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
[D] (5916) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
[D] (5937) main: GetMAC From NVS (Type=1) 02:25:08:09:01:0f
W800:
W800:
W800:
W800: wifi connect 61test 12345678
connecting, please wait...
[D] (8680) dt: add device rng
[D] (8701) wifi: distribution event 1
[D] (8701) wifi: sta connected 94:83:C4:3A:BA:9E channel 6 authmode 3
[D] (8702) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
sta connected
[D] (9421) wifi: distribution event 0
[D] (9421) wifi: sta got ip 192.168.8.243
W800: ntp start
W800: ntp
NTP is running:
0st sntp server: ntp.sjtu.edu.cn
1st sntp server: cn.ntp.org.cn
2st sntp server: us.pool.ntp.org
[D] (9921) wifi: distribution event 1
[D] (9921) wifi: sta got ip6 FE80::2A6D:CEFF:FE8D:2DDA (index 0)
[D] (14147) dt: add device rtc
Beijing time: 2025-02-10 10:29:18
W800: aws_mqtt
W800:
W800: [I] (16369) DEMO: Establishing a TLS session to a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com:8883.
[D] (16370) aws_mbedtls:
  . Seeding the random number generator...
[D] (16373) aws_mbedtls:  ok
[D] (16373) aws_mbedtls:   . Loading the CA root certificate ...
[D] (16384) aws_mbedtls:  ok (0 skipped)
[D] (16394) aws_mbedtls:  ok (0 skipped)
[D] (16414) aws_mbedtls:  ok (0 skipped)
[D] (16415) aws_mbedtls:   . Connecting to tcp/a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com/8883...
[D] (17124) aws_mbedtls:  ok
[D] (17125) aws_mbedtls:   . Setting up the SSL/TLS structure...
[D] (17133) aws_mbedtls:  ok
[D] (17133) aws_mbedtls:   . Performing the SSL/TLS handshake...
[D] (17451) dt: add device rsa
[D] (20611) aws_mbedtls:  ok
[D] (20611) MQTT: Encoded size for length 92 is 1 bytes.
[D] (20611) MQTT: CONNECT packet remaining length=92 and packet size=94.
[D] (20611) MQTT: CONNECT packet size is 94 and remaining length is 92.
[D] (20613) MQTT: sendMessageVector: Bytes Sent=12, Bytes Remaining=82
[D] (20615) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=80
[D] (20616) MQTT: sendMessageVector: Bytes Sent=10, Bytes Remaining=70
[D] (20617) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=68
[D] (20618) MQTT: sendMessageVector: Bytes Sent=68, Bytes Remaining=0
[D] (21221) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (21222) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (21222) MQTT: Encoded size for length 2 is 1 bytes.
[D] (21222) aws_mbedtls: mbedtls_ssl_read recv 2 bytes
[D] (21222) MQTT: BytesReceived=2, BytesRemaining=0, TotalBytesReceived=2.
[D] (21222) MQTT: Packet received. ReceivedBytes=2.
[D] (21222) MQTT: CONNACK session present bit not set.
[D] (21222) MQTT: Connection accepted.
[D] (21222) MQTT: Received MQTT CONNACK successfully from broker.
[I] (21222) MQTT: MQTT connection established with the broker.
[I] (21223) DEMO: MQTT connection successfully established with broker.
[I] (21223) DEMO: A clean MQTT connection is established. Cleaning up all the stored outgoing publishes.
[I] (21223) DEMO: Subscribing to the MQTT topic testclient/example/topic.
[D] (21224) MQTT: Encoded size for length 29 is 1 bytes.
[D] (21224) MQTT: Subscription packet remaining length=29 and packet size=31.
[D] (21224) MQTT: SUBSCRIBE packet size is 31 and remaining length is 29.
[D] (21226) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=27
[D] (21247) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=25
[D] (21248) MQTT: sendMessageVector: Bytes Sent=24, Bytes Remaining=1
[D] (21249) MQTT: sendMessageVector: Bytes Sent=1, Bytes Remaining=0
[I] (21249) DEMO: SUBSCRIBE sent for topic testclient/example/topic to broker.
[D] (21835) aws_mbedtls: mbedtls_ssl_read recv 5 bytes
[D] (21836) MQTT: Encoded size for length 3 is 1 bytes.
[D] (21836) MQTT: Received packet of type 90.
[D] (21836) MQTT: Packet identifier 1.
[D] (21836) MQTT: Topic filter 0 accepted, max QoS 1.
[I] (21836) DEMO: Subscribed to the topic testclient/example/topic. with maximum QoS 1.
[I] (21836) DEMO: Sending Publish to the MQTT topic testclient/example/topic.
[D] (21837) MQTT: Encoded size for length 40 is 1 bytes.
[D] (21837) MQTT: Encoded size for length 40 is 1 bytes.
[D] (21837) MQTT: PUBLISH packet remaining length=40 and packet size=42.
[D] (21837) MQTT: Encoded size for length 40 is 1 bytes.
[D] (21837) MQTT: Adding QoS as QoS1 in PUBLISH flags.
[D] (21839) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=38
[D] (21840) MQTT: sendMessageVector: Bytes Sent=24, Bytes Remaining=14
[D] (21842) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=12
[D] (21842) MQTT: sendMessageVector: Bytes Sent=12, Bytes Remaining=0
[I] (21843) DEMO: PUBLISH sent for topic testclient/example/topic to broker with packet ID 2.
[D] (22449) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (22450) MQTT: Encoded size for length 2 is 1 bytes.
[D] (22450) MQTT: Received packet of type 40.
[D] (22450) MQTT: Packet identifier 2.
[I] (22450) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (22450) MQTT: State record updated. New state=MQTTPublishDone.
[I] (22450) DEMO: PUBACK received for packet id 2.
[I] (22450) DEMO: Cleaned up outgoing publish packet with packet id 2.
[D] (22471) aws_mbedtls: mbedtls_ssl_read recv 42 bytes
[D] (22472) MQTT: Encoded size for length 40 is 1 bytes.
[D] (22472) MQTT: QoS is 1.
[D] (22472) MQTT: Retain bit is 0.
[D] (22472) MQTT: DUP bit is 0.
[D] (22472) MQTT: Topic name length: 24.
[D] (22472) MQTT: Packet identifier 1.
[D] (22472) MQTT: Payload length 12.
[I] (22472) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (22473) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (22473) DEMO: Incoming QOS : 1.
[I] (22473) DEMO: Incoming Publish Topic Name: testclient/example/topic matches subscribed topic.
Incoming Publish message Packet Id is 1.
Incoming Publish Message : Hello World!.
[D] (22476) MQTT: sendBuffer: Bytes Sent=4, Bytes Remaining=0
```
