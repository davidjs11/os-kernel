/*   kernel/mm/vmm/vmm.c   */

/* Virtual memory manager implementation. */

#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <kernel/kheap.h>
#include <kernel/kprintf.h>
#include <kernel/arch/paging.h>
#include <kernel/platform/mem.h>
#include "vmm_layout.h"

#define KERNEL_REGION_HHDM          0
#define KERNEL_REGION_KERNEL_CODE   1
#define KERNEL_REGION_KERNEL_HEAP   2
#define VMM_STATIC_OBJECTS_MAX      3
#define VMM_STATIC_REGIONS_MAX      3

extern vmm_object_ops_t object_ops_anon;
extern vmm_object_ops_t object_ops_phys;

vmm_space_t  kernel_space;
static vmm_object_t kernel_objects[VMM_STATIC_OBJECTS_MAX];
static vmm_region_t kernel_regions[VMM_STATIC_REGIONS_MAX];
static vmm_space_t *current_space = NULL;

static void vmm_region_map(vmm_space_t *space, vmm_region_t *region)
{
    for (vaddr_t page_addr = region->addr;
        page_addr < region->addr + region->size;
        page_addr += PAGE_SIZE) {
            region->object->ops->map(space, region, page_addr, region->prot);
    }
}

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
        .size   = 0x100000000,
        .prot   = PROT_READ | PROT_WRITE | PROT_PRESENT,
    };

    list_init(&kernel_regions[KERNEL_REGION_HHDM].node);
    list_push_back(
        &kernel_space.regions,
        &kernel_regions[KERNEL_REGION_HHDM].node
    );

    kernel_objects[KERNEL_REGION_HHDM] = (vmm_object_t) {
        .refcount  = 1,
        .ops       = &object_ops_phys,
        .phys.base = 0x0,
        .phys.size = 0x100000000,
    };

    vmm_region_map(&kernel_space, &kernel_regions[KERNEL_REGION_HHDM]);
    
    /* Kernel heap. */
    kernel_regions[KERNEL_REGION_KERNEL_HEAP] = (vmm_region_t) {
        .addr   = KERNEL_LAYOUT_HEAP_BASE,
        .space  = &kernel_space,
        .object = &kernel_objects[KERNEL_REGION_KERNEL_HEAP],
        .offset = 0,
        .size   = KERNEL_LAYOUT_HEAP_SIZE,
        .prot   = PROT_READ | PROT_WRITE | PROT_PRESENT,
    };

    list_init(&kernel_regions[KERNEL_REGION_KERNEL_HEAP].node);
    list_push_back(
        &kernel_space.regions,
        &kernel_regions[KERNEL_REGION_KERNEL_HEAP].node
    );

    kernel_objects[KERNEL_REGION_KERNEL_HEAP] = (vmm_object_t) {
        .refcount  = 1,
        .ops       = &object_ops_anon,
    };

    /* Pre-allocate a piece of heap to avoid initial stack overflow. */
    for (vaddr_t va = KERNEL_LAYOUT_HEAP_BASE;
         va < KERNEL_LAYOUT_HEAP_BASE + (1024 * 1024); /* 1 MiB. */
         va += PAGE_SIZE) {
        paddr_t pa = pmm_alloc();
        arch_paging_map(
            kernel_space.pt_phys,
            va,
            pa,
            PROT_READ | PROT_WRITE | PROT_PRESENT
        );
    }

    /* Kernel image. */
    /* TODO: Split into .text, .data/.bss and .rodata */
    kernel_regions[KERNEL_REGION_KERNEL_CODE] = (vmm_region_t) {
        .addr   = platform_mem_get_kernel_image_vaddr(),
        .space  = &kernel_space,
        .object = &kernel_objects[KERNEL_REGION_KERNEL_CODE],
        .offset = 0,
        .size   = platform_mem_get_kernel_image_size(),
        .prot   = PROT_READ | PROT_WRITE | PROT_EXEC | PROT_PRESENT,
    };

    list_init(&kernel_regions[KERNEL_REGION_KERNEL_CODE].node);
    list_push_back(
        &kernel_space.regions,
        &kernel_regions[KERNEL_REGION_KERNEL_CODE].node
    );

    kernel_objects[KERNEL_REGION_KERNEL_CODE] = (vmm_object_t) {
        .refcount  = 1,
        .ops       = &object_ops_phys,
        .phys.base = platform_mem_get_kernel_image_paddr(),
        .phys.size = platform_mem_get_kernel_image_size(),
    };

    vmm_region_map(&kernel_space, &kernel_regions[KERNEL_REGION_KERNEL_CODE]);
}

void vmm_init(void)
{
    arch_paging_init();
    vmm_kernel_space_init();
    vmm_space_print_regions(&kernel_space);

    kprintf("[vmm] Initialized kernel address space.\n");

    /* Switch to new address space. */
    vmm_space_switch(&kernel_space);
    kprintf("[vmm] Page table switched.\n");

    kheap_init(
        kernel_regions[KERNEL_REGION_KERNEL_HEAP].addr,
        kernel_regions[KERNEL_REGION_KERNEL_HEAP].size,
        NULL
    );

    kprintf(
        "[vmm] Initialized kernel heap at 0x%x.\n",
        kernel_regions[KERNEL_REGION_KERNEL_HEAP].addr
    );
}

void vmm_space_switch(vmm_space_t *space)
{
    if (space == NULL) {
        return;
    }

    current_space = space;
    arch_paging_space_load(space->pt_phys);
}


vmm_region_t *vmm_space_find_region(vmm_space_t *space, vaddr_t addr)
{
    list_foreach(region_node, &space->regions) {
        vmm_region_t *region = VMM_REGION_LIST_NODE(region_node);
        if (addr < region->addr) {
            return NULL; /* Regions are sorted. */
        }
        if (addr < region->addr + region->size) {
            return region;
        }
    }

    return NULL;
}

void vmm_space_print_regions(vmm_space_t *space)
{
    kprintf("Regions of the address space located at 0x%x:\n", space->pt_phys);

    list_foreach(region_node, &space->regions) {
        vmm_region_t *region = VMM_REGION_LIST_NODE(region_node);
        kprintf(
            " - 0x%x - 0x%x (size: %d bytes)\n",
            region->addr,
            region->addr + region->size,
            region->size
        );
    }
}

void vmm_handle_page_fault(
    vaddr_t addr,
    bool present,
    bool write,
    bool user,
    bool exec)
{
    vmm_region_t *region = vmm_space_find_region(current_space, addr);
    if (region == NULL) {
        /* TODO: Segfault. */
        for (;;);
    }

    /* TODO: Compare permissions against region. */
    region->object->ops->map(
        current_space,
        region,
        PAGE_ALIGN_DOWN(addr),
        region->prot
    );
}
