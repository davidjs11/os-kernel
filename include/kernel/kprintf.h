/*   include/kernel/kprintf.c   */

/* Kernel printf function API. */

#ifndef KERNEL_API_KPRINTF_H
#define KERNEL_API_KPRINTF_H

#include <kernel/types.h>

void kprintf(const char *fmt, ...);

#endif /* KERNEL_API_KPRINTF_H */
