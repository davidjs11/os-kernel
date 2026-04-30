/*   kernel/mm/vmm/vmm_object.c   */

/* Virtual memory object management. */

#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <kernel/arch/paging.h>
#include <kernel/kheap.h>
#include <kernel/types.h>

static int vmm_map_phys(
    vmm_space_t *space,
    vmm_region_t *region,
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

static int vmm_map_anon(
    vmm_space_t *space,
    vmm_region_t *region,
    vaddr_t addr,
    vmm_prot_t flags)
{
    /* Obtain a new physical page frame. */
    paddr_t new_page = pmm_alloc();
    if (new_page == 0) {
        return -1;
    }

    /* Map it to virtual memory space. */
    arch_paging_map(
        space->pt_phys,
        addr,
        new_page,
        flags | PROT_PRESENT
    );

    return 0;
}

static void unmap_phys(vmm_space_t *space, vmm_region_t *region)
{
    for (vaddr_t i = region->addr;
         i < region->addr + region->size;
         i += PAGE_SIZE) {
            arch_paging_unmap(space->pt_phys, i);
    }
}

static void unmap_anon(vmm_space_t *space, vmm_region_t *region)
{
    for (vaddr_t i = region->addr;
         i < region->addr + region->size;
         i += PAGE_SIZE) {
            pmm_free(arch_paging_get_paddr(space->pt_phys, i));
            arch_paging_unmap(space->pt_phys, i);
    }
}

vmm_object_ops_t object_ops_phys = {
    .map   = vmm_map_phys,
    .unmap = unmap_phys
};

vmm_object_ops_t object_ops_anon = {
    .map   = vmm_map_anon,
    .unmap = unmap_anon
};

static void vmm_object_init_phys(vmm_object_t *obj, paddr_t base, size_t size)
{
    obj->refcount = 1;
    obj->ops = &object_ops_phys;
    obj->phys.base = base;
    obj->phys.size = size;
}

vmm_object_t *vmm_object_create_phys(paddr_t base, size_t size)
{
    vmm_object_t *result = kmalloc(sizeof(*result));
    if (result != NULL) {
        vmm_object_init_phys(result, base, size);
    }

    return result;
}

static void vmm_object_init_anon(vmm_object_t *obj)
{
    obj->refcount = 1;
    obj->ops = &object_ops_anon;
}

vmm_object_t *vmm_object_create_anon()
{
    vmm_object_t *result = kmalloc(sizeof(*result));
    if (result != NULL) {
        vmm_object_init_anon(result);
    }

    return result;
}
