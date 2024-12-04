#include <string.h>
#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_dt.h"
#include "wm_drv_flash.h"
#include "wm_ft_param.h"
#include "wm_partition_table.h"
#include "wm_drv_crc.h"

#define LOG_TAG "ft"
#include "wm_log.h"

#define FT_MAC_ADDR_LEN               (6)
#define FT_GAIN_LEN                   (84)
#define FT_PARAM_EXT_REVERSED_LEN     32

#define FT_MAGICNUM_ADDR_DEFAULT      (0) //write or read flash no need flash base address
#define FT_PARAM_RUNTIME_ADDR_DEFAULT (FT_MAGICNUM_ADDR_DEFAULT + 0x1000)
//#define FT_FREQERR_ADDR                     (g_ft_addr_0 + sizeof(wm_ft_param_t))
#define FT_FREQERR_LEN                (4)
//#define FT_CAL_FLAG_ADDR                    (g_ft_addr_0 + sizeof(wm_ft_param_t) + 4)
#define FT_CAL_FLAG_LEN               (4)

#define FT_PARAM_SIGNATURE_WORD       (0xA0FFFF9FUL)

enum {
    FT_CMD_WIFI_MAC = 0x01,
    FT_CMD_BT_MAC,
    FT_CMD_TX_DC,
    FT_CMD_RX_DC,
    FT_CMD_TX_IQ_GAIN,
    FT_CMD_RX_IQ_GAIN,
    FT_CMD_TX_IQ_PHASE,
    FT_CMD_RX_IQ_PHASE,
    FT_CMD_TX_GAIN,
    FT_CMD_TX_ADC_CAL,
    FT_CMD_FREQ_ERR,
    FT_CMD_RF_CAL_FLAG,
    FT_CMD_ALL,
};

typedef struct {
    uint32_t magic_no;
    uint32_t checksum;
    uint8_t wifi_mac_addr[FT_MAC_ADDR_LEN];
    uint16_t version_no;
    uint8_t bt_mac_addr[FT_MAC_ADDR_LEN];
    uint16_t ext_param_len;
    uint32_t tx_dcoffset;
    uint32_t rx_dcoffset;
    uint32_t tx_iq_gain;
    uint32_t rx_iq_gain;
    uint32_t tx_iq_phase;
    uint32_t rx_iq_phase;
    uint8_t tx_gain[FT_GAIN_LEN];
} wm_ft_param_t;

typedef struct {
    int ref_val;
    int real_val;
} wm_ft_temp_cal_t;

typedef struct {
    uint32_t rf_freq_err;
    uint32_t rf_cal_flag;
    wm_ft_adc_cal_t adc_cal_param;
    wm_ft_temp_cal_t temp_cal_param;
} wm_ft_param_ext_t;

typedef struct {
    wm_ft_param_t ft_param;
    uint32_t ext_checksum;
    wm_ft_param_ext_t ft_ext1;
    uint8_t _reversed[FT_PARAM_EXT_REVERSED_LEN];
} wm_ft_param_ver_t;

static wm_ft_param_ver_t g_ft_param_ver;

static wm_ft_param_t *g_ft_param = (wm_ft_param_t *)&g_ft_param_ver;

static uint32_t g_ft_addr_0 = FT_MAGICNUM_ADDR_DEFAULT;
static uint32_t g_ft_addr_1 = FT_PARAM_RUNTIME_ADDR_DEFAULT;

extern int wm_drv_iflash_wr_with_erase_private(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len);

static int ft_ext_valid(wm_ft_param_t *pft)
{
    if (pft->version_no > 0 && pft->version_no < 0xFFFF && pft->ext_param_len >= sizeof(wm_ft_param_ext_t) &&
        pft->ext_param_len <= (sizeof(wm_ft_param_ext_t) + FT_PARAM_EXT_REVERSED_LEN)) {
        return true;
    }

    return false;
}

