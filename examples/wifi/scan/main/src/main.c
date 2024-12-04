#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"
#include "wm_event.h"
#include "wm_wifi.h"

#define LOG_TAG "scan_example"
#include "wm_log.h"

#define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE

static void print_auth_mode(int authmode)
{
    switch (authmode) {
        case WM_WIFI_AUTH_OPEN:
            wm_log_info("Authmode \tWM_WIFI_AUTH_OPEN");
            break;
        case WM_WIFI_AUTH_WEP:
            wm_log_info("Authmode \tWM_WIFI_AUTH_WEP");
            break;
        case WM_WIFI_AUTH_WPA_PSK:
            wm_log_info("Authmode \tWM_WIFI_AUTH_WPA_PSK");
            break;
        case WM_WIFI_AUTH_WPA2_PSK:
            wm_log_info("Authmode \tWM_WIFI_AUTH_WPA2_PSK");
            break;
        case WM_WIFI_AUTH_WPA_WPA2_PSK:
            wm_log_info("Authmode \tWM_WIFI_AUTH_WPA_WPA2_PSK");
            break;
        case WM_WIFI_AUTH_WPA2_ENTERPRISE:
            wm_log_info("Authmode \tWM_WIFI_AUTH_WPA2_ENTERPRISE");
            break;
        case WM_WIFI_AUTH_WPA3_PSK:
            wm_log_info("Authmode \tWM_WIFI_AUTH_WPA3_PSK");
            break;
        case WM_WIFI_AUTH_WPA2_WPA3_PSK:
            wm_log_info("Authmode \tWM_WIFI_AUTH_WPA2_WPA3_PSK");
            break;
        default:
            wm_log_info("Authmode \tWM_WIFI_AUTH_UNKNOWN");
            break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
        case WM_WIFI_CIPHER_TYPE_NONE:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_NONE");
            break;
        case WM_WIFI_CIPHER_TYPE_WEP40:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_WEP40");
            break;
        case WM_WIFI_CIPHER_TYPE_WEP104:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_WEP104");
            break;
        case WM_WIFI_CIPHER_TYPE_TKIP:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_TKIP");
            break;
        case WM_WIFI_CIPHER_TYPE_CCMP:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_CCMP");
            break;
        case WM_WIFI_CIPHER_TYPE_TKIP_CCMP:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_TKIP_CCMP");
            break;
        case WM_WIFI_CIPHER_TYPE_AES_CMAC128:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_AES_CMAC128");
            break;
        case WM_WIFI_CIPHER_TYPE_SMS4:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_SMS4");
            break;
        case WM_WIFI_CIPHER_TYPE_GCMP:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_GCMP");
            break;
        case WM_WIFI_CIPHER_TYPE_GCMP256:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_GCMP256");
            break;
        default:
            wm_log_info("Pairwise Cipher \tWM_WIFI_CIPHER_TYPE_UNKNOWN");
            break;
    }

    switch (group_cipher) {
        case WM_WIFI_CIPHER_TYPE_NONE:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_NONE");
            break;
        case WM_WIFI_CIPHER_TYPE_WEP40:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_WEP40");
            break;
        case WM_WIFI_CIPHER_TYPE_WEP104:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_WEP104");
            break;
        case WM_WIFI_CIPHER_TYPE_TKIP:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_TKIP");
            break;
        case WM_WIFI_CIPHER_TYPE_CCMP:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_CCMP");
            break;
        case WM_WIFI_CIPHER_TYPE_TKIP_CCMP:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_TKIP_CCMP");
            break;
        case WM_WIFI_CIPHER_TYPE_SMS4:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_SMS4");
            break;
        case WM_WIFI_CIPHER_TYPE_GCMP:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_GCMP");
            break;
        case WM_WIFI_CIPHER_TYPE_GCMP256:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_GCMP256");
            break;
        default:
            wm_log_info("Group Cipher \tWM_WIFI_CIPHER_TYPE_UNKNOWN");
            break;
    }
}

/* Initialize Wi-Fi as sta and set scan method */
static void wifi_scan(void)
{
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wm_wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;

    int err = wm_wifi_init();

    if (!err)
        wm_wifi_scan_start(NULL, true);

    if (!err) {
        memset(ap_info, 0, sizeof(ap_info));
        err = wm_wifi_scan_get_ap_records(&number, ap_info);
    }
    if (!err)
        err = wm_wifi_scan_get_ap_num(&ap_count);

    if (err) {
        wm_log_error("get records failed.");
        return;
    }

    wm_log_info("Total APs scanned = %u", ap_count);

    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        wm_log_info("SSID \t\t%s", ap_info[i].ssid);
        wm_log_info("RSSI \t\t%d", ap_info[i].rssi);

        print_auth_mode(ap_info[i].authmode);

        if (ap_info[i].authmode != WM_WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }

        wm_log_info("Channel \t\t%d\n", ap_info[i].primary);
    }
}

int main(void)
{
    wifi_scan();

    return 0;
}
