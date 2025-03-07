# AWS Shadow Demo

## 功能概述

本示例基于 AWS 官方示例进行开发，主要目的在于展示设备通过MQTTS协议接入AWS平台，订阅shadow相关topic，上报本地参数信息，并接收平台下发消息。具体实现的功能如下：

1. 设备借助 MQTTS 协议成功接入 AWS 服务器。
2. 设备向AWS发布空消息，topic为“$aws/things/THING_NAME/shadow/delete/accepted”，用以删除影子设备，其中 THING_NAME 代表物品名称，下文提及的 THING_NAME 含义相同。
3. 向AWS发布消息，topic为“$aws/things/THING_NAME/shadow/update”，用以创建影子设备，消息格式预定义在SHADOW_DESIRED_JSON，用户可以根据实际情况修改

## 环境要求

1. 首先，需要在 AWS 平台完成物品注册操作，详细步骤请参考：[AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. 配置 root CA 证书，其存放路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. 配置设备证书，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. 配置设备证书密钥，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. 设备接入地址：定义在 aws_user_def.h 的 AWS_IOT_ENDPOINT，其值为 AWS 控制台中的 “iot:Data-ATS” 域名。
6. 物品名称：在 “aws_user_def.h” 文件中定义的 “THING_NAME”，即 AWS 控制台创建设备时输入的物品名称。

## 编译和烧录

示例位置：`examples/network/cloud/aws/shadow/shadow_demo_main`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 设备使用说明

使用如下命令行运行此 demo：
1. wifi init： 初始化 WiFi
2. wifi connect ssid password：将 “ssid” 和 “password” 替换为实际可用的无线网络名称和密码，以此连接到可用热点。
3. ntp start：启动网络时钟。
4. aws_shadow：启动 AWS demo。

## 运行结果

成功运行将输出如下日志

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
[D] (3281) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
W800:
W800:
W800:
W800: wifi connect 61test 12345678
connecting, please wait...
[D] (5984) dt: add device rng
[D] (6006) wifi: distribution event 1
[D] (6007) wifi: sta connected 94:83:C4:3A:BA:9E channel 6 authmode 3
[D] (6008) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
sta connected
[D] (6756) wifi: distribution event 0
[D] (6756) wifi: sta got ip 192.168.8.243
W800: ntp start
W800: ntp
NTP is running:
0st sntp server: ntp.sjtu.edu.cn
1st sntp server: cn.ntp.org.cn
2st sntp server: us.pool.ntp.org
[D] (7280) wifi: distribution event 1
[D] (7281) wifi: sta got ip6 FE80::2A6D:CEFF:FE8D:2DDA (index 0)
[D] (11428) dt: add device rtc
Beijing time: 2025-02-10 13:59:37
W800: aws_shadow
W800:
W800: [I] (76625) SHADOW_DEMO: Establishing a TLS session to a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com:8883.
[D] (76625) aws_mbedtls:
  . Seeding the random number generator...
[D] (76629) aws_mbedtls:  ok
[D] (76629) aws_mbedtls:   . Loading the CA root certificate ...
[D] (76639) aws_mbedtls:  ok (0 skipped)
[D] (76650) aws_mbedtls:  ok (0 skipped)
[D] (76664) aws_mbedtls:  ok (0 skipped)
[D] (76665) aws_mbedtls:   . Connecting to tcp/a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com/8883...
[D] (77033) aws_mbedtls:  ok
[D] (77034) aws_mbedtls:   . Setting up the SSL/TLS structure...
[D] (77042) aws_mbedtls:  ok
[D] (77043) aws_mbedtls:   . Performing the SSL/TLS handshake...
[D] (77371) dt: add device rsa
[D] (79478) aws_mbedtls:  ok
[D] (79479) MQTT: Encoded size for length 91 is 1 bytes.
[D] (79479) MQTT: CONNECT packet remaining length=91 and packet size=93.
[D] (79479) MQTT: CONNECT packet size is 93 and remaining length is 91.
[D] (79481) MQTT: sendMessageVector: Bytes Sent=12, Bytes Remaining=81
[D] (79482) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=79
[D] (79483) MQTT: sendMessageVector: Bytes Sent=9, Bytes Remaining=70
[D] (79484) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=68
[D] (79486) MQTT: sendMessageVector: Bytes Sent=68, Bytes Remaining=0
[D] (80042) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (80042) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (80043) MQTT: Encoded size for length 2 is 1 bytes.
[D] (80043) aws_mbedtls: mbedtls_ssl_read recv 2 bytes
[D] (80043) MQTT: BytesReceived=2, BytesRemaining=0, TotalBytesReceived=2.
[D] (80043) MQTT: Packet received. ReceivedBytes=2.
[D] (80043) MQTT: CONNACK session present bit not set.
[D] (80043) MQTT: Connection accepted.
[D] (80043) MQTT: Received MQTT CONNACK successfully from broker.
[I] (80043) MQTT: MQTT connection established with the broker.
[I] (80043) SHADOW_DEMO: MQTT connection successfully established with broker.
[I] (80043) SHADOW_DEMO: A clean MQTT connection is established. Cleaning up all the stored outgoing publishes.
[D] (80044) MQTT: Encoded size for length 49 is 1 bytes.
[D] (80045) MQTT: Subscription packet remaining length=49 and packet size=51.
[D] (80045) MQTT: SUBSCRIBE packet size is 51 and remaining length is 49.
[D] (80047) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=47
[D] (80048) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=45
[D] (80068) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=1
[D] (80069) MQTT: sendMessageVector: Bytes Sent=1, Bytes Remaining=0
[I] (80069) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/delete/accepted to broker.
[D] (80807) aws_mbedtls: mbedtls_ssl_read recv 5 bytes
[D] (80808) MQTT: Encoded size for length 3 is 1 bytes.
[D] (80808) MQTT: Received packet of type 90.
[D] (80808) MQTT: Packet identifier 1.
[D] (80808) MQTT: Topic filter 0 accepted, max QoS 1.
[I] (80808) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[D] (80808) MQTT: Encoded size for length 49 is 1 bytes.
[D] (80809) MQTT: Subscription packet remaining length=49 and packet size=51.
[D] (80809) MQTT: SUBSCRIBE packet size is 51 and remaining length is 49.
[D] (80811) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=47
[D] (80812) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=45
[D] (80813) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=1
[D] (80814) MQTT: sendMessageVector: Bytes Sent=1, Bytes Remaining=0
[I] (80815) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/delete/rejected to broker.
[D] (81421) aws_mbedtls: mbedtls_ssl_read recv 5 bytes
[D] (81421) MQTT: Encoded size for length 3 is 1 bytes.
[D] (81421) MQTT: Received packet of type 90.
[D] (81421) MQTT: Packet identifier 2.
[D] (81422) MQTT: Topic filter 0 accepted, max QoS 1.
[I] (81422) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[I] (81422) SHADOW_DEMO: Published Topic: $aws/things/wm_demo_1/shadow/delete
[I] (81422) SHADOW_DEMO: Published payload:
[D] (81422) MQTT: Encoded size for length 39 is 1 bytes.
[D] (81422) MQTT: Encoded size for length 39 is 1 bytes.
[D] (81422) MQTT: PUBLISH packet remaining length=39 and packet size=41.
[D] (81422) MQTT: Encoded size for length 39 is 1 bytes.
[D] (81422) MQTT: Adding QoS as QoS1 in PUBLISH flags.
[D] (81424) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=37
[D] (81426) MQTT: sendMessageVector: Bytes Sent=35, Bytes Remaining=2
[D] (81427) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=0
[I] (81427) SHADOW_DEMO: PUBLISH sent for topic $aws/things/wm_demo_1/shadow/delete to broker with packet ID 3.
[D] (82053) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (82054) MQTT: Encoded size for length 2 is 1 bytes.
[D] (82054) MQTT: Received packet of type 40.
[D] (82054) MQTT: Packet identifier 3.
[I] (82054) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (82054) MQTT: State record updated. New state=MQTTPublishDone.
[I] (82054) SHADOW_DEMO: PUBACK received for packet id 3.
[I] (82054) SHADOW_DEMO: Cleaned up outgoing publish packet with packet id 3.
[D] (82056) aws_mbedtls: mbedtls_ssl_read recv 87 bytes
[D] (82057) MQTT: Encoded size for length 85 is 1 bytes.
[D] (82057) MQTT: QoS is 1.
[D] (82057) MQTT: Retain bit is 0.
[D] (82057) MQTT: DUP bit is 0.
[D] (82057) MQTT: Topic name length: 44.
[D] (82057) MQTT: Packet identifier 1.
[D] (82057) MQTT: Payload length 37.
[I] (82057) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (82058) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (82058) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/delete/accepted.
[I] (82058) SHADOW: Received an MQTT incoming publish on /delete/accepted topic.
[D] (82080) MQTT: sendBuffer: Bytes Sent=4, Bytes Remaining=0
[D] (92080) MQTT: Encoded size for length 48 is 1 bytes.
[D] (92080) MQTT: Subscription packet remaining length=48 and packet size=50.
[D] (92080) MQTT: UNSUBSCRIBE packet size is 50 and remaining length is 48.
[D] (92087) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=46
[D] (92089) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=44
[D] (92090) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=0
[I] (92090) SHADOW_DEMO: UNSUBSCRIBE sent topic $aws/things/wm_demo_1/shadow/delete/accepted to broker.
[D] (92714) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (92715) MQTT: Encoded size for length 2 is 1 bytes.
[D] (92715) MQTT: Received packet of type b0.
[D] (92715) MQTT: Packet identifier 4.
[I] (92715) SHADOW_DEMO: MQTT_PACKET_TYPE_UNSUBACK.
[D] (92715) MQTT: Encoded size for length 48 is 1 bytes.
[D] (92715) MQTT: Subscription packet remaining length=48 and packet size=50.
[D] (92716) MQTT: UNSUBSCRIBE packet size is 50 and remaining length is 48.
[D] (92718) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=46
[D] (92719) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=44
[D] (92720) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=0
[I] (92721) SHADOW_DEMO: UNSUBSCRIBE sent topic $aws/things/wm_demo_1/shadow/delete/rejected to broker.
[D] (93301) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (93301) MQTT: Encoded size for length 2 is 1 bytes.
[D] (93301) MQTT: Received packet of type b0.
[D] (93301) MQTT: Packet identifier 5.
[I] (93301) SHADOW_DEMO: MQTT_PACKET_TYPE_UNSUBACK.
[D] (93302) MQTT: Encoded size for length 46 is 1 bytes.
[D] (93302) MQTT: Subscription packet remaining length=46 and packet size=48.
[D] (93302) MQTT: SUBSCRIBE packet size is 48 and remaining length is 46.
[D] (93304) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=44
[D] (93305) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=42
[D] (93307) MQTT: sendMessageVector: Bytes Sent=41, Bytes Remaining=1
[D] (93308) MQTT: sendMessageVector: Bytes Sent=1, Bytes Remaining=0
[I] (93308) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/update/delta to broker.
[D] (93912) aws_mbedtls: mbedtls_ssl_read recv 5 bytes
[D] (93913) MQTT: Encoded size for length 3 is 1 bytes.
[D] (93913) MQTT: Received packet of type 90.
[D] (93913) MQTT: Packet identifier 6.
[D] (93913) MQTT: Topic filter 0 accepted, max QoS 1.
[I] (93913) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[D] (93913) MQTT: Encoded size for length 49 is 1 bytes.
[D] (93913) MQTT: Subscription packet remaining length=49 and packet size=51.
[D] (93914) MQTT: SUBSCRIBE packet size is 51 and remaining length is 49.
[D] (93916) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=47
[D] (93917) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=45
[D] (93918) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=1
[D] (93919) MQTT: sendMessageVector: Bytes Sent=1, Bytes Remaining=0
[I] (93920) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/update/accepted to broker.
[D] (94529) aws_mbedtls: mbedtls_ssl_read recv 5 bytes
[D] (94529) MQTT: Encoded size for length 3 is 1 bytes.
[D] (94529) MQTT: Received packet of type 90.
[D] (94529) MQTT: Packet identifier 7.
[D] (94530) MQTT: Topic filter 0 accepted, max QoS 1.
[I] (94530) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[D] (94530) MQTT: Encoded size for length 49 is 1 bytes.
[D] (94530) MQTT: Subscription packet remaining length=49 and packet size=51.
[D] (94530) MQTT: SUBSCRIBE packet size is 51 and remaining length is 49.
[D] (94532) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=47
[D] (94534) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=45
[D] (94535) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=1
[D] (94536) MQTT: sendMessageVector: Bytes Sent=1, Bytes Remaining=0
[I] (94536) SHADOW_DEMO: SUBSCRIBE topic $aws/things/wm_demo_1/shadow/update/rejected to broker.
[D] (95139) aws_mbedtls: mbedtls_ssl_read recv 5 bytes
[D] (95140) MQTT: Encoded size for length 3 is 1 bytes.
[D] (95140) MQTT: Received packet of type 90.
[D] (95140) MQTT: Packet identifier 8.
[D] (95140) MQTT: Topic filter 0 accepted, max QoS 1.
[I] (95140) SHADOW_DEMO: MQTT_PACKET_TYPE_SUBACK.
[I] (95140) SHADOW: Send desired power state with 1.
[I] (95141) SHADOW_DEMO: Published Topic: $aws/things/wm_demo_1/shadow/update
[I] (95141) SHADOW_DEMO: Published payload: {"state":{"desired":{"powerOn":1}},"clientToken":"473557"}
[D] (95141) MQTT: Encoded size for length 98 is 1 bytes.
[D] (95141) MQTT: Encoded size for length 98 is 1 bytes.
[D] (95141) MQTT: PUBLISH packet remaining length=98 and packet size=100.
[D] (95141) MQTT: Encoded size for length 98 is 1 bytes.
[D] (95141) MQTT: Adding QoS as QoS1 in PUBLISH flags.
[D] (95144) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=96
[D] (95146) MQTT: sendMessageVector: Bytes Sent=35, Bytes Remaining=61
[D] (95147) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=59
[D] (95148) MQTT: sendMessageVector: Bytes Sent=59, Bytes Remaining=0
[I] (95164) SHADOW_DEMO: PUBLISH sent for topic $aws/things/wm_demo_1/shadow/update to broker with packet ID 9.
[D] (95760) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (95761) MQTT: Encoded size for length 2 is 1 bytes.
[D] (95761) MQTT: Received packet of type 40.
[D] (95761) MQTT: Packet identifier 9.
[I] (95761) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (95761) MQTT: State record updated. New state=MQTTPublishDone.
[I] (95761) SHADOW_DEMO: PUBACK received for packet id 9.
[I] (95761) SHADOW_DEMO: Cleaned up outgoing publish packet with packet id 9.
[D] (95764) aws_mbedtls: mbedtls_ssl_read recv 178 bytes
[D] (95764) MQTT: Encoded size for length 175 is 2 bytes.
[D] (95764) MQTT: QoS is 1.
[D] (95764) MQTT: Retain bit is 0.
[D] (95765) MQTT: DUP bit is 0.
[D] (95765) MQTT: Topic name length: 41.
[D] (95765) MQTT: Packet identifier 1.
[D] (95765) MQTT: Payload length 130.
[I] (95765) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (95765) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (95765) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/update/delta.
[I] (95765) SHADOW: /update/delta json payload:{"version":65,"timestamp":1739167261,"state":{"powerOn":1},"metadata":{"powerOn":{"timestamp":1739167261}},"clientToken":"473557"}.
[I] (95785) SHADOW: version: 65
[I] (95786) SHADOW: version:65, currentVersion:0
[I] (95786) SHADOW: The new power on state newState:1, currentPowerOnState:0
[D] (95811) MQTT: sendBuffer: Bytes Sent=4, Bytes Remaining=0
[D] (95814) aws_mbedtls: mbedtls_ssl_read recv 205 bytes
[D] (95815) MQTT: Encoded size for length 202 is 2 bytes.
[D] (95815) MQTT: QoS is 1.
[D] (95832) MQTT: Retain bit is 0.
[D] (95832) MQTT: DUP bit is 0.
[D] (95832) MQTT: Topic name length: 44.
[D] (95832) MQTT: Packet identifier 2.
[D] (95832) MQTT: Payload length 154.
[I] (95832) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (95854) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (95854) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/update/accepted.
[I] (95854) SHADOW: /update/accepted json payload:{"state":{"desired":{"powerOn":1}},"metadata":{"desired":{"powerOn":{"timestamp":1739167261}}},"version":65,"timestamp":1739167261,"clientToken":"473557"}.
[I] (95876) SHADOW: clientToken: 473557
[I] (95876) SHADOW: receivedToken:473557, clientToken:473557
[I] (95898) SHADOW: Received response from the device shadow. Previously published update with clientToken=473557 has been accepted.
[D] (95899) MQTT: sendBuffer: Bytes Sent=4, Bytes Remaining=0
[I] (105899) SHADOW: Report to the state change: 1
[I] (105899) SHADOW_DEMO: Published Topic: $aws/things/wm_demo_1/shadow/update
[I] (105899) SHADOW_DEMO: Published payload: {"state":{"reported":{"powerOn":1}},"clientToken":"484315"}
[D] (105899) MQTT: Encoded size for length 99 is 1 bytes.
[D] (105899) MQTT: Encoded size for length 99 is 1 bytes.
[D] (105900) MQTT: PUBLISH packet remaining length=99 and packet size=101.
[D] (105900) MQTT: Encoded size for length 99 is 1 bytes.
[D] (105900) MQTT: Adding QoS as QoS1 in PUBLISH flags.
[D] (105902) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=97
[D] (105904) MQTT: sendMessageVector: Bytes Sent=35, Bytes Remaining=62
[D] (105905) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=60
[D] (105906) MQTT: sendMessageVector: Bytes Sent=60, Bytes Remaining=0
[I] (105906) SHADOW_DEMO: PUBLISH sent for topic $aws/things/wm_demo_1/shadow/update to broker with packet ID 10.
[D] (106731) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (106732) MQTT: Encoded size for length 2 is 1 bytes.
[D] (106732) MQTT: Received packet of type 40.
[D] (106732) MQTT: Packet identifier 10.
[I] (106732) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (106732) MQTT: State record updated. New state=MQTTPublishDone.
[I] (106732) SHADOW_DEMO: PUBACK received for packet id 10.
[I] (106732) SHADOW_DEMO: Cleaned up outgoing publish packet with packet id 10.
[D] (106735) aws_mbedtls: mbedtls_ssl_read recv 207 bytes
[D] (106735) MQTT: Encoded size for length 204 is 2 bytes.
[D] (106736) MQTT: QoS is 1.
[D] (106736) MQTT: Retain bit is 0.
[D] (106736) MQTT: DUP bit is 0.
[D] (106736) MQTT: Topic name length: 44.
[D] (106736) MQTT: Packet identifier 1.
[D] (106736) MQTT: Payload length 156.
[I] (106736) MQTT: De-serialized incoming PUBLISH packet: DeserializerResult=MQTTSuccess.
[I] (106736) MQTT: State record updated. New state=MQTTPubAckSend.
[I] (106736) SHADOW: pPublishInfo->pTopicName:$aws/things/wm_demo_1/shadow/update/accepted.
[I] (106756) SHADOW: /update/accepted json payload:{"state":{"reported":{"powerOn":1}},"metadata":{"reported":{"powerOn":{"timestamp":1739167272}}},"version":66,"timestamp":1739167272,"clientToken":"484315"}.
[I] (106757) SHADOW: clientToken: 484315
[I] (106757) SHADOW: receivedToken:484315, clientToken:484315
[I] (106781) SHADOW: Received response from the device shadow. Previously published update with clientToken=484315 has been accepted.
[D] (106783) MQTT: sendBuffer: Bytes Sent=4, Bytes Remaining=0
[I] (116803) SHADOW: Start to unsubscribe shadow topics and disconnect from MQTT.
[D] (116803) MQTT: Encoded size for length 45 is 1 bytes.
[D] (116803) MQTT: Subscription packet remaining length=45 and packet size=47.
[D] (116803) MQTT: UNSUBSCRIBE packet size is 47 and remaining length is 45.
[D] (116810) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=43
[D] (116812) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=41
[D] (116813) MQTT: sendMessageVector: Bytes Sent=41, Bytes Remaining=0
[I] (116814) SHADOW_DEMO: UNSUBSCRIBE sent topic $aws/things/wm_demo_1/shadow/update/delta to broker.
[D] (117362) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (117362) MQTT: Encoded size for length 2 is 1 bytes.
[D] (117362) MQTT: Received packet of type b0.
[D] (117363) MQTT: Packet identifier 11.
[I] (117363) SHADOW_DEMO: MQTT_PACKET_TYPE_UNSUBACK.
[D] (117363) MQTT: Encoded size for length 48 is 1 bytes.
[D] (117363) MQTT: Subscription packet remaining length=48 and packet size=50.
[D] (117363) MQTT: UNSUBSCRIBE packet size is 50 and remaining length is 48.
[D] (117365) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=46
[D] (117367) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=44
[D] (117368) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=0
[I] (117368) SHADOW_DEMO: UNSUBSCRIBE sent topic $aws/things/wm_demo_1/shadow/update/accepted to broker.
[D] (117983) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (117984) MQTT: Encoded size for length 2 is 1 bytes.
[D] (117984) MQTT: Received packet of type b0.
[D] (117984) MQTT: Packet identifier 12.
[I] (117984) SHADOW_DEMO: MQTT_PACKET_TYPE_UNSUBACK.
[D] (117984) MQTT: Encoded size for length 48 is 1 bytes.
[D] (117984) MQTT: Subscription packet remaining length=48 and packet size=50.
[D] (117985) MQTT: UNSUBSCRIBE packet size is 50 and remaining length is 48.
[D] (117987) MQTT: sendMessageVector: Bytes Sent=4, Bytes Remaining=46
[D] (117988) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=44
[D] (117989) MQTT: sendMessageVector: Bytes Sent=44, Bytes Remaining=0
[I] (117990) SHADOW_DEMO: UNSUBSCRIBE sent topic $aws/things/wm_demo_1/shadow/update/rejected to broker.
[D] (118544) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (118545) MQTT: Encoded size for length 2 is 1 bytes.
[D] (118545) MQTT: Received packet of type b0.
[D] (118545) MQTT: Packet identifier 13.
[I] (118545) SHADOW_DEMO: MQTT_PACKET_TYPE_UNSUBACK.
[D] (118545) MQTT: MQTT DISCONNECT packet size is 2.
[D] (118547) MQTT: sendBuffer: Bytes Sent=2, Bytes Remaining=0
[D] (118547) MQTT: Sent 2 bytes of DISCONNECT packet.
[I] (118547) MQTT: Disconnected from the broker.
[I] (118551) SHADOW: Demo iteration 1 is successful.
[I] (118551) SHADOW: Demo completed successfully.
```
