/*   arch/x86_64/context.h   */

/* Architecture-specific context structure definition. */

#ifndef ARCH_X86_64_CONTEXT_H
#define ARCH_X86_64_CONTEXT_H

#include <kernel/types.h>

/*
 * Only callee-saved registers need to be in the struct. Caller-saved registers
 * are already on the kernel stack at the point of switching (because the
 * switch happens inside a C function call). This is the same optimization
 * Linux and FreeBSD use.
 */
typedef struct cpu_context {
    uint64_t rbx, rbp, r12, r13, r14, r15;
    uint64_t rsp;
    uint64_t rip;
    uint64_t rflags;
} cpu_context_t;

#endif /* ARCH_X86_64_CONTEXT_H */
