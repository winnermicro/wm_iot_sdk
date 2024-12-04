import re
import sys
import tomlkit
import argparse

parser = argparse.ArgumentParser(usage=None, description='wmsdk device table tool', prog="wmdt.py")
parser.add_argument("-i", "--input", help="device table config file", default="")
parser.add_argument("-c", "--config", help="kconfig file", default="")
parser.add_argument("-o", "--output", help="souce file path", default="")
parser.add_argument("-d", "--debug", help="show debug log", action="store_true", default=False)

wm_header_license = '''/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
'''

wm_dt_hw_c_include = '''#include "wm_soc_cfgs.h"
#include "wm_irq.h"
'''

class wm_device_table:
    def __init__(self, config_file, out_c_path, kconfig_file, debug_mode = False):
        self.debug_mode = debug_mode
        self.config_file = config_file
        self.out_c_path = out_c_path
        self.table_name = ""
        self.config = None
        self.wm_dt_dev_name_h = ""
        self.wm_dt_hw_h = ""
        self.wm_dt_hw_c = ""
        self.wm_dt_hw_c_ops_def = ""
        self.wm_dt_hw_table = [] #[dev_name, hw, ops_addr, priority]

        if kconfig_file:
            self.check_and_load_kconfig(kconfig_file)
        else:
            self.chip_type = "W800"
            self.kconfig = ["uart", "timer", "iflash", "eflash", "crypto", "crc", "hash", "rng", "rsa", "sdmmc", "sdspi", "sdio_slave", "tftlcd", "nv3041a", "st7735", "gz035", "i2c", "spim", "touch_sensor", "pwm", "rtc", "pmu", "i2s", "seg_lcd", "adc", "gpio", "wdt", "psram"]

    def check_and_load_kconfig(self, kconfig_file):
        with open(kconfig_file, "r", encoding="utf-8") as file:
            self.kconfig = []
            for line in file:
                line = line.strip()

                if line.startswith("#"):
                    continue

                if line.startswith("CONFIG_CHIP_NAME"):
                    match = re.search(r'="([^"]*)"', line)
                    if match:
                        self.chip_type = match.group(1)

                if line == "CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED=y":
                    self.kconfig.append("iflash")
                elif line == "CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED=y":
                    self.kconfig.append("eflash")
                elif line == "CONFIG_COMPONENT_DRIVER_TIMER_ENABLED=y":
                    self.kconfig.append("timer")
                elif line == "CONFIG_COMPONENT_DRIVER_UART_ENABLED=y":
                    self.kconfig.append("uart")
                elif line == "CONFIG_COMPONENT_DRIVER_GPIO_ENABLED=y":
                    self.kconfig.append("gpio")
                elif line == "CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y":
                    self.kconfig.append("psram")
                elif line == "CONFIG_COMPONENT_DRIVER_PWM_ENABLED=y":
                    self.kconfig.append("pwm")
                elif line == "CONFIG_COMPONENT_DRIVER_RTC_ENABLED=y":
                    self.kconfig.append("rtc")
                elif line == "CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED=y":
                    self.kconfig.append("sdmmc")
                elif line == "CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED=y":
                    self.kconfig.append("sdspi")
                elif line == "CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED=y":
                    self.kconfig.append("sdio_slave")
                elif line == "CONFIG_COMPONENT_DRIVER_SPIM_ENABLED=y":
                    self.kconfig.append("spim")
                elif line == "CONFIG_COMPONENT_DRIVER_WDT_ENABLED=y":
                    self.kconfig.append("wdt")
                elif line == "CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED=y":
                    self.kconfig.append("tftlcd")
                elif line == "CONFIG_COMPONENT_DRIVER_LCD_ST7735_SPI=y":
                    self.kconfig.append("st7735")
                elif line == "CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y":
                    self.kconfig.append("nv3041a")
                elif line == "CONFIG_COMPONENT_DRIVER_LCD_GZ035_SPI=y":
                    self.kconfig.append("gz035")
                elif line == "CONFIG_COMPONENT_DRIVER_ADC_ENABLED=y":
                    self.kconfig.append("adc")
                elif line == "CONFIG_COMPONENT_DRIVER_CRC_ENABLED=y":
                    self.kconfig.append("crc")
                elif line == "CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED=y":
                    self.kconfig.append("crypto")
                elif line == "CONFIG_COMPONENT_DRIVER_HASH_ENABLED=y":
                    self.kconfig.append("hash")
                elif line == "CONFIG_COMPONENT_DRIVER_RNG_ENABLED=y":
                    self.kconfig.append("rng")
                elif line == "CONFIG_COMPONENT_DRIVER_RSA_ENABLED=y":
                    self.kconfig.append("rsa")
                elif line == "CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED=y":
                    self.kconfig.append("touch_sensor")
                elif line == "CONFIG_COMPONENT_DRIVER_I2S_ENABLED=y":
                    self.kconfig.append("i2s")
                elif line == "CONFIG_COMPONENT_DRIVER_CODEC_ES8374_ENABLED=y":
                    self.kconfig.append("es8374")
                elif line == "CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED=y":
                    self.kconfig.append("seg_lcd")
                elif line == "CONFIG_COMPONENT_DRIVER_I2C_ENABLED=y":
                    self.kconfig.append("i2c")

    def load_config(self):
        with open(self.config_file, mode="rt", encoding="utf-8") as fp:
            config = tomlkit.load(fp)
            if "table_name" in config:
                self.table_name = config["table_name"]
            self.config = config["dev"]

    def save_cfile(self):
        if self.table_name:
            dt_hw_file_name = "wm_dt_hw_" + self.table_name + ".c"
        else:
            dt_hw_file_name = "wm_dt_hw.c"

        self.wm_dt_hw_c = self.wm_dt_hw_c_ops_def + self.wm_dt_hw_c

        self.wm_dt_hw_c = self.wm_dt_hw_c + "const static struct wm_dt_table_entry dt_hw_table" + self.append_table_name() + "entry[] = {\n"
        #self.wm_dt_hw_table.sort(key=lambda x: x['priority'], reverse=True)
        dev_names = []
        for entry in self.wm_dt_hw_table:
            dev_names.append(entry['dev_name'].strip("\""))
            if entry['ops_addr']:
                self.wm_dt_hw_c = self.wm_dt_hw_c + "    { .dev_name = " + entry['dev_name'] + ", .hw_addr = (void *)&" + entry['hw_addr'] + ", .ops_addr = (void *)&" + entry['ops_addr'] + " },\n"
            else:
                self.wm_dt_hw_c = self.wm_dt_hw_c + "    { .dev_name = " + entry['dev_name'] + ", .hw_addr = (void *)&" + entry['hw_addr'] + ", .ops_addr = NULL },\n"
        self.wm_dt_hw_c = self.wm_dt_hw_c + "};\n\n"

        self.wm_dt_hw_c = self.wm_dt_hw_c + "WM_DT_TABLE_DEFINE(" + self.table_name + ", (sizeof(dt_hw_table" + self.append_table_name() + "entry) / sizeof(dt_hw_table" + self.append_table_name() + "entry[0])), (void *)&dt_hw_table" + self.append_table_name() + "entry[0]);"

        max_width_len = max(len(name) for name in dev_names) + 8
        self.wm_dt_dev_name_h = [f"#define WM_DEV_{name.upper()}_NAME{' ' * (max_width_len - len(name))}\"{name}\"" for name in dev_names]

        self.wm_dt_dev_name_h = '''/**
 * @file wm_dt_dev_name.h
 *
 * @brief Device Name Module
 *
 */

''' + wm_header_license + '''#ifndef __WM_DT_DEV_NAME_H__
#define __WM_DT_DEV_NAME_H__

#ifdef __cplusplus
extern "C" {
#endif

''' + "\n".join(self.wm_dt_dev_name_h) + '''

#ifdef __cplusplus
}
#endif

#endif /* __WM_DT_DEV_NAME_H__ */'''

        self.wm_dt_hw_h = '''/**
 * @file wm_dt_hw.h
 *
 * @brief Hardware Info Module
 *
 */

''' + wm_header_license + '''#ifndef __WM_DT_HW_H__
#define __WM_DT_HW_H__

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)
''' + self.wm_dt_hw_h + '''
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* __WM_DT_HW_H__ */'''

        self.wm_dt_hw_c = '''/**
 * @file ''' + dt_hw_file_name + '''
 *
 * @brief Device Hardware Information Module
 *
 */

''' + wm_header_license + '''
#include "wm_dt_hw.h"
#include "wm_dt_op.h"

''' + wm_dt_hw_c_include + '''

struct wm_drv_ops_structure;

''' + self.wm_dt_hw_c

        if self.debug_mode:
            print("========================wm_dt_dev_name.h======================")
            print(self.wm_dt_dev_name_h)
            print("============================ end =============================")

            print("==========================wm_dt_hw.h==========================")
            print(self.wm_dt_hw_h)
            print(f"====================={dt_hw_file_name}=======================")
            print(self.wm_dt_hw_c)
            print("============================ end =============================")

        if self.out_c_path:
            with open(self.out_c_path + "/wm_dt_dev_name.h", mode="w", encoding="utf-8") as fp:
                fp.write(self.wm_dt_dev_name_h)
            with open(self.out_c_path + "/wm_dt_hw.h", mode="w", encoding="utf-8") as fp:
                fp.write(self.wm_dt_hw_h)
            with open(self.out_c_path + "/" + dt_hw_file_name, mode="w", encoding="utf-8") as fp:
                fp.write(self.wm_dt_hw_c)

    def append_table_name(self):
        #if self.table_name:
        #    return "_" + self.table_name + "_"
        #else:
        #    return "_"
        return "_"

    def is_extra_part_all_digits(self, shorter, longer):
        prefix_length = min(len(longer), len(shorter))
        for i in range(prefix_length):
            if longer[i] != shorter[i]:
                return False
        extra_part = longer[prefix_length:]
        return extra_part.isdigit()

    def update_dt_hw_c_init_cfg(self, item):
        if "system" == item["init_cfg"]["init_level"]:
            init_level = 1
        elif "app" == item["init_cfg"]["init_level"]:
            init_level = 0
        else:
            print(f"{dev_name} param init_level invalid")
            sys.exit(1)
        self.wm_dt_hw_c = self.wm_dt_hw_c + "    .init_cfg = { .init_level = " + str(init_level) + ", .init_priority = " + str(item["init_cfg"]["init_priority"]) + " },\n"

    def update_dt_hw_c_irq_cfg(self, item):
        irq_num = ["WM_IRQ_SDIO", "WM_IRQ_MAC", "WM_IRQ_RF_CFG", "WM_IRQ_SEC", "WM_IRQ_DMA_Channel0", "WM_IRQ_DMA_Channel1", "WM_IRQ_DMA_Channel2", "WM_IRQ_DMA_Channel3", "WM_IRQ_DMA_Channel4_7", "WM_IRQ_DMA_BRUST", "WM_IRQ_I2C", "WM_IRQ_ADC", "WM_IRQ_SPI_LS", "WM_IRQ_SPI_HS", "WM_IRQ_GPIOA", "WM_IRQ_GPIOB", "WM_IRQ_UART0", "WM_IRQ_UART1", "WM_IRQ_TOUCH", "WM_IRQ_UART24", "WM_IRQ_BLE", "WM_IRQ_BT", "WM_IRQ_PWM", "WM_IRQ_I2S", "WM_IRQ_SIDO_HOST", "WM_IRQ_SYS_TICK", "WM_IRQ_RSA", "WM_IRQ_CRYPTION", "WM_IRQ_FLASH", "WM_IRQ_PMU", "WM_IRQ_TIMER", "WM_IRQ_WDG"]
        self.wm_dt_hw_c = self.wm_dt_hw_c + "    .irq_cfg = { .irq_num = " + irq_num[item["irq_cfg"]["irq_num"]] + ", .irq_priority = " + str(item["irq_cfg"]["irq_priority"]) + " },\n"

    def update_dt_hw_c_gpio_cfg(self, dev_name, item):
        self.wm_dt_hw_c += f"const static wm_dt_hw_gpio_cfg_t dt_hw_{dev_name}_pin_cfg[] = {{\n"

        num_mapping = {
            "0": "WM_GPIO_NUM_0", "1": "WM_GPIO_NUM_1", "2": "WM_GPIO_NUM_2", "3": "WM_GPIO_NUM_3",
            "4": "WM_GPIO_NUM_4", "5": "WM_GPIO_NUM_5", "6": "WM_GPIO_NUM_6", "7": "WM_GPIO_NUM_7",
            "8": "WM_GPIO_NUM_8", "9": "WM_GPIO_NUM_9", "10": "WM_GPIO_NUM_10", "11": "WM_GPIO_NUM_11",
            "12": "WM_GPIO_NUM_12", "13": "WM_GPIO_NUM_13", "14": "WM_GPIO_NUM_14", "15": "WM_GPIO_NUM_15",
            "16": "WM_GPIO_NUM_16", "17": "WM_GPIO_NUM_17", "18": "WM_GPIO_NUM_18", "19": "WM_GPIO_NUM_19",
            "20": "WM_GPIO_NUM_20", "21": "WM_GPIO_NUM_21", "22": "WM_GPIO_NUM_22", "23": "WM_GPIO_NUM_23",
            "24": "WM_GPIO_NUM_24", "25": "WM_GPIO_NUM_25", "26": "WM_GPIO_NUM_26", "27": "WM_GPIO_NUM_27",
            "28": "WM_GPIO_NUM_28", "29": "WM_GPIO_NUM_29", "30": "WM_GPIO_NUM_30", "31": "WM_GPIO_NUM_31",
            "32": "WM_GPIO_NUM_32", "33": "WM_GPIO_NUM_33", "34": "WM_GPIO_NUM_34", "35": "WM_GPIO_NUM_35",
            "36": "WM_GPIO_NUM_36", "37": "WM_GPIO_NUM_37", "38": "WM_GPIO_NUM_38", "39": "WM_GPIO_NUM_39",
            "40": "WM_GPIO_NUM_40", "41": "WM_GPIO_NUM_41", "42": "WM_GPIO_NUM_42", "43": "WM_GPIO_NUM_43",
            "44": "WM_GPIO_NUM_44"
        }

        func_mapping = {
            "fun1": "WM_GPIO_IOMUX_FUN1", "fun2": "WM_GPIO_IOMUX_FUN2", "fun3": "WM_GPIO_IOMUX_FUN3",
            "fun4": "WM_GPIO_IOMUX_FUN4", "fun5": "WM_GPIO_IOMUX_FUN5", "fun6": "WM_GPIO_IOMUX_FUN6",
            "fun7": "WM_GPIO_IOMUX_FUN7"
        }

        dir_mapping = {
            "input": "WM_GPIO_DIR_INPUT", "output": "WM_GPIO_DIR_OUTPUT"
        }

        pupd_mapping = {
            "float": "WM_GPIO_FLOAT", "pull_up": "WM_GPIO_PULL_UP", "pull_down": "WM_GPIO_PULL_DOWN"
        }

        int_mode_mapping = {
            "none": "0",
            "falling_edge": "WM_GPIO_IRQ_TRIG_FALLING_EDGE", "rising_edge": "WM_GPIO_IRQ_TRIG_RISING_EDGE",
            "double_edge": "WM_GPIO_IRQ_TRIG_DOUBLE_EDGE", "low_level": "WM_GPIO_IRQ_TRIG_LOW_LEVEL",
            "high_level": "WM_GPIO_IRQ_TRIG_HIGH_LEVEL"
        }

        if "gpio_cfg" in item:
            for gpio in item["gpio_cfg"]:
                pin = str(gpio["pin"])
                func = gpio["fun"]
                dir = gpio["dir"]
                pupd = gpio["pupd"]
                int_mode = gpio["interrupt_mode"]
            
                func_val = func_mapping.get(func.lower(), "WM_GPIO_IOMUX_FUN5")
                dir_val = dir_mapping.get(dir.lower(), "WM_GPIO_DIR_INPUT")
                pupd_val = pupd_mapping.get(pupd.lower(), "WM_GPIO_FLOAT")
                int_mode_val = int_mode_mapping.get(int_mode.lower(), "0")
            
                self.wm_dt_hw_c += (
                    f"    {{ .pin_num = {num_mapping.get(pin, 'WM_GPIO_NUM_0')}, .pin_mux = {func_val}, "
                    f".pin_dir = {dir_val}, .pin_pupd = {pupd_val}, .int_mode = {int_mode_val} }},\n"
                )

        self.wm_dt_hw_c += "};\n\n"

    def update_dt_hw_c_adc_cfg(self, dev_name, item):

        adc_cfg_list = item.get("adc_cfg", [])

        level_mapping1 = {
            "level0": "LEVEL_0",
            "level1": "LEVEL_1",
            "level2": "LEVEL_2",
            "level3": "LEVEL_3",
            "level4": "LEVEL_4",
            "level5": "LEVEL_5",
        }

        level_mapping2 = {
            "level0": "LEVEL_0",
            "level1": "LEVEL_1",
            "level2": "LEVEL_2",
            "level3": "LEVEL_3",
        }
        self.wm_dt_hw_c += f"const static wm_dt_hw_adc_cfg_t dt_hw_{dev_name}_adc_cfg[] = {{\n"

        for adc_item in adc_cfg_list:
            adc_channel = adc_item["adc_channel"]
            pga_gain1 = level_mapping1.get(adc_item["pga_gain1"], "LEVEL_0")
            pga_gain2 = level_mapping2.get(adc_item["pga_gain2"], "LEVEL_0")

            adc_cmp = 0
            if "adc_cmp" in adc_item:
                if adc_item["adc_cmp"]:
                    adc_cmp = 1

            if "cmp_data" in adc_item:
                cmp_data = adc_item["cmp_data"]
            else:
                cmp_data = 0

            cmp_pol = 0
            if "cmp_pol" in adc_item:
                if adc_item["cmp_pol"]:
                    cmp_pol = 1

            if adc_channel == 0:
                channel_str = "WM_ADC_CHANNEL_0"
            elif adc_channel == 1:
                channel_str = "WM_ADC_CHANNEL_1"
            elif adc_channel == 2:
                channel_str = "WM_ADC_CHANNEL_2"
            elif adc_channel == 3:
                channel_str = "WM_ADC_CHANNEL_3"
            elif adc_channel == 8:
                channel_str = "WM_ADC_CHANNEL_0_1_DIFF_INPUT"
            elif adc_channel == 9:
                channel_str = "WM_ADC_CHANNEL_2_3_DIFF_INPUT"
            else:
                channel_str = "WM_ADC_CHANNEL_MAX"

            adc_cfg_str = f"    {{ .adc_channel = {channel_str},"
            adc_cfg_str += f" .pga_gain1 = WM_ADC_GAIN1_{pga_gain1},"
            adc_cfg_str += f" .pga_gain2 = WM_ADC_GAIN2_{pga_gain2},"
            adc_cfg_str += f" .adc_cmp = {adc_cmp},"
            adc_cfg_str += f" .cmp_data = {cmp_data},"
            adc_cfg_str += f" .cmp_pol = {cmp_pol}"
            adc_cfg_str += f" }},\n"

            self.wm_dt_hw_c += adc_cfg_str

        self.wm_dt_hw_c += "};\n\n"


    def update_dt_hw_c_touch_button_cfg(self, dev_name, item):
        self.wm_dt_hw_c += f"const static wm_dt_hw_touch_button_cfg_t dt_hw_{dev_name}_touch_button_cfg[] = {{\n"

        for touch_button in item["touch_button_cfg"]:
            key_num = str(touch_button["key_num"])
            threshold = str(touch_button["threshold"])

            self.wm_dt_hw_c += (
                f"    {{ .key_num = {key_num}, .threshold = {threshold} }},\n"
            )

        self.wm_dt_hw_c += "};\n\n"

    def update_dt_hw_c_dma_device(self, item):
        if "dma_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .dma_device_name = \"" + item["dma_device"] + "\",\n"

    def update_dt_hw_c_rcc_device(self, item):
        if "rcc_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .rcc_device_name = \"" + item["rcc_device"] + "\",\n"

    def update_dt_hw_c_i2s_device(self, item):
        if "i2s_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .i2s_device_name = \"" + item["i2s_device"] + "\",\n"

    def update_dt_hw_c_i2c_device(self, item):
        if "i2c_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .i2c_device_name = \"" + item["i2c_device"] + "\",\n"

    def update_dt_hw_c_seg_lcd_device(self, item):
        if "seg_lcd_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .seg_lcd_device_name = \"" + item["seg_lcd_device"] + "\",\n"

    def update_dt_hw_c_spi_device(self, item):
        if "spi_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .spi_device_name = \"" + item["spi_device"] + "\",\n"

    def update_dt_hw_c_gpio_device(self, item):
        if "gpio_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .gpio_device_name = \"" + item["gpio_device"] + "\",\n"

    def update_dt_hw_c_touch_sensor_device(self, item):
        if "touch_sensor_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .touch_sensor_device_name = \"" + item["touch_sensor_device"] + "\",\n"

    def update_dt_hw_c_reg_base(self, item):
        self.wm_dt_hw_c = self.wm_dt_hw_c + "    .reg_base = " + hex(item["reg_base"]) + ",\n"

    def update_dt_hw_c_pin_lcd_reset(self, item):
        if "irq_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .pin_lcd_reset = \"" + item["pin_lcd_reset"] + "\",\n"

    def update_dt_hw_c_pin_lcd_dcx(self, item):
        if "irq_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .pin_lcd_dcx = \"" + item["pin_lcd_dcx"] + "\",\n"

    def update_dt_hw_c_irq_pin_lcd_led(self, item):
        if "irq_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .pin_lcd_led = \"" + item["pin_lcd_led"] + "\",\n"

    def update_dt_hw_c_begin(self, base_name, dev_name):
        self.wm_dt_hw_c = self.wm_dt_hw_c + "const static wm_dt_hw_" + base_name + "_t dt_hw" + self.append_table_name() + dev_name + " = {\n"

    def update_dt_hw_c_end(self):
        self.wm_dt_hw_c = self.wm_dt_hw_c + "};\n\n"

    def update_dt_hw_c_pin_cfg(self, dev_name, item):
        if not "pin_cfg" in item:
            return

        self.wm_dt_hw_c = self.wm_dt_hw_c + "const static wm_dt_hw_pin_cfg_t dt_hw" + self.append_table_name() + dev_name + "_pin[] = {\n"

        if isinstance(item["pin_cfg"], dict):
            pin_dir = "WM_GPIO_DIR_DEFAULT"
            pin_pupd = "WM_GPIO_FLOAT"

            if "dir" in item["pin_cfg"]:
                if item["pin_cfg"]["dir"] == "input":
                    pin_dir = "WM_GPIO_DIR_INPUT"
                elif item["pin_cfg"]["dir"] == "output":
                    pin_dir = "WM_GPIO_DIR_OUTPUT"
                else:
                    sys.exit(1)

            if "pupd" in item["pin_cfg"]:
                if item["pin_cfg"]["pupd"] == "float":
                    pin_pupd = "WM_GPIO_FLOAT"
                elif item["pin_cfg"]["pupd"] == "pullup":
                    pin_pupd = "WM_GPIO_PULL_UP"
                elif item["pin_cfg"]["pupd"] == "pulldown":
                    pin_pupd = "WM_GPIO_PULL_DOWN"
                else:
                    sys.exit(1)

            pin_cfg_str = f"    {{ .pin_num = WM_GPIO_NUM_{item['pin_cfg']['pin']}, .pin_mux = WM_GPIO_IOMUX_FUN{item['pin_cfg']['fun'][len('fun'):]}"

            if "dir" in item["pin_cfg"]:
                pin_cfg_str += f", .pin_dir = {pin_dir}"

            if "pupd" in item["pin_cfg"]:
                pin_cfg_str += f", .pin_pupd = {pin_pupd}"

            pin_cfg_str += " },\n"

            self.wm_dt_hw_c += pin_cfg_str
        else:
            count = len(item["pin_cfg"])
            for i in range(count):

                pin_dir = "WM_GPIO_DIR_DEFAULT"
                pin_pupd = "WM_GPIO_FLOAT"

                if "dir" in item["pin_cfg"][i]:
                    if item["pin_cfg"][i]["dir"] == "input":
                        pin_dir = "WM_GPIO_DIR_INPUT"
                    elif item["pin_cfg"][i]["dir"] == "output":
                        pin_dir = "WM_GPIO_DIR_OUTPUT"
                    else:
                        sys.exit(1)

                if "pupd" in item["pin_cfg"][i]:
                    if item["pin_cfg"][i]["pupd"] == "float":
                        pin_pupd = "WM_GPIO_FLOAT"
                    elif item["pin_cfg"][i]["pupd"] == "pullup":
                        pin_pupd = "WM_GPIO_PULL_UP"
                    elif item["pin_cfg"][i]["pupd"] == "pulldown":
                        pin_pupd = "WM_GPIO_PULL_DOWN"
                    else:
                        sys.exit(1)

                pin_cfg_str = f"    {{ .pin_num = WM_GPIO_NUM_{item['pin_cfg'][i]['pin']}, .pin_mux = WM_GPIO_IOMUX_FUN{item['pin_cfg'][i]['fun'][len('fun'):]}"

                if "dir" in item["pin_cfg"][i]:
                    pin_cfg_str += f", .pin_dir = {pin_dir}"

                if "pupd" in item["pin_cfg"][i]:
                    pin_cfg_str += f", .pin_pupd = {pin_pupd}"

                pin_cfg_str += " },\n"

                self.wm_dt_hw_c += pin_cfg_str

        self.wm_dt_hw_c = self.wm_dt_hw_c + "};\n\n"

    def update_dt_hw_c_rcc_cfg(self, dev_name, item):
        self.wm_dt_hw_c += f"const static wm_dt_hw_rcc_cfg_t dt_hw_{dev_name}_rcc[] = {{\n"

        for rcc in item["rcc_cfg"]:

            rcc_type_mapping = {
                "peripheral": "WM_RCC_TYPE_PERIPHERAL",
                "wlan": "WM_RCC_TYPE_WLAN",
                "cpu": "WM_RCC_TYPE_CPU",
                "sd_adc": "WM_RCC_TYPE_SD_ADC",
                "qflash": "WM_RCC_TYPE_QFLASH",
                "gpsec": "WM_RCC_TYPE_GPSEC",
                "rsa": "WM_RCC_TYPE_RSA",
                "apb": "WM_RCC_TYPE_APB"
            }

            rcc_type = rcc_type_mapping.get(rcc["type"].lower(), "WM_RCC_TYPE_MAX")
            clock = str(rcc["clock"])

            self.wm_dt_hw_c += (
                f"    {{ .type = {rcc_type}, .clock = {clock} }},\n"
            )

        self.wm_dt_hw_c += "};\n\n"

    def update_dt_hw_c_pin_cfg_link(self, dev_name, item):
        if not "pin_cfg" in item:
            return

        pin_cfg = "dt_hw" + self.append_table_name() + dev_name + "_pin"
        self.wm_dt_hw_c = self.wm_dt_hw_c + "    .pin_cfg_count = sizeof(" + pin_cfg + ") / sizeof(" + pin_cfg + "[0]),\n"
        self.wm_dt_hw_c = self.wm_dt_hw_c + "    .pin_cfg = (wm_dt_hw_pin_cfg_t *)" + pin_cfg + ",\n"

    def update_dt_hw_c_gpio_cfg_link(self, dev_name):
        gpio_cfg = f"dt_hw_{dev_name}_pin_cfg"
        self.wm_dt_hw_c += f"    .gpio_cfg_count = sizeof({gpio_cfg}) / sizeof({gpio_cfg}[0]),\n"
        self.wm_dt_hw_c += f"    .gpio_cfg = (wm_dt_hw_gpio_cfg_t *){gpio_cfg},\n"

    def update_dt_hw_c_rcc_cfg_link(self, dev_name):
        rcc_cfg = f"dt_hw_{dev_name}_rcc"
        self.wm_dt_hw_c += f"    .rcc_cfg_count = sizeof({rcc_cfg}) / sizeof({rcc_cfg}[0]),\n"
        self.wm_dt_hw_c += f"    .rcc_cfg = (wm_dt_hw_rcc_cfg_t *){rcc_cfg},\n"

    def update_dt_hw_c_touch_button_cfg_link(self, dev_name):
        touch_button_cfg = f"dt_hw_{dev_name}_touch_button_cfg"
        self.wm_dt_hw_c += f"    .touch_button_cfg_count = sizeof({touch_button_cfg}) / sizeof({touch_button_cfg}[0]),\n"
        self.wm_dt_hw_c += f"    .touch_button_cfg = (wm_dt_hw_touch_button_cfg_t *){touch_button_cfg},\n"

    def update_dt_hw_c_adc_cfg_link(self, dev_name):
        adc_cfg_link_str = f"    .adc_cfg_count = sizeof(dt_hw_{dev_name}_adc_cfg) / sizeof(dt_hw_{dev_name}_adc_cfg[0]),\n"
        adc_cfg_link_str += f"    .adc_cfg = (wm_dt_hw_adc_cfg_t *)dt_hw_{dev_name}_adc_cfg,\n"

        self.wm_dt_hw_c += adc_cfg_link_str

    def update_dt_hw_h(self, members):
        self.wm_dt_hw_h = self.wm_dt_hw_h + members

    def update_dt_hw_c_ops_def(self, *args):
        # add ops typedf and extern ops in dt
        dev_name = args[0]
        self.wm_dt_hw_c_ops_def = self.wm_dt_hw_c_ops_def + "typedef struct wm_drv_ops_structure wm_drv_" + dev_name + "_ops_t;\n"
        argc = len(args)
        if argc > 1:
            for i in range(1, argc):
                new_name = args[i]
                self.wm_dt_hw_c_ops_def = self.wm_dt_hw_c_ops_def + "extern const wm_drv_" + dev_name + "_ops_t wm_drv_" + new_name + "_ops;\n"
                if i == argc - 1:
                    self.wm_dt_hw_c_ops_def = self.wm_dt_hw_c_ops_def + "\n"
        else:
            self.wm_dt_hw_c_ops_def = self.wm_dt_hw_c_ops_def + "extern const wm_drv_" + dev_name + "_ops_t wm_drv_" + dev_name + "_ops;\n\n"

    def update_dt_table_ex(self, dev_name, st_name, ops_name, item):
        if ops_name:
            self.wm_dt_hw_table.append({"dev_name": "\"" + dev_name + "\"", "hw_addr": "dt_hw" + self.append_table_name() + st_name, "ops_addr": "wm_drv_" + ops_name + "_ops", "priority": item["init_cfg"]["init_priority"]})
        else:
            self.wm_dt_hw_table.append({"dev_name": "\"" + dev_name + "\"", "hw_addr": "dt_hw" + self.append_table_name() + st_name, "ops_addr": None, "priority": item["init_cfg"]["init_priority"]})

    def update_dt_table(self, dev_name, base_name, item):
        self.update_dt_table_ex(dev_name, dev_name, base_name, item)

    def generate_common(self):
        members = '''
typedef struct {
    uint8_t init_level;
    uint8_t init_priority;
} wm_dt_hw_init_cfg_t;

typedef struct {
    uint8_t irq_num;  /**< @ref wm_irq_no_t */
    uint8_t irq_priority;
} wm_dt_hw_irq_cfg_t;

typedef struct {
    uint8_t pin_num;  /**< @ref wm_gpio_num_t */
    uint8_t pin_mux;  /**< @ref wm_gpio_pin_mux_t */
    uint8_t pin_dir;  /**< @ref wm_gpio_dir_t */
    uint8_t pin_pupd; /**< @ref wm_gpio_pupd_t */
} wm_dt_hw_pin_cfg_t;
'''

        self.update_dt_hw_h(members)

    def generate_rcc(self):
        members = '''
typedef struct {
    wm_rcc_type_t type; /**< @ref wm_rcc_type_t */
    uint32_t clock;     /**< Define the clock for each of bus/controller, unit:Hz,
                         *   The range is same as that defined by \c wm_drv_rcc_config_clock()*/
} wm_dt_hw_rcc_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    uint8_t rcc_cfg_count;
    wm_dt_hw_rcc_cfg_t *rcc_cfg;
} wm_dt_hw_rcc_t;
'''
        base_name = "rcc"

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_rcc_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_rcc_cfg_link(dev_name)
                self.update_dt_hw_c_end()

    def generate_dma(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_dma_t;
'''
        base_name = "dma"

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)
                self.update_dt_hw_c_end()

    def generate_gpio(self):

        members = '''
typedef struct {
    wm_gpio_num_t pin_num;
    wm_gpio_pin_mux_t pin_mux;
    wm_gpio_dir_t pin_dir;
    wm_gpio_pupd_t pin_pupd;
    wm_gpio_intr_mode_t int_mode;
} wm_dt_hw_gpio_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint8_t gpio_cfg_count;
    wm_dt_hw_gpio_cfg_t *gpio_cfg;
} wm_dt_hw_gpio_t;
'''
        base_name = "gpio"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_gpio_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_gpio_cfg_link(dev_name)
                self.update_dt_hw_c_end()

    def generate_uart(self):
        members = '''
