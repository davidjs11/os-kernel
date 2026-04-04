/*   kernel/pmm/freelist.h   */

/* Freelist physical memory management algorithm. */

#ifndef KERNEL_API_PMM_FREELIST_H
#define KERNEL_API_PMM_FREELIST_H

#include <pmm.h>

#define _PMM_ALGORITHM_FREELIST (pmm_algorithm_t) {  \
    .init      = pmm_freelist_init,                 \
    .alloc     = pmm_freelist_alloc,                \
    .free      = pmm_freelist_free,                 \
    .available = pmm_freelist_available,            \
}

void pmm_freelist_init(uint64_t hh_offset);
uintptr_t pmm_freelist_alloc(void);
void pmm_freelist_free(uintptr_t addr);
size_t pmm_freelist_available(void);

#endif /* KERNEL_API_PMM_FREELIST_H */
