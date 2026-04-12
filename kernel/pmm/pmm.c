/*   kernel/pmm.c   */

/* Kernel physical memory manager. */

#include <platform/mem.h>
#include <arch/paging.h>
#include <pmm.h>
#include "freelist.h"

pmm_algorithm_t pmm_algorithm = _PMM_ALGORITHM_FREELIST;

void pmm_init(pmm_algorithm_name_t pmm_algorithm_name)
{
    mmap_entry_t *mmap = platform_mem_get_mmap();

    /* Initialize PMM algorithm. */
    switch (pmm_algorithm_name) {
        case PMM_ALGORITHM_FREELIST:
        default:
            pmm_algorithm = _PMM_ALGORITHM_FREELIST;
            break;
    }
    pmm_algorithm.init(platform_mem_get_hh_offset());

    /* Build free frame list. */
    for (size_t i = 0; mmap[i].base != 0 && mmap[i].size != 0; i++) {
        /* Skip unusable regions. */
        if (mmap[i].type != MMAP_USABLE) {
            continue;
        }

        uintptr_t start = mmap[i].base;
        uintptr_t end = start + mmap[i].size;
        for (uintptr_t page = start; page < end; page += PAGE_SIZE) {
            pmm_algorithm.free(page);
        }
    }
}

uintptr_t pmm_alloc(void)
{
    uintptr_t page = pmm_algorithm.alloc();
    if (page == 0) {
        /* TODO: panic. */
    }

    return page;
}

void pmm_free(uintptr_t addr)
{
    pmm_algorithm.free(addr);
}

size_t pmm_available(void)
{
    return pmm_algorithm.available();
}
