#include <stdint.h>
#include <csi_config.h>
#include <csi_core.h>
#include "wm_attr.h"
#include "wm_irq.h"
#include "wm_rcc_reg_struct.h"
#include "wm_reg_op.h"
#include "wm_regs_base.h"
#include "wm_soc_cfgs.h"
#include "wm_soc.h"
#include "wm_ll_rcc.h"

#define WM_CLK_DIV_VAL_MASK (0x0F)
#define WM_CLK_MS_UNIT      (1000)
#define WM_CLK_US_UNIT      (1)

extern int32_t g_top_irqstack;

extern uint32_t csi_coret_get_load(void);
extern uint32_t csi_coret_get_value(void);

uint32_t irq_vectors[CONFIG_IRQ_VECTOR_SIZE / sizeof(uint32_t)] ATTRIBUTE_IRQRAM;

static void _delay(uint32_t n_us)
{
    uint32_t load  = csi_coret_get_load();
    uint32_t start = csi_coret_get_value();
    uint32_t cur = start, clk_div_val, cnt, ticks = 0;

    wm_rcc_reg_t *rcc_dev = (wm_rcc_reg_t *)WM_CLK_BASE_ADDR;

    clk_div_val = wm_ll_rcc_get_cpu_clock_div(rcc_dev);

    /* cpu is not ready */
    if ((clk_div_val == 0) || (load == 0)) {
        return;
    }

    cnt = (WM_PLL_CLOCK * n_us) / clk_div_val;
    while (1) {
        start = cur;
        cur = csi_coret_get_value();

        if (start > cur) {
            ticks += (start - cur);
        } else {
            ticks += (load - cur + start);
        }

        if (ticks >= cnt) {
            return;
        }
    }
}

void mdelay(uint32_t ms)
{
    if (ms == 0) {
        return;
    }

    while (ms--) {
        _delay(WM_CLK_MS_UNIT);
    }
}

void udelay(uint32_t us)
{
    if (us == 0) {
        return;
    }

    _delay(us);
}

void cpu_init(void)
{
    __set_VBR((uint32_t) & (irq_vectors));

#if defined(CONFIG_SEPARATE_IRQ_SP) && !defined(CONFIG_KERNEL_NONE)
    /* 801 not supported */
    __set_Int_SP((uint32_t)&g_top_irqstack);
    __set_CHR(__get_CHR() | CHR_ISE_Msk);
    VIC->TSPR = 0xFF;
#endif

    __set_CHR(__get_CHR() | CHR_IAE_Msk);

    /* Clear active and pending IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;

#ifdef CONFIG_KERNEL_NONE
    __enable_excp_irq();
#endif

    //csi_coret_config(g_system_clock / CONFIG_SYSTICK_HZ, SYS_TICK_IRQn);    //10ms
    //#ifndef CONFIG_KERNEL_NONE
    csi_vic_enable_irq(WM_IRQ_SYS_TICK);
    //#endif

    NVIC_DisableIRQ(WM_IRQ_UART0);
    NVIC_ClearPendingIRQ(WM_IRQ_UART0);
}
