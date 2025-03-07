import os
import sys
import time
import argparse
import threading

from waitress import serve
from flask import Flask, request, Response, render_template
from config_helper import ConfigHelper

if sys.version_info.major == 3:
    # Python3
    from urllib.parse import quote
    from urllib.parse import unquote

parser = argparse.ArgumentParser(usage = None, description='wmsdk device config', prog = "dev_conf.py")
parser.add_argument("-i", "--ip", help = "dev config server ip address", default = "")
parser.add_argument("-p", "--port", help = "dev config server port", default = "")
parser.add_argument("-c", "--config", help = "kconfig file", default = "")
parser.add_argument("-t", "--toml", help = "device table toml file", default = "")
parser.add_argument("-a", "--alive", help = "keep alive time (second)", type=int, default = 0)

default_ip = '127.0.0.1'
default_port = 31118

parser_args = parser.parse_args()
configHelper = ConfigHelper(parser_args.toml, parser_args.config)

app = Flask(__name__)
app.config['STATIC_FOLDER'] = 'static'
app.config['STATIC_URL'] = '/static/'

alive_time = time.time()
@app.route("/keep_alive", methods=["POST"])
def keep_alive():
    global alive_time
    alive_time = time.time()
    return Response(status=200)

@app.route('/')
def home():
    return render_template('default.html')

@app.route('/wmdt/chip', methods = ['GET', 'POST'])
def wmdt_chip():
    return render_template('chip.html', data=configHelper.get_devices_pin())

