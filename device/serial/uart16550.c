/*   device/serial/uart16550.c   */

/* UART16550 UART driver implementation. */

#include <kernel/device/serial/serial.h>

#ifdef ARCH_X86_64

/* Register offsets from base port */
#define UART_RBR 0 /* Receive buffer   (DLAB=0, read).  */
#define UART_THR 0 /* Transmit holding (DLAB=0, write). */
#define UART_IER 1
#define UART_FCR 2
#define UART_LCR 3
#define UART_MCR 4
#define UART_LSR 5
#define UART_DLL 0 /* Divisor latch low  (DLAB=1). */
#define UART_DLH 1 /* Divisor latch high (DLAB=1). */

#define UART_LCR_8N1    0x03
#define UART_LCR_DLAB   (1 << 7)
#define UART_LSR_DR     (1 << 0)
#define UART_LSR_THRE   (1 << 5)
#define UART_FCR_ENABLE (1 << 0)
#define UART_FCR_CLRRX  (1 << 1)
#define UART_FCR_CLRTX  (1 << 2)

#define UART16550_MAX_INSTANCES 4

typedef struct uart16550_config {
    uint16_t port;
} uart16550_config_t;

static uart16550_config_t uart16550_config_pool[UART16550_MAX_INSTANCES];
static serial_device_t uart16550_device_pool[UART16550_MAX_INSTANCES];
static int uart16550_pool_used = 0;

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static void uart16550_init(void *data)
{
    uint16_t port = ((uart16550_config_t *) data)->port;

    outb(port + UART_IER, 0x00);
    outb(port + UART_LCR, UART_LCR_DLAB);
    outb(port + UART_DLL, 0x01);
    outb(port + UART_DLH, 0x00);
    outb(port + UART_LCR, UART_LCR_8N1);
    outb(port + UART_FCR, UART_FCR_ENABLE | UART_FCR_CLRRX | UART_FCR_CLRTX);
    outb(port + UART_MCR, 0x03);
}

static void uart16550_putc(void *data, char c)
{
    uint16_t port = ((uart16550_config_t *) data)->port;

    /* Wait for buffer empty. */
    while (!(inb(port + UART_LSR) & UART_LSR_THRE));

    if (c == '\n') {
        uart16550_putc(data, '\r');
    }
    outb(port + UART_THR, (uint8_t) c);
}

static char uart16550_getc(void *data)
{
    uint16_t port = ((uart16550_config_t *) data)->port;

    /* Wait for buffer empty. */
    while (!(inb(port + UART_LSR) & UART_LSR_DR));

    return (char) inb(port + UART_RBR);
}

static int uart16550_ready(void *data)
{
    uint16_t port = ((uart16550_config_t *) data)->port;

    return !!(inb(port + UART_LSR) & UART_LSR_THRE);
}

static serial_device_t uart16550_dev = {
    .init  = uart16550_init,
    .putc  = uart16550_putc,
    .getc  = uart16550_getc,
    .ready = uart16550_ready,
};

serial_device_t *uart16550_register(uint16_t port)
{
    if (uart16550_pool_used >= UART16550_MAX_INSTANCES) {
        return NULL;
    }

    uart16550_config_pool[uart16550_pool_used] = (uart16550_config_t) {
        .port = port
    };

    uart16550_device_pool[uart16550_pool_used] = (serial_device_t) {
        .init  = uart16550_init,
        .putc  = uart16550_putc,
        .getc  = uart16550_getc,
        .ready = uart16550_ready,
        .data  = &uart16550_config_pool[uart16550_pool_used],
    };

    return &uart16550_device_pool[uart16550_pool_used++];
}

#endif /* ARCH_X86_64 */
