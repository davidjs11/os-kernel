/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <arch.h>
#include <arch/paging.h>
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
    pmm_init(NULL);
    vmm_init();

    int *a = kmalloc(17);
    int *b = kmalloc(10);
    *a = 10; /* This will cause a #PF. */

    for (;;);
    return;
}
