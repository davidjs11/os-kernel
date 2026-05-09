/*   arch/aarch64/platform/qemu-virt/irq.c   */

/* IRQ kernel API implementation for qemu-virt board's interrupt controller. */

#include <stdint.h>
#include <kernel/platform/irq.h>
#include <kernel/arch/interrupt.h>
#include <kernel/kprintf.h>
#include <kernel/irq.h>
#include "gic.h"

void platform_irq_mask(uint8_t irq)
{

}

void platform_irq_unmask(uint8_t irq)
{

}

void platform_irq_eoi(uint8_t irq)
{

}

void platform_irq_handle(void)
{
    gic_handle_irq();
}
