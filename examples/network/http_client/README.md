# HTTP 客户端

## 功能概述
本例演示了如何实现一个简单的 HTTP 客户端。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。

## 编译和烧录

示例位置：`examples/network/http_client`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 注意

示例中请求的地址包含 www.baidu.com, www.qq.com 等地址，获取内容有中文等符号，部分终端打印可能存在乱码。
且由于时间不同获取到的内容也不相同，使用者可以参考当时上述域名显示内容对比参考。


## 运行结果

成功运行将输出如下日志

```
D/wifi            [1.512] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
D/wm_nm_core      [2.536] distribution WM_LWIP_EV event 0
I/wm_nm_core      [2.540] sta got ip: 192.168.3.34
D/wm_nm_core      [2.544] WM_NM_EV post event 4 success
I/example         [2.718] wifi station start success, network is ready!
I/NO_TAG          [2.726] pHTTPSession->HttpConnection.HttpSocket=1

I/example         [2.784] WM_HTTP_EVENT_CONNECTED
I/example         [2.790] WM_HTTP_EVENT_HEADER_SENTED
I/example         [2.794] Content-Type: application/x-www-form-urlencoded
Host: www.baidu.com
User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)

I/example         [2.886] WM_HTTP_EVENT_RECV_HEADER
I/example         [2.890] HTTP/1.1 200 OK
Accept-Ranges: bytes
Cache-Control: max-age=315360000
Content-Length: 1575
Content-Type: image/gif
Date: Mon, 01 Jul 2024 05:43:29 GMT
Etag: "627-4d648041f6b80"
Expires: Thu, 29 Jun 2034 05:43:29 GMT
Last-Modified: Fri, 22 Feb 2013 03:45:02 GMT
P3p: CP=" OTI DSP COR IVA OUR IND COM "
Server: Apache
Set-Cookie: BAIDUID=4D7D1BB6A4603FCBA32DFFFF8925452F:FG=1; expires=Tue, 01-Jul-25 05:43:29 GMT; max-age=31536000; path=/; domain=.baidu.com; version=1


I/example         [2.936] Start to receive data from remote server...
I/example         [2.944] WM_HTTP_EVENT_RECV_DATA
I/example         [2.948] GIF89
I/example         [2.950] WM_HTTP_EVENT_RECV_DATA
I/example         [2.972] WM_HTTP_EVENT_DISCONNECTED
I/NO_TAG          [2.978] pHTTPSession->HttpConnection.HttpSocket=1


  . Seeding the random number generator... ok
  . Loading the CA root certificate ... ok (0 skipped)
  . Connecting to tcp... ok
  . Setting up the SSL/TLS structure... ok
  . Performing the SSL/TLS handshake... ok
I/example         [3.524] WM_HTTP_EVENT_CONNECTED
I/example         [3.532] WM_HTTP_EVENT_HEADER_SENTED
I/example         [3.538] Content-Type: application/x-www-form-urlencoded
Host: www.qq.com
User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)
Connection: Keep-Alive

I/example         [3.694] WM_HTTP_EVENT_RECV_HEADER
I/example         [3.698] HTTP/1.1 200 OK
Date: Mon, 01 Jul 2024 05:43:30 GMT
Content-Type: text/html; charset=utf-8
Content-Length: 112576
Connection: keep-alive
Server: tRPC-Gateway
Vary: Accept-Encoding
Inews_trace_id: 40396647090701134330
Inews-Trace-Id: 40396647090701134330
X-Upstream-Latency: 5
X-Proxy-Latency: 1
X-Frame-Options: SAMEORIGIN
Content-Security-Policy: frame-ancestors none


I/example         [3.738] Start to receive data from remote server...
I/example         [3.746] WM_HTTP_EVENT_RECV_DATA
I/example         [3.750] <!DOCTYPE html>
<html lang="zh-CN">

<head>
  <title>腾讯网</title>
  <meta charset="utf-8" />
  <meta http-equiv="X-UA-Compatible" content="IE=Edge" />
  <meta name="baidu-site-verification" content="cNitg6enc2" />
  <meta name="baidu_union_verify" content="4508fc7dced37cf569c36f88135276d2" />
  <meta name="theme-color" content="#FFF" />
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <meta name="format-detection" content="telephone=no" />
  <meta name="Keywords" content="资讯,新闻,财经,房产,视频,NBA,科技,腾讯网,腾讯,QQ,Tencent" />
  <meta name="description" content="腾讯网从2003年创立至今，已经成为集新闻信息，区域垂直生活服务、社会化媒体资讯和产品为一体的互联网媒体平台。腾讯网下设新闻、科技、财经、娱乐、体育、汽车、时尚等多个频道，充分满足用户对不同类型资讯的需求。同时专注不同领域内容，打造精品栏目，骞堕『搴灞瓒匡ㄥ虹缁存
I/example         [3.842] WM_HTTP_EVENT_RECV_DATA
I/example         [3.848] 绛创新形式，改变了用户获取资讯的方式和习惯。" />
  <meta property="og:type" content="webpage" />
  <meta property="og:url" content="http://www/qq/com" />
  <meta property="og:title" content="腾讯网" />
  <meta property="og:description" content="腾讯网从2003年创立至今，已经成为集新闻信息，区域垂直生活服务、社会化媒体资讯和产品为一体的互联网媒体平台。腾讯网下设新闻、科技、财经、娱乐、体育、汽车、时尚等多个频道，充分满足用户对不同类型资讯的需求。同时专注不同领域内容，打造精品栏目，并顺应技术发展趋势，推出网络直播等创新形式，改变了用户获取资讯的方式和习惯。" />
  <link rel="shortcut icon" href="https://mat1.gtimg.com/www/icon/favicon2.ico" />
    <script>
    try {
      var uaReg = /android|webos|iphone|ipad|ipod|blackberry|opera mini|mobile/i;
      if (uaReg.test(window.navigator.userAgent.toLowerCase()) && window.location.search.i
I/example         [3.940] WM_HTTP_EVENT_RECV_DATA
I/example         [3.944] ndexOf('no-redirect=1') < 0) {
        window.location.href = 'https://xw.qq.com?f=qqcom&no-redirect=1';
      }
    } catch (err) {}
  </script>
......
  "json": null, 
  "origin": "1.80.144.8", 
  "url": "http://httpbin.org/put?user=foo"
}

I/example         [29.532] WM_HTTP_EVENT_DISCONNECTED
```
