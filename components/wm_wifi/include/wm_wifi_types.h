#ifndef __WM_WIFI_TYPES_H__
#define __WM_WIFI_TYPES_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WM_WIFI_IF_STA = 0, /**< station interface */
    WM_WIFI_IF_AP  = 1, /**< soft-AP interface */

    WM_WIFI_IF_MAX = 2
} wm_wifi_interface_t;

typedef enum {
    WM_WIFI_SECOND_CHAN_NONE = 0, /**< the channel width is HT20 */
    WM_WIFI_SECOND_CHAN_ABOVE,    /**< the channel width is HT40 and the secondary channel is above the primary channel */
    WM_WIFI_SECOND_CHAN_BELOW,    /**< the channel width is HT40 and the secondary channel is below the primary channel */
} wm_wifi_second_chan_t;

typedef enum {
    WM_WIFI_AUTH_OPEN = 0,        /**< authenticate mode : open */
    WM_WIFI_AUTH_WEP,             /**< authenticate mode : WEP */
    WM_WIFI_AUTH_WPA_PSK,         /**< authenticate mode : WPA_PSK */
    WM_WIFI_AUTH_WPA2_PSK,        /**< authenticate mode : WPA2_PSK */
    WM_WIFI_AUTH_WPA_WPA2_PSK,    /**< authenticate mode : WPA_WPA2_PSK */
    WM_WIFI_AUTH_WPA2_ENTERPRISE, /**< authenticate mode : WPA2_ENTERPRISE */
    WM_WIFI_AUTH_WPA3_PSK,        /**< authenticate mode : WPA3_PSK */
    WM_WIFI_AUTH_WPA2_WPA3_PSK,   /**< authenticate mode : WPA2_WPA3_PSK */
    WM_WIFI_AUTH_WAPI_PSK,        /**< authenticate mode : WAPI_PSK */
    WM_WIFI_AUTH_MAX
} wm_wifi_auth_mode_t;

typedef enum {
    WM_WIFI_CIPHER_TYPE_NONE = 0,    /**< the cipher type is none */
    WM_WIFI_CIPHER_TYPE_WEP40,       /**< the cipher type is WEP40 */
    WM_WIFI_CIPHER_TYPE_WEP104,      /**< the cipher type is WEP104 */
    WM_WIFI_CIPHER_TYPE_TKIP,        /**< the cipher type is TKIP */
    WM_WIFI_CIPHER_TYPE_CCMP,        /**< the cipher type is CCMP */
    WM_WIFI_CIPHER_TYPE_TKIP_CCMP,   /**< the cipher type is TKIP and CCMP */
    WM_WIFI_CIPHER_TYPE_AES_CMAC128, /**< the cipher type is AES-CMAC-128 */
    WM_WIFI_CIPHER_TYPE_SMS4,        /**< the cipher type is SMS4 */
    WM_WIFI_CIPHER_TYPE_GCMP,        /**< the cipher type is GCMP */
    WM_WIFI_CIPHER_TYPE_GCMP256,     /**< the cipher type is GCMP-256 */
    WM_WIFI_CIPHER_TYPE_AES_GMAC128, /**< the cipher type is AES-GMAC-128 */
    WM_WIFI_CIPHER_TYPE_AES_GMAC256, /**< the cipher type is AES-GMAC-256 */
    WM_WIFI_CIPHER_TYPE_UNKNOWN,     /**< the cipher type is unknown */
} wm_wifi_cipher_type_t;

typedef enum {
    WM_WIFI_COUNTRY_POLICY_AUTO,   /**< Country policy is auto, use the country info of AP to which the station is connected */
    WM_WIFI_COUNTRY_POLICY_MANUAL, /**< Country policy is manual, always use the configured country info */
} wm_wifi_country_policy_t;

