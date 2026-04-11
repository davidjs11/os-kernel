/*   arch/x86_64/platform/pc/mem.c   */

/* Implementation of PC platform-specific memory management functionality. */

#include <platform/mem.h>
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

/* Kernel's own copy of the memory map. */
#define MMAP_ENTRIES_MAX 64
static mmap_entry_t mmap_entry[MMAP_ENTRIES_MAX];
bool mmap_copied = false;

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

mmap_entry_t *platform_mem_get_mmap()
{
    if (!mmap_copied) {
        struct limine_memmap_response *limine_mmap =
            limine_memmap_request.response;

        if (limine_mmap == NULL) {
            __asm__ volatile ("cli; hlt");
        }

        /* Copy entries from Limine memory to kernel memory. */
        size_t i = 0;
        for (; i < limine_mmap->entry_count && i < MMAP_ENTRIES_MAX - 1; i++) {
            mmap_entry[i] = (mmap_entry_t) {
                .base  = limine_mmap->entries[i]->base,
                .size = limine_mmap->entries[i]->length,
                .type  = limine_to_kernel_type(limine_mmap->entries[i]->type),
            };
        }

        /* TODO: use memset when available. */
        /* Null sentinel entry that marks end of the map. */
        mmap_entry[i] = (mmap_entry_t) {
            .base = 0,
            .size = 0,
            .type = 0,
        };
    }

    return &mmap_entry[0];
}

uint64_t platform_mem_get_hh_offset(void)
{
    if (limine_hhdm_request.response == NULL) {
        __asm__ volatile ("cli; hlt");
    }

    return limine_hhdm_request.response->offset;
}
