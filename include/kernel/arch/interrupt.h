/*   include/arch/interrupt.h   */

#ifndef KERNEL_API_ARCH_INTERRUPT_H
#define KERNEL_API_ARCH_INTERRUPT_H

/*
 * @brief Initialize interrupts and install a generic handler for each
 *        interrupt source.
 */
void arch_interrupt_init(void);

/*
 * @brief Enable interrupts.
 */
void arch_interrupt_enable(void);

/*
 * @brief Disable interrupts.
 */
void arch_interrupt_disable(void);

#endif /* KERNEL_API_ARCH_INTERRUPT_H */
