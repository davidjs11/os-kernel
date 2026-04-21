/*   kernel/mm/vmm/vmm_object.c   */

/* Virtual memory space management. */

#include <vmm.h>
#include <pmm.h>
#include <arch/paging.h>
#include <kheap.h>
#include <types.h>

vmm_space_t *vmm_space_create(void)
{
    vmm_space_t *space = kmalloc(sizeof(*space));
    if (space == NULL) {
        return NULL;
    }

    space->pt_phys = arch_paging_space_create();
    list_init(&space->regions);

    return space;
}

void vmm_space_delete(vmm_space_t *space)
{
    if (space == NULL) {
        return;
    }

    /* Delete all regions. */
    list_foreach_safe(node, tmp, &space->regions) {
        vmm_region_t *region = VMM_REGION_LIST_NODE(node);
        vmm_region_delete(region);
    }

    arch_paging_space_delete(space->pt_phys);
    kfree(space);
}

void vmm_space_add_region(vmm_space_t *space, vmm_region_t *region)
{
    region->space = space;

    /* Find insertion point sorted by addr, overlap check. */
    list_foreach(node, &space->regions) {
        vmm_region_t *existing = VMM_REGION_LIST_NODE(node);

        /* Overlap check. */
        if (region->addr < existing->addr + existing->size &&
            region->addr + region->size > existing->addr) {
            /* TODO: panic/assert, overlapping regions are a kernel bug. */
            return;
        }

        /* Insert before the first region with a higher base address. */
        if (region->addr < existing->addr) {
            list_insert_before(node, &region->node);
            return;
        }
    }

    /* No higher region found, append at the end. */
    list_push_back(&space->regions, &region->node);
}
