/*   include/kernel/platform/timer.h   */

/* Timer public kernel API. */
//
#ifndef KERNEL_API_PLATFORM_TIMER_H
#define KERNEL_API_PLATFORM_TIMER_H

#include <stdint.h>

/*
 * @brief Initialize the platform timer to fire at the given frequency.
 *
 * @param hz Frequency of the timer in Hz.
 */
void platform_timer_init(uint32_t hz);

#endif /* KERNEL_API_PLATFORM_TIMER_H */