static int ft_param_init(uint32_t ft_addr, wm_ft_param_t *pft)
{
    uint32_t crcvalue           = 0;
    wm_ft_param_ver_t *pft_ver1 = NULL;
    wm_device_t *flash_dev      = NULL;

    flash_dev = wm_dt_get_device_by_name("iflash");

    wm_drv_flash_read(flash_dev, ft_addr, (uint8_t *)pft, sizeof(wm_ft_param_ver_t));
    if (pft->magic_no == FT_PARAM_SIGNATURE_WORD) {
        crcvalue = wm_drv_crc32_reverse((uint8_t *)pft + 8, sizeof(wm_ft_param_t) - 8);
        if (pft->checksum != crcvalue) {
            return WM_ERR_FAILED;
        }

        do {
            if (ft_ext_valid(pft)) {
                pft_ver1 = (wm_ft_param_ver_t *)pft;
                crcvalue = wm_drv_crc32_reverse((uint8_t *)&pft_ver1->ft_ext1, pft->ext_param_len);
                if (pft_ver1->ext_checksum == crcvalue) {
                    return 1;
                } else {
                    return WM_ERR_FAILED;
                }
            }

            pft->version_no    = 0xFFFF;
            pft->ext_param_len = 0xFFFF;
        } while (0);

        return 2;
    }

    return WM_ERR_FAILED;
}

static int wm_ft_param_get(uint32_t opnum, void *data, uint32_t rdlen)
{
    int ret                = 0;
    wm_device_t *flash_dev = NULL;

    flash_dev = wm_dt_get_device_by_name("iflash");

    switch (opnum) {
        case FT_CMD_FREQ_ERR:
        {
            if (ft_ext_valid(g_ft_param)) {
                memcpy(data, (char *)&g_ft_param_ver.ft_ext1.rf_freq_err, FT_FREQERR_LEN);
            } else {
                ret = wm_drv_flash_read(flash_dev, g_ft_addr_0 + sizeof(wm_ft_param_t), data, FT_FREQERR_LEN);
                if (ret) {
                    return WM_ERR_FAILED;
                }
            }
            break;
        }
        case FT_CMD_RF_CAL_FLAG:
        {
            if (ft_ext_valid(g_ft_param)) {
                memcpy(data, (char *)&g_ft_param_ver.ft_ext1.rf_cal_flag, FT_CAL_FLAG_LEN);
            } else {
                ret = wm_drv_flash_read(flash_dev, g_ft_addr_0 + sizeof(wm_ft_param_t) + 4 /*FT_CAL_FLAG_ADDR*/, data,
                                        FT_CAL_FLAG_LEN);
                if (ret) {
                    return WM_ERR_FAILED;
                }
            }
            break;
        }
        case FT_CMD_TX_ADC_CAL:
        {
            if (ft_ext_valid(g_ft_param)) {
                memcpy(data, (uint8_t *)&g_ft_param_ver.ft_ext1.adc_cal_param, rdlen);
            } else {
                return WM_ERR_FAILED;
            }
            break;
        }
        case FT_CMD_WIFI_MAC: /*WIFI MAC*/
        {
            memcpy(data, g_ft_param->wifi_mac_addr, rdlen);
            break;
        }
        case FT_CMD_BT_MAC: /*BT MAC*/
        {
            memcpy(data, g_ft_param->bt_mac_addr, rdlen);
            break;
        }
        case FT_CMD_TX_DC: /*tx_dcoffset*/
        {
            *(uint32_t *)data = g_ft_param->tx_dcoffset;
            break;
        }
        case FT_CMD_RX_DC: /*rx_dcoffset*/
        {
            *(uint32_t *)data = g_ft_param->rx_dcoffset;
            break;
        }
        case FT_CMD_TX_IQ_GAIN:
        {
            *(uint32_t *)data = g_ft_param->tx_iq_gain;
            break;
        }
        case FT_CMD_RX_IQ_GAIN:
        {
            *(uint32_t *)data = g_ft_param->rx_iq_gain;
            break;
        }
        case FT_CMD_TX_IQ_PHASE:
        {
            *(uint32_t *)data = g_ft_param->tx_iq_phase;
            break;
        }
        case FT_CMD_RX_IQ_PHASE:
        {
            *(uint32_t *)data = g_ft_param->rx_iq_phase;
            break;
        }
        case FT_CMD_TX_GAIN: /*gain*/
        {
            if (rdlen < FT_GAIN_LEN) {
                memcpy(data, g_ft_param->tx_gain, rdlen);
            } else {
                memcpy(data, g_ft_param->tx_gain, FT_GAIN_LEN);
            }
            break;
        }
        default:
        {
            return WM_ERR_FAILED;
        }
    }

    return WM_ERR_SUCCESS;
}

