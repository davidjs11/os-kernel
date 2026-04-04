/*   include/pmm.h   */

/* Kernel physical memory manager. */

#ifndef KERNEL_API_PMM_H
#define KERNEL_API_PMM_H

#include <stddef.h>
#include <stdint.h>

/* PMM algorithm interface. Each algorithm MUST implement these functions. */
typedef struct pmm_algorithm {
    void      (*init)(void);           /* Initialize pmm. */
    uintptr_t (*alloc)(void);          /* Allocate a page. */
    void      (*free)(uintptr_t addr); /* Mark a page as free. */
    size_t    (*available)(void);      /* Number of free pages. */
} pmm_algorithm_t;

/*
 * @brief Initialize physical memory manager.
 */
void pmm_init(void);

/*
 * @brief Allocate a new physical page.
 */
uintptr_t pmm_alloc(void);

/*
 * @brief Free a physical page and make it available.
 */
void pmm_free(uintptr_t addr);

/*
 * @brief Obtain the number of available pages.
 */
size_t pmm_available(void);

#endif /* KERNEL_API_PMM_H */
