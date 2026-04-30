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

/*
 * @brief Set an interrupt handler that will be called by the generic
 *        dispatcher. These handlers won't need to worry about context-saving,
 *        or CPU registers, with the cost of a bit of overhead.
 *
 * @param handler A pointer to the handler function.
 */
void arch_interrupt_install_generic(void *(*handler)(void));

#endif /* KERNEL_API_ARCH_INTERRUPT_H */
