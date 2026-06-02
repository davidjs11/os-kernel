/*   device/serial/pl011.c   */

/* PL011 UART driver implementation. */

#include <kernel/device/serial/serial.h>
#include <kernel/device/serial/pl011.h>
#include <kernel/device/irq/irq.h>
#include <kernel/interrupt.h>
#include <kernel/sched.h>

/* Register offsets (byte offset, accessed as 32-bit) */
#define PL011_DR        0x000   /* Data register */
#define PL011_FR        0x018   /* Flag register */
#define PL011_IBRD      0x024   /* Integer baud rate divisor */
#define PL011_FBRD      0x028   /* Fractional baud rate divisor */
#define PL011_LCR_H     0x02C   /* Line control register */
#define PL011_CR        0x030   /* Control register */
#define PL011_IMSC      0x038   /* Interrupt mask set/clear */
#define PL011_ICR       0x044   /* Interrupt clear register */

/* FR bits */
#define PL011_FR_TXFF   (1 << 5)    /* TX FIFO full */
#define PL011_FR_RXFE   (1 << 4)    /* RX FIFO empty */
#define PL011_FR_BUSY   (1 << 3)    /* UART busy */
#define PL011_FR_TXFE   (1 << 7)    /* TX FIFO empty (TX idle) */

/* LCR_H bits */
#define PL011_LCR_WLEN8 (0x3 << 5)  /* 8-bit word length */
#define PL011_LCR_FEN   (1 << 4)    /* Enable FIFOs */

/* CR bits */
#define PL011_CR_UARTEN (1 << 0)    /* UART enable */
#define PL011_CR_TXE    (1 << 8)    /* TX enable */
#define PL011_CR_RXE    (1 << 9)    /* RX enable */

/* IMSC and ICR bits */
#define PL011_RXIM      (1 << 4)    /* RX interrupt */

#define PL011_MAX_INSTANCES 4

typedef struct pl011_config {
    uint64_t base;           /* Base address.                      */
    bool     rx_irq_enabled; /* RX is interrupt driven.            */
    char     rx_buf;         /* Last received character.           */
    bool     rx_ready;       /* rx_buf holds an unread character.  */
    task_t   *waiter;        /* Task blocked waiting for a char.   */
} pl011_config_t;

static pl011_config_t pl011_config_pool[PL011_MAX_INSTANCES];
static serial_device_t pl011_device_pool[PL011_MAX_INSTANCES];
static int pl011_pool_used = 0;

/* Device that owns the RX IRQ. */
static pl011_config_t *rx_dev = NULL;

static inline volatile uint32_t *pl011_reg(uint64_t base, uint32_t offset)
{
    return (volatile uint32_t *)(base + offset);
}

static void pl011_rx_handler(void)
{
    if (rx_dev == NULL) {
        return;
    }

    rx_dev->rx_buf = (char) (*pl011_reg(rx_dev->base, PL011_DR) & 0xFF);
    *pl011_reg(rx_dev->base, PL011_ICR) = PL011_RXIM;
    rx_dev->rx_ready = true;

    /* Wake up the waiting task. */
    if (rx_dev->waiter != NULL) {
        sched_unblock(rx_dev->waiter);
        rx_dev->waiter = NULL;
    }
}

static void pl011_init(void *data)
{
    pl011_config_t *config = (pl011_config_t *) data;

    /* Disable UART before reconfiguring. */
    *pl011_reg(config->base, PL011_CR) = 0;

    /* Wait for any ongoing transmission to finish. */
    while (*pl011_reg(config->base, PL011_FR) & PL011_FR_BUSY);

    /* Mask all interrupts. */
    *pl011_reg(config->base, PL011_IMSC) = 0;

    /* TODO: Don't hardcode baudrate. */
    *pl011_reg(config->base, PL011_IBRD) = 13;
    *pl011_reg(config->base, PL011_FBRD) = 1;

    /* 8n1, FIFOs enabled. */
    *pl011_reg(config->base, PL011_LCR_H) = PL011_LCR_WLEN8 | PL011_LCR_FEN;

    /* Enable UART, TX, RX. */
    *pl011_reg(config->base, PL011_CR) =
        PL011_CR_UARTEN | PL011_CR_TXE | PL011_CR_RXE;
}

static void pl011_putc(void *data, char c)
{
    pl011_config_t *config = (pl011_config_t *) data;

    /* Block while TX FIFO is full */
    while (*pl011_reg(config->base, PL011_FR) & PL011_FR_TXFF);
    *pl011_reg(config->base, PL011_DR) = (uint32_t)c;
}

static char pl011_getc(void *data)
{
    pl011_config_t *config = (pl011_config_t *) data;

    /*
     * Busy wait if RX is not interrupt driven, if there is no task to block
     * or if another task is already waiting.
     */
    if (!config->rx_irq_enabled || sched_current() == NULL
    || (!config->rx_ready && config->waiter != NULL)) {
        while (*pl011_reg(config->base, PL011_FR) & PL011_FR_RXFE);
        return (char)(*pl011_reg(config->base, PL011_DR) & 0xFF);
    }

    /* Block until the RX handler receives a character. */
    if (!config->rx_ready) {
        config->waiter = sched_current();
        sched_block();
    }

    config->rx_ready = false;
    return config->rx_buf;
}

static int pl011_ready(void *data)
{
    pl011_config_t *config = (pl011_config_t *) data;

    /* TX ready = TX FIFO not full */
    return !(*pl011_reg(config->base, PL011_FR) & PL011_FR_TXFF);
}

serial_device_t *pl011_register(uint64_t pl011_base)
{
    if (pl011_pool_used >= PL011_MAX_INSTANCES) {
        return NULL;
    }

    pl011_config_pool[pl011_pool_used] = (pl011_config_t) {
        .base           = pl011_base,
        .rx_irq_enabled = false,
        .rx_buf         = '\0',
        .rx_ready       = false,
        .waiter         = NULL,
    };

    pl011_device_pool[pl011_pool_used] = (serial_device_t) {
        .init  = pl011_init,
        .putc  = pl011_putc,
        .getc  = pl011_getc,
        .ready = pl011_ready,
        .data  = &pl011_config_pool[pl011_pool_used],
    };

    return &pl011_device_pool[pl011_pool_used++];
}

void pl011_enable_rx_irq(serial_device_t *dev, uint32_t irq)
{
    pl011_config_t *config = (pl011_config_t *) dev->data;

    rx_dev = config;
    *pl011_reg(config->base, PL011_IMSC) = PL011_RXIM;
    irq_set(irq, pl011_rx_handler);
    irq_unmask(irq);
    config->rx_irq_enabled = true;
}
