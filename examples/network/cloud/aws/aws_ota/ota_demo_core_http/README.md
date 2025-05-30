# AWS OTA By HTTP

## 功能概述

本示例基于 AWS 官方示例进行开发，主要目的在于展示设备如何借助 HTTPS 协议来实现 AWS 的 OTA（Over - the - Air，空中下载）功能。具体实现的功能如下：

1. 设备运用 MQTTS 协议成功接入 AWS 服务器。
2. 设备向服务器订阅 “$aws/things/THING_NAME/jobs/notify-next” 主题，其中 THING_NAME 代表物品名称，下文提及的 THING_NAME 含义相同。
3. 设备向服务器发布空消息，发布主题为 “$aws/things/THING_NAME/jobs/start-next”。
4. 设备等待 AWS 控制台下发 OTA Job（任务）。
5. 当设备接收到与 OTA 相关的 Job 时，自动执行升级命令，并在升级完成后完成设备重启，从而实现 OTA 功能。

## 环境要求

1. 首先，需要在 AWS 平台完成物品注册操作，详细步骤请参考：[AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. 配置 root CA 证书，其存放路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. 配置设备证书，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. 配置设备证书密钥，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. 设备接入地址：定义在 aws_user_def.h 的 AWS_IOT_ENDPOINT，其值为 AWS 控制台中的 “iot:Data-ATS” 域名。
6. 客户端 ID：在 aws_user_def.h 文件中定义的 CLIENT_IDENTIFIER，其值为在 AWS 控制台创建设备时输入的物品名称。


## 编译和烧录

示例位置：`examples/network/cloud/aws/aws_ota/ota_demo_core_http`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 设备使用说明

使用如下命令行运行此 demo：
1. wifi init： 初始化 WiFi
2. wifi connect ssid password：将 “ssid” 和 “password” 替换为实际可用的无线网络名称和密码，以此连接到可用热点。
3. ntp start：启动网络时钟。
4. aws_ota：启动 AWS demo。

## AWS 控制台使用说明

1. 上传OTA新固件，访问 [Amazon S3 服务器](https://console.aws.amazon.com/s3)，由于国内访问部分服务器响应速度较慢，本示例选择的是首尔服务器。
2. 在 S3 服务器上创建存储桶，并按照系统提示上传 OTA 固件。
3. 登录 [AWS IoT 控制台](https://console.aws.amazon.com/console)。
4. 依次进入 AWS IoT -> Remote actions -> Jobs -> Create job，创建 FreeRTOS OTA 任务。需要注意的是，在选择用于文件传输的协议时，应勾选 “HTTP” 选项。
5. 当前 demo 中未对文件进行签名，但仍需按照 AWS 的要求进行相应填写。选择 “签名并选择您的文件”（使用我的自定义签名文件），原始哈希算法选择（SHA - 256），原始加密算法选择（ECDSA）。
6. 在文件选择中，选择现有文件，并通过浏览 S3 存储桶选中即将推送的固件。
7. IMA 角色需要选择具备 OTA 权限的角色，具体操作可参考 “IMA 权限控制，添加 IMA 角色” 章节。[AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)。

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
[D] (24387) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
[D] (24408) main: GetMAC From NVS (Type=1) 02:25:08:09:01:0f
W800:
W800:
W800:
W800: wifi connect 61test 12345678
connecting, please wait...
[D] (27163) dt: add device rng
[D] (27186) wifi: distribution event 1
[D] (27186) wifi: sta connected 94:83:C4:3A:BA:9E channel 6 authmode 3
[D] (27187) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
sta connected
[D] (27904) wifi: distribution event 0
[D] (27904) wifi: sta got ip 192.168.8.243
W800: ntp start
W800: ntp
NTP is running:
0st sntp server: ntp.sjtu.edu.cn
1st sntp server: cn.ntp.org.cn
2st sntp server: us.pool.ntp.org
[D] (28399) wifi: distribution event 1
[D] (28399) wifi: sta got ip6 FE80::2A6D:CEFF:FE8D:2DDA (index 0)
[D] (32582) dt: add device rtc
Beijing time: 2025-02-10 13:34:35
W800: aws_ota
W800:
W800: [I] (96210) OTA_DEMO_HTTP: OTA over HTTP demo, Application version 0.9.3
[D] (96210) OTA: OTA Event Queue created.
[I] (96211) OTA_DEMO_HTTP: Establishing a TLS session to a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com:8883.
[D] (96211) aws_mbedtls:
  . Seeding the random number generator...
[D] (96214) aws_mbedtls:  ok
[D] (96214) aws_mbedtls:   . Loading the CA root certificate ...
[D] (96225) aws_mbedtls:  ok (0 skipped)
[D] (96235) aws_mbedtls:  ok (0 skipped)
[D] (96249) aws_mbedtls:  ok (0 skipped)
[D] (96249) aws_mbedtls:   . Connecting to tcp/a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com/8883...
[D] (96604) aws_mbedtls:  ok
[D] (96604) aws_mbedtls:   . Setting up the SSL/TLS structure...
[D] (96612) aws_mbedtls:  ok
[D] (96613) aws_mbedtls:   . Performing the SSL/TLS handshake...
[D] (96930) dt: add device rsa
[D] (99372) aws_mbedtls:  ok
[I] (99372) OTA_DEMO_HTTP: Creating an MQTT connection to a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com.
[D] (99372) MQTT: Encoded size for length 86 is 1 bytes.
[D] (99373) MQTT: CONNECT packet remaining length=86 and packet size=88.
[D] (99373) MQTT: CONNECT packet size is 88 and remaining length is 86.
[D] (99375) MQTT: sendMessageVector: Bytes Sent=12, Bytes Remaining=76
[D] (99376) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=74
[D] (99377) MQTT: sendMessageVector: Bytes Sent=9, Bytes Remaining=65
[D] (99378) MQTT: sendMessageVector: Bytes Sent=2, Bytes Remaining=63
[D] (99380) MQTT: sendMessageVector: Bytes Sent=63, Bytes Remaining=0
[D] (99983) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (99983) aws_mbedtls: mbedtls_ssl_read recv 1 bytes
[D] (99983) MQTT: Encoded size for length 2 is 1 bytes.
[D] (99984) aws_mbedtls: mbedtls_ssl_read recv 2 bytes
[D] (99984) MQTT: BytesReceived=2, BytesRemaining=0, TotalBytesReceived=2.
[D] (99984) MQTT: Packet received. ReceivedBytes=2.
[D] (99984) MQTT: CONNACK session present bit not set.
[D] (99984) MQTT: Connection accepted.
[D] (99984) MQTT: Received MQTT CONNACK successfully from broker.
[I] (99984) MQTT: MQTT connection established with the broker.
[I] (99984) OTA_DEMO_HTTP: MQTT connection successfully established with broker.
[D] (99984) OTA_DEMO_HTTP: Success creating MQTT connection to a2crv3ycvx4d9c-ats.iot.ap-northeast-2.amazonaws.com.
[D] (99985) OTA: OTA Event Sent.
[D] (99985) OTA: Added event message to OTA event queue.
[D] (99985) OTA: OTA Event received
[D] (99986) OTA: Found valid event handler for state transition: State=[Ready], Event=[Start]
[I] (99986) OTA: otaPal_GetPlatformImageState
[D] (99986) OTA: OTA Event Sent.
[D] (100008) OTA: Added event message to OTA event queue.
[D] (100008) OTA: Executing handler for state transition:
[I] (100008) OTA: Current State=[RequestingJob], Event=[Start], New state=[RequestingJob]
[D] (100008) OTA: OTA Event received
[D] (100008) OTA: Found valid event handler for state transition: State=[RequestingJob], Event=[RequestJobDocument]
[D] (109986) MQTT: MQTT PINGREQ packet size is 2.
[D] (109993) MQTT: sendBuffer: Bytes Sent=2, Bytes Remaining=0
[D] (109993) MQTT: Sent 2 bytes of PINGREQ packet.
[I] (109993) OTA_DEMO_HTTP:  Received: 0   Queued: 0   Processed: 0   Dropped: 0
[D] (111606) OTA: OTA Event received
[D] (111628) OTA: Found valid event handler for state transition: State=[WaitingForJob], Event=[ReceivedJobDocument]
[D] (111628) OTA: JSON received: {"clientToken":":wm_demo_1","timestamp":1739165753,"execution":{"jobId":"AFR_OTA-TEST_19","status":"QUEUED","queuedAt":1739165736,"lastUpdatedAt":1739165736,"versionNumber":1,"executionNumber":1,"jobDocument":{"afr_ota":{"protocols":["HTTP"],"files":[{"filepath":"test","filesize":448952,"fileid":0,"certfile":"test","update_data_url":"https://seoul-job-1.s3.ap-northeast-2.amazonaws.com/ota_demo_core_http_ota.img?X-Amz-Security-Token=IQoJb3JpZ2luX2VjEJ7%2F%2F%2F%2F%2F%2F%2F%2F%2F%2FwEaDmFwLW5vcnRoZWFzdC0yIkgwRgIhAKS7WnH2ks6iNWVw2F477N1vgCw0cErgVTnkVrnYhdAFAiEA65tNgxjqeyivI7w0a5kvY8GKqo9T%2BmcXJR2nTBpv7gEq5gIIt%2F%2F%2F%2F%2F%2F%2F%2F%2F%2F%2FARAAGgwxOTQ3MjI0NDI4MTEiDF39d5kYtmFKzkTUiSq6AiSujsHFV%2FXrzdyckLIuLFAz4rEer55BW0QD9SxHmWSLP4AN4yW2QayHsjDRIWe6arKxYv7dFd8zm6DjWfhgSXzezVlcOhnsHtLk74LeEXxbRaEwAgTrpVQD%2FdwEebCzw0Vl4AoJrJ1YM6TxR4GbSTGSG1r7Rl6Oq0BDhsC8wbCvjNtjrn%2FWExvw0ksTTEJv2HNCJeUIqn7GQaAdAvcamawLkY2ksn0hk4mmEuz9pc%2BOYZOGoX6XdJcAQAC8k2IPlVe7ETfEzZteHBN66KEjkuhjdg7ec%2FbpQK12IqDofM%2BBGHzld0%2FeuseCcxOd6XkODekobhN9%2F6FFI6lZhiteKbZ%2Bs%2Foyh%2BAahq3B5470GkelF7F1%2F7Nrwei9l3wCq9ws3EUqo0sPiuoZMwCwHXFfeqv5HnvywjtwEbQAMLmgpr0GOr4BsP%2BaF%2B6fmWonk1CVBryiqw7KYEAbmHmY2nZI7IiT6HXMobxtIEQvrsncobknE5JSMwo8JY9NBQZjmzuN28lr8B53n4r42UqVARYE85ICOUTvaegMiyS%2FT0Xd57CVO9qEiGuusrrQinfYPYeXzWobUHE4h6yJb9hv0OGy0ajm%2Bvs84f2tmlwHlVLcdFVnI1qIL8%2BHbzMa9aizwvhDa18vH9J%2FPufJ%2BHglQSWrIyS%2BlD37%2BdiVYxEUCshPLs9FRQ%3D%3D&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20250210T053553Z&X-Amz-SignedHeaders=host&X-Amz-Expires=1800&X-Amz-Credential=ASIAS2VS4VY57WFCDB7X%2F20250210%2Fap-northeast-2%2Fs3%2Faws4_request&X-Amz-Signature=cf380604351957fbe4b0d9894f458e7249d47df7f8ad1feb71ed14963c3234a3","auth_scheme":"aws.s3.presigned","sig-sha256-ecdsa":"TEST"}]}}}}
[I] (111787) OTA: Extracted parameter: [key: value]=[execution.jobId: AFR_OTA-TEST_19]
[I] (111820) OTA: Extracted parameter: [key: value]=[execution.jobDocument.afr_ota.protocols: ["HTTP"]]
[I] (111824) OTA: Extracted parameter: [key: value]=[filepath: test]
[I] (111825) OTA: Extracted parameter: [key: value]=[filesize: 448952]
[I] (111829) OTA: Extracted parameter: [key: value]=[fileid: 0]
[I] (111830) OTA: Extracted parameter: [key: value]=[certfile: test]
[I] (111832) OTA: Extracted parameter: [key: value]=[update_data_url: https://seoul-job-1.s3.ap-northeast-2.amazonaws.com/ota_demo_core_http_ota.img?X-Amz-Security-Token=IQoJb3JpZ2luX2VjEJ7%2F%2F%2F%2F%2F%2F%2F%2F%2F%2FwEaDmFwLW5vcnRoZWFzdC0yIkgwRgIhAKS7WnH2ks6iNWVw2F477N1vgCw0cErgVTnkVrnYhdAFAiEA65tNgxjqeyivI7w0a5kvY8GKqo9T%2BmcXJR2nTBpv7gEq5gIIt%2F%2F%2F%2F%2F%2F%2F%2F%2F%2F%2FARAAGgwxOTQ3MjI0NDI4MTEiDF39d5kYtmFKzkTUiSq6AiSujsHFV%2FXrzdyckLIuLFAz4rEer55BW0QD9SxHmWSLP4AN4yW2QayHsjDRIWe6arKxYv7dFd8zm6DjWfhgSXzezVlcOhnsHtLk74LeEXxbRaEwAgTrpVQD%2FdwEebCzw0Vl4AoJrJ1YM6TxR4GbSTGSG1r7Rl6Oq0BDhsC8wbCvjNtjrn%2FWExvw0ksTTEJv2HNCJeUIqn7GQaAdAvcamawLkY2ksn0hk4mmEuz9pc%2BOYZOGoX6XdJcAQAC8k2IPlVe7ETfEzZteHBN66KEjkuhjdg7ec%2FbpQK12IqDofM%2BBGHzld0%2FeuseCcxOd6XkODekobhN9%2F6FFI6lZhiteKbZ%2Bs%2Foyh%2BAahq3B5470GkelF7F1%2F7Nrwei9l3wCq9ws3EUqo0sPiuoZMwCwHXFfeqv5HnvywjtwEbQAMLmgpr0GOr4BsP%2BaF%2B6fmWonk1CVBryiqw7KYEAbmHmY2nZI7IiT6HXMobxtIEQvrsncobknE5JSMwo8JY9NBQZjmzuN28lr8B53n4r42UqVARYE85ICOUTvaegMiyS%2FT0Xd57CVO9qEiGuusrrQinfYPYeXzWobUHE4h6yJb9hv0OGy0ajm%2Bvs84f2tmlwHlVLcdFVnI1qIL8%2BHbzMa9aizwvhDa18vH9J%2FPufJ%2BHglQSWrIyS%2BlD37%2BdiVYxEUCshPLs9FRQ%3D%3D&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20250210T053553Z&X-Amz-SignedHeaders=host&X-Amz-Expires=1800&X-Amz-Credential=ASIAS2VS4VY57WFCDB7X%2F20250210%2Fap-northeast-2%2Fs3%2Faws4_request&X-Amz-Signature=cf380604351957fbe4b0d9894f458e7249d47df7f8ad1feb71ed14963c3234a3]
[I] (111964) OTA: Extracted parameter: [key: value]=[auth_scheme: aws.s3.presigned]
[I] (111966) OTA: Extracted parameter [ sig-sha256-ecdsa: TEST"}]}}}}... ]
[I] (111969) OTA: Job document was accepted. Attempting to begin the update.
[I] (111985) OTA: Job parsing success: OtaJobParseErr_t=OtaJobParseErrNone, Job name=AFR_OTA-TEST_19
[D] (111985) OTA_DEMO_HTTP: Received invalid callback event from OTA Agent.
[I] (111985) OTA: otaPal_GetPlatformImageState
[I] (112007) OTA: otaPal_CreateFileForRx
[D] (112007) ota_ops: ota partition offset=0x0000f000,size=0x122000
[D] (112007) ota_ops: app_ota partition offset=0x00131000,size=0xaf000
[I] (112198) OTA: aws ota begin succeeded
[I] (112198) OTA: otaPal_GetPlatformImageState
[I] (112198) OTA: Setting OTA data interface.
[D] (112198) OTA: OTA Event Sent.
[D] (112198) OTA: Added event message to OTA event queue.
[D] (112199) OTA_DEMO_HTTP: Received OtaJobEventProcessed callback from OTA Agent.
[D] (112199) OTA: Executing handler for state transition:
[I] (112199) OTA: Current State=[CreatingFile], Event=[ReceivedJobDocument], New state=[CreatingFile]
[D] (112199) OTA: OTA Event received
[D] (112199) OTA: Found valid event handler for state transition: State=[CreatingFile], Event=[CreateFile]
[D] (112199) OTA: Invoking initFileTransfer_Http
[I] (112199) OTA_DEMO_HTTP: Performing TLS handshake on top of the TCP connection.
[I] (112199) OTA_DEMO_HTTP: Establishing a TLS session with seoul-job-1.s3.ap-northeast-2.amazonaws.com:443.
[D] (112201) aws_mbedtls:
  . Seeding the random number generator...
[D] (112203) aws_mbedtls:  ok
[D] (112204) aws_mbedtls:   . Loading the CA root certificate ...
[D] (112214) aws_mbedtls:  ok (0 skipped)
[D] (112223) aws_mbedtls:   . Connecting to tcp/seoul-job-1.s3.ap-northeast-2.amazonaws.com/443...
[D] (112608) aws_mbedtls:  ok
[D] (112608) aws_mbedtls:   . Setting up the SSL/TLS structure...
[D] (112608) aws_mbedtls:  ok
[D] (112609) aws_mbedtls:   . Performing the SSL/TLS handshake...
[D] (114252) aws_mbedtls:  ok
[D] (114252) OTA: OTA Event Sent.
[D] (114252) OTA: Added event message to OTA event queue.
[D] (114252) OTA: Executing handler for state transition:
[I] (114252) OTA: Current State=[RequestingFileBlock], Event=[CreateFile], New state=[RequestingFileBlock]
[D] (114253) OTA: OTA Event received
[D] (114253) OTA: Found valid event handler for state transition: State=[RequestingFileBlock], Event=[RequestFileBlock]
[D] (114253) OTA: OTA Timer created.
[D] (114253) OTA: OTA Timer started.
[D] (114253) OTA: Invoking requestDataBlock_Http
[D] (114254) HTTP: Sending HTTP request headers: HeaderBytes=1454
[D] (114266) HTTP: Sent data over the transport: BytesSent=1454, BytesRemaining=0, TotalBytesSent=1454
[D] (114266) HTTP: A request body was not sent: pRequestBodyBuf is NULL.
[D] (115080) aws_mbedtls: mbedtls_ssl_read recv 538 bytes
[D] (115081) HTTP: Response parsing: Found the start of the response message.
[D] (115081) HTTP: Response parsing: Found the Reason-Phrase: StatusCode=206, ReasonPhrase=Partial Content
[D] (115081) HTTP: Response parsing: StatusCode=206
[D] (115081) HTTP: Response parsing: Found a header field: HeaderField=x-amz-id-2
[D] (115082) HTTP: Response parsing: Found a header value: HeaderValue=09uGmBIiBTxXsQsdZYFfOmXb072qSbHekwL6+dZkpAI0IxuHW4QYO+JPm4X/ZdkmDg4lJ0zo8SxzpOu1qdye3V5YubhgpirgILwBx10MPxA=
[D] (115082) HTTP: Response parsing: Found complete header: HeaderField=x-amz-id-2, HeaderValue=09uGmBIiBTxXsQsdZYFfOmXb072qSbHekwL6+dZkpAI0IxuHW4QYO+JPm4X/ZdkmDg4lJ0zo8SxzpOu1qdye3V5YubhgpirgILwBx10MPxA=
[D] (115082) HTTP: Response parsing: Found a header field: HeaderField=x-amz-request-id
[D] (115083) HTTP: Response parsing: Found a header value: HeaderValue=PFX6JSVRMD550PEJ
[D] (115083) HTTP: Response parsing: Found complete header: HeaderField=x-amz-request-id, HeaderValue=PFX6JSVRMD550PEJ
[D] (115083) HTTP: Response parsing: Found a header field: HeaderField=Date
[D] (115108) HTTP: Response parsing: Found a header value: HeaderValue=Mon, 10 Feb 2025 05:35:59 GMT
[D] (115108) HTTP: Response parsing: Found complete header: HeaderField=Date, HeaderValue=Mon, 10 Feb 2025 05:35:59 GMT
[D] (115130) HTTP: Response parsing: Found a header field: HeaderField=Last-Modified
[D] (115130) HTTP: Response parsing: Found a header value: HeaderValue=Mon, 20 Jan 2025 09:41:06 GMT
[D] (115130) HTTP: Response parsing: Found complete header: HeaderField=Last-Modified, HeaderValue=Mon, 20 Jan 2025 09:41:06 GMT
[D] (115152) HTTP: Response parsing: Found a header field: HeaderField=ETag
[D] (115152) HTTP: Response parsing: Found a header value: HeaderValue="ccc64e8de01482433fa55bd098d00696"
[D] (115174) HTTP: Response parsing: Found complete header: HeaderField=ETag, HeaderValue="ccc64e8de01482433fa55bd098d00696"
[D] (115174) HTTP: Response parsing: Found a header field: HeaderField=x-amz-server-side-encryption
[D] (115174) HTTP: Response parsing: Found a header value: HeaderValue=AES256
[D] (115196) HTTP: Response parsing: Found complete header: HeaderField=x-amz-server-side-encryption, HeaderValue=AES256
[D] (115196) HTTP: Response parsing: Found a header field: HeaderField=x-amz-version-id
[D] (115218) HTTP: Response parsing: Found a header value: HeaderValue=n02Hf_p3F5lmydZTMVZfDm74s4k20abW
[D] (115218) HTTP: Response parsing: Found complete header: HeaderField=x-amz-version-id, HeaderValue=n02Hf_p3F5lmydZTMVZfDm74s4k20abW
[D] (115240) HTTP: Response parsing: Found a header field: HeaderField=Accept-Ranges
[D] (115240) HTTP: Response parsing: Found a header value: HeaderValue=bytes
[D] (115240) HTTP: Response parsing: Found complete header: HeaderField=Accept-Ranges, HeaderValue=bytes
[D] (115264) HTTP: Response parsing: Found a header field: HeaderField=Content-Range
[D] (115264) HTTP: Response parsing: Found a header value: HeaderValue=bytes 0-4095/448952
[D] (115264) HTTP: Response parsing: Found complete header: HeaderField=Content-Range, HeaderValue=bytes 0-4095/448952
[D] (115286) HTTP: Response parsing: Found a header field: HeaderField=Content-Type
[D] (115286) HTTP: Response parsing: Found a header value: HeaderValue=binary/octet-stream
[D] (115286) HTTP: Response parsing: Found complete header: HeaderField=Content-Type, HeaderValue=binary/octet-stream
[D] (115308) HTTP: Response parsing: Found a header field: HeaderField=Content-Length
[D] (115308) HTTP: Response parsing: Found a header value: HeaderValue=4096
[D] (115330) HTTP: Response parsing: Found complete header: HeaderField=Content-Length, HeaderValue=4096
[D] (115330) HTTP: Response parsing: Found a header field: HeaderField=Server
[D] (115330) HTTP: Response parsing: Found a header value: HeaderValue=AmazonS3
[D] (115352) HTTP: Response parsing: Found complete header: HeaderField=Server, HeaderValue=AmazonS3
[D] (115352) HTTP: Response parsing: Found the end of the headers.
[D] (115352) HTTP: llhttp errno description: HPE_OK
[D] (115388) aws_mbedtls: mbedtls_ssl_read recv 4096 bytes
[D] (115389) HTTP: Response parsing: Found the response body: BodyLength=4096
[D] (115389) HTTP: Response parsing: Response message complete.
[D] (115389) HTTP: llhttp errno description: HPE_OK
[D] (115390) OTA: OTA Event Sent.
[D] (115396) OTA: Added event message to OTA event queue.
[D] (115396) OTA: Executing handler for state transition:
[I] (115396) OTA: Current State=[WaitingForFileBlock], Event=[RequestFileBlock], New state=[WaitingForFileBlock]
[D] (115420) OTA: OTA Event received
[D] (115421) OTA: Found valid event handler for state transition: State=[WaitingForFileBlock], Event=[ReceivedFileBlock]
[D] (115421) OTA: OTA Timer restarted.
[I] (115421) OTA: Received valid file block: Block index=0, Size=4096
[I] (115442) OTA: otaPal_WriteBlock
[D] (115443) ota_ops: magic_no: A0FFFF9F
[D] (115443) ota_ops: img_attr: 00100001
[D] (115443) ota_ops: img_addr: 0800F400
[D] (115443) ota_ops: img_len: 0006D978
[D] (115443) ota_ops: img_header_addr: 0800F000
[D] (115464) ota_ops: upgrade_img_addr: 08131000
[D] (115464) ota_ops: org_checksum: 423790A8
[D] (115465) ota_ops: upd_no: 00000000
[D] (115465) ota_ops: ver: 2.3-alpha.2
[D] (115465) ota_ops: log_level: 00000000
[D] (115465) ota_ops: skip_esc_check: 00000000
[D] (115486) ota_ops: skip_upgrade_check: 00000000
[D] (115487) ota_ops: skip_integrity_check: 00000000
[D] (115487) ota_ops: _reserved0: 00000000
[D] (115487) ota_ops: _reserved1: 00000000
[D] (115487) ota_ops: next: 00000000
[D] (115508) ota_ops: hd_checksum: 43D9B450
[I] (115509) ota_ops: ota version is the same
[I] (115528) OTA: Number of blocks remaining: 109
[D] (115528) OTA: update the job status=0, subReason=0.
*********************Firmware download process, Ignore logs*********************
[I] (336754) OTA_DEMO_HTTP:  Received: 110   Queued: 110   Processed: 109   Dropped: 0
[D] (337059) aws_mbedtls: mbedtls_ssl_read recv 4 bytes
[D] (337059) MQTT: Encoded size for length 2 is 1 bytes.
[D] (337059) MQTT: Received packet of type 40.
[D] (337059) MQTT: Packet identifier 8.
[I] (337059) MQTT: Ack packet deserialized with result: MQTTSuccess.
[I] (337059) MQTT: State record updated. New state=MQTTPublishDone.
[I] (337060) OTA_DEMO_HTTP: PUBACK received for packet id 8.
[I] (337060) OTA_DEMO_HTTP:  Received: 110   Queued: 110   Processed: 109   Dropped: 0
[D] (337060) OTA: Published to MQTT topic: topic=$aws/things/wm_demo_1/jobs/AFR_OTA-TEST_19/update
[D] (337060) OTA: Published update to the job status.
[D] (337060) OTA: OTA Timer Stopped for Timerid=0.
[D] (337060) OTA: OTA Event Sent.
[D] (337060) OTA: Added event message to OTA event queue.
[I] (337060) OTA_DEMO_HTTP: Received OtaJobEventActivate callback from OTA Agent.
[I] (337061) OTA: otaPal_ActivateNewImage
[I] (337061) OTA: otaPal_ResetDevice
[I] (338062) ota_ops: ota reboot, please wait for the restart...
[I] unxz...
......................................
[I] unxz succeed
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
[D] (8) nvs_store: group load end
[I] (11) main: ver: 2.3-alpha.2 build at Jan 20 2025 16:28:45
[I] (12) main: boot reason 5
[I] (12) main: heap size 226768(221.45KB)
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
