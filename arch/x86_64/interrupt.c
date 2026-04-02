/*   arch/x86_64/interrupt.c   */

#include "interrupt.h"
#include "segment.h"
#include <stddef.h>

__attribute__((aligned(16)))
static idt_t idt;

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
extern void isr_stub_0(void);
extern void isr_stub_1(void);
extern void isr_stub_2(void);
extern void isr_stub_3(void);
extern void isr_stub_4(void);
extern void isr_stub_5(void);
extern void isr_stub_6(void);
extern void isr_stub_7(void);
extern void isr_stub_8(void);
extern void isr_stub_9(void);
extern void isr_stub_10(void);
extern void isr_stub_11(void);
extern void isr_stub_12(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);
extern void isr_stub_15(void);
extern void isr_stub_16(void);
extern void isr_stub_17(void);
extern void isr_stub_18(void);
extern void isr_stub_19(void);
extern void isr_stub_20(void);
extern void isr_stub_21(void);
extern void isr_stub_22(void);
extern void isr_stub_23(void);
extern void isr_stub_24(void);
extern void isr_stub_25(void);
extern void isr_stub_26(void);
extern void isr_stub_27(void);
extern void isr_stub_28(void);
extern void isr_stub_29(void);
extern void isr_stub_30(void);
extern void isr_stub_31(void);

void interrupt_init(void)
{
    /* Setup IDT. */
    /* Divide error. */
    idt_set_gate(0,  isr_stub_0,  0, 0, IDT_TYPE_INTERRUPT);
    /* Debug exception. */
    idt_set_gate(1,  isr_stub_1,  1, 0, IDT_TYPE_TRAP);
    /* NMI interrupt. */
    idt_set_gate(2,  isr_stub_2,  2, 0, IDT_TYPE_INTERRUPT);
    /* Breakpoint. */
    idt_set_gate(3,  isr_stub_3,  0, 0, IDT_TYPE_TRAP);
    /* Overflow. */
    idt_set_gate(4,  isr_stub_4,  0, 0, IDT_TYPE_TRAP);
    /* BOUND range exceeded. */
    idt_set_gate(5,  isr_stub_5,  0, 0, IDT_TYPE_INTERRUPT);
    /* Invalid opcode (Undefined opcode). */
    idt_set_gate(6,  isr_stub_6,  0, 0, IDT_TYPE_INTERRUPT);
    /* Device not available (no math coporocessor). */
    idt_set_gate(7,  isr_stub_7,  0, 0, IDT_TYPE_INTERRUPT);
    /* Double fault. */
    idt_set_gate(8,  isr_stub_8,  3, 0, IDT_TYPE_INTERRUPT);
    /* Coprocessor segment overrun. */
    idt_set_gate(9,  isr_stub_9,  0, 0, IDT_TYPE_INTERRUPT);
    /* Invalid TSS. */
    idt_set_gate(10, isr_stub_10, 0, 0, IDT_TYPE_INTERRUPT);
    /* Segment not present. */
    idt_set_gate(11, isr_stub_11, 0, 0, IDT_TYPE_INTERRUPT);
    /* Stack-segment fault. */
    idt_set_gate(12, isr_stub_12, 0, 0, IDT_TYPE_INTERRUPT);
    /* General protection. */
    idt_set_gate(13, isr_stub_13, 0, 0, IDT_TYPE_INTERRUPT);
    /* Page fault. */
    idt_set_gate(14, isr_stub_14, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(15, isr_stub_15, 0, 0, IDT_TYPE_INTERRUPT);
    /* x87 FPU floating-point error. */
    idt_set_gate(16, isr_stub_16, 0, 0, IDT_TYPE_INTERRUPT);
    /* Alignment check. */
    idt_set_gate(17, isr_stub_17, 0, 0, IDT_TYPE_INTERRUPT);
    /* Machine check. */
    idt_set_gate(18, isr_stub_18, 4, 0, IDT_TYPE_INTERRUPT);
    /* SIMD floating-point exception. */
    idt_set_gate(19, isr_stub_19, 0, 0, IDT_TYPE_INTERRUPT);
    /* Virtualization exception. */
    idt_set_gate(20, isr_stub_20, 0, 0, IDT_TYPE_INTERRUPT);
    /* Control protection exception. */
    idt_set_gate(21, isr_stub_21, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(22, isr_stub_22, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(23, isr_stub_23, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(24, isr_stub_24, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(25, isr_stub_25, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(26, isr_stub_26, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(27, isr_stub_27, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(28, isr_stub_28, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(29, isr_stub_29, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(30, isr_stub_30, 0, 0, IDT_TYPE_INTERRUPT);
    /* Intel-reserved. */
    idt_set_gate(31, isr_stub_31, 0, 0, IDT_TYPE_INTERRUPT);

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

void generic_handler(interrupt_frame_t *frame)
{
    /* TODO: do something useful here. */
}
