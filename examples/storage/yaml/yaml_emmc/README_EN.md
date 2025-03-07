# libyaml

## Overview

This example demonstrates how to use fatfs to call libyaml to parse yaml files.

## Requirements

An external EMMC card is required.

## Building and Flashing

Example Location： `examples/storage/yaml/yaml_emmc`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output.

```
diskio_sdmmc: Initializing sdmmc device...
yaml_emmc_example: File system mount success, fs_type: 3
yaml_emmc_example: localhost, abc
```