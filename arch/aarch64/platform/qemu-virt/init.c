/*   arch/aarch64/platform/qemu-virt/init.c   */

/* Platform-specific initialization code implementation for qemu-virt board. */

#include <kernel/platform/init.h>
#include <kernel/platform/irq.h>
#include <kernel/platform/timer.h>
#include <kernel/ktimer.h>
#include <kernel/kprintf.h>
#include <kernel/device/irq/irq.h>
#include <kernel/device/irq/arm_gicv2.h>
#include <kernel/device/serial/serial.h>
#include <kernel/device/serial/pl011.h>

/* QEMU-virt base addresses. */
#define GICD_BASE  0x08000000UL
#define GICC_BASE  0x08010000UL
#define PL011_BASE 0x09000000UL


void platform_init(void)
{
    /* Initialize serial port. */
    serial_device_t *pl011_device = pl011_register(PL011_BASE);
    serial_handle_t handle = serial_register(pl011_device);
    serial_set_console_handle(handle);
    kprintf("[qemu-virt] Initialized PL011 UART.\n");

    /* Initialize interrupt controller. */
    arm_gicv2_config(GICD_BASE, GICC_BASE);
    irq_register(&irq_device_arm_gicv2);
    irq_init();
    kprintf("[qemu-virt] Initialized ARM GICv2.\n"),

    /* Test IRQ. */
    kprintf("[qemu-virt] Sending 3 SGI interrupts to this CPU.\n");
    arm_gicv2_trigger(0);
    arm_gicv2_trigger(1);
    arm_gicv2_trigger(2);
}
