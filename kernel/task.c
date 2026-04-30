/*   include/task.c   */

/* Task management implementation. */

#include <kernel/task.h>
#include <kernel/kheap.h>

task_t *task_create(
        const char *name,
        vaddr_t entry,
        vmm_space_t *space,
        uint8_t priority)
{
    /* Allocate task struct. */
    task_t *task = kmalloc(sizeof(*task));
    if (task == NULL) {
        goto done;
    }

    /* TODO: strcpy(task->name). */
    task->name[TASK_NAME_LENGTH - 1] = '\0';
    for (size_t i = 0; i < TASK_NAME_LENGTH - 1 && name[i]; i++) {
        task->name[i] = name[i];
    }

    /* Create a new address space for the task or add the given one. */
    if (space == NULL) {
        /* TODO. */
    } else {
        task->space = space;
    }

    /* Allocate kernel stack. */
    uint8_t *kernel_stack = kmalloc(KERNEL_STACK_SIZE);
    if (kernel_stack == NULL) {
        kfree(task);
        task = NULL;
        goto done;
    }
    task->kstack      = kernel_stack;
    task->kstack_size = KERNEL_STACK_SIZE;
    
    task->priority = priority;
    task->state    = TASK_READY;

done:
    return task;
}

void task_delete(task_t *task)
{

}
