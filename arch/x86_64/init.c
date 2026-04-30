/*   arch/x86_64/init.c   */

/* Architecture-specific initialization code implementation for x86_64. */

#include <kernel/arch.h>
#include <stddef.h>
#include "segment.h"
#include "idt.h"
#include "pgtable.h"

void arch_init(void)
{
    segment_init();

    /* Trigger #PF exception */
    // volatile uint64_t *p = (uint64_t *)0xdeadbeef;
    // *p;
}
