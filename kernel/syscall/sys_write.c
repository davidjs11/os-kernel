/*   kernel/syscall/sys_write.c   */

/* sys_write syscall implementation. */

#include <kernel/syscall.h>
#include <kernel/device/serial/serial.h>

uint64_t sys_write(
        uint64_t fd,
        uint64_t buf,
        uint64_t count,
        uint64_t,
        uint64_t,
        uint64_t)
{
    /* Only stdout and stderr (serial console) for the moment. */
    if (fd != 1 && fd != 2) {
        return (uint64_t) -1;
    }

    const char *s = (const char *) buf;
    for (uint64_t i = 0; i < count; i++) {
        serial_putc(s[i]);
    }

    return count;
}
