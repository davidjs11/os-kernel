/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <arch.h>
#include <pmm.h>
#include <list.h>

/*
 * @brief Kernel entry point.
 */
void kmain(void)
{
    arch_init();
    pmm_init(PMM_ALGORITHM_FREELIST);

    uintptr_t newpage = pmm_alloc();
    pmm_free(newpage);

    for (;;);
    return;
}
