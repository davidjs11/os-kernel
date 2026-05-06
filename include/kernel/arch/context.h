/*   include/arch/context.h   */

/* Architecture specific context management. */

#ifndef KERNEL_API_ARCH_CONTEXT_H
#define KERNEL_API_ARCH_CONTEXT_H

#include <kernel/types.h>
#include <arch/context.h>

typedef struct cpu_context cpu_context_t;

void arch_context_init(
        cpu_context_t *ctx,
        uintptr_t entry,
        uintptr_t kstack_top);

void arch_context_init_user(
        cpu_context_t *ctx,
        uintptr_t entry,
        uintptr_t kstack_top,
        uintptr_t ustack_top);

uintptr_t arch_context_get_stack_pointer(cpu_context_t *ctx);
uintptr_t arch_context_set_stack_pointer(cpu_context_t *ctx, uintptr_t sp);

void arch_context_switch(cpu_context_t *prev, cpu_context_t *next);

#endif /* KERNEL_API_ARCH_CONTEXT_H */
