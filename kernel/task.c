/*   include/task.c   */

/* Task management implementation. */

#include <kernel/task.h>
#include <kernel/kheap.h>

static uint64_t last_pid = 0;

task_t *task_create(
        const char *name,
        void (*entry)(void),
        uint8_t priority,
        vmm_space_t *space,
        task_t *parent)
{
    bool ok = false;

    /* Allocate task struct. */
    task_t *task = kmalloc(sizeof(*task));
    if (task == NULL) {
        /* TODO: log here. */
        goto done;
    }

    /* Allocate and initialize kernel stack for the task. */
    task->kstack = kmalloc(TASK_KSTACK_SIZE);
    if (task->kstack == NULL) {
        /* TODO: log here. */
        goto done;
    }
    task->kstack_size = TASK_KSTACK_SIZE;
    arch_context_init(
        &task->context,
        (uintptr_t) entry,
        (uintptr_t) (task->kstack + task->kstack_size)
    );

    /* Copy task name. */
    /* TODO: strncpy(task->name). */
    task->name[TASK_NAME_LENGTH - 1] = '\0';
    for (size_t i = 0; i < TASK_NAME_LENGTH - 1 && name[i]; i++) {
        task->name[i] = name[i];
    }
    task->name[TASK_NAME_LENGTH - 1] = '\0';

    /* TODO: Allocate PID function. */
    task->pid = last_pid++;
    task->state = TASK_READY;
    task->priority = priority;
    task->exit_code = 0;
    task->space = space;
    task->parent = parent;
    list_init(&task->sched_node);

    ok = true;
done:
    if (!ok) {
        kfree(task->kstack);
        kfree(task);
        task = NULL;
    }

    return task;
}

void task_delete(task_t *task)
{
    if (task == NULL) {
        return;
    }

    kfree(task->kstack);
    kfree(task);
}
