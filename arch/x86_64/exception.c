/*   arch/x86_64/exception.h   */

/* Exception handlers and indexes. */

#include <kernel/kprintf.h>
#include "exception.h"

typedef struct __attribute__((packed)) gpf_error_code {
    uint64_t raw;
    struct {
        uint64_t present : 16; /* bits 15:0  - offset[15:0]            */
    } bits;
} gpf_error_code_t;

void exception_gpf_handler(interrupt_frame_t *frame)
{
    kprintf("[!] General Protection Fault:\n");
    kprintf("   - Error code: 0x%x\n", frame->error_code);
}
