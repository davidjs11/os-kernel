/*   x86_64/platform/pc/irq_pic.c   */

/*
 * IRQ kernel API implementation for Intel 8259 Programmable Interrupt
 * Controller (PIC).
 */

#include <stdint.h>
#include <kernel/platform/irq.h>
#include <kernel/arch/interrupt.h>
#include <kernel/kprintf.h>
#include <kernel/irq.h>
#include "pic.h"
#include "../idt.h"
#include "../io.h"

/* 8259 PIC port addresses. */
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

/* Initialization Command Words. */
#define PIC_ICW1_INIT   0x11    /* Start init sequence, ICW4 needed. */
#define PIC_ICW4_8086   0x01    /* 8086 mode.                        */

/* Commands. */
#define PIC_EOI         0x20    /* End Of Interrupt (EOI).           */
#define PIC_READ_ISR    0x0B    /* Read in-service register.         */

/* PIC's IRQ dispatcher. */ 
void pic_handle_irq(interrupt_frame_t *frame)
{
    /* TODO: Translate hardware IRQ vector to virtual IRQ vector. */
    uint8_t irq = frame->vector - 0x20;
    irq_handle(irq);
    platform_irq_eoi(irq);
}

static void pic_remap(uint8_t base_master, uint8_t base_slave)
{
    uint8_t mask1 = inb(PIC1_DATA); /* Save masks. */
    uint8_t mask2 = inb(PIC2_DATA);

    /* ICW1: Start initialization. */
    outb(PIC1_CMD, PIC_ICW1_INIT); io_wait();
    outb(PIC2_CMD, PIC_ICW1_INIT); io_wait();

    /* ICW2: Vector offsets. */
    outb(PIC1_DATA, base_master); io_wait();
    outb(PIC2_DATA, base_slave);  io_wait();

    /* ICW3: Cascading. */
    outb(PIC1_DATA, 0x04); io_wait(); /* slave on IRQ2 */
    outb(PIC2_DATA, 0x02); io_wait(); /* slave ID = 2  */

    /* ICW4: 8086 mode. */
    outb(PIC1_DATA, PIC_ICW4_8086); io_wait();
    outb(PIC2_DATA, PIC_ICW4_8086); io_wait();

    /* Restore masks. */
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);

    /* Set IRQ common handler for all IRQs. */
    for (size_t i = 0x20; i < 0x2f; i++) {
        isr_set_handler(i, pic_handle_irq);
    }
}

static uint16_t pic_read_isr(void)
{
    outb(PIC1_CMD, PIC_READ_ISR);
    outb(PIC2_CMD, PIC_READ_ISR);
    return ((uint16_t) inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

void platform_irq_init(void)
{
    pic_remap(IRQ_BASE, IRQ_BASE + 8);

    /* Mask all IRQs by default, caller unmasks what it needs. */
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void platform_irq_mask(uint8_t irq)
{
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t  bit  = irq & 0x07;
    outb(port, inb(port) | (1 << bit));
}

void platform_irq_unmask(uint8_t irq)
{
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t  bit  = irq & 0x07;

    /* Unmask cascade line on master when unmasking any slave IRQ. */
    if (irq >= 8) {
        outb(PIC1_DATA, inb(PIC1_DATA) & ~(1 << IRQ_CASCADE));
    }
    outb(port, inb(port) & ~(1 << bit));
}

void platform_irq_eoi(uint8_t irq)
{
    /* Check for spurious IRQ before sending EOI. */
    if (irq == 7 && !(pic_read_isr() & (1 << 7))) {
        return;
    }

    /* Still need to ACK master */
    if (irq == 15 && !(pic_read_isr() & (1 << 15))) {
        outb(PIC1_CMD, PIC_EOI);
        return;
    }

    /* ACK slave first. */
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }

    /* Always ACK master. */
    outb(PIC1_CMD, PIC_EOI);
}
