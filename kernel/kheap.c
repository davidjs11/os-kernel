/*   kernel/kheap.c   */

/* Kernel heap implementation. */

#include <kheap.h>

static kheap_ops_t *kheap_ops = KHEAP_ALLOCATOR_DEFAULT;

void kheap_init(vaddr_t base, size_t size, kheap_ops_t *ops)
{
    if (ops != NULL) {
        kheap_ops = ops;
    }

    kheap_ops->init(base, size);
}

void *kmalloc(size_t size)
{
    return kheap_ops->alloc(size);
}

void kfree(void *ptr)
{
    kheap_ops->free(ptr);
}
