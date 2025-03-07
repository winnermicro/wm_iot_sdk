# RSA Demo

## Overview

Initiate a FreeRTOS task to run the RSA demo once.

The RSA demo realizes the RSA checking program mbedtls_rsa_self_test based on the mbedtls library, and verifies the implementation of wm_drv_rsa_expt_mod in our driver library according to the results.

## Requirements

The board which has a W80x chip.

## Building and Flashing

Example Location： `examples/peripheral/rsa`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output.
```
D/dt              [0.000] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273144(266.74KB)
D/main            [0.004] flash size 0x200000(2MB)
I/test            [0.004] rsa demo.
D/dt              [0.006] add device rsa
  RSA key validation: passed
  PKCS#1 encryption : passed
  PKCS#1 decryption : passed
  PKCS#1 data sign  : passed
  PKCS#1 sig. verify: passed

I/test            [0.166] rsa demo.
I/test            [1.166] rsa demo.
I/test            [2.166] rsa demo.
I/test            [3.166] rsa demo.
I/test            [4.166] rsa demo.
```