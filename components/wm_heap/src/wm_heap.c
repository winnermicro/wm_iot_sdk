#include <string.h>
#include <assert.h>
#include "wm_types.h"
#include "wm_error.h"
#include "wm_heap.h"
#include "wm_osal.h"
#include "wm_drv_psram.h"

#define LOG_TAG "heap"
#include "wm_log.h"

#define HEAP_PROTECT_DISABLE_INTR 1

#if !HEAP_PROTECT_DISABLE_INTR
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#define HEAP_MINIMUM_BLOCK_SIZE  ((size_t)(wm_heaps.heap_struct_size << 1))

#define HEAP_BITS_PER_BYTE       ((size_t)8)
#define HEAP_BYTE_ALIGNMENT      8
#define HEAP_BYTE_ALIGNMENT_MASK (0x0007)

#if defined(CONFIG_HEAP_POISONING_LIGHT) || defined(CONFIG_HEAP_POISONING_COMPREHENSIVE)
#define HEAP_CORRUPTION_MAGIC_HEADER 0xA5A5A5A5
#endif

#ifdef CONFIG_HEAP_POISONING_COMPREHENSIVE
#define HEAP_CORRUPTION_MAGIC_TAILER 0x5A5A5A5A
#endif

#ifdef CONFIG_HEAP_USE_ASSERT
#define HEAP_ASSERT(a) assert(a)
#else
#define HEAP_ASSERT(a)
#endif

typedef struct {
    size_t heap_struct_size;

    size_t free_bytes_remaining;
    size_t minimum_ever_free_bytes_remaining;
    size_t number_of_successful_allocations;
    size_t number_of_successful_frees;
} wm_heaps_t;

extern uint32_t __bss_end__;
extern uint32_t _eshram;

static wm_heaps_t wm_heaps;

#ifdef CONFIG_HEAP_USE_TRACING
static wm_heap_block_t heap_used_block_list = { 0 };

static void heap_insert_block_into_used_list(wm_heap_block_t *block_to_insert)
{
    block_to_insert->next_free_block     = heap_used_block_list.next_free_block;
    heap_used_block_list.next_free_block = block_to_insert;

    heap_used_block_list.block_size += block_to_insert->block_size;
    heap_used_block_list.file += 1;
}

static void heap_remove_block_from_used_list(wm_heap_t *heap, wm_heap_block_t *block_to_remove)
{
    wm_heap_block_t *previous_block, *block;

    previous_block = &heap_used_block_list;
    block          = previous_block->next_free_block;

    while (block) {
        if (block == block_to_remove) {
            previous_block->next_free_block = block->next_free_block;
            block->next_free_block          = NULL;

            heap_used_block_list.block_size -= block->block_size & (~heap->block_allocated_bit);
            heap_used_block_list.file -= 1;

            break;
        }

        previous_block = block;
        block          = block->next_free_block;
    }
}
#endif //CONFIG_HEAP_USE_TRACING

static void heap_insert_block_into_free_list(wm_heap_t *heap, wm_heap_block_t *block_to_insert)
{
    wm_heap_block_t *iterator;
    uint8_t *puc;

    for (iterator = &heap->start; iterator->next_free_block < block_to_insert; iterator = iterator->next_free_block) {
        /* Nothing to do here, just iterate to the right position. */
    }

    puc = (uint8_t *)iterator;

    if ((puc + iterator->block_size) == (uint8_t *)block_to_insert) {
        iterator->block_size += block_to_insert->block_size;
        block_to_insert = iterator;
    }

    puc = (uint8_t *)block_to_insert;

    if ((puc + block_to_insert->block_size) == (uint8_t *)iterator->next_free_block) {
        if (iterator->next_free_block != heap->end) {
            block_to_insert->block_size += iterator->next_free_block->block_size;
            block_to_insert->next_free_block = iterator->next_free_block->next_free_block;
        } else {
            block_to_insert->next_free_block = heap->end;
        }
    } else {
        block_to_insert->next_free_block = iterator->next_free_block;
    }

    if (iterator != block_to_insert) {
        iterator->next_free_block = block_to_insert;
    }
}

