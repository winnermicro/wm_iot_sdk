/*
 * Copyright (c) 2016 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <sys/check.h>
#include <stdint.h>

/**
 * @brief Initialize kernel memory slab subsystem.
 *
 * Perform any initialization of memory slabs that wasn't done at build time.
 * Currently this just involves creating the list of free blocks for each slab.
 *
 * @retval 0 on success.
 * @retval -EINVAL if @p slab contains invalid configuration and/or values.
 */
static int create_free_list(struct k_mem_slab *slab)
{
	uint32_t j;
	char *p;

	/* blocks must be word aligned */
	CHECKIF(((slab->block_size | (uint32_t)slab->buffer) &
				(sizeof(void *) - 1)) != 0U) {
		return -EINVAL;
	}

	slab->free_list = NULL;
	p = slab->buffer;

	for (j = 0U; j < slab->num_blocks; j++) {
		*(char **)p = slab->free_list;
		slab->free_list = p;
		p += slab->block_size;
	}
	return 0;
}

int k_mem_slab_init(struct k_mem_slab *slab, void *buffer,
		    size_t block_size, uint32_t num_blocks)
{
	int rc = 0;

    sys_dlist_init(&slab->wait_q);
	slab->num_blocks = num_blocks;
	slab->block_size = block_size;
	slab->buffer = (char *)k_malloc((num_blocks) * WB_UP(block_size));
    __ASSERT_NO_MSG(slab->buffer);
	slab->num_used = 0U;

#ifdef CONFIG_MEM_SLAB_TRACE_MAX_UTILIZATION
	slab->max_used = 0U;
#endif

	rc = create_free_list(slab);
	if (rc < 0) {
		goto out;
	}
out:

	return rc;
}

int k_mem_slab_deinit(struct k_mem_slab *slab)
{

    if(slab == NULL) return -EINVAL;

    if(slab->buffer) {
        k_free(slab->buffer);
        slab->buffer = NULL;
    }

    return 0;
}

int k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, k_timeout_t timeout)
{
	int result = 0;
    uint32_t key = k_irq_lock();

	if (slab->free_list != NULL) {
		/* take a free block */
		*mem = slab->free_list;
		slab->free_list = *(char **)(slab->free_list);
		slab->num_used++;

#ifdef CONFIG_MEM_SLAB_TRACE_MAX_UTILIZATION
		slab->max_used = MAX(slab->num_used, slab->max_used);
#endif
		result = 0;
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT) ||
		   !IS_ENABLED(CONFIG_MULTITHREADING)) {
		/* don't wait for a free block to become available */
		*mem = NULL;
		result = -ENOMEM;
	} else {
        *mem = NULL;
        result = -ENOMEM;
	}
	k_irq_unlock(key);

	return result;
}

void k_mem_slab_free(struct k_mem_slab *slab, void *mem)
{
	__ASSERT(((char *)mem >= slab->buffer) &&
		 ((((char *)mem - slab->buffer) % slab->block_size) == 0) &&
		 ((char *)mem <= (slab->buffer + (slab->block_size * (slab->num_blocks - 1)))),
		 "Invalid memory pointer provided");
    uint32_t key = k_irq_lock();

	*(char **) mem = slab->free_list;
	slab->free_list = (char *) mem;
	slab->num_used--;

	k_irq_unlock(key);
}


#ifdef CONFIG_MEM_SLAB_TRACE_MAX_UTILIZATION
int k_mem_slab_runtime_stats_reset_max(struct k_mem_slab *slab)
{
	if (slab == NULL) {
		return -EINVAL;
	}

	uint32_t key = k_irq_lock();

	slab->max_used = slab->num_used;

	k_irq_unlock(key);

	return 0;
}
#endif
