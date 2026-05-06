/*   arch/aarch64/paging.c   */

/* AArch64 paging mechanism, implementing kernel's public paging API. */

#include <kernel/arch/paging.h>
#include <kernel/platform/mem.h>
#include <kernel/libc/string.h>
#include <kernel/pmm.h>

/* Virtual address of the Higher Half Direct Mapping (HHDM) offset. */
static uint64_t hhdm_offset = 0;

static vmm_prot_t flags_pte_to_kernel(uint64_t flags)
{

}

static uint64_t flags_kernel_to_pte(vmm_prot_t flags)
{

}

void arch_paging_init()
{

}

paddr_t arch_paging_space_create(void)
{

}

void arch_paging_space_delete(paddr_t space)
{

}

void arch_paging_space_load(paddr_t space)
{

}

paddr_t arch_paging_space_get(void)
{

}

void arch_paging_map(
        paddr_t space,
        vaddr_t vaddr,
        paddr_t paddr,
        vmm_prot_t flags)
{

}

void arch_paging_unmap(paddr_t space, vaddr_t vaddr)
{

}

void arch_paging_map_range(
        paddr_t space,
        vaddr_t vaddr,
        paddr_t paddr,
        size_t num_pages,
        vmm_prot_t flags)
{

}

vmm_prot_t arch_paging_get_flags(paddr_t space, vaddr_t vaddr)
{

}

void arch_paging_set_flags(paddr_t space, vaddr_t vaddr, vmm_prot_t flags)
{

}

void arch_paging_flush_tlb(vaddr_t vaddr)
{

}

paddr_t arch_paging_get_paddr(paddr_t space, vaddr_t vaddr)
{

}
