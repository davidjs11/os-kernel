/*   arch/aarch64/init.c   */

/* Architecture-specific initialization code implementation for AArch64. */

#include <kernel/arch/init.h>
#include <stdint.h>
#include "exception.h"

void arch_init(void)
{
    /*
     * Memory Attribute Indirection Register.
     *     Attr0 = 0xFF: Normal memory, inner/outer write-back write-allocate.
     */
    __asm__ volatile ("msr mair_el1, %0" : : "r" (0xFFULL) : "memory");

    /*
     * Translation Control Register: 48-bit virtual addresses and 4 KiB
     * granule for both TTBR0 (lower half) and TTBR1 (higher half), with
     * write-back cacheable and inner shareable page table walks.
     */
    uint64_t tcr = (16ULL << 0)     /* T0SZ:  48-bit VA for TTBR0.      */
                 | (1ULL  << 8)     /* IRGN0: Write-back write-alloc.   */
                 | (1ULL  << 10)    /* ORGN0: Write-back write-alloc.   */
                 | (3ULL  << 12)    /* SH0:   Inner shareable.          */
                 | (16ULL << 16)    /* T1SZ:  48-bit VA for TTBR1.      */
                 | (1ULL  << 24)    /* IRGN1: Write-back write-alloc.   */
                 | (1ULL  << 26)    /* ORGN1: Write-back write-alloc.   */
                 | (3ULL  << 28)    /* SH1:   Inner shareable.          */
                 | (2ULL  << 30);   /* TG1:   4 KiB granule (TG0 = 0).  */
    __asm__ volatile (
        "msr tcr_el1, %0\n"
        "isb\n"
        :
        : "r" (tcr)
        : "memory"
    );

    exception_load_vector_table();
}
