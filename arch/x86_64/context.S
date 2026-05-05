/*   arch/x86_64/context.S   */

/* Register offset inside context struct. */
CTX_RBX = 0
CTX_RBP = 8
CTX_R12 = 16
CTX_R13 = 24
CTX_R14 = 32
CTX_R15 = 40
CTX_RSP = 48

.section .text
.global arch_context_switch
.type   arch_context_switch, @function

/*
 * Saves callee-saved registers of prev, restores next's, then ret which pops
 * the saved RIP off next's kernel stack.
 */
arch_context_switch:
    /* Save previous context. */
    movq    %rbx,  CTX_RBX(%rdi)
    movq    %rbp,  CTX_RBP(%rdi)
    movq    %r12,  CTX_R12(%rdi)
    movq    %r13,  CTX_R13(%rdi)
    movq    %r14,  CTX_R14(%rdi)
    movq    %r15,  CTX_R15(%rdi)
    movq    %rsp,  CTX_RSP(%rdi)

    /* Restore next context. */
    movq    CTX_RSP(%rsi), %rsp
    movq    CTX_RBX(%rsi), %rbx
    movq    CTX_RBP(%rsi), %rbp
    movq    CTX_R12(%rsi), %r12
    movq    CTX_R13(%rsi), %r13
    movq    CTX_R14(%rsi), %r14
    movq    CTX_R15(%rsi), %r15

    /* 
     * ret pops the return address that arch_context_init pushed, or wherever
     * this task last called arch_context_switch from.
     */
    ret

.size arch_context_switch, . - arch_context_switch

.global do_resched
do_resched:
    /* Check need_reschedule. */
    movq need_reschedule(%rip), %rax
    testq %rax, %rax
    jz .no_switch

    /* Call kernel sched_resched with current rsp. */
    movq %rsp, %rdi

    /* Returns new rsp in rax. */
    call sched_resched

    ret

.no_switch:
    movq %rsp, %rax /* Return current rsp unchanged. */
    ret

.size do_resched, . - do_resched
