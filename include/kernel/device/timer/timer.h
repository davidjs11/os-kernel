/*   include/kernel/device/timer/timer.h   */

/* Timer device driver API. */

#ifndef KERNEL_API_DEVICE_TIMER_H
#define KERNEL_API_DEVICE_TIMER_H

#include <kernel/types.h>

/* Timer device operations. */
typedef struct timer_device {
    void     (*init)(uint32_t freq); /* Start the timer at a frequency (Hz). */
    uint32_t (*irq)(void);           /* Return the IRQ line of the timer.    */
    void     (*ack)(void);           /* Acknowledge and rearm the timer.     */
} timer_device_t;

/*
 * @brief Register a new timer device.
 */
void timer_register(timer_device_t *dev);

/*
 * @brief Initialize the timer device.
 *
 * @param freq Frequency of the timer in hertzs.
 */
void timer_init(uint32_t freq);

/*
 * @brief Obtain the IRQ line used by the timer device.
 *
 * @return IRQ line number.
 */
uint32_t timer_irq(void);

/*
 * @brief Acknowledge the timer interrupt and rearm the timer for the next tick.
 */
void timer_ack(void);

#endif /* KERNEL_API_DEVICE_TIMER_H */
