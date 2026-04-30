/*   kernel/mm/kheap/kheap_freelist.c   */

/* Free-list with best-fit allocator implementation. */

#include <kernel/kheap.h>
#include <stdint.h>

#define ALIGN_UP(x, a)   (((x) + ((a) - 1)) & ~((a) - 1))
#define HEAP_ALIGNMENT   16

/* Block metadata. */
typedef struct block {
    size_t        size;
    bool          free;
    struct block *next;
} __attribute__((aligned(16))) block_t;

static block_t *list_head = NULL;

static void block_split(block_t *block, size_t new_size)
{
    block_t *next_block = 
        (block_t *) ((uint8_t *) block + sizeof(block_t) + new_size);

    *next_block = (block_t) {
        .size = block->size - new_size - sizeof(block_t),
        .free = true,
        .next = block->next
    };

    *block = (block_t) {
        .size = new_size,
        .free = false,
        .next = next_block
    };
}

static void kheap_freelist_init(vaddr_t base, size_t size)
{
    *((block_t *) base) = (block_t) {
        .size = size - sizeof(block_t),
        .free = true,
        .next = NULL,
    };

    list_head = (block_t *) base;
}

static void *kheap_freelist_alloc(size_t size)
{
    block_t *block = list_head;
    block_t *prev = NULL;
    void *result = NULL;
    size = ALIGN_UP(size, HEAP_ALIGNMENT);

    while (result == NULL && block != NULL) {
        if (block->size >= size) {
            /* Split block if it's bigger than requested size. */
            if (block->size >= size + sizeof(block_t) + sizeof(void *)) {
                block_t *next_block =
                    (block_t *) ((uint8_t *) block + sizeof(block_t) + size);

                next_block->size = block->size - size - sizeof(block_t);
                next_block->free = true;
                next_block->next = block->next;

                block->size = size;
                block->next = next_block;
            }
            
            if (prev == NULL) {
                list_head = block->next;
            } else {
                prev->next = block->next;
            }

            block->free = false;
            result = (uint8_t *) block + sizeof(block_t);
        }

        prev = block;
        block = block->next;
    }

    return result;
}

void *kheap_freelist_realloc(void *ptr, size_t new_size)
{
    if (ptr == NULL)
        return kmalloc(new_size);

    block_t *block = (block_t *) ((uint8_t *) ptr - sizeof(block_t));

    /* Already fits. */
    if (block->size >= new_size)
        return ptr;

    void *new = kmalloc(new_size);
    if (new == NULL)
        return NULL;

    
    /* TODO: Use memcpy(new, ptr, block->size); */
    for (size_t i = 0; i < block->size; i++) {
        ((uint8_t *) new)[i] = ((uint8_t *) ptr)[i];
    }
    kfree(ptr);
    return new;
}

static void kheap_freelist_free(void *ptr)
{
    block_t *block = (block_t *) ((uint8_t *) ptr - sizeof(block_t));
    block->free = true;

    /* Re-insert block in address order. */
    if (block < list_head) {
        block->next = list_head;
        list_head = block;
    } else {
        block_t *prev = list_head;
        while (prev->next != NULL && prev->next < block)
            prev = prev->next;
        block->next = prev->next;
        prev->next  = block;
    }

    /* Coalesce with next block. */
    if (block->next != NULL
    && (uint8_t *) block + sizeof(block_t) + block->size
    == (uint8_t *) block->next) {
        block->size += sizeof(block_t) + block->next->size;
        block->next  = block->next->next;
    }

    /* Coalesce with previous block. */
    block_t *prev = list_head;
    while (prev->next != NULL && prev->next != block)
        prev = prev->next;
    if (prev != block
    && (uint8_t *) prev + sizeof(block_t) + prev->size
    == (uint8_t *) block) {
        prev->size += sizeof(block_t) + block->size;
        prev->next  = block->next;
    }
}

/* Operations structure definition. */
kheap_ops_t _kheap_allocator_freelist = {
    .init     = kheap_freelist_init,
    .alloc    = kheap_freelist_alloc,
    .realloc  = kheap_freelist_realloc,
    .free     = kheap_freelist_free,
};
