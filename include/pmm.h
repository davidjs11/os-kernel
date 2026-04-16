/*   include/pmm.h   */

/* Kernel physical memory manager. */

#ifndef KERNEL_API_PMM_H
#define KERNEL_API_PMM_H

#include <types.h>

/* PMM algorithm interface. Each algorithm MUST implement these functions. */
typedef struct pmm_ops {
    void      (*init)(paddr_t hhdm_offset); /* Initialize operations data. */
    paddr_t   (*alloc)(void);               /* Allocate a page. */
    void      (*free)(paddr_t addr);        /* Mark a page as free. */
    size_t    (*available)(void);           /* Number of free pages. */
} pmm_ops_t;

/* List of PMM algorithms. */
extern pmm_ops_t _pmm_algorithm_freelist;
#define PMM_ALGORITHM_FREELIST (pmm_ops_t *) (&_pmm_algorithm_freelist)
#define PMM_OPS_DEFAULT        PMM_ALGORITHM_FREELIST

/*
 * @brief Initialize physical memory manager.
 */
void pmm_init(pmm_ops_t *ops);

/*
 * @brief Allocate a new physical page.
 */
paddr_t pmm_alloc(void);

/*
 * @brief Free a physical page and make it available.
 */
void pmm_free(paddr_t addr);

/*
 * @brief Obtain the number of available pages.
 */
size_t pmm_available(void);

#endif /* KERNEL_API_PMM_H */
