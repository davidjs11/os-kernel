/*   arch/aarch64/context.c   */

/* AArch64 context management implementation. */

#include <kernel/arch/context.h>
#include <kernel/sched.h>
#include <kernel/libc/string.h>

void arch_context_init(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top)
{

}

void arch_context_init_user(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top,
    uintptr_t ustack_top)
{

}

uintptr_t arch_context_get_stack_pointer(cpu_context_t *ctx)
{

}

void arch_context_set_stack_pointer(cpu_context_t *ctx, uintptr_t sp)
{

}

void arch_task_activate(void)
{

}
