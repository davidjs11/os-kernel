/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <kernel/arch.h>
#include <kernel/arch/paging.h>
#include <kernel/arch/interrupt.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <kernel/list.h>
#include <kernel/kheap.h>
#include <kernel/task.h>

/*
 * @brief Kernel entry point.
 */
void kmain(void)
{
    arch_init();
    arch_interrupt_init();
    arch_interrupt_enable();
    pmm_init(NULL);
    vmm_init();

    task_create("test task", (vaddr_t) &kmain, NULL, 0);

    for (;;);
    return;
}

