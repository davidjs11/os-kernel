/*   arch/aarch64/platform/qemu-virt/init.c   */

/* Platform-specific initialization code implementation for qemu-virt board. */

#include <kernel/platform/init.h>
#include <kernel/platform/mem.h>
#include <kernel/kprintf.h>
#include <kernel/device/irq/irq.h>
#include <kernel/device/irq/arm_gicv2.h>
#include <kernel/device/serial/serial.h>
#include <kernel/device/serial/pl011.h>

/* QEMU-virt physical base addresses. */
#define GICD_BASE  0x08000000UL
#define GICC_BASE  0x08010000UL
#define PL011_BASE 0x09000000UL

void platform_init(void)
{
    /*
     * Devices are accessed through the HHDM, so they are still mapped after
     * switching to the kernel's own page tables.
     */
    uint64_t hhdm = platform_mem_get_hhdm_vaddr();

    /* Initialize serial port. */
    serial_device_t *pl011_device = pl011_register(hhdm + PL011_BASE);
    serial_handle_t handle = serial_register(pl011_device);
    serial_set_console_handle(handle);
    serial_init(handle);
    kprintf("[qemu-virt] Initialized PL011 UART.\n");

    /* Initialize interrupt controller. */
    arm_gicv2_config(hhdm + GICD_BASE, hhdm + GICC_BASE);
    irq_register(&irq_device_arm_gicv2);
    irq_init();
    kprintf("[qemu-virt] Initialized ARM GICv2.\n");
}