static int wm_ft_param_set(uint32_t opnum, void *data, uint32_t len)
{
    uint32_t writelen      = 0;
    wm_ft_param_t *pft     = NULL;
    int ret                = 0;
    wm_device_t *flash_dev = NULL;

    if (!data || !len) {
        return WM_ERR_FAILED;
    }

    pft = wm_os_internal_malloc(sizeof(wm_ft_param_ver_t));
    if (pft == NULL) {
        return WM_ERR_FAILED;
    }

    flash_dev = wm_dt_get_device_by_name("iflash");

    memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
    ret = ft_param_init(g_ft_addr_1, pft);
    if (ret != WM_ERR_FAILED) {
        memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
        ret = ft_param_init(g_ft_addr_0, pft);
        if (WM_ERR_FAILED == ret || memcmp(pft, g_ft_param, sizeof(wm_ft_param_ver_t))) {
            wm_drv_iflash_wr_with_erase_private(flash_dev, g_ft_addr_0, (uint8_t *)g_ft_param, sizeof(wm_ft_param_ver_t));
            memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
            ret = ft_param_init(g_ft_addr_0, pft);
            if (WM_ERR_FAILED == ret || memcmp(pft, g_ft_param, sizeof(wm_ft_param_ver_t))) {
                memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
                wm_os_internal_free(pft);
                return WM_ERR_FAILED;
            }
        }
    }

    if (!ft_ext_valid(g_ft_param)) {
        g_ft_param->version_no    = 1;
        g_ft_param->ext_param_len = sizeof(wm_ft_param_ext_t);
        memset((char *)&g_ft_param_ver.ft_ext1, 0xFF, g_ft_param->ext_param_len);
    }

    switch (opnum) {
        case FT_CMD_FREQ_ERR:
        {
            memcpy((char *)&g_ft_param_ver.ft_ext1.rf_freq_err, (char *)data, FT_FREQERR_LEN);
            break;
        }
        case FT_CMD_RF_CAL_FLAG:
        {
            memcpy((char *)&g_ft_param_ver.ft_ext1.rf_cal_flag, (char *)data, FT_CAL_FLAG_LEN);
            break;
        }
        case FT_CMD_TX_ADC_CAL:
        {
            memcpy((uint8_t *)&g_ft_param_ver.ft_ext1.adc_cal_param, data, len);
            break;
        }
        case FT_CMD_WIFI_MAC: /*MAC*/
        {
            memcpy(g_ft_param->wifi_mac_addr, (uint8_t *)data, len);
            break;
        }
        case FT_CMD_BT_MAC: /*BT MAC*/
        {
            memcpy(g_ft_param->bt_mac_addr, (uint8_t *)data, len);
            break;
        }
        case FT_CMD_TX_DC: /*tx_dcoffset*/
        {
            g_ft_param->tx_dcoffset = *(uint32_t *)data;
            break;
        }
        case FT_CMD_RX_DC: /*rx_dcoffset*/
        {
            g_ft_param->rx_dcoffset = *(uint32_t *)data;
            break;
        }
        case FT_CMD_TX_IQ_GAIN:
        {
            g_ft_param->tx_iq_gain = *(uint32_t *)data;
            break;
        }
        case FT_CMD_RX_IQ_GAIN:
        {
            g_ft_param->rx_iq_gain = *(uint32_t *)data;
            break;
        }
        case FT_CMD_TX_IQ_PHASE:
        {
            g_ft_param->tx_iq_phase = *(uint32_t *)data;
            break;
        }
        case FT_CMD_RX_IQ_PHASE:
        {
            g_ft_param->rx_iq_phase = *(uint32_t *)data;
            break;
        }
        case FT_CMD_TX_GAIN: /*gain*/
        {
            if (len >= FT_GAIN_LEN) {
                writelen = FT_GAIN_LEN;
            } else {
                writelen = len;
            }
            memcpy(g_ft_param->tx_gain, data, writelen);
            break;
        }
        default:
        {
            wm_os_internal_free(pft);
            return WM_ERR_FAILED;
        }
    }

    /*len to write to flash*/
    writelen                    = sizeof(wm_ft_param_ver_t);
    g_ft_param_ver.ext_checksum = wm_drv_crc32_reverse((uint8_t *)&g_ft_param_ver.ft_ext1, g_ft_param->ext_param_len);

    g_ft_param->magic_no = FT_PARAM_SIGNATURE_WORD;
    g_ft_param->checksum = wm_drv_crc32_reverse((uint8_t *)g_ft_param + 8, sizeof(wm_ft_param_t) - 8);

    wm_drv_iflash_wr_with_erase_private(flash_dev, g_ft_addr_1, (uint8_t *)g_ft_param, writelen);
    memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
    ret = ft_param_init(g_ft_addr_1, pft);
    if (!ret || memcmp(pft, g_ft_param, sizeof(wm_ft_param_ver_t))) {
        memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
        wm_os_internal_free(pft);
        return WM_ERR_FAILED;
    }
    wm_drv_iflash_wr_with_erase_private(flash_dev, g_ft_addr_0, (uint8_t *)g_ft_param, writelen);
    memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
    ret = ft_param_init(g_ft_addr_0, pft);
    if (!ret || memcmp(pft, g_ft_param, sizeof(wm_ft_param_ver_t))) {
        memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
        wm_os_internal_free(pft);
        return WM_ERR_FAILED;
    }
    memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
    wm_os_internal_free(pft);

    return WM_ERR_SUCCESS;
}

