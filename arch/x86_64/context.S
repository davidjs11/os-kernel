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

.global arch_context_first_switch
.type   arch_context_first_switch, @function

/*
 * Restores next's callee-saved registers without saving the current ones.
 * Used only for the first switch, leaving kmain's stack behind.
 */
arch_context_first_switch:
    movq    CTX_RSP(%rdi), %rsp
    movq    CTX_RBX(%rdi), %rbx
    movq    CTX_RBP(%rdi), %rbp
    movq    CTX_R12(%rdi), %r12
    movq    CTX_R13(%rdi), %r13
    movq    CTX_R14(%rdi), %r14
    movq    CTX_R15(%rdi), %r15
    ret

.size arch_context_first_switch, . - arch_context_first_switch

.global arch_kernel_task_trampoline
.type   arch_kernel_task_trampoline, @function

/*
 * First code run by a new kernel task. The iretq frame built by
 * arch_context_init is on top of the stack.
 */
arch_kernel_task_trampoline:
    iretq

.size arch_kernel_task_trampoline, . - arch_kernel_task_trampoline

.global arch_user_task_trampoline
.type   arch_user_task_trampoline, @function

/*
 * First code run by a new user task. Clears caller-saved registers so no
 * kernel data is leaked to userspace, then iretq to ring 3.
 */
arch_user_task_trampoline:
    xorq    %rax, %rax
    xorq    %rcx, %rcx
    xorq    %rdx, %rdx
    xorq    %rdi, %rdi
    xorq    %rsi, %rsi
    xorq    %r8,  %r8
    xorq    %r9,  %r9
    xorq    %r10, %r10
    xorq    %r11, %r11
    iretq

.size arch_user_task_trampoline, . - arch_user_task_trampoline
