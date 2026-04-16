/*   kernel/vmm.c   */

/* Virtual memory manager implementation. */

#include <vmm.h>
#include <pmm.h>
#include <kheap.h>
#include <arch/paging.h>
#include <platform/mem.h>
#include "vm_layout.h"

#define KERNEL_REGION_HHDM          0
#define KERNEL_REGION_KERNEL_CODE   1
#define KERNEL_REGION_KERNEL_HEAP   2
#define VMM_STATIC_OBJECTS_MAX      3
#define VMM_STATIC_REGIONS_MAX      3

static int vmm_page_fault_phys(
            vmm_object_t *obj,
            vmm_space_t *space,
            vaddr_t addr,
            size_t offset,
            vmm_prot_t flags)
{
    arch_paging_map(
        space->pt_phys,
        addr + offset,
        obj->phys.base + offset,
        flags
    );

    return 0;
}

static int vmm_page_fault_anon(
            vmm_object_t *obj,
            vmm_space_t *space,
            vaddr_t addr,
            size_t offset,
            vmm_prot_t flags)
{
    paddr_t new_page = pmm_alloc();
    arch_paging_map(space->pt_phys, addr + offset, new_page, flags);

    return 0;
}

static void vmm_region_map(vmm_region_t *region) {
    size_t page_count = PAGE_ALIGN(region->object->size) / PAGE_SIZE;
    for (size_t i = 0; i < page_count; i++) {
        region->object->ops->fault(
            region->object,
            region->space,
            region->addr,
            PAGE_SIZE * i,
            region->prot
        );
    }
}

static vmm_space_t  kernel_space;
static vmm_object_t kernel_objects[VMM_STATIC_OBJECTS_MAX];
static vmm_region_t kernel_regions[VMM_STATIC_REGIONS_MAX];

static vmm_object_ops_t object_ops_phys = {
    .fault = vmm_page_fault_phys,
    .release = NULL
};

static vmm_object_ops_t object_ops_anon = {
    .fault = vmm_page_fault_anon,
    .release = NULL
};

static void vmm_kernel_space_init(void)
{
    kernel_space.pt_phys = arch_paging_space_create();
    list_init(&kernel_space.regions);

    /* HHDM region. */
    kernel_regions[KERNEL_REGION_HHDM] = (vmm_region_t) {
        .addr   = platform_mem_get_hhdm_vaddr(),
        .space  = &kernel_space,
        .object = &kernel_objects[KERNEL_REGION_HHDM],
        .offset = 0,
        .prot   = PROT_READ | PROT_WRITE,
    };

    list_init(&kernel_regions[KERNEL_REGION_HHDM].node);
    list_push_back(
        &kernel_space.regions,
        &kernel_regions[KERNEL_REGION_HHDM].node
    );

    kernel_objects[KERNEL_REGION_HHDM] = (vmm_object_t) {
        .refcount  = 1,
        .size      = platform_mem_get_size(),
        .ops       = &object_ops_phys,
        .phys.base = 0x0,
    };
    
    /* Kernel heap (between HHDM and kernel image). */
    vaddr_t heap_addr =
        platform_mem_get_hhdm_vaddr() + platform_mem_get_size();
    size_t heap_size = (size_t)
        (platform_mem_get_kernel_image_vaddr() - heap_addr);

    kernel_regions[KERNEL_REGION_KERNEL_HEAP] = (vmm_region_t) {
        .addr   = KERNEL_LAYOUT_HEAP_BASE,
        .space  = &kernel_space,
        .object = &kernel_objects[KERNEL_REGION_KERNEL_HEAP],
        .offset = 0,
        .prot   = PROT_READ | PROT_WRITE | PROT_PRESENT,
    };

    list_init(&kernel_regions[KERNEL_REGION_KERNEL_HEAP].node);
    list_push_back(
        &kernel_space.regions,
        &kernel_regions[KERNEL_REGION_KERNEL_HEAP].node
    );

    kernel_objects[KERNEL_REGION_KERNEL_HEAP] = (vmm_object_t) {
        .refcount  = 1,
        .size      = KERNEL_LAYOUT_HEAP_SIZE,
        .ops       = &object_ops_anon,
    };

    /* Kernel image. */
    /* TODO: Split into .text, .data/.bss and .rodata */
    kernel_regions[KERNEL_REGION_KERNEL_CODE] = (vmm_region_t) {
        .addr   = platform_mem_get_kernel_image_vaddr(),
        .space  = &kernel_space,
        .object = &kernel_objects[KERNEL_REGION_KERNEL_CODE],
        .offset = 0,
        .prot   = PROT_READ | PROT_WRITE | PROT_EXEC | PROT_PRESENT,
    };

    list_init(&kernel_regions[KERNEL_REGION_KERNEL_CODE].node);
    list_push_back(
        &kernel_space.regions,
        &kernel_regions[KERNEL_REGION_KERNEL_CODE].node
    );

    kernel_objects[KERNEL_REGION_KERNEL_CODE] = (vmm_object_t) {
        .refcount  = 1,
        .size      = platform_mem_get_kernel_image_size(),
        .ops       = &object_ops_phys,
        .phys.base = platform_mem_get_kernel_image_paddr(),
    };
}

void vmm_init(void)
{
    arch_paging_init();
    vmm_kernel_space_init();

    /* Map HHDM and kernel image regions. */
    vmm_region_map(&kernel_regions[KERNEL_REGION_HHDM]);
    vmm_region_map(&kernel_regions[KERNEL_REGION_KERNEL_CODE]);

    /* Switch to new address space. */
    arch_paging_space_load(kernel_space.pt_phys);

    kheap_init(
        kernel_regions[KERNEL_REGION_KERNEL_HEAP].addr,
        kernel_regions[KERNEL_REGION_KERNEL_HEAP].object->size,
        NULL
    );
}

/*
 * @brief Initialize a memory region.
 *
 * @param addr Starting address of the region.
 * @param size Size of the region in bytes.
 * @param prot Memory protection flags (R/W/X).
 */
void vmm_region_init(
    vmm_region_t *region,
    vaddr_t addr,
    size_t size,
    vmm_prot_t prot)
{
    /* TODO: Implement this. */
}

vmm_region_t *vmm_region_create(vaddr_t addr, size_t size, vmm_prot_t prot);
void vmm_region_delete(vmm_region_t *region);
void vmm_region_destroy(vmm_region_t *region);
