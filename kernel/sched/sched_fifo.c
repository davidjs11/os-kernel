/*   kernel/sched/sched_fifo.h   */

/* FIFO scheduler algorithm implementation. */

#include <kernel/sched.h>
#include <kernel/types.h>

sched_ops_t _sched_algorithm_fifo;

static list_node_t run_queue;

void sched_fifo_init(void)
{
    list_init(&run_queue);
}

void sched_fifo_enqueue(task_t *task)
{
    list_push_back(&run_queue, &task->sched_node);
}

void sched_fifo_dequeue(task_t *task)
{
    list_remove(&task->sched_node);
}

void sched_fifo_block(task_t *task)
{
    list_remove(&task->sched_node);
}

void sched_fifo_unblock(task_t *task)
{
    task->state = TASK_READY;
    list_push_back(&run_queue, &task->sched_node);
}

task_t *sched_fifo_pick_next(void)
{
    if (list_empty(&run_queue)) {
        return NULL;
    }

    list_node_t *node = list_first(&run_queue);
    list_remove(node);
    return LIST_CONTAINER_OF(node, task_t, sched_node);
}

sched_ops_t _sched_algorithm_fifo = (sched_ops_t) {
    .init      = sched_fifo_init,
    .enqueue   = sched_fifo_enqueue,
    .dequeue   = sched_fifo_dequeue,
    .block     = sched_fifo_block,
    .unblock   = sched_fifo_unblock,
    .tick      = NULL, /* Not preemptive. */
    .pick_next = sched_fifo_pick_next,
};
