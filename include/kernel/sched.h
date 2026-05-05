/*   include/kernel/sched.h   */

/* Kernel scheduler API declaration. */

#ifndef KERNEL_API_SCHED_H
#define KERNEL_API_SCHED_H

#include <kernel/arch/context.h>
#include <kernel/types.h>
#include <kernel/list.h>
#include <kernel/task.h>

/* Scheduling algorithm interface. */
typedef struct sched_ops {
    void    (*init)(void);             /* Initialize scheduler.             */
    void    (*enqueue)(task_t *task);  /* Add a task to the run queue.      */
    void    (*dequeue)(task_t *task);  /* Remove a task from the run queue. */
    void    (*block)(task_t *task);    /* Block current task.               */
    void    (*unblock)(task_t *task);  /* Unblock and enqueue a task.       */
    void    (*tick)(void);             /* Called on timer tick.             */
    task_t *(*pick_next)(void);        /* Pick the next task to run.        */
} sched_ops_t;

/* List of scheduler algorithms. */
extern sched_ops_t _sched_algorithm_fifo;
extern sched_ops_t _sched_algorithm_rr;
#define SCHED_ALGORITHM_FIFO  (sched_ops_t *) (&_sched_algorithm_fifo)
#define SCHED_ALGORITHM_RR    (sched_ops_t *) (&_sched_algorithm_rr)
#define SCHED_OPS_DEFAULT     SCHED_ALGORITHM_RR

/*
 * @brief Initialize the scheduler. Must be called before any task is enqueued.
 *        Creates the idle task internally.
 */
void sched_init(sched_ops_t *ops);

/*
 * @brief Add a task to the run queue. State must be TASK_READY.
 *
 * @param task Task to be enqueued.
 */
void sched_enqueue(task_t *task);

/*
 * @brief Remove a task from the run queue. State must be TASK_DEAD.
 *
 * @param task Task to be enqueued.
 */
void sched_dequeue(task_t *task);

/*
 * @brief Block the current task. Switches away immediately. Caller is
 *        responsible for putting the task on a wait queue.
 */
void sched_block(void);

/*
 * @brief Unblock a task (move it from TASK_BLOCKED back to TASK_READY and
 *        enqueue it).
 *
 * @param task Task to be unblocked.
 */
void sched_unblock(task_t *task);

/*
 * @brief Called from the timer IRQ. Drives preemption.
 */
void sched_tick(void);

/*
 * @brief Mark current task as dead and switch away. Never returns.
 *
 * @param exit_code Exit code of the task.
 */
void sched_exit(int exit_code);

/*
 * @brief Voluntarily yield the CPU. Picks next task and switches.
 */
void sched_yield(void);

/*
 * @brief Returns the currently running task on this CPU.
 */
task_t *sched_current(void);

#endif /* KERNEL_API_SCHED_H */
