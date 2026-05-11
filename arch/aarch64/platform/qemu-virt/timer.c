/*   arch/aarch64/platform/qemu-virt/timer_pit.c   */

/* Kernel timer API implementation for qemu-virt board. */

#include <stdint.h>
#include <kernel/arch/interrupt.h>
#include <kernel/platform/timer.h>
#include <kernel/ktimer.h>

static volatile uint64_t ticks = 0;

void platform_timer_init(uint32_t freq)
{

}

uint64_t platform_timer_ticks(void)
{
    return ticks;
}
