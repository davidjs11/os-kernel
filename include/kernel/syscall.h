/*   include/kernel/syscall.h   */

/* Kernel system call API. */

#ifndef KERNEL_API_SYSCALL_H
#define KERNEL_API_SYSCALL_H

#include <stdint.h>

/* Syscall number definition. */
typedef enum syscall_num {
    SYS_EXIT   = 0,
    SYS_WRITE  = 1,
    SYS_READ   = 2,
    SYS_GETPID = 3,

    /* If SYSCALL_MAX changes, assembly syscall entry should be updated. */
    SYSCALL_MAX,
} syscall_num_t;

/*
 * All syscall handlers have this signature. Arguments map to the syscall
 * calling convention:
 *     arg0 = rdi                                  (x86_64) / x0 (aarch64)
 *     arg1 = rsi                                  (x86_64) / x1 (aarch64)
 *     arg2 = rdx                                  (x86_64) / x2 (aarch64)
 *     arg3 = rcx (moved from r10 by entry stub)   (x86_64) / x3 (aarch64)
 *     arg4 = r8                                   (x86_64) / x4 (aarch64)
 *     arg5 = r9                                   (x86_64) / x5 (aarch64)
 * Return value in rax (x86_64) / x0 (aarch64).
 */
typedef uint64_t (*syscall_handler_t)(
    uint64_t arg0,
    uint64_t arg1,
    uint64_t arg2,
    uint64_t arg3,
    uint64_t arg4,
    uint64_t arg5);

extern syscall_handler_t syscall_table[SYSCALL_MAX];

uint64_t sys_exit(
        uint64_t code,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t);

uint64_t sys_write (
        uint64_t fd,
        uint64_t buf,
        uint64_t count,
        uint64_t,
        uint64_t,
        uint64_t);

uint64_t sys_read(
        uint64_t fd,
        uint64_t buf,
        uint64_t count,
        uint64_t,
        uint64_t,
        uint64_t);

uint64_t sys_getpid(
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t);

#endif /* KERNEL_API_SYSCALL_H */
