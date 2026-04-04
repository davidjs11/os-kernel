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
    pmm_init();

    for (;;);
    return;
}
