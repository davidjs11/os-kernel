/*   include/arch.h   */

/* Architecture-specific HAL. */

#ifndef KERNEL_API_ARCH_HAL_H
#define KERNEL_API_ARCH_HAL_H

/*
 * @brief Initialize architecture-specific structs.
 *
 * Implemented by <arch>/init.c.
 */
void arch_init(void);

#endif /* KERNEL_API_ARCH_HAL_H */
