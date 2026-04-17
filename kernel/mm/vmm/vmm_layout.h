/*   kernel/mm/vmm/vmm_layout.h   */

/* Kernel virtual memory layout private definitions. */

#ifndef KERNEL_PRIVATE_VM_LAYOUT_H
#define KERNEL_PRIVATE_VM_LAYOUT_H

#define KERNEL_LAYOUT_HEAP_BASE 0xffff900000000000UL
#define KERNEL_LAYOUT_HEAP_SIZE (256UL * 1024 * 1024 * 1024) /* 256 GiB. */

#endif /* KERNEL_PRIVATE_VM_LAYOUT_H */          
