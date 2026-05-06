/*   include/task.c   */

/* Task management implementation. */

#include <kernel/task.h>
#include <kernel/kheap.h>
#include <kernel/libc/string.h>

static uint64_t last_pid = 0;

task_t *task_alloc(
        const char *name,
        void (*entry)(void),
        uint8_t priority,
        vmm_space_t *space,
        task_t *parent)
{
    bool ok = false;
    task_t *task = NULL;

    /* Allocate task struct. */
    task = kmalloc(sizeof(*task));
    if (task == NULL)  {
        goto done;
    }

    /* Zero the struct to avoid uninitialized fields. */
    memset(task, 0, sizeof(*task));

    /* Address space. */
    if (space == NULL) {
        task->space = vmm_space_create();
        if (task->space == NULL) {
            goto done;
        }
    } else {
        task->space = space;
    }

    /* Kernel stack. */
    task->kstack = kmalloc(TASK_KSTACK_SIZE);
    if (task->kstack == NULL) {
        goto done;
    }
    task->kstack_size = TASK_KSTACK_SIZE;

    /* Name. */
    for (size_t i = 0; i < TASK_NAME_LENGTH - 1 && name[i]; i++)
        task->name[i] = name[i];
    task->name[TASK_NAME_LENGTH - 1] = '\0';

    /* Fields. */
    task->pid        = last_pid++;
    task->state      = TASK_READY;
    task->priority   = priority;
    task->exit_code  = 0;
    task->parent     = parent;
    task->entry      = entry;

    list_init(&task->sched_node);

    ok = true;
done:
    if (!ok && task != NULL) {
        if (space == NULL && task->space != NULL) {
            vmm_space_delete(task->space);
        }
        kfree(task->kstack);
        kfree(task);
        task = NULL;
    }

    return task;
}

extern vmm_space_t kernel_space;

task_t *task_create_kernel(
        const char *name,
        void (*entry)(void),
        uint8_t priority)
{
    task_t *task = task_alloc(name, entry, priority, &kernel_space, NULL);
    if (task == NULL) {
        return NULL;
    }

    arch_context_init(
        &task->context,
        (uintptr_t) entry,
        (uintptr_t) (task->kstack + task->kstack_size)
    );

    return task;
}

task_t *task_create_user(
        const char *name,
        void (*entry)(void),
        uint8_t priority)
{
    task_t *task = task_alloc(name, entry, priority, NULL, NULL);
    if (task == NULL) {
        return NULL;
    }

    /* Allocate and initialize user stack for the task. */
    vmm_object_t *ustack_object = vmm_object_create_anon();
    vmm_region_t *ustack_region = vmm_region_create(
        TASK_USTACK_BASE,
        TASK_USTACK_SIZE,
        PROT_READ | PROT_WRITE | PROT_USER
    );
    vmm_region_set_object(ustack_region, ustack_object);
    vmm_space_add_region(task->space, ustack_region);
    task->ustack      = TASK_USTACK_BASE;
    task->ustack_size = TASK_USTACK_SIZE;

    arch_context_init_user(
        &task->context,
        (uintptr_t) entry,
        (uintptr_t) (task->kstack + task->kstack_size),
        (uintptr_t) (task->ustack + task->ustack_size)
    );

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