/** @brief Structure describing WiFi country-based regional restrictions. */
typedef struct {
    char cc[3];                      /**< country code string */
    uint8_t schan;                   /**< start channel */
    uint8_t nchan;                   /**< total channel number */
    int8_t max_tx_power;             /**< This field is used for getting WiFi maximum transmitting power. */
    wm_wifi_country_policy_t policy; /**< country policy */
} wm_wifi_country_t;

/** @brief Description of a WiFi AP */
typedef struct {
    uint8_t bssid[6];                      /**< MAC address of AP */
    uint8_t ssid[33];                      /**< SSID of AP */
    uint8_t primary;                       /**< channel of AP */
    wm_wifi_second_chan_t second;          /**< secondary channel of AP */
    int8_t rssi;                           /**< signal strength of AP */
    wm_wifi_auth_mode_t authmode;          /**< authmode of AP */
    wm_wifi_cipher_type_t pairwise_cipher; /**< pairwise cipher of AP */
    wm_wifi_cipher_type_t group_cipher;    /**< group cipher of AP */
    uint32_t phy_11n  : 1;                 /**< bit: 0 flag to identify if 11n mode is enabled or not */
    uint32_t phy_11ax : 1;                 /**< bit: 1 flag to identify if 11ax mode is enabled or not */
    uint32_t wps      : 1;                 /**< bit: 2 flag to identify if WPS is supported or not */
    uint32_t reserved : 29;                /**< bit: 3..31 reserved */
    wm_wifi_country_t country;             /**< country information of AP */
} wm_wifi_ap_record_t;

typedef enum {
    WM_WIFI_FAST_SCAN = 0,    /**< Do fast scan, scan will end after find SSID match AP */
    WM_WIFI_ALL_CHANNEL_SCAN, /**< All channel scan, scan will end after scan all the channel */
} wm_wifi_scan_method_t;

typedef enum {
    WM_WIFI_CONNECT_AP_BY_SIGNAL = 0, /**< Sort match AP in scan list by RSSI */
    WM_WIFI_CONNECT_AP_BY_SECURITY,   /**< Sort match AP in scan list by security mode */
} wm_wifi_sort_method_t;

/** @brief Structure describing parameters for a WiFi fast scan */
typedef struct {
    int8_t rssi;                  /**< The minimum rssi to accept in the fast scan mode */
    wm_wifi_auth_mode_t authmode; /**< The weakest authmode to accept in the fast scan mode */
} wm_wifi_scan_threshold_t;

typedef enum {
    WM_WIFI_PS_NONE,      /**< No power save */
    WM_WIFI_PS_MIN_MODEM, /**< Minimum modem power saving. In this mode, station wakes up to receive beacon every DTIM period */
    WM_WIFI_PS_MAX_MODEM, /**< Maximum modem power saving. In this mode, interval to receive beacons is determined by the listen_interval parameter in wm_wifi_sta_config_t */
} wm_wifi_ps_type_t;

typedef enum {
    WM_WIFI_PROTOCOL_11B  = 1,
    WM_WIFI_PROTOCOL_11G  = 2,
    WM_WIFI_PROTOCOL_11N  = 4,
    WM_WIFI_PROTOCOL_11AX = 8,
} wm_wifi_protocol_type_t;

typedef enum {
    WM_WIFI_BW_HT20 = 1, /* Bandwidth is HT20 */
    WM_WIFI_BW_HT40,     /* Bandwidth is HT40 */
} wm_wifi_bandwidth_t;

/** Configuration structure for Protected Management Frame */
typedef struct {
    bool capable; /**< Deprecated variable. Device will always connect in PMF mode if other device also advertizes PMF capability. */
    bool required; /**< Advertizes that Protected Management Frame is required. Device will not associate to non-PMF capable devices. */
} wm_wifi_pmf_config_t;

/** Configuration for SAE PWE derivation */
typedef enum {
    WM_SAE_PWE_UNSPECIFIED,
    WM_SAE_PWE_HUNT_AND_PECK,
    WM_SAE_PWE_HASH_TO_ELEMENT,
    WM_SAE_PWE_BOTH,
} wm_wifi_sae_pwe_method_t;

