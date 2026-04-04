/*   include/arch.h   */

/* Architecture-specific HAL. */

#ifndef KERNEL_API_ARCH_HAL_H
#define KERNEL_API_ARCH_HAL_H

#define PAGE_SIZE             4096UL
#define PAGE_SHIFT            12
#define PAGE_ALIGN(addr)      (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~(PAGE_SIZE - 1))

/*
 * @brief Initialize architecture-specific structs.
 *
 * Implemented by <arch>/init.c.
 */
void arch_init(void);

#endif /* KERNEL_API_ARCH_HAL_H */
