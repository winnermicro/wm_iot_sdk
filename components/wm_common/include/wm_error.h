#ifndef __WM_ERROR_H__
#define __WM_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define WM_ERR_SUCCESS          0
#define WM_ERR_FAILED           -1
#define WM_ERR_NO_MEM           -2
#define WM_ERR_INVALID_PARAM    -3
#define WM_ERR_BUSY             -4
#define WM_ERR_TIMEOUT          -5
#define WM_ERR_NO_SUPPORT       -6
#define WM_ERR_NO_INITED        -7
#define WM_ERR_NOT_FOUND        -8
#define WM_ERR_NOT_ALLOWED      -9
#define WM_ERR_ALREADY_INITED   -10
#define WM_ERR_EOS              -11 /**< end of stream. */

#define WM_ERR_MQTT_CLIENT_BASE -5000
#define WM_ERR_PWM_BASE         -6000
#define WM_ERR_OTA_BASE         -10000

#ifdef __cplusplus
}
#endif

#endif /* __WM_ERROR_H__ */
