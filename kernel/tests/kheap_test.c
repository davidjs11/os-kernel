/*   kernel/tests/kheap_test.c   */

/* Kernel heap tests. */

#include <kernel/ktest.h>
#include <kernel/kheap.h>
#include <kernel/task.h>

/* Allocations of different sizes. */
static void test_kmalloc_basic(void)
{
    void *p1 = kmalloc(8);
    void *p2 = kmalloc(64);
    void *p3 = kmalloc(256);
    void *p4 = kmalloc(4096);
    void *p5 = kmalloc(8192);

    KTEST_ASSERT_NOT_NULL(p1);
    KTEST_ASSERT_NOT_NULL(p2);
    KTEST_ASSERT_NOT_NULL(p3);
    KTEST_ASSERT_NOT_NULL(p4);
    KTEST_ASSERT_NOT_NULL(p5);

    kfree(p1);
    kfree(p2);
    kfree(p3);
    kfree(p4);
    kfree(p5);
}

/* Freed blocks can be allocated again. */
static void test_kmalloc_reuse(void)
{
    void *p1 = kmalloc(8);
    void *p2 = kmalloc(256);
    KTEST_ASSERT_NOT_NULL(p1);
    KTEST_ASSERT_NOT_NULL(p2);

    kfree(p1);
    kfree(p2);

    void *p3 = kmalloc(8);
    void *p4 = kmalloc(256);
    KTEST_ASSERT_NOT_NULL(p3);
    KTEST_ASSERT_NOT_NULL(p4);

    kfree(p3);
    kfree(p4);
}

/* Many small allocations at the same time. */
static void test_kmalloc_many(void)
{
    void *ptrs[64];

    for (int i = 0; i < 64; i++) {
        ptrs[i] = kmalloc(64);
        KTEST_ASSERT_NOT_NULL(ptrs[i]);
    }

    for (int i = 0; i < 64; i++) {
        kfree(ptrs[i]);
    }
}

/* Same allocations as task_create_kernel() (task struct and kernel stack). */
static void test_kmalloc_task_pattern(void)
{
    void *task = kmalloc(sizeof(task_t));
    void *kstack = kmalloc(TASK_KSTACK_SIZE);

    KTEST_ASSERT_NOT_NULL(task);
    KTEST_ASSERT_NOT_NULL(kstack);

    kfree(task);
    kfree(kstack);
}

void kheap_tests(void)
{
    KTEST_RUN("kheap", test_kmalloc_basic);
    KTEST_RUN("kheap", test_kmalloc_reuse);
    KTEST_RUN("kheap", test_kmalloc_many);
    KTEST_RUN("kheap", test_kmalloc_task_pattern);
}
