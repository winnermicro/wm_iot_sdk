# HTTP 请求

## 功能概述
使用 [BSD socket](https://man.openbsd.org/socket.2) 发起一个非常简单的 HTTP 请求。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。

## 编译和烧录

示例位置：`examples/network/http_request`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [4.246] got ip: 192.168.3.208
D/wm_nm_core      [4.250] WM_NM_EV post event 4 success
I/example         [4.754] wifi station start success, network is ready!
I/example         [4.782] DNS lookup succeeded. IP=93.184.215.14
I/example         [4.788] ... allocated socket
I/example         [5.004] ... connected
I/example         [5.010] ... socket send success
I/example         [5.014] ... set socket receiving timeout success
HTTP/1.0 200 OK
Accept-Ranges: bytes
Age: 207267
Cache-Control: max-age=604800
Content-Type: text/html; charset=UTF-8
Date: Sun, 16 Jun 2024 04:16:27 GMT
Etag: "3147526947+gzip"
Expires: Sun, 23 Jun 2024 04:16:27 GMT
Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT
Server: ECAcc (sac/2525)
Vary: Accept-Encoding
X-Cache: HIT
Content-Length: 1256
Connection: close

<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background-color: #f0f0f2;
        margin: 0;
        padding: 0;
        font-family: -apple-system, system-ui, BlinkMacSystemFont, "Segoe UI", "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;

    }
    div {
        width: 600px;
        margin: 5em auto;
        padding: 2em;
        background-color: #fdfdff;
        border-radius: 0.5em;
        box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);
    }
    a:link, a:visited {
        color: #38488f;
        text-decoration: none;
    }
    @media (max-width: 700px) {
        div {
            margin: 0 auto;
            width: auto;
        }
    }
    </style>
</head>

<body>
<div>
    <h1>Example Domain</h1>
    <p>This domain is for use in illustrative examples in documents. You may use this
    domain in literature without prior coordination or asking for permission.</p>
    <p><a href="https://www.iana.org/domains/example">More information...</a></p>
</div>
</body>
</html>
I/example         [7.320] ... done reading from socket. Last read return=0 errno=128.
I/example         [7.332] 10...
I/example         [8.332] 9...
I/example         [9.336] 8...
I/example         [10.336] 7...
I/example         [11.338] 6...
I/example         [12.340] 5...
I/example         [13.342] 4...
I/example         [14.344] 3...
I/example         [15.348] 2...
I/example         [16.350] 1...
I/example         [17.352] 0...
I/example         [18.354] Starting again!
```
