/*   arch/x86_64/paging.c   */

/* x86_64 paging mechanism, implementing kernel's public paging API. */

#include <kernel/arch/paging.h>
#include <kernel/platform/mem.h>
#include <kernel/libc/string.h>
#include <kernel/pmm.h>
#include "pgtable.h"
#include "idt.h"

/* Virtual address of the Higher Half Direct Mapping (HHDM) offset. */
static uint64_t hhdm_offset = 0;

/*
 * @brief Obtain the contents of the CR2 register.
 *
 * @return Value of the CR2 register (virtual address that caused a page fault).
 */
static inline paddr_t cr2_read(void)
{
    uint64_t cr2;
    __asm__ volatile (
        "movq %%cr2, %0"
        : "=r" (cr2)
        :
        : "memory"
    );
    return cr2;
}

/*
 * @brief Obtain the contents of the CR3 register.
 *
 * @return Value of the CR3 register (physical address of active PML4).
 */
static inline paddr_t cr3_read(void)
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
    paddr_t new_addr;

    pml4 = (pte_t *) (PTE_ADDR(space) + hhdm_offset);
    if (!(pml4[PML4_INDEX(vaddr)] & PTE_PRESENT)) {
        new_addr = pmm_alloc();
        memset((void *) (new_addr + hhdm_offset), 0, PAGE_SIZE);
        pml4[PML4_INDEX(vaddr)] = new_addr | PTE_PRESENT | PTE_WRITE;
    }

    pdpt = (pte_t *) (PTE_ADDR(pml4[PML4_INDEX(vaddr)]) + hhdm_offset);
    if (!(pdpt[PDPT_INDEX(vaddr)] & PTE_PRESENT)) {
        new_addr = pmm_alloc();
        memset((void *) (new_addr + hhdm_offset), 0, PAGE_SIZE);
        pdpt[PDPT_INDEX(vaddr)] = new_addr | PTE_PRESENT | PTE_WRITE;
    }

    pd = (pte_t *) (PTE_ADDR(pdpt[PDPT_INDEX(vaddr)]) + hhdm_offset);
    if (!(pd[PD_INDEX(vaddr)] & PTE_PRESENT)) {
        new_addr = pmm_alloc();
        memset((void *) (new_addr + hhdm_offset), 0, PAGE_SIZE);
        pd[PD_INDEX(vaddr)] = new_addr | PTE_PRESENT | PTE_WRITE;
    }

    pt = (pte_t *) (PTE_ADDR(pd[PD_INDEX(vaddr)]) + hhdm_offset);

    return &pt[PT_INDEX(vaddr)];
}

static vmm_prot_t flags_pte_to_kernel(uint64_t flags)
{
    vmm_prot_t kernel_flags = 0;

    if (flags & PTE_PRESENT) kernel_flags |= PROT_PRESENT;
    if (flags & PTE_WRITE)   kernel_flags |= PROT_WRITE;
    if (flags & PTE_USER)    kernel_flags |= PROT_USER;
    if (flags & PTE_DIRTY)   kernel_flags |= PROT_DIRTY;
    if (flags & PTE_HUGE)    kernel_flags |= PROT_HUGE;
    if (!(flags & PTE_NX))   kernel_flags |= PROT_EXEC;

    return kernel_flags;
}

static uint64_t flags_kernel_to_pte(vmm_prot_t flags)
{
    uint64_t pte_flags = 0;

    if (flags & PROT_PRESENT) pte_flags |= PTE_PRESENT;
    if (flags & PROT_WRITE)   pte_flags |= PTE_WRITE;
    if (flags & PROT_USER)    pte_flags |= PTE_USER;
    if (flags & PROT_DIRTY)   pte_flags |= PTE_DIRTY;
    if (flags & PROT_HUGE)    pte_flags |= PTE_HUGE;
    if (!(flags & PROT_EXEC)) pte_flags |= PTE_NX;

    return pte_flags;
}

void pf_handler(interrupt_frame_t *frame)
{
    uint64_t err = frame->error_code;

    vmm_handle_page_fault(
        cr2_read(),
        err & PF_ERROR_PRESENT,
        err & PF_ERROR_WRITE,
        err & PF_ERROR_USER,
        err & PF_ERROR_FETCH
    );
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

    return flags_pte_to_kernel((*pte) & (PTE_NX | 0xfffUL));
}

void arch_paging_set_flags(paddr_t space, vaddr_t vaddr, vmm_prot_t flags)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    *pte = ((*pte) & ~(PTE_NX | 0xfffUL)) | flags_kernel_to_pte(flags);
}

void arch_paging_flush_tlb(vaddr_t vaddr)
{
    flush_tlb(vaddr);
}

paddr_t arch_paging_get_paddr(paddr_t space, vaddr_t vaddr)
{
    pte_t *pte = pgtable_get_pte(space, vaddr);
    return (*pte) & 0x000ffffffffff000ULL;
}
