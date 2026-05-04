/*   include/kernel/platform/irq.h   */

/* Platform interrupt controller kernel API. */

#ifndef KERNEL_API_PLATFORM_IRQ_H
#define KERNEL_API_PLATFORM_IRQ_H

#include <stdint.h>
#include <stdbool.h>

/*
 * @brief Initialize the interrupt controller.
 */
void platform_irq_init(void);

/*
 * @brief Disable a specific IRQ line.
 *
 * @param irq Number of the interrupt line to mask.
 */
void platform_irq_mask(uint8_t irq);

/*
 * @brief Enable a specific IRQ line.
 *
 * @param irq Number of the interrupt line to unmask.
 */
void platform_irq_unmask(uint8_t irq);

/*
 * @brief Signal the End Of Interrupt to the interrupt controller.
 *
 * @param irq Number of the interrupt line to send the EOI signal.
 */
void platform_irq_eoi(uint8_t irq);

/*
 * @brief Register an IRQ handler. This function acts as a wrapper for
 *        arch_interrupt_set_handler().
 *
 * @param irq     IRQ vector number.
 * @param handler Handler function for the IRQ.
 */
void platform_irq_register(uint8_t irq, void (*handler)(void));

#endif /* KERNEL_API_PLATFORM_IRQ_H */
