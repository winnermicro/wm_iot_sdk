#include <string.h>
#include "wm_ll_rcc.h"
#include "wm_hal_rcc.h"
#include "wm_drv_rcc.h"
#include "wm_soc_cfgs.h"
#include "wm_error.h"
#include "wm_dt_hw.h"
#include "wm_drv_ops_rcc.c"

int wm_drv_clock_enable(wm_device_t *dev, uint32_t module_type)
{
    wm_drv_clock_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || !module_type) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = (wm_drv_clock_ops_t *)dev->ops;
    if (ops && ops->clock_enable) {
        ret = ops->clock_enable(dev, module_type);
    }

    return ret;
}

int wm_drv_clock_disable(wm_device_t *dev, uint32_t module_type)
{
    wm_drv_clock_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || !module_type) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = (wm_drv_clock_ops_t *)dev->ops;
    if (ops && ops->clock_disable) {
        ret = ops->clock_disable(dev, module_type);
    }

    return ret;
}

int wm_drv_clock_reset(wm_device_t *dev, uint32_t module_type)
{
    wm_drv_clock_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || !module_type) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = (wm_drv_clock_ops_t *)dev->ops;
    if (ops && ops->clock_reset) {
        ret = ops->clock_reset(dev, module_type);
    }

    return ret;
}

int wm_drv_rcc_config_clock(wm_device_t *dev, wm_rcc_type_t module_type, uint16_t MHz)
{
    wm_drv_clock_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || !MHz) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = (wm_drv_clock_ops_t *)dev->ops;
    if (ops && ops->config_clock) {
        ret = ops->config_clock(dev, module_type, MHz);
    }

    return ret;
}

int wm_drv_rcc_get_config_clock(wm_device_t *dev, wm_rcc_type_t module_type)
{
    wm_drv_clock_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = (wm_drv_clock_ops_t *)dev->ops;
    if (ops && ops->get_config_clock) {
        ret = ops->get_config_clock(dev, module_type);
    }

    return ret;
}

wm_device_t *wm_drv_clock_init(char *dev_name)
{
    wm_device_t *clock_dev  = NULL;
    wm_drv_clock_ops_t *ops = NULL;
    int ret                 = WM_ERR_SUCCESS;

    clock_dev = wm_dt_get_device_by_name(dev_name);
    if (clock_dev && clock_dev->ops) {
        ops = clock_dev->ops;
        if (ops->init) {
            ret = ops->init(clock_dev);
        }
    }

    return ret == WM_ERR_SUCCESS ? clock_dev : NULL;
}

int wm_drv_clock_deinit(wm_device_t *dev)
{
    wm_drv_clock_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = (wm_drv_clock_ops_t *)dev->ops;
    if (ops && ops->deinit) {
        ret = ops->deinit(dev);
    }

    dev = NULL;

    return ret;
}

int wm_drv_rcc_set_i2s_clock(wm_device_t *dev, bool extal_en, bool mclk_en, uint32_t mclk_div, uint32_t bclk_div)
{
    wm_drv_clock_ops_t *ops = NULL;
    wm_drv_rcc_ioctl_args_t arg;
    int ret = WM_ERR_INVALID_PARAM;

    if (dev == NULL || !mclk_div || !bclk_div) {
        return WM_ERR_INVALID_PARAM;
    }

    arg.cmd            = WM_IOCTL_RCC_CMD_SET_I2S;
    arg.u.i2s.extal_en = extal_en;
    arg.u.i2s.mclk_en  = mclk_en;
    arg.u.i2s.mclk_div = mclk_div;
    arg.u.i2s.bclk_div = bclk_div;

    ops = (wm_drv_clock_ops_t *)dev->ops;
    if (ops && ops->ioctl) {
        ret = ops->ioctl(dev, &arg);
    }

    return ret;
}
