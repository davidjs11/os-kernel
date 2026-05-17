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
void      arch_context_set_stack_pointer(cpu_context_t *ctx, uintptr_t sp);

void arch_context_switch(cpu_context_t *prev, cpu_context_t *next);

/*
 * @brief Switch to a context without saving the current one. Used for the
 *        first switch away from kmain.
 *
 * @param next Context to switch to.
 */
void arch_context_first_switch(cpu_context_t *next);

/*
 * @brief Update architecture state for the task that is about to run (current
 *        task in the scheduler), such as its address space or kernel stack.
 */
void arch_task_activate(void);

#endif /* KERNEL_API_ARCH_CONTEXT_H */
