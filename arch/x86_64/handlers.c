/*   arch/x86_64/handlers.c   */

/* x86_64 ISR and IRQ handlers. */

#include "handlers.h"
#include <kernel/device/irq/irq.h>

/* Interrupt Service Routines. */
static void (*isr_handlers[IDT_NUM_VECTORS])(interrupt_frame_t *frame);

void isr_set_handler(uint8_t vector, void (*handler)(interrupt_frame_t *frame))
{
    isr_handlers[vector] = handler;
}

void isr_common_handler(interrupt_frame_t *frame)
{
    if (isr_handlers[frame->vector] != NULL) {
        isr_handlers[frame->vector](frame);
    }
}

void irq_common_handler(interrupt_frame_t *frame)
{
    uint8_t irq = frame->vector - 0x20;
    irq_handle(irq);
    irq_eoi(irq);
}
