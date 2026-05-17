/*   kernel/sched/sched.c   */

/* Kernel scheduler implementation. */

#include <kernel/sched.h>
#include <kernel/kprintf.h>

static sched_ops_t   *sched_ops = SCHED_OPS_DEFAULT;
static task_t        *current   = NULL;
static task_t        *idle      = NULL;
static cpu_context_t kmain_context; /* Saves kmain state separately. */
bool need_reschedule = false;

/* Idle task function. */
static void idle_function(void) {
    for (;;);
}

/*
 * @brief Perform the actual task switch from current to next. Called by
 *        sched_yield, sched_tick, sched_block, sched_exit.
 *
 * @param next Next task to run.
 */
static void sched_switch(task_t *next) {
    task_t *prev = current;
    if (prev == next) {
        return;
    }

    if (prev->state == TASK_RUNNING) {
        prev->state = TASK_READY;
    }

    next->state = TASK_RUNNING;
    current = next;
    arch_task_activate();

    arch_context_switch(&prev->context, &next->context);
}

void sched_init(sched_ops_t *ops)
{
    if (ops != NULL) {
        sched_ops = ops;
    }

    sched_ops->init();
    kprintf("[sched] Initialized scheduler.\n");

    idle = task_create_kernel("idle", idle_function, TASK_PRIORITY_MIN);
    idle->state = TASK_RUNNING;
    kprintf("[sched] Created idle task.\n");

    /* As sched_init is called from kmain, current task is NULL. */
    current = NULL;
}

void sched_enqueue(task_t *task)
{
    task->state = TASK_READY;
    sched_ops->enqueue(task);
}

void sched_dequeue(task_t *task)
{
    sched_ops->dequeue(task);
}

void sched_block(void)
{
    current->state = TASK_BLOCKED;
    sched_ops->block(current);

    task_t *next = sched_ops->pick_next();
    if (next == NULL) {
        next = idle;
    }

    sched_switch(next);
}

void sched_unblock(task_t *task)
{
    sched_ops->unblock(task);
}

void sched_yield(void) {
    need_reschedule = true;
}

void sched_tick(void)
{
    need_reschedule = true;
}

void sched_exit(int exit_code)
{
    /* Mark task as dead. */
    current->state     = TASK_DEAD;
    current->exit_code = exit_code;

    /* Obtain next task. */
    task_t *next = sched_ops->pick_next();
    if (next == NULL) {
        next = idle;
    }
    sched_switch(next);
}

task_t *sched_current(void) {
    return current;
}

void sched_resched(void)
{
    if (sched_ops == NULL || !need_reschedule) {
        return;
    }
    need_reschedule = false;

    task_t *prev = current;
    task_t *next = sched_ops->pick_next();

    if (next == NULL) {
        next = idle;
    }
    if (next == prev) {
        return;
    }

    if (prev != NULL && prev->state == TASK_RUNNING) {
        prev->state = TASK_READY;
        sched_ops->enqueue(prev);
    }

    next->state = TASK_RUNNING;
    current     = next;
    arch_task_activate();

    /* First switch comes from kmain, whose context is not saved. */
    if (prev == NULL) {
        arch_context_first_switch(&next->context);
    } else {
        arch_context_switch(&prev->context, &next->context);
    }
}
