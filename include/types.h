/*   include/types.h   */

/* Type definition for public kernel API. */

#ifndef KERNEL_API_TYPES_H
#define KERNEL_API_TYPES_H

#include <stdint.h> /* For uintx_t. */
#include <stddef.h> /* For size_t. */
#include <stdbool.h> /* For bool. */

typedef uintptr_t paddr_t; /* Physical address. */
typedef uintptr_t vaddr_t; /* Virtual address. */

#endif /* KERNEL_API_TYPES_H */
