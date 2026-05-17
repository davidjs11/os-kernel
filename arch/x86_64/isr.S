/*   arch/x86_64/isr.S   */

/* Save general purpose registers. */
.macro SAVE_REGS
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
.endm

/* Restore general purpose registers. */
.macro RESTORE_REGS
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
.endm

/* Generic exception-handler stub. */
.macro ISR n handler error
.global isr_stub_\n
isr_stub_\n:

    /* Dummy error code. */
    .ifb \error
        pushq $0
    .endif

    /* Vector code. */
    pushq $\n

    SAVE_REGS

    /* Call handler. */
    cld
    mov %rsp, %rdi
    call \handler

    RESTORE_REGS

    /* Return. */
    addq $16, %rsp
    iretq
.endm

/*
 * Generic IRQ-handler stub. Hardware IRQs are the only preemption points:
 * sched_resched may switch to another task, and the interrupted task resumes
 * from here when it's scheduled again, with its interrupt frame intact.
 */
.macro IRQ n handler
.global isr_stub_\n
isr_stub_\n:

    /* Dummy error code and vector code. */
    pushq $0
    pushq $\n

    SAVE_REGS

    /* Call handler. */
    cld
    mov %rsp, %rdi
    call \handler

    /* Switch to another task if needed. */
    call sched_resched

    RESTORE_REGS

    /* Return. */
    addq $16, %rsp
    iretq
.endm

.section .text
/* Exceptions. */
ISR  0 isr_common_handler       /* Divide error.                      */
ISR  1 isr_common_handler       /* Debug exception.                   */
ISR  2 isr_common_handler       /* NMI interrupt.                     */
ISR  3 isr_common_handler       /* Breakpoint.                        */
ISR  4 isr_common_handler       /* Overflow.                          */
ISR  5 isr_common_handler       /* BOUND range exceeded.              */
ISR  6 isr_common_handler       /* Invalid opcode (Undefined opcode). */
ISR  7 isr_common_handler       /* No math coprocessor.               */
ISR  8 isr_common_handler ERROR /* Double fault.                      */
ISR  9 isr_common_handler       /* Coprocessor segment overrun.       */
ISR 10 isr_common_handler ERROR /* Invalid TSS.                       */
ISR 11 isr_common_handler ERROR /* Segment not present.               */
ISR 12 isr_common_handler ERROR /* Stack-segment fault.               */
ISR 13 isr_common_handler ERROR /* General protection.                */
ISR 14 isr_common_handler ERROR /* Page fault.                        */
ISR 15 isr_common_handler       /* Intel-reserved.                    */
ISR 16 isr_common_handler       /* x87 FPU floating-point error.      */
ISR 17 isr_common_handler ERROR /* Alignment check.                   */
ISR 18 isr_common_handler       /* Machine check.                     */
ISR 19 isr_common_handler       /* SIMD floating-point exception.     */
ISR 20 isr_common_handler       /* Virtualization exception.          */
ISR 21 isr_common_handler ERROR /* Control protection exception.      */
ISR 22 isr_common_handler       /* Intel-reserved.                    */
ISR 23 isr_common_handler       /* Intel-reserved.                    */
ISR 24 isr_common_handler       /* Intel-reserved.                    */
ISR 25 isr_common_handler       /* Intel-reserved.                    */
ISR 26 isr_common_handler       /* Intel-reserved.                    */
ISR 27 isr_common_handler       /* Intel-reserved.                    */
ISR 28 isr_common_handler       /* Intel-reserved.                    */
ISR 29 isr_common_handler       /* Intel-reserved.                    */
ISR 30 isr_common_handler       /* Intel-reserved.                    */
ISR 31 isr_common_handler       /* Intel-reserved.                    */

/* Hardware IRQs. */
IRQ 32 isr_common_handler       /* IRQ0 - Timer.      */
IRQ 33 isr_common_handler       /* IRQ1 - Keyboard.   */
IRQ 34 isr_common_handler       /* IRQ2 - Cascade.    */
IRQ 35 isr_common_handler       /* IRQ3 - Serial 2.   */
IRQ 36 isr_common_handler       /* IRQ4 - Serial 1.   */
IRQ 37 isr_common_handler       /* IRQ5 - Parallel 2. */
IRQ 38 isr_common_handler       /* IRQ6 - Floppy.     */
IRQ 39 isr_common_handler       /* IRQ7 - Parallel 1. */
IRQ 40 isr_common_handler       /* IRQ8 - RTC.        */
IRQ 41 isr_common_handler       /* IRQ9 - ACPI.       */
IRQ 42 isr_common_handler       /* IRQ10 - Open.      */
IRQ 43 isr_common_handler       /* IRQ11 - Open.      */
IRQ 44 isr_common_handler       /* IRQ12 - Mouse.     */
IRQ 45 isr_common_handler       /* IRQ13 - FPU.       */
IRQ 46 isr_common_handler       /* IRQ14 - ATA1.      */
IRQ 47 isr_common_handler       /* IRQ15 - ATA2.      */

/* Pointer table. */
.section .rodata
.global isr_stubs
isr_stubs:
    .quad isr_stub_0
    .quad isr_stub_1
    .quad isr_stub_2
    .quad isr_stub_3
    .quad isr_stub_4
    .quad isr_stub_5
    .quad isr_stub_6
    .quad isr_stub_7
    .quad isr_stub_8
    .quad isr_stub_9
    .quad isr_stub_10
    .quad isr_stub_11
    .quad isr_stub_12
    .quad isr_stub_13
    .quad isr_stub_14
    .quad isr_stub_15
    .quad isr_stub_16
    .quad isr_stub_17
    .quad isr_stub_18
    .quad isr_stub_19
    .quad isr_stub_20
    .quad isr_stub_21
    .quad isr_stub_22
    .quad isr_stub_23
    .quad isr_stub_24
    .quad isr_stub_25
    .quad isr_stub_26
    .quad isr_stub_27
    .quad isr_stub_28
    .quad isr_stub_29
    .quad isr_stub_30
    .quad isr_stub_31
    .quad isr_stub_32
    .quad isr_stub_33
    .quad isr_stub_34
    .quad isr_stub_35
    .quad isr_stub_36
    .quad isr_stub_37
    .quad isr_stub_38
    .quad isr_stub_39
    .quad isr_stub_40
    .quad isr_stub_41
    .quad isr_stub_42
    .quad isr_stub_43
    .quad isr_stub_44
    .quad isr_stub_45
    .quad isr_stub_46
    .quad isr_stub_47
