/*   kernel/syscall.c   */

/* Kernel system call module. */

#include <kernel/syscall.h>

syscall_handler_t syscall_table[SYSCALL_MAX] = {
    [SYS_EXIT]    = sys_exit,
    [SYS_WRITE]   = sys_write,
    [SYS_READ]    = sys_read,
    [SYS_GETPID]  = sys_getpid,
};
