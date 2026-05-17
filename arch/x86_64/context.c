/*   arch/x86_64/context.c   */

/* x86_64 context management implementation. */

#include <kernel/arch/context.h>
#include <kernel/sched.h>
#include <kernel/vmm.h>
#include <kernel/libc/string.h>

/* Defined in context.S. */
extern void arch_kernel_task_trampoline(void);
extern void arch_user_task_trampoline(void);

/* Defined in segment.c. */
extern void tss_update_rsp0(void);

/*
 * New tasks start through a trampoline: arch_context_switch ret's into it and
 * it iretq's into the task's entry point using the frame built here. This way
 * the task starts with the right segments and with interrupts enabled.
 */
void arch_context_init(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top)
{
    uintptr_t *sp = (uintptr_t *) (kstack_top & ~0xFULL);
    uintptr_t task_rsp = (uintptr_t) sp - 8; /* ABI alignment on entry. */

    /* iretq frame. */
    *--sp = 0x10;           /* ss     = SEL_KDATA                    */
    *--sp = task_rsp;       /* rsp                                   */
    *--sp = 0x202;          /* rflags = IF=1, reserved bit 1         */
    *--sp = 0x08;           /* cs     = SEL_KCODE                    */
    *--sp = entry;          /* rip    = task entry point             */

    /* Return address of arch_context_switch. */
    *--sp = (uintptr_t) arch_kernel_task_trampoline;

    memset(ctx, 0, sizeof(cpu_context_t));
    ctx->rsp = (uintptr_t) sp;
}

void arch_context_init_user(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top,
    uintptr_t ustack_top)
{
    uintptr_t *sp = (uintptr_t *) (kstack_top & ~0xFULL);

    /* iretq frame. */
    *--sp = 0x1B;           /* ss     = SEL_UDATA                   */
    *--sp = ustack_top;     /* rsp    = user stack top              */
    *--sp = 0x202;          /* rflags = IF=1                        */
    *--sp = 0x23;           /* cs     = SEL_UCODE                   */
    *--sp = entry;          /* rip    = userspace entry point       */

    /* Return address of arch_context_switch. */
    *--sp = (uintptr_t) arch_user_task_trampoline;

    memset(ctx, 0, sizeof(cpu_context_t));
    ctx->rsp = (uintptr_t) sp;
}

void arch_task_activate(void)
{
    /* Interrupts from ring 3 must land on the new task's kernel stack. */
    tss_update_rsp0();

    task_t *task = sched_current();
    if (task != NULL && task->space != NULL) {
        vmm_space_switch(task->space);
    }
}

uintptr_t arch_context_get_stack_pointer(cpu_context_t *ctx)
{
    return (ctx != NULL) ? ctx->rsp : 0;
}

void arch_context_set_stack_pointer(cpu_context_t *ctx, uintptr_t sp)
{
    if (ctx != NULL) {
        ctx->rsp = sp;
    }
}
