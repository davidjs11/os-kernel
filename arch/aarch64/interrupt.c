/*   arch/aarch64/interrupt.c   */

/*
 * Architecture-dependent interrupt (exception) API implementation for AArch64.
 */

#include <kernel/arch/interrupt.h>

static inline void interrupts_enable(void)
{
    /* Clear IRQ mask bit. */
    __asm__ volatile ("msr daifclr, #3");
}

void arch_interrupt_init(void)
{

}

void arch_interrupt_enable(void)
{
    interrupts_enable();
}

void arch_interrupt_disable(void)
{

}
