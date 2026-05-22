/*   kernel/syscall/sys_read.c   */

/* sys_read syscall implementation. */

#include <kernel/syscall.h>
#include <kernel/device/serial/serial.h>

uint64_t sys_read(
        uint64_t fd,
        uint64_t buf,
        uint64_t count,
        uint64_t,
        uint64_t,
        uint64_t)
{
    /* Only stdin (serial console) for the moment. */
    if (fd != 0) {
        return (uint64_t) -1;
    }

    if (count == 0) {
        return 0;
    }

    /* Read one character at a time. */
    char *s = (char *) buf;
    s[0] = serial_getc();

    return 1;
}