typedef struct {
    int baudrate;      /**< @ref wm_uart_baudrate_t */
    uint8_t parity;    /**< @ref wm_uart_parity_t */
    uint8_t stop_bits; /**< @ref wm_uart_stop_bits_t */
    uint8_t data_bits; /**< @ref wm_uart_data_bits_t */
    uint8_t flow_ctrl; /**< @ref wm_uart_flowctrl_t */
} wm_dt_hw_uart_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    wm_dt_hw_uart_cfg_t uart_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_uart_t;
'''
        base_name = "uart"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name in dev_name:
                if self.is_extra_part_all_digits(base_name, dev_name):
                    if (dev_name == "uart5") and not (self.chip_type == "W801S" or self.chip_type == "W802" or self.chip_type == "W806"):
                        continue

                    self.update_dt_table(dev_name, base_name, item)

                    self.update_dt_hw_c_pin_cfg(dev_name, item)

                    self.update_dt_hw_c_begin(base_name, dev_name)
                    self.update_dt_hw_c_init_cfg(item)
                    self.update_dt_hw_c_reg_base(item)
                    self.update_dt_hw_c_irq_cfg(item)

                    if "none" == item["uart_cfg"]["parity"]:
                        uart_parity = "WM_UART_PARITY_NONE"
                    elif "even" == item["uart_cfg"]["parity"]:
                        uart_parity = "WM_UART_PARITY_EVEN"
                    elif "odd" == item["uart_cfg"]["parity"]:
                        uart_parity = "WM_UART_PARITY_ODD"
                    else:
                        print(f"{dev_name} param parity invalid")
                        sys.exit(1)

                    if "none" == item["uart_cfg"]["flow_ctrl"]:
                        uart_flow_ctrl = "WM_UART_FLOW_CTRL_DISABLE"
                    elif "rts" == item["uart_cfg"]["flow_ctrl"]:
                        uart_flow_ctrl = "WM_UART_FLOW_CTRL_RTS"
                    elif "cts" == item["uart_cfg"]["flow_ctrl"]:
                        uart_flow_ctrl = "WM_UART_FLOW_CTRL_CTS"
                    elif "rts_cts" == item["uart_cfg"]["flow_ctrl"]:
                        uart_flow_ctrl = "WM_UART_FLOW_CTRL_RTS_CTS"
                    else:
                        print(f"{dev_name} param flow_ctrl invalid")
                        sys.exit(1)

                    self.wm_dt_hw_c = self.wm_dt_hw_c + "    .uart_cfg = { .baudrate = " + "WM_UART_BAUDRATE_B" + str(item["uart_cfg"]["baudrate"]) + ", .parity = " + uart_parity + ", .stop_bits = " + "WM_UART_STOP_BIT_" + str(item["uart_cfg"]["stop_bits"]) + ", .data_bits = " + "WM_UART_DATA_BIT_" + str(item["uart_cfg"]["data_bits"]) + ", .flow_ctrl = " + uart_flow_ctrl + " },\n"

                    self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                    self.update_dt_hw_c_dma_device(item)
                    self.update_dt_hw_c_rcc_device(item)
                    self.update_dt_hw_c_end()

    def generate_timer(self):
        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;

    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_timer_t;
'''
        base_name = "timer"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name in dev_name:
                if self.is_extra_part_all_digits(base_name, dev_name):
                    self.update_dt_table(dev_name, base_name, item)
                    self.update_dt_hw_c_begin(base_name, dev_name)
                    self.update_dt_hw_c_init_cfg(item)
                    self.update_dt_hw_c_reg_base(item)
                    self.update_dt_hw_c_irq_cfg(item)
                    self.update_dt_hw_c_end()

    def generate_pwm(self):
        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
} wm_dt_hw_pwm_t;
'''
        base_name = "pwm"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)

                self.update_dt_hw_c_pin_cfg(dev_name, item)

                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)

                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_dma_device(item)
                self.update_dt_hw_c_end()

    def generate_iflash(self):

        members = '''
