/*   arch/x86_64/context.c   */

/* x86_64 context management implementation. */

#include <kernel/arch/context.h>
#include <kernel/sched.h>
#include <kernel/libc/string.h>

/*
 * Safety net for tasks that return from their entry function instead of
 * calling sched_exit() explicitly. Pushed below the entry point by so a bare
 * return lands here rather than jumping to garbage.
 */
static void sched_exit_trampoline(void) {
    sched_exit(0);
}

void arch_context_init(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top)
{
    uintptr_t *sp = (uintptr_t *) (kstack_top & ~0xFULL);

    /* CPU-pushed frame (iretq pops these). */
    *--sp = 0x10;           /* ss     = SEL_KDATA                    */
    *--sp = kstack_top;     /* rsp                                   */
    *--sp = 0x202;          /* rflags = IF=1, reserved bit 1         */
    *--sp = 0x08;           /* cs     = SEL_KCODE                    */
    *--sp = entry;          /* rip    = task_entry_wrapper           */

    /* ISR stub-pushed. */
    *--sp = 0;              /* error_code                            */
    *--sp = 0;              /* vector                                */

    /* GPRs. */
    *--sp = 0;              /* rax                                   */
    *--sp = 0;              /* rbx                                   */
    *--sp = 0;              /* rcx                                   */
    *--sp = 0;              /* rdx                                   */
    *--sp = 0;              /* rdi                                   */
    *--sp = 0;              /* rsi                                   */
    *--sp = 0;              /* rbp                                   */
    *--sp = 0;              /* r8                                    */
    *--sp = 0;              /* r9                                    */
    *--sp = 0;              /* r10                                   */
    *--sp = 0;              /* r11                                   */
    *--sp = 0;              /* r12                                   */
    *--sp = 0;              /* r13                                   */
    *--sp = 0;              /* r14                                   */
    *--sp = 0;              /* r15                                   */

    ctx->rsp = (uintptr_t) sp;
}

uintptr_t arch_context_get_stack_pointer(cpu_context_t *ctx)
{
    return (ctx != NULL) ? ctx->rsp : 0;
}

uintptr_t arch_context_set_stack_pointer(cpu_context_t *ctx, uintptr_t sp)
{
    if (ctx != NULL) {
        ctx->rsp = sp;
    }
}
