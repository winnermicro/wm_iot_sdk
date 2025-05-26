const clockConfig = {
    pll: {
        text: 'PLL',
        frequency: ''
    },
    dividers: {
        div3: {
            text: window.GLOBAL_CONFIG.wm_cpuFreqDiv,
            options: ['1/2', '1/3', '1/4', '1/5', '1/6', '1/8', '1/10', '1/12', '1/15', '1/16', '1/20', '1/24', '1/30', '1/32', '1/40', '1/48', '1/60', '1/80', '1/96', '1/120', '1/160', '1/240']
        },
        div4: {
            text: window.GLOBAL_CONFIG.wm_wlanFreqDiv,
            options: ['1/3', '1/4', '1/5','1/6', '1/8', '1/10', '1/12', '1/15', '1/20', '1/24', '1/30', '1/40', '1/60', '1/120', '1/240']
        },
        div3Second: {
            text: '1/3'
        }
    },
    middleNodes: {
        div3Sub1: {
            text: '1/1'
        },
        div3Sub2: {
            text: '1/2'
        },
        div3Sub3: {
            text: '1/4'
        },
        div1First: {
            text: '1/1'
        },
        div4Second: {
            text: '1/4'
        },
        div40: {
            text: '1/40'
        },
        empty1: {
            text: ''
        },
        apb: {
            text: 'APB'
        }
    },
    terminalNodes: {
        cpu: { 
            text: 'CPU', 
            freq: window.GLOBAL_CONFIG.wm_cpuFreq
        },
        dma: { 
            text: 'DMA', 
            freq: window.GLOBAL_CONFIG.wm_cpuFreq
        },
        wifi: { 
            text: 'WiFi', 
            freq: window.GLOBAL_CONFIG.wm_wlanFreq, 
            special: true 
        },
        sdio_slave: { 
            text: 'SDIO SLAVE', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        high_spi_slave: { 
            text: 'HIGH SPI SLAVE', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        i2c: { 
            text: 'I2C', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        spi: { 
            text: 'SPI', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        uart: { 
            text: 'UART', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        card_7816: { 
            text: '7816', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        gpio: { 
            text: 'GPIO', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        wdt: { 
            text: 'WDT', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        timer: { 
            text: 'TIMER', 
            freq: window.GLOBAL_CONFIG.wm_apbFreq
        },
        rsa: { 
            text: 'RSA', 
            freq: '160MHz' 
        },
        gpsec: { 
            text: 'GPSEC', 
            freq: '160MHz' 
        },
        i2s: { 
            text: 'I2S', 
            freq: '160MHz' 
        },
        qflash: { 
            text: 'QFLASH', 
            freq: '80MHz' 
        },
        touch_sensor: { 
            text: 'TOUCH SENSOR', 
            freq: '40MHz' 
        },
        pwm: { 
            text: 'PWM', 
            freq: '40MHz' 
        },
        adc: { 
            text: 'ADC', 
            freq: '1MHz' 
        }
    }
}; 