/*   kernel/libc/string.c   */

#include <kernel/libc/string.h>

void *memset(void *dest, int ch, size_t count)
{
    register unsigned char *ptr = (unsigned char *) dest;

    while (count--) {
        *ptr++ = (unsigned char) ch;
    }

    return dest;
};
