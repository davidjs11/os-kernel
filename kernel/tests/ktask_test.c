/*   kernel/tests/ktask_test.c   */

/* Task management tests. */

#include <kernel/ktest.h>
#include <kernel/task.h>

/* Kernel task creation. */
static void test_task_create_not_null(void)
{
    task_t *t = task_create_kernel("kt_task", NULL, TASK_PRIORITY_NORM);
    KTEST_ASSERT_NOT_NULL(t);
    task_delete(t);
}

/* Kernel tasks have no parent (hierarchy not implemented yet). */
static void test_task_parent_null(void)
{
    task_t *a = task_create_kernel("kt_a", NULL, TASK_PRIORITY_NORM);
    task_t *b = task_create_kernel("kt_b", NULL, TASK_PRIORITY_NORM);

    KTEST_ASSERT_NOT_NULL(a);
    KTEST_ASSERT_NOT_NULL(b);
    if (a != NULL && b != NULL) {
        KTEST_ASSERT_NULL(a->parent);
        KTEST_ASSERT_NULL(b->parent);
    }

    task_delete(b);
    task_delete(a);
}

/* Kernel stack is 16-byte aligned. */
static void test_task_kstack_aligned(void)
{
    task_t *t = task_create_kernel("kt_align", NULL, TASK_PRIORITY_NORM);

    KTEST_ASSERT_NOT_NULL(t);
    if (t != NULL) {
        KTEST_ASSERT_EQ((uintptr_t) t->kstack % 16, 0);
    }

    task_delete(t);
}

/* PIDs are unique and increasing. */
static void test_task_pid_unique(void)
{
    task_t *a = task_create_kernel("kt_pid_a", NULL, TASK_PRIORITY_NORM);
    task_t *b = task_create_kernel("kt_pid_b", NULL, TASK_PRIORITY_NORM);

    KTEST_ASSERT_NOT_NULL(a);
    KTEST_ASSERT_NOT_NULL(b);
    if (a != NULL && b != NULL) {
        KTEST_ASSERT_EQ(b->pid, a->pid + 1);
    }

    task_delete(b);
    task_delete(a);
}

void ktask_tests(void)
{
    KTEST_RUN("task", test_task_create_not_null);
    KTEST_RUN("task", test_task_parent_null);
    KTEST_RUN("task", test_task_kstack_aligned);
    KTEST_RUN("task", test_task_pid_unique);
}
