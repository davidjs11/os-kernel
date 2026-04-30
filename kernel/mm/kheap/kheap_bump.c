/*   kernel/mm/kheap/kheap_bump.c   */

/* Bump allocator heap implementation. */

#include <kernel/kheap.h>
#include <stdint.h>

#define ALIGN_UP(x, a)   (((x) + ((a) - 1)) & ~((a) - 1))
#define HEAP_ALIGNMENT   16

/* Bump allocator data. */
static struct {
    uintptr_t base;
    uintptr_t current;
    uintptr_t end;
} heap;

static void kheap_bump_init(vaddr_t base, size_t size)
{
    heap.base    = base;
    heap.current = base;
    heap.end     = base + size;
}

static void *kheap_bump_alloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    size = ALIGN_UP(size, HEAP_ALIGNMENT);

    uintptr_t next = heap.current + size;

    /* Heap is out of size. */
    if (next > heap.end) {
        return NULL;
    }

    void *ptr = (void *) heap.current;
    heap.current = next;

    return ptr;
}

static void kheap_bump_free(void *ptr)
{
    /* No operation by design. */
    (void) ptr;
}

/* Operations structure definition. */
kheap_ops_t _kheap_allocator_bump = {
    .init  = kheap_bump_init,
    .alloc = kheap_bump_alloc,
    .free  = kheap_bump_free,
};
