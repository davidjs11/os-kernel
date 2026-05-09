/*   arch/aarch64/platform/qemu-virt/gic.c   */

/* ARM Generic Interrupt Controller version 2 management. */

#include <stdint.h>
#include <kernel/kprintf.h>
#include <kernel/arch/interrupt.h>
#include "gic.h"

void gicd_write(uint32_t off, uint32_t val)
{
    *(volatile uint32_t *) (GICD_BASE + off) = val;
}

uint32_t gicd_read(uint32_t off)
{
    return *(volatile uint32_t *) (GICD_BASE + off);
}

void gicc_write(uint32_t off, uint32_t val)
{
    *(volatile uint32_t *) (GICC_BASE + off) = val;
}

uint32_t gicc_read(uint32_t off)
{
    return *(volatile uint32_t *) (GICC_BASE + off);
}

void gic_init(void)
{
    /* Disable distributor before configuring. */
    gicd_write(GICD_CTLR, 0);

    /* Set all interrupt priorities to a middle value. */
    uint32_t num_irqs = ((gicd_read(GICD_TYPER) & 0x1f) + 1) * 32;
    for (uint32_t i = 0; i < num_irqs; i += 4) {
        gicd_write(GICD_IPRIORITYR + i, 0xa0a0a0a0);
    }

    /* Target all SPIs to CPU0. */
    for (uint32_t i = 32; i < num_irqs; i += 4) {
        gicd_write(GICD_ITARGETSR + i, 0x01010101);
    }

    /* Enable distributor. */
    gicd_write(GICD_CTLR, 1);

    /* Set CPU interface priority mask (allow all priorities). */
    gicc_write(GICC_PMR, 0xff);

    /* Enable CPU interface. */
    gicc_write(GICC_CTLR, 1);
}

void gic_handle_irq(void)
{
    /* Acknowledge and get IRQ number. */
    uint32_t iar = gicc_read(GICC_IAR);

    /* Lower 10 bits are the IRQ ID. */
    uint32_t irq = iar & 0x3ff;

    if (irq == 1023) {
        /* Spurious interrupt, ignore. */
        return;
    }

    /* Dispatch IRQ. */
    switch (irq) {
        default:
            kprintf("Unhandled IRQ: %u\n", irq);
            break;
    }

    /* Signal end of interrupt. */
    gicc_write(GICC_EOIR, iar);
}
