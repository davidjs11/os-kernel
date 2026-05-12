/*   kernel/ktimer.c   */

/* Kernel timer implementation. */

#include <kernel/ktimer.h>
#include <kernel/device/irq/irq.h>
#include <kernel/interrupt.h>
#include <kernel/device/timer/timer.h>
#include <kernel/kprintf.h>
#include <kernel/sched.h>

static uint64_t ticks;
static uint32_t freq;

void ktimer_tick(void)
{
    timer_ack();
    ticks++;
    sched_tick();
}

void ktimer_init(uint32_t _freq)
{
    uint32_t irq = timer_irq();

    timer_init(_freq);
    irq_set(irq, ktimer_tick);
    irq_unmask(irq);
    freq  = _freq;
    ticks = 0;
    kprintf("[ktimer] Initialized kernel timer.\n");
}

uint64_t ktimer_ticks(void)
{
    return ticks;
}
