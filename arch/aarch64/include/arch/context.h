/*   arch/aarch64/context.h   */

/* Architecture-specific context structure definition. */

#ifndef ARCH_AARCH64_CONTEXT_H
#define ARCH_AARCH64_CONTEXT_H

#include <kernel/types.h>

/*
 * Only callee-saved registers need to be in the struct. Caller-saved registers
 * are already on the kernel stack at the point of switching (because the
 * switch happens inside a C function call). This is the same optimization
 * Linux and FreeBSD use.
 */
typedef struct cpu_context {
    uint64_t x19, x20, x21, x22, x23, x24, x25, x26, x27, x28;
    uint64_t fp; /* x29. */
    uint64_t lr; /* x30, where arch_context_switch returns to. */
    uint64_t sp;
} cpu_context_t;

#endif /* ARCH_AARCH64_CONTEXT_H */
