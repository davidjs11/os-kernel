/*   arch/x86_64/init.c   */

/* Architecture-specific initialization code implementation for x86_64. */

#include <kernel/arch/init.h>
#include <stddef.h>
#include "segment.h"
#include "idt.h"
#include "pgtable.h"
#include "exception.h"
#include "syscall.h"
#include "percpu.h"
#include "handlers.h"

void arch_init(void)
{
    segment_init();
    idt_init();

    /* Set exception handlers. */
    isr_set_handler(IDT_ENTRY_GPF, exception_gpf_handler);

    /* Set IRQ common handler for all IRQs. */
    for (size_t i = 0x20; i < 0x30; i++) {
        isr_set_handler(i, irq_common_handler);
    }

    percpu_init();
    syscall_init();
}
