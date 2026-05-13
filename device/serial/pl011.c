/*   device/serial/pl011.c   */

/* PL011 UART driver implementation. */

#include <kernel/device/serial/serial.h>

/* Register offsets (byte offset, accessed as 32-bit) */
#define PL011_DR        0x000   /* Data register */
#define PL011_FR        0x018   /* Flag register */
#define PL011_IBRD      0x024   /* Integer baud rate divisor */
#define PL011_FBRD      0x028   /* Fractional baud rate divisor */
#define PL011_LCR_H     0x02C   /* Line control register */
#define PL011_CR        0x030   /* Control register */
#define PL011_IMSC      0x038   /* Interrupt mask set/clear */

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

#define PL011_MAX_INSTANCES 4

typedef struct pl011_config {
    uint64_t base; /* Base address. */
} pl011_config_t;

static pl011_config_t pl011_config_pool[PL011_MAX_INSTANCES];
static serial_device_t pl011_device_pool[PL011_MAX_INSTANCES];
static int pl011_pool_used = 0;

static inline volatile uint32_t *pl011_reg(uint64_t base, uint32_t offset)
{
    return (volatile uint32_t *)(base + offset);
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

    /* Block while RX FIFO is empty */
    while (*pl011_reg(config->base, PL011_FR) & PL011_FR_RXFE) ;
    return (char)(*pl011_reg(config->base, PL011_DR) & 0xFF);
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
        .base = pl011_base
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
