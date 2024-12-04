#ifndef __WM_FT_PARAM_H__
#define __WM_FT_PARAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WM_FT_TX_GAIN_LEN (28 * 3)

typedef struct {
    uint16_t ref_val;
    uint16_t real_val;
} wm_ft_adc_cal_unit_t;

typedef struct {
    uint32_t valid_cnt;
    wm_ft_adc_cal_unit_t units[6];
    float a;
    float b;
} wm_ft_adc_cal_t;

int wm_ft_get_adc_cal_param(wm_ft_adc_cal_t *adc_cal);
int wm_ft_get_rx_iq_gain(uint8_t *rx_gain);
int wm_ft_get_rx_iq_phase(uint8_t *rx_phase);
int wm_ft_get_tx_gain(uint8_t *tx_gain);
int wm_ft_get_tx_iq_gain(uint8_t *tx_gain);
int wm_ft_get_tx_iq_phase(uint8_t *tx_phase);
int wm_ft_get_tx_lo(uint8_t *txlo);

int wm_ft_set_adc_cal_param(wm_ft_adc_cal_t *adc_cal);
int wm_ft_set_rx_iq_gain(uint8_t *rx_gain);
int wm_ft_set_rx_iq_phase(uint8_t *rx_phase);
int wm_ft_set_tx_gain(uint8_t *tx_gain);
int wm_ft_set_tx_iq_gain(uint8_t *tx_gain);
int wm_ft_set_tx_iq_phase(uint8_t *tx_phase);
int wm_ft_set_tx_lo(uint8_t *txlo);

int wm_ft_freq_err_op(uint8_t *freqerr, uint8_t flag);
int wm_ft_rf_cal_finish_op(uint8_t *calflag, uint8_t flag);

int wm_ft_get_mac_addr(uint8_t *mac);
int wm_ft_set_mac_addr(uint8_t *mac);

int wm_ft_get_bt_mac_addr(uint8_t *mac);
int wm_ft_set_bt_mac_addr(uint8_t *mac);

int wm_ft_param_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __WM_FT_PARAM_H__ */
