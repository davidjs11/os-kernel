/*   arch/x86_64/entry.S   */

/* Generic ISR-handler stub. */

.extern generic_handler

/* Macro for no-error ISR. */
.macro ISR_NOERR n
.globl isr_stub_\n
isr_stub_\n:
    pushq $0  /* Dummy error code. */
    pushq $\n /* Vector code. */
    jmp isr_stub
.endm

/* Macro for error ISR. */
.macro ISR_ERR n
.globl isr_stub_\n
isr_stub_\n:
    pushq $\n /* Vector code. */
    jmp isr_stub
.endm

ISR_NOERR  0 /* Divide error. */
ISR_NOERR  1 /* Debug exception. */
ISR_NOERR  2 /* NMI interrupt. */
ISR_NOERR  3 /* Breakpoint. */
ISR_NOERR  4 /* Overflow. */
ISR_NOERR  5 /* BOUND range exceeded. */
ISR_NOERR  6 /* Invalid opcode (Undefined opcode). */
ISR_NOERR  7 /* Device not available (no math coporocessor). */
ISR_ERR    8 /* Double fault. */
ISR_NOERR  9 /* Coprocessor segment overrun. */
ISR_ERR   10 /* Invalid TSS. */
ISR_ERR   11 /* Segment not present. */
ISR_ERR   12 /* Stack-segment fault. */
ISR_ERR   13 /* General protection. */
ISR_ERR   14 /* Page fault. */
ISR_NOERR 15 /* Intel-reserved. */
ISR_NOERR 16 /* x87 FPU floating-point error. */
ISR_ERR   17 /* Alignment check. */
ISR_NOERR 18 /* Machine check. */
ISR_NOERR 19 /* SIMD floating-point exception. */
ISR_NOERR 20 /* Virtualization exception. */
ISR_ERR   21 /* Control protection exception. */
ISR_NOERR 22 /* Intel-reserved. */
ISR_NOERR 23 /* Intel-reserved. */
ISR_NOERR 24 /* Intel-reserved. */
ISR_NOERR 25 /* Intel-reserved. */
ISR_NOERR 26 /* Intel-reserved. */
ISR_NOERR 27 /* Intel-reserved. */
ISR_NOERR 28 /* Intel-reserved. */
ISR_NOERR 29 /* Intel-reserved. */
ISR_NOERR 30 /* Intel-reserved. */
ISR_NOERR 31 /* Intel-reserved. */

.section .text
.globl isr_stub
isr_stub:
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

    /* call C handler */
    cld
    mov %rsp, %rdi
    call generic_handler

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
    addq $16, %rsp
    iretq