typedef enum {
    WM_WIFI_SCAN_TYPE_ACTIVE = 0, /**< active scan */
    WM_WIFI_SCAN_TYPE_PASSIVE,    /**< passive scan */
} wm_wifi_scan_type_t;

/** @brief Range of active scan times per channel */
typedef struct {
    uint32_t min; /**< minimum active scan time per channel, units: millisecond */
    uint32_t max; /**< maximum active scan time per channel, units: millisecond, values above 1500ms may
                        cause station to disconnect from AP and are not recommended.  */
} wm_wifi_active_scan_time_t;

/** @brief Aggregate of active & passive scan time per channel */
typedef struct {
    wm_wifi_active_scan_time_t active; /**< active scan time per channel, units: millisecond. */
    uint32_t passive;                  /**< passive scan time per channel, units: millisecond, values above 1500ms may
                                            cause station to disconnect from AP and are not recommended. */
} wm_wifi_scan_time_t;

/** @brief Parameters for an SSID scan. */
typedef struct {
    uint8_t *ssid;                 /**< SSID of AP */
    uint8_t *bssid;                /**< MAC address of AP */
    uint8_t channel;               /**< channel, scan the specific channel */
    bool show_hidden;              /**< enable to scan AP whose SSID is hidden */
    wm_wifi_scan_type_t scan_type; /**< scan type, active or passive */
    wm_wifi_scan_time_t scan_time; /**< scan time per channel */
    uint8_t home_chan_dwell_time;  /**< time spent at home channel between scanning consecutive channels.*/
} wm_wifi_scan_config_t;

/** @brief Soft-AP configuration settings */
typedef struct {
    uint8_t ssid[32];                      /**< SSID of soft-AP. If ssid_len field is 0, this must be a Null terminated string.
                                       Otherwise, length is set according to ssid_len. */
    uint8_t password[64];                  /**< Password of soft-AP. */
    uint8_t ssid_len;                      /**< Optional length of SSID field. */
    uint8_t channel;                       /**< Channel of soft-AP */
    wm_wifi_auth_mode_t authmode;          /**< Auth mode of soft-AP. Do not support AUTH_WEP in soft-AP mode */
    uint8_t ssid_hidden;                   /**< Broadcast SSID or not, default 0, broadcast the SSID */
    uint8_t max_connection;                /**< Max number of stations allowed to connect in, default 4, max 8 */
    uint16_t beacon_interval;              /**< Beacon interval which should be multiples of 100.
                                       Unit: TU(time unit, 1 TU = 1024 us). Range: 100 ~ 60000. Default value: 100 */
    wm_wifi_cipher_type_t pairwise_cipher; /**< pairwise cipher of SoftAP, group cipher will be derived using this.
                                                cipher values are valid starting from WM_WIFI_CIPHER_TYPE_TKIP,
                                                enum values before that will be considered as invalid and default
                                                cipher suites(TKIP+CCMP) will be used. Valid cipher suites in softAP
                                                mode are WM_WIFI_CIPHER_TYPE_TKIP, WM_WIFI_CIPHER_TYPE_CCMP and
                                                WM_WIFI_CIPHER_TYPE_TKIP_CCMP. */
    wm_wifi_pmf_config_t pmf_cfg;          /**< Configuration for Protected Management Frame */
    wm_wifi_sae_pwe_method_t sae_pwe_h2e;  /**< Configuration for SAE PWE derivation method */
} wm_wifi_ap_config_t;

