/*   include/kernel/device/serial/uart16550.h   */

/* PL011 UART device driver header. */

#ifndef KERNEL_API_DEVICE_SERIAL_UART16550_H
#define KERNEL_API_DEVICE_SERIAL_UART16550_H

#include <kernel/device/serial/serial.h>

/*
 * @brief Register a UART16550 device.
 *
 * @param port Port index of the UART16550 device.
 *
 * @return A pointer to the serial device struct.
 */
serial_device_t *uart16550_register(uint16_t port);

/*
 * @brief Make RX interrupt driven, so reading blocks the task instead of busy
 *        waiting. IRQ controller must be initialized first.
 *
 * @param dev Device returned by uart16550_register().
 * @param irq IRQ line of the device.
 */
void uart16550_enable_rx_irq(serial_device_t *dev, uint32_t irq);

#endif /* KERNEL_API_DEVICE_SERIAL_UART16550_H */
