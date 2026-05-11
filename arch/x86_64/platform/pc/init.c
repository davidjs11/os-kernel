/*   arch/x86_64/platform/pc/init.c   */

/* Platform-specific initialization code implementation for x86_64/pc. */

#include <kernel/platform/init.h>
#include <kernel/platform/timer.h>
#include <kernel/ktimer.h>
#include <kernel/kprintf.h>
#include <kernel/device/irq/irq.h>
#include <kernel/device/irq/i8259.h>
#include <kernel/device/serial/serial.h>
#include <kernel/device/serial/uart16550.h>

#define SERIAL_COM1_PORT 0x3F8

void platform_init(void)
{
    /* Serial port. */
    serial_device_t *uart16550_device = uart16550_register(SERIAL_COM1_PORT);
    serial_handle_t handle = serial_register(uart16550_device);
    serial_set_console_handle(handle);
    kprintf("[pc] Initialized UART16550 serial port.\n");

    /* Initialize interrupt controller. */
    irq_register(&irq_device_i8259);
    irq_init();
    kprintf("[pc] Initialized Intel 8259 PIC.\n");
}
