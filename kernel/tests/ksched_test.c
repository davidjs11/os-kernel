/*   kernel/tests/ksched_test.c   */

/* Scheduler tests. */

#include <kernel/ktest.h>
#include <kernel/task.h>
#include <kernel/sched.h>

extern bool need_reschedule;

/* Enqueued tasks are ready. */
static void test_sched_enqueue_sets_ready(void)
{
    task_t *t = task_create_kernel("ks_enq", NULL, TASK_PRIORITY_NORM);
    KTEST_ASSERT_NOT_NULL(t);
    if (t == NULL) {
        return;
    }

    /* New tasks are ready. */
    KTEST_ASSERT_EQ(t->state, TASK_READY);

    /* A blocked task is ready again after enqueueing it. */
    t->state = TASK_BLOCKED;
    sched_enqueue(t);
    KTEST_ASSERT_EQ(t->state, TASK_READY);

    sched_dequeue(t);
    task_delete(t);
}

/*
 * sched_yield() only marks a reschedule as pending, the switch happens on the
 * next IRQ. Tests run with interrupts disabled, so only the flag is checked.
 */
static void test_sched_yield_sets_flag(void)
{
    need_reschedule = false;
    sched_yield();
    KTEST_ASSERT(need_reschedule);
    need_reschedule = false;
}

void ksched_tests(void)
{
    KTEST_RUN("scheduler", test_sched_enqueue_sets_ready);
    KTEST_RUN("scheduler", test_sched_yield_sets_flag);
}
