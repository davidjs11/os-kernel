/*   kernel/kprintf.c   */

/* Kernel printf function implementation. */

#include <kernel/kprintf.h>
#include <kernel/device/serial/serial.h>
#include <stdarg.h>

static void print_uint(uint64_t n, int base) {
    static const char digits[] = "0123456789abcdef";
    char buf[64];
    int i = 0;

    if (n == 0) {
        serial_putc('0');
        return;
    }

    while (n) {
        buf[i++] = digits[n % base];
        n /= base;
    }
    while (i--) {
        serial_putc(buf[i]);
    }
}

void kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            serial_putc(*fmt);
            continue;
        }
        fmt++;
        switch (*fmt) {
            case 'c':
                serial_putc((char)va_arg(ap, int));
                break;
            case 's':
                serial_puts(va_arg(ap, const char *));
                break;
            case 'd': {
                long n = va_arg(ap, long);
                if (n < 0) {
                    serial_putc('-');
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
                serial_puts("0x");
                print_uint((uint64_t)va_arg(ap, void *), 16);
                break;
            case '%':
                serial_putc('%');
                break;
            default:
                serial_putc('?');
                break;
        }
    }

    va_end(ap);
}
