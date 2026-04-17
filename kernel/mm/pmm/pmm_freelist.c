/*   kernel/mm/pmm/pmm_freelist.c   */

/* Freelist physical memory manager algorithm implementation. */

#include <pmm.h>
#include <types.h>

static uint64_t kernel_hhdm_offset = 0;
static paddr_t list_top = 0;
static size_t available_pages = 0;

void pmm_freelist_init(uint64_t hhdm_offset)
{
    kernel_hhdm_offset = hhdm_offset;
}

paddr_t pmm_freelist_alloc(void)
{
    paddr_t addr = list_top;

    /* Out of memory check. */
    if (addr == 0) {
        return 0;
    }

    /* Top of the list now points to the next entry */
    list_top = *((paddr_t *) (list_top + kernel_hhdm_offset));
    available_pages--;

    return addr;
}

void pmm_freelist_free(paddr_t addr)
{
    /* NULL-pointer check. */
    if (addr == 0) {
        return;
    }

    *((paddr_t *) (kernel_hhdm_offset + addr)) = list_top;
    list_top = addr;
    available_pages++;
}

size_t pmm_freelist_available(void)
{
    return available_pages;
}

pmm_ops_t _pmm_algorithm_freelist = (pmm_ops_t) {
    .init      = pmm_freelist_init,
    .alloc     = pmm_freelist_alloc,
    .free      = pmm_freelist_free,
    .available = pmm_freelist_available
};
