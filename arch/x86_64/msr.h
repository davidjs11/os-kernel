/*   arch/x86_64/msr.h   */

/* Machine-Specific Register management. */

#ifndef ARCH_X86_64_MSR_H
#define ARCH_X86_64_MSR_H

#include <stdint.h>

/* Register indexes. */
#define MSR_EFER           0xC0000080
#define MSR_STAR           0xC0000081
#define MSR_LSTAR          0xC0000082
#define MSR_SFMASK         0xC0000084
#define MSR_KERNEL_GS_BASE 0xC0000102

/*
 * @brief Read a MSR.
 *
 * @param msr MSR index.
 *
 * @return The value of the indicated MSR.
 */
static inline uint64_t msr_read(uint32_t msr)
{
    uint32_t lo, hi;
    __asm__ volatile (
        "rdmsr"
        : "=a"(lo), "=d"(hi)
        : "c"(msr)
    );
    return ((uint64_t)hi << 32) | lo;
}

/*
 * @brief Write a MSR.
 *
 * @param msr MSR index.
 * @param val Value to put into the MSR.
 */
static inline void msr_write(uint32_t msr, uint64_t val)
{
    __asm__ volatile (
        "wrmsr"
        :
        : "c"(msr), "a"((uint32_t)val), "d"((uint32_t)(val >> 32))
    );
}

#endif /* ARCH_X86_64_MSR_H */
