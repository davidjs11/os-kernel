/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <arch.h>
#include <platform/mmap.h>

/*
 * @brief Kernel entry point.
 */
void kmain(void) {
    arch_init();
    mmap_entry_t *mmap = platform_mmap_get();

    for (;;);
    return;
}
