/*   arch/aarch64/platform/qemu-virt/serial.c   */

/* Platform-specific serial port API implementation for qemu-virt board. */

#include <kernel/platform/serial.h>
#include <stdint.h>
#include <stdbool.h>

static uint8_t *uart_base = (uint8_t *) 0x09000000;

void platform_serial_init(void)
{

}

void platform_serial_putc(char c)
{
    *uart_base = c;
}

void platform_serial_puts(const char *s)
{
    while (*s) {
        platform_serial_putc(*s++);
    }
}
