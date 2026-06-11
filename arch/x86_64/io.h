/*   arch/x86_64/io.h   */

/* x86_64 Input/Output utilities. */

#ifndef ARCH_X86_64_IO_H
#define ARCH_X86_64_IO_H

#include <kernel/types.h>

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

static inline void io_wait(void)
{
    outb(0x80, 0x00);   /* Write to unused port, gives PIC time to settle. */
}

#endif /* ARCH_X86_64_IO_H */