/** @brief STA configuration settings */
typedef struct {
    uint8_t ssid[32];                  /**< SSID of target AP. */
    uint8_t password[64];              /**< Password of target AP. */
    wm_wifi_scan_method_t scan_method; /**< do all channel scan or fast scan */
    bool bssid_set;   /**< whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0;
                           and it needs to be 1 only when users need to check the MAC address of the AP.*/
    uint8_t bssid[6]; /**< MAC address of target AP*/
    uint8_t channel; /**< channel of target AP. Set to 1~13 to scan starting from the specified channel before connecting to AP.
                           If the channel of AP is unknown, set it to 0.*/
    uint16_t listen_interval;             /**< Listen interval for station to receive beacon when WM_WIFI_PS_MAX_MODEM is set.
                                   Units: AP beacon intervals. Defaults to 10 if set to 0. */
    wm_wifi_sort_method_t sort_method;    /**< sort the connect AP in the list by rssi or security mode */
    wm_wifi_scan_threshold_t threshold;   /**< When sort_method is set, only APs which have an auth mode that is more
                                              secure than the selected auth mode and a signal stronger than
                                              the minimum RSSI will be used. */
    wm_wifi_pmf_config_t pmf_cfg;         /**< Configuration for Protected Management Frame.
                                              Will be advertized in RSN Capabilities in RSN IE. */
    wm_wifi_sae_pwe_method_t sae_pwe_h2e; /**< Configuration for SAE PWE derivation method */
} wm_wifi_sta_config_t;

/** @brief Configuration data for AP or STA.
 *
 * The usage of this union (for ap or sta configuration) is determined by the accompanying
 * interface argument passed to wm_wifi_set_config() or wm_wifi_get_config()
 *
 */
typedef union {
    wm_wifi_ap_config_t ap;   /**< configuration of AP */
    wm_wifi_sta_config_t sta; /**< configuration of STA */
} wm_wifi_config_t;

#define WM_WIFI_MAX_CONN_NUM (8) /**< max number of stations which can connect to soft-AP */

/** @brief Description of STA associated with AP */
typedef struct {
    uint8_t mac[6];         /**< mac address */
    uint16_t aid;           /**< the aid */
    uint32_t phy_11n  : 1;  /**< bit: 0 flag to identify if 11n mode is enabled or not */
    uint32_t phy_11ax : 1;  /**< bit: 1 flag to identify if 11ax mode is enabled or not */
    uint32_t reserved : 30; /**< bit: 2..31 reserved */
} wm_wifi_sta_info_t;

/** @brief List of stations associated with the Soft-AP */
typedef struct {
    wm_wifi_sta_info_t sta[WM_WIFI_MAX_CONN_NUM]; /**< station list */
    int num;                                      /**< number of stations in the list (other entries are invalid) */
} wm_wifi_sta_list_t;

typedef enum {
    WM_WIFI_STORAGE_FLASH, /**< all configuration will store in both memory and flash */
    WM_WIFI_STORAGE_RAM,   /**< all configuration will only store in the memory */
} wm_wifi_storage_t;

/**
  * @brief WiFi PHY rate encodings
  *
  */
