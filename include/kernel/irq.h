/*   include/kernel/irq.h   */

/* Kernel's interrupt subsystem API. */

#ifndef KERNEL_API_IRQ_H
#define KERNEL_API_IRQ_H

#define IRQ_HANDLER_NUM 256 /* Maximum of interrupt handlers. */

/*
 * @brief Disable a specific IRQ.
 *
 * @param irq Number of the interrupt to mask.
 */
void irq_mask(uint8_t irq);

/*
 * @brief Enable a specific IRQ.
 *
 * @param irq Number of the interrupt to unmask.
 */
void irq_unmask(uint8_t irq);

/*
 * @brief Signal the End Of Interrupt to the interrupt controller.
 *
 * @param irq Number of the interrupt line to send the EOI signal.
 */
void irq_eoi(uint8_t irq);

/*
 * @brief Set an IRQ handler to an specific IRQ vector.
 *
 * @param irq     IRQ vector number.
 * @param handler Handler function for the IRQ.
 */
void irq_set(uint8_t irq, void (*handler)(void));

/*
 * @brief Unset the IRQ handler of an specific IRQ vector.
 *
 * @param irq     IRQ vector number.
 * @param handler Handler function for the IRQ.
 */
void irq_unset(uint8_t irq);

/*
 * @brief Handle an IRQ.
 *
 * @param irq     IRQ vector number.
 * @param handler Handler function for the IRQ.
 */
void irq_handle(uint8_t irq);

#endif /* KERNEL_API_IRQ_H */
