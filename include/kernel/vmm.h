/*   include/vmm.h   */

/* Virtual memory manager API. */

#ifndef KERNEL_API_VMM_H
#define KERNEL_API_VMM_H

#include <kernel/types.h>
#include <kernel/list.h>

/* Page macros. */
#define PAGE_SIZE             4096UL
#define PAGE_SHIFT            12
#define PAGE_ALIGN(addr)      (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~(PAGE_SIZE - 1))

/* Kernel-specific page flags. */
typedef uint32_t vmm_prot_t;

#define PROT_READ    (1 << 0) /* Read.           */
#define PROT_WRITE   (1 << 1) /* Write.          */
#define PROT_EXEC    (1 << 2) /* Execute.        */
#define PROT_USER    (1 << 3) /* User-space.     */
#define PROT_HUGE    (1 << 4) /* Huge page size. */
#define PROT_PRESENT (1 << 5) /* Present.        */
#define PROT_DIRTY   (1 << 6) /* Dirty.          */

/* Virtual memory address space. */
typedef struct vmm_space {
    paddr_t     pt_phys; /* Root page table's physical address. */
    list_node_t regions; /* List head of the regions.           */
} vmm_space_t;

/* Virtual memory object. */
typedef struct vmm_object {
    int                   refcount; /* Number of processes with the object. */
    struct vmm_object_ops *ops;     /* Operations of the object.            */
    union {
        /* Physical memory. */
        struct { paddr_t base; size_t size; } phys;

        /* Anonymous memory. */
        struct { /* anon state */ } anon;

        /* File-backed memory. */
        struct { int fd; size_t offset; } file;
    };
} vmm_object_t;

/* Virtual memory region. */
typedef struct vmm_region {
    vaddr_t                 addr;    /* Virtual address.                */
    vmm_space_t             *space;  /* Space this region belongs to.   */
    vmm_object_t            *object; /* Object of this region.          */
    size_t                  offset;  /* Offset into the object.         */
    size_t                  size;    /* Size of the region.             */
    vmm_prot_t              prot;    /* Memory protection flags.        */
    list_node_t             node;    /* Node on the regions list.       */
} vmm_region_t;

#define VMM_REGION_LIST_NODE(ptr) \
    ((vmm_region_t *) ((char *) (ptr) - offsetof(vmm_region_t, node)))

/* Operations performed for each memory region. */
typedef struct vmm_object_ops {
    /*
     * @brief Map the object into memory given a certain address inside virtual
     *        memory space.
     *
     * @param space  Space in which to apply the mapping.
     * @param region Region of the address to be mapped.
     * @param addr   Address to be mapped.
     * @param flags  Flags of the fault.
     *
     * @return Result of the operation (0 on succeed).
     */
    int (*map)(
        vmm_space_t *space,
        vmm_region_t *region,
        vaddr_t addr,
        vmm_prot_t flags);

    /*
     * @brief Handle a refcount = 0 of the memory object.
     *
     * @param space  Space in which to apply the unmapping.
     * @param region Region to be unmapped.
     */
    void (*unmap)(
        vmm_space_t *space,
        vmm_region_t *region);
} vmm_object_ops_t;

/*
 * @brief Handle a page fault.
 *
 * @param present Page is present.
 * @param write   Page was tried to be written.
 * @param user   Page was accessed on user-mode.
 * @param exec   Page was tried to be executed.
 */
void vmm_handle_page_fault(
    vaddr_t addr,
    bool present,
    bool write,
    bool user,
    bool exec);

/*
 * @brief Initialize paging and create kernel virtual memory address space.
 */
void vmm_init(void);

/*
 * @brief Create a new dynamically-allocated address space struct. Programmer
 *        is responsible of freeing it using vmm_space_delete().
 *
 * @return The new initialized address space.
 */
vmm_space_t *vmm_space_create(void);

/*
 * @brief Delete a dynamically-allocated address space struct.
 *
 * @param space The address space to be deleted.
 */
void vmm_space_delete(vmm_space_t *space);

/*
 * @brief Add a memory region to an address space.
 *
 * This function ensures these conditions:
 *  - New region doesn't overlap with other regions.
 *  - Regions is addred in address order.
 *
 * @param space  The address space in which to add the region.
 * @param region The region to add.
 *
 * @return -1 if the operation fails.
 */
void vmm_space_add_region(vmm_space_t *space, vmm_region_t *region);

/*
 * @brief Switch current address space.
 *
 * @param space The address space that will be loaded.
 */
void vmm_space_switch(vmm_space_t *space);

/*
 * @brief Create a new memory region.
 *
 * This function ensures these conditions:
 *  - Address is page-aligned.
 *  - Size is page-aligned.
 *  - Address is in canonical form.
 *
 * @param addr Starting address of the region.
 * @param size Size of the region in bytes.
 * @param prot Memory protection flags (R/W/X).
 *
 * @return A pointer to the new created region.
 */
vmm_region_t *vmm_region_create(vaddr_t addr, size_t size, vmm_prot_t prot);

/*
 * @brief Delete a memory region.
 *
 * @param region The region that will be deleted.
 */
void vmm_region_delete(vmm_region_t *region);

/*
 * @brief Get the region of a memory space that contains a certain virtual
 *        memory address.
 *
 * @param space Memory space in which the region will be searched.
 * @param addr  Virtual memory address whose region will be searched.
 *
 * @return A pointer to the memory region that contains the virtual address.
 */
vmm_region_t *vmm_space_find_region(vmm_space_t *space, vaddr_t addr);

/*
 * @brief Set the backing object of a region.
 * 
 * @param region Region whose object will be set.
 * @param obj    Object to set.
 */
void vmm_region_set_object(vmm_region_t *region, vmm_object_t *obj);

/*
 * @brief Grow a certain region.
 *
 * @param region   Region to grow.
 * @param new_size New size of the region.
 */
void vmm_region_grow(vmm_region_t *region, size_t new_size);

/*
 * @brief Create an anonymous memory object.
 *
 * @param base Physical address of the memory object base.
 * @param size Size in bytes of the memory object.
 *
 * @return A pointer to the anonymous memory object.
 */
vmm_object_t *vmm_object_create_phys(paddr_t base, size_t size);

/*
 * @brief Create an anonymous memory object.
 *
 * @return A pointer to the anonymous memory object.
 */
vmm_object_t *vmm_object_create_anon(void);

#endif /* KERNEL_API_VMM_H */
