# AWS Jobs Demo

## Overview

This example is developed based on the official AWS example, mainly to demonstrate how devices can access the AWS platform through the MQTTS protocol. The device will subscribe to JOB - related Topics and execute corresponding instructions according to the JOBs issued by the platform. The specific functions are as follows:

1. The device successfully accesses the AWS server via the MQTTS protocol.
2. The device subscribes to the topic "$aws/things/THING_NAME/jobs/notify-next" on the server, where THING_NAME is the name used when creating an item on the AWS platform.
3. The device publishes an empty message to the server with the topic "$aws/things/THING_NAME/jobs/start-next".
4. The device waits for the server to issue a JOB, and the relevant processing function is implemented in "prvProcessJobDocument".

## Requirements

1. First, you need to complete the item registration operation on the AWS platform. For detailed steps, please refer to: [AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. Configure the root CA certificate. Its storage path is "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. Configure the device certificate. The path is "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. Configure the device certificate key. The path is "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. Device access address: Defined as AWS_IOT_ENDPOINT in aws_user_def.h, and its value is the "iot:Data-ATS" domain name in the AWS console.
6. Things name: "THING_NAME" defined in the "aws_user_def.h" file, which is the item name entered when creating a device in the AWS console.

## Building and Flashing

Example Location：`examples/network/cloud/aws/job/jobs_demo`

compile, burn, and more, see：[Quick Start Guide](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Device Usage Instructions

Run this demo using the following command lines:
1. wifi init: Initialize WiFi.
2. wifi connect ssid password: Replace "ssid" and "password" with the actual available wireless network name and password to connect to an available hotspot.
3. ntp start: Start the network clock.
4. aws_job: Start the AWS demo.
5. Issue a Reboot Job in the AWS console.

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
[D] (3972) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
W800: wifi connect 61test 12345678
connecting, please wait...
[D] (6684) dt: add device rng
[D] (6702) wifi: distribution event 1
[D] (6703) wifi: sta connected 94:83:C4:3A:BA:9E channel 6 authmode 3
[D] (6703) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
sta connected
[D] (7471) wifi: distribution event 0
[D] (7471) wifi: sta got ip 192.168.8.243
W800: ntp start
W800: ntp
NTP is running:
0st sntp server: ntp.sjtu.edu.cn
1st sntp server: cn.ntp.org.cn
2st sntp server: us.pool.ntp.org
[D] (7975) wifi: distribution event 1
[D] (7977) wifi: sta got ip6 FE80::2A6D:CEFF:FE8D:2DDA (index 0)
[D] (12140) dt: add device rtc
Beijing time: 2025-02-10 09:48:35
W800: aws_job
W800: [D] (20307) aws: Establishing a TLS session to a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com:8883.
[D] (20307) aws_mbedtls:
  . Seeding the random number generator...
[D] (20311) aws_mbedtls:  ok
[D] (20311) aws_mbedtls:   . Loading the CA root certificate ...
[D] (20321) aws_mbedtls:  ok (0 skipped)
[D] (20332) aws_mbedtls:  ok (0 skipped)
[D] (20346) aws_mbedtls:  ok (0 skipped)
[D] (20346) aws_mbedtls:   . Connecting to tcp/a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com/8883...
[D] (20679) aws_mbedtls:  ok
[D] (20679) aws_mbedtls:   . Setting up the SSL/TLS structure...
[D] (20687) aws_mbedtls:  ok
[D] (20688) aws_mbedtls:   . Performing the SSL/TLS handshake...
[D] (21028) dt: add device rsa
[D] (23143) aws_mbedtls:  ok
[D] (23143) MQTT: Encoded size for length 92 is 1 bytes.
[D] (23143) MQTT: CONNECT packet remaining length=92 and packet size=94.
[D] (23143) MQTT: CONNECT packet size is 94 and remaining length is 92.
[D] (23145) MQTT: sendMessageVector: Bytes Sent=12, Bytes Remaining=82
[D] (23147) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=80
[D] (23148) MQTT: sendMessageVector: Bytes Sent=10, Bytes Remaining=70
[D] (23149) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=68
[D] (23151) MQTT: sendMessageVector: Bytes Sent=68, Bytes Remaining=0
[D] (23751) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (23751) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (23751) MQTT: Encoded size for length 2 is 1 bytes.
[D] (23751) aws_mbedtls: mbedtls_ssl_read recv 2 bytes
[D] (23751) MQTT: BytesReceived=2, BytesRemaining=0, TotalBytesReceived=2.
[D] (23752) MQTT: Packet received. ReceivedBytes=2.
[D] (23752) MQTT: CONNACK session present bit not set.
[D] (23752) MQTT: Connection accepted.
[D] (23752) MQTT: Received MQTT CONNACK successfully from broker.
[I] (23752) MQTT: MQTT connection established with the broker.
[D] (23752) aws: MQTT connection successfully established with broker.
[D] (23752) aws: A clean MQTT connection is established. Cleaning up all the stored outgoing publishes.
[D] (23752) MQTT: Encoded size for length 43 is 1 bytes.
[D] (23753) MQTT: Subscription packet remaining length=43 and packet size=45.
[D] (23754) MQTT: SUBSCRIBE packet size is 45 and remaining length is 43.
[D] (23756) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=41
[D] (23757) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=39
[D] (23777) MQTT: sendMessageVector: Bytes Sent=38, Bytes Remaining=1
[D] (23778) MQTT: sendMessageVector: Bytes Sent=1, Bytes Remaining=0
[D] (23778) aws: SUBSCRIBE topic $aws/things/wm_demo_1/jobs/notify-next to broker.
[D] (24469) aws_mbedtls: mbedtls_ssl_read recv 5 bytes
[D] (24469) MQTT: Encoded size for length 3 is 1 bytes.
[D] (24470) MQTT: Received packet of type 90.
[D] (24470) MQTT: Packet identifier 1.
[D] (24470) MQTT: Topic filter 0 accepted, max QoS 1.
[D] (24470) aws: MQTT Packet type SUBACK received.
[I] (24470) aws: requesting first job
[D] (24470) aws: Published payload:
[D] (24470) MQTT: Encoded size for length 41 is 1 bytes.
[D] (24471) MQTT: Encoded size for length 41 is 1 bytes.
[D] (24471) MQTT: PUBLISH packet remaining length=41 and packet size=43.
[D] (24471) MQTT: Encoded size for length 41 is 1 bytes.
[D] (24471) MQTT: Adding QoS as QoS1 in PUBLISH flags.
[D] (24473) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=39
[D] (24475) MQTT: sendMessageVector: Bytes Sent=37, Bytes Remaining=2
[D] (24476) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=0
[D] (24476) aws: PUBLISH sent for topic $aws/things/wm_demo_1/jobs/start-next to broker with packet ID 2.
[D] (25067) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (25068) MQTT: Encoded size for length 2 is 1 bytes.
[D] (25068) MQTT: Received packet of type 40.
[D] (25068) MQTT: Packet identifier 2.
[I] (25068) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (25071) MQTT: State record updated. New state=MQTTPublishDone.
[D] (25071) aws: PUBACK received for packet id 2.
[D] (25071) aws: Cleaned up outgoing publish packet with packet id 2.
[D] (25097) aws_mbedtls: mbedtls_ssl_read recv 1887 bytes
[D] (25097) MQTT: Encoded size for length 1884 is 2 bytes.
[D] (25097) MQTT: QoS is 0.
[D] (25097) MQTT: Retain bit is 0.
[D] (25098) MQTT: DUP bit is 0.
[D] (25098) MQTT: Topic name length: 46.
[D] (25098) MQTT: Payload length 1836.
[I] (25098) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (25098) MQTT: State record updated. New state=MQTTPublishDone.
[I] (25103) aws: no action
[D] (25103) aws: Published payload: {"status":"SUCCEEDED"}
[D] (25103) MQTT: Encoded size for length 69 is 1 bytes.
[D] (25103) MQTT: Encoded size for length 69 is 1 bytes.
[D] (25103) MQTT: PUBLISH packet remaining length=69 and packet size=71.
[D] (25104) MQTT: Encoded size for length 69 is 1 bytes.
[D] (25107) MQTT: Adding QoS as QoS1 in PUBLISH flags.
[D] (25109) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=67
[D] (25111) MQTT: sendMessageVector: Bytes Sent=43, Bytes Remaining=24
[D] (25112) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=22
[D] (25130) MQTT: sendMessageVector: Bytes Sent=22, Bytes Remaining=0
[D] (25130) aws: PUBLISH sent for topic $aws/things/wm_demo_1/jobs/AFR_OTA-3/update to broker with packet ID 3.
[D] (25700) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (25701) MQTT: Encoded size for length 2 is 1 bytes.
[D] (25701) MQTT: Received packet of type 40.
[D] (25701) MQTT: Packet identifier 3.
[I] (25701) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (25701) MQTT: State record updated. New state=MQTTPublishDone.
[D] (25701) aws: PUBACK received for packet id 3.
[D] (25701) aws: Cleaned up outgoing publish packet with packet id 3.
[D] (25701) aws: MQTT_ProcessLoop successful.
[D] (26204) aws_mbedtls: mbedtls_ssl_read recv 80 bytes
[D] (26204) MQTT: Encoded size for length 78 is 1 bytes.
[D] (26205) MQTT: QoS is 0.
[D] (26205) MQTT: Retain bit is 0.
[D] (26205) MQTT: DUP bit is 0.
[D] (26205) MQTT: Topic name length: 52.
[D] (26205) MQTT: Payload length 24.
[I] (26205) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (26205) MQTT: State record updated. New state=MQTTPublishDone.
[I] (26205) aws: ignore topic: $aws/things/wm_demo_1/jobs/AFR_OTA-3/update/accepted
[D] (27337) aws_mbedtls: mbedtls_ssl_read recv 68 bytes
[D] (27337) MQTT: Encoded size for length 66 is 1 bytes.
[D] (27338) MQTT: QoS is 1.
[D] (27338) MQTT: Retain bit is 0.
[D] (27338) MQTT: DUP bit is 0.
[D] (27338) MQTT: Topic name length: 38.
[D] (27338) MQTT: Packet identifier 1.
[D] (27338) MQTT: Payload length 24.
[I] (27338) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (27338) MQTT: State record updated. New state=MQTTPubAckSend.
[E] (27339) aws: invalid job document
[I] (27339) aws: unexpected message, topic: $aws/things/wm_demo_1/jobs/notify-next
[D] (27341) MQTT: sendBuffer: Bytes Sent=4, Bytes Remaining=0
[D] (27341) aws: MQTT_ProcessLoop successful.
[D] (37841) aws: MQTT_ProcessLoop successful.
[D] (48341) aws: MQTT_ProcessLoop successful.
[D] (247892) MQTT: MQTT PINGREQ packet size is 2.
[D] (247899) MQTT: sendBuffer: Bytes Sent=2, Bytes Remaining=0
[D] (247899) MQTT: Sent 2 bytes of PINGREQ packet.
[D] (247899) aws: MQTT_ProcessLoop successful.
[D] (248401) aws_mbedtls: mbedtls_ssl_read recv 2 bytes
[D] (248401) MQTT: Encoded size for length 0 is 1 bytes.
[D] (248401) MQTT: Received packet of type d0.
[D] (255387) aws_mbedtls: mbedtls_ssl_read recv 353 bytes
[D] (255388) MQTT: Encoded size for length 350 is 2 bytes.
[D] (255388) MQTT: QoS is 1.
[D] (255388) MQTT: Retain bit is 0.
[D] (255388) MQTT: DUP bit is 0.
[D] (255388) MQTT: Topic name length: 38.
[D] (255388) MQTT: Packet identifier 1.
[D] (255388) MQTT: Payload length 308.
[I] (255388) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (255388) MQTT: State record updated. New state=MQTTPubAckSend.
[D] (255390) aws: Published payload: {"status":"SUCCEEDED"}
[D] (255390) MQTT: Encoded size for length 66 is 1 bytes.
[D] (255391) MQTT: Encoded size for length 66 is 1 bytes.
[D] (255391) MQTT: PUBLISH packet remaining length=66 and packet size=68.
[D] (255391) MQTT: Encoded size for length 66 is 1 bytes.
[D] (255391) MQTT: Adding QoS as QoS1 in PUBLISH flags.
[D] (255393) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=64
[D] (255395) MQTT: sendMessageVector: Bytes Sent=40, Bytes Remaining=24
[D] (255396) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=22
[D] (255415) MQTT: sendMessageVector: Bytes Sent=22, Bytes Remaining=0
[D] (255416) aws: PUBLISH sent for topic $aws/things/wm_demo_1/jobs/reboot/update to broker with packet ID 4.
[I] (255416) aws: recv Reboot, system is about to restart
[D] (0) dt: add device rcc
[D] (0) dt: add device pmu
[D] (1) dt: add device iflash
[D] (1) ptb: partition table base 0xe000
[D] (2) dt: add device hash
[D] (2) ft: ft_0 base 0, ft_1 base 1000
[D] (2) dt: add device crc
[D] (3) ft: ft init 1
[D] (6) nvs_sm: last sn=2
[D] (6) nvs_sm: active sec_id=0,sn=1
[D] (6) nvs_sm: active sec_id=1,sn=2
[D] (7) nvs_sm: sec load end
[D] (9) nvs_store: group load end
[I] (12) main: ver: 2.3-alpha.2 build at Feb 10 2025 09:45:41
[I] (12) main: boot reason 4
[I] (12) main: heap size 259440(253.36KB)
[I] (13) main: flash size 0x200000(2MB)
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
```
