#include <stdint.h>
#include <string.h>
#include "wm_dt.h"
#include "wmsdk_config.h"
#include "wm_task_config.h"
#include "wm_types.h"
#include "wm_component.h"
#include "wm_heap.h"
#include "wm_osal.h"
#include "core_804.h"
#include "wm_debug.h"
#include "wm_hal_irq.h"
#ifdef CONFIG_COMPONENT_LOG_ENABLED
#define LOG_TAG "main"
#include "wm_log.h"
#endif
#include "wm_partition_table.h"
#if defined(CONFIG_CHIP_HAVE_WIFI) || defined(CONFIG_CHIP_HAVE_BT)
#include "wm_ft_param.h"
#endif
#if CONFIG_CHIP_HAVE_RF
#include "wm_rf.h"
#endif
#ifdef CONFIG_COMPONENT_NVS_ENABLED
#include "wm_nvs.h"
#include "wm_key_config.h"
#endif
#include "wm_soc_cfgs.h"
#include "wm_drv_rcc.h"
#include "wm_drv_flash.h"
#include "wm_drv_uart.h"

#define WM_SYSTEM_UART_LOG_RX_BUF_SIZE 512
#define WM_SYSTEM_UART_LOG_TX_BUF_SIZE 1024

extern int main(void);
extern void wm_soc_basic_init(wm_device_t *rcc_dev);

static void wm_sys_clk_set(uint8_t clk)
{
    uint8_t cpuDiv;

    if ((clk < 2) || (clk > 240)) {
        return;
    }

    cpuDiv = WM_PLL_CLOCK / clk;
    SysTick_Config(WM_PLL_CLOCK * UNIT_MHZ / cpuDiv / HZ);

    return;
}

static void wm_sys_init(wm_device_t *rcc_dev)
{
    uint32_t cpu_clk = wm_drv_rcc_get_config_clock(rcc_dev, WM_RCC_TYPE_CPU);

    wm_os_internal_init(NULL);

    wm_sys_clk_set(cpu_clk);
}

