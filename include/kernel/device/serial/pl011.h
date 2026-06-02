/*   include/kernel/device/serial/pl011.h   */

/* PL011 UART device driver header. */

#ifndef KERNEL_API_DEVICE_SERIAL_PL011_H
#define KERNEL_API_DEVICE_SERIAL_PL011_H

#include <kernel/device/serial/serial.h>

/*
 * @brief Register a PL011 device given its base address in MMIO.
 *
 * @param pl011_base Base address of the PL011 to register.
 *
 * @return A pointer to the serial device struct.
 */
serial_device_t *pl011_register(uint64_t pl011_base);

/*
 * @brief Make RX interrupt driven, so reading blocks the task instead of busy
 *        waiting. IRQ controller must be initialized first.
 *
 * @param dev Device returned by pl011_register().
 * @param irq GIC interrupt ID of the device.
 */
void pl011_enable_rx_irq(serial_device_t *dev, uint32_t irq);

#endif /* KERNEL_API_DEVICE_SERIAL_PL011_H */
