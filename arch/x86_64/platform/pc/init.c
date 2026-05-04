/*   arch/x86_64/platform/pc/init.c   */

/* Platform-specific initialization code implementation for x86_64/pc. */

#include <kernel/platform/init.h>
#include <kernel/platform/irq.h>
#include <kernel/platform/timer.h>
#include <kernel/ktimer.h>
#include "pic.h"

void platform_init(void)
{
    /* Initialize interrupt controller. */
    platform_irq_init();
}