typedef enum {
    WM_WIFI_PHY_RATE_1M_L = 0, /**< 1 Mbps with long preamble */
    WM_WIFI_PHY_RATE_2M_L,     /**< 2 Mbps with long preamble */
    WM_WIFI_PHY_RATE_2M_S,     /**< 2 Mbps with short preamble */
    WM_WIFI_PHY_RATE_5M_L,     /**< 5.5 Mbps with long preamble */
    WM_WIFI_PHY_RATE_5M_S,     /**< 5.5 Mbps with short preamble */
    WM_WIFI_PHY_RATE_11M_L,    /**< 11 Mbps with long preamble */
    WM_WIFI_PHY_RATE_11M_S,    /**< 11 Mbps with short preamble */
    WM_WIFI_PHY_RATE_6M,       /**< 6 Mbps */
    WM_WIFI_PHY_RATE_9M,       /**< 9 Mbps */
    WM_WIFI_PHY_RATE_12M,      /**< 12 Mbps */
    WM_WIFI_PHY_RATE_18M,      /**< 18 Mbps */
    WM_WIFI_PHY_RATE_24M,      /**< 24 Mbps */
    WM_WIFI_PHY_RATE_36M,      /**< 36 Mbps */
    WM_WIFI_PHY_RATE_48M,      /**< 48 Mbps */
    WM_WIFI_PHY_RATE_54M,      /**< 54 Mbps */
    WM_WIFI_PHY_RATE_MCS0_LGI, /**< MCS0 with long GI, 6.5 Mbps for 20MHz, 13.5 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS1_LGI, /**< MCS1 with long GI, 13 Mbps for 20MHz, 27 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS2_LGI, /**< MCS2 with long GI, 19.5 Mbps for 20MHz, 40.5 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS3_LGI, /**< MCS3 with long GI, 26 Mbps for 20MHz, 54 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS4_LGI, /**< MCS4 with long GI, 39 Mbps for 20MHz, 81 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS5_LGI, /**< MCS5 with long GI, 52 Mbps for 20MHz, 108 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS6_LGI, /**< MCS6 with long GI, 58.5 Mbps for 20MHz, 121.5 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS7_LGI, /**< MCS7 with long GI, 65 Mbps for 20MHz, 135 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS0_SGI, /**< MCS0 with short GI, 7.2 Mbps for 20MHz, 15 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS1_SGI, /**< MCS1 with short GI, 14.4 Mbps for 20MHz, 30 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS2_SGI, /**< MCS2 with short GI, 21.7 Mbps for 20MHz, 45 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS3_SGI, /**< MCS3 with short GI, 28.9 Mbps for 20MHz, 60 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS4_SGI, /**< MCS4 with short GI, 43.3 Mbps for 20MHz, 90 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS5_SGI, /**< MCS5 with short GI, 57.8 Mbps for 20MHz, 120 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS6_SGI, /**< MCS6 with short GI, 65 Mbps for 20MHz, 135 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MCS7_SGI, /**< MCS7 with short GI, 72.2 Mbps for 20MHz, 150 Mbps for 40MHz */
    WM_WIFI_PHY_RATE_MAX,
} wm_wifi_phy_rate_t;

/** @brief Received packet radio metadata header, this is the common header at the beginning of all promiscuous mode RX callback buffers */
typedef struct {
    signed rssi                : 8;  /**< Received Signal Strength Indicator(RSSI) of packet. unit: dBm */
    unsigned rate              : 5;  /**< PHY rate encoding of the packet. Only valid for non HT(11bg) packet */
    unsigned sig_mode          : 2;  /**< 0: non HT(11bg) packet; 1: HT(11n) packet; 3: HE(11ax) packet */
    unsigned                   : 1;  /**< reserved */
    unsigned mcs               : 7;  /**< Modulation Coding Scheme. If is HT(11n) packet, shows the modulation,
                                range from 0 to 7(MSC0 ~ MCS7) */
    unsigned cwb               : 1;  /**< Channel Bandwidth of the packet. 0: 20MHz; 1: 40MHz */
    unsigned smoothing         : 1;  /**< reserved */
    unsigned not_sounding      : 1;  /**< reserved */
    unsigned aggregation       : 1;  /**< Aggregation. 0: MPDU packet; 1: AMPDU packet */
    unsigned stbc              : 2;  /**< Space Time Block Code(STBC). 0: non STBC packet; 1: STBC packet */
    unsigned fec_coding        : 1;  /**< Flag is set for 11n packets which are LDPC */
    unsigned sgi               : 1;  /**< Short Guide Interval(SGI). 0: Long GI; 1: Short GI */
    unsigned                   : 1;  /**< reserved */
    signed noise_floor         : 8;  /**< noise floor of Radio Frequency Module(RF). unit: 0.25dBm*/
    unsigned ampdu_cnt         : 8;  /**< ampdu cnt */
    unsigned channel           : 4;  /**< primary channel on which this packet is received */
    unsigned secondary_channel : 4;  /**< secondary channel on which this packet is received. 0: none; 1: above; 2: below */
    unsigned rx_state          : 8;  /**< state of the packet. 0: no error; others: error numbers which are not public */
    unsigned timestamp         : 32; /**< timestamp. The local time when this packet is received.
                                         It is precise only if modem sleep or light sleep is not enabled. unit: millisecond */
    unsigned sig_len           : 12; /**< length of packet excluding Frame Check Sequence(FCS) */
    unsigned                   : 20; /**< reserved */
} wm_wifi_pkt_rx_ctrl_t;

