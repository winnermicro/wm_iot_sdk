if(CONFIG_COMPONENT_WPA_SUPPLICANT_ENABLED)

    #list(APPEND ADD_INCLUDE ""
    #                        )

    list(APPEND ADD_PRIVATE_INCLUDE "src"
                                    "src/utils"
                                    "src/crypto"
                                    "src/l2_packet"
                                    "src/drivers"
                                    "wpa_supplicant"
                                    "."
                                    )

    list(APPEND ADD_SRCS "src/utils/common.c"
                         "src/utils/wpa_debug.c"
                         "src/utils/wpabuf.c"
                         "src/utils/crc32.c"
    #                     "src/rsn_supp/wpa_ft.c"
                         "src/rsn_supp/wpa.c"
                         "src/rsn_supp/pmksa_cache.c"
                         "src/rsn_supp/wpa_ie.c"
                         "src/crypto/aes-unwrap.c"
                         "src/crypto/aes-omac1.c"
                         "src/crypto/aes-wrap.c"
                         "src/crypto/md5.c"
                         "src/crypto/rc4.c"
                         "src/crypto/sha256-kdf.c"
                         "src/crypto/sha256.c"
                         "src/crypto/sha256-prf.c"
                         "src/crypto/sha256-internal.c"
                         "src/crypto/dh_groups.c"
                         "src/crypto/sha1.c"
                         "src/crypto/sha1-prf.c"
                         "src/crypto/sha1-pbkdf2.c"
                         "src/common/wpa_common.c"
                         "src/common/ieee802_11_common.c"
                         "src/common/hw_features_common.c"
                         "src/drivers/driver_common.c"
                         "src/drivers/drivers.c"
                         "wpa_supplicant/config.c"
                         "wpa_supplicant/notify.c"
                         "wpa_supplicant/bss.c"
                         "wpa_supplicant/op_classes.c"
                         "wpa_supplicant/rrm.c"
    #                     "wpa_supplicant/wmm_ac.c"
                         "wpa_supplicant/ctrl_iface.c"
                         "wpa_supplicant/wpa_supplicant.c"
                         "wpa_supplicant/events.c"
                         "wpa_supplicant/bssid_ignore.c"
                         "wpa_supplicant/wpas_glue.c"
                         "wpa_supplicant/scan.c"
                         "port/main_freertos.c"
                         "port/os_freertos.c"
                         )

    if(CONFIG_WPA_SUPPLICANT_ENABLE_AP)
        list(APPEND ADD_SRCS "src/ap/hostapd.c"
                             "src/ap/wpa_auth_glue.c"
                             "src/ap/utils.c"
                             "src/ap/ap_config.c"
                             "src/ap/sta_info.c"
                             "src/ap/tkip_countermeasures.c"
                             "src/ap/ap_mlme.c"
                             "src/ap/ieee802_11_shared.c"
                             "src/ap/drv_callbacks.c"
                             "src/ap/ap_drv_ops.c"
                             "src/ap/beacon.c"
                             "src/ap/bss_load.c"
                             "src/ap/ieee802_11_ht.c"
        #                     "src/ap/ieee802_11_vht.c"
                             "src/ap/ieee802_11_he.c"
                             "src/ap/ctrl_iface_ap.c"
                             "src/ap/wmm.c"
                             "src/ap/ap_list.c"
                             "src/ap/ieee802_11.c"
                             "src/ap/hw_features.c"
                             "src/ap/wpa_auth.c"
                             "src/ap/wpa_auth_ie.c"
                             "src/ap/pmksa_cache_auth.c"
                             "wpa_supplicant/ap.c"
                             )
    endif()

    if(NOT CONFIG_WPA_SUPPLICANT_HARD_CRYPTO_ENABLED)
        list(APPEND ADD_SRCS "src/crypto/md5-internal.c"
                             "src/crypto/sha1-internal.c"
                             "src/crypto/aes-internal-dec.c"
                             "src/crypto/aes-internal.c"
                             "src/crypto/aes-internal-enc.c"
                             )
        list(APPEND ADD_DEFINITIONS -DCONFIG_CRYPTO_INTERNAL
                                    )
    endif()

    if(${CONFIG_CHIP_W80X})
        list(APPEND ADD_SRCS "port/driver_w800.c"
                             "port/l2_packet_w800.c"
                             )
    endif()

    set(__source_code_path ${CMAKE_CURRENT_SOURCE_DIR}/port)
    is_path_component(__exist_source_code ${__source_code_path})

    if(NOT __exist_source_code)
        if(USE_CMAKE_BUILD_DEBUG)
            message(STATUS "wpa_supplicant port use lib")
        endif()
        list(APPEND ADD_STATIC_LIB "${SDK_PATH}/components/wm_soc/${CONFIG_CHIP_FAMILY_NAME}/lib/libwpas.a"
                                   )
    endif()

    list(APPEND ADD_DEFINITIONS -DCONFIG_WEP
                                -DCONFIG_CTRL_IFACE
                                -DCONFIG_CTRL_IFACE_FREERTOS
    #                            -DCONFIG_IEEE80211AC
                                -DCONFIG_IEEE80211AX
    #                            -DCONFIG_IEEE80211R
                                -DCONFIG_SHA256
                                )

    if(CONFIG_WPA_SUPPLICANT_ENABLE_SAE)
        list(APPEND ADD_SRCS "wpa_supplicant/sme.c"
                             "src/common/sae.c"
                             "src/common/dragonfly.c"
                             "src/crypto/aes-ccm.c"
                             #"src/common/sae_pk.c"
                             #"src/crypto/aes-siv.c"
                             #"src/crypto/aes-ctr.c"
                             )

        list(APPEND ADD_DEFINITIONS -DCONFIG_SAE
                                    -DCONFIG_SME
                                    -DCONFIG_WM_SME_OPT
                                    )
        if(CONFIG_WPA_SUPPLICANT_WFA_VERSION)
            list(APPEND ADD_DEFINITIONS -DCONFIG_WM_WFA_VERSION
                                        #-DCONFIG_SAE_PK
                                        #-DALL_DH_GROUPS
                                        )
        else()
            list(APPEND ADD_DEFINITIONS -DCONFIG_PMKSA_CACHE_SIMPLE
                                        -DCONFIG_PMKSA_CACHE_NVS
                                        )
        endif()
    endif()

    if(CONFIG_WPA_SUPPLICANT_WFA_VERSION)
        list(APPEND ADD_SRCS "wpa_supplicant/wnm_sta.c"
                             "wpa_supplicant/mbo.c"
                             )
        list(APPEND ADD_DEFINITIONS -DCONFIG_WM_WFA_VERSION
                                    -DCONFIG_WNM
                                    -DCONFIG_MBO
                                    )
    endif()

    if(NOT CONFIG_WPA_SUPPLICANT_CRYPTO_HARD_EC)
        list(APPEND ADD_DEFINITIONS -DCONFIG_NO_HARD_CRYPTO_EC
                                    )
    endif()

    if(CONFIG_WPA_SUPPLICANT_ENABLE_AP)
        list(APPEND ADD_DEFINITIONS -DCONFIG_AP
                                    -DNEED_AP_MLME
                                    )
    endif()

    list(APPEND ADD_DEFINITIONS -DOS_NO_C_LIB_DEFINES
                                -DCONFIG_NO_RADIUS
                                -DCONFIG_NO_ACCOUNTING
                                -DCONFIG_NO_VLAN
                                -DCONFIG_NO_MSCS
                                -DCONFIG_NO_RANDOM_POOL
                                -DCONFIG_NO_CONFIG_WRITE
                                -DCONFIG_NO_CONFIG_BLOBS
                                -DCONFIG_NO_AP_GAS_SERV
                                -DCONFIG_NO_AP_DFS
                                -DCONFIG_NO_RADIO_ADD
                                -DCONFIG_NO_BRIDGE_IFACE
                                -DCONFIG_NO_WAKE_ON_WLAN
                                -DCONFIG_NO_CSA
                                -DCONFIG_NO_NEIGHBOR
                                -DCONFIG_NO_HE_6GHZ
                                -DCONFIG_NO_EAP_SERVER
                                -DCONFIG_NO_AP_RRM
                                -DCONFIG_NO_STA_RRM
                                -DCONFIG_NO_EDMG_CHANNEL
                                -DCONFIG_NO_RNR
                                -DCONFIG_NO_WMM_AC
                                -DCONFIG_NO_RANDOM_ADDR
                                -DCONFIG_NO_SCHED_SCAN
                                -DCONFIG_NO_AP_MULTI_AP
                                -DCONFIG_NO_AP_WDS
                                -DCONFIG_NO_AP_WMM
                                -DCONFIG_NO_11A_5GHZ
                                -DCONFIG_NO_11AD_6GHZ
                                -DCONFIG_NO_AP_CHECK_11N40MHZ
                                -DCONFIG_NO_SME_AUTH_AND_ASSO
                                )

    list(APPEND ADD_DEFINITIONS -D__BYTE_ORDER=1234
                                -D__LITTLE_ENDIAN=1234
                                -DCONFIG_DRIVER_WM_WIFI
                                -DCONFIG_DRIVER_WM_WIFI_NAME="wm_wifi"
                                -DCONFIG_IFACE_NAME_STA="wlan0"
                                -DCONFIG_IFACE_NAME_AP="wlan1"
                                -DCONFIG_USE_EVENT_POST
                                -DCONFIG_CONNET_ALG_OPT
                                -DCONFIG_AP_TKIP_WEP_OPT
                                -DCONFIG_NOT_FUOUND_AP_STOP
                                -DCONFIG_NOT_FUOUND_AP_NUM=3
                                )

    if(NOT CONFIG_WPA_SUPPLICANT_ENABLE_DEBUG)
        list(APPEND ADD_DEFINITIONS -DCONFIG_NO_STDOUT_DEBUG
                                    -DCONFIG_NO_WPA_MSG
                                    )
    endif()

    if (CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
        list(APPEND ADD_DEFINITIONS -fno-profile-arcs -fno-test-coverage)
    endif()

    register_component()

    if(__exist_source_code)
        if(USE_CMAKE_BUILD_DEBUG)
            message(STATUS "wpa_supplicant port use source code: ${__source_code_path}")
        endif()
        add_subdirectory(${__source_code_path})
    endif()

endif()
