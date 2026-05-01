/*   include/kernel/platform/serial.h   */

/* Kernel public interface for platform serial output. */

#ifndef KERNEL_API_PLATFORM_SERIAL_H
#define KERNEL_API_PLATFORM_SERIAL_H

void platform_serial_init(void);
void platform_serial_putc(char c);
void platform_serial_puts(const char *str);

#endif /* KERNEL_API_PLATFORM_SERIAL_H */
