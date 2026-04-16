/*   kernel/pmm.c   */

/* Kernel physical memory manager. */

#include <platform/mem.h>
#include <arch/paging.h>
#include <pmm.h>

static pmm_ops_t *pmm_ops = PMM_OPS_DEFAULT;

void pmm_init(pmm_ops_t *ops)
{
    mmap_entry_t *mmap = platform_mem_get_mmap();

    /* Initialize PMM algorithm. */
    if (ops != NULL) {
        pmm_ops = ops;
    }
    pmm_ops->init(platform_mem_get_hhdm_vaddr());

    /* Build free frame list. */
    for (size_t i = 0; mmap[i].base != 0 && mmap[i].size != 0; i++) {
        /* Skip unusable regions. */
        if (mmap[i].type != MMAP_USABLE) {
            continue;
        }

        paddr_t start = mmap[i].base;
        paddr_t end = start + mmap[i].size;
        for (paddr_t page = start; page < end; page += PAGE_SIZE) {
            pmm_ops->free(page);
        }
    }
}

paddr_t pmm_alloc(void)
{
    paddr_t page = pmm_ops->alloc();
    if (page == 0) {
        /* TODO: panic. */
    }

    return page;
}

void pmm_free(paddr_t addr)
{
    pmm_ops->free(addr);
}

size_t pmm_available(void)
{
    return pmm_ops->available();
}
