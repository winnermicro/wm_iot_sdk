#ifndef __WM_HEAP_H__
#define __WM_HEAP_H__

#include "wmsdk_config.h"
#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief memory heap names
 */
#define WM_HEAP_SRAM_NAME  "SRAM"
#define WM_HEAP_DRAM_NAME  "DRAM"
#define WM_HEAP_PSRAM_NAME "PSRAM"

/**
 * @brief Flags to indicate the capabilities of the various memory systems
 */
typedef enum {
    WM_HEAP_CAP_DEFAULT =
        (1 << 0), ///< Memory can be returned in a non-capability-specific memory allocation (e.g. malloc(), calloc()) call

    WM_HEAP_CAP_INTERNAL =
        (1 << 1), ///< Memory must be internal; specifically it should not disappear when flash/spiram cache is switched off
    WM_HEAP_CAP_SPIRAM = (1 << 2), ///< Memory must be in SPI RAM

    WM_HEAP_CAP_EXEC   = (1 << 3), ///< Memory must be able to run executable code
    WM_HEAP_CAP_SHARED = (1 << 4), ///< Memory must be able to accessed by WiFi & HSPI/SDIO Slave

    WM_HEAP_CAP_INVALID = (1 << 20), ///< Memory can't be used / list end marker
} wm_heap_cap_type_t;

typedef struct wm_heap_block {
    struct wm_heap_block *next_free_block; /*<< The next free block in the list. */
    size_t block_size;                     /*<< The size of the free block. */

    void *heap;
    wm_heap_cap_type_t caps;

#ifdef CONFIG_HEAP_USE_TRACING
    char *file;
    int line;
#endif

#if defined(CONFIG_HEAP_POISONING_LIGHT) || defined(CONFIG_HEAP_POISONING_COMPREHENSIVE)
    uint32_t corruption_magic;
#endif
} wm_heap_block_t;

typedef struct wm_heap {
    wm_heap_block_t start;
    wm_heap_block_t *end;

    size_t free_bytes_remaining;
    size_t minimum_ever_free_bytes_remaining;
    size_t number_of_successful_allocations;
    size_t number_of_successful_frees;

    size_t block_allocated_bit;
} wm_heap_t;

typedef struct {
    const char *mem_name;
    uint32_t start_address;
    size_t size_in_bytes;
    wm_heap_cap_type_t caps;

    wm_heap_t heap;
} wm_memory_layout_t;

extern size_t wm_soc_memory_count;
extern wm_memory_layout_t wm_soc_memory[];

/**
 * @defgroup WM_HEAP_APIs WM HEAP APIs
 * @brief WinnerMicro HEAP APIs
 */

/**
 * @addtogroup WM_HEAP_APIs
 * @{
 */

#ifdef CONFIG_HEAP_USE_TRACING
void *wm_heap_caps_alloc_tracing(size_t size, wm_heap_cap_type_t caps, const char *file, int line);
void *wm_heap_caps_realloc_tracing(void *old_mem, size_t new_size, wm_heap_cap_type_t caps, const char *file, int line);
#define wm_heap_caps_alloc(size, caps)                wm_heap_caps_alloc_tracing(size, caps, __FILE__, __LINE__)
#define wm_heap_caps_realloc(old_mem, new_size, caps) wm_heap_caps_realloc_tracing(old_mem, new_size, caps, __FILE__, __LINE__)
#else
/**
 * @brief          This function is called by memory allocation
 *
 * @param[in]      size
 * @param[in]      caps
 *
 * @retval         NULL    malloc failed
 * @retval         pointer pointer to the address of the allocated memory
 */
void *wm_heap_caps_alloc(size_t size, wm_heap_cap_type_t caps);

/**
 * @brief          This function is called by memory reallocation
 *
 * @param[in]      old_mem
 * @param[in]      new_size
 * @param[in]      caps
 *
 * @retval         NULL    realloc failed
 * @retval         pointer pointer to the address of the allocated memory
 */
void *wm_heap_caps_realloc(void *old_mem, size_t new_size, wm_heap_cap_type_t caps);
#endif

/**
 * @brief      This function is used to free memory
 *
 * @param[in]  p  memory to be freed
 *
 * @return     None
 */
void wm_heap_caps_free(void *p);

/**
 * @brief  Get free heap size (bytes)
 *
 * @return size_t
 */
size_t wm_heap_get_free_size(void);

/**
 * @brief  Get minimum ever free heap size (bytes)
 *
 * @return size_t
 */
size_t wm_heap_get_minimum_ever_free_size(void);

/**
  * @brief  Show heap memory allocation information
  *
  * @param[in]  p  memory pointer that requires an exception hint and can be NULL when normal
  *
  * @return     None
  */
void wm_heap_print_tracing(void *p);

/**
  * @brief  Show heap memory statistics
  *
  * @return     None
  */
void wm_heap_print_stats(void);

/**
  * @brief  Initialize heap
  *
  * @return     None
  *
  * @note This API must be called before all other heap API can be called,
  * This API will not initialize PSRAM as heap, call wm_heap_append_to_heap to append PSRAM to heap
  */
void wm_heap_init(void);

/**
  * @brief  Append the memory to heap
  *
  * @return
  *   WM_ERR_SUCCESS : OK,
  *   WM_ERR_INVALID_PARAM :fail
  *
  * @note Use this api append ram to heap, such as PSRAM
  */
int wm_heap_append_to_heap(const char *mem_name);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
