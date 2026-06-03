/*   arch/aarch64/context.c   */

/* AArch64 context management implementation. */

#include <kernel/arch/context.h>
#include <kernel/sched.h>
#include <kernel/vmm.h>
#include <kernel/libc/string.h>
#include "exception.h"

/* Defined in exception.S. */
extern void kernel_task_trampoline(void);
extern void user_task_trampoline(void);

/* Defined in vmm.c. */
extern vmm_space_t kernel_space;

/*
 * New tasks start through a trampoline: arch_context_switch ret's into it and
 * it eret's into the task's entry point using the exception frame built here.
 */
void arch_context_init(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top)
{
    uintptr_t sp = kstack_top & ~0xFUL;

    /* Exception frame. */
    exception_frame_t *frame = (exception_frame_t *) (sp - sizeof(*frame));
    memset(frame, 0, sizeof(*frame));
    frame->sp     = sp;
    frame->pc     = entry;
    frame->pstate = 0x5; /* EL1h (EL1 using SP_EL1), IRQs unmasked. */

    memset(ctx, 0, sizeof(cpu_context_t));
    ctx->sp = (uintptr_t) frame;
    ctx->lr = (uintptr_t) kernel_task_trampoline;
}

void arch_context_init_user(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top,
    uintptr_t ustack_top)
{
    uintptr_t sp = kstack_top & ~0xFUL;

    /* Exception frame. */
    exception_frame_t *frame = (exception_frame_t *) (sp - sizeof(*frame));
    memset(frame, 0, sizeof(*frame));
    frame->sp     = ustack_top;
    frame->pc     = entry;
    frame->pstate = 0x0; /* EL0t (EL0 using SP_EL0). */

    memset(ctx, 0, sizeof(cpu_context_t));
    ctx->sp = (uintptr_t) frame;
    ctx->lr = (uintptr_t) user_task_trampoline;
}

void arch_task_activate(void)
{
    task_t *task = sched_current();

    /* Kernel tasks only use the higher half (TTBR1), which never changes. */
    if (task == NULL || task->space == &kernel_space) {
        return;
    }

    /* User address space lives in TTBR0 (lower half). */
    vmm_space_set_current(task->space);
    __asm__ volatile (
        "dsb  ish\n"
        "msr  ttbr0_el1, %0\n"
        "isb\n"
        "tlbi vmalle1is\n"
        "dsb  ish\n"
        "isb\n"
        :
        : "r" (task->space->pt_phys)
        : "memory"
    );
}

uintptr_t arch_context_get_stack_pointer(cpu_context_t *ctx)
{
    return (ctx != NULL) ? ctx->sp : 0;
}

void arch_context_set_stack_pointer(cpu_context_t *ctx, uintptr_t sp)
{
    if (ctx != NULL) {
        ctx->sp = sp;
    }
}
