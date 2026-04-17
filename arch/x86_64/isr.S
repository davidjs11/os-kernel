/*   arch/x86_64/isr.S   */

/* Generic ISR-handler stub. */

/* Macro for defining ISRs. */
.macro ISR n handler error
.global isr_handler_\n
isr_handler_\n:

    /* Vector code. */
    pushq $\n 

    /* Dummy error code. */
    .ifb \error
        pushq $0
    .endif

    /* Save registers. */
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rdi
    pushq %rsi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    /* Call handler. */
    cld
    mov %rsp, %rdi
    call \handler

    /* Load registers. */
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rsi
    popq %rdi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax

    /* Return. */
    addq $16, %rsp
    iretq
.endm

.section .text
ISR  0 generic_handler       /* Divide error. */
ISR  1 generic_handler       /* Debug exception. */
ISR  2 generic_handler       /* NMI interrupt. */
ISR  3 generic_handler       /* Breakpoint. */
ISR  4 generic_handler       /* Overflow. */
ISR  5 generic_handler       /* BOUND range exceeded. */
ISR  6 generic_handler       /* Invalid opcode (Undefined opcode). */
ISR  7 generic_handler       /* Device not available (no math coporocessor). */
ISR  8 generic_handler ERROR /* Double fault. */
ISR  9 generic_handler       /* Coprocessor segment overrun. */
ISR 10 generic_handler ERROR /* Invalid TSS. */
ISR 11 generic_handler ERROR /* Segment not present. */
ISR 12 generic_handler ERROR /* Stack-segment fault. */
ISR 13 generic_handler ERROR /* General protection. */
ISR 14 pf_handler      ERROR /* Page fault. */
ISR 15 generic_handler       /* Intel-reserved. */
ISR 16 generic_handler       /* x87 FPU floating-point error. */
ISR 17 generic_handler ERROR /* Alignment check. */
ISR 18 generic_handler       /* Machine check. */
ISR 19 generic_handler       /* SIMD floating-point exception. */
ISR 20 generic_handler       /* Virtualization exception. */
ISR 21 generic_handler ERROR /* Control protection exception. */
ISR 22 generic_handler       /* Intel-reserved. */
ISR 23 generic_handler       /* Intel-reserved. */
ISR 24 generic_handler       /* Intel-reserved. */
ISR 25 generic_handler       /* Intel-reserved. */
ISR 26 generic_handler       /* Intel-reserved. */
ISR 27 generic_handler       /* Intel-reserved. */
ISR 28 generic_handler       /* Intel-reserved. */
ISR 29 generic_handler       /* Intel-reserved. */
ISR 30 generic_handler       /* Intel-reserved. */
ISR 31 generic_handler       /* Intel-reserved. */
