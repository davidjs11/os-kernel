/*   kernel/pmm.c   */

/* Kernel physical memory manager. */

#include <platform/mem.h>
#include <pmm.h>
#include "freelist.h"

static uint64_t hh_offset = 0;
pmm_algorithm_t pmm_algorithm = PMM_ALGORITHM_FREELIST;

void pmm_init(void)
{
    pmm_algorithm.init();
}

uintptr_t pmm_alloc(void)
{
    return pmm_algorithm.alloc();
}

void pmm_free(uintptr_t addr)
{
    pmm_algorithm.free(addr);
}

size_t pmm_available(void)
{
    return pmm_algorithm.available();
}
