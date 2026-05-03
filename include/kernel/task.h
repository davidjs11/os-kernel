/*   include/task.h   */

/* Task-struct definition and task management API declaration. */

#ifndef KERNEL_API_TASK_H
#define KERNEL_API_TASK_H

#include <kernel/arch/context.h>
#include <kernel/types.h>
#include <kernel/list.h>
#include <kernel/vmm.h>

#define TASK_NAME_LENGTH    32
#define TASK_KSTACK_SIZE    2 * 0x1000 /* 2 pages. */
#define TASK_PRIORITY_MIN   0
#define TASK_PRIORITY_MAX   255
#define TASK_PRIORITY_NORM  128

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
 * @brief Allocate and initialize a new task.
 *
 * @param name     Name of the task.
 * @param entry    Function to run, or ELF e_entry.
 * @param priority Task priority (TASK_PRIORITY_NORM is default).
 * @param space    NULL for kernel tasks; vmm_space_create() for a new
 *                 userspace process; parent->space for a new thread.
 *                 Caller is responsible of allocating/freeint the new space.
 * @param parent   Pointer to the parent task (NULL if none).
 *
 * @return NULL on allocation failure.
 */
task_t *task_create(
        const char *name,
        void (*entry)(void),
        uint8_t priority,
        vmm_space_t *space,
        task_t *parent);

/*
 * @brief Delete a task and free its resources. Frees the task's kernel stack
 *        and the task struct itself. The task must not be running or enqueued
 *        in the scheduler when this is called.
 *
 * @param task The task to delete.
 */
void task_delete(task_t *task);

#endif /* KERNEL_API_TASK_H */
