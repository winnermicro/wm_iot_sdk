
#include "wmsdk_config.h"
#include "wm_heap.h"
#include "wm_soc_cfgs.h"
#include "wm_drv_dma.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_pmu.h"
#include "wm_debug.h"
#include "wm_component.h"

#ifdef CONFIG_COMPONENT_LOG_ENABLED
#define LOG_TAG "soc"
#include "wm_log.h"
#endif

extern void wm_log_port_init(void);
extern void cxx_system_init(void);

void wm_soc_basic_init(wm_device_t *rcc_dev)
{
    uint32_t value;
    wm_drv_clock_ctx_t *drv_ctx = NULL;
    wm_hal_clock_dev_t *hal_dev = NULL;
    wm_device_t *pmu_dev        = NULL;
    wm_pmu_wakeup_pre_status_t status;

    /* init PMU */
    pmu_dev = wm_drv_pmu_init("pmu");

    if (pmu_dev) {
        /* set standby reason here if pmu irq standby flag is 1,as pmu irq register will be cleared soon */
        if (wm_drv_pmu_get_status_before_wakeup(pmu_dev, &status) == WM_ERR_SUCCESS &&
            status == WM_PMU_WAKEUP_PRE_STATUS_STANDBY) {
            wm_set_reboot_reason(WM_REBOOT_REASON_STANDBY);
        }
    }

    /* init clock/irq/dma */
    if (rcc_dev) {
        drv_ctx = (wm_drv_clock_ctx_t *)rcc_dev->drv;
        hal_dev = &(drv_ctx->hal_dev);
        if (drv_ctx == NULL || hal_dev == NULL) {
            return;
        }

        /*Close those not initialized clk except touchsensor/trng, uart0,uart1,sdadc,gpio,rfcfg*/
        wm_hal_clock_disable(hal_dev, WM_RCC_I2C_GATE_EN | WM_RCC_UART0_GATE_EN | WM_RCC_UART1_GATE_EN | WM_RCC_UART2_GATE_EN |
                                          WM_RCC_UART3_GATE_EN | WM_RCC_UART4_GATE_EN | WM_RCC_UART5_GATE_EN |
                                          WM_RCC_LS_SPI_GATE_EN | WM_RCC_DMA_GATE_EN | WM_RCC_TIMER_GATE_EN |
                                          WM_RCC_SD_ADC_GATE_EN | WM_RCC_PWM_GATE_EN | WM_RCC_LCD_GATE_EN | WM_RCC_I2S_GATE_EN |
                                          WM_RCC_RSA_GATE_EN | WM_RCC_GPSEC_GATE_EN | WM_RCC_SDIOM_GATE_EN |
                                          WM_RCC_QSRAM_GATE_EN | WM_RCC_TOUCH_GATE_EN | WM_RCC_BT_GATE_EN);
    }

    value = WM_REG32_READ(0x40000E04);
    value &= ~(WM_BIT(3) | WM_BIT(4) | WM_BIT(5));
    WM_REG32_WRITE(0x40000E04, value);
    WM_REG32_WRITE(0x40000E08, 0xF);

    /*clear bt mask*/
    WM_REG32_WRITE(0x40002A10, 0xFFFFFFFF);
    wm_hal_irq_clear_pending(WM_IRQ_BT);

    // fix it, need delete when dt system init
    wm_drv_dma_init("dma");
    wm_drv_gpio_init("gpio");
}

void soc_init(void)
{
    /* heap init, not include psram */
    wm_heap_init();

    wm_components_init0();

    /* uart init for early print */
    wm_log_port_init();

    /* c++ variable init */
    cxx_system_init();

#if CONFIG_LOG_DEFAULT_LEVEL
#ifdef CONFIG_COMPONENT_LOG_ENABLED
    wm_log_init();
#endif /* CONFIG_COMPONENT_LOG_ENABLED */
#endif /* CONFIG_LOG_DEFAULT_LEVEL */
}
