/*   arch/aarch64/platform/qemu-virt/init.c   */

/* Platform-specific initialization code implementation for qemu-virt board. */

#include <kernel/platform/init.h>
#include <kernel/platform/irq.h>
#include <kernel/platform/timer.h>
#include <kernel/ktimer.h>
#include "gic.h"

void platform_init(void)
{
    gic_init();
}