/** @brief Payload passed to 'buf' parameter of promiscuous mode RX callback.
 */
typedef struct {
    wm_wifi_pkt_rx_ctrl_t rx_ctrl; /**< metadata header */
    uint8_t *payload;              /**< Data or management payload. Length of payload is described by rx_ctrl.sig_len.
                           Type of content determined by packet type argument of callback. */
} wm_wifi_promiscuous_pkt_t;

/**
  * @brief Promiscuous frame type
  *
  * Passed to promiscuous mode RX callback to indicate the type of parameter in the buffer.
  *
  */
typedef enum {
    WM_WIFI_PKT_MGMT, /**< Management frame, indicates 'buf' argument is wm_wifi_promiscuous_pkt_t */
    WM_WIFI_PKT_CTRL, /**< Control frame, indicates 'buf' argument is wm_wifi_promiscuous_pkt_t */
    WM_WIFI_PKT_DATA, /**< Data frame, indiciates 'buf' argument is wm_wifi_promiscuous_pkt_t */
    WM_WIFI_PKT_MISC, /**< Other type, such as MIMO etc. 'buf' argument is wm_wifi_promiscuous_pkt_t but the payload is zero length. */
} wm_wifi_promiscuous_pkt_type_t;

/**
  * @brief The RX callback function in the promiscuous mode.
  *        Each time a packet is received, the callback function will be called.
  *
  * @param buf  Data received. Type of data in buffer (wm_wifi_promiscuous_pkt_t or wm_wifi_pkt_rx_ctrl_t) indicated by 'type' parameter.
  * @param type  promiscuous packet type.
  *
  */
typedef void (*wm_wifi_promiscuous_cb_t)(wm_wifi_promiscuous_pkt_t *buf, wm_wifi_promiscuous_pkt_type_t type);

typedef enum {
    WM_WIFI_PROMIS_FILTER_MASK_MGMT       = (1),          /**< filter the packets with type of WM_WIFI_PKT_MGMT */
    WM_WIFI_PROMIS_FILTER_MASK_CTRL       = (1 << 1),     /**< filter the packets with type of WM_WIFI_PKT_CTRL */
    WM_WIFI_PROMIS_FILTER_MASK_DATA       = (1 << 2),     /**< filter the packets with type of WM_WIFI_PKT_DATA */
    WM_WIFI_PROMIS_FILTER_MASK_MISC       = (1 << 3),     /**< filter the packets with type of WM_WIFI_PKT_MISC */
    WM_WIFI_PROMIS_FILTER_MASK_DATA_MPDU  = (1 << 4),     /**< filter the MPDU which is a kind of WM_WIFI_PKT_DATA */
    WM_WIFI_PROMIS_FILTER_MASK_DATA_AMPDU = (1 << 5),     /**< filter the AMPDU which is a kind of WM_WIFI_PKT_DATA */
    WM_WIFI_PROMIS_FILTER_MASK_FCSFAIL    = (1 << 6),     /**< filter the FCS failed packets, do not open it in general */
    WM_WIFI_PROMIS_FILTER_MASK_ALL        = (0xFFFFFFFF), /**< filter all packets */

    WM_WIFI_PROMIS_CTRL_FILTER_MASK_WRAPPER  = (1 << 23),   /**< filter the control packets with subtype of Control Wrapper */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_BAR      = (1 << 24),   /**< filter the control packets with subtype of Block Ack Request */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_BA       = (1 << 25),   /**< filter the control packets with subtype of Block Ack */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_PSPOLL   = (1 << 26),   /**< filter the control packets with subtype of PS-Poll */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_RTS      = (1 << 27),   /**< filter the control packets with subtype of RTS */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_CTS      = (1 << 28),   /**< filter the control packets with subtype of CTS */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_ACK      = (1 << 29),   /**< filter the control packets with subtype of ACK */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_CFEND    = (1 << 30),   /**< filter the control packets with subtype of CF-END */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_CFENDACK = (1 << 31),   /**< filter the control packets with subtype of CF-END+CF-ACK */
    WM_WIFI_PROMIS_CTRL_FILTER_MASK_ALL      = (0xFF800000) /**< filter all control packets */
} wm_wifi_promiscuous_filter_type_t;

