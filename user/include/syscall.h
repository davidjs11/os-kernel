/*   user/include/syscall.h   */

/*
 * User-space system call wrappers. Calling convention is the same as in the
 * kernel side:
 *     x86_64:  number in rax, arguments in rdi, rsi, rdx, return in rax.
 *     aarch64: number in x8,  arguments in x0,  x1,  x2,  return in x0.
 */

#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

/* Must match syscall_num_t in kernel/syscall.h. */
#define SYS_EXIT   0
#define SYS_WRITE  1
#define SYS_READ   2
#define SYS_GETPID 3

typedef unsigned long usize_t;

#if defined(ARCH_X86_64)

static inline long __syscall3(long num, long arg0, long arg1, long arg2)
{
    register long rax __asm__("rax") = num;
    register long rdi __asm__("rdi") = arg0;
    register long rsi __asm__("rsi") = arg1;
    register long rdx __asm__("rdx") = arg2;

    /*
     * Kernel entry doesn't preserve argument registers, so all of them are
     * marked as modified.
     */
    __asm__ volatile (
        "syscall"
        : "+r" (rax), "+r" (rdi), "+r" (rsi), "+r" (rdx)
        :
        : "rcx", "r11", "r8", "r9", "r10", "memory"
    );

    return rax;
}

#elif defined(ARCH_AARCH64)

static inline long __syscall3(long num, long arg0, long arg1, long arg2)
{
    register long x0 __asm__("x0") = arg0;
    register long x1 __asm__("x1") = arg1;
    register long x2 __asm__("x2") = arg2;
    register long x8 __asm__("x8") = num;

    __asm__ volatile (
        "svc #0"
        : "+r" (x0)
        : "r" (x1), "r" (x2), "r" (x8)
        : "memory", "cc"
    );

    return x0;
}

#else
#error "Unsupported architecture."
#endif

static inline long sys_exit(int code)
{
    return __syscall3(SYS_EXIT, code, 0, 0);
}

static inline long sys_write(int fd, const void *buf, usize_t count)
{
    return __syscall3(SYS_WRITE, fd, (long) buf, (long) count);
}

static inline long sys_read(int fd, void *buf, usize_t count)
{
    return __syscall3(SYS_READ, fd, (long) buf, (long) count);
}

static inline long sys_getpid(void)
{
    return __syscall3(SYS_GETPID, 0, 0, 0);
}

#endif /* USER_SYSCALL_H */
