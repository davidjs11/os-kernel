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

#endif /* KERNEL_API_DEVICE_SERIAL_UART16550_H */
