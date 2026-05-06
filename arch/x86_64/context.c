/*   arch/x86_64/context.c   */

/* x86_64 context management implementation. */

#include <kernel/arch/context.h>
#include <kernel/sched.h>
#include <kernel/libc/string.h>

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

    /* GPRs all zero. */
    for (int i = 0; i < 15; i++) {
        *--sp = 0;
    }

    ctx->rsp = (uintptr_t) sp;
}

void arch_context_init_user(
    cpu_context_t *ctx,
    uintptr_t entry,
    uintptr_t kstack_top,
    uintptr_t ustack_top)
{
    uintptr_t *sp = (uintptr_t *) (kstack_top & ~0xFULL);

    /* CPU-pushed frame. */
    *--sp = 0x1B;           /* ss     = SEL_UDATA                   */
    *--sp = ustack_top;     /* rsp    = user stack top              */
    *--sp = 0x202;          /* rflags = IF=1                        */
    *--sp = 0x23;           /* cs     = SEL_UCODE                   */
    *--sp = entry;          /* rip    = userspace entry point       */
    /* stub-pushed */
    *--sp = 0;              /* error_code                           */
    *--sp = 0;              /* vector                               */

    /* GPRs all zero. */
    for (int i = 0; i < 15; i++) {
        *--sp = 0;
    }

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
