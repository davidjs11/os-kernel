/*   arch/x86_64/context.c   */

/* x86_64 context management implementation. */

#include <kernel/arch/context.h>
#include <kernel/sched.h>

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
    /* Push a fake return address onto the stack so ret jumps to entry. */
    uintptr_t *sp = (uintptr_t *) (kstack_top & ~0xFULL);
    *--sp = (uintptr_t) sched_exit_trampoline;
    *--sp = entry;

    ctx->rsp = (uint64_t) sp;
    ctx->rbp = 0;
    ctx->rbx = 0;
    ctx->r12 = 0;
    ctx->r13 = 0;
    ctx->r14 = 0;
    ctx->r15 = 0;
    ctx->r15 = 0;
}
