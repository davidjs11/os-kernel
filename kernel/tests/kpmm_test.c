/*   kernel/tests/kpmm_test.c   */

/* Physical memory manager tests. */

#include <kernel/ktest.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>

/* Allocated frames are never 0 (out of memory). */
static void test_pmm_alloc_nonzero(void)
{
    paddr_t p = pmm_alloc();
    KTEST_ASSERT_NE(p, 0);
    pmm_free(p);
}

/* Two allocations return different frames. */
static void test_pmm_alloc_distinct(void)
{
    paddr_t a = pmm_alloc();
    paddr_t b = pmm_alloc();

    KTEST_ASSERT_NE(a, 0);
    KTEST_ASSERT_NE(b, 0);
    KTEST_ASSERT_NE(a, b);

    pmm_free(b);
    pmm_free(a);
}

/* Allocated frames are page aligned. */
static void test_pmm_alloc_aligned(void)
{
    paddr_t p = pmm_alloc();
    KTEST_ASSERT_NE(p, 0);
    KTEST_ASSERT_EQ(p % PAGE_SIZE, 0);
    pmm_free(p);
}

/* A freed frame is the next one to be allocated. */
static void test_pmm_free_reuse(void)
{
    paddr_t a = pmm_alloc();
    KTEST_ASSERT_NE(a, 0);
    pmm_free(a);

    paddr_t b = pmm_alloc();
    KTEST_ASSERT_EQ(b, a);
    pmm_free(b);
}

/* Available frames count decreases on alloc and increases on free. */
static void test_pmm_available_accounting(void)
{
    size_t before = pmm_available();
    paddr_t p = pmm_alloc();

    KTEST_ASSERT_NE(p, 0);
    KTEST_ASSERT_EQ(pmm_available(), before - 1);

    pmm_free(p);
    KTEST_ASSERT_EQ(pmm_available(), before);
}

void kpmm_tests(void)
{
    KTEST_RUN("pmm", test_pmm_alloc_nonzero);
    KTEST_RUN("pmm", test_pmm_alloc_distinct);
    KTEST_RUN("pmm", test_pmm_alloc_aligned);
    KTEST_RUN("pmm", test_pmm_free_reuse);
    KTEST_RUN("pmm", test_pmm_available_accounting);
}
