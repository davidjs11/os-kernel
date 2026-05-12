/*   kernel/kprintf.c   */

/* Kernel printf function implementation. */

#include <kernel/ktimer.h>
#include <kernel/device/irq/irq.h>
#include <kernel/interrupt.h>
#include <kernel/platform/timer.h>
#include <kernel/kprintf.h>
#include <kernel/sched.h>

static uint64_t ticks;
static uint32_t freq;

void ktimer_tick(void)
{
    ticks++;
    sched_tick();
}

void ktimer_init(uint32_t _freq)
{
    platform_timer_init(_freq);
    irq_set(0, ktimer_tick);
    irq_unmask(0);
    freq  = _freq;
    ticks = 0;
    kprintf("[ktimer] Initialized kernel timer.\n");
}

uint64_t ktimer_ticks(void)
{
    return ticks;
}
