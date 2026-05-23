/*   device/serial/uart16550.c   */

/* UART16550 UART driver implementation. */

#include <kernel/device/serial/serial.h>
#include <kernel/device/serial/uart16550.h>
#include <kernel/device/irq/irq.h>
#include <kernel/interrupt.h>
#include <kernel/sched.h>

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
#define UART_IER_RDA    (1 << 0) /* Received data available interrupt. */

#define UART16550_MAX_INSTANCES 4

typedef struct uart16550_config {
    uint16_t port;           /* Base I/O port.                     */
    bool     rx_irq_enabled; /* RX is interrupt driven.            */
    char     rx_buf;         /* Last received character.           */
    bool     rx_ready;       /* rx_buf holds an unread character.  */
    task_t   *waiter;        /* Task blocked waiting for a char.   */
} uart16550_config_t;

static uart16550_config_t uart16550_config_pool[UART16550_MAX_INSTANCES];
static serial_device_t uart16550_device_pool[UART16550_MAX_INSTANCES];
static int uart16550_pool_used = 0;

/* Device that owns the RX IRQ. */
static uart16550_config_t *rx_dev = NULL;

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

static void uart16550_rx_handler(void)
{
    if (rx_dev == NULL) {
        return;
    }

    rx_dev->rx_buf = (char) inb(rx_dev->port + UART_RBR);
    rx_dev->rx_ready = true;

    /* Wake up the waiting task. */
    if (rx_dev->waiter != NULL) {
        sched_unblock(rx_dev->waiter);
        rx_dev->waiter = NULL;
    }
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
    uart16550_config_t *config = (uart16550_config_t *) data;

    /*
     * Busy wait if RX is not interrupt driven, if there is no task to block
     * or if another task is already waiting.
     */
    if (!config->rx_irq_enabled || sched_current() == NULL
    || (!config->rx_ready && config->waiter != NULL)) {
        while (!(inb(config->port + UART_LSR) & UART_LSR_DR));
        return (char) inb(config->port + UART_RBR);
    }

    /* Block until the RX handler receives a character. */
    if (!config->rx_ready) {
        config->waiter = sched_current();
        sched_block();
    }

    config->rx_ready = false;
    return config->rx_buf;
}

static int uart16550_ready(void *data)
{
    uint16_t port = ((uart16550_config_t *) data)->port;

    return !!(inb(port + UART_LSR) & UART_LSR_THRE);
}

serial_device_t *uart16550_register(uint16_t port)
{
    if (uart16550_pool_used >= UART16550_MAX_INSTANCES) {
        return NULL;
    }

    uart16550_config_pool[uart16550_pool_used] = (uart16550_config_t) {
        .port           = port,
        .rx_irq_enabled = false,
        .rx_buf         = '\0',
        .rx_ready       = false,
        .waiter         = NULL,
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

void uart16550_enable_rx_irq(serial_device_t *dev, uint32_t irq)
{
    uart16550_config_t *config = (uart16550_config_t *) dev->data;

    rx_dev = config;
    outb(config->port + UART_IER, UART_IER_RDA);
    irq_set(irq, uart16550_rx_handler);
    irq_unmask(irq);
    config->rx_irq_enabled = true;
}

#endif /* ARCH_X86_64 */
