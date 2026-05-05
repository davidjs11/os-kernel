/*   arch/x86_64/exception.h   */

/* Exception handlers and indexes. */

#ifndef ARCH_X86_64_EXCEPTION_H
#define ARCH_X86_64_EXCEPTION_H

#include "idt.h"

void exception_gpf_handler(interrupt_frame_t *frame);

#endif /* ARCH_X86_64_EXCEPTION_H */
