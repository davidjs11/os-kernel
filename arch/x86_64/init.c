/*   arch/x86_64/init.c   */

/* Architecture-specific initialization code implementation for x86_64. */

#include <kernel/arch/init.h>
#include <stddef.h>
#include "segment.h"
#include "idt.h"
#include "pgtable.h"
#include "exception.h"

void arch_init(void)
{
    segment_init();
    idt_init();
    isr_set_handler(IDT_ENTRY_GPF, exception_gpf_handler);
}
