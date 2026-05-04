/*   kernel/kprintf.c   */

/* Kernel printf function implementation. */

#include <kernel/ktimer.h>
#include <kernel/irq.h>
#include <kernel/platform/timer.h>
#include <kernel/kprintf.h>

static uint64_t ticks;
static uint32_t freq;

void ktimer_tick(void)
{
    ticks++;
}

void ktimer_init(uint32_t _freq)
{
    platform_timer_init(_freq);
    irq_set(0, ktimer_tick);
    irq_unmask(0);
    freq  = _freq;
    ticks = 0;
}

uint64_t ktimer_ticks(void)
{
    return ticks;
}
