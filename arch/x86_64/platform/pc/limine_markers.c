/*   arch/x86_64/platform/pc/limine_markers.c   */

/*
 * Limine bootloader request section markers. More info on Limine bootloader
 * protocol documentation.
 */

#include "limine.h"

/* Start of Limine requests. */
__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] =
    LIMINE_REQUESTS_START_MARKER;

/* End of Limine requests. */
__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;