#ifdef CONFIG_HEAP_USE_TRACING
static void *heap_alloc_tracing(wm_memory_layout_t *memory, size_t wanted_size, wm_heap_cap_type_t caps, const char *file,
                                int line)
#else
static void *heap_alloc(wm_memory_layout_t *memory, size_t wanted_size, wm_heap_cap_type_t caps)
#endif
{
    wm_heap_block_t *block, *previous_block, *new_block_link;
    void *return_mem = NULL;
#if HEAP_PROTECT_DISABLE_INTR
    wm_os_internal_set_critical();
#else
    vTaskSuspendAll();
#endif

    HEAP_ASSERT(memory->heap.end != NULL);

    if ((wanted_size & memory->heap.block_allocated_bit) == 0) {
        if (wanted_size > 0) {
            wanted_size += wm_heaps.heap_struct_size;

#ifdef CONFIG_HEAP_POISONING_COMPREHENSIVE
            wanted_size += sizeof(uint32_t); //for tailer
#endif

            if ((wanted_size & HEAP_BYTE_ALIGNMENT_MASK) != 0x00) {
                wanted_size += (HEAP_BYTE_ALIGNMENT - (wanted_size & HEAP_BYTE_ALIGNMENT_MASK));
                HEAP_ASSERT((wanted_size & HEAP_BYTE_ALIGNMENT_MASK) == 0);
            }
        }

        if ((wanted_size > 0) && (wanted_size <= memory->heap.free_bytes_remaining)) {
            previous_block = &memory->heap.start;
            block          = memory->heap.start.next_free_block;

            while ((block->block_size < wanted_size) && (block->next_free_block != NULL)) {
                previous_block = block;
                block          = block->next_free_block;
            }

            if (block != memory->heap.end) {
                return_mem = (void *)(((uint8_t *)previous_block->next_free_block) + wm_heaps.heap_struct_size);

                previous_block->next_free_block = block->next_free_block;

                if ((block->block_size - wanted_size) > HEAP_MINIMUM_BLOCK_SIZE) {
                    new_block_link = (void *)(((uint8_t *)block) + wanted_size);
                    HEAP_ASSERT((((size_t)new_block_link) & HEAP_BYTE_ALIGNMENT_MASK) == 0);

                    new_block_link->block_size = block->block_size - wanted_size;
                    block->block_size          = wanted_size;

                    heap_insert_block_into_free_list(&memory->heap, new_block_link);
                }

                memory->heap.free_bytes_remaining -= block->block_size;

                if (memory->heap.free_bytes_remaining < memory->heap.minimum_ever_free_bytes_remaining) {
                    memory->heap.minimum_ever_free_bytes_remaining = memory->heap.free_bytes_remaining;
                }

                block->heap = &memory->heap;
                block->caps = caps;

                wm_heaps.free_bytes_remaining -= block->block_size;
                if (wm_heaps.minimum_ever_free_bytes_remaining < memory->heap.minimum_ever_free_bytes_remaining) {
                    wm_heaps.minimum_ever_free_bytes_remaining = memory->heap.minimum_ever_free_bytes_remaining;
                }

#if defined(CONFIG_HEAP_POISONING_LIGHT) || defined(CONFIG_HEAP_POISONING_COMPREHENSIVE)
                block->corruption_magic = HEAP_CORRUPTION_MAGIC_HEADER;
#endif

#ifdef CONFIG_HEAP_POISONING_COMPREHENSIVE
                *(uint32_t *)(((uint8_t *)return_mem) + (wanted_size - wm_heaps.heap_struct_size) - sizeof(uint32_t)) =
                    HEAP_CORRUPTION_MAGIC_TAILER;
#endif

#ifdef CONFIG_HEAP_USE_TRACING
                block->file = (char *)file;
                block->line = line;
                heap_insert_block_into_used_list(block);
#else
                block->next_free_block = NULL;
#endif

                block->block_size |= memory->heap.block_allocated_bit;
                memory->heap.number_of_successful_allocations++;
                wm_heaps.number_of_successful_allocations++;
            }
        }
    }

#if HEAP_PROTECT_DISABLE_INTR
    wm_os_internal_release_critical();
#else
    xTaskResumeAll();
#endif

    HEAP_ASSERT((((size_t)return_mem) & (size_t)HEAP_BYTE_ALIGNMENT_MASK) == 0);

    return return_mem;
}

