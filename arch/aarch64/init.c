/*   arch/aarch64/init.c   */

/* Architecture-specific initialization code implementation for AArch64. */

#include <kernel/arch/init.h>
#include <stddef.h>
#include "exception.h"

void arch_init(void)
{
    exception_load_vector_table();

    __asm__ volatile ("svc #4");

    // char *null_ptr = NULL;
    // *null_ptr = 8;
}