typedef struct {
    uint8_t mode;              /**< support 0(CPOL=0,CPHA=0), 1(CPOL=0,CPHA=1), 2(CPOL=1,CPHA=0), 3(CPOL=1,CPHA=1) */
    uint32_t freq;             /**< freq for current device, unit is HZ, freq = FAPB_CLK/ ( 2 x (Divider +1))
                                    range: WM_SPIM_MIN_CLOCK ~ WM_SPIM_MAX_CLOCK*/
    wm_dt_hw_pin_cfg_t pin_cs; /**< the CS GPIO number for select slave device */
} wm_dt_hw_spim_dev_cfg_t;

typedef struct {
    bool quad_spi;
} wm_dt_hw_flash_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;

    wm_dt_hw_flash_cfg_t flash_cfg;
} wm_dt_hw_iflash_t;
'''
        base_name = "iflash"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name, "internal_flash")

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, "internal_flash", item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)

                flash_cfg = item["flash_cfg"]
                self.wm_dt_hw_c += (
                    "    .flash_cfg = { .quad_spi = " + str(flash_cfg['quad_spi']).lower() + " },\n"
                )

                self.update_dt_hw_c_end()

    def generate_seg_lcd(self):

        members = '''
typedef struct {
    uint8_t duty_sel;    /**< @ref wm_seg_lcd_duty_sel_t */
    uint8_t vlcd_cc;     /**< @ref wm_seg_lcd_vlcd_cc_t */
    uint8_t bias;        /**< @ref wm_seg_lcd_bias_t */
    uint8_t hd;          /**< @ref wm_seg_lcd_hd_t */
    uint32_t frame_freq;
    uint8_t com_num;     /**< @ref wm_seg_lcd_com_id_t */
} wm_dt_hw_seg_lcd_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;

    wm_dt_hw_seg_lcd_cfg_t seg_lcd_cfg;

    char *rcc_device_name;
} wm_dt_hw_seg_lcd_t;
'''
        base_name = "seg_lcd"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)

                duty_sel_mapping = {
                    "static": "WM_SEG_LCD_DUTY_SEL_STATIC",
                    "1/2": "WM_SEG_LCD_DUTY_SEL_1_2",
                    "1/3": "WM_SEG_LCD_DUTY_SEL_1_3",
                    "1/4": "WM_SEG_LCD_DUTY_SEL_1_4",
                    "1/5": "WM_SEG_LCD_DUTY_SEL_1_5",
                    "1/6": "WM_SEG_LCD_DUTY_SEL_1_6",
                    "1/7": "WM_SEG_LCD_DUTY_SEL_1_7",
                    "1/8": "WM_SEG_LCD_DUTY_SEL_1_8"
                }
                duty_sel = duty_sel_mapping.get(item['seg_lcd_cfg']['duty_sel'], "WM_SEG_LCD_DUTY_SEL_MAX")

                vlcd_cc_mapping = {
                    "2.7v": "WM_SEG_LCD_VLCD_CC_2_7V",
                    "2.9v": "WM_SEG_LCD_VLCD_CC_2_9V",
                    "3.1v": "WM_SEG_LCD_VLCD_CC_3_1V",
                    "3.3v": "WM_SEG_LCD_VLCD_CC_3_3V"
                }
                vlcd_cc = vlcd_cc_mapping.get(item['seg_lcd_cfg']['vlcd_cc'], "WM_SEG_LCD_VLCD_CC_MAX")

                bias_mapping = {
                    "1/4": "WM_SEG_LCD_BIAS_1_4",
                    "1/2": "WM_SEG_LCD_BIAS_1_2",
                    "1/3": "WM_SEG_LCD_BIAS_1_3",
                    "static": "WM_SEG_LCD_BIAS_STATIC"
                }
                bias = bias_mapping.get(item['seg_lcd_cfg']['bias'], "WM_SEG_LCD_BIAS_MAX")

                drive_strength_mapping = {
                    "low": "WM_SEG_LCD_DRIVE_STRENGTH_LOW",
                    "high": "WM_SEG_LCD_DRIVE_STRENGTH_HIGH"
                }
                drive_strength = drive_strength_mapping.get(item['seg_lcd_cfg']['hd'], "WM_SEG_LCD_DRIVE_STRENGTH_MAX")

                #com_num_mapping = {
                #    0: "WM_SEG_LCD_COM0",
                #    1: "WM_SEG_LCD_COM1",
                #    2: "WM_SEG_LCD_COM2",
                #    3: "WM_SEG_LCD_COM3",
                #    4: "WM_SEG_LCD_COM4",
                #    5: "WM_SEG_LCD_COM5",
                #    6: "WM_SEG_LCD_COM6",
                #    7: "WM_SEG_LCD_COM7"
                #}
                #com_num = com_num_mapping.get(item['seg_lcd_cfg']['com_num'], "WM_SEG_LCD_COM_MAX")

                self.wm_dt_hw_c += (
                    f"    .seg_lcd_cfg = {{ .duty_sel = {duty_sel}, "
                    f".vlcd_cc = {vlcd_cc}, "
                    f".bias = {bias}, "
                    f".hd = {drive_strength}, "
                    f".frame_freq = {item['seg_lcd_cfg']['frame_freq']}, "
                    f".com_num = {item['seg_lcd_cfg']['com_num']} }},\n"
                )
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

        self.generate_gdc0689()

    def generate_i2c(self):
        members = '''