static void heap_free(void *pv)
{
    uint8_t *puc = (uint8_t *)pv;
    wm_heap_block_t *link;
    wm_heap_t *heap;

    if (pv != NULL) {
        puc -= wm_heaps.heap_struct_size;

        link = (void *)puc;
        heap = link->heap;

#ifdef CONFIG_HEAP_USE_TRACING
#if HEAP_PROTECT_DISABLE_INTR
        wm_os_internal_set_critical();
#else
        vTaskSuspendAll();
#endif
        heap_remove_block_from_used_list(heap, link);
#if HEAP_PROTECT_DISABLE_INTR
        wm_os_internal_release_critical();
#else
        xTaskResumeAll();
#endif
#endif

        HEAP_ASSERT((link->block_size & heap->block_allocated_bit) != 0);
        HEAP_ASSERT(link->next_free_block == NULL);

        if ((link->block_size & heap->block_allocated_bit) != 0) {
            if (link->next_free_block == NULL) {
#if defined(CONFIG_HEAP_POISONING_LIGHT) || defined(CONFIG_HEAP_POISONING_COMPREHENSIVE)
                if (link->corruption_magic != HEAP_CORRUPTION_MAGIC_HEADER) {
                    wm_heap_print_tracing(link);
                }
                HEAP_ASSERT(link->corruption_magic == HEAP_CORRUPTION_MAGIC_HEADER);
#endif

#ifdef CONFIG_HEAP_POISONING_COMPREHENSIVE
                if (*(uint32_t *)(puc + (link->block_size & (~heap->block_allocated_bit)) - sizeof(uint32_t)) !=
                    HEAP_CORRUPTION_MAGIC_TAILER) {
                    wm_heap_print_tracing(link);
                }
                HEAP_ASSERT(*(uint32_t *)(puc + (link->block_size & (~heap->block_allocated_bit)) - sizeof(uint32_t)) ==
                            HEAP_CORRUPTION_MAGIC_TAILER);
#endif

                link->block_size &= ~heap->block_allocated_bit;

#if HEAP_PROTECT_DISABLE_INTR
                wm_os_internal_set_critical();
#else
                vTaskSuspendAll();
#endif

#ifdef CONFIG_HEAP_USE_TRACING
                link->file = NULL;
                link->line = -1;
#endif

                heap->free_bytes_remaining += link->block_size;
                wm_heaps.free_bytes_remaining += link->block_size;
                heap_insert_block_into_free_list(heap, (wm_heap_block_t *)link);
                heap->number_of_successful_frees++;
                wm_heaps.number_of_successful_frees++;

#if HEAP_PROTECT_DISABLE_INTR
                wm_os_internal_release_critical();
#else
                xTaskResumeAll();
#endif
            }
        }
    }
}