/** @brief Mask for filtering different packet types in promiscuous mode. */
typedef struct {
    uint32_t filter_mask; /**< OR of one or more filter values wm_wifi_promiscuous_filter_type_t */
} wm_wifi_promiscuous_filter_t;

typedef enum {
    WM_WIFI_REASON_UNSPECIFIED              = 1,
    WM_WIFI_REASON_AUTH_EXPIRE              = 2,
    WM_WIFI_REASON_AUTH_LEAVE               = 3,
    WM_WIFI_REASON_ASSOC_EXPIRE             = 4,
    WM_WIFI_REASON_ASSOC_TOOMANY            = 5,
    WM_WIFI_REASON_NOT_AUTHED               = 6,
    WM_WIFI_REASON_NOT_ASSOCED              = 7,
    WM_WIFI_REASON_ASSOC_LEAVE              = 8,
    WM_WIFI_REASON_ASSOC_NOT_AUTHED         = 9,
    WM_WIFI_REASON_DISASSOC_PWRCAP_BAD      = 10,
    WM_WIFI_REASON_DISASSOC_SUPCHAN_BAD     = 11,
    WM_WIFI_REASON_BSS_TRANSITION_DISASSOC  = 12,
    WM_WIFI_REASON_IE_INVALID               = 13,
    WM_WIFI_REASON_MIC_FAILURE              = 14,
    WM_WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
    WM_WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
    WM_WIFI_REASON_IE_IN_4WAY_DIFFERS       = 17,
    WM_WIFI_REASON_GROUP_CIPHER_INVALID     = 18,
    WM_WIFI_REASON_PAIRWISE_CIPHER_INVALID  = 19,
    WM_WIFI_REASON_AKMP_INVALID             = 20,
    WM_WIFI_REASON_UNSUPP_RSN_IE_VERSION    = 21,
    WM_WIFI_REASON_INVALID_RSN_IE_CAP       = 22,
    WM_WIFI_REASON_CIPHER_SUITE_REJECTED    = 24,
    WM_WIFI_REASON_UNSPECIFIED_QOS_REASON   = 32,
    WM_WIFI_REASON_NOT_ENOUGH_BANDWIDTH     = 33,
    WM_WIFI_REASON_DISASSOC_LOW_ACK         = 34,
    WM_WIFI_REASON_EXCEEDED_TXOP            = 35,
    WM_WIFI_REASON_STA_LEAVING              = 36,
    WM_WIFI_REASON_TIMEOUT                  = 39,
    WM_WIFI_REASON_INVALID_PMKID            = 49,

    WM_WIFI_REASON_NO_AP_FOUND    = 200,
    WM_WIFI_REASON_AUTH_FAIL      = 201,
    WM_WIFI_REASON_ASSOC_FAIL     = 202,
    WM_WIFI_REASON_BEACON_TIMEOUT = 203,
} wm_wifi_reason_code_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_WIFI_TYPES_H__ */
