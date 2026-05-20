/*   kernel/sched/sched_rr.c   */

/* Round-robin scheduler algorithm implementation. */

#include <kernel/sched.h>
#include <kernel/types.h>

sched_ops_t _sched_algorithm_rr;

static list_node_t run_queue;

void sched_rr_init(void)
{
    list_init(&run_queue);
}

/* Number of ticks a task can run before being preempted. */
static uint8_t rr_quantum(uint8_t priority)
{
    if (priority == 0) {
        return 1;
    }
    if (priority > TASK_PRIORITY_QUANTUM_MAX) {
        return TASK_PRIORITY_QUANTUM_MAX;
    }
    return priority;
}

void sched_rr_enqueue(task_t *task)
{
    task->quantum_remaining = rr_quantum(task->priority);
    list_push_back(&run_queue, &task->sched_node);
}

void sched_rr_dequeue(task_t *task)
{
    list_remove(&task->sched_node);
}

void sched_rr_block(task_t *task)
{
    list_remove(&task->sched_node);
}

void sched_rr_unblock(task_t *task)
{
    task->state = TASK_READY;
    sched_rr_enqueue(task);
}

task_t *sched_rr_pick_next(void)
{
    if (list_empty(&run_queue)) {
        return NULL;
    }

    list_node_t *node = list_first(&run_queue);
    list_remove(node);
    return LIST_CONTAINER_OF(node, task_t, sched_node);
}

void sched_rr_tick(void)
{
    task_t *task = sched_current();

    /* First tick, coming from kmain. */
    if (task == NULL) {
        sched_yield();
        return;
    }

    if (task->quantum_remaining > 0) {
        task->quantum_remaining--;
    }
    if (task->quantum_remaining == 0) {
        sched_yield();
    }
}

sched_ops_t _sched_algorithm_rr = (sched_ops_t) {
    .init      = sched_rr_init,
    .enqueue   = sched_rr_enqueue,
    .dequeue   = sched_rr_dequeue,
    .block     = sched_rr_block,
    .unblock   = sched_rr_unblock,
    .tick      = sched_rr_tick,
    .pick_next = sched_rr_pick_next,
};
