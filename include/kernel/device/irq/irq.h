/*   include/kernel/device/irq/irq.h   */

/* IRQ controller driver API. */

#ifndef KERNEL_API_DEVICE_IRQ_H
#define KERNEL_API_DEVICE_IRQ_H

#include <kernel/types.h>

#define IRQ_HANDLER_NUM 256

/* IRQ device operations. */
typedef struct irq_device {
    void     (*init)(void);           /* Initialize the device.              */
    void     (*mask)(uint32_t irq);   /* Mask an IRQ line.                   */
    void     (*unmask)(uint32_t irq); /* Unmask an IRQ line.                 */
    uint32_t (*ack)(void);            /* Send ACK signal and return IRQ num. */
    void     (*eoi)(uint32_t irq);    /* Send EOI signal.                    */
} irq_device_t;

/*
 * @brief Register a new IRQ controller device
 */
void irq_register(irq_device_t *irq);

/*
 * @brief Initialize the IRQ controller device.
 */
void irq_init(void);

/*
 * @brief Mask an interrupt line.
 *
 * @param irq IRQ number.
 */
void irq_mask(uint32_t irq);

/*
 * @brief Unmask an interrupt line.
 *
 * @param irq IRQ number.
 */
void irq_unmask(uint32_t irq);

/*
 * @brief Send an ACK signal to the IRQ controller to notify that IRQ is being
 *        served and obtain the IRQ number.
 *
 * @return IRQ number of the IRQ that is going to be handled.
 */
uint32_t irq_ack(void);

/*
 * @brief Send End Of Interrupt and notify IRQ controller that IRQ has been
 * served.
 *
 * @param irq IRQ number.
 */
void irq_eoi(uint32_t irq);

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
 * @param irq     IRQ vector number.
 */
void irq_unset(uint32_t irq);

/*
 * @brief Handle an IRQ.
 *
 * @param irq     IRQ vector number.
 */
void irq_handle(uint32_t irq);

/*
 * @brief Activate an IRQ and call the handler.
 *
 * @param irq     IRQ vector number.
 */
void irq_handle(uint32_t irq);

#endif /* KERNEL_API_DEVICE_IRQ_H */
