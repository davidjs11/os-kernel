/*   arch/aarch64/exception.c   */

/* High level exception handlers for each exception type. */

#include "exception.h"
#include <kernel/kprintf.h>
#include <kernel/vmm.h>
#include <kernel/device/irq/irq.h>
#include <kernel/interrupt.h>
#include <kernel/syscall.h>

/* Exception classes (ESR_EL1 bits 31:26). */
#define EC_SVC_AARCH64      0x15 /* SVC instruction.            */
#define EC_INSTR_ABORT_EL0  0x20 /* Instruction abort from EL0. */
#define EC_INSTR_ABORT_EL1  0x21 /* Instruction abort from EL1. */
#define EC_DATA_ABORT_EL0   0x24 /* Data abort from EL0.        */
#define EC_DATA_ABORT_EL1   0x25 /* Data abort from EL1.        */

/* Instruction Specific Syndrome (ESR_EL1 bits 24:0) fields for aborts. */
#define ISS_FSC_MASK        0x3C        /* Fault status code (type).  */
#define ISS_FSC_TRANSLATION 0x04        /* Translation fault.         */
#define ISS_WNR             (1U << 6)   /* Write not Read.            */

/* Sync exception. */
void exception_handler_sync(exception_frame_t *frame)
{
    uint32_t exception_class = (frame->syndrome >> 26) & 0x3f;
    uint32_t iss = frame->syndrome & 0x1ffffff;

    /* System call: number in x8, arguments in x0-x5, return value in x0. */
    if (exception_class == EC_SVC_AARCH64) {
        uint64_t num = frame->x[8];
        if (num < SYSCALL_MAX && syscall_table[num] != NULL) {
            frame->x[0] = syscall_table[num](
                frame->x[0],
                frame->x[1],
                frame->x[2],
                frame->x[3],
                frame->x[4],
                frame->x[5]
            );
        } else {
            frame->x[0] = (uint64_t) -1;
        }
        return;
    }

    /* Page faults. */
    switch (exception_class) {
        case EC_DATA_ABORT_EL0:
        case EC_DATA_ABORT_EL1:
        case EC_INSTR_ABORT_EL0:
        case EC_INSTR_ABORT_EL1: {
            bool data = exception_class == EC_DATA_ABORT_EL0
                     || exception_class == EC_DATA_ABORT_EL1;
            bool user = exception_class == EC_DATA_ABORT_EL0
                     || exception_class == EC_INSTR_ABORT_EL0;

            vmm_handle_page_fault(
                frame->fault_addr,
                (iss & ISS_FSC_MASK) != ISS_FSC_TRANSLATION,
                data && (iss & ISS_WNR),
                user,
                !data
            );
            return;
        }
    }

    kprintf(
        "[!] Unhandled sync exception! EC=0x%x FAR=0x%x PC=0x%x\n",
        exception_class,
        frame->fault_addr,
        frame->pc
    );
    for (;;);
}

/* IRQ exception. */
void exception_handler_irq(exception_frame_t *frame)
{
    uint32_t irq = irq_ack();

    /* Spurious interrupt, no EOI needed. */
    if (irq == 0xffffffff) {
        return;
    }

    irq_handle(irq);
    irq_eoi(irq);
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
    for (;;);
}
