/*   arch/x86_64/pgtable.h   */

/*
 * x86-64 page table definitions. Paging interface is defined in
 * architecture-specific kernel public API and implemented by x86_64/paging.c.
 *
 * References
 *   Intel SDM Vol. 3A, Chapter 5 - Paging.
 */

#ifndef ARCH_X86_64_PGTABLE_H
#define ARCH_X86_64_PGTABLE_H

#include <stddef.h>
#include <stdint.h>
#include <kernel/types.h>

typedef uint64_t pte_t;

/* Page Table Entry flags (SDM Vol. 3A Fig. 5-7). */
#define PTE_PRESENT  (1ULL << 0)  /* Page is present.                 */
#define PTE_WRITE    (1ULL << 1)  /* Writing is allowed.              */
#define PTE_USER     (1ULL << 2)  /* User-mode access is allowed.     */
#define PTE_PWT      (1ULL << 3)  /* Page-level write through.        */
#define PTE_PCD      (1ULL << 4)  /* Don't cache this page.           */
#define PTE_ACCESSED (1ULL << 5)  /* Page has been accessed.          */
#define PTE_DIRTY    (1ULL << 6)  /* Page has been written.           */
#define PTE_HUGE     (1ULL << 7)  /* Page Size - 2MB/1GB pages.       */
#define PTE_GLOBAL   (1ULL << 8)  /* Not flushed on CR3 reload.       */
#define PTE_NX       (1ULL << 63) /* No execute. Requires EFER.NXE=1. */

/* Page Fault error code flags (SDM Vol. 3A Fig. 7-11). */
#define PF_ERROR_PRESENT      (1ULL << 0)  /* Page is present.                */
#define PF_ERROR_WRITE        (1ULL << 1)  /* Page was tried to be writen.    */
#define PF_ERROR_USER         (1ULL << 2)  /* Page was accessed on user-mode. */
#define PF_ERROR_RESERVED     (1ULL << 3)  /* Reserved bit was set in a page. */
#define PF_ERROR_FETCH        (1ULL << 4)  /* A fetch occured on the page.    */
#define PF_ERROR_PROT_KEY     (1ULL << 5)  /* Page protection key violation.  */
#define PF_ERROR_SHADOW_STACK (1ULL << 6)  /* Shadow-stack access performed.  */
#define PF_ERROR_HUGE         (1ULL << 7)  /* HLAT paging.                    */
#define PF_ERROR_GLOBAL       (1ULL << 15) /* SGX access-control violated.    */

/*
 * @brief Extract the physical address of a page entry
 *
 * @param entry Content of the entry on the Page Table.
 */
#define PTE_ADDR(entry) ((entry) & ~0xfffULL & ~PTE_NX)

/* Virtual address decomposition. */
#define PML4_INDEX(virt) (((virt) >> 39) & 0x1ff)
#define PDPT_INDEX(virt) (((virt) >> 30) & 0x1ff)
#define PD_INDEX(virt)   (((virt) >> 21) & 0x1ff)
#define PT_INDEX(virt)   (((virt) >> 12) & 0x1ff)

#endif /* ARCH_X86_64_PGTABLE_H */
