#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_heap.h"

/* The contents of start_address and size_in_bytes in this table are just initial values.
 * They will be recalculated and overwritten by the linker result in wm_heap.c.
 *
 * The reason why the caps type of pSRAM is not WM_HEAP_CAP_DEFAULT is that we cannot determine
 * how the application should use the malloced buffer, as there is a limitation that the W80X pSRAM 
 * cannot be accessed simultaneously by the CPU and DMA.
 */
wm_memory_layout_t wm_soc_memory[] = {
    { WM_HEAP_SRAM_NAME,  CONFIG_SRAM_BASE_ADDR,  CONFIG_SRAM_LENGTH,
     WM_HEAP_CAP_DEFAULT | WM_HEAP_CAP_INTERNAL | WM_HEAP_CAP_EXEC                            },
    { WM_HEAP_DRAM_NAME,  CONFIG_DRAM_BASE_ADDR,  CONFIG_DRAM_LENGTH,
     WM_HEAP_CAP_DEFAULT | WM_HEAP_CAP_INTERNAL | WM_HEAP_CAP_SHARED                          },
#ifdef CONFIG_HEAP_USE_PSRAM
    { WM_HEAP_PSRAM_NAME, CONFIG_PSRAM_BASE_ADDR, CONFIG_PSRAM_MAX_LENGTH, WM_HEAP_CAP_SPIRAM }
#endif
};

size_t wm_soc_memory_count = sizeof(wm_soc_memory) / sizeof(wm_memory_layout_t);
