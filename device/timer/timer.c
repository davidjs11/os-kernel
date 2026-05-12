/*   device/timer/timer.c   */

/* Timer device management. */

#include <kernel/device/timer/timer.h>

static timer_device_t *timer_dev = NULL;

void timer_register(timer_device_t *dev)
{
    timer_dev = dev;
}

void timer_init(uint32_t freq)
{
    if (timer_dev != NULL && timer_dev->init != NULL) {
        timer_dev->init(freq);
    }
}

uint32_t timer_irq(void)
{
    if (timer_dev != NULL && timer_dev->irq != NULL) {
        return timer_dev->irq();
    }
    return 0xffffffff;
}

void timer_ack(void)
{
    if (timer_dev != NULL && timer_dev->ack != NULL) {
        timer_dev->ack();
    }
}
