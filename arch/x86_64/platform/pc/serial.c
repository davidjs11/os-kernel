/*   arch/x86_64/platform/pc/serial.c   */

#include <kernel/platform/serial.h>
#include <stdint.h>
#include <stdbool.h>
#include "../io.h"

#define COM1 0x3F8

static bool serial_initialized = false;

void platform_serial_init(void)
{
    outb(COM1 + 1, 0x00);   /* Disable interrupts. */
    outb(COM1 + 3, 0x80);   /* Enable DLAB. */
    outb(COM1 + 0, 0x03);   /* Divisor lo: 38400 baud. */
    outb(COM1 + 1, 0x00);   /* Divisor hi. */
    outb(COM1 + 3, 0x03);   /* 8 bits, no parity, 1 stop bit. */
    outb(COM1 + 2, 0xC7);   /* Enable FIFO. */
    outb(COM1 + 4, 0x03);   /* RTS/DSR. */
}

void platform_serial_putc(char c)
{
    if (!serial_initialized) {
        platform_serial_init();
        serial_initialized = true;
    }

    /* Wait for transmit buffer empty. */
    while (!(inb(COM1 + 5) & 0x20));

    if (c == '\n') {
        platform_serial_putc('\r');
    }
    outb(COM1, c);
}

void platform_serial_puts(const char *s)
{
    while (*s) {
        platform_serial_putc(*s++);
    }
}
