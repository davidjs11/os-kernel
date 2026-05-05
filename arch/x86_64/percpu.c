/*   arch/x86_64/percpu.c   */

/* CPU unique parameters. Used by system call entry point. */

#include "percpu.h"
#include "msr.h"

percpu_t percpu;

void percpu_init(void)
{
    msr_write(MSR_KERNEL_GS_BASE, (uint64_t) &percpu.kstack_top);
}
