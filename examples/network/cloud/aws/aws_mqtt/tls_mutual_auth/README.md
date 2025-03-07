# AWS MQTT Mutual Auth Demo

## 功能概述

本示例基于 AWS 官方示例开发，主要用于展示设备如何通过 MQTTS 协议接入 AWS 平台。具体实现功能如下：

1. 设备借助 MQTTS 协议成功接入 AWS 服务器。
2. 设备向服务器订阅名为 MQTT_EXAMPLE_TOPIC 的主题，该主题参数可在 mqtt_demo_mutual_auth.c 文件中灵活定义，以满足不同的应用场景需求。
3. 设备向 MQTT_EXAMPLE_TOPIC 主题发布 MQTT_EXAMPLE_MESSAGE 消息，实现设备与服务器之间的信息交互。

## 环境要求

1. 首先，需要在 AWS 平台完成物品注册操作，详细步骤请参考：[AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. 配置 root CA 证书，其存放路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. 配置设备证书，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. 配置设备证书密钥，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. 设备接入地址：定义在 aws_user_def.h 的 AWS_IOT_ENDPOINT，其值为 AWS 控制台中的 “iot:Data-ATS” 域名。

## 编译和烧录

示例位置：`examples/network/cloud/aws/aws_mqtt/tls_mutual_auth`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 设备使用说明

使用如下命令行运行此 demo：
1. wifi init： 初始化 WiFi
2. wifi connect ssid password：将 “ssid” 和 “password” 替换为实际可用的无线网络名称和密码，以此连接到可用热点。
3. ntp start：启动网络时钟。
4. aws_mqtt：启动 AWS demo。

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
