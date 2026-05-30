/*   arch/aarch64/exception.S   */

/* Exception management. */

/* Vector Table entries macro. */
.macro vector_entry label
    .balign 128
    b \label
.endm

/* Load the table into VBAR_EL1. */
.section .text
.global exception_load_vector_table
.type exception_load_vector_table, %function
exception_load_vector_table:
    ldr	x0, =exception_vector_table
    msr	vbar_el1, x0
    isb
    ret
.size exception_load_vector_table, . - exception_load_vector_table

/* Save the current context in the stack frame. */
.macro exception_store_context
    /* Create space on the stack. */
    sub sp, sp, #0x120

    /* Save x0–x29 in pairs. */
    stp x0,  x1,  [sp, #0x00]
    stp x2,  x3,  [sp, #0x10]
    stp x4,  x5,  [sp, #0x20]
    stp x6,  x7,  [sp, #0x30]
    stp x8,  x9,  [sp, #0x40]
    stp x10, x11, [sp, #0x50]
    stp x12, x13, [sp, #0x60]
    stp x14, x15, [sp, #0x70]
    stp x16, x17, [sp, #0x80]
    stp x18, x19, [sp, #0x90]
    stp x20, x21, [sp, #0xa0]
    stp x22, x23, [sp, #0xb0]
    stp x24, x25, [sp, #0xc0]
    stp x26, x27, [sp, #0xd0]
    stp x28, x29, [sp, #0xe0]

    /* x30 (LR), SP. */
    add x9, sp, #0x120
    stp x30, x9, [sp, #0xf0]

    /* ELR_EL1 (PC), SPSR_EL1 (PSTATE). */
    mrs  x10, elr_el1
    mrs  x11, spsr_el1
    stp  x10, x11, [sp, #0x100]

    /* ESR_EL1, FAR_EL1. */
    mrs  x12, esr_el1
    mrs  x13, far_el1
    stp  x12, x13, [sp, #0x110]
.endm

/* Restore the current context from the stack frame. */
.macro exception_restore_context
    /* ELR_EL1 (PC) and SPSR_EL1 (PSTATE). */
    ldp x10, x11, [sp, #0x100]
    msr elr_el1,  x10
    msr spsr_el1, x11

    /* x30 (LR) and SP. */
    ldr x30, [sp, #0xf0]

    /* x0–x29 in pairs. */
    ldp  x28, x29, [sp, #0xe0]
    ldp  x26, x27, [sp, #0xd0]
    ldp  x24, x25, [sp, #0xc0]
    ldp  x22, x23, [sp, #0xb0]
    ldp  x20, x21, [sp, #0xa0]
    ldp  x18, x19, [sp, #0x90]
    ldp  x16, x17, [sp, #0x80]
    ldp  x14, x15, [sp, #0x70]
    ldp  x12, x13, [sp, #0x60]
    ldp  x10, x11, [sp, #0x50]
    ldp  x8,  x9,  [sp, #0x40]
    ldp  x6,  x7,  [sp, #0x30]
    ldp  x4,  x5,  [sp, #0x20]
    ldp  x2,  x3,  [sp, #0x10]
    ldp  x0,  x1,  [sp, #0x00]

    /* Reclaim the stack space. */
    add  sp, sp, #0x120

.endm
    
/* Exception Vector Table. */
.section .text
.balign 2048
exception_vector_table:
    vector_entry stub_sync
    vector_entry stub_irq
    vector_entry stub_fiq
    vector_entry stub_serror
    vector_entry stub_sync
    vector_entry stub_irq
    vector_entry stub_fiq
    vector_entry stub_serror
    vector_entry stub_sync
    vector_entry stub_irq
    vector_entry stub_fiq
    vector_entry stub_serror
    vector_entry stub_sync
    vector_entry stub_irq
    vector_entry stub_fiq
    vector_entry stub_serror
.size exception_vector_table, . - exception_vector_table

/* Unhandled exceptions use this stub. */
stub_unhandled:
    exception_store_context
    exception_restore_context
    b .
.size stub_unhandled, . - stub_unhandled

/* Synchronous exceptions stub. */
stub_sync:
    exception_store_context
    mov x0, sp
    bl exception_handler_sync
    exception_restore_context
    eret
.size stub_sync, . - stub_sync

/* Interrupt Request exceptions stub. */
stub_irq:
    exception_store_context
    mov x0, sp
    bl exception_handler_irq

    /*
     * Switch to another task if needed. When this task runs again, sp points
     * to its exception frame again.
     */
    bl do_resched
    exception_restore_context
    eret
.size stub_irq, . - stub_irq

/* Fast Interrupt Request exceptions stub. */
stub_fiq:
    exception_store_context
    mov x0, sp
    bl exception_handler_fiq
    exception_restore_context
    eret
.size stub_fiq, . - stub_fiq

/* SError exceptions stub. */
stub_serror:
    exception_store_context
    mov x0, sp
    bl exception_handler_serror
    b . /* Usually fatal, don't return for the moment. */
.size stub_serror, . - stub_serror

/*
 * First code run by a new kernel task. sp points to the exception frame built
 * by arch_context_init, so eret starts the task at EL1 with IRQs unmasked
 * (a plain ret would keep the IRQ mask of the interrupt that switched to it).
 */
.global kernel_task_trampoline
.type   kernel_task_trampoline, %function
kernel_task_trampoline:
    exception_restore_context
    eret
.size kernel_task_trampoline, . - kernel_task_trampoline
