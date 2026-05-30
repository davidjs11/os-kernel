/*   arch/aarch64/context.S   */

/* Register offset inside context struct. */
CTX_X19 = 0
CTX_X21 = 16
CTX_X23 = 32
CTX_X25 = 48
CTX_X27 = 64
CTX_FP  = 80
CTX_SP  = 96

.section .text
.global arch_context_switch
.type   arch_context_switch, %function

/*
 * Saves callee-saved registers of prev, restores next's, then ret which
 * branches to next's saved LR.
 */
arch_context_switch:
    /* Save previous context. */
    stp     x19, x20, [x0, #CTX_X19]
    stp     x21, x22, [x0, #CTX_X21]
    stp     x23, x24, [x0, #CTX_X23]
    stp     x25, x26, [x0, #CTX_X25]
    stp     x27, x28, [x0, #CTX_X27]
    stp     x29, x30, [x0, #CTX_FP]
    mov     x9,  sp
    str     x9,  [x0, #CTX_SP]

    /* Restore next context. */
    ldp     x19, x20, [x1, #CTX_X19]
    ldp     x21, x22, [x1, #CTX_X21]
    ldp     x23, x24, [x1, #CTX_X23]
    ldp     x25, x26, [x1, #CTX_X25]
    ldp     x27, x28, [x1, #CTX_X27]
    ldp     x29, x30, [x1, #CTX_FP]
    ldr     x9,  [x1, #CTX_SP]
    mov     sp,  x9

    /*
     * ret branches to the LR set by arch_context_init, or wherever this task
     * last called arch_context_switch from.
     */
    ret

.size arch_context_switch, . - arch_context_switch

.global arch_context_first_switch
.type   arch_context_first_switch, %function

/*
 * Restores next's callee-saved registers without saving the current ones.
 * Used only for the first switch, leaving kmain's stack behind.
 */
arch_context_first_switch:
    ldp     x19, x20, [x0, #CTX_X19]
    ldp     x21, x22, [x0, #CTX_X21]
    ldp     x23, x24, [x0, #CTX_X23]
    ldp     x25, x26, [x0, #CTX_X25]
    ldp     x27, x28, [x0, #CTX_X27]
    ldp     x29, x30, [x0, #CTX_FP]
    ldr     x9,  [x0, #CTX_SP]
    mov     sp,  x9
    ret

.size arch_context_first_switch, . - arch_context_first_switch

.global do_resched
.type   do_resched, %function

/* Called from the IRQ stub, switches to another task if needed. */
do_resched:
    stp     x29, x30, [sp, #-16]!
    bl      sched_resched
    ldp     x29, x30, [sp], #16
    ret

.size do_resched, . - do_resched
