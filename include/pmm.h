/*   include/pmm.h   */

/* Kernel physical memory manager. */

#ifndef KERNEL_API_PMM_H
#define KERNEL_API_PMM_H

#include <stddef.h>
#include <stdint.h>

/* PMM algorithm interface. Each algorithm MUST implement these functions. */
typedef struct pmm_algorithm {
    void      (*init)(uintptr_t hhdm_offset); /* Initialize algorithm data. */
    uintptr_t (*alloc)(void);                 /* Allocate a page. */
    void      (*free)(uintptr_t addr);        /* Mark a page as free. */
    size_t    (*available)(void);             /* Number of free pages. */
} pmm_algorithm_t;

typedef enum pmm_algorithm_name {
    PMM_ALGORITHM_FREELIST
} pmm_algorithm_name_t;

/*
 * @brief Initialize physical memory manager.
 */
void pmm_init(pmm_algorithm_name_t pmm_algorithm_name);

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
