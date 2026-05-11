/*   x86_64/platform/pc/irq_pic.c   */

/* ARM's Generic Interrupt Controller (GIC) version 2 driver implementation. */

#include <kernel/device/irq/irq.h>
#include <kernel/device/irq/arm_gicv2.h>

#include <stdint.h>
#include <kernel/platform/irq.h>
#include <kernel/arch/interrupt.h>
#include <kernel/kprintf.h>

/* Distributor ports. */
#define GICD_CTLR       0x000 /* Distributor Control.           */
#define GICD_TYPER      0x004 /* Interrupt Controller Type.     */
#define GICD_ISENABLER  0x100 /* Interrupt Set-Enable.          */
#define GICD_ICENABLER  0x180 /* Interrupt Set-Enable.          */
#define GICD_ISPENDR    0x200 /* Interrupt Set-Pending.         */
#define GICD_IPRIORITYR 0x400 /* Interrupt Priority.            */
#define GICD_ITARGETSR  0x800 /* Interrupt Targets (which CPU). */
#define GICD_ICFGR      0xC00 /* Interrupt Configuration.       */
#define GICD_SGIR       0xF00 /* Software Generated Interrupt.  */

/* CPU interface ports. */
#define GICC_CTLR       0x000 /* CPU interface control.  */
#define GICC_PMR        0x004 /* Priority mask.          */
#define GICC_IAR        0x00C /* Interrupt acknowledge.  */
#define GICC_EOIR       0x010 /* End Of Interrupt (EOI). */

/* Driver private data. */
static uint64_t gicd_base;
static uint64_t gicc_base;
static uint32_t last_iar = 0;

static void gicd_write(uint32_t off, uint32_t val)
{
    *(volatile uint32_t *) (gicd_base + off) = val;
}

static uint32_t gicd_read(uint32_t off)
{
    return *(volatile uint32_t *) (gicd_base + off);
}

static void gicc_write(uint32_t off, uint32_t val)
{
    *(volatile uint32_t *) (gicc_base + off) = val;
}

static uint32_t gicc_read(uint32_t off)
{
    return *(volatile uint32_t *) (gicc_base + off);
}

void arm_gicv2_config(uint64_t _gicd_base, uint64_t _gicc_base)
{
    gicd_base = _gicd_base;
    gicc_base = _gicc_base;
}

void arm_gicv2_init(void)
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

void arm_gicv2_mask(uint32_t irq)
{
    uint32_t reg = irq / 32;
    uint32_t bit = irq % 32;
    gicd_write(GICD_ICENABLER + reg * 4, 1 << bit);
}

void arm_gicv2_unmask(uint32_t irq)
{
    uint32_t reg = irq / 32;
    uint32_t bit = irq % 32;
    gicd_write(GICD_ISENABLER + reg * 4, 1 << bit);
}

uint32_t arm_gicv2_ack(void)
{
    /* Acknowledge and get IRQ number. */
    uint32_t iar = gicc_read(GICC_IAR);

    /* Lower 10 bits are the IRQ ID. */
    uint32_t irq = iar & 0x3ff;

    if (irq == 1023) {
        /* Spurious interrupt, ignore. */
        return 0xffffffff;
    }

    last_iar = iar;
    return irq;
}

void arm_gicv2_eoi(uint32_t irq)
{
    gicc_write(GICC_EOIR, last_iar);
}

void arm_gicv2_trigger(uint32_t intid) {
    /* Send a SGI interrupt having this CPU as a target. */
    gicd_write(GICD_SGIR, (0x2 << 24) | (intid & 0xF));
}

/* Device implementation. */
irq_device_t irq_device_arm_gicv2 = (irq_device_t) {
    .init   = arm_gicv2_init,
    .mask   = arm_gicv2_mask,
    .unmask = arm_gicv2_unmask,
    .ack    = arm_gicv2_ack,
    .eoi    = arm_gicv2_eoi
};