int wm_ft_param_init(void)
{
    int ret            = 0;
    wm_ft_param_t *pft = NULL;
    wm_drv_flash_info_t fls_info;
    wm_device_t *flash_dev = NULL;
    wm_partition_item_t partition;

    if (g_ft_param->magic_no == FT_PARAM_SIGNATURE_WORD) {
        return WM_ERR_SUCCESS;
    }

    flash_dev = wm_dt_get_device_by_name("iflash");

    if (wm_drv_flash_get_device_info(flash_dev, &fls_info)) {
        wm_log_error("fls info error");
        return WM_ERR_FAILED;
    }

    if (wm_partition_table_find("ft", &partition)) {
        wm_log_error("ft_0 not found");
        return WM_ERR_FAILED;
    }
    g_ft_addr_0 = partition.offset;
    g_ft_addr_1 = partition.offset + 0x1000;

    wm_log_debug("ft_0 base %x, ft_1 base %x", g_ft_addr_0, g_ft_addr_1);

    memset(g_ft_param, 0xFF, sizeof(wm_ft_param_ver_t));

    pft = wm_os_internal_malloc(sizeof(wm_ft_param_ver_t));
    if (pft == NULL) {
        return WM_ERR_NO_MEM;
    }

    memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));

    ret = ft_param_init(g_ft_addr_1, pft);
    if (ret < 0) {
        wm_log_debug("ft init too");
        memset(pft, 0xFF, sizeof(wm_ft_param_ver_t));
        ret = ft_param_init(g_ft_addr_0, pft);
    }

    if (1 == ret) {
        memcpy((uint8_t *)g_ft_param, (uint8_t *)pft, sizeof(wm_ft_param_ver_t));
        ret = WM_ERR_SUCCESS;
        wm_log_debug("ft init 1");
    } else if (2 == ret) {
        memcpy((uint8_t *)g_ft_param, (uint8_t *)pft, sizeof(wm_ft_param_t));
        ret = WM_ERR_SUCCESS;
        wm_log_debug("ft init 2");
    } else {
        ret = WM_ERR_FAILED;
        wm_log_debug("ft init error");
    }

    wm_os_internal_free(pft);

    return ret;
}

int wm_ft_get_mac_addr(uint8_t *mac)
{
    return wm_ft_param_get(FT_CMD_WIFI_MAC, mac, 6);
}

int wm_ft_set_mac_addr(uint8_t *mac)
{
    return wm_ft_param_set(FT_CMD_WIFI_MAC, mac, 6);
}

int wm_ft_get_bt_mac_addr(uint8_t *mac)
{
    return wm_ft_param_get(FT_CMD_BT_MAC, mac, 6);
}

int wm_ft_set_bt_mac_addr(uint8_t *mac)
{
    return wm_ft_param_set(FT_CMD_BT_MAC, mac, 6);
}

