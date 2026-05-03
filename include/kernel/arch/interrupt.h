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
 * @brief Set an interrupt handler for a given interrupt vector.
 *
 * @param vector  The vector number of the interrupt.
 * @param handler A pointer to the handler function.
 */
void arch_interrupt_set_handler(uint8_t vector, void (*handler)(uint8_t irq));

#endif /* KERNEL_API_ARCH_INTERRUPT_H */
