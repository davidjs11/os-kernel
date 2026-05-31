/*   device/timer/arm_vtimer.c   */

/* ARM Generic Timer (virtual timer) driver implementation. */

#include <kernel/device/timer/timer.h>
#include <kernel/device/timer/arm_vtimer.h>

#ifdef ARCH_AARCH64

/* Virtual timer PPI in the GIC. */
#define CNTV_INTID 27

/* Timer period in counter ticks. */
static uint32_t period = 0;

static inline uint64_t cntfrq_read(void)
{
    uint64_t val;
    __asm__ volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

static inline void cntv_tval_write(uint32_t val)
{
    __asm__ volatile ("msr cntv_tval_el0, %0" : : "r" ((uint64_t) val));
}

static inline void cntv_ctl_write(uint64_t val)
{
    __asm__ volatile (
        "msr cntv_ctl_el0, %0\n"
        "isb\n"
        :
        : "r" (val)
    );
}

static void arm_vtimer_init(uint32_t freq)
{
    period = (uint32_t) (cntfrq_read() / freq);
    cntv_tval_write(period);
    cntv_ctl_write(1); /* ENABLE = 1, IMASK = 0. */
}

static uint32_t arm_vtimer_irq(void)
{
    return CNTV_INTID;
}

static void arm_vtimer_ack(void)
{
    /*
     * Timer is a countdown and keeps the IRQ line asserted until rearmed, so
     * writing a new count clears the IRQ and schedules the next tick.
     */
    cntv_tval_write(period);
}

timer_device_t timer_device_arm_vtimer = (timer_device_t) {
    .init = arm_vtimer_init,
    .irq  = arm_vtimer_irq,
    .ack  = arm_vtimer_ack
};

#endif /* ARCH_AARCH64 */
