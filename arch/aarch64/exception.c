/*   arch/aarch64/exception.c   */

/* High level exception handlers for each exception type. */

#include "exception.h"
#include <kernel/kprintf.h>
#include <kernel/platform/irq.h>

/* Sync exception. */
void exception_handler_sync(exception_frame_t *frame)
{
    uint32_t exception_class = (frame->syndrome >> 26) & 0x3f;

    kprintf(
        "[!] Sync exception! EC=0x%x FAR=0x%x PC=0x%x\n",
        exception_class,
        frame->fault_addr,
        frame->pc
    );
}

/* IRQ exception. */
void exception_handler_irq(exception_frame_t *frame)
{
    platform_irq_handle();
    kprintf("[!] IRQ exception!\n");
}

/* FIQ exception. */
void exception_handler_fiq(exception_frame_t *frame)
{
    kprintf("[!] FIQ exception!\n");
}

/* SError exception. */
void exception_handler_serror(exception_frame_t *frame)
{
    kprintf("[!] SError exception!\n");
}
