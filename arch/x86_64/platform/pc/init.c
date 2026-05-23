/*   arch/x86_64/platform/pc/init.c   */

/* Platform-specific initialization code implementation for x86_64/pc. */

#include <kernel/platform/init.h>
#include <kernel/platform/mem.h>
#include <kernel/kprintf.h>
#include <kernel/device/irq/irq.h>
#include <kernel/device/irq/i8259.h>
#include <kernel/device/timer/timer.h>
#include <kernel/device/timer/pit.h>
#include <kernel/device/serial/serial.h>
#include <kernel/device/serial/uart16550.h>
#include <kernel/device/framebuffer/framebuffer.h>
#include <kernel/device/framebuffer/limine_framebuffer.h>

#define SERIAL_COM1_PORT 0x3F8

void platform_init(void)
{
    /* Serial port. */
    serial_device_t *uart16550_device = uart16550_register(SERIAL_COM1_PORT);
    serial_handle_t serial_handle = serial_register(uart16550_device);
    serial_set_console_handle(serial_handle);
    serial_init(serial_handle);
    kprintf("[pc] Initialized UART16550 serial port.\n");

    /* Framebuffer. */
    framebuffer_device_t *framebuffer_device = limine_framebuffer_register(
        platform_mem_get_framebuffer_base()
    );
    framebuffer_handle_t fb_handle = framebuffer_register(framebuffer_device);
    framebuffer_init(fb_handle);
    framebuffer_set_default_handle(fb_handle);
    kprintf("[pc] Initialized framebuffer.\n");

    /* Initialize interrupt controller. */
    irq_register(&irq_device_i8259);
    irq_init();
    kprintf("[pc] Initialized Intel 8259 PIC.\n");

    /* Serial port RX interrupt. */
    uart16550_enable_rx_irq(uart16550_device, I8259_IRQ_SERIAL1);
    kprintf(
        "[pc] Enabled UART16550 RX interrupt on IRQ%d.\n",
        I8259_IRQ_SERIAL1
    );

    /* Timer. */
    timer_register(&timer_device_pit);
    kprintf("[pc] Registered Intel 8254 PIT.\n");
}
