/*   arch/x86_64/paging.c   */

/* x86_64 paging mechanism, implementing kernel's public paging API. */

#include <arch/paging.h>
#include <platform/mem.h>
#include <pmm.h>
#include "pgtable.h"

/* Physical address of the Higher Half Direct Mapping (HHDM) offset. */
static uint64_t hhdm_offset = 0;

/*
 * @brief Obtain the contents of the CR3 register.
 *
 * @return Value of the CR3 register (physical address of active PML4).
 */
static inline paddr_t cr3_read()
{
    uint64_t cr3;
    __asm__ volatile (
        "movq %%cr3, %0"
        : "=r" (cr3)
        :
        : "memory"
    );
    return cr3;
}

/*
 * @brief Write a value to the CR3 register.
 */
static inline void cr3_write(paddr_t cr3)
{
    __asm__ volatile (
        "movq %0, %%cr3"
        :
        : "r" (cr3)
        : "memory"
    );
}

/*
 * @brief Flush a single TLB entry.
 *
 * @param virt Virtual address to invalidate.
 */
static inline void flush_tlb(vaddr_t virt)
{
    __asm__ volatile (
        "invlpg (%0)"
        :
        : "r"(virt)
        : "memory"
    );
}

/*
 * @brief Walk through the page table and obtain the PTE of a virtual address.
 *
 * @return A pointer to the PTE entry for that virtual address.
 */
static pte_t *pgtable_get_pte(paddr_t space, vaddr_t vaddr)
{
    pte_t *pml4, *pdpt, *pd, *pt;

    pml4 = (pte_t *) (PTE_ADDR(space) + hhdm_offset);
    if (!(pml4[PML4_INDEX(vaddr)] & PTE_PRESENT)) {
        pml4[PML4_INDEX(vaddr)] = pmm_alloc() | PTE_PRESENT;
        /* TODO: memset() */
    }

    pdpt = (pte_t *) (PTE_ADDR(pml4[PML4_INDEX(vaddr)]) + hhdm_offset);
    if (!(pdpt[PDPT_INDEX(vaddr)] & PTE_PRESENT)) {
        pdpt[PDPT_INDEX(vaddr)] = pmm_alloc() | PTE_PRESENT;
        /* TODO: memset() */
    }

    pd = (pte_t *) (PTE_ADDR(pdpt[PDPT_INDEX(vaddr)]) + hhdm_offset);
    if (!(pd[PD_INDEX(vaddr)] & PTE_PRESENT)) {
        pd[PD_INDEX(vaddr)] = pmm_alloc() | PTE_PRESENT;
        /* TODO: memset() */
    }

    pt = (pte_t *) (PTE_ADDR(pd[PD_INDEX(vaddr)]) + hhdm_offset);

    return &pt[PT_INDEX(vaddr)];
}

static mmu_flags_t flags_pte_to_kernel(uint64_t flags)
{
    mmu_flags_t kernel_flags = 0;

    if (flags & PTE_PRESENT) kernel_flags |= MMU_PRESENT;
    if (flags & PTE_WRITE)   kernel_flags |= MMU_WRITE;
    if (flags & PTE_USER)    kernel_flags |= MMU_USER;
    if (flags & PTE_DIRTY)   kernel_flags |= MMU_DIRTY;
    if (flags & PTE_HUGE)    kernel_flags |= MMU_HUGE;
    if (!(flags & PTE_NX))   kernel_flags |= MMU_EXEC;

    return kernel_flags;
}

static uint64_t flags_kernel_to_pte(mmu_flags_t flags)
{
    uint64_t pte_flags = 0;

    if (flags & MMU_PRESENT) pte_flags |= PTE_PRESENT;
    if (flags & MMU_WRITE)   pte_flags |= PTE_WRITE;
    if (flags & MMU_USER)    pte_flags |= PTE_USER;
    if (flags & MMU_DIRTY)   pte_flags |= PTE_DIRTY;
    if (flags & MMU_HUGE)    pte_flags |= PTE_HUGE;
    if (!(flags & MMU_EXEC)) pte_flags |= PTE_NX;

    return pte_flags;
}

void arch_paging_init()
{
    hhdm_offset = platform_mem_get_hh_offset();
}

paddr_t arch_paging_space_create(void)
{
    paddr_t space = pmm_alloc();
    /* TODO: memset(0). */
    return space;
}

void arch_paging_space_delete(paddr_t space)
{
    pte_t *pml4, *pdpt, *pd;
    size_t pml4_idx, pdpt_idx, pd_idx, pt_idx;

    /* PML4 (Userspace half). */
    pml4 = (pte_t *) (PTE_ADDR(space) + hhdm_offset);
    for (pml4_idx = 0; pml4_idx < 256; pml4_idx++) {
        if (!(pml4[pml4_idx] & PTE_PRESENT)) continue;

        /* PDPT. */
        pdpt = (pte_t *) (PTE_ADDR(pml4[pml4_idx]) + hhdm_offset);
        for (pdpt_idx = 0; pdpt_idx < 512; pdpt_idx++) {
            if (!(pdpt[pdpt_idx] & PTE_PRESENT)) continue;

            /* PD. */
            pd = (pte_t *) (PTE_ADDR(pdpt[pdpt_idx]) + hhdm_offset);
            for (pd_idx = 0; pd_idx < 512; pd_idx++) {
                if (!(pd[pd_idx] & PTE_PRESENT)) continue;
                pmm_free(PTE_ADDR(pd[pd_idx]));
            }
            pmm_free(PTE_ADDR(pdpt[pdpt_idx]));
        }
        pmm_free(PTE_ADDR(pml4[pml4_idx]));
    }
    pmm_free(PTE_ADDR(space));
}

void arch_paging_space_load(paddr_t space)
{
    cr3_write(space);
}

paddr_t arch_paging_space_get(void)
{
    return cr3_read();
}

void arch_paging_map(
        paddr_t space,
        vaddr_t vaddr,
        paddr_t paddr,
        mmu_flags_t flags)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    *pte = PTE_ADDR(paddr) | flags_kernel_to_pte(flags);
}

void arch_paging_unmap(paddr_t space, vaddr_t vaddr)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    *pte = 0;
    flush_tlb(vaddr);
}

void arch_paging_map_range(
        paddr_t space,
        vaddr_t vaddr,
        paddr_t paddr,
        size_t num_pages,
        mmu_flags_t flags)
{
    size_t offset = 0;
    while (num_pages--) {
        arch_paging_map(space, vaddr + offset, paddr + offset, flags);
        offset += PAGE_SIZE;
    }
}

mmu_flags_t arch_paging_get_flags(paddr_t space, vaddr_t vaddr)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);

    return flags_pte_to_kernel((*pte) & (PTE_NX | 0xfffUL));
}

void arch_paging_set_flags(paddr_t space, vaddr_t vaddr, mmu_flags_t flags)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    *pte = ((*pte) & ~(PTE_NX | 0xfffUL)) | flags_kernel_to_pte(flags);
}

void arch_paging_flush_tlb(vaddr_t vaddr)
{
    flush_tlb(vaddr);
}
