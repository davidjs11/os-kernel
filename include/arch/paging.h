/*   include/arch/paging.h   */

/*
 * Paging kernel public API. This interface must be implemented by each
 * architecture.
 */

#ifndef KERNEL_API_ARCH_PAGING_H
#define KERNEL_API_ARCH_PAGING_H

#include <types.h>

/* Page macros. */
#define PAGE_SIZE             4096UL
#define PAGE_SHIFT            12
#define PAGE_ALIGN(addr)      (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~(PAGE_SIZE - 1))

/* Kernel-specific page flags. */
typedef uint32_t mmu_flags_t;

#define MMU_READ     (1 << 0) /* Read.           */
#define MMU_WRITE    (1 << 1) /* Write.          */
#define MMU_EXEC     (1 << 2) /* Execute.        */
#define MMU_USER     (1 << 3) /* User-space.     */
#define MMU_HUGE     (1 << 4) /* Huge page size. */
#define MMU_PRESENT  (1 << 5) /* Present.        */
#define MMU_DIRTY    (1 << 6) /* Dirty.          */

/*
 * @brief Initialize paging.
 */
void arch_paging_init(void);

/*
 * @brief Create a new address space and get its address.
 *        TODO: see if really necessary. For the moment this only will call
 *        pmm_alloc(). Maybe this task can be done by VMM.
 *
 * @return The address of the new space.
 */
paddr_t arch_paging_space_create(void);

/*
 * @brief Delete an address space.
 */
void arch_paging_space_delete(paddr_t space);

/*
 * @brief Map a virtual page to a physical page frame inside a certain address
 *        space.
 *
 * @param space Address space in which the mapping will be applied.
 * @param vaddr Virtual address of the page.
 * @param paddr Physical address of the page frame.
 * @param flags Flags to apply to the mapping.
 */
void arch_paging_map(
        paddr_t space,
        vaddr_t vaddr,
        paddr_t paddr,
        mmu_flags_t flags);

/*
 * @brief Unmap a virtual page from its associated physical page frame inside a
 *        certain address space.
 *
 * @param space Address space in which the mapping will be applied.
 * @param vaddr Virtual address of the page.
 */
void arch_paging_unmap(paddr_t space, vaddr_t vaddr);

/*
 * @brief Map a contiguous range of virtual pages to a contiguous range
 *        physical page frame inside a certain address space.
 *
 * @param space     Address space in which the mapping will be applied.
 * @param vaddr     Virtual address of the page.
 * @param paddr     Physical address of the page frame.
 * @param num_pages Size of the range in pages.
 * @param flags     Flags to apply to the mapping.
 */
void arch_paging_map_range(
        paddr_t space,
        vaddr_t vaddr,
        paddr_t paddr,
        size_t num_pages,
        mmu_flags_t flags);

/*
 * @brief Load an address space.
 *
 * @param space Physical address of the space to load.
 */
void arch_paging_space_load(paddr_t space);

/*
 * @brief Get the current address space.
 *
 * @return The physical address of the space that is loaded.
 */
paddr_t arch_paging_space_get(void);

/*
 * @brief Get the flags of a certain page.
 *
 * @param vaddr The virtual address of the page.
 *
 * @return The flags of the page.
 */
mmu_flags_t arch_paging_get_flags(paddr_t space, vaddr_t vaddr);

/*
 * @brief Set the flags of a certain page.
 *
 * @param vaddr The virtual address of the page.
 * @param flags The flags to be set.
 */
void arch_paging_set_flags(paddr_t space, vaddr_t vaddr, mmu_flags_t flags);

/*
 * @brief Flush an entry in the TLB.
 *
 * @param vaddr The virtual address to flush from the TLB.
 */
void arch_paging_flush_tlb(vaddr_t vaddr);

#endif /* KERNEL_API_PAGING_H */
