/*   x86_64/platform/pc/irq_pic.c   */

/* Intel 8259 Programmable Interrupt Controller (PIC) driver implementation. */

#include <kernel/device/irq/irq.h>
#include <kernel/device/irq/i8259.h>
#include <stdint.h>
#include <kernel/arch/interrupt.h>
#include <kernel/kprintf.h>

#ifdef ARCH_X86_64

/* Write a byte to an I/O port. */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Read a byte from an I/O port. */
static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

/* Wait by writing to an unused I/O port. */
static inline void io_wait(void) {
    outb(0x80, 0x00);   /* write to unused port — gives PIC time to settle */
}

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
}

static uint16_t pic_read_isr(void)
{
    outb(PIC1_CMD, PIC_READ_ISR);
    outb(PIC2_CMD, PIC_READ_ISR);
    return ((uint16_t) inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

static void i8259_init(void)
{
    pic_remap(I8259_IRQ_BASE, I8259_IRQ_BASE + 8);

    /* Mask all IRQs by default, caller unmasks what it needs. */
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

static void i8259_mask(uint32_t irq)
{
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t  bit  = irq & 0x07;
    outb(port, inb(port) | (1 << bit));
}

static void i8259_unmask(uint32_t irq)
{
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t  bit  = irq & 0x07;

    /* Unmask cascade line on master when unmasking any slave IRQ. */
    if (irq >= 8) {
        outb(PIC1_DATA, inb(PIC1_DATA) & ~(1 << I8259_IRQ_CASCADE));
    }
    outb(port, inb(port) & ~(1 << bit));
}

static uint32_t i8259_ack(void)
{
    /* Read ISR to find which IRQ is active. */
    outb(PIC1_CMD, PIC_READ_ISR);
    outb(PIC2_CMD, PIC_READ_ISR);
    uint8_t isr1 = inb(PIC1_CMD);
    uint8_t isr2 = inb(PIC2_CMD);

    /* IRQ 8-15. */
    if (isr2 != 0) {
        return 8 + __builtin_ctz(isr2);
    }

    /* IRQ 0-7. */
    if (isr1 != 0) {
        return __builtin_ctz(isr1);
    }

    return 0xffffffff;
}

static void i8259_eoi(uint32_t irq)
{
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

/* Device implementation. */
irq_device_t irq_device_i8259 = (irq_device_t) {
    .init   = i8259_init,
    .mask   = i8259_mask,
    .unmask = i8259_unmask,
    .ack    = i8259_ack,
    .eoi    = i8259_eoi
};

#endif /* ARCH_X86_64 */
