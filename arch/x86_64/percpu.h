/*   arch/x86_64/percpu.h   */

/* CPU unique parameters. Used by system call entry point. */

#ifndef ARCH_X86_64_PERCPU_H
#define ARCH_X86_64_PERCPU_H

#include <stdint.h>

typedef struct __attribute__((packed)) percpu {
    /*
     * Don't move these elements, as their index is used by the assembler in
     * syscall_entry.S.
     */
    uintptr_t kstack_top; /* Kernel stack top.   */
    uintptr_t user_rsp;   /* User stack pointer. */
} percpu_t;

void percpu_init(void);

#endif /* ARCH_X86_64_PERCPU_H */
