/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <arch.h>
#include <arch/paging.h>
#include <arch/interrupt.h>
#include <pmm.h>
#include <vmm.h>
#include <list.h>
#include <kheap.h>

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

    int *a = kmalloc(17);
    int *b = kmalloc(10);
    *a = 67; /* This will cause a #PF. */
    *b = 67; /* This will NOT cause a #PF. */
    b = kmalloc(4096);
    *b = 67; /* This will NOT cause a #PF. */
    a = kmalloc(4);
    *a = 67; /* This will cause a #PF. */

    for (;;);
    return;
}
