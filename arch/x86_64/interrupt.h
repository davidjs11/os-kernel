/*   arch/x86_64/include/interrupt.h   */

/*
 * x86-64 interrupt and exception handling.
 *
 * References
 *   Intel SDM Vol. 3A, Chapter 7 - Interrupt and Exception Handling
 */

#ifndef ARCH_X86_64_INTERRUPT_H
#define ARCH_X86_64_INTERRUPT_H

#include <stdint.h>

/* Interrupt descriptor table definitions.                       */
#define IDT_NUM_VECTORS     256 /* Number of entries in the IDT. */
#define IDT_TYPE_INTERRUPT  0xE /* Clears IF on entry.           */
#define IDT_TYPE_TRAP       0xF /* Preserves IF on entry.        */

/* Interrupt/Trap gate (SDM Vol. 3A Fig. 7-8).                            */
typedef union __attribute__((packed)) idt_gate_desc {
    uint64_t raw[2];
    struct {
        uint64_t offset_low : 16; /* bits 15:0  - offset[15:0]            */
        uint64_t selector   : 16; /* bits 31:16 - destination CS selector */
        uint64_t ist        :  3; /* bits 34:32 - interrupt stack table   */
        uint64_t _zero0     :  5; /* bits 39:35 - set to zero             */
        uint64_t type       :  4; /* bits 43:40 - type                    */
        uint64_t _zero1     :  1; /* bit  44    - set to zero             */
        uint64_t dpl        :  2; /* bits 46:45 - privilege level         */
        uint64_t present    :  1; /* bit  47    - segment present         */
        uint64_t offset_mid : 16; /* bits 63:48 - offset[31:16]           */
        uint64_t offset_high: 32; /* bits 95:64 - offset[63:32]           */
        uint64_t reserved   : 32; /* bits 127:96- reserved                */
    } bits;
} idt_gate_t;

/* Interrupt Descriptor Table (SDM Vol. 3A 7.10). */
typedef struct __attribute__((packed)) idt {
    idt_gate_t gate[IDT_NUM_VECTORS];
} idt_t;

/*
 * @brief Construct an interrupt or trap gate.
 *
 * @param handler   Address of the raw ASM entry stub.
 * @param sel       CS selector (use SEL_KCODE for all kernel handlers).
 * @param ist_idx   0-7 IST slot (0 = legacy RSP0 switching).
 * @param gate_dpl  DPL (0 = kernel-only, 3 = user-invokable).
 * @param gate_type IDT_TYPE_INTERRUPT or IDT_TYPE_TRAP.
 */
#define IDT_GATE(handler, sel, ist_idx, gate_dpl, gate_type) ((idt_gate_t) { \
.bits = {                                                                    \
    .offset_low  = (uint16_t) ((uintptr_t) (handler) & 0xffff),              \
    .selector    = (sel),                                                    \
    .ist         = (ist_idx),                                                \
    ._zero0      = 0,                                                        \
    .type        = (gate_type),                                              \
    ._zero1      = 0,                                                        \
    .dpl         = (gate_dpl),                                               \
    .present     = 1,                                                        \
    .offset_mid  = (uint16_t) (((uintptr_t) (handler) >> 16) & 0xffff),      \
    .offset_high = (uint32_t) (((uintptr_t) (handler) >> 32) & 0xffffffff),  \
    .reserved    = 0,                                                        \
}})

/* IDTR pseudo-descriptor (SDM Vol. 3A 7.10, Fig. 7-1). */
typedef struct __attribute__((packed)) idtr {
    uint16_t limit; /* Byte length of IDT minus 1.      */
    uint64_t base;  /* Linear address of IDT.           */
} idtr_t;

/* Interrupt handler stack frame struct (SDM Vol. 3A 7.14.4, Fig 7-10). */
typedef struct interrupt_frame {
    /* GPRs. */
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rsi, rdi, rdx, rcx, rbx, rax;
    /* Stub-pushed */
    uint64_t vector, error_code;
    /* CPU-pushed */
    uint64_t rip, cs, rflags, rsp, ss;
} interrupt_frame_t;

/*
 * @brief Initialize interrupts.
 */
void interrupt_init(void);

/*
 * @brief Set an specific gate in the IDT.
 *
 * @param idt Interrupt Descriptor Table pointer.
 */
void idt_set_gate(
    uint8_t vec,
    void (*handler)(void),
    uint8_t ist,
    uint8_t dpl,
    uint8_t type
);

static inline void interrupt_enable()
{
    __asm__ volatile ("sti" ::: "memory");
}

static inline void interrupt_disable()
{
    __asm__ volatile ("cli" ::: "memory");
}

#endif /* ARCH_X86_64_INTERRUPT_H */
