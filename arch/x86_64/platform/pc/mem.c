/*   arch/x86_64/platform/pc/mem.c   */

/* Implementation of PC platform-specific memory management functionality. */

#include <platform/mem.h>
#include <libc/string.h>
#include <types.h>
#include "limine.h"

/* Limine memory map request. */
__attribute__((used, section(".limine_requests")))
volatile struct limine_memmap_request limine_memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0,
};

/* Limine higher half direct mapping request. */
__attribute__((used, section(".limine_requests")))
volatile struct limine_hhdm_request limine_hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0,
};

/* Limine executable address request. */
__attribute__((used, section(".limine_requests")))
volatile struct limine_executable_address_request
    limine_executable_address_request = {
        .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID,
        .revision = 0,
};

/* Kernel's own copy of the memory map. */
#define MMAP_ENTRIES_MAX 64
static mmap_entry_t mmap_entry[MMAP_ENTRIES_MAX];
static size_t mmap_entry_count;
static size_t mem_size;
static uint64_t hhdm_offset;
static paddr_t kernel_paddr;
static vaddr_t kernel_vaddr;
extern char _kernel_image_start[];
extern char _kernel_image_end[];
bool limine_parsed = false;

/*
 * @brief Convert from Limine memory map entry type to kernel type.
 *
 * @param type Type of the limine mmap entry.
 */
static inline mmap_type_t limine_to_kernel_type(uint64_t type)
{
    switch (type) {
        case LIMINE_MEMMAP_USABLE:                 return MMAP_USABLE;
        case LIMINE_MEMMAP_RESERVED:               return MMAP_RESERVED;
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:       return MMAP_ACPI_RECLAIMABLE;
        case LIMINE_MEMMAP_ACPI_NVS:               return MMAP_ACPI_NVS;
        case LIMINE_MEMMAP_BAD_MEMORY:             return MMAP_BAD_MEMORY;
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: return MMAP_BOOTLOADER_RECLAIMABLE;
        case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES: return MMAP_KERNEL_AND_MODULES;
        case LIMINE_MEMMAP_FRAMEBUFFER:            return MMAP_FRAMEBUFFER;
        default:                                   return MMAP_RESERVED;
    }
}

/*
 * @brief Parse Limine requests.
 */
static void parse_limine_requests()
{
    bool panic = false;

    /* Memory map request. */
    struct limine_memmap_response *limine_mmap =
        limine_memmap_request.response;

    if (limine_mmap == NULL) {
        panic = true;
        goto done;
    }

    size_t i = 0;
    mmap_entry_count = limine_mmap->entry_count;
    mem_size = 0;
    for (; i < mmap_entry_count && i < MMAP_ENTRIES_MAX - 1; i++) {
        /* Copy entry from Limine memory to kernel memory. */
        mmap_entry[i] = (mmap_entry_t) {
            .base  = limine_mmap->entries[i]->base,
            .size  = limine_mmap->entries[i]->length,
            .type  = limine_to_kernel_type(limine_mmap->entries[i]->type),
        };

        /* Keep track of the highest entry for obtaining memory size. */
        switch (mmap_entry[i].type) {
            case LIMINE_MEMMAP_USABLE:
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
                paddr_t end = mmap_entry[i].base + mmap_entry[i].size;
                if (end > mem_size) {
                    mem_size = end;
                }
        }
    }

    /* Null sentinel entry that marks end of the map. */
    memset(&mmap_entry[i], 0, sizeof(mmap_entry_t));

    /* HHDM request. */
    if (limine_hhdm_request.response == NULL) {
        panic = true;
        goto done;
    }
    hhdm_offset = limine_hhdm_request.response->offset;

    /* Kernel address request. */
    if (limine_executable_address_request.response == NULL) {
        panic = true;
        goto done;
    }
    kernel_vaddr = limine_executable_address_request.response->virtual_base;
    kernel_paddr = limine_executable_address_request.response->physical_base;

done:
    if (panic) {
        /* TODO: Panic here. */
    }
}

mmap_entry_t *platform_mem_get_mmap()
{
    if (!limine_parsed) {
        parse_limine_requests();
        limine_parsed = true;
    }

    return &mmap_entry[0];
}

size_t platform_mem_get_mmap_count(void)
{
    if (!limine_parsed) {
        parse_limine_requests();
        limine_parsed = true;
    }

    return mmap_entry_count;
}

size_t platform_mem_get_size(void)
{
    if (!limine_parsed) {
        parse_limine_requests();
        limine_parsed = true;
    }

    return mem_size;
}

uint64_t platform_mem_get_hhdm_vaddr(void)
{
    if (!limine_parsed) {
        parse_limine_requests();
        limine_parsed = true;
    }

    return hhdm_offset;
}

uint64_t platform_mem_get_kernel_image_vaddr(void)
{
    if (!limine_parsed) {
        parse_limine_requests();
        limine_parsed = true;
    }

    return kernel_vaddr;
}

uint64_t platform_mem_get_kernel_image_paddr(void)
{
    if (!limine_parsed) {
        parse_limine_requests();
        limine_parsed = true;
    }

    return kernel_paddr;
}

size_t platform_mem_get_kernel_image_size(void)
{
    return (size_t)((vaddr_t) _kernel_image_end - (vaddr_t) _kernel_image_start);
}
