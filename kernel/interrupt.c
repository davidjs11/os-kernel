/*   kernel/interrupt.c   */

/* Kernel interrupt handling implementation. */

#include <kernel/interrupt.h>

static void (*irq_handlers[IRQ_HANDLER_NUM])(void);

void irq_set(uint32_t irq, void (*handler)(void))
{
    if (irq < IRQ_HANDLER_NUM) {
        irq_handlers[irq] = handler;
    }
}

void irq_unset(uint32_t irq)
{
    if (irq < IRQ_HANDLER_NUM) {
        irq_handlers[irq] = NULL;
    }
}

void irq_handle(uint32_t irq)
{
    if (irq < IRQ_HANDLER_NUM && irq_handlers[irq] != NULL) {
        irq_handlers[irq]();
    }
}
