/*   arch/aarch64/paging.c   */

/* AArch64 paging mechanism, implementing kernel's public paging API. */

#include <kernel/arch/paging.h>
#include <kernel/platform/mem.h>
#include <kernel/libc/string.h>
#include <kernel/pmm.h>
#include "pgtable.h"

/* Virtual address of the Higher Half Direct Mapping (HHDM) offset. */
static uint64_t hhdm_offset = 0;

/*
 * @brief Flush a single TLB entry (inner shareable domain).
 *
 * @param virt Virtual address to invalidate.
 */
static inline void flush_tlb(vaddr_t virt)
{
    __asm__ volatile (
        "dsb  ishst\n"
        "tlbi vale1is, %0\n"
        "dsb  ish\n"
        "isb\n"
        :
        : "r" (virt >> 12)
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
    pte_t *l0, *l1, *l2, *l3;
    paddr_t new_addr;

    l0 = (pte_t *) (PTE_ADDR(space) + hhdm_offset);
    if (!(l0[L0_INDEX(vaddr)] & PTE_VALID)) {
        new_addr = pmm_alloc();
        memset((void *) (new_addr + hhdm_offset), 0, PAGE_SIZE);
        l0[L0_INDEX(vaddr)] = new_addr | PTE_TABLE | PTE_VALID;
    }

    l1 = (pte_t *) (PTE_ADDR(l0[L0_INDEX(vaddr)]) + hhdm_offset);
    if (!(l1[L1_INDEX(vaddr)] & PTE_VALID)) {
        new_addr = pmm_alloc();
        memset((void *) (new_addr + hhdm_offset), 0, PAGE_SIZE);
        l1[L1_INDEX(vaddr)] = new_addr | PTE_TABLE | PTE_VALID;
    }

    l2 = (pte_t *) (PTE_ADDR(l1[L1_INDEX(vaddr)]) + hhdm_offset);
    if (!(l2[L2_INDEX(vaddr)] & PTE_VALID)) {
        new_addr = pmm_alloc();
        memset((void *) (new_addr + hhdm_offset), 0, PAGE_SIZE);
        l2[L2_INDEX(vaddr)] = new_addr | PTE_TABLE | PTE_VALID;
    }

    l3 = (pte_t *) (PTE_ADDR(l2[L2_INDEX(vaddr)]) + hhdm_offset);
    return &l3[L3_INDEX(vaddr)];
}

static vmm_prot_t flags_pte_to_kernel(uint64_t flags)
{
    vmm_prot_t prot = 0;
    uint64_t ap = flags & PTE_AP_RO_ALL;

    if (flags & PTE_VALID)                           prot |= PROT_PRESENT;
    if (ap == PTE_AP_RW_KERN || ap == PTE_AP_RW_ALL) prot |= PROT_WRITE;
    if (ap == PTE_AP_RW_ALL || ap == PTE_AP_RO_ALL)  prot |= PROT_USER;
    if (!(flags & PTE_UXN) || !(flags & PTE_PXN))    prot |= PROT_EXEC;

    return prot;
}

static uint64_t flags_kernel_to_pte(vmm_prot_t flags)
{
    /* Valid page, normal memory, inner shareable, already accessed. */
    uint64_t pte_flags = PTE_PAGE | PTE_MATTR_NORMAL | PTE_SH_INNER | PTE_AF;

    /* Access permissions. */
    if (flags & PROT_WRITE) {
        pte_flags |= (flags & PROT_USER) ? PTE_AP_RW_ALL : PTE_AP_RW_KERN;
    } else {
        pte_flags |= (flags & PROT_USER) ? PTE_AP_RO_ALL : PTE_AP_RO_KERN;
    }

    /*
     * Unlike x86_64, there is an execute never bit for each exception level.
     * Kernel never executes user pages and user never executes kernel pages.
     */
    if (!(flags & PROT_EXEC)) {
        pte_flags |= PTE_UXN | PTE_PXN;
    } else if (flags & PROT_USER) {
        pte_flags |= PTE_PXN;
    } else {
        pte_flags |= PTE_UXN;
    }

    return pte_flags;
}

void arch_paging_init()
{
    hhdm_offset = platform_mem_get_hhdm_vaddr();
}

paddr_t arch_paging_space_create(void)
{
    paddr_t space = pmm_alloc();
    memset((void *) (space + hhdm_offset), 0, PAGE_SIZE);
    return space;
}

void arch_paging_space_delete(paddr_t space)
{
    pte_t *l0, *l1, *l2;
    size_t l0_idx, l1_idx, l2_idx;

    /* L0. */
    l0 = (pte_t *) (PTE_ADDR(space) + hhdm_offset);
    for (l0_idx = 0; l0_idx < 512; l0_idx++) {
        if (!(l0[l0_idx] & PTE_VALID)) continue;

        /* L1. */
        l1 = (pte_t *) (PTE_ADDR(l0[l0_idx]) + hhdm_offset);
        for (l1_idx = 0; l1_idx < 512; l1_idx++) {
            if (!(l1[l1_idx] & PTE_VALID)) continue;

            /* L2. */
            l2 = (pte_t *) (PTE_ADDR(l1[l1_idx]) + hhdm_offset);
            for (l2_idx = 0; l2_idx < 512; l2_idx++) {
                if (!(l2[l2_idx] & PTE_VALID)) continue;
                pmm_free(PTE_ADDR(l2[l2_idx]));
            }
            pmm_free(PTE_ADDR(l1[l1_idx]));
        }
        pmm_free(PTE_ADDR(l0[l0_idx]));
    }
    pmm_free(PTE_ADDR(space));
}

void arch_paging_space_load(paddr_t space)
{
    /*
     * Kernel space lives in TTBR1 (higher half). Page table writes must be
     * visible to the table walker before switching.
     */
    __asm__ volatile (
        "dsb  ish\n"
        "msr  ttbr1_el1, %0\n"
        "isb\n"
        "tlbi vmalle1is\n"
        "dsb  ish\n"
        "isb\n"
        :
        : "r" (space)
        : "memory"
    );
}

paddr_t arch_paging_space_get(void)
{
    uint64_t ttbr1;
    __asm__ volatile (
        "mrs %0, ttbr1_el1"
        : "=r" (ttbr1)
    );
    return ttbr1 & ~0xFFFULL;
}

void arch_paging_map(
        paddr_t space,
        vaddr_t vaddr,
        paddr_t paddr,
        vmm_prot_t flags)
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
        vmm_prot_t flags)
{
    size_t offset = 0;
    while (num_pages--) {
        arch_paging_map(space, vaddr + offset, paddr + offset, flags);
        offset += PAGE_SIZE;
    }
}

vmm_prot_t arch_paging_get_flags(paddr_t space, vaddr_t vaddr)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    return flags_pte_to_kernel(*pte);
}

void arch_paging_set_flags(paddr_t space, vaddr_t vaddr, vmm_prot_t flags)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    *pte = PTE_ADDR(*pte) | flags_kernel_to_pte(flags);
    flush_tlb(vaddr);
}

void arch_paging_flush_tlb(vaddr_t vaddr)
{
    flush_tlb(vaddr);
}

paddr_t arch_paging_get_paddr(paddr_t space, vaddr_t vaddr)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    if (!(*pte & PTE_VALID)) {
        return 0;
    }
    return PTE_ADDR(*pte);
}
