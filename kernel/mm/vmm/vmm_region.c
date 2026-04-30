/*   kernel/mm/vmm/vmm_region.c   */

/* Virtual memory region management. */

#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <kernel/arch/paging.h>
#include <kernel/kheap.h>
#include <kernel/types.h>

vmm_region_t *vmm_region_create(vaddr_t addr, size_t size, vmm_prot_t prot)
{
    /* Ensure page alignment. */
    addr = PAGE_ALIGN_DOWN(addr);
    size = PAGE_ALIGN(size);

    vmm_region_t *region = kmalloc(sizeof(*region));
    if (region == NULL) {
        return NULL;
    }

    region->addr   = addr;
    region->size   = size;
    region->prot   = prot;
    region->offset = 0;
    region->object = NULL;
    region->space  = NULL;

    return region;
}

void vmm_region_delete(vmm_region_t *region)
{
    if (region == NULL) {
        return;
    }

    if (region->object != NULL) {
        region->object->refcount--;
        if (region->object->refcount == 0) {
            if (region->object->ops->unmap != NULL)
                region->object->ops->unmap(region->space, region);
            kfree(region->object);
        }
        region->object = NULL;
    }

    kfree(region);
}

void vmm_region_set_object(vmm_region_t *region, vmm_object_t *obj)
{
    if (region == NULL || obj == NULL) {
        return;
    }

    /* Drop reference to previous object if any. */
    if (region->object != NULL) {
        region->object->refcount--;
        if (region->object->refcount == 0) {
            region->object->ops->unmap(region->space, region);
            kfree(region->object);
        }
    }

    region->object = obj;
    if (obj != NULL) {
        obj->refcount++;
    }
}