typedef struct {
    uint32_t max_clock;
    bool addr_10_bits;
} wm_dt_hw_i2c_cfg_t;


typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    wm_dt_hw_i2c_cfg_t i2c_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_i2c_t;
'''
        base_name = "i2c"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)

                i2c_cfg = item["i2c_cfg"]
                self.wm_dt_hw_c += (
                    "    .i2c_cfg = { .max_clock = " + str(i2c_cfg['max_clock']) + ", "
                    ".addr_10_bits = " + str(i2c_cfg['addr_10_bits']).lower() + " },\n")

                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

        self.generate_eeprom()

    def generate_spim(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_spim_t;
'''
        base_name = "spim"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_dma_device(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_end()

    def generate_rtc(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_rtc_t;
'''
        base_name = "rtc"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)
                self.update_dt_hw_c_end()

    def generate_pmu(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    wm_pmu_clock_source_t clk_src;
} wm_dt_hw_pmu_t;
'''
        base_name = "pmu"

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)

                clk_src = item["clk_src"]
                if clk_src == "internal":
                    clk_src_enum = "WM_PMU_CLOCK_SRC_32K"
                elif clk_src == "external":
                    clk_src_enum = "WM_PMU_CLOCK_SRC_40M_DIV"

                self.wm_dt_hw_c += (

                    "    .clk_src = " + clk_src_enum + ",\n"
                )

                self.update_dt_hw_c_end()

    def generate_touch_sensor(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *rcc_device_name;
} wm_dt_hw_touch_sensor_t;
'''
        base_name = "touch_sensor"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)

                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

        self.generate_touch_button()

    def generate_touch_button(self):

        members = '''
typedef struct {
    uint8_t key_num;
    uint32_t threshold;
} wm_dt_hw_touch_button_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint8_t touch_button_cfg_count;
    wm_dt_hw_touch_button_cfg_t *touch_button_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *touch_sensor_device_name;
} wm_dt_hw_touch_button_t;
'''
        base_name = "touch_button"

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)

                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_hw_c_touch_button_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_touch_sensor_device(item)
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_touch_button_cfg_link(dev_name)
                self.update_dt_hw_c_end()

    def generate_adc(self):

        members = '''