static void heap_init(wm_memory_layout_t *memory)
{
    wm_heap_block_t *first_free_block;
    uint8_t *aligned_heap;
    size_t address;
    size_t total_heap_size = memory->size_in_bytes;

    address = (size_t)memory->start_address;

    if ((address & HEAP_BYTE_ALIGNMENT_MASK) != 0) {
        address += (HEAP_BYTE_ALIGNMENT - 1);
        address &= ~((size_t)HEAP_BYTE_ALIGNMENT_MASK);
        total_heap_size -= address - (size_t)memory->start_address;
    }

    aligned_heap = (uint8_t *)address;

    memory->heap.start.next_free_block = (void *)aligned_heap;
    memory->heap.start.block_size      = (size_t)0;

    address = ((size_t)aligned_heap) + total_heap_size;
    address -= wm_heaps.heap_struct_size;
    address &= ~((size_t)HEAP_BYTE_ALIGNMENT_MASK);
    memory->heap.end                  = (void *)address;
    memory->heap.end->block_size      = 0;
    memory->heap.end->next_free_block = NULL;

    first_free_block                  = (void *)aligned_heap;
    first_free_block->block_size      = address - (size_t)first_free_block;
    first_free_block->next_free_block = memory->heap.end;

    memory->heap.minimum_ever_free_bytes_remaining = first_free_block->block_size;
    memory->heap.free_bytes_remaining              = first_free_block->block_size;

    memory->heap.block_allocated_bit = ((size_t)1) << ((sizeof(size_t) * HEAP_BITS_PER_BYTE) - 1);
}

static inline void wm_heap_print_stats_internal(void)
{
    size_t i;
    size_t remain;
    float remain_k;

    remain   = wm_heaps.free_bytes_remaining;
    remain_k = remain / 1024.0;

    wm_printf_direct("heap remain %u (%0.2f%s):\r\n", remain, (size_t)remain_k > 1024 ? remain_k / 1024 : remain_k,
                     (size_t)remain_k > 1024 ? "MB" : "KB");

    for (i = 0; i < wm_soc_memory_count; i++) {
        wm_memory_layout_t *memory = &wm_soc_memory[i];

        if (!(WM_HEAP_CAP_INVALID & memory->caps)) {
            remain   = memory->heap.free_bytes_remaining;
            remain_k = remain / 1024.0;
            wm_printf_direct("    %-8s remain %-7u (%0.2f%s)\r\n", memory->mem_name, remain,
                             (size_t)remain_k > 1024 ? remain_k / 1024 : remain_k, (size_t)remain_k > 1024 ? "MB" : "KB");
        }
    }
}

void wm_heap_print_stats(void)
{
#if HEAP_PROTECT_DISABLE_INTR
    wm_os_internal_set_critical();
#else
    vTaskSuspendAll();
#endif

    wm_heap_print_stats_internal();

#if HEAP_PROTECT_DISABLE_INTR
    wm_os_internal_release_critical();
#else
    xTaskResumeAll();
#endif
}

void wm_heap_print_tracing(void *p)
{
#ifdef CONFIG_HEAP_USE_TRACING
    wm_heap_block_t *block;
    size_t index = 0;
    wm_heap_t *heap;
    wm_heap_block_t *link = p;
#if HEAP_PROTECT_DISABLE_INTR
    wm_os_internal_set_critical();
#else
    vTaskSuspendAll();
#endif

    block = heap_used_block_list.next_free_block;

    while (block) {
        heap = block->heap;
        wm_printf_direct("%u\t%p\t%s:%d\t%u\r\n", index++, (uint8_t *)block + wm_heaps.heap_struct_size,
                         block->file ? block->file : '\0', block->line >= 0 ? block->line : -1,
                         block->block_size & (~heap->block_allocated_bit));

        block = block->next_free_block;
    }

    wm_printf_direct("summary: malloc size %u, malloc count %u\r\n", heap_used_block_list.block_size,
                     (size_t)heap_used_block_list.file);

    if (link != NULL) {
        heap = link->heap;
        wm_printf_direct("corruption detected %p in %s:%d size %u\r\n", (uint8_t *)link + wm_heaps.heap_struct_size, link->file,
                         link->line, link->block_size & (~heap->block_allocated_bit));
        wm_heap_print_stats_internal();
    }

#if HEAP_PROTECT_DISABLE_INTR
    wm_os_internal_release_critical();
#else
    xTaskResumeAll();
#endif

#else //CONFIG_HEAP_USE_TRACING

#if defined(CONFIG_HEAP_POISONING_LIGHT) || defined(CONFIG_HEAP_POISONING_COMPREHENSIVE)
    wm_heap_block_t *link = p;

    if (link != NULL) {
        printf("corruption detected in %p\r\n", (uint8_t *)link + wm_heaps.heap_struct_size);
        wm_heap_print_stats_internal();
    }
#endif //CONFIG_HEAP_POISONING_LIGHT || CONFIG_HEAP_POISONING_COMPREHENSIVE

#endif //CONFIG_HEAP_USE_TRACING
}

