/*   include/kernel/interrupt.h   */

/* Kernel interrupt handling API. */

#ifndef KERNEL_API_INTERRUPT_H
#define KERNEL_API_INTERRUPT_H

#include <kernel/types.h>

#define IRQ_HANDLER_NUM 256

/*
 * @brief Set an IRQ handler to an specific IRQ vector.
 *
 * @param irq     IRQ vector number.
 * @param handler Handler function for the IRQ.
 */
void irq_set(uint32_t irq, void (*handler)(void));

/*
 * @brief Unset the IRQ handler of an specific IRQ vector.
 *
 * @param irq IRQ vector number.
 */
void irq_unset(uint32_t irq);

/*
 * @brief Call the handler registered for an IRQ vector.
 *
 * @param irq IRQ vector number.
 */
void irq_handle(uint32_t irq);

#endif /* KERNEL_API_INTERRUPT_H */
