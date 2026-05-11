/*   x86_64/platform/pc/timer_pit.c   */

/*
 * Intel Programmable Interval Timer (PIT) driver implementing timer kernel
 * API.
 */

#include <stdint.h>
#include <kernel/arch/interrupt.h>
#include <kernel/platform/timer.h>
#include <kernel/platform/irq.h>
#include <kernel/ktimer.h>
#include "../io.h"

#define PIT_CHANNEL0    0x40
#define PIT_CMD         0x43
#define PIT_BASE_FREQ   1193182

static volatile uint64_t ticks = 0;

void platform_timer_init(uint32_t freq)
{
    uint16_t divisor = PIT_BASE_FREQ / freq;

    /* Channel 0, lobyte/hibyte, rate generator (mode 2). */
    outb(PIT_CMD, 0x34);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

uint64_t platform_timer_ticks(void)
{
    return ticks;
}
