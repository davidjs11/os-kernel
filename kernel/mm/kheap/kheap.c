/*   kernel/mm/kheap/kheap.c   */

/* Kernel heap implementation. */

#include <kernel/kheap.h>
#include <kernel/libc/string.h>

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

void *krealloc(void *ptr, size_t size)
{
    return kheap_ops->realloc(ptr, size);
}

void *kzalloc(size_t size)
{
    void *ptr = kmalloc(size);

    if (ptr != NULL) {
        memset(ptr, 0, size);
    }

    return ptr;
}

void *kcalloc(size_t nmemb, size_t size)
{
    /* Prevent overflow. */
    if (nmemb != 0 && size > SIZE_MAX / nmemb) {
        return NULL;
    }

    return kzalloc(nmemb * size);
}

void kfree(void *ptr)
{
    if (ptr != NULL) {
        kheap_ops->free(ptr);
    }
}
