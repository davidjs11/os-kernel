/*   kernel/syscall/sys_exit.c   */

/* sys_exit syscall implementation. */

#include <kernel/syscall.h>
#include <kernel/sched.h>

uint64_t sys_exit(
        uint64_t code,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t,
        uint64_t)
{
    sched_exit((int) code);

    /* Never reached. */
    return 0;
}
