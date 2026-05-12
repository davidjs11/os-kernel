/*   device/timer/pit.c   */

/* Intel 8253/8254 Programmable Interval Timer (PIT) driver implementation. */

#include <kernel/device/timer/timer.h>
#include <kernel/device/timer/pit.h>

#ifdef ARCH_X86_64

#define PIT_CHANNEL0    0x40
#define PIT_CMD         0x43
#define PIT_BASE_FREQ   1193182

/* Write a byte to an I/O port. */
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void pit_init(uint32_t freq)
{
    uint16_t divisor = PIT_BASE_FREQ / freq;

    /* Channel 0, lobyte/hibyte, rate generator (mode 2). */
    outb(PIT_CMD, 0x34);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

static uint32_t pit_irq(void)
{
    /* PIT is always wired to IRQ0. */
    return 0;
}

static void pit_ack(void)
{
    /* Mode 2 repeats by itself, nothing to rearm. */
}

timer_device_t timer_device_pit = (timer_device_t) {
    .init = pit_init,
    .irq  = pit_irq,
    .ack  = pit_ack
};

#endif /* ARCH_X86_64 */
