/*   include/kernel/ktest.h   */

/* Kernel unit test framework API. */

#ifndef KERNEL_API_KTEST_H
#define KERNEL_API_KTEST_H

#include <kernel/types.h>

/*
 * @brief Initialize the test framework. Must be called before running any
 *        test.
 */
void ktest_init(void);

/*
 * @brief Run a test and record if it passed or failed.
 *
 * @param suite Name of the test suite.
 * @param name  Name of the test.
 * @param fn    Test function.
 */
void ktest_run(const char *suite, const char *name, void (*fn)(void));

/*
 * @brief Print a summary of the tests that passed and failed.
 *
 * @return 0 if all tests passed, 1 otherwise.
 */
int ktest_summary(void);

/* Used by the assertion macros. */
void _ktest_assert(int cond, const char *expr, const char *file, int line);

void _ktest_assert_eq(
    uintptr_t a,
    uintptr_t b,
    const char *as,
    const char *bs,
    const char *file,
    int line);

void _ktest_assert_ne(
    uintptr_t a,
    uintptr_t b,
    const char *as,
    const char *bs,
    const char *file,
    int line);

void _ktest_assert_ge(
    uintptr_t a,
    uintptr_t b,
    const char *as,
    const char *bs,
    const char *file,
    int line);

/* Assertions. Operands are casted to uintptr_t (integers and pointers). */
#define KTEST_ASSERT(expr)                                            \
    _ktest_assert((int) (expr), #expr, __FILE__, __LINE__)

#define KTEST_ASSERT_EQ(a, b)                                         \
    _ktest_assert_eq(                                                 \
        (uintptr_t) (a), (uintptr_t) (b), #a, #b, __FILE__, __LINE__  \
    )

#define KTEST_ASSERT_NE(a, b)                                         \
    _ktest_assert_ne(                                                 \
        (uintptr_t) (a), (uintptr_t) (b), #a, #b, __FILE__, __LINE__  \
    )

#define KTEST_ASSERT_GE(a, b)                                         \
    _ktest_assert_ge(                                                 \
        (uintptr_t) (a), (uintptr_t) (b), #a, #b, __FILE__, __LINE__  \
    )

#define KTEST_ASSERT_NULL(ptr)                                        \
    _ktest_assert((ptr) == NULL, #ptr " == NULL", __FILE__, __LINE__)

#define KTEST_ASSERT_NOT_NULL(ptr)                                    \
    _ktest_assert((ptr) != NULL, #ptr " != NULL", __FILE__, __LINE__)

/* Run a test using the function name as test name. */
#define KTEST_RUN(suite, fn) ktest_run((suite), #fn, (fn))

#endif /* KERNEL_API_KTEST_H */
