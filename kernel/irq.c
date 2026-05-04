/*   kernel/irq.c   */

/* Kernel interrupt subsystem. */

#include <kernel/types.h>
#include <kernel/platform/irq.h>
#include <kernel/irq.h>

static void (*irq_handlers[IRQ_HANDLER_NUM])(void);

void irq_mask(uint8_t irq)
{
    platform_irq_mask(irq);
}

void irq_unmask(uint8_t irq)
{
    platform_irq_unmask(irq);
}

void irq_eoi(uint8_t irq)
{
    platform_irq_unmask(irq);
}

void irq_set(uint8_t irq, void (*handler)(void))
{
    if (irq < IRQ_HANDLER_NUM) {
        irq_handlers[irq] = handler;
    }
}

void irq_unset(uint8_t irq) {
    if (irq < IRQ_HANDLER_NUM) {
        irq_handlers[irq] = NULL;
    }
}

void irq_handle(uint8_t irq)
{
    if (irq_handlers[irq] != NULL) {
        irq_handlers[irq]();
    }
}
