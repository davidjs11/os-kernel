/*   kernel/kprintf.c   */

/* Kernel printf function implementation. */

#include <kernel/kprintf.h>
#include <kernel/platform/serial.h>
#include <stdarg.h>

static void print_uint(uint64_t n, int base) {
    static const char digits[] = "0123456789abcdef";
    char buf[64];
    int i = 0;

    if (n == 0) {
        platform_serial_putc('0');
        return;
    }

    while (n) {
        buf[i++] = digits[n % base];
        n /= base;
    }
    while (i--) {
        platform_serial_putc(buf[i]);
    }
}

void kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            platform_serial_putc(*fmt);
            continue;
        }
        fmt++;
        switch (*fmt) {
            case 'c':
                platform_serial_putc((char)va_arg(ap, int));
                break;
            case 's':
                platform_serial_puts(va_arg(ap, const char *));
                break;
            case 'd': {
                long n = va_arg(ap, long);
                if (n < 0) {
                    platform_serial_putc('-');
                    n = -n;
                }
                print_uint(n, 10);
                break;
            }
            case 'u':
                print_uint(va_arg(ap, unsigned long), 10);
                break;
            case 'x':
                print_uint(va_arg(ap, unsigned long), 16);
                break;
            case 'p':
                platform_serial_puts("0x");
                print_uint((uint64_t)va_arg(ap, void *), 16);
                break;
            case '%':
                platform_serial_putc('%');
                break;
            default:
                platform_serial_putc('?');
                break;
        }
    }

    va_end(ap);
}
