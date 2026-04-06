/*   include/platform/mem.h   */

/* Platform-specific memory management interface. */

#ifndef KERNEL_API_PLATFORM_MEM_H
#define KERNEL_API_PLATFORM_MEM_H

#include <stdint.h>
#include <stddef.h>

/* Memory map region types. */
typedef enum mmap_type {
    MMAP_USABLE = 0,
    MMAP_RESERVED,
    MMAP_ACPI_RECLAIMABLE,
    MMAP_ACPI_NVS,
    MMAP_BAD_MEMORY,
    MMAP_BOOTLOADER_RECLAIMABLE,
    MMAP_KERNEL_AND_MODULES,
    MMAP_FRAMEBUFFER,
} mmap_type_t;

/* Memory map entry. */
typedef struct mmap_entry {
    uint64_t base;
    uint64_t size;
    mmap_type_t type;
} mmap_entry_t;

/*
 * @brief Obtain the system memory map in kernel's public API format.
 *
 * @return A pointer to the memory map entries array, finishing with a NULL
 * entry (base = NULL).
 */
mmap_entry_t *platform_mem_get_mmap(void);

/*
 * @brief Obtain higher half memory offset in case it's available (otherwise
 * 0).
 */
uint64_t platform_mem_get_hh_offset(void);

#endif /* KERNEL_API_PLATFORM_MEM_H */