int wm_ft_get_tx_lo(uint8_t *txlo)
{
    return wm_ft_param_get(FT_CMD_TX_DC, txlo, 4);
}

int wm_ft_set_tx_lo(uint8_t *txlo)
{
    return wm_ft_param_set(FT_CMD_TX_DC, txlo, 4);
}

int wm_ft_get_tx_iq_gain(uint8_t *tx_gain)
{
    return wm_ft_param_get(FT_CMD_TX_IQ_GAIN, tx_gain, 4);
}

int wm_ft_set_tx_iq_gain(uint8_t *tx_gain)
{
    return wm_ft_param_set(FT_CMD_TX_IQ_GAIN, tx_gain, 4);
}

int wm_ft_get_rx_iq_gain(uint8_t *rx_gain)
{
    return wm_ft_param_get(FT_CMD_RX_IQ_GAIN, rx_gain, 4);
}

int wm_ft_set_rx_iq_gain(uint8_t *rx_gain)
{
    return wm_ft_param_set(FT_CMD_RX_IQ_GAIN, rx_gain, 4);
}

int wm_ft_get_tx_iq_phase(uint8_t *tx_phase)
{
    return wm_ft_param_get(FT_CMD_TX_IQ_PHASE, tx_phase, 4);
}

int wm_ft_set_tx_iq_phase(uint8_t *tx_phase)
{
    return wm_ft_param_set(FT_CMD_TX_IQ_PHASE, tx_phase, 4);
}

int wm_ft_get_rx_iq_phase(uint8_t *rx_phase)
{
    return wm_ft_param_get(FT_CMD_RX_IQ_PHASE, rx_phase, 4);
}

int wm_ft_set_rx_iq_phase(uint8_t *rx_phase)
{
    return wm_ft_param_set(FT_CMD_RX_IQ_PHASE, rx_phase, 4);
}

int wm_ft_freq_err_op(uint8_t *freqerr, uint8_t flag)
{
    int ret   = 0;
    int value = 0;

    if (flag) {
        ret = wm_ft_param_set(FT_CMD_FREQ_ERR, freqerr, FT_FREQERR_LEN);
    } else {
        ret = wm_ft_param_get(FT_CMD_FREQ_ERR, freqerr, FT_FREQERR_LEN);
        if (!ret) {
            memcpy(&value, freqerr, FT_FREQERR_LEN);
            if (value > 200000) /*when freq offset is out of range (-200KHz, 200KHz),do not use it*/
            {
                value = 200000;
                memcpy((char *)freqerr, (char *)&value, FT_FREQERR_LEN);
            } else if (value < -200000) {
                value = -200000;
                memcpy((char *)freqerr, (char *)&value, FT_FREQERR_LEN);
            }
        }
    }
    if (ret == 0) {
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

int wm_ft_rf_cal_finish_op(uint8_t *calflag, uint8_t flag)
{
    int ret = 0;

    if (flag) {
        ret = wm_ft_param_set(FT_CMD_RF_CAL_FLAG, calflag, FT_CAL_FLAG_LEN);
    } else {
        ret = wm_ft_param_get(FT_CMD_RF_CAL_FLAG, calflag, FT_CAL_FLAG_LEN);
    }

    if (ret == 0) {
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

int wm_ft_get_tx_gain(uint8_t *tx_gain)
{
    return wm_ft_param_get(FT_CMD_TX_GAIN, tx_gain, WM_FT_TX_GAIN_LEN);
}

int wm_ft_set_tx_gain(uint8_t *tx_gain)
{
    return wm_ft_param_set(FT_CMD_TX_GAIN, tx_gain, WM_FT_TX_GAIN_LEN);
}

int wm_ft_get_adc_cal_param(wm_ft_adc_cal_t *adc_cal)
{
    return wm_ft_param_get(FT_CMD_TX_ADC_CAL, adc_cal, sizeof(wm_ft_adc_cal_t));
}

int wm_ft_set_adc_cal_param(wm_ft_adc_cal_t *adc_cal)
{
    return wm_ft_param_set(FT_CMD_TX_ADC_CAL, adc_cal, sizeof(wm_ft_adc_cal_t));
}
