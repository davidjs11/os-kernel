/*   kernel/include/dynarray.h   */

/* Dynamic array header-only data structure. */

#ifndef KERNEL_API_DYNARRAY_H
#define KERNEL_API_DYNARRAY_H

#include <kernel/kheap.h>
#include <kernel/types.h>

/* Initial capacity of a dynamic array. */
#define DYNARRAY_INIT_CAP 4

/*
 * @brief Declare a typed dynamic array struct.
 *
 * @param T    Type of the darta container.
 * @param name Name that the struct will be given.
 */
#define DYNARRAY_TYPEDEF(T, name)   \
    typedef struct {                \
        T      *data;               \
        size_t  len;                \
        size_t  cap;                \
    } name

/*
 * @brief Zero-initialize the array.
 *
 * @param arr Array to initialize.
 */
#define DYNARRAY_INIT(arr)          \
    do {                            \
        (arr).data = NULL;          \
        (arr).len  = 0;             \
        (arr).cap  = 0;             \
    } while (0)

/*
 * @brief Push a value.
 *
 * @param arr Array that the value will be pushed to
 * @param val Value to push.
 *
 * @return 0 on success, -1 otherwise.
 */
#define DYNARRAY_PUSH(arr, val)                                    \
    ({                                                             \
        int _ret = 0;                                              \
        if ((arr).len == (arr).cap) {                              \
            size_t _new_cap = (arr).cap ? (arr).cap * 2            \
                                        : DYNARRAY_INIT_CAP;       \
            void *_new = krealloc((arr).data,                      \
                                  _new_cap * sizeof(*(arr).data)); \
            if (_new == NULL) {                                    \
                _ret = -1;                                         \
            } else {                                               \
                (arr).data = _new;                                 \
                (arr).cap  = _new_cap;                             \
            }                                                      \
        }                                                          \
        if (_ret == 0)                                             \
            (arr).data[(arr).len++] = (val);                       \
        _ret;                                                      \
    })

/*
 * @brief Access the element at a certain index. Bounds are not checked.
 *
 * @param arr Array that will be accessed.
 * @param i   Element position in the array.
 */
#define DYNARRAY_GET(arr, i) ((arr).data[i])

/*
 * @brief Get the number of elements.
 *
 * @param arr The array to get the number of elements from.
 */
#define DYNARRAY_LEN(arr) ((arr).len)

/*
 * @brief Iterate over all elements.
 *
 * @param arr The array to iterate.
 * @param T   The type of the array.
 * @param it  The iterator pointer name.
 */
#define DYNARRAY_FOREACH(arr, T, it) \
    for (T *it = (arr).data; it < (arr).data + (arr).len; it++)

/*
 * Free the backing buffer. Does NOT call any destructor on elements, so if
 * elements need cleanup, iterate first.
 */
/*
 * @brief Free the dynamic array. This function doesn't call the destructor of
 *        the underlying elements, for that it must be iterated previously.
 *
 * @param arr The array to free.
 */
#define DYNARRAY_FREE(arr)          \
    do {                            \
        kfree((arr).data);          \
        (arr).data = NULL;          \
        (arr).len  = 0;             \
        (arr).cap  = 0;             \
    } while (0)

#endif /* DYNARRAY_H */
