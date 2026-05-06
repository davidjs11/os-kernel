/*   kernel/syscall/sys_exit.c   */

/* sys_exit syscall implementation. */

#include <kernel/kprintf.h>
#include <kernel/syscall.h>

uint64_t sys_exit(
        uint64_t code,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t)
{
    kprintf("[!] hello from sys_exit!\n");
}
