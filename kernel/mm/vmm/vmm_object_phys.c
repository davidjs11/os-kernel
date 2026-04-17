/*   kernel/mm/vmm/vmm_object_phys.c   */

/* Object API implementation for physical objects (physical memory). */

#include <arch/paging.h>
#include <types.h>
#include <vmm.h>

static int vmm_map_phys(
    vmm_region_t *region,
    vmm_space_t *space,
    vaddr_t addr,
    vmm_prot_t flags)
{
    register size_t offset = addr - region->addr + region->offset;

    arch_paging_map(
        space->pt_phys,
        addr,
        region->object->phys.base + offset,
        flags | PROT_PRESENT
    );

    return 0;
}

vmm_object_ops_t object_ops_phys = {
    .map   = vmm_map_phys,
    .unmap = NULL
};
