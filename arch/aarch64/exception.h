/*   arch/aarch64/exception.h   */

/* AArch64 exception management internal API. */

#ifndef ARCH_AARCH64_EXCEPTION_H
#define ARCH_AARCH64_EXCEPTION_H

#include <stdint.h>

/* Exception Stack Frame.                                           */
typedef struct __attribute__((packed)) exception_frame {
    uint64_t x[31];      /* GPRs.                                   */
    uint64_t sp;         /* Stack Pointer.                          */
    uint64_t pc;         /* Exception Link Register (PC).           */
    uint64_t pstate;     /* Saved Program Status Register (PSTATE). */
    uint64_t syndrome;   /* Exception Syndrome Register.            */
    uint64_t fault_addr; /* Fault Address Register.                 */
} exception_frame_t;

void exception_load_vector_table(void);

#endif /* ARCH_AARCH64_EXCEPTION_H */
