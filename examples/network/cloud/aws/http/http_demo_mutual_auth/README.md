# AWS HTTP Mutual Auth Demo

## 功能概述

本示例基于 AWS 官方示例进行构建，主要用于展示设备如何通过 HTTPS 协议接入 AWS 平台，具体实现了以下功能：

1. 使设备能够借助 HTTPS 协议成功接入 AWS 服务器。
2. 设备向服务器发送 POST 请求，消息内容为 “Hello, world”。

## 环境要求

1. 首先，需要在 AWS 平台完成物品注册操作，详细步骤请参考：[AWS IoT](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/aws.html)
2. 配置 root CA 证书，其存放路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/AmazonRootCA1.pem"
3. 配置设备证书，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/certificate.pem.crt"
4. 配置设备证书密钥，路径为 "wm_iot_sdk/examples/network/cloud/aws/cert/common/private.pem.key"
5. 设备接入地址：定义在 aws_user_def.h 的 AWS_IOT_ENDPOINT，其值为 AWS 控制台中的 “iot:Data-ATS” 域名。

## 编译和烧录

示例位置：`examples/network/cloud/aws/http/http_demo_mutual_auth`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 设备使用说明

使用如下命令行运行此 demo
1. wifi init： 初始化 WiFi
2. wifi connect ssid password：将 “ssid” 和 “password” 替换为实际可用的无线网络名称和密码，以此连接到可用热点。
3. ntp start：启动网络时钟。
4. aws_http：启动 AWS demo。

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
[D] (12502) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
W800:
W800:
W800:
W800: wifi connect 61test 12345678
connecting, please wait...
[D] (15261) dt: add device rng
[D] (15280) wifi: distribution event 1
[D] (15280) wifi: sta connected 94:83:C4:3A:BA:9E channel 6 authmode 3
[D] (15281) main: GetMAC From NVS (Type=0) 28:6d:ce:8d:2d:da
sta connected
[D] (15995) wifi: distribution event 0
[D] (15996) wifi: sta got ip 192.168.8.243
W800: ntp start
W800: ntp
NTP is running:
0st sntp server: ntp.sjtu.edu.cn
1st sntp server: cn.ntp.org.cn
2st sntp server: us.pool.ntp.org
[D] (19330) dt: add device rtc
Beijing time: 2025-02-10 09:36:49
W800: aws_http
W800:
W800: [I] (128613) DEMO: Performing TLS handshake on top of the TCP connection.
[D] (128614) aws_mbedtls:
  . Seeding the random number generator...
[D] (128617) aws_mbedtls:  ok
[D] (128617) aws_mbedtls:   . Loading the CA root certificate ...
[D] (128628) aws_mbedtls:  ok (0 skipped)
[D] (128638) aws_mbedtls:  ok (0 skipped)
[D] (128652) aws_mbedtls:  ok (0 skipped)
[D] (128652) aws_mbedtls:   . Connecting to tcp/a2crv3ycvx4d9c-ats.iot.ap-south-1.amazonaws.com/443...
[D] (129113) aws_mbedtls:  ok
[D] (129113) aws_mbedtls:   . Setting up the SSL/TLS structure...
[D] (129121) aws_mbedtls:  ok
[D] (129122) aws_mbedtls:   . Performing the SSL/TLS handshake...
[D] (129467) dt: add device rsa
[D] (130957) aws_mbedtls:  ok
[I] (130958) DEMO: Sending HTTP POST request to a2crv3ycvx4d9c-ats.iot.ap-south-1.amazonaws.com/topics/topic?qos=1...
[D] (130958) DEMO: Request Headers:
POST /topics/topic?qos=1 HTTP/1.1
User-Agent: my-platform-name
Host: a2crv3ycvx4d9c-ats.iot.ap-south-1.amazonaws.com
Connection: keep-alive
Request Body:
{ "message": "Hello, world" }
[D] (130958) HTTP: Sending HTTP request headers: HeaderBytes=166
[D] (130961) HTTP: Sent data over the transport: BytesSent=166, BytesRemaining=0, TotalBytesSent=166
[D] (130962) HTTP: Sending the HTTP request body: BodyBytes=29
[D] (130963) HTTP: Sent data over the transport: BytesSent=29, BytesRemaining=0, TotalBytesSent=29
[D] (131676) aws_mbedtls: mbedtls_ssl_read recv 319 bytes
[D] (131676) HTTP: Response parsing: Found the start of the response message.
[D] (131676) HTTP: Response parsing: Found the Reason-Phrase: StatusCode=403, ReasonPhrase=Forbidden
[D] (131677) HTTP: Response parsing: StatusCode=403
[D] (131677) HTTP: Response parsing: Found a header field: HeaderField=content-type
[D] (131677) HTTP: Response parsing: Found a header value: HeaderValue=application/json
[D] (131677) HTTP: Response parsing: Found complete header: HeaderField=content-type, HeaderValue=application/json
[D] (131677) HTTP: Response parsing: Found a header field: HeaderField=content-length
[D] (131677) HTTP: Response parsing: Found a header value: HeaderValue=85
[D] (131678) HTTP: Response parsing: Found complete header: HeaderField=content-length, HeaderValue=85
[D] (131679) HTTP: Response parsing: Found a header field: HeaderField=date
[D] (131679) HTTP: Response parsing: Found a header value: HeaderValue=Mon, 10 Feb 2025 01:38:41 GMT
[D] (131679) HTTP: Response parsing: Found complete header: HeaderField=date, HeaderValue=Mon, 10 Feb 2025 01:38:41 GMT
[D] (131703) HTTP: Response parsing: Found a header field: HeaderField=x-amzn-RequestId
[D] (131703) HTTP: Response parsing: Found a header value: HeaderValue=be4bdf9f-959b-e315-d00b-6fb6f3800ca1
[D] (131725) HTTP: Response parsing: Found complete header: HeaderField=x-amzn-RequestId, HeaderValue=be4bdf9f-959b-e315-d00b-6fb6f3800ca1
[D] (131725) HTTP: Response parsing: Found a header field: HeaderField=connection
[D] (131725) HTTP: Response parsing: Found a header value: HeaderValue=keep-alive
[D] (131747) HTTP: Response parsing: Found complete header: HeaderField=connection, HeaderValue=keep-alive
[D] (131747) HTTP: Response parsing: Found a header field: HeaderField=x-amzn-ErrorType
[D] (131747) HTTP: Response parsing: Found a header value: HeaderValue=ForbiddenException:
[D] (131769) HTTP: Response parsing: Found complete header: HeaderField=x-amzn-ErrorType, HeaderValue=ForbiddenException:
[D] (131769) HTTP: Response parsing: Found the end of the headers.
[D] (131791) HTTP: Response parsing: Found the response body: BodyLength=85
[D] (131791) HTTP: Response parsing: Response message complete.
[D] (131791) HTTP: llhttp errno description: HPE_OK
[I] (131792) DEMO: Received HTTP response from a2crv3ycvx4d9c-ats.iot.ap-south-1.amazonaws.com/topics/topic?qos=1...
[I] (131835) DEMO: Demo completed successfully.
```
