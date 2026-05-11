/*   include/kernel/device/irq/i8259.h   */

/* Intel 8259 Programmable Interrupt Controller (PIC) definitions. */

#ifndef KERNEL_API_DEVICE_IRQ_I8259_H
#define KERNEL_API_DEVICE_IRQ_I8259_H

#define I8259_IRQ_BASE      0x20
#define I8259_IRQ_VECTOR(n) (IRQ_BASE + (n))

#define I8259_IRQ_TIMER    0
#define I8259_IRQ_KEYBOARD 1
#define I8259_IRQ_CASCADE  2
#define I8259_IRQ_SERIAL2  3
#define I8259_IRQ_SERIAL1  4
#define I8259_IRQ_RTC      8
#define I8259_IRQ_MOUSE    12
#define I8259_IRQ_FPU      13
#define I8259_IRQ_ATA1     14
#define I8259_IRQ_ATA2     15

extern irq_device_t irq_device_i8259;

#endif /* KERNEL_API_DEVICE_IRQ_I8259_H */
