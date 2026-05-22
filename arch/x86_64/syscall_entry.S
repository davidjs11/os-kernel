/*   arch/x86_64/syscall_entry.S   */

KSTACK_TOP_OFFSET = 0    /* Must match offsetof(percpu_t, kstack_top). */
USER_RSP_OFFSET   = 8    /* Must match offsetof(percpu_t, user_rsp).   */
SYSCALL_MAX       = 4    /* Number of syscalls in the system.   */

.section .text
.global syscall_entry
.type   syscall_entry, @function
syscall_entry:
    /* Get kernel GS. */
    swapgs

    /* Switch to kernel stack. */
    movq %rsp, %gs:USER_RSP_OFFSET
    movq %gs:KSTACK_TOP_OFFSET, %rsp
    sti

    /* Save user registers in kernel stack. */
    pushq %rcx /* User RIP.    */
    pushq %r11 /* User RFLAGS. */
    pushq %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    /*
     * We lost RCS, as syscall instruction uses this register. We'll replace it
     * R10 in our syscall calling convention.
     */
    movq %r10, %rcx

    /* Dispatch syscall handler. */
    cmpq $SYSCALL_MAX, %rax
    jae .invalid_syscall
    callq *syscall_table(, %rax, 8)
    jmp .syscall_return

.invalid_syscall:
    movq $-1, %rax

.syscall_return:
    /* Restore user registers. */
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    popq %r11 /* User RFLAGS. */
    popq %rcx /* User RIP.    */

    /* Switch back to user stack. */
    movq %gs:USER_RSP_OFFSET, %rsp

    cli
    swapgs
    sysretq

.size syscall_entry, . - syscall_entry
