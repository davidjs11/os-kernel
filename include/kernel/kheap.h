/*   include/kheap.h   */

/* Kernel heap interface. */

#ifndef KERNEL_API_KHEAP_H
#define KERNEL_API_KHEAP_H

#include <kernel/types.h>

/* Kernel heap allocator interface. */
typedef struct kheap_ops {
    /*
     * @brief Initialize heap.
     *
     * @param base Base address of the heap.
     * @param size Initial size of the heap in bytes.
     */
    void  (*init)(vaddr_t base, size_t size);

    /*
     * @brief Allocate data.
     *
     * @param size Size of the allocated data.
     *
     * @return A pointer to the dinamically allocated data.
     */
    void *(*alloc)(size_t size);

    /*
     * @brief Reallocate data.
     *
     * @param ptr  A pointer to the previously allocated data.
     * @param size New size of the allocated data.
     *
     * @return A pointer to the dinamically allocated data.
     */
    void *(*realloc)(void *ptr, size_t size);

    /*
     * @brief Free data.
     *
     * @param ptr A pointer to the data to be freed.
     */
    void  (*free)(void *ptr);
} kheap_ops_t;

/* List of heap allocator algorithms. */
extern kheap_ops_t _kheap_allocator_bump;
extern kheap_ops_t _kheap_allocator_freelist;
#define KHEAP_ALLOCATOR_BUMP     (kheap_ops_t *) (&_kheap_allocator_bump)
#define KHEAP_ALLOCATOR_FREELIST (kheap_ops_t *) (&_kheap_allocator_freelist)
#define KHEAP_ALLOCATOR_DEFAULT  KHEAP_ALLOCATOR_FREELIST

/*
 * @brief Initialize kernel heap.
 *
 * @param base Heap base address.
 * @param size Initial heap size in bytes.
 * @param ops  Heap allocator algorithm.
 */
void kheap_init(vaddr_t base, size_t size, kheap_ops_t *ops);

/*
 * @brief Allocate a memory block in the heap. Programmer must free this block
 *        after using it.
 *
 * @param size Size of the block in bytes.
 *
 * @return A generic pointer to the new allocated block, aligned at a 16-byte
 *         boundary.
 */
void *kmalloc(size_t size);

/*
 * @brief Reallocate a previously allocated memory block in the heap.
 *        Programmer must free this block after using it.
 *
 * @param ptr  A pointer to the previously allocated block.
 * @param size New size of the block in bytes.
 *
 * @return A generic pointer to the new allocated block, aligned at a 16-byte
 *         boundary.
 */
void *krealloc(void *ptr, size_t size);

/*
 * @brief Same as kmalloc but zeroing the contents of the block.
 *
 * @param size  Size of the allocated block.
 *
 * @return A generic pointer to the new allocated block, aligned at a 16-byte
 *         boundary.
 */
void *kzalloc(size_t size);

/*
 * @brief Same as kmalloc but zeroing the contents of the block.
 *
 * @param nmemb Number of elements in the allocated block.
 * @param size  Size of each element in bytes.
 *
 * @return A generic pointer to the new allocated block, aligned at a 16-byte
 *         boundary.
 */
void *kcalloc(size_t nmemb, size_t size);

/*
 * @brief Free a memory block in the heap.
 *
 * @param ptr A pointer to the block inside the heap.
 */
void kfree(void *ptr);

#endif /* KERNEL_API_KHEAP_H */
