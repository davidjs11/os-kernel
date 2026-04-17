/*   arch/x86_64/interrupt.c   */

#include "idt.h"
#include "segment.h"
#include <stddef.h>
#include <arch/interrupt.h>

__attribute__((aligned(16)))
static idt_t idt;

/*
 * @brief Set an interrupt gate for an specific interrupt vector in IDT.
 *
 * @param vec     Vector number of the interrupt.
 * @param handler Handler function.
 * @param ist     Interrupt Stack Table index.
 * @param dpl     Descriptor Privilege Level.
 * @param type    Type of the interrupt.
 */
void idt_set_gate(
    uint8_t vec,
    void (*handler)(void),
    uint8_t ist,
    uint8_t dpl,
    uint8_t type)
{
    idt.gate[vec] = IDT_GATE(handler, SEL_KCODE, ist, dpl, type);
}

/* ISR stubs. */
extern void isr_handler_0(void);
extern void isr_handler_1(void);
extern void isr_handler_2(void);
extern void isr_handler_3(void);
extern void isr_handler_4(void);
extern void isr_handler_5(void);
extern void isr_handler_6(void);
extern void isr_handler_7(void);
extern void isr_handler_8(void);
extern void isr_handler_9(void);
extern void isr_handler_10(void);
extern void isr_handler_11(void);
extern void isr_handler_12(void);
extern void isr_handler_13(void);
extern void isr_handler_14(void);
extern void isr_handler_15(void);
extern void isr_handler_16(void);
extern void isr_handler_17(void);
extern void isr_handler_18(void);
extern void isr_handler_19(void);
extern void isr_handler_20(void);
extern void isr_handler_21(void);
extern void isr_handler_22(void);
extern void isr_handler_23(void);
extern void isr_handler_24(void);
extern void isr_handler_25(void);
extern void isr_handler_26(void);
extern void isr_handler_27(void);
extern void isr_handler_28(void);
extern void isr_handler_29(void);
extern void isr_handler_30(void);
extern void isr_handler_31(void);

void arch_interrupt_init(void)
{
    /* Setup IDT. */
    /* Divide error. */
    idt_set_gate(0,  isr_handler_0,  0, 0, IDT_TYPE_INTERRUPT);
    /* Debug exception. */
    idt_set_gate(1,  isr_handler_1,  1, 0, IDT_TYPE_TRAP);
    /* NMI interrupt. */
    idt_set_gate(2,  isr_handler_2,  2, 0, IDT_TYPE_INTERRUPT);
    /* Breakpoint. */
    idt_set_gate(3,  isr_handler_3,  0, 0, IDT_TYPE_TRAP);
    /* Overflow. */
    idt_set_gate(4,  isr_handler_4,  0, 0, IDT_TYPE_TRAP);
    /* BOUND range exceeded. */
    idt_set_gate(5,  isr_handler_5,  0, 0, IDT_TYPE_INTERRUPT);
    /* Invalid opcode (Undefined opcode). */
    idt_set_gate(6,  isr_handler_6,  0, 0, IDT_TYPE_INTERRUPT);
    /* Device not available (no math coporocessor). */
    idt_set_gate(7,  isr_handler_7,  0, 0, IDT_TYPE_INTERRUPT);
    /* Double fault. */
    idt_set_gate(8,  isr_handler_8,  3, 0, IDT_TYPE_INTERRUPT);
    /* Coprocessor segment overrun. */
    idt_set_gate(9,  isr_handler_9,  0, 0, IDT_TYPE_INTERRUPT);
    /* Invalid TSS. */
    idt_set_gate(10, isr_handler_10, 0, 0, IDT_TYPE_INTERRUPT);
    /* Segment not present. */
    idt_set_gate(11, isr_handler_11, 0, 0, IDT_TYPE_INTERRUPT);
    /* Stack-segment fault. */
    idt_set_gate(12, isr_handler_12, 0, 0, IDT_TYPE_INTERRUPT);
    /* General protection. */
    idt_set_gate(13, isr_handler_13, 0, 0, IDT_TYPE_INTERRUPT);
    /* Page fault. */
    idt_set_gate(14, isr_handler_14, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(15, isr_handler_15, 0, 0, IDT_TYPE_INTERRUPT);
    /* x87 FPU floating-point error. */
    idt_set_gate(16, isr_handler_16, 0, 0, IDT_TYPE_INTERRUPT);
    /* Alignment check. */
    idt_set_gate(17, isr_handler_17, 0, 0, IDT_TYPE_INTERRUPT);
    /* Machine check. */
    idt_set_gate(18, isr_handler_18, 4, 0, IDT_TYPE_INTERRUPT);
    /* SIMD floating-point exception. */
    idt_set_gate(19, isr_handler_19, 0, 0, IDT_TYPE_INTERRUPT);
    /* Virtualization exception. */
    idt_set_gate(20, isr_handler_20, 0, 0, IDT_TYPE_INTERRUPT);
    /* Control protection exception. */
    idt_set_gate(21, isr_handler_21, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(22, isr_handler_22, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(23, isr_handler_23, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(24, isr_handler_24, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(25, isr_handler_25, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(26, isr_handler_26, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(27, isr_handler_27, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(28, isr_handler_28, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(29, isr_handler_29, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(30, isr_handler_30, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(31, isr_handler_31, 0, 0, IDT_TYPE_INTERRUPT);

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

void arch_interrupt_enable(void)
{
    __asm__ volatile ("sti" ::: "memory");
}

void arch_interrupt_disable(void)
{
    __asm__ volatile ("cli" ::: "memory");
}

void generic_handler(interrupt_frame_t *frame)
{
    /* TODO: do something useful here. */
}
