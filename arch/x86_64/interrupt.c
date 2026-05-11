/*   arch/x86_64/interrupt.c   */

#include "idt.h"
#include "segment.h"
#include <stddef.h>
#include <kernel/arch/interrupt.h>
#include <kernel/kprintf.h>

__attribute__((aligned(16)))
static idt_t idt;

/* ISR stubs. */
extern void (*isr_stubs[48])(void);

void idt_init(void)
{
    /* Exceptions. */
    /* Divide error. */
    idt_set_gate(0,  isr_stubs[0],  0, 0, IDT_TYPE_INTERRUPT);
    /* Debug exception. */
    idt_set_gate(1,  isr_stubs[1],  1, 0, IDT_TYPE_TRAP);
    /* NMI interrupt. */
    idt_set_gate(2,  isr_stubs[2],  2, 0, IDT_TYPE_INTERRUPT);
    /* Breakpoint. */
    idt_set_gate(3,  isr_stubs[3],  0, 0, IDT_TYPE_TRAP);
    /* Overflow. */
    idt_set_gate(4,  isr_stubs[4],  0, 0, IDT_TYPE_TRAP);
    /* BOUND range exceeded. */
    idt_set_gate(5,  isr_stubs[5],  0, 0, IDT_TYPE_INTERRUPT);
    /* Invalid opcode (Undefined opcode). */
    idt_set_gate(6,  isr_stubs[6],  0, 0, IDT_TYPE_INTERRUPT);
    /* Device not available (no math coporocessor). */
    idt_set_gate(7,  isr_stubs[7],  0, 0, IDT_TYPE_INTERRUPT);
    /* Double fault. */
    idt_set_gate(8,  isr_stubs[8],  3, 0, IDT_TYPE_INTERRUPT);
    /* Coprocessor segment overrun. */
    idt_set_gate(9,  isr_stubs[9],  0, 0, IDT_TYPE_INTERRUPT);
    /* Invalid TSS. */
    idt_set_gate(10, isr_stubs[10], 0, 0, IDT_TYPE_INTERRUPT);
    /* Segment not present. */
    idt_set_gate(11, isr_stubs[11], 0, 0, IDT_TYPE_INTERRUPT);
    /* Stack-segment fault. */
    idt_set_gate(12, isr_stubs[12], 0, 0, IDT_TYPE_INTERRUPT);
    /* General protection. */
    idt_set_gate(13, isr_stubs[13], 0, 0, IDT_TYPE_INTERRUPT);
    /* Page fault. */
    idt_set_gate(14, isr_stubs[14], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(15, isr_stubs[15], 0, 0, IDT_TYPE_INTERRUPT);
    /* x87 FPU floating-point error. */
    idt_set_gate(16, isr_stubs[16], 0, 0, IDT_TYPE_INTERRUPT);
    /* Alignment check. */
    idt_set_gate(17, isr_stubs[17], 0, 0, IDT_TYPE_INTERRUPT);
    /* Machine check. */
    idt_set_gate(18, isr_stubs[18], 4, 0, IDT_TYPE_INTERRUPT);
    /* SIMD floating-point exception. */
    idt_set_gate(19, isr_stubs[19], 0, 0, IDT_TYPE_INTERRUPT);
    /* Virtualization exception. */
    idt_set_gate(20, isr_stubs[20], 0, 0, IDT_TYPE_INTERRUPT);
    /* Control protection exception. */
    idt_set_gate(21, isr_stubs[21], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(22, isr_stubs[22], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(23, isr_stubs[23], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(24, isr_stubs[24], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(25, isr_stubs[25], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(26, isr_stubs[26], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(27, isr_stubs[27], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(28, isr_stubs[28], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(29, isr_stubs[29], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(30, isr_stubs[30], 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(31, isr_stubs[31], 0, 0, IDT_TYPE_INTERRUPT);

    /* Hardware IRQs. */
    for (size_t i = 0; i < 16; i++) {
        idt_set_gate(32 + i, isr_stubs[32 + i], 0, 0, IDT_TYPE_INTERRUPT);
    }

    /* Setup IDTR. */
    idtr_t idtr = (idtr_t) {
        .base = (uintptr_t) &idt,
        .limit = (uint16_t) (sizeof(idt_t) - 1)
    };

    /* Load IDT. */
    __asm__ volatile (
        "lidt %0"
        :
        : "m" (idtr)
        : "memory"
    );
}

void idt_set_gate(
    uint8_t vector,
    void (*handler)(void),
    uint8_t ist,
    uint8_t dpl,
    uint8_t type)
{
    idt.gate[vector] = IDT_GATE(handler, SEL_KCODE, ist, dpl, type);
}

void arch_interrupt_enable(void)
{
    __asm__ volatile ("sti" ::: "memory");
}

void arch_interrupt_disable(void)
{
    __asm__ volatile ("cli" ::: "memory");
}
