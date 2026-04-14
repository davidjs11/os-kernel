/*   include/platform/mem.h   */

/* Platform-specific memory management interface. */

#ifndef KERNEL_API_PLATFORM_MEM_H
#define KERNEL_API_PLATFORM_MEM_H

#include <types.h>

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
 * @brief Obtain the number of entries of the system's memory map.
 *
 * @return Number of entries in the memory map.
 */
size_t platform_mem_get_mmap_count(void);

/*
 * @brief Get the size of physical memory.
 *
 * @return Size of the physical memory in bytes.
 */
size_t platform_mem_get_size(void);

/*
 * @brief Obtain higher half memory region offset virtual address.
 *
 * @return Offset of the HHDM region in the virtual address space.
 */
vaddr_t platform_mem_get_hhdm_vaddr(void);

/*
 * @brief Obtain the kernel image offset in physical memory.
 *
 * @return Offset of the kernel image in the physical address space.
 */
paddr_t platform_mem_get_kernel_image_paddr(void);

/*
 * @brief Obtain the kernel image offset in virtual memory.
 *
 * @return Offset of the kernel image region in the virtual address space.
 */
vaddr_t platform_mem_get_kernel_image_vaddr(void);

/*
 * @brief Get the size of the kernel image.
 *
 * @return Size of the kernel image in bytes.
 */
size_t platform_mem_get_kernel_image_size(void);

#endif /* KERNEL_API_PLATFORM_MEM_H */
