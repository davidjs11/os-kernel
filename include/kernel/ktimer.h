/*   include/kernel/ktimer.h   */

/* Kernel timer API. */

#ifndef KERNEL_API_TIMER_H
#define KERNEL_API_TIMER_H

#include <kernel/types.h>

/*
 * @brief Initialize kernel timer.
 *
 * @param freq Frequency of the kernel timer in hertzs.
 */
void ktimer_init(uint32_t freq);

/*
 * @brief Timer tick (called by timer interrupt).
 */
void ktimer_tick(void);

/*
 * @brief Obtain the number of ticks since kernel initialization.
 *
 * @return Number of ticks.
 */
uint64_t ktimer_ticks(void);

#endif /* KERNEL_API_TIMER_H */
