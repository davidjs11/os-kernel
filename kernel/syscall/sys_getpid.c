/*   kernel/syscall/sys_getpid.c   */

/* sys_getpid syscall implementation. */

#include <kernel/syscall.h>
#include <kernel/sched.h>

uint64_t sys_getpid(
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t)
{
    return (uint64_t) sched_current()->pid;
}
