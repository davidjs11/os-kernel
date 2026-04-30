/*   include/task.h   */

/* Task-struct definition and task management API declaration. */

#ifndef KERNEL_API_TASK_H
#define KERNEL_API_TASK_H

#include <kernel/arch/context.h>
#include <kernel/types.h>
#include <kernel/list.h>
#include <kernel/vmm.h>

#define KERNEL_STACK_SIZE 16 * 1024 /* 16 KiB. */
#define TASK_NAME_LENGTH  32

/* Process (task) identifier. */
typedef uint64_t pid_t;

/* States of execution of a task.                                            */
typedef enum task_state {
    TASK_READY,     /* Task is ready to be executed.                         */
    TASK_RUNNING,   /* Task is currently running.                            */
    TASK_BLOCKED,   /* Task is blocked and waiting for some event to happen. */
    TASK_DEAD       /* Task has finished its execution.                      */
} task_state_t;

/* Task structure.                                                   */
typedef struct task {
    pid_t         pid;         /* Unique system-wide identifier.     */
    char          name[TASK_NAME_LENGTH];    /* Name of the process. */
    task_state_t  state;       /* Process state.                     */
    int           exit_code;   /* Task's exit code. */

    /* Memory. */
    vmm_space_t   *space;      /* Virtual memory address space.      */

    /* Kernel stack. */
    uint8_t       *kstack;     /* Kernel stack pointer for the task. */
    size_t        kstack_size; /* Size of this task's kernel stack.  */

    /* Arch-defined saved register state. */
    cpu_context_t context;     /* Saved registers.                   */

    /* Scheduling. */
    uint8_t       priority;    /* Priority level of the task.        */
    list_node_t   sched_node;  /* Mode inside scheduler's list.      */

    /* Hierarchy (future). */
    struct task   *parent;     /* Parent task.                       */
    list_node_t   children;    /* Head of child list.                */
    list_node_t   sibling_node;/* Node in parent's children list.    */
} task_t;

/*
 * @brief Create a new task.
 *
 * Allocates and initializes a new task with its own kernel stack. The task
 * is not enqueued in the scheduler automatically, call sched_enqueue() after
 * creation.
 *
 * @param name     Human-readable name for debugging purposes.
 * @param entry    Virtual address of the task's entry point.
 * @param space    Address space for this task.
 * @param priority Scheduling priority (higher value = higher priority).
 *
 * @return Pointer to the new task, or NULL on allocation failure.
 */
task_t *task_create(const char *name, vaddr_t entry, vmm_space_t *space, uint8_t priority);

/*
 * @brief Delete a task and free its resources.
 *
 * Frees the task's kernel stack and the task struct itself. The task must
 * not be running or enqueued in the scheduler when this is called.
 *
 * @param task The task to delete.
 */
void task_delete(task_t *task);

/*
 * @brief Block a task, preventing it from being scheduled.
 *
 * Transitions the task to BLOCKED state and removes it from the run queue.
 * The task will not run again until task_unblock() is called on it.
 *
 * @param task The task to block.
 */
void task_block(task_t *task);

/*
 * @brief Unblock a task, making it eligible for scheduling again.
 *
 * Transitions the task from BLOCKED to READY state and re-enqueues it
 * in the scheduler run queue.
 *
 * @param task The task to unblock.
 */
void task_unblock(task_t *task);

/*
 * @brief Get the currently running task.
 *
 * @return Pointer to the task currently executing on this CPU.
 */
task_t *task_current(void);

#endif /* KERNEL_API_TASK_H */
