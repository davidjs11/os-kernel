/*   device/irq/irq.c   */

/* IRQ controller device management. */

#include <kernel/device/irq/irq.h>

static irq_device_t *irq_dev = NULL;

void irq_register(irq_device_t *irq)
{
    irq_dev = irq;
}

void irq_init(void)
{
    if (irq_dev != NULL && irq_dev->init != NULL) {
        irq_dev->init();
    }
}

void irq_mask(uint32_t irq)
{
    if (irq_dev != NULL && irq_dev->mask != NULL) {
        irq_dev->mask(irq);
    }
}

void irq_unmask(uint32_t irq)
{
    if (irq_dev != NULL && irq_dev->unmask != NULL) {
        irq_dev->unmask(irq);
    }
}

uint32_t irq_ack(void)
{
    if (irq_dev != NULL && irq_dev->ack != NULL) {
        return irq_dev->ack();
    }
    return 0xffffffff;
}

void irq_eoi(uint32_t irq)
{
    if (irq_dev != NULL && irq_dev->eoi != NULL) {
        irq_dev->eoi(irq);
    }
}
