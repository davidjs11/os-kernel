/*   arch/x86_64/arch_init.c   */

/* Architecture-specific initialization code implementation for x86_64. */

#include <arch_init.h>
#include "segments.h"

gdt_t gdt;
tss64_t tss;

void arch_init(void) {
    /* Initialize segments. */
    segments_init(&gdt, &tss);
}
