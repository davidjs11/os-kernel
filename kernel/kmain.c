/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <arch.h>
#include <arch/paging.h>
#include <pmm.h>
#include <list.h>

/*
 * @brief Kernel entry point.
 */
void kmain(void)
{
    arch_init();
    pmm_init(PMM_ALGORITHM_FREELIST);
    arch_paging_init();

    for (;;);
    return;
}
