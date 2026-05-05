/*   arch/x86_64/syscall.h   */

/* System call support API. */

#ifndef ARCH_X86_64_SYSCALL_H
#define ARCH_X86_64_SYSCALL_H

#define EFER_SCE        (1 << 0)    /* Syscall enable. */

void syscall_init(void);

#endif /* ARCH_X86_64_SYSCALL_H */
