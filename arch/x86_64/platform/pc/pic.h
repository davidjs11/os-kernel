/*   arch/x86_64/platform/pc/pic.h   */

/*
 * Intel 8259 Programmable Interrupt Controller (PIC) definitions.
 */

#define IRQ_BASE        0x20
#define IRQ_VECTOR(n)   (IRQ_BASE + (n))

#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1
#define IRQ_CASCADE     2
#define IRQ_SERIAL2     3
#define IRQ_SERIAL1     4
#define IRQ_RTC         8
#define IRQ_MOUSE       12
#define IRQ_FPU         13
#define IRQ_ATA1        14
#define IRQ_ATA2        15