size_t wm_heap_get_free_size(void)
{
    return wm_heaps.free_bytes_remaining;
}

size_t wm_heap_get_minimum_ever_free_size(void)
{
    return wm_heaps.minimum_ever_free_bytes_remaining;
}

#ifdef CONFIG_HEAP_USE_TRACING
void *wm_heap_caps_realloc_tracing(void *old_mem, size_t new_size, wm_heap_cap_type_t caps, const char *file, int line)
#else
void *wm_heap_caps_realloc(void *old_mem, size_t new_size, wm_heap_cap_type_t caps)
#endif
{
    uint8_t *puc  = (uint8_t *)old_mem;
    void *new_mem = NULL;
    wm_heap_block_t *link;
    wm_heap_t *heap;
    size_t old_size;

    if (!old_mem) {
#ifdef CONFIG_HEAP_USE_TRACING
        return wm_heap_caps_alloc_tracing(new_size, caps, file, line);
#else
        return wm_heap_caps_alloc(new_size, caps);
#endif
    }

    if (0 == new_size) {
        wm_heap_caps_free(old_mem);
        return NULL;
    }

    puc -= wm_heaps.heap_struct_size;
    link = (void *)puc;
    heap = link->heap;

    HEAP_ASSERT((link->block_size & heap->block_allocated_bit) != 0);
#ifndef CONFIG_HEAP_USE_TRACING
    HEAP_ASSERT(link->next_free_block == NULL);
#endif

    if (0 == caps) {
        caps = link->caps;
    }

#ifdef CONFIG_HEAP_USE_TRACING
    if (!(new_mem = wm_heap_caps_alloc_tracing(new_size, caps, file, line)))
#else
    if (!(new_mem = wm_heap_caps_alloc(new_size, caps)))
#endif
    {
        return NULL;
    }

    old_size = (link->block_size & (~heap->block_allocated_bit));
    memcpy(new_mem, old_mem, old_size <= new_size ? old_size : new_size);

    wm_heap_caps_free(old_mem);

    return new_mem;
}

#ifdef CONFIG_HEAP_USE_TRACING
void *wm_heap_caps_alloc_tracing(size_t size, wm_heap_cap_type_t caps, const char *file, int line)
#else
void *wm_heap_caps_alloc(size_t size, wm_heap_cap_type_t caps)
#endif
{
    size_t i;
    void *p = NULL;

    if (0 == size) {
        return NULL;
    }

    if (0 == caps)
        caps = WM_HEAP_CAP_DEFAULT;

    for (i = 0; i < wm_soc_memory_count; i++) {
        wm_memory_layout_t *memory = &wm_soc_memory[i];

        if (WM_HEAP_CAP_INVALID & memory->caps)
            continue;

        if ((caps & memory->caps) == caps) {
#ifdef CONFIG_HEAP_USE_TRACING
            p = heap_alloc_tracing(memory, size, caps, file, line);
#else
            p = heap_alloc(memory, size, caps);
#endif
            if (p) {
                return p;
            }
        }
    }

    return NULL;
}

void wm_heap_caps_free(void *p)
{
    if (NULL == p)
        return;

    heap_free(p);
}