#if defined(CONFIG_CHIP_HAVE_WIFI) || defined(CONFIG_CHIP_HAVE_BT)
/* mac_addr array format: [0x01,0x23,0x45,0x67,0x89,0xAB] with 6 bytes length */
static bool is_valid_mac_addr_data(uint8_t *mac_addr)
{
    uint8_t invalid_mac1[WM_MAC_ADDR_LEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t invalid_mac0[WM_MAC_ADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    if (mac_addr == NULL || (memcmp(mac_addr, invalid_mac0, WM_MAC_ADDR_LEN) == 0) ||
        (memcmp(mac_addr, invalid_mac1, WM_MAC_ADDR_LEN) == 0)) {
        return false;
    }
    return true;
}

ATTRIBUTE_WEAK int wm_sys_get_mac_addr(wm_mac_type_t mac_type, uint8_t *mac_addr, uint8_t mac_addr_len)
{
    int ret                          = WM_ERR_FAILED;
    uint8_t def_mac[WM_MAC_ADDR_LEN] = WM_DEFAULT_BASE_MAC_ADDRESS;
    size_t len                       = WM_MAC_ADDR_LEN;

    if (mac_addr == NULL || mac_addr_len != WM_MAC_ADDR_LEN) {
        return WM_ERR_INVALID_PARAM;
    }

    /* Step1: Get real mac address from NVStore if NVStore is enabled*/
#if CONFIG_COMPONENT_NVS_ENABLED
    const char *_nv_key   = NULL;
    const char *_nv_group = NULL;

    wm_nvs_handle_t handle = NULL;

    switch (mac_type) {
        case WM_MAC_TYPE_STA:
        {
            _nv_key   = WM_KEY_STA_MAC_ADDR;
            _nv_group = WM_GROUP_WIFI;
            break;
        }
        case WM_MAC_TYPE_SAP:
        {
            _nv_key   = WM_KEY_SAP_MAC_ADDR;
            _nv_group = WM_GROUP_WIFI;
            break;
        }
        case WM_MAC_TYPE_BT:
        {
            _nv_key   = WM_KEY_BT_MAC_ADDR;
            _nv_group = WM_GROUP_BLE;
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    if (wm_nvs_open(WM_NVS_DEF_PARTITION, _nv_group, WM_NVS_OP_READ_WRITE, &handle) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    if (wm_nvs_get_binary(handle, _nv_key, mac_addr, &len) == WM_ERR_SUCCESS) {
        if (is_valid_mac_addr_data(mac_addr)) {
            wm_log_debug("GetMAC From NVS (Type=%d) %02x:%02x:%02x:%02x:%02x:%02x", mac_type, mac_addr[0], mac_addr[1],
                         mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            wm_nvs_close(handle);
            return WM_ERR_SUCCESS;
        }
    }

    wm_nvs_close(handle);
#endif

    /* Step2: get raw mac address from FT (on OTP or Flash protected Region) */
    if (mac_type == WM_MAC_TYPE_STA || mac_type == WM_MAC_TYPE_SAP) {
        ret = wm_ft_get_mac_addr(mac_addr);
    } else if (mac_type == WM_MAC_TYPE_BT) {
        ret = wm_ft_get_bt_mac_addr(mac_addr);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    wm_log_debug("Get RAW MAC From FT (Type=%d) %02x:%02x:%02x:%02x:%02x:%02x", mac_type, mac_addr[0], mac_addr[1], mac_addr[2],
                 mac_addr[3], mac_addr[4], mac_addr[5]);

    /* Step3: adopt the default MAC as the MAC source if the result from getting the FT value is not as expected */
    if (ret != WM_ERR_SUCCESS || !is_valid_mac_addr_data(mac_addr)) {
        memcpy(mac_addr, def_mac, sizeof(def_mac));
    } else if (mac_type == WM_MAC_TYPE_BT) {
        /* use the FT valid MAC address directly as the real mac address for BT */
        return WM_ERR_SUCCESS;
    }

    /* Step4: generate the real mac address base on the selected mac source*/
    switch (mac_type) {
        case WM_MAC_TYPE_STA:
        {
            break; /* use the mac source directly */
        }
        case WM_MAC_TYPE_SAP:
        {
            if (mac_addr[0] & WM_BIT(1)) {
                mac_addr[5] += 2; /* increase the last byte if it is a local address */
            } else {
                mac_addr[0] |= WM_BIT(1); /* set as local address */
            }
            break;
        }
        case WM_MAC_TYPE_BT:
        {
            mac_addr[5] += 1; /* increase the last byte */
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    wm_log_debug("Return MAC (Type=%d) %02x:%02x:%02x:%02x:%02x:%02x", mac_type, mac_addr[0], mac_addr[1], mac_addr[2],
                 mac_addr[3], mac_addr[4], mac_addr[5]);

    return WM_ERR_SUCCESS;
}

ATTRIBUTE_WEAK int wm_sys_set_mac_addr(wm_mac_type_t mac_type, uint8_t *mac_addr, uint8_t mac_addr_len)
{
    if (mac_addr_len != WM_MAC_ADDR_LEN || !(is_valid_mac_addr_data(mac_addr))) {
        return WM_ERR_INVALID_PARAM;
    }

    /* Step1: Write real mac address into NVStore if NVStore is enabled*/
#if CONFIG_COMPONENT_NVS_ENABLED
    const char *_nv_key    = NULL;
    wm_nvs_handle_t handle = NULL;
    const char *_nv_group  = NULL;

    switch (mac_type) {
        case WM_MAC_TYPE_STA:
        {
            _nv_key   = WM_KEY_STA_MAC_ADDR;
            _nv_group = WM_GROUP_WIFI;
            break;
        }
        case WM_MAC_TYPE_SAP:
        {
            _nv_key   = WM_KEY_SAP_MAC_ADDR;
            _nv_group = WM_GROUP_WIFI;
            break;
        }
        case WM_MAC_TYPE_BT:
        {
            _nv_key   = WM_KEY_BT_MAC_ADDR;
            _nv_group = WM_GROUP_BLE;
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    if (wm_nvs_open(WM_NVS_DEF_PARTITION, _nv_group, WM_NVS_OP_READ_WRITE, &handle) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    if (wm_nvs_set_binary(handle, _nv_key, mac_addr, mac_addr_len) == WM_ERR_SUCCESS) {
        wm_log_debug("SetMAC into NVS (Type=%d) %02x:%02x:%02x:%02x:%02x:%02x", mac_type, mac_addr[0], mac_addr[1], mac_addr[2],
                     mac_addr[3], mac_addr[4], mac_addr[5]);
        wm_nvs_close(handle);
        return WM_ERR_SUCCESS;
    } else {
        wm_nvs_close(handle);
        return WM_ERR_FAILED;
    }

#endif

    /* it is allowed to add more MAC storage stratage*/

    /* Step2: Return Failure if no usable storage is enabled for MAC address*/
    return WM_ERR_NO_SUPPORT;
}
#endif

static void factory_params_init(void)
{
    uint8_t mac_addr[WM_MAC_ADDR_LEN] = { 0 };
    size_t mac_len                    = WM_MAC_ADDR_LEN;

    /* Store the MAC address in NVStore if there is none */
#if CONFIG_COMPONENT_NVS_ENABLED && (CONFIG_CHIP_HAVE_WIFI || CONFIG_CHIP_HAVE_BT)
    wm_nvs_handle_t handle = NULL;

#if CONFIG_CHIP_HAVE_WIFI

    if (wm_nvs_open(WM_NVS_DEF_PARTITION, WM_GROUP_WIFI, WM_NVS_OP_READ_WRITE, &handle) != WM_ERR_SUCCESS) {
        return;
    }
    if (wm_nvs_get_binary(handle, WM_KEY_STA_MAC_ADDR, mac_addr, &mac_len) != WM_ERR_SUCCESS) {
        if (wm_sys_get_mac_addr(WM_MAC_TYPE_STA, mac_addr, sizeof(mac_addr)) == WM_ERR_SUCCESS) {
            wm_sys_set_mac_addr(WM_MAC_TYPE_STA, mac_addr, sizeof(mac_addr));
        }
        wm_log_debug("Get WM_MAC_TYPE_STA %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
                     mac_addr[4], mac_addr[5]);
    }

    if (wm_nvs_get_binary(handle, WM_KEY_SAP_MAC_ADDR, mac_addr, &mac_len) != WM_ERR_SUCCESS) {
        if (wm_sys_get_mac_addr(WM_MAC_TYPE_SAP, mac_addr, sizeof(mac_addr)) == WM_ERR_SUCCESS) {
            wm_sys_set_mac_addr(WM_MAC_TYPE_SAP, mac_addr, sizeof(mac_addr));
        }
        wm_log_debug("Get WM_MAC_TYPE_SAP %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
                     mac_addr[4], mac_addr[5]);
    }
    wm_nvs_close(handle);
#endif

#if CONFIG_CHIP_HAVE_BT

    if (wm_nvs_open(WM_NVS_DEF_PARTITION, WM_GROUP_BLE, WM_NVS_OP_READ_WRITE, &handle) != WM_ERR_SUCCESS) {
        return;
    }

    if (wm_nvs_get_binary(handle, WM_KEY_BT_MAC_ADDR, mac_addr, &mac_len) != WM_ERR_SUCCESS) {
        if (wm_sys_get_mac_addr(WM_MAC_TYPE_BT, mac_addr, sizeof(mac_addr)) == WM_ERR_SUCCESS) {
            wm_sys_set_mac_addr(WM_MAC_TYPE_BT, mac_addr, sizeof(mac_addr));
        }
        wm_log_debug("Get WM_MAC_TYPE_BT %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
                     mac_addr[4], mac_addr[5]);
    }

    wm_nvs_close(handle);
#endif

#endif
    /*to avoid the compiler warning*/
    ((void)mac_addr);
    ((void)mac_len);
}

static void dump_chip_info(void)
{
#ifdef CONFIG_COMPONENT_LOG_ENABLED
    wm_log_info("ver: %s build at %s %s", CONFIG_BUILD_VERSION, __DATE__, __TIME__);

    wm_log_info("boot reason %d", wm_get_reboot_reason());

    uint32_t heap_size = wm_heap_get_free_size();
    float heap_size_k  = heap_size / 1024.0;
    wm_log_info("heap size %u(%0.2f%s)", heap_size, heap_size_k > 1024 ? heap_size_k / 1024 : heap_size_k,
                heap_size_k > 1024 ? "MB" : "KB");

    wm_drv_flash_info_t flash_info = { 0 };
    wm_drv_flash_get_device_info(wm_dt_get_device_by_name("iflash"), &flash_info);
    wm_log_info("flash size 0x%x(%uMB)", flash_info.flash_size, flash_info.flash_size / 1024 / 1024);

    //wm_partition_table_print();
#endif /* CONFIG_COMPONENT_LOG_ENABLED */
}

static void wm_start_task(void *data)
{
    wm_drv_uart_init(CONFIG_LOG_PRINT_UART_DEVICE, WM_SYSTEM_UART_LOG_RX_BUF_SIZE, WM_SYSTEM_UART_LOG_TX_BUF_SIZE);

    wm_drv_flash_init("iflash");

    wm_partition_table_init();

#if defined(CONFIG_CHIP_HAVE_WIFI) || defined(CONFIG_CHIP_HAVE_BT)
    wm_ft_param_init();
#endif

#if CONFIG_CHIP_HAVE_RF
    wm_rf_check();
#endif

#ifdef CONFIG_COMPONENT_NVS_ENABLED
    wm_nvs_init(WM_NVS_DEF_PARTITION);
#endif

    dump_chip_info();

    factory_params_init();

    /* auto driver init */
    wm_dt_auto_init_device();

    /* auto components init */
    wm_components_init();

    main();

    wm_os_internal_task_del(NULL);
}

void wm_start(void)
{
    wm_device_t *rcc_dev = NULL;

    wm_hal_irq_init();

    rcc_dev = wm_drv_clock_init("rcc");

    wm_heap_append_to_heap(WM_HEAP_PSRAM_NAME);

    wm_soc_basic_init(rcc_dev);

    wm_sys_init(rcc_dev);

    wm_os_internal_task_create(NULL, "main", wm_start_task, NULL, WM_TASK_MAIN_STACK, WM_TASK_MAIN_PRIO, 0);

    wm_os_internal_start_scheduler();

    return;
}
