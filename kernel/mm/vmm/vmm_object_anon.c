/*   kernel/mm/vmm/vmm_object_anon.c   */

/* Object API implementation for anonymous objects (on-demand memory). */

#include <arch/paging.h>
#include <types.h>
#include <vmm.h>
#include <pmm.h>

static int vmm_map_anon(
    vmm_region_t *region,
    vmm_space_t *space,
    vaddr_t addr,
    vmm_prot_t flags)
{
    paddr_t new_page = pmm_alloc();
    arch_paging_map(
        space->pt_phys,
        addr,
        new_page,
        flags | PROT_PRESENT);

    return 0;
}

vmm_object_ops_t object_ops_anon = {
    .map   = vmm_map_anon,
    .unmap = NULL
};

