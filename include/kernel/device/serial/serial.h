/*   include/kernel/device/serial/serial.h   */

/* Serial port driver API. */

#ifndef KERNEL_API_DEVICE_SERIAL_H
#define KERNEL_API_DEVICE_SERIAL_H

#include <kernel/types.h>

#define SERIAL_MAX_DEVICES 4

/* 
 * Each serial device will have a handle (identifier) used by different parts
 * of the kernel. This will enable output to different serial ports.
 */
typedef uint8_t serial_handle_t;

typedef struct serial_device {
    void (*init)(void *data);
    void (*putc)(void *data, char c);
    char (*getc)(void *data);
    int  (*ready)(void *data);
    void *data;
} serial_device_t;

/*
 * @brief Register a new serial device.
 *
 * @param dev Device to register as the serial device.
 *
 * @return A handler for the new registered serial.
 */
serial_handle_t serial_register(serial_device_t *dev);

/*
 * @brief Set the default handle for the console output.
 *
 * @param dev Device to register as the serial device.
 */
void serial_set_console_handle(serial_handle_t h);

/*
 * @brief Output a character through the default console.
 *
 * @param c Character to output.
 */
void serial_putc(char c);

/*
 * @brief Output a string through the default console.
 *
 * @param s String to output.
 */
void serial_puts(const char *s);

/*
 * @brief Obtain a character from the default console.
 * TODO: Add blocking/nonblocking separation.
 *
 * @return The obtained character from the default console.
 */
char serial_getc(void);

/*
 * @brief 
 */
int serial_ready(void);

/*
 * @brief Output a character to a certain serial port.
 *
 * @param c             Character to output.
 * @parma serial_handle Handle of the serial port.
 */
void serial_putc_to(serial_handle_t serial_handle, char c);

/*
 * @brief Output a string to a certain serial port.
 *
 * @param s             String to output.
 * @parma serial_handle Handle of the serial port.
 */
void serial_puts_to(serial_handle_t serial_handle, const char *s);

/*
 * @brief Input a character from a certain serial port.
 *
 * @parma serial_handle Handle of the serial port.
 *
 * @return The obtained character from the serial port.
 */
char serial_getc_from(serial_handle_t serial_handle);

/*
 * @brief 
 */
int serial_ready_on(serial_handle_t serial_handle);

#endif /* KERNEL_API_DEVICE_SERIAL_H */
