/*   arch/aarch64/pgtable.h   */

/*
 * AArch64 page table definitions (4 KiB granule, 48-bit virtual addresses, 4
 * levels). Paging interface is defined in architecture-specific kernel public
 * API and implemented by aarch64/paging.c.
 *
 * References
 *   Arm Architecture Reference Manual for A-profile (DDI 0487), Chapter D8.
 */

#ifndef ARCH_AARCH64_PGTABLE_H
#define ARCH_AARCH64_PGTABLE_H

#include <stdint.h>
#include <kernel/types.h>

typedef uint64_t pte_t;

/* Descriptor type (bits 1:0). */
#define PTE_VALID           (1ULL << 0)  /* Valid descriptor.               */
#define PTE_TABLE           (1ULL << 1)  /* L0-L2: table. L3: page.         */
#define PTE_PAGE            (PTE_VALID | PTE_TABLE)

/* Memory attribute index in MAIR_EL1 (bits 4:2). */
#define PTE_MATTR(n)        (((uint64_t) (n) & 0x7) << 2)
#define PTE_MATTR_NORMAL    PTE_MATTR(0) /* Normal memory (see arch_init).  */

/* Access permissions AP[2:1] (bits 7:6). */
#define PTE_AP_RW_KERN      (0ULL << 6)  /* EL1 RW, EL0 none.               */
#define PTE_AP_RW_ALL       (1ULL << 6)  /* EL1 RW, EL0 RW.                 */
#define PTE_AP_RO_KERN      (2ULL << 6)  /* EL1 RO, EL0 none.               */
#define PTE_AP_RO_ALL       (3ULL << 6)  /* EL1 RO, EL0 RO.                 */

/* Shareability (bits 9:8). */
#define PTE_SH_INNER        (3ULL << 8)  /* Inner shareable.                */

/* Access flag. */
#define PTE_AF              (1ULL << 10)

/* Execute never. */
#define PTE_PXN             (1ULL << 53) /* EL1 execute never.              */
#define PTE_UXN             (1ULL << 54) /* EL0 execute never.              */

/* Output address of a descriptor. */
#define PTE_ADDR(entry)     ((entry) & 0x0000FFFFFFFFF000ULL)

/* Table indexes of a virtual address. */
#define L0_INDEX(va)        (((va) >> 39) & 0x1ffUL)
#define L1_INDEX(va)        (((va) >> 30) & 0x1ffUL)
#define L2_INDEX(va)        (((va) >> 21) & 0x1ffUL)
#define L3_INDEX(va)        (((va) >> 12) & 0x1ffUL)

#endif /* ARCH_AARCH64_PGTABLE_H */
