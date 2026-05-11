/*   arch/x86_64/handlers.c   */

/* x86_64 ISR and IRQ handler declaration. */

#ifndef ARCH_X86_64_HANDLERS_H
#define ARCH_X86_64_HANDLERS_H

#include <kernel/types.h>
#include "idt.h"

void isr_set_handler(uint8_t vector, void (*handler)(interrupt_frame_t *frame));

void isr_common_handler(interrupt_frame_t *frame);

/* Common IRQ handler installed for IRQ lines. */
void irq_common_handler(interrupt_frame_t *frame);

#endif /* ARCH_X86_64_HANDLERS_H */
