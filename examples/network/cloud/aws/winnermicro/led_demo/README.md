# AWS Winnermicro LED Demo

## 功能概述

本示例参考 AWS shadow 官方示例进行开发，主要演示设备如何通过 MQTTS 协议接入 AWS 平台，并通过订阅 shadow 相关 topic，实现设备 LED 状态和模拟温度传感器状态的上报，以及执行平台下发的命令。具体实现功能如下：

1. 设备借助 MQTTS 协议成功连接到 AWS 服务器。
2. 设备向 AWS 发布空消息，发布的 topic 为 “$aws/things/THING_NAME/shadow/delete/accepted”，其中 THING_NAME 代表物品名称，下文提及的 THING_NAME 含义相同。该操作旨在删除影子设备，为后续操作做准备。
3. 设备向 AWS 发布消息，topic 为 “$aws/things/THING_NAME/shadow/update”，用于创建影子设备。消息由 aws_device_update 函数进行组装，用户可通过修改或扩展 s_dev_attr 变量来自定义消息内容。
4. 设备持续监听 AWS 平台消息，在收到消息后进行解析，并通过用户填写在 s_dev_attr 中的函数指针，实现对平台下发消息的处理能力。

## 环境要求

1. 首先，需要在 AWS 平台完成物品注册操作，详细步骤请参考：[AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. 配置 root CA 证书，其存放路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. 配置设备证书，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. 配置设备证书密钥，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. 设备接入地址：定义在 aws_user_def.h 的 AWS_IOT_ENDPOINT，其值为 AWS 控制台中的 “iot:Data-ATS” 域名。
6. 物品名称：在 “aws_user_def.h” 文件中定义的 “THING_NAME”，即 AWS 控制台创建设备时输入的物品名称。

## 编译和烧录

示例位置：`examples/network/cloud/aws/winnermicro/led_demo`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 设备使用说明

使用如下命令行运行此 demo：
1. wifi init： 初始化 WiFi
2. wifi connect ssid password：将 “ssid” 和 “password” 替换为实际可用的无线网络名称和密码，以此连接到可用热点。
3. ntp start：启动网络时钟。
4. aws_led：启动 AWS demo。

## 远程控制

用户可使用 MQTT.fx 工具实现远程控制，具体步骤如下：
1. 配置服务器信息：
    + Broker Address：与 AWS_IOT_ENDPOINT 地址一致，Broker Port：8883。
2. 配置 SSL/TLS：选择 SSL/TLS，并启用该选项，同时配置以下文件路径：
   + CA File：wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem
   + Client Certificate File：wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt
   + Client Key File：wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key
3. 订阅主题：Subscribe: $aws/things/THING_NAME/shadow/update/accepted，订阅该 topic 后可接收设备上报的消息。
4. 发布消息：Publish: $aws/things/THING_NAME/shadow/update，发布消息可将命令透传给设备。消息格式如下，其中 LED 支持 “blue”、“green”、“red”。数据格式：{"state":{"reported":{"led":"blue"}}}

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
