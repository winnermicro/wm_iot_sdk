# RSA demo

## 功能概述

启动一个 FreeRTOS 任务，运行一次 rsa demo 。

rsa demo 基于 mbedtls 库实现了 RSA 检查程序 mbedtls_rsa_self_test，根据结果验证了我们驱动库中wm_drv_rsa_expt_mod 的实现。

## 环境要求

含有 w80x 的板子。

## 编译和烧录

示例位置：`examples/peripheral/rsa`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (5) main: ver: 2.1.6dev build at Nov 26 2024 16:56:35
[I] (5) main: boot reason 0
[I] (6) main: heap size 269912(263.59KB)
[I] (6) main: flash size 0x200000(2MB)
[I] (8) rsa_example: rsa demo.
  RSA key validation: passed
  PKCS#1 encryption : passed
  PKCS#1 decryption : passed
  PKCS#1 data sign  : passed
  PKCS#1 sig. verify: passed

  parse RSA private key and public key passed
  PKCS#1 encryption : passed
  PKCS#1 decryption : passed
[I] (524) rsa_example: rsa demo.
[I] (1524) rsa_example: rsa demo.
[I] (2524) rsa_example: rsa demo.
[I] (3524) rsa_example: rsa demo.
[I] (4524) rsa_example: rsa demo.
[I] (5524) rsa_example: rsa demo.
```