void wm_heap_init(void)
{
    size_t i;

    wm_soc_memory[0].size_in_bytes -= (uint32_t)&__bss_end__ - wm_soc_memory[0].start_address;
    wm_soc_memory[0].start_address = (uint32_t)&__bss_end__;

    wm_soc_memory[1].size_in_bytes -=
        (uint32_t)&_eshram - wm_soc_memory[1].start_address + 4; //w800 tailer 4byte save reboot reason
    wm_soc_memory[1].start_address = (uint32_t)&_eshram;

    wm_heaps.heap_struct_size =
        (sizeof(wm_heap_block_t) + ((size_t)(HEAP_BYTE_ALIGNMENT - 1))) & ~((size_t)HEAP_BYTE_ALIGNMENT_MASK);
    wm_heaps.free_bytes_remaining              = 0;
    wm_heaps.minimum_ever_free_bytes_remaining = 0;
    wm_heaps.number_of_successful_allocations  = 0;
    wm_heaps.number_of_successful_frees        = 0;

    for (i = 0; i < wm_soc_memory_count; i++) {
        wm_memory_layout_t *memory = &wm_soc_memory[i];

        /* Not init PSRAM here as device table not ready */
        if (!strcmp(memory->mem_name, WM_HEAP_PSRAM_NAME)) {
            memory->caps |= WM_HEAP_CAP_INVALID;
            continue;
        }
        memory->heap.end                               = NULL;
        memory->heap.free_bytes_remaining              = 0;
        memory->heap.minimum_ever_free_bytes_remaining = 0;
        memory->heap.number_of_successful_allocations  = 0;
        memory->heap.number_of_successful_frees        = 0;
        memory->heap.block_allocated_bit               = 0;

        heap_init(memory);

        wm_heaps.free_bytes_remaining += memory->heap.free_bytes_remaining;
        wm_heaps.minimum_ever_free_bytes_remaining = wm_heaps.free_bytes_remaining;
    }
}

int wm_heap_append_to_heap(const char *mem_name)
{
    if (!mem_name) {
        return WM_ERR_INVALID_PARAM;
    }

    for (int i = 0; i < wm_soc_memory_count; i++) {
        wm_memory_layout_t *memory = &wm_soc_memory[i];

        if (strcmp(memory->mem_name, mem_name)) {
            continue;
        }

        memory->heap.end                               = NULL;
        memory->heap.free_bytes_remaining              = 0;
        memory->heap.minimum_ever_free_bytes_remaining = 0;
        memory->heap.number_of_successful_allocations  = 0;
        memory->heap.number_of_successful_frees        = 0;
        memory->heap.block_allocated_bit               = 0;
#ifdef CONFIG_HEAP_USE_PSRAM
        {
            wm_device_t *psram_dev;
            uint32_t psram_size;
            extern uint32_t _epsram;

            psram_dev = wm_drv_psram_init("psram");
            if (wm_drv_psram_get_size(psram_dev, &psram_size) != WM_ERR_SUCCESS) {
                wm_log_error("Fail to detect PSRAM!");
                return WM_ERR_FAILED;
            }

            /* psram heap start from the end of variable in psram to the end of psram */
            memory->start_address = (uint32_t)(&_epsram);
            memory->size_in_bytes = psram_size - (memory->start_address - CONFIG_PSRAM_BASE_ADDR);

            wm_log_debug("psram start:0x%x, size=0x%x", memory->start_address, memory->size_in_bytes);

            if (memory->size_in_bytes <= HEAP_MINIMUM_BLOCK_SIZE) {
                wm_log_error("Not enough PSRAM space[%d] for HEAP!", memory->size_in_bytes);
                return WM_ERR_FAILED;
            }

            heap_init(memory);

            wm_heaps.free_bytes_remaining += memory->heap.free_bytes_remaining;
            wm_heaps.minimum_ever_free_bytes_remaining = wm_heaps.free_bytes_remaining;

            memory->caps &= ~WM_HEAP_CAP_INVALID;
        }
#endif
    }
    return WM_ERR_SUCCESS;
}

#ifndef CONFIG_HEAP_USE_TRACING
void *wm_heap_caps_alloc_tracing(size_t size, wm_heap_cap_type_t caps, const char *file, int line)
{
    return wm_heap_caps_alloc(size, caps);
}

void *wm_heap_caps_realloc_tracing(void *old_mem, size_t new_size, wm_heap_cap_type_t caps, const char *file, int line)
{
    return wm_heap_caps_realloc(old_mem, new_size, caps);
}
#endif //CONFIG_HEAP_USE_TRACING
