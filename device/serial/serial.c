/*   device/serial/serial.c   */

/* Serial port driver implementation. */

#include <kernel/device/serial/serial.h>

/* Array of registered serial devices. */
static serial_device_t *serial_devs[SERIAL_MAX_DEVICES];
static serial_handle_t serial_count = 0;

/* Default console serial port handle. */
static serial_handle_t serial_console = -1;

serial_handle_t serial_register(serial_device_t *dev)
{
    if (serial_count >= SERIAL_MAX_DEVICES) {
        return -1; /* Invalid handle. */
    }

    serial_devs[serial_count] = dev;
    return serial_count++;
}

void serial_set_console_handle(serial_handle_t serial_handle)
{
    if (serial_handle >= 0 && serial_handle < serial_count) {
        serial_console = serial_handle;
    }
}

void serial_putc_to(serial_handle_t serial_handle, char c)
{
    if (serial_handle >= 0 && serial_handle < serial_count) {
        serial_devs[serial_handle]->putc(
            serial_devs[serial_handle]->data,
            c
        );
    }
}

void serial_puts_to(serial_handle_t serial_handle, const char *s)
{
    if (serial_handle >= 0 && serial_handle < serial_count) {
        while (*s) {
            serial_devs[serial_handle]->putc(
                serial_devs[serial_handle]->data,
                *s++
            );
        }
    }
}

char serial_getc_from(serial_handle_t serial_handle)
{
    if (serial_handle >= 0 && serial_handle < serial_count) {
        return serial_devs[serial_handle]->getc(
            serial_devs[serial_handle]->data
        );
    }
    return '\0';
}

int serial_ready_on(serial_handle_t serial_handle)
{
    if (serial_handle >= 0 && serial_handle < serial_count) {
        return serial_devs[serial_handle]->ready(
            serial_devs[serial_handle]->data
        );
    }
    return 0;
}

void serial_putc(char c)
{
    serial_putc_to(serial_console, c);
}

void serial_puts(const char *s)
{
    serial_puts_to(serial_console, s);
}

char serial_getc(void)
{
    return serial_getc_from(serial_console);
}

int serial_ready(void)
{
    return serial_ready_on(serial_console);
}
