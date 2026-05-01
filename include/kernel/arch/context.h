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
        uintptr_t kstack_top,
        bool user);

void arch_context_switch(cpu_context_t *prev, cpu_context_t *next);

/* Setup initial stack frame and register context when creating a new task. */

#endif /* KERNEL_API_ARCH_CONTEXT_H */