@app.route('/wmdt/index', methods = ['GET', 'POST'])
def wmdt_index():
    dev_name = 'Clock'
    data = {'dev_name':'Clock'}
    method = request.method
    if method == 'POST':
        if request.form.get("dev_name2"):
            dev_name = request.form.get("dev_name2")
        if dev_name.startswith('Clock'):
            data = configHelper.get_clock_config(dev_name)
            data['clk_div_cpu'] = int(request.form.get("clk_div_cpu"))
            data['clk_div_wlan'] = int(request.form.get("clk_div_wlan"))
            #data['clk_div_peri'] = int(request.form.get("clk_div_peri"))
            #data['clk_div_adc'] = int(request.form.get("clk_div_adc"))
            data['dev_name'] = dev_name
            configHelper.set_clock_config(data)
            #return "success"
        elif dev_name.startswith('Uart'):
            #data = {'baud':115200, 'parity':0, 'stopbits':1, 'databits':8, 'flowctrl':0, 'initlevel':0, 'initpriority':0}
            data = configHelper.get_uart_config(dev_name)
            data['baud'] = int(request.form.get("baud"))
            data['parity'] = int(request.form.get("parity"))
            data['databits'] = int(request.form.get("databits"))
            data['flowctrl'] = int(request.form.get("flowctrl"))
            data['stopbits'] = int(request.form.get("stopbits"))
            data['pintx'] = int(request.form.get("pintx"))
            data['pinrx'] = int(request.form.get("pinrx"))
            try:
                data['pinrts'] = int(request.form.get("pinrts"))
            except TypeError:
                data['pinrts'] = 0
            except ValueError:
                data['pinrts'] = 0
            try:
                data['pincts'] = int(request.form.get("pincts"))
            except TypeError:
                data['pincts'] = 0
            except ValueError:
                data['pincts'] = 0
            try:
                data['enable_dma'] = int(request.form.get("enable_dma"))
            except TypeError:
                data['enable_dma'] = 0
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_uart_config(data)
        elif dev_name.startswith('Timer'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_timer_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_timer_config(data)
        elif dev_name.startswith('Internal_flash'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_iflash_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            try:
                data['quad_spi'] = int(request.form.get("quad_spi"))
            except TypeError:
                data['quad_spi'] = 1
            data['dev_name'] = dev_name
            configHelper.set_iflash_config(data)
        elif dev_name.startswith('External_flash_'):
            #data = {'initlevel':0, 'initpriority':0}
            if dev_name.startswith('External_flash_w25q'):
                data = configHelper.get_eflash_w25q_config(dev_name)
            elif dev_name.startswith('External_flash_gd25q'):
                data = configHelper.get_eflash_gd25q_config(dev_name)
            elif dev_name.startswith('External_flash_xt25f'):
                data = configHelper.get_eflash_xt25f_config(dev_name)
            elif dev_name.startswith('External_flash_th25q'):
                data = configHelper.get_eflash_th25q_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            try:
                data['quad_spi'] = int(request.form.get("quad_spi"))
            except TypeError:
                data['quad_spi'] = 0
            data['mode'] = int(request.form.get("mode"))
            data['freq'] = int(request.form.get("freq"))
            data['cspinnum'] = int(request.form.get("cspinnum"))
            data['dev_name'] = dev_name
            if dev_name.startswith('External_flash_w25q'):
                configHelper.set_eflash_w25q_config(data)
            elif dev_name.startswith('External_flash_gd25q'):
                configHelper.set_eflash_gd25q_config(data)
            elif dev_name.startswith('External_flash_xt25f'):
                configHelper.set_eflash_xt25f_config(data)
            elif dev_name.startswith('External_flash_th25q'):
                configHelper.set_eflash_th25q_config(data)
        elif dev_name.startswith('CRYPTO'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_crypto_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_crypto_config(data)
        elif dev_name.startswith('CRC'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_crc_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_crc_config(data)
        elif dev_name.startswith('HASH'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_hash_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_hash_config(data)
        elif dev_name.startswith('RNG'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_rng_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_rng_config(data)
        elif dev_name.startswith('RSA'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_rsa_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_rsa_config(data)
        elif dev_name.startswith('SDMMC'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_sdmmc_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['clock'] = int(request.form.get("clock"))
            data['bus_width'] = int(request.form.get("bus_width"))
            data['clkpinnum'] = int(request.form.get("clkpinnum"))
            data['cmdpinnum'] = int(request.form.get("cmdpinnum"))
            data['dat0pinnum'] = int(request.form.get("dat0pinnum"))
            data['dat1pinnum'] = int(request.form.get("dat1pinnum"))
            data['dat2pinnum'] = int(request.form.get("dat2pinnum"))
            data['dat3pinnum'] = int(request.form.get("dat3pinnum"))
            data['dev_name'] = dev_name
            configHelper.set_sdmmc_config(data)
        elif dev_name.startswith('SDSPI'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_sdspi_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['clkpinnum'] = int(request.form.get("clkpinnum"))
            data['cmdpinnum'] = int(request.form.get("cmdpinnum"))
            data['dev_name'] = dev_name
            configHelper.set_sdspi_config(data)
        elif dev_name.startswith('NV3041A'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_nv3041a_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['mode'] = int(request.form.get("mode"))
            data['freq'] = int(request.form.get("freq"))
            data['cspinnum'] = int(request.form.get("cspinnum"))
            data['resetpinnum'] = int(request.form.get("resetpinnum"))
            data['ledpinnum'] = int(request.form.get("ledpinnum"))
            data['dcxpinnum'] = int(request.form.get("dcxpinnum"))
            data['dev_name'] = dev_name
            configHelper.set_nv3041a_config(data)
        elif dev_name.startswith('ST7735'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_st7735_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['mode'] = int(request.form.get("mode"))
            data['freq'] = int(request.form.get("freq"))
            data['cspinnum'] = int(request.form.get("cspinnum"))
            data['resetpinnum'] = int(request.form.get("resetpinnum"))
            data['ledpinnum'] = int(request.form.get("ledpinnum"))
            data['dcxpinnum'] = int(request.form.get("dcxpinnum"))
            data['dev_name'] = dev_name
            configHelper.set_st7735_config(data)
        elif dev_name.startswith('GZ035'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_gz035_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['mode'] = int(request.form.get("mode"))
            data['freq'] = int(request.form.get("freq"))
            data['cspinnum'] = int(request.form.get("cspinnum"))
            data['resetpinnum'] = int(request.form.get("resetpinnum"))
            data['ledpinnum'] = int(request.form.get("ledpinnum"))
            data['dcxpinnum'] = int(request.form.get("dcxpinnum"))
            try:
                data['tepinnum'] = int(request.form.get("tepinnum"))
            except TypeError:
                data['tepinnum'] = -1
            data['dev_name'] = dev_name
            configHelper.set_gz035_config(data)
        elif dev_name.startswith('SDIO_Slave'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_sdio_slave_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['clkpinnum'] = int(request.form.get("clkpinnum"))
            data['cmdpinnum'] = int(request.form.get("cmdpinnum"))
            data['dat0pinnum'] = int(request.form.get("dat0pinnum"))
            data['dat1pinnum'] = int(request.form.get("dat1pinnum"))
            data['dat2pinnum'] = int(request.form.get("dat2pinnum"))
            data['dat3pinnum'] = int(request.form.get("dat3pinnum"))
            data['dev_name'] = dev_name
            configHelper.set_sdio_slave_config(data)
        elif dev_name.startswith('I2C'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_i2c_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['clock'] = int(request.form.get("clock"))
            try:
                data['addr_10_bits'] = int(request.form.get("addr_10_bits"))
            except TypeError:
                data['addr_10_bits'] = 0
            data['sclpinnum'] = int(request.form.get("sclpinnum"))
            data['sdapinnum'] = int(request.form.get("sdapinnum"))
            data['dev_name'] = dev_name
            configHelper.set_i2c_config(data)
        elif dev_name.startswith('eeprom0'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_eeprom0_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['speed_hz'] = int(request.form.get("speed_hz"))
            data['size'] = int(request.form.get("size"))
            data['i2c_addr'] = request.form.get("i2c_addr")
            data['page_size'] = int(request.form.get("page_size"))
            data['addr_width'] = int(request.form.get("addr_width"))
            try:
                data['read_only'] = int(request.form.get("read_only"))
            except TypeError:
                data['read_only'] = 0
            data['max_write_time_ms'] = int(request.form.get("max_write_time_ms"))
            try:
                data['enable'] = int(request.form.get("enable"))
            except TypeError:
                data['enable'] = 0
            try:
                data['wppinnum'] = int(request.form.get("wppinnum"))
            except TypeError:
                data['wppinnum'] = 0
            data['dev_name'] = dev_name
            configHelper.set_eeprom0_config(data)
        elif dev_name.startswith('eeprom1'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_eeprom1_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['speed_hz'] = int(request.form.get("speed_hz"))
            data['size'] = int(request.form.get("size"))
            data['i2c_addr'] = request.form.get("i2c_addr")
            data['page_size'] = int(request.form.get("page_size"))
            data['addr_width'] = int(request.form.get("addr_width"))
            try:
                data['read_only'] = int(request.form.get("read_only"))
            except TypeError:
                data['read_only'] = 0
            data['max_write_time_ms'] = int(request.form.get("max_write_time_ms"))
            try:
                data['enable'] = int(request.form.get("enable"))
            except TypeError:
                data['enable'] = 0
            try:
                data['wppinnum'] = int(request.form.get("wppinnum"))
            except TypeError:
                data['wppinnum'] = 0
            data['dev_name'] = dev_name
            configHelper.set_eeprom1_config(data)
        elif dev_name.startswith('SPI_Master'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_spim_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['clkpinnum'] = int(request.form.get("clkpinnum"))
            data['dipinnum'] = int(request.form.get("dipinnum"))
            data['dopinnum'] = int(request.form.get("dopinnum"))
            data['dev_name'] = dev_name
            configHelper.set_spim_config(data)
        elif dev_name.startswith('Touch_Sensor'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_touch_sensor_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['cmodpinnum'] = int(request.form.get("cmodpinnum"))
            data['cdcpinnum'] = int(request.form.get("cdcpinnum"))
            data['dev_name'] = dev_name
            configHelper.set_touch_sensor_config(data)
        elif dev_name.startswith('Touch_Button'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_touch_button_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            for i in range(1, 16):
                try:
                    data['enable' + str(i)] = int(request.form.get("enable" + str(i)))
                except TypeError:
                    data['enable' + str(i)] = 0
                try:
                    data['threshold' + str(i)] = int(request.form.get("threshold" + str(i)))
                except TypeError:
                    data['threshold' + str(i)] = 0
                try:
                    data['pinnum' + str(i)] = int(request.form.get("pinnum" + str(i)))
                except TypeError:
                    data['pinnum' + str(i)] = 0
            data['dev_name'] = dev_name
            configHelper.set_touch_button_config(data)
        elif dev_name.startswith('PWM'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_pwm_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            for j in range(0, 6):
                for i in range(1, 16):
                    try:
                        data['enable' + str(j) + '_' + str(i)] = int(request.form.get("enable" + str(j) + '_' + str(i)))
                    except TypeError:
                        data['enable' + str(j) + '_' + str(i)] = 0
                    try:
                        data['pinnum' + str(j) + '_' + str(i)] = int(request.form.get("pinnum" + str(j) + '_' + str(i)))
                    except TypeError:
                        data['pinnum' + str(j) + '_' + str(i)] = 0
            data['dev_name'] = dev_name
            configHelper.set_pwm_config(data)
        elif dev_name.startswith('RTC'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_rtc_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['dev_name'] = dev_name
            configHelper.set_rtc_config(data)
        elif dev_name.startswith('PMU'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_pmu_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['clk_src'] = int(request.form.get("clk_src"))
            data['dev_name'] = dev_name
            configHelper.set_pmu_config(data)
        elif dev_name.startswith('I2S'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_i2s_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['extal_clock_en'] = int(request.form.get("extal_clock_en"))
            data['extal_clock_hz'] = int(request.form.get("extal_clock_hz"))
            try:
                data['enable_mclk'] = int(request.form.get("enable_mclk"))
            except TypeError:
                data['enable_mclk'] = 0
            try:
                data['mclkpinnum'] = int(request.form.get("mclkpinnum"))
            except TypeError:
                data['mclkpinnum'] = 0
            data['bclkpinnum'] = int(request.form.get("bclkpinnum"))
            data['lrclkpinnum'] = int(request.form.get("lrclkpinnum"))
            try:
                data['enable_di'] = int(request.form.get("enable_di"))
            except TypeError:
                data['enable_di'] = 0
            try:
                data['dipinnum'] = int(request.form.get("dipinnum"))
            except TypeError:
                data['dipinnum'] = 0
            try:
                data['enable_do'] = int(request.form.get("enable_do"))
            except TypeError:
                data['enable_do'] = 0
            try:
                data['dopinnum'] = int(request.form.get("dopinnum"))
            except TypeError:
                data['dopinnum'] = 0
            data['dev_name'] = dev_name
            configHelper.set_i2s_config(data)
        elif dev_name.startswith('SEG_LCD'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_seg_lcd_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['duty_sel'] = int(request.form.get("duty_sel"))
            data['vlcd_cc'] = int(request.form.get("vlcd_cc"))
            data['bias'] = int(request.form.get("bias"))
            data['hd'] = int(request.form.get("hd"))
            data['frame_freq'] = int(request.form.get("frame_freq"))
            data['com_num'] = int(request.form.get("com_num"))
            data['dev_name'] = dev_name
            configHelper.set_seg_lcd_config(data)
        elif dev_name.startswith('ADC'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_adc_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            for i in range(0, 6):
                try:
                    data['enable'][i] = int(request.form.get("enable_" + str(i)))
                except TypeError:
                    data['enable'][i] = 0
                try:
                    data['pga_gain1'][i] = int(request.form.get("pga_gain1_" + str(i)))
                except TypeError:
                    data['pga_gain1'][i] = 0
                try:
                    data['pga_gain2'][i] = int(request.form.get("pga_gain2_" + str(i)))
                except TypeError:
                    data['pga_gain2'][i] = 0
                try:
                    data['cmp_data'][i] = int(request.form.get("cmp_data_" + str(i)))
                except TypeError:
                    data['cmp_data'][i] = 0
                try:
                    data['adc_cmp'][i] = int(request.form.get("adc_cmp_" + str(i)))
                except TypeError:
                    data['adc_cmp'][i] = 0
                try:
                    data['cmp_pol'][i] = int(request.form.get("cmp_pol_" + str(i)))
                except TypeError:
                    data['cmp_pol'][i] = 0
                try:
                    data['pinnum0'][i] = int(request.form.get("pinnum0_" + str(i)))
                except TypeError:
                    data['pinnum0'][i] = 0
                try:
                    data['pinnum1'][i] = int(request.form.get("pinnum1_" + str(i)))
                except TypeError:
                    data['pinnum1'][i] = 0
            data['dev_name'] = dev_name
            configHelper.set_adc_config(data)
        elif dev_name.startswith('GPIO'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_gpio_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            for i in range(0, 46):
                try:
                    data['enable'][i] = int(request.form.get("enable" + str(i)))
                except TypeError:
                    data['enable'][i] = 0
                try:
                    data['pinnum'][i] = int(request.form.get("pinnum" + str(i)))
                except TypeError:
                    data['pinnum'][i] = 0
                try:
                    data['pindir'][i] = int(request.form.get("pindir" + str(i)))
                except TypeError:
                    data['pindir'][i] = 0
                try:
                    data['pinpupd'][i] = int(request.form.get("pinpupd" + str(i)))
                except TypeError:
                    data['pinpupd'][i] = 0
                try:
                    data['pinint'][i] = int(request.form.get("pinint" + str(i)))
                except TypeError:
                    data['pinint'][i] = 0
            data['dev_name'] = dev_name
            configHelper.set_gpio_config(data)
        elif dev_name.startswith('WDT'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_wdt_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['counter_value'] = int(request.form.get("counter_value"))
            data['dev_name'] = dev_name
            configHelper.set_wdt_config(data)
        elif dev_name.startswith('PSRAM'):
            #data = {'initlevel':0, 'initpriority':0}
            data = configHelper.get_psram_config(dev_name)
            try:
                data['initlevel'] = int(request.form.get("initlevel"))
            except TypeError:
                data['initlevel'] = 0
            try:
                data['initpriority'] = int(request.form.get("initpriority"))
            except TypeError:
                data['initpriority'] = 0
            data['mode'] = int(request.form.get("mode"))
            data['clock'] = int(request.form.get("clock"))
            data['ckpinnum'] = int(request.form.get("ckpinnum"))
            data['cspinnum'] = int(request.form.get("cspinnum"))
            data['d0pinnum'] = int(request.form.get("d0pinnum"))
            data['d1pinnum'] = int(request.form.get("d1pinnum"))
            data['d2pinnum'] = int(request.form.get("d2pinnum"))
            data['d3pinnum'] = int(request.form.get("d3pinnum"))
            data['dev_name'] = dev_name
            configHelper.set_psram_config(data)

        return "<script>alert('Save successfully');window.history.back();location.reload();</script>"
    elif method == 'GET':
        if request.args.get('dev_name'):
            dev_name = request.args.get('dev_name')

    if dev_name.startswith('Clock'):
        return render_template('index.html', data=configHelper.get_clock_config(dev_name))
    elif dev_name.startswith('Uart'):
        return render_template('index.html', data=configHelper.get_uart_config(dev_name))
    elif dev_name.startswith('Timer'):
        return render_template('index.html', data=configHelper.get_timer_config(dev_name))
    elif dev_name.startswith('Internal_flash'):
        return render_template('index.html', data=configHelper.get_iflash_config(dev_name))
    elif dev_name.startswith('External_flash_'):
        if dev_name.startswith('External_flash_w25q'):
            return render_template('index.html', data=configHelper.get_eflash_w25q_config(dev_name))
        elif dev_name.startswith('External_flash_gd25q'):
            return render_template('index.html', data=configHelper.get_eflash_gd25q_config(dev_name))
        elif dev_name.startswith('External_flash_xt25f'):
            return render_template('index.html', data=configHelper.get_eflash_xt25f_config(dev_name))
        elif dev_name.startswith('External_flash_th25q'):
            return render_template('index.html', data=configHelper.get_eflash_th25q_config(dev_name))
    elif dev_name.startswith('CRYPTO'):
        return render_template('index.html', data=configHelper.get_crypto_config(dev_name))
    elif dev_name.startswith('CRC'):
        return render_template('index.html', data=configHelper.get_crc_config(dev_name))
    elif dev_name.startswith('HASH'):
        return render_template('index.html', data=configHelper.get_hash_config(dev_name))
    elif dev_name.startswith('RNG'):
        return render_template('index.html', data=configHelper.get_rng_config(dev_name))
    elif dev_name.startswith('RSA'):
        return render_template('index.html', data=configHelper.get_rsa_config(dev_name))
    elif dev_name.startswith('SDMMC'):
        return render_template('index.html', data=configHelper.get_sdmmc_config(dev_name))
    elif dev_name.startswith('SDSPI'):
        return render_template('index.html', data=configHelper.get_sdspi_config(dev_name))
    elif dev_name.startswith('NV3041A'):
        return render_template('index.html', data=configHelper.get_nv3041a_config(dev_name))
    elif dev_name.startswith('ST7735'):
        return render_template('index.html', data=configHelper.get_st7735_config(dev_name))
    elif dev_name.startswith('GZ035'):
        return render_template('index.html', data=configHelper.get_gz035_config(dev_name))
    elif dev_name.startswith('SDIO_Slave'):
        return render_template('index.html', data=configHelper.get_sdio_slave_config(dev_name))
    elif dev_name.startswith('I2C'):
        return render_template('index.html', data=configHelper.get_i2c_config(dev_name))
    elif dev_name.startswith('eeprom0'):
        return render_template('index.html', data=configHelper.get_eeprom0_config(dev_name))
    elif dev_name.startswith('eeprom1'):
        return render_template('index.html', data=configHelper.get_eeprom1_config(dev_name))
    elif dev_name.startswith('SPI_Master'):
        return render_template('index.html', data=configHelper.get_spim_config(dev_name))
    elif dev_name.startswith('Touch_Sensor'):
        return render_template('index.html', data=configHelper.get_touch_sensor_config(dev_name))
    elif dev_name.startswith('Touch_Button'):
        return render_template('index.html', data=configHelper.get_touch_button_config(dev_name))
    elif dev_name.startswith('PWM'):
        return render_template('index.html', data=configHelper.get_pwm_config(dev_name))
    elif dev_name.startswith('RTC'):
        return render_template('index.html', data=configHelper.get_rtc_config(dev_name))
    elif dev_name.startswith('PMU'):
        return render_template('index.html', data=configHelper.get_pmu_config(dev_name))
    elif dev_name.startswith('I2S'):
        return render_template('index.html', data=configHelper.get_i2s_config(dev_name))
    elif dev_name.startswith('SEG_LCD'):
        return render_template('index.html', data=configHelper.get_seg_lcd_config(dev_name))
    elif dev_name.startswith('ADC'):
        return render_template('index.html', data=configHelper.get_adc_config(dev_name))
    elif dev_name.startswith('GPIO'):
        return render_template('index.html', data=configHelper.get_gpio_config(dev_name))
    elif dev_name.startswith('WDT'):
        return render_template('index.html', data=configHelper.get_wdt_config(dev_name))
    elif dev_name.startswith('PSRAM'):
        return render_template('index.html', data=configHelper.get_psram_config(dev_name))
    else:
        return render_template('index.html', data=data)

class HTTPServer:
    def __init__(self, args):
        self.args = args
        self.thread = None  # Thread for running the server
        self.is_connected = False  # Flag to check if the server is running

    def serve_forever(self):
        try:
            if self.args.ip:
                server_host = self.args.ip
            else:
                server_host = default_ip
            if self.args.port:
                server_port = self.args.port
            else:
                server_port = default_port

            #app.run(host=server_host, port=server_port)
            serve(app, host=server_host, port=server_port, threads=8, log_socket_errors=False)
            self.is_connected = True
        except Exception as e:
            print(f"An error occurred: {e}")

    def start_server(self):
        if self.is_connected:
            return
        try:
            self.thread = threading.Thread(target=self.serve_forever)
            self.thread.daemon = True
            self.thread.start()
        except Exception as e:
            print(f"An error occurred: {e}")

if __name__ == '__main__':
    http_server = HTTPServer(parser_args)
    http_server.start_server()
    print("device config server running");
    try:
        while True:
            if parser_args.alive:
                if (time.time() - alive_time) > parser_args.alive:
                    break
                else:
                    time.sleep(0.5)
            else:
                time.sleep(1000)
        print("client closed, exiting device config server...")
        sys.exit(0)
    except KeyboardInterrupt:
        print("user exiting device config server...")