typedef struct {
    wm_adc_channel_t adc_channel;
    wm_adc_gain1_level_t pga_gain1;
    wm_adc_gain2_level_t pga_gain2;
    uint8_t adc_cmp; /**< Only occure after call wm_drv_adc_start_it*/
    int cmp_data;
    uint8_t cmp_pol; /**< 1: an interrupt is generated when adc_result < cmp_value;
                      *   0: an interrupt is generated when adc_result >= cmp_value */
} wm_dt_hw_adc_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    uint8_t adc_cfg_count;
    wm_dt_hw_adc_cfg_t *adc_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_adc_t;
'''
        base_name = "adc"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_hw_c_adc_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_adc_cfg_link(dev_name)
                self.update_dt_hw_c_dma_device(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_eeprom(self):
        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    uint32_t speed_hz;
    uint32_t size;
    uint16_t i2c_addr;
    uint16_t page_size;
    uint8_t addr_width;
    uint8_t read_only;
    uint16_t max_write_time_ms;

    char *i2c_device_name;
} wm_dt_hw_eeprom_t;
'''
        base_name = "eeprom"

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name, "eeprom_nv24c0x")

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name in dev_name:
                if self.is_extra_part_all_digits(base_name, dev_name):

                    self.update_dt_hw_c_pin_cfg(dev_name, item)
                    self.update_dt_table(dev_name, "eeprom_nv24c0x", item)
                    self.update_dt_hw_c_begin(base_name, dev_name)
                    self.update_dt_hw_c_init_cfg(item)

                    self.wm_dt_hw_c += (
                        "    .speed_hz = " + str(item["speed_hz"]) + ",\n"
                        "    .i2c_addr = " + hex(item["i2c_addr"]) + ",\n"
                        "    .size = " + str(item["size"]) + ",\n"
                        "    .page_size = " + str(item["page_size"]) + ",\n"
                        "    .addr_width = " + str(item["addr_width"]) + ",\n"
                        "    .read_only = " + str(item["read_only"]).lower() + ",\n"
                        "    .max_write_time_ms = " + str(item["max_write_time_ms"]) + ",\n"
                        "    .i2c_device_name = \"" + item["i2c_device"] + "\",\n"
                    )

                    self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                    self.update_dt_hw_c_end()

    def generate_gdc0689(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *seg_lcd_device_name;
} wm_dt_hw_gdc0689_t;
'''
        base_name = "gdc0689"

        self.update_dt_hw_h(members)
        #self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, None, item)
                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_seg_lcd_device(item)
                self.update_dt_hw_c_end()

    def generate_eflash(self):
        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    wm_dt_hw_spim_dev_cfg_t spi_cfg;

    wm_dt_hw_flash_cfg_t flash_cfg;

    char *spi_device_name;
} wm_dt_hw_eflash_t;
    '''
        base_name = "flash"

        if not "eflash" in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name, "external_flash")

        for item in self.config:
            dev_name = item["dev_name"]
            if dev_name.startswith(base_name):
                device_type = dev_name[len(base_name)+1:]
                if device_type:

                    self.update_dt_table_ex(dev_name, "e" + dev_name, "external_flash", item)
                    self.update_dt_hw_c_begin("eflash", "e" + dev_name)
                    self.update_dt_hw_c_init_cfg(item)

                    self.wm_dt_hw_c += (
                        "    .flash_cfg = { .quad_spi = " + str(item["flash_cfg"]["quad_spi"]).lower() + " },\n"
                    )

                    self.wm_dt_hw_c += (
                        "    .spi_cfg = { .mode = " + str(item["spi_cfg"]["mode"]) + ",\n"
                        "                 .freq = " + str(item["spi_cfg"]["freq"]) + ",\n"
                        "                 .pin_cs = { .pin_num = WM_GPIO_NUM_" + str(item["spi_cfg"]["pin_cs"]["pin"]) + ",\n"
                        "                             .pin_mux = WM_GPIO_IOMUX_" + item["spi_cfg"]["pin_cs"]["fun"].upper() + ",\n"
                        "                             .pin_dir = WM_GPIO_DIR_" + item["spi_cfg"]["pin_cs"]["dir"].upper() + ",\n"
                        "                             .pin_pupd = WM_GPIO_" + item["spi_cfg"]["pin_cs"]["pupd"].upper() + " }\n"
                        "    },\n"
                    )

                    self.update_dt_hw_c_spi_device(item)
                    self.update_dt_hw_c_end()

    def generate_i2s(self):

        members = '''
typedef struct {
    bool extal_clock_en;     /**< @ref wm_drv_i2s_ioctl_args_t */
    uint32_t mclk_hz;        /**< @ref wm_drv_i2s_ioctl_args_t */
} wm_dt_hw_i2s_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    wm_dt_hw_i2s_cfg_t i2s_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_i2s_t;
'''
        base_name = "i2s"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)
                self.wm_dt_hw_c = self.wm_dt_hw_c + "    .i2s_cfg = { .extal_clock_en = " + str(item["i2s_cfg"]["extal_clock_en"]).lower() + ", .mclk_hz = " + str(item["i2s_cfg"]["mclk_hz"]) + " },\n"
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_dma_device(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

        self.generate_es8374()

    def generate_sdh(self):

        members = '''
typedef struct {
uint32_t clock_hz; /*< Clock frequency supports 1/2, 1/4, 1/6, 1/8, 1/10, 1/12, 1/14, 1/16 of the CPU clock,
                       it is recommended not to exceed 60MHz, and the CPU clock corresponding to
                       WM_RCC_TYPE_CPU can be obtained through the RCC module. */
wm_sdh_bus_width_t bus_width;
} wm_dt_hw_sdh_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    wm_dt_hw_sdh_cfg_t sdh_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_sdh_t;
'''
        if not "sdmmc" in self.kconfig and not "sdspi" in self.kconfig:
            return

        self.update_dt_hw_h(members)

        for item in self.config:
            dev_name = item["dev_name"]
            if dev_name == "sdmmc" or dev_name == "sdspi":
                if not dev_name in self.kconfig:
                    continue

                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_hw_c_begin("sdh", dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)

                if dev_name == "sdmmc":
                    self.update_dt_hw_c_ops_def("sdh_sdmmc")
                    self.update_dt_table(dev_name, "sdh_sdmmc", item)
                    clock_hz = str(item["sdh_cfg"]["clock_hz"])
                    bus_width = item["sdh_cfg"]["bus_width"]

                    if bus_width == 1:
                        bus_width_enum = "WM_SDH_BUS_WIDTH_1BIT"
                    elif bus_width == 4:
                        bus_width_enum = "WM_SDH_BUS_WIDTH_4BITS"
                    else:
                        bus_width_enum = "WM_SDH_BUS_WIDTH_MAX"

                    self.wm_dt_hw_c = self.wm_dt_hw_c + "    .sdh_cfg = { .clock_hz = " + clock_hz + ", .bus_width = " + bus_width_enum + " },\n"
                elif dev_name == "sdspi":
                    self.update_dt_hw_c_ops_def("sdh_spi")
                    self.update_dt_table(dev_name, "sdh_spi", item)
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_dma_device(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()


    def generate_sdio_slave(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t sdio_slave_reg_base;
    uint32_t wrapper_reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *rcc_device_name;
} wm_dt_hw_sdio_slave_t;
'''
        base_name = "sdio_slave"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if dev_name == base_name:
                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.wm_dt_hw_c = self.wm_dt_hw_c + "    .sdio_slave_reg_base = " + hex(item["reg_base"]) + ",\n"
                self.wm_dt_hw_c = self.wm_dt_hw_c + "    .wrapper_reg_base = " + hex(item["wrapper_reg_base"]) + ",\n"
                self.update_dt_hw_c_irq_cfg(item)
                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_tftlcd(self):

        if not "tftlcd" in self.kconfig:
            return

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    wm_dt_hw_spim_dev_cfg_t spi_cfg;

    wm_gpio_num_t io_lcd_reset;
    wm_gpio_num_t io_lcd_led;
    wm_gpio_num_t io_lcd_dcx;
    wm_gpio_num_t io_lcd_te;

    char *spi_device_name;
    char *gpio_device_name;
} wm_dt_hw_tft_lcd_spi_t;
'''

        writed = False
        for item in self.config:
            dev_name = item["dev_name"]
            if "nv3041a_spi" == dev_name or "st7735_spi" == dev_name or "gz035_spi" == dev_name:
                if not writed:
                    self.update_dt_hw_h(members)
                    writed = True

                self.update_dt_hw_c_ops_def("tft_lcd", "tft_lcd_" + dev_name.split("_")[0])
                self.update_dt_table(dev_name, "tft_lcd_" + dev_name.split("_")[0], item)
                self.update_dt_hw_c_begin("tft_lcd_spi", dev_name)
                self.update_dt_hw_c_init_cfg(item)

                mode = str(item["spi_cfg"]["mode"])
                freq = str(item["spi_cfg"]["freq"])
                pin_cs = item["spi_cfg"]["pin_cs"]

                pin_num = str(pin_cs.get("pin_num", "0"))
                pin_mux = pin_cs.get("pin_mux", "fun5")
                pin_dir = pin_cs.get("dir", "output")
                pin_pupd = pin_cs.get("pupd", "float")

                self.wm_dt_hw_c += (
                    "    .spi_cfg = { .mode = " + str(mode).upper() + ",\n"
                    "                 .freq = " + str(freq) + ",\n"
                    "                 .pin_cs = { .pin_num = WM_GPIO_NUM_" + str(pin_num) + ",\n"
                    "                             .pin_mux = WM_GPIO_IOMUX_" + pin_mux.upper() + ",\n"
                    "                             .pin_dir = WM_GPIO_DIR_" + pin_dir.upper() + ",\n"
                    "                             .pin_pupd = WM_GPIO_" + pin_pupd.upper() + " }\n"
                    "    },\n"
                )

                self.wm_dt_hw_c += "    .io_lcd_reset = WM_GPIO_NUM_" + str(item["pin_lcd_reset"]) + ",\n"
                self.wm_dt_hw_c += "    .io_lcd_led   = WM_GPIO_NUM_" + str(item["pin_lcd_led"]) + ",\n"
                self.wm_dt_hw_c += "    .io_lcd_dcx   = WM_GPIO_NUM_" + str(item["pin_lcd_dcx"]) + ",\n"
                if "pin_lcd_te" in item:
                    if item["pin_lcd_te"] != -1:
                        self.wm_dt_hw_c += "    .io_lcd_te    = WM_GPIO_NUM_" + str(item["pin_lcd_te"]) + ",\n"
                else:
                    self.wm_dt_hw_c += "    .io_lcd_te    = WM_GPIO_NUM_MAX" + ",\n"

                self.update_dt_hw_c_spi_device(item)
                self.update_dt_hw_c_gpio_device(item)
                self.update_dt_hw_c_end()

    def generate_wdt(self):

        members = '''
typedef struct {
    uint32_t counter_value;
} wm_dt_hw_wdt_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    wm_dt_hw_wdt_cfg_t wdt_cfg;
} wm_dt_hw_wdt_t;
'''
        base_name = "wdt"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_irq_cfg(item)
                self.wm_dt_hw_c = self.wm_dt_hw_c + "    .wdt_cfg = { .counter_value = " + str(item["wdt_cfg"]["counter_value"]) + " },\n"
                self.update_dt_hw_c_end()

    def generate_psram(self):
        members = '''
typedef struct {
    uint8_t spi_mode;  /**< Define SPI Mode, 0:spi, 1:qspi */
    uint32_t clock_hz; /**< Define the clock support for pSRAM controller, unit:Hz
                         *   psram clock = CPU clock / div, div is in [3,15] and the max psram clock is 80000000(80MHz)*/
} wm_dt_hw_psram_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;

    wm_dt_hw_psram_cfg_t psram_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_psram_t;
'''
        base_name = "psram"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_hw_c_pin_cfg(dev_name, item)
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)

                qspi_enabled = str(item["psram_cfg"]["qspi"]).lower()
                clock_hz = str(item["psram_cfg"]["clock_hz"])

                if qspi_enabled == "true":
                    qspi_type = "1"
                else:
                    qspi_type = "0"

                self.wm_dt_hw_c += (
                    "    .psram_cfg = { .spi_mode = " + qspi_type + ", .clock_hz = " + clock_hz + " },\n"
                )

                self.update_dt_hw_c_pin_cfg_link(dev_name, item)
                self.update_dt_hw_c_dma_device(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_crc(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    char *rcc_device_name;
} wm_dt_hw_crc_t;
'''
        base_name = "crc"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_crypto(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    char *rcc_device_name;
} wm_dt_hw_crypto_t;
'''
        base_name = "crypto"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_hash(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    char *rcc_device_name;
} wm_dt_hw_hash_t;
'''
        base_name = "hash"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_rng(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    char *rcc_device_name;
} wm_dt_hw_rng_t;
'''
        base_name = "rng"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_rsa(self):

        members = '''
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;

    char *rcc_device_name;
} wm_dt_hw_rsa_t;
'''
        base_name = "rsa"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def(base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, base_name, item)
                self.update_dt_hw_c_begin(base_name, dev_name)
                self.update_dt_hw_c_init_cfg(item)
                self.update_dt_hw_c_reg_base(item)
                self.update_dt_hw_c_rcc_device(item)
                self.update_dt_hw_c_end()

    def generate_es8374(self):

        members = '''
typedef struct {
    bool jack_output_gpio1;  /**< GPIO1 is used to output when jack source detect active */
    bool jack_output_gpio2;  /**< GPIO1 is used to output when jack source detect active */
    bool jack_output_level;  /**< the level output when detect active, should match the SOC GPIO */
    bool jack_detect_rin1;   /**< jack detect source from RIN1 */
    bool jack_detect_gpio1;  /**< The variable is mutually exclusive with the dmic clock */
    bool dmic_clk_out_gpio1; /**< set GPIO1 to DMIC clock output, is mutually exclusive with the jack */
    bool dmic;               /**< true if input with digital mic */
    bool lin1;               /**< true if input with LIN1, also need RIN1 to set */
    bool rin1;               /**< true if input with RIN1, also need LIN1 to set */
    bool lin2;               /**< true if input with LIN2, also need RIN2 to set */
    bool rin2;               /**< true if input with RIN2, also need LIN2 to set */
    bool monoout;            /**< if output with phonejack avaliable */
    bool spkout;             /**< if output with classD speaker avaliable */
    bool i2c;                /**< true if use i2c to control, else SPI */
    uint8_t address;         /**< the I2C address of es8374 */
} wm_dt_hw_codec_es8374_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    wm_dt_hw_codec_es8374_cfg_t es8374_cfg;

    uint8_t gpio1;
    uint8_t gpio2;

    char *i2s_device_name;
    char *gpio_device_name;
    char *i2c_device_name;
} wm_dt_hw_codec_i2s_t;
'''
        base_name = "es8374"

        if not base_name in self.kconfig:
            return

        self.update_dt_hw_h(members)
        self.update_dt_hw_c_ops_def("codec_i2s", "codec_i2s_" + base_name)

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:
                self.update_dt_table(dev_name, "codec_i2s_" + base_name, item)
                self.update_dt_hw_c_begin("codec_i2s", dev_name)
                self.update_dt_hw_c_init_cfg(item)

                self.wm_dt_hw_c += (
                    "    .es8374_cfg = { .dmic = " + str(item["es8374_cfg"]["dmic"]).lower() + ", \n                    .lin1 = " + str(item["es8374_cfg"]["lin1"]).lower() + ", \n                    .rin1 = " + str(item["es8374_cfg"]["rin1"]).lower() + ", \n                    .lin2 = " + str(item["es8374_cfg"]["lin2"]).lower() + ", \n                    .rin2 = " + str(item["es8374_cfg"]["rin2"]).lower() + ", \n                    .monoout = " + str(item["es8374_cfg"]["monoout"]).lower() + ", \n                    .spkout = " + str(item["es8374_cfg"]["spkout"]).lower() + ", \n                    .i2c = " + str(item["es8374_cfg"]["i2c"]).lower() + ", \n                    .address = " + hex(item["es8374_cfg"]["address"]) + " },\n"
                )

                self.update_dt_hw_c_i2s_device(item)
                self.update_dt_hw_c_gpio_device(item)
                self.update_dt_hw_c_i2c_device(item)
                self.update_dt_hw_c_end()

    def generate_file(self):
        ######## add begin ########
        wmdt.generate_common()
        wmdt.generate_rcc()
        wmdt.generate_dma()
        wmdt.generate_gpio()
        wmdt.generate_timer()
        wmdt.generate_uart()
        wmdt.generate_pwm()
        wmdt.generate_iflash()
        wmdt.generate_seg_lcd()
        wmdt.generate_i2c()
        wmdt.generate_spim()
        wmdt.generate_rtc()
        wmdt.generate_pmu()
        wmdt.generate_touch_sensor()
        wmdt.generate_adc()
        wmdt.generate_eflash()
        wmdt.generate_i2s()
        wmdt.generate_sdh()
        wmdt.generate_sdio_slave()
        wmdt.generate_tftlcd()
        wmdt.generate_wdt()
        wmdt.generate_psram()
        wmdt.generate_crc()
        wmdt.generate_crypto()
        wmdt.generate_hash()
        wmdt.generate_rng()
        wmdt.generate_rsa()

        # add others ...

        ######## add end ########

if __name__ == '__main__':
    parser_args = parser.parse_args()
    if not parser_args.input:
        print("invalid input file")
        exit(1)
    if not parser_args.output:
        print("invalid output path")
        exit(1)
    wmdt = wm_device_table(parser_args.input, parser_args.output, parser_args.config)
    if parser_args.debug:
        wmdt.debug_mode = True
    wmdt.load_config()
    wmdt.generate_file()
    wmdt.save_cfile()
