/*   kernel/pmm/freelist.c   */

/* Freelist physical memory manager algorithm implementation. */

#include "freelist.h"

static uint64_t kernel_hhdm_offset = 0;
static uintptr_t list_top = 0;
static size_t available_pages = 0;

void pmm_freelist_init(uint64_t hhdm_offset)
{
    kernel_hhdm_offset = hhdm_offset;
}

uintptr_t pmm_freelist_alloc(void)
{
    uintptr_t addr = list_top;

    /* Out of memory check. */
    if (addr == 0) {
        return 0;
    }

    /* Top of the list now points to the next entry */
    list_top = *((uintptr_t *) (list_top + kernel_hhdm_offset));
    available_pages--;

    return addr;
}

void pmm_freelist_free(uintptr_t addr)
{
    /* NULL-pointer check. */
    if (addr == 0) {
        return;
    }

    *((uintptr_t *) (kernel_hhdm_offset + addr)) = list_top;
    list_top = addr;
    available_pages++;
}

size_t pmm_freelist_available(void)
{
    return available_pages;
}
