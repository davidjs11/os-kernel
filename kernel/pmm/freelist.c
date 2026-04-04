/*   kernel/pmm/freelist.c   */

/* Freelist physical memory manager algorithm implementation. */

#include "freelist.h"
#include <platform/mem.h>

static uint64_t hh_offset = 0;

void pmm_freelist_init(void)
{
    mmap_entry_t *mmap = platform_mem_get_mmap();

    hh_offset = platform_mem_get_hh_offset();

    /* Build free frame list. */
    for (size_t i = 0; mmap[i].base != 0 && mmap[i].size != 0; i++) {
        /* Skip unusable regions. */
        if (mmap[i].type != MMAP_USABLE) {
            continue;
        }

        uintptr_t start = mmap[i].base;
        uintptr_t end = start + mmap[i].size;

        for (uintptr_t page = start; page < end; page += PAGE_SIZE) {
            pmm_free(page);
        }
    }
}

uintptr_t pmm_freelist_alloc(void)
{
    /* TODO: Implement this. */
    return 0;
}

void pmm_freelist_free(uintptr_t addr)
{
    /* TODO: Implement this. */
}

size_t pmm_freelist_available(void)
{
    /* TODO: Implement this. */
    return 0;
}
