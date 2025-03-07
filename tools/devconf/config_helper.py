import os
import re
import sys
import tomlkit

class ConfigHelper:
    def __init__(self, *args):
        self.clock_config_data = {'chip_type':'W800', 'dev_name':'Clock', 'exist_in_config':[], 'clk_div_cpu':2, 'clk_div_wlan':3, 'clk_div_peri':3, 'clk_div_adc':40}

        self.uart0_config_data = {'chip_type':'W800', 'exist_in_config':[], 'baud':115200, 'parity':0, 'stopbits':0, 'databits':3, 'flowctrl':0, 'dev_name':'Uart0', 'initlevel':0, 'initpriority':0}
        self.uart1_config_data = {'chip_type':'W800', 'exist_in_config':[], 'baud':115200, 'parity':0, 'stopbits':0, 'databits':3, 'flowctrl':0, 'dev_name':'Uart1', 'initlevel':0, 'initpriority':0}
        self.uart2_config_data = {'chip_type':'W800', 'exist_in_config':[], 'baud':115200, 'parity':0, 'stopbits':0, 'databits':3, 'flowctrl':0, 'dev_name':'Uart2', 'initlevel':0, 'initpriority':0}
        self.uart3_config_data = {'chip_type':'W800', 'exist_in_config':[], 'baud':115200, 'parity':0, 'stopbits':0, 'databits':3, 'flowctrl':0, 'dev_name':'Uart3', 'initlevel':0, 'initpriority':0}
        self.uart4_config_data = {'chip_type':'W800', 'exist_in_config':[], 'baud':115200, 'parity':0, 'stopbits':0, 'databits':3, 'flowctrl':0, 'dev_name':'Uart4', 'initlevel':0, 'initpriority':0}
        self.uart5_config_data = {'chip_type':'W800', 'exist_in_config':[], 'baud':115200, 'parity':0, 'stopbits':0, 'databits':3, 'flowctrl':0, 'dev_name':'Uart5', 'initlevel':0, 'initpriority':0}

        self.timer0_config_data = {'chip_type':'W800', 'dev_name':'Timer0', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.timer1_config_data = {'chip_type':'W800', 'dev_name':'Timer1', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.timer2_config_data = {'chip_type':'W800', 'dev_name':'Timer2', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.timer3_config_data = {'chip_type':'W800', 'dev_name':'Timer3', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.timer4_config_data = {'chip_type':'W800', 'dev_name':'Timer4', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.timer5_config_data = {'chip_type':'W800', 'dev_name':'Timer5', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}

        self.iflash_config_data = {'chip_type':'W800', 'dev_name':'Internal_flash', 'exist_in_config':[], 'quad_spi':1, 'initlevel':0, 'initpriority':0}

        self.eflash_w25q_config_data = {'chip_type':'W800', 'dev_name':'External_flash_w25q', 'exist_in_config':[], 'quad_spi':0, 'freq':2000000, 'initlevel':0, 'initpriority':0}
        self.eflash_gd25q_config_data = {'chip_type':'W800', 'dev_name':'External_flash_gd25q', 'exist_in_config':[], 'quad_spi':0, 'freq':2000000, 'initlevel':0, 'initpriority':0}
        self.eflash_xt25f_config_data = {'chip_type':'W800', 'dev_name':'External_flash_xt25f', 'exist_in_config':[], 'quad_spi':0, 'freq':2000000, 'initlevel':0, 'initpriority':0}
        self.eflash_th25q_config_data = {'chip_type':'W800', 'dev_name':'External_flash_th25q', 'exist_in_config':[], 'quad_spi':0, 'freq':2000000, 'initlevel':0, 'initpriority':0}

        self.crypto_config_data = {'chip_type':'W800', 'dev_name':'CRYPTO', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.crc_config_data = {'chip_type':'W800', 'dev_name':'CRC', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.hash_config_data = {'chip_type':'W800', 'dev_name':'HASH', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.rng_config_data = {'chip_type':'W800', 'dev_name':'RNG', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.rsa_config_data = {'chip_type':'W800', 'dev_name':'RSA', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}

        self.sdmmc_config_data = {'chip_type':'W800', 'dev_name':'SDMMC', 'exist_in_config':[], 'clkdiv':[2,4,6,8,10,12,14,16], 'clock':40000000, 'initlevel':0, 'initpriority':0}
        self.sdspi_config_data = {'chip_type':'W800', 'dev_name':'SDSPI', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}

        self.nv3041a_config_data = {'chip_type':'W800', 'dev_name':'NV3041A', 'exist_in_config':[], 'freq':60000000, 'initlevel':0, 'initpriority':0}
        self.st7735_config_data = {'chip_type':'W800', 'dev_name':'ST7735', 'exist_in_config':[], 'freq':15000000, 'initlevel':0, 'initpriority':0}
        self.gz035_config_data = {'chip_type':'W800', 'dev_name':'GZ035', 'exist_in_config':[], 'freq':60000000, 'initlevel':0, 'initpriority':0}

        self.sdio_slave_config_data = {'chip_type':'W800', 'dev_name':'SDIO_Slave', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}

        self.i2c_config_data = {'chip_type':'W800', 'dev_name':'I2C', 'exist_in_config':[], 'initlevel':0, 'initpriority':0, 'addr_10_bits':0}
        self.eeprom0_config_data = {'chip_type':'W800', 'dev_name':'eeprom0', 'exist_in_config':[], 'initlevel':0, 'initpriority':0, 'size':256, 'i2c_addr':'0x50', 'page_size':16, 'addr_width':8, 'read_only':0, 'max_write_time_ms':5}
        self.eeprom1_config_data = {'chip_type':'W800', 'dev_name':'eeprom1', 'exist_in_config':[], 'initlevel':0, 'initpriority':0, 'size':256, 'i2c_addr':'0x52', 'page_size':16, 'addr_width':8, 'read_only':0, 'max_write_time_ms':10}

        self.spim_config_data = {'chip_type':'W800', 'dev_name':'SPI_Master', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}

        self.touch_sensor_config_data = {'chip_type':'W800', 'dev_name':'Touch_Sensor', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}

        self.touch_button_config_data = {'chip_type':'W800', 'dev_name':'Touch_Button', 'exist_in_config':[], 'initlevel':0, 'initpriority':0, 'channel':[]}
        for i in range(1, 16):
            self.touch_button_config_data['threshold' + str(i)] = 0

        self.pwm_config_data = {'chip_type':'W800', 'dev_name':'PWM', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.rtc_config_data = {'chip_type':'W800', 'dev_name':'RTC', 'exist_in_config':[], 'initlevel':0, 'initpriority':0}
        self.pmu_config_data = {'chip_type':'W800', 'dev_name':'PMU', 'exist_in_config':[], 'clk_src':0, 'initlevel':0, 'initpriority':0}
        self.i2s_config_data = {'chip_type':'W800', 'dev_name':'I2S', 'exist_in_config':[], 'extal_clock_en':0, 'extal_clock_hz':0, 'initlevel':0, 'initpriority':0}

        self.seg_lcd_config_data = {'chip_type':'W800', 'dev_name':'SEG_LCD', 'exist_in_config':[], 'frame_freq':0, 'com_num':0, 'initlevel':0, 'initpriority':0, 'pinnum':[], 'enable':[]}
        for i in range(0, 48):
            self.seg_lcd_config_data['enable'].append(0)
            self.seg_lcd_config_data['pinnum'].append(i)

        self.adc_config_data = {'chip_type':'W800', 'dev_name':'ADC', 'exist_in_config':[], 'initlevel':0, 'initpriority':0, 'enable':[], 'pga_gain1':[], 'pga_gain2':[], 'cmp_data':[], 'adc_cmp':[], 'cmp_pol':[], 'pinnum0':[], 'pinnum1':[]}
        for i in range(0, 6):
            self.adc_config_data['enable'].append(0)
            self.adc_config_data['pga_gain1'].append(0)
            self.adc_config_data['pga_gain2'].append(0)
            self.adc_config_data['cmp_data'].append(0)
            self.adc_config_data['adc_cmp'].append(0)
            self.adc_config_data['cmp_pol'].append(0)
            self.adc_config_data['pinnum0'].append(0)
            self.adc_config_data['pinnum1'].append(0)

        self.gpio_config_data = {'chip_type':'W800', 'dev_name':'GPIO', 'exist_in_config':[], 'initlevel':0, 'initpriority':0, 'pinnum':[], 'pindir':[], 'pinpupd':[], 'pinint':[], 'enable':[]}
        for i in range(0, 46):
            self.gpio_config_data['enable'].append(0)
            self.gpio_config_data['pinnum'].append(i)
            self.gpio_config_data['pindir'].append(0)
            self.gpio_config_data['pinpupd'].append(0)
            self.gpio_config_data['pinint'].append(0)

        self.wdt_config_data = {'chip_type':'W800', 'dev_name':'WDT', 'exist_in_config':[], 'counter_value':10000000, 'initlevel':0, 'initpriority':0}
        self.psram_config_data = {'chip_type':'W800', 'dev_name':'PSRAM', 'exist_in_config':[], 'mode': 1, 'clock':80000000, 'initlevel':0, 'initpriority':0}

        if (len(args) >= 1 and len(args[0])):
            self.toml_file = args[0]
            exist_in_config = []
            with open(self.toml_file, mode = "rt", encoding = "utf-8") as fp:
                config = tomlkit.load(fp)
                self.config = config

                for item in config["dev"]:
                    dev_name = item["dev_name"]

                    if dev_name == "rcc":
                        config_data = self.clock_config_data
                        exist_in_config.append(dev_name.lower())

                        pll = 480
                        for cfg in item["rcc_cfg"]:
                            if cfg["type"] == "cpu":
                                config_data["clk_div_cpu"] = pll / cfg["clock"]
                            elif cfg["type"] == "wlan":
                                config_data["clk_div_wlan"] = pll / cfg["clock"]
                            #elif cfg["type"] == "peripheral":
                            #    config_data["clk_div_peri"] = pll / cfg["clock"]
                        #for cfg in item["rcc_cfg"]:
                        #    if cfg["type"] == "sd_adc":
                        #        config_data["clk_div_adc"] = pll / config_data["clk_div_peri"] / cfg["clock"]

                        self.load_init_cfg(item, config_data)

                        self.clock_config_data = config_data

                    elif dev_name.startswith("uart"):
                        if dev_name == "uart0":
                            config_data = self.uart0_config_data
                        elif dev_name == "uart1":
                            config_data = self.uart1_config_data
                        elif dev_name == "uart2":
                            config_data = self.uart2_config_data
                        elif dev_name == "uart3":
                            config_data = self.uart3_config_data
                        elif dev_name == "uart4":
                            config_data = self.uart4_config_data
                        elif dev_name == "uart5":
                            config_data = self.uart5_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["baud"] = item["uart_cfg"]["baudrate"]

                        if item["uart_cfg"]["parity"] == "none":
                            config_data["parity"] = 0
                        elif  item["uart_cfg"]["parity"] == "even":
                            config_data["parity"] = 1
                        elif  item["uart_cfg"]["parity"] == "odd":
                            config_data["parity"] = 2
                        else:
                            config_data["parity"] = 0

                        config_data["stopbits"] = item["uart_cfg"]["stop_bits"] - 1
                        config_data["databits"] = 3

                        if item["uart_cfg"]["flow_ctrl"] == "none":
                            config_data["flowctrl"] = 0
                        elif item["uart_cfg"]["flow_ctrl"] == "rts":
                            config_data["flowctrl"] = 1
                            config_data["pinrts"] = item["pin_cfg"][2]["pin"]
                        elif item["uart_cfg"]["flow_ctrl"] == "cts":
                            config_data["flowctrl"] = 2
                            config_data["pincts"] = item["pin_cfg"][2]["pin"]
                        elif item["uart_cfg"]["flow_ctrl"] == "rts_cts":
                            config_data["flowctrl"] = 3
                            config_data["pinrts"] = item["pin_cfg"][2]["pin"]
                            config_data["pincts"] = item["pin_cfg"][3]["pin"]
                        else:
                            config_data["flowctrl"] = 0

                        config_data["pintx"] = item["pin_cfg"][0]["pin"]
                        config_data["pinrx"] = item["pin_cfg"][1]["pin"]

                        if "dma_device" in item:
                            config_data["enable_dma"] = 1

                        if item["init_cfg"]["init_level"] == "system":
                            config_data["initlevel"] = 1
                        elif item["init_cfg"]["init_level"] == "app":
                            config_data["initlevel"] = 0
                        else:
                            config_data["initlevel"] = 0

                        config_data["initpriority"] = item["init_cfg"]["init_priority"]

                        if dev_name == "uart0":
                            self.uart0_config_data = config_data
                        elif dev_name == "uart1":
                            self.uart1_config_data = config_data
                        elif dev_name == "uart2":
                            self.uart2_config_data = config_data
                        elif dev_name == "uart3":
                            self.uart3_config_data = config_data
                        elif dev_name == "uart4":
                            self.uart4_config_data = config_data
                        elif dev_name == "uart5":
                            self.uart5_config_data = config_data

                    elif dev_name.startswith("timer"):
                        if dev_name == "timer0":
                            config_data = self.timer0_config_data
                        elif dev_name == "timer1":
                            config_data = self.timer1_config_data
                        elif dev_name == "timer2":
                            config_data = self.timer2_config_data
                        elif dev_name == "timer3":
                            config_data = self.timer3_config_data
                        elif dev_name == "timer4":
                            config_data = self.timer4_config_data
                        elif dev_name == "timer5":
                            config_data = self.timer5_config_data
                        exist_in_config.append(dev_name.lower())

                        self.load_init_cfg(item, config_data)

                        if dev_name == "timer0":
                            self.timer0_config_data = config_data
                        elif dev_name == "timer1":
                            self.timer1_config_data = config_data
                        elif dev_name == "timer2":
                            self.timer2_config_data = config_data
                        elif dev_name == "timer3":
                            self.timer3_config_data = config_data
                        elif dev_name == "timer4":
                            self.timer4_config_data = config_data
                        elif dev_name == "timer5":
                            self.timer5_config_data = config_data

                    elif dev_name == "iflash":
                        config_data = self.iflash_config_data
                        exist_in_config.append(dev_name.lower())

                        if item["flash_cfg"]["quad_spi"]:
                            config_data["quad_spi"] = 1
                        else:
                            config_data["quad_spi"] = 0

                        self.load_init_cfg(item, config_data)

                        self.iflash_config_data = config_data

                    elif dev_name.startswith("flash_"):
                        if dev_name == "flash_w25q":
                            config_data = self.eflash_w25q_config_data
                        elif dev_name == "flash_gd25q":
                            config_data = self.eflash_gd25q_config_data
                        elif dev_name == "flash_xt25f":
                            config_data = self.eflash_xt25f_config_data
                        elif dev_name == "flash_th25q":
                            config_data = self.eflash_th25q_config_data
                        exist_in_config.append(dev_name.lower())

                        if item["flash_cfg"]["quad_spi"]:
                            config_data["quad_spi"] = 1
                        else:
                            config_data["quad_spi"] = 0

                        config_data["mode"] = item["spi_cfg"]["mode"]
                        config_data["freq"] = item["spi_cfg"]["freq"]
                        config_data["cspinnum"] = item["spi_cfg"]["pin_cs"]["pin"]

                        self.load_init_cfg(item, config_data)

                        if dev_name == "flash_w25q":
                            self.eflash_w25q_config_data = config_data
                        elif dev_name == "flash_gd25q":
                            self.eflash_gd25q_config_data = config_data
                        elif dev_name == "flash_xt25f":
                            self.eflash_xt25f_config_data = config_data
                        elif dev_name == "flash_th25q":
                            self.eflash_th25q_config_data = config_data

                    elif dev_name == "crypto":
                        config_data = self.crypto_config_data
                        exist_in_config.append(dev_name.lower())
                        self.load_init_cfg(item, config_data)
                        self.crypto_config_data = config_data

                    elif dev_name == "crc":
                        config_data = self.crc_config_data
                        exist_in_config.append(dev_name.lower())
                        self.load_init_cfg(item, config_data)
                        self.crc_config_data = config_data

                    elif dev_name == "hash":
                        config_data = self.hash_config_data
                        exist_in_config.append(dev_name.lower())
                        self.load_init_cfg(item, config_data)
                        self.hash_config_data = config_data

                    elif dev_name == "rng":
                        config_data = self.rng_config_data
                        exist_in_config.append(dev_name.lower())
                        self.load_init_cfg(item, config_data)
                        self.rng_config_data = config_data

                    elif dev_name == "rsa":
                        config_data = self.rsa_config_data
                        exist_in_config.append(dev_name.lower())
                        self.load_init_cfg(item, config_data)
                        self.rsa_config_data = config_data

                    elif dev_name == "sdmmc":
                        config_data = self.sdmmc_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["clock"] = item["sdh_cfg"]["clock_hz"]
                        if item["sdh_cfg"]["bus_width"] == 4:
                            config_data["bus_width"] = 1
                        else:
                            config_data["bus_width"] = 0

                        config_data["clkpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["cmdpinnum"] = item["pin_cfg"][1]["pin"]
                        config_data["dat0pinnum"] = item["pin_cfg"][2]["pin"]
                        config_data["dat1pinnum"] = item["pin_cfg"][3]["pin"]
                        config_data["dat2pinnum"] = item["pin_cfg"][4]["pin"]
                        config_data["dat3pinnum"] = item["pin_cfg"][5]["pin"]

                        self.load_init_cfg(item, config_data)

                        self.sdmmc_config_data = config_data

                    elif dev_name == "sdspi":
                        config_data = self.sdspi_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["clkpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["cmdpinnum"] = item["pin_cfg"][1]["pin"]

                        self.load_init_cfg(item, config_data)

                        self.sdspi_config_data = config_data

                    elif dev_name == "nv3041a_spi":
                        config_data = self.nv3041a_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["mode"] = item["spi_cfg"]["mode"]
                        config_data["freq"] = item["spi_cfg"]["freq"]
                        config_data["cspinnum"] = item["spi_cfg"]["pin_cs"]["pin_num"]
                        config_data["resetpinnum"] = item["pin_lcd_reset"]
                        config_data["ledpinnum"] = item["pin_lcd_led"]
                        config_data["dcxpinnum"] = item["pin_lcd_dcx"]

                        self.load_init_cfg(item, config_data)

                        self.nv3041a_config_data = config_data

                    elif dev_name == "st7735_spi":
                        config_data = self.st7735_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["mode"] = item["spi_cfg"]["mode"]
                        config_data["freq"] = item["spi_cfg"]["freq"]
                        config_data["cspinnum"] = item["spi_cfg"]["pin_cs"]["pin_num"]
                        config_data["resetpinnum"] = item["pin_lcd_reset"]
                        config_data["ledpinnum"] = item["pin_lcd_led"]
                        config_data["dcxpinnum"] = item["pin_lcd_dcx"]

                        self.load_init_cfg(item, config_data)

                        self.st7735_config_data = config_data

                    elif dev_name == "gz035_spi":
                        config_data = self.gz035_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["mode"] = item["spi_cfg"]["mode"]
                        config_data["freq"] = item["spi_cfg"]["freq"]
                        config_data["cspinnum"] = item["spi_cfg"]["pin_cs"]["pin_num"]
                        config_data["resetpinnum"] = item["pin_lcd_reset"]
                        config_data["ledpinnum"] = item["pin_lcd_led"]
                        config_data["dcxpinnum"] = item["pin_lcd_dcx"]
                        config_data["tepinnum"] = item["pin_lcd_te"]

                        self.load_init_cfg(item, config_data)

                        self.gz035_config_data = config_data

                    elif dev_name == "sdio_slave":
                        config_data = self.sdio_slave_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["clkpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["cmdpinnum"] = item["pin_cfg"][1]["pin"]
                        config_data["dat0pinnum"] = item["pin_cfg"][2]["pin"]
                        config_data["dat1pinnum"] = item["pin_cfg"][3]["pin"]
                        config_data["dat2pinnum"] = item["pin_cfg"][4]["pin"]
                        config_data["dat3pinnum"] = item["pin_cfg"][5]["pin"]

                        self.load_init_cfg(item, config_data)

                        self.sdio_slave_config_data = config_data

                    elif dev_name == "i2c":
                        config_data = self.i2c_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["clock"] = item["i2c_cfg"]["max_clock"]
                        if item["i2c_cfg"]["addr_10_bits"]:
                            config_data["addr_10_bits"] = 1
                        else:
                            config_data["addr_10_bits"] = 0
                        config_data["sclpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["sdapinnum"] = item["pin_cfg"][1]["pin"]

                        self.load_init_cfg(item, config_data)

                        self.i2c_config_data = config_data

                    elif dev_name == "eeprom0":
                        config_data = self.eeprom0_config_data
                        exist_in_config.append(dev_name.lower())
                        config_data["speed_hz"] = item["speed_hz"]
                        config_data["size"] = item["size"]
                        config_data["i2c_addr"] = hex(item["i2c_addr"])
                        config_data["page_size"] = item["page_size"]
                        config_data["addr_width"] = item["addr_width"]
                        if item["read_only"]:
                            config_data["read_only"] = 1
                        else:
                            config_data["read_only"] = 0
                        config_data["max_write_time_ms"] = item["max_write_time_ms"]

                        if "pin_cfg" in item:
                            config_data["enable"] = 1
                            config_data["wppinnum"] = item["pin_cfg"]["pin"]
                        else:
                            config_data["enable"] = 0

                        self.load_init_cfg(item, config_data)

                        self.eeprom0_config_data = config_data

                    elif dev_name == "eeprom1":
                        config_data = self.eeprom1_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["speed_hz"] = item["speed_hz"]
                        config_data["size"] = item["size"]
                        config_data["i2c_addr"] = hex(item["i2c_addr"])
                        config_data["page_size"] = item["page_size"]
                        config_data["addr_width"] = item["addr_width"]
                        if item["read_only"]:
                            config_data["read_only"] = 1
                        else:
                            config_data["read_only"] = 0
                        config_data["max_write_time_ms"] = item["max_write_time_ms"]

                        if "pin_cfg" in item:
                            config_data["enable"] = 1
                            config_data["wppinnum"] = item["pin_cfg"]["pin"]
                        else:
                            config_data["enable"] = 0

                        self.load_init_cfg(item, config_data)

                        self.eeprom1_config_data = config_data

                    elif dev_name == "spim":
                        config_data = self.spim_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["clkpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["dipinnum"] = item["pin_cfg"][1]["pin"]
                        config_data["dopinnum"] = item["pin_cfg"][2]["pin"]

                        self.load_init_cfg(item, config_data)

                        self.spim_config_data = config_data

                    elif dev_name == "touch_sensor":
                        config_data = self.touch_sensor_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["cmodpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["cdcpinnum"] = item["pin_cfg"][1]["pin"]

                        self.load_init_cfg(item, config_data)

                        self.touch_sensor_config_data = config_data

                    elif dev_name == "touch_button":
                        config_data = self.touch_button_config_data
                        exist_in_config.append(dev_name.lower())

                        for cfg in item["touch_button_cfg"]:
                            config_data["enable" + str(cfg["key_num"])] = 1
                            config_data["threshold" + str(cfg["key_num"])] = cfg["threshold"]
                            config_data["pinnum" + str(cfg["key_num"])] = item["pin_cfg"][item["touch_button_cfg"].index(cfg)]["pin"]

                        self.load_init_cfg(item, config_data)

                        self.touch_button_config_data = config_data

                    elif dev_name == "pwm":
                        config_data = self.pwm_config_data
                        exist_in_config.append(dev_name.lower())

                        pwm_channel_0 = [2, 10, 16, 28, 35]
                        pwm_channel_1 = [3, 11, 17, 29, 36]
                        pwm_channel_2 = [0, 12, 18, 30, 40]
                        pwm_channel_3 = [1, 13, 19, 31, 41]
                        pwm_channel_4 = [4,  7, 14, 32, 42]
                        pwm_channel_b = [5,  8, 15, 24, 33]
                        for cfg in item["pin_cfg"]:
                            if cfg["pin"] in pwm_channel_0:
                                config_data["enable0_" + str(pwm_channel_0.index(cfg["pin"]) + 1)] = 1
                                config_data["pinnum0_" + str(pwm_channel_0.index(cfg["pin"]) + 1)] = cfg["pin"]
                            elif cfg["pin"] in pwm_channel_1:
                                config_data["enable1_" + str(pwm_channel_1.index(cfg["pin"]) + 1)] = 1
                                config_data["pinnum1_" + str(pwm_channel_1.index(cfg["pin"]) + 1)] = cfg["pin"]
                            elif cfg["pin"] in pwm_channel_2:
                                config_data["enable2_" + str(pwm_channel_2.index(cfg["pin"]) + 1)] = 1
                                config_data["pinnum2_" + str(pwm_channel_2.index(cfg["pin"]) + 1)] = cfg["pin"]
                            elif cfg["pin"] in pwm_channel_3:
                                config_data["enable3_" + str(pwm_channel_3.index(cfg["pin"]) + 1)] = 1
                                config_data["pinnum3_" + str(pwm_channel_3.index(cfg["pin"]) + 1)] = cfg["pin"]
                            elif cfg["pin"] in pwm_channel_4:
                                config_data["enable4_" + str(pwm_channel_4.index(cfg["pin"]) + 1)] = 1
                                config_data["pinnum4_" + str(pwm_channel_4.index(cfg["pin"]) + 1)] = cfg["pin"]
                            elif cfg["pin"] in pwm_channel_b:
                                config_data["enable5_" + str(pwm_channel_b.index(cfg["pin"]) + 1)] = 1
                                config_data["pinnum5_" + str(pwm_channel_b.index(cfg["pin"]) + 1)] = cfg["pin"]

                        self.load_init_cfg(item, config_data)

                        self.pwm_config_data = config_data

                    elif dev_name == "rtc":
                        config_data = self.rtc_config_data
                        exist_in_config.append(dev_name.lower())
                        self.load_init_cfg(item, config_data)
                        self.rtc_config_data = config_data

                    elif dev_name == "pmu":
                        config_data = self.pmu_config_data
                        exist_in_config.append(dev_name.lower())

                        if item["clk_src"] == "internal":
                            config_data["clk_src"] = 0
                        elif item["clk_src"] == "external":
                            config_data["clk_src"] = 1
                        else:
                            config_data["clk_src"] = 0

                        self.load_init_cfg(item, config_data)
                        self.pmu_config_data = config_data

                    elif dev_name == "i2s":
                        config_data = self.i2s_config_data
                        exist_in_config.append(dev_name.lower())

                        if item["i2s_cfg"]["extal_clock_en"]:
                            config_data["extal_clock_en"] = 1
                        else:
                            config_data["extal_clock_en"] = 0
                        config_data["extal_clock_hz"] = item["i2s_cfg"]["mclk_hz"]

                        config_data["bclkpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["lrclkpinnum"] = item["pin_cfg"][1]["pin"]

                        mclk_pinnum = [0, 7, 33]
                        mclk_pinfun = ["fun1", "fun3", "fun4"]

                        di_pinnum   = [7, 11, 26, 31]
                        di_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                        do_pinnum   = [0, 10, 27, 30]
                        do_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                        config_data["enable_mclk"] = 0
                        config_data["enable_di"] = 0
                        config_data["enable_do"] = 0

                        for pin_cfg in item["pin_cfg"]:
                            for i in range(0, len(mclk_pinnum)):
                                if pin_cfg["pin"] == mclk_pinnum[i] and pin_cfg["fun"] == mclk_pinfun[i]:
                                    config_data["enable_mclk"] = 1
                                    config_data["mclkpinnum"] = pin_cfg["pin"]
                                    break
                            for i in range(0, len(di_pinnum)):
                                if pin_cfg["pin"] == di_pinnum[i] and pin_cfg["fun"] == di_pinfun[i]:
                                    config_data["enable_di"] = 1
                                    config_data["dipinnum"] = pin_cfg["pin"]
                                    break
                            for i in range(0, len(do_pinnum)):
                                if pin_cfg["pin"] == do_pinnum[i] and pin_cfg["fun"] == do_pinfun[i]:
                                    config_data["enable_do"] = 1
                                    config_data["dopinnum"] = pin_cfg["pin"]
                                    break

                        self.load_init_cfg(item, config_data)
                        self.i2s_config_data = config_data

                    elif dev_name == "seg_lcd":
                        config_data = self.seg_lcd_config_data
                        exist_in_config.append(dev_name.lower())

                        if item["seg_lcd_cfg"]["duty_sel"] == "1/2":
                            config_data["duty_sel"] = 1
                        elif item["seg_lcd_cfg"]["duty_sel"] == "1/3":
                            config_data["duty_sel"] = 2
                        elif item["seg_lcd_cfg"]["duty_sel"] == "1/4":
                            config_data["duty_sel"] = 3
                        elif item["seg_lcd_cfg"]["duty_sel"] == "1/5":
                            config_data["duty_sel"] = 4
                        elif item["seg_lcd_cfg"]["duty_sel"] == "1/6":
                            config_data["duty_sel"] = 5
                        elif item["seg_lcd_cfg"]["duty_sel"] == "1/7":
                            config_data["duty_sel"] = 6
                        elif item["seg_lcd_cfg"]["duty_sel"] == "1/8":
                            config_data["duty_sel"] = 7
                        else:
                            config_data["duty_sel"] = 0

                        if item["seg_lcd_cfg"]["vlcd_cc"] == "2.9v".lower():
                            config_data["vlcd_cc"] = 1
                        elif item["seg_lcd_cfg"]["vlcd_cc"] == "3.1v".lower():
                            config_data["vlcd_cc"] = 2
                        elif item["seg_lcd_cfg"]["vlcd_cc"] == "3.3v".lower():
                            config_data["vlcd_cc"] = 3
                        else:#2.7v
                            config_data["vlcd_cc"] = 0

                        if item["seg_lcd_cfg"]["bias"] == "1/2":
                            config_data["bias"] = 1
                        elif item["seg_lcd_cfg"]["bias"] == "1/3":
                            config_data["bias"] = 2
                        elif item["seg_lcd_cfg"]["bias"] == "static".lower():
                            config_data["bias"] = 3
                        else:#1/4
                            config_data["bias"] = 0

                        if item["seg_lcd_cfg"]["hd"] == "high".lower():
                            config_data["hd"] = 1
                        else:
                            config_data["hd"] = 0

                        config_data["frame_freq"] = item["seg_lcd_cfg"]["frame_freq"]
                        config_data["com_num"] = item["seg_lcd_cfg"]["com_num"]

                        for cfg in item["pin_cfg"]:
                            config_data["enable"][cfg["pin"]] = 1
                            config_data["pinnum"][cfg["pin"]] = cfg["pin"]

                        self.load_init_cfg(item, config_data)

                        self.seg_lcd_config_data = config_data

                    elif dev_name == "adc":
                        config_data = self.adc_config_data
                        exist_in_config.append(dev_name.lower())

                        index = 0
                        for cfg in item["adc_cfg"]:
                            if cfg["adc_channel"] == 0:
                                config_data["enable"][0] = 1
                                if "cmp_pol" in cfg:
                                    if cfg["cmp_pol"]:
                                        config_data["cmp_pol"][0] = 1
                                    else:
                                        config_data["cmp_pol"][0] = 0
                                if cfg["adc_cmp"]:
                                    config_data["adc_cmp"][0] = 1
                                else:
                                    config_data["adc_cmp"][0] = 0
                                if "cmp_data" in cfg:
                                    config_data["cmp_data"][0] = cfg["cmp_data"]
                                else:
                                    config_data["cmp_data"][0] = 0
                                config_data["pga_gain1"][0] = cfg["pga_gain1"][5:]
                                config_data["pga_gain2"][0] = cfg["pga_gain2"][5:]
                                config_data["pinnum0"][0] = item["pin_cfg"][index]["pin"]
                            elif cfg["adc_channel"] == 1:
                                config_data["enable"][1] = 1
                                if "cmp_pol" in cfg:
                                    if cfg["cmp_pol"]:
                                        config_data["cmp_pol"][1] = 1
                                    else:
                                        config_data["cmp_pol"][1] = 0
                                if cfg["adc_cmp"]:
                                    config_data["adc_cmp"][1] = 1
                                    config_data["cmp_data"][1] = cfg["cmp_data"]
                                else:
                                    config_data["adc_cmp"][1] = 0
                                    config_data["cmp_data"][1] = 0
                                config_data["pga_gain1"][1] = cfg["pga_gain1"][5:]
                                config_data["pga_gain2"][1] = cfg["pga_gain2"][5:]
                                config_data["pinnum0"][1] = item["pin_cfg"][index]["pin"]
                            elif cfg["adc_channel"] == 2:
                                config_data["enable"][2] = 1
                                if "cmp_pol" in cfg:
                                    if cfg["cmp_pol"]:
                                        config_data["cmp_pol"][2] = 1
                                    else:
                                        config_data["cmp_pol"][2] = 0
                                if cfg["adc_cmp"]:
                                    config_data["adc_cmp"][2] = 1
                                    config_data["cmp_data"][2] = cfg["cmp_data"]
                                else:
                                    config_data["adc_cmp"][2] = 0
                                    config_data["cmp_data"][2] = 0
                                config_data["pga_gain1"][2] = cfg["pga_gain1"][5:]
                                config_data["pga_gain2"][2] = cfg["pga_gain2"][5:]
                                config_data["pinnum0"][2] = item["pin_cfg"][index]["pin"]
                            elif cfg["adc_channel"] == 3:
                                config_data["enable"][3] = 1
                                if "cmp_pol" in cfg:
                                    if cfg["cmp_pol"]:
                                        config_data["cmp_pol"][3] = 1
                                    else:
                                        config_data["cmp_pol"][3] = 0
                                if cfg["adc_cmp"]:
                                    config_data["adc_cmp"][3] = 1
                                    config_data["cmp_data"][3] = cfg["cmp_data"]
                                else:
                                    config_data["adc_cmp"][3] = 0
                                    config_data["cmp_data"][3] = 0
                                config_data["pga_gain1"][3] = cfg["pga_gain1"][5:]
                                config_data["pga_gain2"][3] = cfg["pga_gain2"][5:]
                                config_data["pinnum0"][3] = item["pin_cfg"][index]["pin"]
                            elif cfg["adc_channel"] == 8:
                                config_data["enable"][4] = 1
                                if "cmp_pol" in cfg:
                                    if cfg["cmp_pol"]:
                                        config_data["cmp_pol"][4] = 1
                                    else:
                                        config_data["cmp_pol"][4] = 0
                                if cfg["adc_cmp"]:
                                    config_data["adc_cmp"][4] = 1
                                    config_data["cmp_data"][4] = cfg["cmp_data"]
                                else:
                                    config_data["adc_cmp"][4] = 0
                                    config_data["cmp_data"][4] = 0
                                config_data["pga_gain1"][4] = cfg["pga_gain1"][5:]
                                config_data["pga_gain2"][4] = cfg["pga_gain2"][5:]
                                config_data["pinnum0"][4] = item["pin_cfg"][index]["pin"]
                                config_data["pinnum1"][4] = item["pin_cfg"][index + 1]["pin"]
                                index = index + 1
                            elif cfg["adc_channel"] == 9:
                                config_data["enable"][5] = 1
                                if "cmp_pol" in cfg:
                                    if cfg["cmp_pol"]:
                                        config_data["cmp_pol"][5] = 1
                                    else:
                                        config_data["cmp_pol"][5] = 0
                                if cfg["adc_cmp"]:
                                    config_data["adc_cmp"][5] = 1
                                    config_data["cmp_data"][5] = cfg["cmp_data"]
                                else:
                                    config_data["adc_cmp"][5] = 0
                                    config_data["cmp_data"][5] = 0
                                config_data["pga_gain1"][5] = cfg["pga_gain1"][5:]
                                config_data["pga_gain2"][5] = cfg["pga_gain2"][5:]
                                config_data["pinnum0"][5] = item["pin_cfg"][index]["pin"]
                                config_data["pinnum1"][5] = item["pin_cfg"][index + 1]["pin"]
                                index = index + 1
                            index = index + 1

                        self.load_init_cfg(item, config_data)
                        self.adc_config_data = config_data

                    elif dev_name == "gpio":
                        config_data = self.gpio_config_data
                        exist_in_config.append(dev_name.lower())

                        for cfg in item["gpio_cfg"]:
                            config_data['enable'][cfg["pin"]] = 1
                            config_data['pinnum'][cfg["pin"]] = cfg["pin"]
                            if cfg["dir"] == "output":
                                config_data['pindir'][cfg["pin"]] = 1
                            else:
                                config_data['pindir'][cfg["pin"]] = 0
                            if cfg["pupd"] == "pull_up":
                                config_data['pinpupd'][cfg["pin"]] = 1
                            elif cfg["pupd"] == "pull_down":
                                config_data['pinpupd'][cfg["pin"]] = 2
                            else:
                                config_data['pinpupd'][cfg["pin"]] = 0
                            if cfg["interrupt_mode"] == "falling_edge":
                                config_data['pinint'][cfg["pin"]] = 1
                            elif cfg["interrupt_mode"] == "rising_edge":
                                config_data['pinint'][cfg["pin"]] = 2
                            elif cfg["interrupt_mode"] == "double_edge":
                                config_data['pinint'][cfg["pin"]] = 3
                            elif cfg["interrupt_mode"] == "low_level":
                                config_data['pinint'][cfg["pin"]] = 4
                            elif cfg["interrupt_mode"] == "high_level":
                                config_data['pinint'][cfg["pin"]] = 5
                            else:
                                config_data['pinint'][cfg["pin"]] = 0

                        self.load_init_cfg(item, config_data)
                        self.gpio_config_data = config_data

                    elif dev_name == "wdt":
                        config_data = self.wdt_config_data
                        exist_in_config.append(dev_name.lower())

                        config_data["counter_value"] = item["wdt_cfg"]["counter_value"]

                        self.load_init_cfg(item, config_data)
                        self.wdt_config_data = config_data

                    elif dev_name == "psram":
                        config_data = self.psram_config_data
                        exist_in_config.append(dev_name.lower())

                        if item["psram_cfg"]["qspi"]:
                            config_data["mode"] = 1
                        else:
                            config_data["mode"] = 0
                        config_data["clock"] = item["psram_cfg"]["clock_hz"]
                        config_data["ckpinnum"] = item["pin_cfg"][0]["pin"]
                        config_data["cspinnum"] = item["pin_cfg"][1]["pin"]
                        config_data["d0pinnum"] = item["pin_cfg"][2]["pin"]
                        config_data["d1pinnum"] = item["pin_cfg"][3]["pin"]
                        config_data["d2pinnum"] = item["pin_cfg"][4]["pin"]
                        config_data["d3pinnum"] = item["pin_cfg"][5]["pin"]

                        self.load_init_cfg(item, config_data)
                        self.psram_config_data = config_data

                self.set_devices_exist_in_config(exist_in_config)

        kconfig_default = 0
        kconfig = {"wifi":kconfig_default, "uart":kconfig_default, "timer":kconfig_default, "iflash":kconfig_default, "eflash":kconfig_default, "crypto":kconfig_default, "crc":kconfig_default, "hash":kconfig_default, "rng":kconfig_default, "rsa":kconfig_default, "sdmmc":kconfig_default, "sdspi":kconfig_default, "tftlcd":kconfig_default, "nv3041a":kconfig_default, "st7735":kconfig_default, "gz035":kconfig_default, "sdio_slave":kconfig_default,"i2c":kconfig_default, "spim":kconfig_default, "touch_sensor":kconfig_default, "pwm":kconfig_default, "rtc":kconfig_default, "pmu":kconfig_default, "i2s":kconfig_default, "seg_lcd":kconfig_default, "adc":kconfig_default, "gpio":kconfig_default, "wdt":kconfig_default, "psram":kconfig_default}
        self.set_devices_kconfig(kconfig)
        if (len(args) > 1 and len(args[1])):
            self.kconfig_file = args[1]
            chip_type = "W800"
            with open(self.kconfig_file, "r", encoding="utf-8") as file:
                #kconfig_default = 0
                #kconfig = {"wifi":kconfig_default, "uart":kconfig_default, "timer":kconfig_default, "iflash":kconfig_default, "eflash":kconfig_default, "crypto":kconfig_default, "crc":kconfig_default, "hash":kconfig_default, "rng":kconfig_default, "rsa":kconfig_default, "sdmmc":kconfig_default, "sdspi":kconfig_default, "tftlcd":kconfig_default, "nv3041a":kconfig_default, "st7735":kconfig_default, "gz035":kconfig_default, "sdio_slave":kconfig_default, "i2c":kconfig_default, "spim":kconfig_default, "touch_sensor":kconfig_default, "pwm":kconfig_default, "rtc":kconfig_default, "pmu":1, "i2s":kconfig_default, "seg_lcd":kconfig_default, "adc":kconfig_default, "gpio":kconfig_default, "wdt":kconfig_default, "psram":kconfig_default}
                for line in file:
                    line = line.strip()

                    if line.startswith("#"):
                        continue

                    if line.startswith("CONFIG_CHIP_NAME"):
                        match = re.search(r'="([^"]*)"', line)
                        if match:
                            chip_type = match.group(1)

                    if line == "CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED=y":
                        kconfig["iflash"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED=y":
                        kconfig["eflash"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_TIMER_ENABLED=y":
                        kconfig["timer"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_UART_ENABLED=y":
                        kconfig["uart"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_GPIO_ENABLED=y":
                        kconfig["gpio"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y":
                        kconfig["psram"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_PWM_ENABLED=y":
                        kconfig["pwm"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_RTC_ENABLED=y":
                        kconfig["rtc"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED=y":
                        kconfig["sdmmc"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED=y":
                        kconfig["sdspi"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED=y":
                        kconfig["sdio_slave"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_SPIM_ENABLED=y":
                        kconfig["spim"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_WDT_ENABLED=y":
                        kconfig["wdt"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED=y":
                        kconfig["tftlcd"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_LCD_ST7735_SPI=y":
                        kconfig["st7735"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_LCD_GZ035_SPI=y":
                        kconfig["gz035"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y":
                        kconfig["nv3041a"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_ADC_ENABLED=y":
                        kconfig["adc"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_CRC_ENABLED=y":
                        kconfig["crc"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED=y":
                        kconfig["crypto"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_HASH_ENABLED=y":
                        kconfig["hash"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_RNG_ENABLED=y":
                        kconfig["rng"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_RSA_ENABLED=y":
                        kconfig["rsa"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED=y":
                        kconfig["touch_sensor"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_I2S_ENABLED=y":
                        kconfig["i2s"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED=y":
                        kconfig["seg_lcd"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_I2C_ENABLED=y":
                        kconfig["i2c"] = 1
                    elif line == "CONFIG_COMPONENT_WIFI_ENABLED=y":
                        kconfig["wifi"] = 1
                    elif line == "CONFIG_COMPONENT_DRIVER_PMU_ENABLED=y":
                        kconfig["pmu"] = 1

                self.set_devices_kconfig(kconfig)
                self.clock_config_data["chip_type"] = chip_type
                self.iflash_config_data["chip_type"] = chip_type
                self.eflash_w25q_config_data["chip_type"] = chip_type
                self.eflash_gd25q_config_data["chip_type"] = chip_type
                self.eflash_xt25f_config_data["chip_type"] = chip_type
                self.eflash_th25q_config_data["chip_type"] = chip_type
                self.timer0_config_data["chip_type"] = chip_type
                self.timer1_config_data["chip_type"] = chip_type
                self.timer2_config_data["chip_type"] = chip_type
                self.timer3_config_data["chip_type"] = chip_type
                self.timer4_config_data["chip_type"] = chip_type
                self.timer5_config_data["chip_type"] = chip_type
                self.uart0_config_data["chip_type"] = chip_type
                self.uart1_config_data["chip_type"] = chip_type
                self.uart2_config_data["chip_type"] = chip_type
                self.uart3_config_data["chip_type"] = chip_type
                self.uart4_config_data["chip_type"] = chip_type
                self.uart5_config_data["chip_type"] = chip_type
                self.gpio_config_data["chip_type"] = chip_type
                self.psram_config_data["chip_type"] = chip_type
                self.pwm_config_data["chip_type"] = chip_type
                self.rtc_config_data["chip_type"] = chip_type
                self.pmu_config_data["chip_type"] = chip_type
                self.sdmmc_config_data["chip_type"] = chip_type
                self.sdspi_config_data["chip_type"] = chip_type
                self.sdio_slave_config_data["chip_type"] = chip_type
                self.spim_config_data["chip_type"] = chip_type
                self.wdt_config_data["chip_type"] = chip_type
                self.nv3041a_config_data["chip_type"] = chip_type
                self.st7735_config_data["chip_type"] = chip_type
                self.gz035_config_data["chip_type"] = chip_type
                self.adc_config_data["chip_type"] = chip_type
                self.crc_config_data["chip_type"] = chip_type
                self.crypto_config_data["chip_type"] = chip_type
                self.hash_config_data["chip_type"] = chip_type
                self.rng_config_data["chip_type"] = chip_type
                self.rsa_config_data["chip_type"] = chip_type
                self.touch_sensor_config_data["chip_type"] = chip_type
                self.touch_button_config_data["chip_type"] = chip_type
                self.i2s_config_data["chip_type"] = chip_type
                self.seg_lcd_config_data["chip_type"] = chip_type
                self.i2c_config_data["chip_type"] = chip_type
                self.eeprom0_config_data["chip_type"] = chip_type
                self.eeprom1_config_data["chip_type"] = chip_type

                self.set_devices_pin()

    def set_devices_kconfig(self, kconfig):
        self.clock_config_data["kconfig"] = kconfig
        self.iflash_config_data["kconfig"] = kconfig
        self.eflash_w25q_config_data["kconfig"] = kconfig
        self.eflash_gd25q_config_data["kconfig"] = kconfig
        self.eflash_xt25f_config_data["kconfig"] = kconfig
        self.eflash_th25q_config_data["kconfig"] = kconfig
        self.timer0_config_data["kconfig"] = kconfig
        self.timer1_config_data["kconfig"] = kconfig
        self.timer2_config_data["kconfig"] = kconfig
        self.timer3_config_data["kconfig"] = kconfig
        self.timer4_config_data["kconfig"] = kconfig
        self.timer5_config_data["kconfig"] = kconfig
        self.uart0_config_data["kconfig"] = kconfig
        self.uart1_config_data["kconfig"] = kconfig
        self.uart2_config_data["kconfig"] = kconfig
        self.uart3_config_data["kconfig"] = kconfig
        self.uart4_config_data["kconfig"] = kconfig
        self.uart5_config_data["kconfig"] = kconfig
        self.gpio_config_data["kconfig"] = kconfig
        self.psram_config_data["kconfig"] = kconfig
        self.pwm_config_data["kconfig"] = kconfig
        self.rtc_config_data["kconfig"] = kconfig
        self.pmu_config_data["kconfig"] = kconfig
        self.sdmmc_config_data["kconfig"] = kconfig
        self.sdspi_config_data["kconfig"] = kconfig
        self.sdio_slave_config_data["kconfig"] = kconfig
        self.spim_config_data["kconfig"] = kconfig
        self.wdt_config_data["kconfig"] = kconfig
        self.nv3041a_config_data["kconfig"] = kconfig
        self.st7735_config_data["kconfig"] = kconfig
        self.gz035_config_data["kconfig"] = kconfig
        self.adc_config_data["kconfig"] = kconfig
        self.crc_config_data["kconfig"] = kconfig
        self.crypto_config_data["kconfig"] = kconfig
        self.hash_config_data["kconfig"] = kconfig
        self.rng_config_data["kconfig"] = kconfig
        self.rsa_config_data["kconfig"] = kconfig
        self.touch_sensor_config_data["kconfig"] = kconfig
        self.touch_button_config_data["kconfig"] = kconfig
        self.i2s_config_data["kconfig"] = kconfig
        self.seg_lcd_config_data["kconfig"] = kconfig
        self.i2c_config_data["kconfig"] = kconfig
        self.eeprom0_config_data["kconfig"] = kconfig
        self.eeprom1_config_data["kconfig"] = kconfig

    def set_devices_exist_in_config(self, exist_in_config):
        self.clock_config_data["exist_in_config"] = exist_in_config
        self.iflash_config_data["exist_in_config"] = exist_in_config
        self.eflash_w25q_config_data["exist_in_config"] = exist_in_config
        self.eflash_gd25q_config_data["exist_in_config"] = exist_in_config
        self.eflash_xt25f_config_data["exist_in_config"] = exist_in_config
        self.eflash_th25q_config_data["exist_in_config"] = exist_in_config
        self.timer0_config_data["exist_in_config"] = exist_in_config
        self.timer1_config_data["exist_in_config"] = exist_in_config
        self.timer2_config_data["exist_in_config"] = exist_in_config
        self.timer3_config_data["exist_in_config"] = exist_in_config
        self.timer4_config_data["exist_in_config"] = exist_in_config
        self.timer5_config_data["exist_in_config"] = exist_in_config
        self.uart0_config_data["exist_in_config"] = exist_in_config
        self.uart1_config_data["exist_in_config"] = exist_in_config
        self.uart2_config_data["exist_in_config"] = exist_in_config
        self.uart3_config_data["exist_in_config"] = exist_in_config
        self.uart4_config_data["exist_in_config"] = exist_in_config
        self.uart5_config_data["exist_in_config"] = exist_in_config
        self.gpio_config_data["exist_in_config"] = exist_in_config
        self.psram_config_data["exist_in_config"] = exist_in_config
        self.pwm_config_data["exist_in_config"] = exist_in_config
        self.rtc_config_data["exist_in_config"] = exist_in_config
        self.pmu_config_data["exist_in_config"] = exist_in_config
        self.sdmmc_config_data["exist_in_config"] = exist_in_config
        self.sdspi_config_data["exist_in_config"] = exist_in_config
        self.sdio_slave_config_data["exist_in_config"] = exist_in_config
        self.spim_config_data["exist_in_config"] = exist_in_config
        self.wdt_config_data["exist_in_config"] = exist_in_config
        self.nv3041a_config_data["exist_in_config"] = exist_in_config
        self.st7735_config_data["exist_in_config"] = exist_in_config
        self.gz035_config_data["exist_in_config"] = exist_in_config
        self.adc_config_data["exist_in_config"] = exist_in_config
        self.crc_config_data["exist_in_config"] = exist_in_config
        self.crypto_config_data["exist_in_config"] = exist_in_config
        self.hash_config_data["exist_in_config"] = exist_in_config
        self.rng_config_data["exist_in_config"] = exist_in_config
        self.rsa_config_data["exist_in_config"] = exist_in_config
        self.touch_sensor_config_data["exist_in_config"] = exist_in_config
        self.touch_button_config_data["exist_in_config"] = exist_in_config
        self.i2s_config_data["exist_in_config"] = exist_in_config
        self.seg_lcd_config_data["exist_in_config"] = exist_in_config
        self.i2c_config_data["exist_in_config"] = exist_in_config
        self.eeprom0_config_data["exist_in_config"] = exist_in_config
        self.eeprom1_config_data["exist_in_config"] = exist_in_config

    def set_devices_pin(self):
        self.pinmux = {}
        self.pinmux["chip_type"] = self.clock_config_data["chip_type"]
        kconfig = self.clock_config_data["kconfig"]
        self.pinmux["pinmux"] = [{} for i in range(46)]
        for item in self.config["dev"]:
            dev_name = item["dev_name"]

            if dev_name == "uart0" and kconfig["uart"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "UART0_TX"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "UART0_RX"
                if item["uart_cfg"]["flow_ctrl"] == "rts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART0_RTS"
                elif item["uart_cfg"]["flow_ctrl"] == "cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART0_CTS"
                elif item["uart_cfg"]["flow_ctrl"] == "rts_cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART0_RTS"
                    self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "UART0_CTS"

            elif dev_name == "uart1" and kconfig["uart"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "UART1_TX"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "UART1_RX"
                if item["uart_cfg"]["flow_ctrl"] == "rts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART1_RTS"
                elif item["uart_cfg"]["flow_ctrl"] == "cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART1_CTS"
                elif item["uart_cfg"]["flow_ctrl"] == "rts_cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART1_RTS"
                    self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "UART1_CTS"

            elif dev_name == "uart2" and kconfig["uart"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "UART2_TX"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "UART2_RX"
                if item["uart_cfg"]["flow_ctrl"] == "rts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART2_RTS"
                elif item["uart_cfg"]["flow_ctrl"] == "cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART2_CTS"
                elif item["uart_cfg"]["flow_ctrl"] == "rts_cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART2_RTS"
                    self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "UART2_CTS"

            elif dev_name == "uart3" and kconfig["uart"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "UART3_TX"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "UART3_RX"
                if item["uart_cfg"]["flow_ctrl"] == "rts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART3_RTS"
                elif item["uart_cfg"]["flow_ctrl"] == "cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART3_CTS"
                elif item["uart_cfg"]["flow_ctrl"] == "rts_cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART3_RTS"
                    self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "UART3_CTS"

            elif dev_name == "uart4" and kconfig["uart"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "UART4_TX"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "UART4_RX"
                if item["uart_cfg"]["flow_ctrl"] == "rts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART4_RTS"
                elif item["uart_cfg"]["flow_ctrl"] == "cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART4_CTS"
                elif item["uart_cfg"]["flow_ctrl"] == "rts_cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART4_RTS"
                    self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "UART4_CTS"

            elif dev_name == "uart5" and kconfig["uart"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "UART5_TX"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "UART5_RX"
                if item["uart_cfg"]["flow_ctrl"] == "rts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART5_RTS"
                elif item["uart_cfg"]["flow_ctrl"] == "cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART5_CTS"
                elif item["uart_cfg"]["flow_ctrl"] == "rts_cts":
                    self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "UART5_RTS"
                    self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "UART5_CTS"

            elif dev_name.startswith("flash_") and kconfig["eflash"]:
                self.pinmux["pinmux"][item["spi_cfg"]["pin_cs"]["pin"]][dev_name] = "GPIO"

            elif dev_name == "sdmmc" and kconfig["sdmmc"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "MMC_CLK"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "MMC_CMD"
                self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "MMC_DAT0"
                self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "MMC_DAT1"
                self.pinmux["pinmux"][item["pin_cfg"][4]["pin"]][dev_name] = "MMC_DAT2"
                self.pinmux["pinmux"][item["pin_cfg"][5]["pin"]][dev_name] = "MMC_DAT3"

            elif dev_name == "sdspi" and kconfig["sdspi"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "MMC_CLK"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "MMC_CMD"

            elif dev_name == "nv3041a_spi" and kconfig["nv3041a"]:
                self.pinmux["pinmux"][item["spi_cfg"]["pin_cs"]["pin_num"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_reset"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_dcx"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_led"]][dev_name] = "GPIO"

            elif dev_name == "st7735_spi" and kconfig["st7735"]:
                self.pinmux["pinmux"][item["spi_cfg"]["pin_cs"]["pin_num"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_reset"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_dcx"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_led"]][dev_name] = "GPIO"

            elif dev_name == "gz035_spi" and kconfig["gz035"]:
                self.pinmux["pinmux"][item["spi_cfg"]["pin_cs"]["pin_num"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_reset"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_dcx"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_led"]][dev_name] = "GPIO"
                self.pinmux["pinmux"][item["pin_lcd_te"]][dev_name] = "GPIO"

            elif dev_name == "sdio_slave" and kconfig["sdio_slave"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "MMC_CLK"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "MMC_CMD"
                self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "MMC_DAT0"
                self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "MMC_DAT1"
                self.pinmux["pinmux"][item["pin_cfg"][4]["pin"]][dev_name] = "MMC_DAT2"
                self.pinmux["pinmux"][item["pin_cfg"][5]["pin"]][dev_name] = "MMC_DAT3"

            elif dev_name == "i2c" and kconfig["i2c"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "I2C_SCL"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "I2C_SDA"

            elif dev_name == "eeprom0" and kconfig["i2c"]:
                if "pin_cfg" in item:
                    self.pinmux["pinmux"][item["pin_cfg"]["pin"]][dev_name] = "GPIO"

            elif dev_name == "eeprom1" and kconfig["i2c"]:
                if "pin_cfg" in item:
                    self.pinmux["pinmux"][item["pin_cfg"]["pin"]][dev_name] = "GPIO"

            elif dev_name == "spim" and kconfig["spim"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "LSPI_CK"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "LSPI_MISO"
                self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "LSPI_MOSI"

            elif dev_name == "touch_sensor" and kconfig["touch_sensor"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "CMOD"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "CDC"

            elif dev_name == "touch_button" and kconfig["touch_sensor"]:
                for cfg in item["touch_button_cfg"]:
                    self.pinmux["pinmux"][item["pin_cfg"][item["touch_button_cfg"].index(cfg)]["pin"]][dev_name] = "TOUCH" + str(cfg["key_num"])

            elif dev_name == "pwm" and kconfig["pwm"]:
                pwm_channel_0 = [2, 10, 16, 28, 35]
                pwm_channel_1 = [3, 11, 17, 29, 36]
                pwm_channel_2 = [0, 12, 18, 30, 40]
                pwm_channel_3 = [1, 13, 19, 31, 41]
                pwm_channel_4 = [4,  7, 14, 32, 42]
                pwm_channel_b = [5,  8, 15, 24, 33]
                for cfg in item["pin_cfg"]:
                    if cfg["pin"] in pwm_channel_0:
                        self.pinmux["pinmux"][cfg["pin"]][dev_name] = "PWM0"
                    elif cfg["pin"] in pwm_channel_1:
                        self.pinmux["pinmux"][cfg["pin"]][dev_name] = "PWM1"
                    elif cfg["pin"] in pwm_channel_2:
                        self.pinmux["pinmux"][cfg["pin"]][dev_name] = "PWM2"
                    elif cfg["pin"] in pwm_channel_3:
                        self.pinmux["pinmux"][cfg["pin"]][dev_name] = "PWM3"
                    elif cfg["pin"] in pwm_channel_4:
                        self.pinmux["pinmux"][cfg["pin"]][dev_name] = "PWM4"
                    elif cfg["pin"] in pwm_channel_b:
                        self.pinmux["pinmux"][cfg["pin"]][dev_name] = "PWM_BREAK"

            elif dev_name == "i2s" and kconfig["i2s"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "I2S_BCLK"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "I2S_LRCLK"
                if self.i2s_config_data["enable_mclk"]:
                    self.pinmux["pinmux"][self.i2s_config_data["mclkpinnum"]][dev_name] = "I2S_MCLK"
                if self.i2s_config_data["enable_di"]:
                    self.pinmux["pinmux"][self.i2s_config_data["dipinnum"]][dev_name] = "I2S_DI"
                if self.i2s_config_data["enable_do"]:
                    self.pinmux["pinmux"][self.i2s_config_data["dopinnum"]][dev_name] = "I2S_DO"

            elif dev_name == "adc" and kconfig["adc"]:
                config_data = self.adc_config_data

                index = 0
                for cfg in item["adc_cfg"]:
                    if cfg["adc_channel"] == 0:
                        self.pinmux["pinmux"][item["pin_cfg"][index]["pin"]][dev_name] = "ADC0"
                    elif cfg["adc_channel"] == 1:
                        self.pinmux["pinmux"][item["pin_cfg"][index]["pin"]][dev_name] = "ADC1"
                    elif cfg["adc_channel"] == 2:
                        self.pinmux["pinmux"][item["pin_cfg"][index]["pin"]][dev_name] = "ADC2"
                    elif cfg["adc_channel"] == 3:
                        self.pinmux["pinmux"][item["pin_cfg"][index]["pin"]][dev_name] = "ADC3"
                    elif cfg["adc_channel"] == 8:
                        self.pinmux["pinmux"][item["pin_cfg"][index]["pin"]][dev_name] = "ADC0"
                        self.pinmux["pinmux"][item["pin_cfg"][index + 1]["pin"]][dev_name] = "ADC1"
                        index = index + 1
                    elif cfg["adc_channel"] == 9:
                        self.pinmux["pinmux"][item["pin_cfg"][index]["pin"]][dev_name] = "ADC2"
                        self.pinmux["pinmux"][item["pin_cfg"][index + 1]["pin"]][dev_name] = "ADC3"
                        index = index + 1
                    index = index + 1

            elif dev_name == "gpio" and kconfig["gpio"]:
                for cfg in item["gpio_cfg"]:
                    self.pinmux["pinmux"][cfg["pin"]][dev_name] = "GPIO"

            elif dev_name == "psram" and kconfig["psram"]:
                self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "PSRAM_CK"
                self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "PSRAM_CS"
                self.pinmux["pinmux"][item["pin_cfg"][2]["pin"]][dev_name] = "PSRAM_DAT0"
                self.pinmux["pinmux"][item["pin_cfg"][3]["pin"]][dev_name] = "PSRAM_DAT1"
                self.pinmux["pinmux"][item["pin_cfg"][4]["pin"]][dev_name] = "PSRAM_DAT2"
                self.pinmux["pinmux"][item["pin_cfg"][5]["pin"]][dev_name] = "PSRAM_DAT3"

    def get_devices_pin(self):
        #self.pinmux["chip_type"] = self.clock_config_data["chip_type"]
        return self.pinmux

    def update_config_file(self):
        with open(self.toml_file, mode="wt", encoding = "utf-8") as fp:
            tomlkit.dump(self.config, fp)
        self.set_devices_pin()

    def load_init_cfg(self, item, config_data):
        if item["init_cfg"]["init_level"] == "system":
            config_data["initlevel"] = 1
        elif item["init_cfg"]["init_level"] == "app":
            config_data["initlevel"] = 0
        else:
            config_data["initlevel"] = 0

        config_data["initpriority"] = item["init_cfg"]["init_priority"]

    def update_init_cfg(self, config_data, item):
        if config_data["initlevel"] == 1:
            item["init_cfg"]["init_level"] = "system"
        elif config_data["initlevel"] == 0:
            item["init_cfg"]["init_level"] = "app"
        else:
            item["init_cfg"]["init_level"] = "app"

        item["init_cfg"]["init_priority"] = config_data["initpriority"]

    def get_clock_config(self, dev_name):
        if dev_name == 'Clock':
            return self.clock_config_data

    def get_uart_config(self, dev_name):
        if dev_name == 'Uart0':
            return self.uart0_config_data
        elif dev_name == 'Uart1':
            return self.uart1_config_data
        elif dev_name == 'Uart2':
            return self.uart2_config_data
        elif dev_name == 'Uart3':
            return self.uart3_config_data
        elif dev_name == 'Uart4':
            return self.uart4_config_data
        elif dev_name == 'Uart5':
            return self.uart5_config_data

    def get_timer_config(self, dev_name):
        if dev_name == 'Timer0':
            return self.timer0_config_data
        elif dev_name == 'Timer1':
            return self.timer1_config_data
        elif dev_name == 'Timer2':
            return self.timer2_config_data
        elif dev_name == 'Timer3':
            return self.timer3_config_data
        elif dev_name == 'Timer4':
            return self.timer4_config_data
        elif dev_name == 'Timer5':
            return self.timer5_config_data

    def get_iflash_config(self, dev_name):
        if dev_name == 'Internal_flash':
            return self.iflash_config_data

    def get_eflash_w25q_config(self, dev_name):
        if dev_name == 'External_flash_w25q':
            return self.eflash_w25q_config_data

    def get_eflash_gd25q_config(self, dev_name):
        if dev_name == 'External_flash_gd25q':
            return self.eflash_gd25q_config_data

    def get_eflash_xt25f_config(self, dev_name):
        if dev_name == 'External_flash_xt25f':
            return self.eflash_xt25f_config_data

    def get_eflash_th25q_config(self, dev_name):
        if dev_name == 'External_flash_th25q':
            return self.eflash_th25q_config_data

    def get_crypto_config(self, dev_name):
        if dev_name == 'CRYPTO':
            return self.crypto_config_data

    def get_crc_config(self, dev_name):
        if dev_name == 'CRC':
            return self.crc_config_data

    def get_hash_config(self, dev_name):
        if dev_name == 'HASH':
            return self.hash_config_data

    def get_rng_config(self, dev_name):
        if dev_name == 'RNG':
            return self.rng_config_data

    def get_rsa_config(self, dev_name):
        if dev_name == 'RSA':
            return self.rsa_config_data

    def get_sdmmc_config(self, dev_name):
        if dev_name == 'SDMMC':
            self.sdmmc_config_data["cpu_freq"] = (480 / self.clock_config_data["clk_div_cpu"]) * 1000000 #HZ
            return self.sdmmc_config_data

    def get_sdspi_config(self, dev_name):
        if dev_name == 'SDSPI':
            return self.sdspi_config_data

    def get_nv3041a_config(self, dev_name):
        if dev_name == 'NV3041A':
            return self.nv3041a_config_data

    def get_st7735_config(self, dev_name):
        if dev_name == 'ST7735':
            return self.st7735_config_data

    def get_gz035_config(self, dev_name):
        if dev_name == 'GZ035':
            return self.gz035_config_data

    def get_sdio_slave_config(self, dev_name):
        if dev_name == 'SDIO_Slave':
            return self.sdio_slave_config_data

    def get_i2c_config(self, dev_name):
        if dev_name == 'I2C':
            return self.i2c_config_data

    def get_eeprom0_config(self, dev_name):
        if dev_name == 'eeprom0':
            return self.eeprom0_config_data

    def get_eeprom1_config(self, dev_name):
        if dev_name == 'eeprom1':
            return self.eeprom1_config_data

    def get_spim_config(self, dev_name):
        if dev_name == 'SPI_Master':
            return self.spim_config_data

    def get_touch_sensor_config(self, dev_name):
        if dev_name == 'Touch_Sensor':
            return self.touch_sensor_config_data

    def get_touch_button_config(self, dev_name):
        if dev_name == 'Touch_Button':
            return self.touch_button_config_data

    def get_pwm_config(self, dev_name):
        if dev_name == 'PWM':
            return self.pwm_config_data

    def get_rtc_config(self, dev_name):
        if dev_name == 'RTC':
            return self.rtc_config_data

    def get_pmu_config(self, dev_name):
        if dev_name == 'PMU':
            return self.pmu_config_data

    def get_i2s_config(self, dev_name):
        if dev_name == 'I2S':
            return self.i2s_config_data

    def get_seg_lcd_config(self, dev_name):
        if dev_name == 'SEG_LCD':
            return self.seg_lcd_config_data

    def get_adc_config(self, dev_name):
        if dev_name == 'ADC':
            return self.adc_config_data

    def get_gpio_config(self, dev_name):
        if dev_name == 'GPIO':
            return self.gpio_config_data

    def get_wdt_config(self, dev_name):
        if dev_name == 'WDT':
            return self.wdt_config_data

    def get_psram_config(self, dev_name):
        if dev_name == 'PSRAM':
            self.psram_config_data["cpu_freq"] = (480 / self.clock_config_data["clk_div_cpu"]) * 1000000 #HZ
            return self.psram_config_data

    def set_clock_config(self, data):
        if data['dev_name'] == 'Clock':
            self.clock_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "rcc":
                item["init_cfg"]["init_level"] = "app"
                item["init_cfg"]["init_priority"] = 0
                pll = 480
                item["rcc_cfg"].clear()
                item["rcc_cfg"].append({"type":"cpu", "clock":int(pll/self.clock_config_data["clk_div_cpu"])})
                item["rcc_cfg"].append({"type":"wlan", "clock":int(pll/self.clock_config_data["clk_div_wlan"])})
                #item["rcc_cfg"].append({"type":"peripheral", "clock":int(pll/self.clock_config_data["clk_div_peri"])})
                #item["rcc_cfg"].append({"type":"sd_adc", "clock":int(pll/self.clock_config_data["clk_div_peri"]/self.clock_config_data["clk_div_adc"])})

                self.update_config_file()
                break

    def set_uart_config(self, data):
        if data['dev_name'] == 'Uart0':
            self.uart0_config_data = data
        elif data['dev_name'] == 'Uart1':
            self.uart1_config_data = data
        elif data['dev_name'] == 'Uart2':
            self.uart2_config_data = data
        elif data['dev_name'] == 'Uart3':
            self.uart3_config_data = data
        elif data['dev_name'] == 'Uart4':
            self.uart4_config_data = data
        elif data['dev_name'] == 'Uart5':
            self.uart5_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            dev_name = item["dev_name"]
            if dev_name == data['dev_name'].lower():
                if dev_name == "uart0":
                    config_data = self.uart0_config_data
                elif dev_name == "uart1":
                    config_data = self.uart1_config_data
                elif dev_name == "uart2":
                    config_data = self.uart2_config_data
                elif dev_name == "uart3":
                    config_data = self.uart3_config_data
                elif dev_name == "uart4":
                    config_data = self.uart4_config_data
                elif dev_name == "uart5":
                    config_data = self.uart5_config_data

                item["uart_cfg"]["baudrate"] = config_data["baud"]

                if config_data["parity"] == 0:
                    item["uart_cfg"]["parity"] = "none"
                elif config_data["parity"] == 1:
                    item["uart_cfg"]["parity"] = "even"
                elif config_data["parity"] == 2:
                    item["uart_cfg"]["parity"] = "odd"
                else:
                    item["uart_cfg"]["parity"] = "none"

                item["uart_cfg"]["stop_bits"] = config_data["stopbits"] + 1
                item["uart_cfg"]["data_bits"] = 8

                rts_pin_num = [0, 2, 5, 10, 15, 20, 29, 35, 37] #0 indication invalid pin
                rts_pin_func = ["fun1", "fun1", "fun2", "fun2", "fun2", "fun2", "fun3", "fun3", "fun1"]

                cts_pin_num = [0, 3, 6, 11, 14, 21, 28, 36, 38] #0 indication invalid pin
                cts_pin_func = ["fun1", "fun1", "fun2", "fun2", "fun2", "fun2", "fun3", "fun3", "fun1"]

                if config_data["flowctrl"] == 1:
                    item["uart_cfg"]["flow_ctrl"] = "rts"
                    if len(item["pin_cfg"]) < 3:
                        if data['dev_name'] == 'Uart3' and config_data["pinrts"] == 2:
                            item["pin_cfg"].append({"pin":config_data["pinrts"], "fun":"fun4"})
                        elif data['dev_name'] == 'Uart4' and config_data["pinrts"] == 5:
                            item["pin_cfg"].append({"pin":config_data["pinrts"], "fun":"fun4"})
                        else:
                            item["pin_cfg"].append({"pin":config_data["pinrts"], "fun":rts_pin_func[rts_pin_num.index(config_data["pinrts"])]})
                    else:
                        item["pin_cfg"][2]["pin"] = config_data["pinrts"]
                        if data['dev_name'] == 'Uart3' and config_data["pinrts"] == 2:
                            item["pin_cfg"][2]["fun"] = "fun4"
                        elif data['dev_name'] == 'Uart4' and config_data["pinrts"] == 5:
                            item["pin_cfg"][2]["fun"] = "fun4"
                        else:
                            item["pin_cfg"][2]["pin"] = config_data["pinrts"]
                            item["pin_cfg"][2]["fun"] = rts_pin_func[rts_pin_num.index(config_data["pinrts"])]
                elif config_data["flowctrl"] == 2:
                    item["uart_cfg"]["flow_ctrl"] = "cts"
                    if len(item["pin_cfg"]) < 3:
                        if data['dev_name'] == 'Uart3' and config_data["pincts"] == 3:
                            item["pin_cfg"].append({"pin":config_data["pincts"], "fun":"fun4"})
                        elif data['dev_name'] == 'Uart4' and config_data["pincts"] == 6:
                            item["pin_cfg"].append({"pin":config_data["pincts"], "fun":"fun4"})
                        else:
                            item["pin_cfg"].append({"pin":config_data["pincts"], "fun":cts_pin_func[cts_pin_num.index(config_data["pincts"])]})
                    else:
                        item["pin_cfg"][2]["pin"] = config_data["pincts"]
                        if data['dev_name'] == 'Uart3' and config_data["pincts"] == 3:
                            item["pin_cfg"][2]["fun"] = "fun4"
                        elif data['dev_name'] == 'Uart4' and config_data["pincts"] == 6:
                            item["pin_cfg"][2]["fun"] = "fun4"
                        else:
                            item["pin_cfg"][2]["pin"] = config_data["pincts"]
                            item["pin_cfg"][2]["fun"] = cts_pin_func[cts_pin_num.index(config_data["pincts"])]
                elif config_data["flowctrl"] == 3:
                    item["uart_cfg"]["flow_ctrl"] = "rts_cts"
                    if len(item["pin_cfg"]) < 3:
                        if data['dev_name'] == 'Uart3' and config_data["pinrts"] == 2:
                            item["pin_cfg"].append({"pin":config_data["pinrts"], "fun":"fun4"})
                        elif data['dev_name'] == 'Uart4' and config_data["pinrts"] == 5:
                            item["pin_cfg"].append({"pin":config_data["pinrts"], "fun":"fun4"})
                        else:
                            item["pin_cfg"].append({"pin":config_data["pinrts"], "fun":rts_pin_func[rts_pin_num.index(config_data["pinrts"])]})
                    else:
                        item["pin_cfg"][2]["pin"] = config_data["pinrts"]
                        if data['dev_name'] == 'Uart3' and config_data["pinrts"] == 2:
                            item["pin_cfg"][2]["fun"] = "fun4"
                        elif data['dev_name'] == 'Uart4' and config_data["pinrts"] == 5:
                            item["pin_cfg"][2]["fun"] = "fun4"
                        else:
                            item["pin_cfg"][2]["pin"] = config_data["pinrts"]
                            item["pin_cfg"][2]["fun"] = rts_pin_func[rts_pin_num.index(config_data["pinrts"])]
                    if len(item["pin_cfg"]) < 4:
                        if data['dev_name'] == 'Uart3' and config_data["pincts"] == 3:
                            item["pin_cfg"].append({"pin":config_data["pincts"], "fun":"fun4"})
                        elif data['dev_name'] == 'Uart4' and config_data["pincts"] == 6:
                            item["pin_cfg"].append({"pin":config_data["pincts"], "fun":"fun4"})
                        else:
                            item["pin_cfg"].append({"pin":config_data["pincts"], "fun":cts_pin_func[cts_pin_num.index(config_data["pincts"])]})
                    else:
                        if data['dev_name'] == 'Uart3' and config_data["pincts"] == 3:
                            item["pin_cfg"][3]["fun"] = "fun4"
                        elif data['dev_name'] == 'Uart4' and config_data["pincts"] == 6:
                            item["pin_cfg"][3]["fun"] = "fun4"
                        else:
                            item["pin_cfg"][3]["pin"] = config_data["pincts"]
                            item["pin_cfg"][3]["fun"] = cts_pin_func[cts_pin_num.index(config_data["pincts"])]
                else:
                    item["uart_cfg"]["flow_ctrl"] = "none"
                    if len(item["pin_cfg"]) > 3:
                        item["pin_cfg"].pop(3)
                        item["pin_cfg"].pop(2)
                    elif len(item["pin_cfg"]) > 2:
                        item["pin_cfg"].pop(2)

                tx_pin_num = [2, 5, 8, 12, 16, 18, 20, 22, 34, 35, 43]
                tx_pin_func = ["fun2", "fun1", "fun2", "fun2", "fun3", "fun3", "fun3", "fun1", "fun1", "fun1", "fun2"]
                item["pin_cfg"][0]["pin"] = config_data["pintx"]
                if dev_name == "uart5" and config_data["pintx"] == 8:
                    item["pin_cfg"][0]["fun"] = "fun3"
                else:
                    item["pin_cfg"][0]["fun"] = tx_pin_func[tx_pin_num.index(config_data["pintx"])]

                rx_pin_num = [3, 6, 9, 17, 19, 21, 23, 29, 32, 33, 36]
                rx_pin_func = ["fun2", "fun1", "fun2", "fun3", "fun3", "fun3", "fun1", "fun2", "fun4", "fun1", "fun1"]
                item["pin_cfg"][1]["pin"] = config_data["pinrx"]
                if dev_name == "uart5" and config_data["pinrx"] == 9:
                    item["pin_cfg"][1]["fun"] = "fun3"
                else:
                    item["pin_cfg"][1]["fun"] = rx_pin_func[rx_pin_num.index(config_data["pinrx"])]

                if config_data["enable_dma"]:
                    item["dma_device"] = "dma"
                else:
                    item.pop("dma_device", "")

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_timer_config(self, data):
        if data['dev_name'] == 'Timer0':
            self.timer0_config_data = data
        elif data['dev_name'] == 'Timer1':
            self.timer1_config_data = data
        elif data['dev_name'] == 'Timer2':
            self.timer2_config_data = data
        elif data['dev_name'] == 'Timer3':
            self.timer3_config_data = data
        elif data['dev_name'] == 'Timer4':
            self.timer4_config_data = data
        elif data['dev_name'] == 'Timer5':
            self.timer5_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            dev_name = item["dev_name"]
            if dev_name == data['dev_name'].lower():
                if dev_name == "timer0":
                    config_data = self.timer0_config_data
                elif dev_name == "timer1":
                    config_data = self.timer1_config_data
                elif dev_name == "timer2":
                    config_data = self.timer2_config_data
                elif dev_name == "timer3":
                    config_data = self.timer3_config_data
                elif dev_name == "timer4":
                    config_data = self.timer4_config_data
                elif dev_name == "timer5":
                    config_data = self.timer5_config_data

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_iflash_config(self, data):
        if data['dev_name'] == 'Internal_flash':
            self.iflash_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "iflash":
                config_data = self.iflash_config_data

                if config_data["quad_spi"]:
                    item["flash_cfg"]["quad_spi"] = True
                else:
                    item["flash_cfg"]["quad_spi"] = False

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_eflash_w25q_config(self, data):
        if data['dev_name'] == 'External_flash_w25q':
            self.eflash_w25q_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "flash_w25q":
                config_data = self.eflash_w25q_config_data

                if config_data["quad_spi"]:
                    item["flash_cfg"]["quad_spi"] = True
                else:
                    item["flash_cfg"]["quad_spi"] = False

                item["spi_cfg"]["mode"] = config_data["mode"]
                item["spi_cfg"]["freq"] = config_data["freq"]
                item["spi_cfg"]["pin_cs"]["pin"] = config_data["cspinnum"]
                item["spi_cfg"]["pin_cs"]["fun"] = "fun5"
                item["spi_cfg"]["pin_cs"]["dir"] = "output"
                item["spi_cfg"]["pin_cs"]["pupd"] = "float"

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_eflash_gd25q_config(self, data):
        if data['dev_name'] == 'External_flash_gd25q':
            self.eflash_gd25q_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "flash_gd25q":
                config_data = self.eflash_gd25q_config_data

                if config_data["quad_spi"]:
                    item["flash_cfg"]["quad_spi"] = True
                else:
                    item["flash_cfg"]["quad_spi"] = False

                item["spi_cfg"]["mode"] = config_data["mode"]
                item["spi_cfg"]["freq"] = config_data["freq"]
                item["spi_cfg"]["pin_cs"]["pin"] = config_data["cspinnum"]
                item["spi_cfg"]["pin_cs"]["fun"] = "fun5"
                item["spi_cfg"]["pin_cs"]["dir"] = "output"
                item["spi_cfg"]["pin_cs"]["pupd"] = "float"

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_eflash_xt25f_config(self, data):
        if data['dev_name'] == 'External_flash_xt25f':
            self.eflash_xt25f_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "flash_xt25f":
                config_data = self.eflash_xt25f_config_data

                if config_data["quad_spi"]:
                    item["flash_cfg"]["quad_spi"] = True
                else:
                    item["flash_cfg"]["quad_spi"] = False

                item["spi_cfg"]["mode"] = config_data["mode"]
                item["spi_cfg"]["freq"] = config_data["freq"]
                item["spi_cfg"]["pin_cs"]["pin"] = config_data["cspinnum"]
                item["spi_cfg"]["pin_cs"]["fun"] = "fun5"
                item["spi_cfg"]["pin_cs"]["dir"] = "output"
                item["spi_cfg"]["pin_cs"]["pupd"] = "float"

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_eflash_th25q_config(self, data):
        if data['dev_name'] == 'External_flash_th25q':
            self.eflash_th25q_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "flash_th25q":
                config_data = self.eflash_th25q_config_data

                if config_data["quad_spi"]:
                    item["flash_cfg"]["quad_spi"] = True
                else:
                    item["flash_cfg"]["quad_spi"] = False

                item["spi_cfg"]["mode"] = config_data["mode"]
                item["spi_cfg"]["freq"] = config_data["freq"]
                item["spi_cfg"]["pin_cs"]["pin"] = config_data["cspinnum"]
                item["spi_cfg"]["pin_cs"]["fun"] = "fun5"
                item["spi_cfg"]["pin_cs"]["dir"] = "output"
                item["spi_cfg"]["pin_cs"]["pupd"] = "float"

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_crypto_config(self, data):
        if data['dev_name'] == 'CRYPTO':
            self.crypto_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "crypto":
                config_data = self.crypto_config_data
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_crc_config(self, data):
        if data['dev_name'] == 'CRC':
            self.crc_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "crc":
                config_data = self.crc_config_data
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_hash_config(self, data):
        if data['dev_name'] == 'HASH':
            self.hash_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "hash":
                config_data = self.hash_config_data
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_rng_config(self, data):
        if data['dev_name'] == 'RNG':
            self.rng_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "rng":
                config_data = self.rng_config_data
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_rsa_config(self, data):
        if data['dev_name'] == 'RSA':
            self.rsa_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "rsa":
                config_data = self.rsa_config_data
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_sdmmc_config(self, data):
        if data['dev_name'] == 'SDMMC':
            self.sdmmc_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "sdmmc":
                config_data = self.sdmmc_config_data
                item["sdh_cfg"]["clock_hz"] = config_data["clock"]
                if config_data["bus_width"]:
                    item["sdh_cfg"]["bus_width"] = 4
                else:
                    item["sdh_cfg"]["bus_width"] = 1
                item["pin_cfg"][0]["pin"] = config_data["clkpinnum"]
                if config_data["clkpinnum"] == 9:
                    item["pin_cfg"][0]["fun"] = "fun1"
                else:
                    item["pin_cfg"][0]["fun"] = "fun2"
                item["pin_cfg"][1]["pin"] = config_data["cmdpinnum"]
                if config_data["cmdpinnum"] == 10:
                    item["pin_cfg"][1]["fun"] = "fun1"
                else:
                    item["pin_cfg"][1]["fun"] = "fun2"
                item["pin_cfg"][2]["pin"] = config_data["dat0pinnum"]
                if config_data["dat0pinnum"] == 11:
                    item["pin_cfg"][2]["fun"] = "fun1"
                else:
                    item["pin_cfg"][2]["fun"] = "fun2"
                item["pin_cfg"][3]["pin"] = config_data["dat1pinnum"]
                if config_data["dat1pinnum"] == 12:
                    item["pin_cfg"][3]["fun"] = "fun1"
                else:
                    item["pin_cfg"][3]["fun"] = "fun2"
                item["pin_cfg"][4]["pin"] = config_data["dat2pinnum"]
                if config_data["dat2pinnum"] == 13:
                    item["pin_cfg"][4]["fun"] = "fun1"
                else:
                    item["pin_cfg"][4]["fun"] = "fun2"
                item["pin_cfg"][5]["pin"] = config_data["dat3pinnum"]
                if config_data["dat3pinnum"] == 14:
                    item["pin_cfg"][5]["fun"] = "fun1"
                else:
                    item["pin_cfg"][5]["fun"] = "fun2"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_sdspi_config(self, data):
        if data['dev_name'] == 'SDSPI':
            self.sdspi_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "sdspi":
                config_data = self.sdspi_config_data
                item["pin_cfg"][0]["pin"] = config_data["clkpinnum"]
                if config_data["clkpinnum"] == 9:
                    item["pin_cfg"][0]["fun"] = "fun1"
                else:
                    item["pin_cfg"][0]["fun"] = "fun2"
                item["pin_cfg"][1]["pin"] = config_data["cmdpinnum"]
                if config_data["cmdpinnum"] == 10:
                    item["pin_cfg"][1]["fun"] = "fun1"
                else:
                    item["pin_cfg"][1]["fun"] = "fun2"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_nv3041a_config(self, data):
        if data['dev_name'] == 'NV3041A':
            self.nv3041a_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "nv3041a_spi":
                config_data = self.nv3041a_config_data
                item["spi_cfg"]["mode"] = config_data['mode']
                item["spi_cfg"]["freq"] = config_data['freq']
                item["spi_cfg"]["pin_cs"]["pin_num"] = config_data['cspinnum']
                item["spi_cfg"]["pin_cs"]["pin_mux"] = "fun5"
                item["spi_cfg"]["pin_cs"]["dir"] = "output"
                item["spi_cfg"]["pin_cs"]["pupd"] = "float"
                item["pin_lcd_reset"] = config_data['resetpinnum']
                item["pin_lcd_dcx"] = config_data['dcxpinnum']
                item["pin_lcd_led"] = config_data['ledpinnum']
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_st7735_config(self, data):
        if data['dev_name'] == 'ST7735':
            self.st7735_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "st7735_spi":
                config_data = self.st7735_config_data
                item["spi_cfg"]["mode"] = config_data['mode']
                item["spi_cfg"]["freq"] = config_data['freq']
                item["spi_cfg"]["pin_cs"]["pin_num"] = config_data['cspinnum']
                item["spi_cfg"]["pin_cs"]["pin_mux"] = "fun5"
                item["spi_cfg"]["pin_cs"]["dir"] = "output"
                item["spi_cfg"]["pin_cs"]["pupd"] = "float"
                item["pin_lcd_reset"] = config_data['resetpinnum']
                item["pin_lcd_dcx"] = config_data['dcxpinnum']
                item["pin_lcd_led"] = config_data['ledpinnum']
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_gz035_config(self, data):
        if data['dev_name'] == 'GZ035':
            self.gz035_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "gz035_spi":
                config_data = self.gz035_config_data
                item["spi_cfg"]["mode"] = config_data['mode']
                item["spi_cfg"]["freq"] = config_data['freq']
                item["spi_cfg"]["pin_cs"]["pin_num"] = config_data['cspinnum']
                item["spi_cfg"]["pin_cs"]["pin_mux"] = "fun5"
                item["spi_cfg"]["pin_cs"]["dir"] = "output"
                item["spi_cfg"]["pin_cs"]["pupd"] = "float"
                item["pin_lcd_reset"] = config_data['resetpinnum']
                item["pin_lcd_dcx"] = config_data['dcxpinnum']
                item["pin_lcd_led"] = config_data['ledpinnum']
                if config_data['tepinnum'] != -1:
                    item["pin_lcd_te"] = config_data['tepinnum']
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_sdio_slave_config(self, data):
        if data['dev_name'] == 'SDIO_Slave':
            self.sdio_slave_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "sdio_slave":
                config_data = self.sdio_slave_config_data
                item["pin_cfg"][0]["pin"] = config_data["clkpinnum"]
                if config_data["clkpinnum"] == 9:
                    item["pin_cfg"][0]["fun"] = "fun1"
                else:
                    item["pin_cfg"][0]["fun"] = "fun2"
                item["pin_cfg"][1]["pin"] = config_data["cmdpinnum"]
                if config_data["cmdpinnum"] == 10:
                    item["pin_cfg"][1]["fun"] = "fun1"
                else:
                    item["pin_cfg"][1]["fun"] = "fun2"
                item["pin_cfg"][2]["pin"] = config_data["dat0pinnum"]
                if config_data["dat0pinnum"] == 11:
                    item["pin_cfg"][2]["fun"] = "fun1"
                else:
                    item["pin_cfg"][2]["fun"] = "fun2"
                item["pin_cfg"][3]["pin"] = config_data["dat1pinnum"]
                if config_data["dat1pinnum"] == 12:
                    item["pin_cfg"][3]["fun"] = "fun1"
                else:
                    item["pin_cfg"][3]["fun"] = "fun2"
                item["pin_cfg"][4]["pin"] = config_data["dat2pinnum"]
                if config_data["dat2pinnum"] == 13:
                    item["pin_cfg"][4]["fun"] = "fun1"
                else:
                    item["pin_cfg"][4]["fun"] = "fun2"
                item["pin_cfg"][5]["pin"] = config_data["dat3pinnum"]
                if config_data["dat3pinnum"] == 14:
                    item["pin_cfg"][5]["fun"] = "fun1"
                else:
                    item["pin_cfg"][5]["fun"] = "fun2"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_i2c_config(self, data):
        if data['dev_name'] == 'I2C':
            self.i2c_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "i2c":
                config_data = self.i2c_config_data
                item["i2c_cfg"]["max_clock"] = config_data['clock']
                if config_data['addr_10_bits']:
                    item["i2c_cfg"]["addr_10_bits"] = True
                else:
                    item["i2c_cfg"]["addr_10_bits"] = False
                item["pin_cfg"][0]["pin"] = config_data['sclpinnum']
                if config_data['sclpinnum'] == 1:
                    item["pin_cfg"][0]["fun"] = "fun2"
                else:
                    item["pin_cfg"][0]["fun"] = "fun4"
                item["pin_cfg"][1]["pin"] = config_data['sdapinnum']
                if config_data['sdapinnum'] == 4:
                    item["pin_cfg"][1]["fun"] = "fun2"
                else:
                    item["pin_cfg"][1]["fun"] = "fun4"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_eeprom0_config(self, data):
        if data['dev_name'] == 'eeprom0':
            self.eeprom0_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "eeprom0":
                config_data = self.eeprom0_config_data
                item["speed_hz"] = config_data['speed_hz']
                item["size"] = config_data['size']
                if config_data['i2c_addr'].startswith("0x") or config_data['i2c_addr'].startswith("0X"):
                    i2c_addr = config_data['i2c_addr'][2:]
                    item["i2c_addr"] = int(i2c_addr, 16)
                else:
                    item["i2c_addr"] = int(config_data['i2c_addr'], 16)
                item["page_size"] = config_data['page_size']
                item["addr_width"] = config_data['addr_width']
                if config_data['read_only']:
                    item["read_only"] = True
                else:
                    item["read_only"] = False
                item["max_write_time_ms"] = config_data['max_write_time_ms']

                if config_data['enable']:
                    item.pop("pin_cfg", "")
                    pin_cfg = tomlkit.inline_table()
                    pin_cfg["pin"] = config_data['wppinnum']
                    pin_cfg["fun"] = "fun5"
                    pin_cfg["dir"] = "output"
                    pin_cfg["pupd"] = "float"
                    item["pin_cfg"] = pin_cfg
                else:
                    item.pop("pin_cfg", "")

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_eeprom1_config(self, data):
        if data['dev_name'] == 'eeprom1':
            self.eeprom1_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "eeprom1":
                config_data = self.eeprom1_config_data
                item["speed_hz"] = config_data['speed_hz']
                item["size"] = config_data['size']
                if config_data['i2c_addr'].startswith("0x") or config_data['i2c_addr'].startswith("0X"):
                    i2c_addr = config_data['i2c_addr'][2:]
                    item["i2c_addr"] = int(i2c_addr, 16)
                else:
                    item["i2c_addr"] = int(config_data['i2c_addr'], 16)
                item["page_size"] = config_data['page_size']
                item["addr_width"] = config_data['addr_width']
                if config_data['read_only']:
                    item["read_only"] = True
                else:
                    item["read_only"] = False
                item["max_write_time_ms"] = config_data['max_write_time_ms']

                if config_data['enable']:
                    item.pop("pin_cfg", "")
                    pin_cfg = tomlkit.inline_table()
                    pin_cfg["pin"] = config_data['wppinnum']
                    pin_cfg["fun"] = "fun5"
                    pin_cfg["dir"] = "output"
                    pin_cfg["pupd"] = "float"
                    item["pin_cfg"] = pin_cfg
                else:
                    item.pop("pin_cfg", "")

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_spim_config(self, data):
        if data['dev_name'] == 'SPI_Master':
            self.spim_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "spim":
                config_data = self.spim_config_data
                item["pin_cfg"][0]["pin"] = config_data['clkpinnum']
                if config_data['clkpinnum'] == 17:
                    item["pin_cfg"][0]["fun"] = "fun2"
                elif config_data['clkpinnum'] == 18:
                    item["pin_cfg"][0]["fun"] = "fun2"
                elif config_data['clkpinnum'] == 31:
                    item["pin_cfg"][0]["fun"] = "fun3"
                else:#40
                    item["pin_cfg"][0]["fun"] = "fun1"
                item["pin_cfg"][1]["pin"] = config_data['dipinnum']
                if config_data['dipinnum'] == 16:
                    item["pin_cfg"][1]["fun"] = "fun2"
                elif config_data['dipinnum'] == 19:
                    item["pin_cfg"][1]["fun"] = "fun2"
                elif config_data['dipinnum'] == 32:
                    item["pin_cfg"][1]["fun"] = "fun3"
                else:#41
                    item["pin_cfg"][1]["fun"] = "fun1"
                item["pin_cfg"][2]["pin"] = config_data['dopinnum']
                if config_data['dopinnum'] == 7:
                    item["pin_cfg"][2]["fun"] = "fun2"
                elif config_data['dopinnum'] == 21:
                    item["pin_cfg"][2]["fun"] = "fun1"
                elif config_data['dopinnum'] == 33:
                    item["pin_cfg"][2]["fun"] = "fun3"
                else:#42
                    item["pin_cfg"][2]["fun"] = "fun1"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_touch_sensor_config(self, data):
        if data['dev_name'] == 'Touch_Sensor':
            self.touch_sensor_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "touch_sensor":
                config_data = self.touch_sensor_config_data
                item["pin_cfg"][0]["pin"] = config_data['cmodpinnum']
                item["pin_cfg"][0]["fun"] = "fun7"
                item["pin_cfg"][1]["pin"] = config_data['cdcpinnum']
                item["pin_cfg"][1]["fun"] = "fun7"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_touch_button_config(self, data):
        if data['dev_name'] == 'Touch_Button':
            self.touch_button_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "touch_button":
                config_data = self.touch_button_config_data
                item["touch_button_cfg"].clear()
                item["pin_cfg"].clear()
                for i in range(1, 16):
                    if config_data["enable" + str(i)]:
                        item["touch_button_cfg"].append({"key_num":i, "threshold":config_data["threshold" + str(i)]})
                        item["pin_cfg"].append({"pin":config_data["pinnum" + str(i)], "fun":"fun7"})
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_pwm_config(self, data):
        if data['dev_name'] == 'PWM':
            self.pwm_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "pwm":
                pwm_pin_num = [0, 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 24, 28, 29, 30, 31, 32, 33, 35, 36, 40, 41, 42]
                pwm_pin_func = ["fun3", "fun3", "fun3", "fun3", "fun3", "fun3", "fun1", "fun1", "fun3", "fun3", "fun3", "fun3", "fun3", "fun3", "fun1", "fun1", "fun1", "fun1", "fun3", "fun2", "fun2", "fun2", "fun2", "fun2", "fun2", "fun2", "fun2", "fun2", "fun2", "fun2"]
                config_data = self.pwm_config_data
                item["pin_cfg"].clear()
                for ch in range(0, 6):
                    for i in range(1, 6):
                        if config_data["enable" + str(ch) + "_" + str(i)]:
                            item["pin_cfg"].append({"pin":config_data["pinnum" + str(ch) + "_" + str(i)], "fun":pwm_pin_func[pwm_pin_num.index(config_data["pinnum" + str(ch) + "_" + str(i)])]})
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_rtc_config(self, data):
        if data['dev_name'] == 'RTC':
            self.rtc_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "rtc":
                config_data = self.rtc_config_data
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_pmu_config(self, data):
        if data['dev_name'] == 'PMU':
            self.pmu_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "pmu":
                config_data = self.pmu_config_data
                if config_data["clk_src"]:
                    item["clk_src"] = "external"
                else:
                    item["clk_src"] = "internal"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_i2s_config(self, data):
        if data['dev_name'] == 'I2S':
            self.i2s_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "i2s":
                config_data = self.i2s_config_data
                if config_data["extal_clock_en"]:
                    item["i2s_cfg"]["extal_clock_en"] = True
                else:
                    item["i2s_cfg"]["extal_clock_en"] = False
                item["i2s_cfg"]["mclk_hz"] = config_data["extal_clock_hz"]

                mclk_pinnum = [0, 7, 33]
                mclk_pinfun = ["fun1", "fun3", "fun4"]

                di_pinnum   = [7, 11, 26, 31]
                di_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                do_pinnum   = [0, 10, 27, 30]
                do_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                to_pop = []
                for pin_cfg in item["pin_cfg"]:
                    for i in range(0, len(mclk_pinnum)):
                        if pin_cfg["pin"] == mclk_pinnum[i] and pin_cfg["fun"] == mclk_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break
                    for i in range(0, len(di_pinnum)):
                        if pin_cfg["pin"] == di_pinnum[i] and pin_cfg["fun"] == di_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break
                    for i in range(0, len(do_pinnum)):
                        if pin_cfg["pin"] == do_pinnum[i] and pin_cfg["fun"] == do_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break

                for i in reversed(to_pop):
                    item["pin_cfg"].pop(i)

                item["pin_cfg"][0]["pin"] = config_data["bclkpinnum"]
                if config_data["bclkpinnum"] == 4:
                    item["pin_cfg"][0]["fun"] = "fun4"
                elif config_data["bclkpinnum"] == 8:
                    item["pin_cfg"][0]["fun"] = "fun4"
                elif config_data["bclkpinnum"] == 24:
                    item["pin_cfg"][0]["fun"] = "fun1"
                else:#28
                    item["pin_cfg"][0]["fun"] = "fun4"
                item["pin_cfg"][1]["pin"] = config_data["lrclkpinnum"]
                if config_data["lrclkpinnum"] == 1:
                    item["pin_cfg"][1]["fun"] = "fun4"
                elif config_data["lrclkpinnum"] == 9:
                    item["pin_cfg"][1]["fun"] = "fun4"
                elif config_data["lrclkpinnum"] == 25:
                    item["pin_cfg"][1]["fun"] = "fun1"
                else:#29
                    item["pin_cfg"][1]["fun"] = "fun4"

                if config_data["enable_mclk"]:
                    if config_data["mclkpinnum"] == 0:
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun1"})
                    elif config_data["mclkpinnum"] == 7:
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun3"})
                    else:#33
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun4"})

                if config_data["enable_di"]:
                    if config_data["dipinnum"] == 7:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})
                    elif config_data["dipinnum"] == 11:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})
                    elif config_data["dipinnum"] == 26:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun1"})
                    else:#31
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})

                if config_data["enable_do"]:
                    if config_data["dopinnum"] == 0:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})
                    elif config_data["dopinnum"] == 10:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})
                    elif config_data["dopinnum"] == 27:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun1"})
                    else:#30
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})

                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_seg_lcd_config(self, data):
        if data['dev_name'] == 'SEG_LCD':
            self.seg_lcd_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "seg_lcd":
                config_data = self.seg_lcd_config_data

                if config_data["duty_sel"] == 1:
                    item["seg_lcd_cfg"]["duty_sel"] = "1/2"
                elif config_data["duty_sel"] == 2:
                    item["seg_lcd_cfg"]["duty_sel"] = "1/3"
                elif config_data["duty_sel"] == 3:
                    item["seg_lcd_cfg"]["duty_sel"] = "1/4"
                elif config_data["duty_sel"] == 4:
                    item["seg_lcd_cfg"]["duty_sel"] = "1/5"
                elif config_data["duty_sel"] == 5:
                    item["seg_lcd_cfg"]["duty_sel"] = "1/6"
                elif config_data["duty_sel"] == 6:
                    item["seg_lcd_cfg"]["duty_sel"] = "1/7"
                elif config_data["duty_sel"] == 7:
                    item["seg_lcd_cfg"]["duty_sel"] = "1/8"
                else:
                    item["seg_lcd_cfg"]["duty_sel"] = "static".lower()

                if config_data["vlcd_cc"] == 1:
                    item["seg_lcd_cfg"]["vlcd_cc"] = "2.9v".lower()
                elif config_data["vlcd_cc"] == 2:
                    item["seg_lcd_cfg"]["vlcd_cc"] = "3.1v".lower()
                elif config_data["vlcd_cc"] == 3:
                    item["seg_lcd_cfg"]["vlcd_cc"] = "3.3v".lower()
                else:
                    item["seg_lcd_cfg"]["vlcd_cc"] = "2.7v".lower()

                if config_data["bias"] == 1:
                    item["seg_lcd_cfg"]["bias"] = "1/2"
                elif config_data["bias"] == 2:
                    item["seg_lcd_cfg"]["bias"] = "1/3"
                elif config_data["bias"] == 3:
                    item["seg_lcd_cfg"]["bias"] = "static".lower()
                else:
                    item["seg_lcd_cfg"]["bias"] = "1/4"

                if config_data["hd"] == 1:
                    item["seg_lcd_cfg"]["hd"] = "high".lower()
                else:
                    item["seg_lcd_cfg"]["hd"] = "low".lower()

                item["seg_lcd_cfg"]["frame_freq"] = config_data["frame_freq"]
                item["seg_lcd_cfg"]["com_num"] = config_data["com_num"]

                item["pin_cfg"].clear()
                for i in range(0, 46):
                    if config_data["enable"][i]:
                        item["pin_cfg"].append({"pin":i, "fun":"fun6"})
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_adc_config(self, data):
        if data['dev_name'] == 'ADC':
            self.adc_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "adc":
                config_data = self.adc_config_data
                item["adc_cfg"].clear()
                item["pin_cfg"].clear()
                for i in range(0, 6):
                    if config_data["enable"][i]:
                        adc_cmp = False
                        cmp_pol = False
                        if config_data["adc_cmp"][i]:
                            adc_cmp = True
                        if config_data["cmp_pol"][i]:
                            cmp_pol = True
                        ch = i
                        if i == 4:
                            ch = 8
                        elif i == 5:
                            ch = 9
                        item["adc_cfg"].append({"adc_channel":ch, "pga_gain1":"level" + str(config_data["pga_gain1"][i]), "pga_gain2":"level" + str(config_data["pga_gain2"][i]), "adc_cmp":adc_cmp, "cmp_data":config_data["cmp_data"][i], "cmp_pol":cmp_pol})
                        item["pin_cfg"].append({"pin":config_data["pinnum0"][i], "fun":"fun6"})
                        if i == 4 or i == 5:
                            item["pin_cfg"].append({"pin":config_data["pinnum1"][i], "fun":"fun6"})
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_gpio_config(self, data):
        if data['dev_name'] == 'GPIO':
            self.gpio_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "gpio":
                config_data = self.gpio_config_data
                item["gpio_cfg"].clear()
                for i in range(0, 46):
                    if config_data["enable"][i]:
                        dir = "input"
                        if config_data["pindir"][i]:
                            dir = "output"
                        pupd = "float"
                        if config_data["pinpupd"][i] == 1:
                            pupd = "pull_up"
                        elif config_data["pinpupd"][i] == 2:
                            pupd = "pull_down"
                        interrupt_mode = "none"
                        if config_data["pinint"][i] == 1:
                            interrupt_mode = "falling_edge"
                        elif config_data["pinint"][i] == 2:
                            interrupt_mode = "rising_edge"
                        elif config_data["pinint"][i] == 3:
                            interrupt_mode = "double_edge"
                        elif config_data["pinint"][i] == 4:
                            interrupt_mode = "low_level"
                        elif config_data["pinint"][i] == 5:
                            interrupt_mode = "high_level"
                        item["gpio_cfg"].append({"pin":i, "fun":"fun5", "dir":dir, "pupd":pupd, "interrupt_mode":interrupt_mode})
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_wdt_config(self, data):
        if data['dev_name'] == 'WDT':
            self.wdt_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "wdt":
                config_data = self.wdt_config_data
                item["wdt_cfg"]["counter_value"] = config_data["counter_value"]
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break

    def set_psram_config(self, data):
        if data['dev_name'] == 'PSRAM':
            self.psram_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "psram":
                config_data = self.psram_config_data
                if config_data["mode"]:
                    item["psram_cfg"]["qspi"] = True
                else:
                    item["psram_cfg"]["qspi"] = False
                item["psram_cfg"]["clock_hz"] = config_data["clock"]
                item["pin_cfg"][0]["pin"] = config_data["ckpinnum"]
                if config_data["ckpinnum"] == 15:
                    item["pin_cfg"][0]["fun"] = "fun1"
                else:#16
                    item["pin_cfg"][0]["fun"] = "fun4"
                item["pin_cfg"][1]["pin"] = config_data["cspinnum"]
                if config_data["cspinnum"] == 17:
                    item["pin_cfg"][1]["fun"] = "fun4"
                else:#43
                    item["pin_cfg"][1]["fun"] = "fun1"
                item["pin_cfg"][2]["pin"] = config_data["d0pinnum"]
                if config_data["d0pinnum"] == 18:
                    item["pin_cfg"][2]["fun"] = "fun4"
                else:#44
                    item["pin_cfg"][2]["fun"] = "fun1"
                item["pin_cfg"][3]["pin"] = config_data["d1pinnum"]
                if config_data["d1pinnum"] == 19:
                    item["pin_cfg"][3]["fun"] = "fun4"
                else:#45
                    item["pin_cfg"][3]["fun"] = "fun1"
                item["pin_cfg"][4]["pin"] = config_data["d2pinnum"]
                if config_data["d2pinnum"] == 20:
                    item["pin_cfg"][4]["fun"] = "fun4"
                else:#46
                    item["pin_cfg"][4]["fun"] = "fun1"
                item["pin_cfg"][5]["pin"] = config_data["d3pinnum"]
                if config_data["d3pinnum"] == 21:
                    item["pin_cfg"][5]["fun"] = "fun4"
                else:#47
                    item["pin_cfg"][5]["fun"] = "fun1"
                self.update_init_cfg(config_data, item)

                self.update_config_file()
                break
