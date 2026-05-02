/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <kernel/arch.h>
#include <kernel/arch/paging.h>
#include <kernel/arch/interrupt.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <kernel/list.h>
#include <kernel/kheap.h>
#include <kernel/task.h>
#include <kernel/platform/serial.h>
#include <kernel/kprintf.h>

#define TEST_STACK_SIZE 4096

static uint8_t stack_a[TEST_STACK_SIZE] __attribute__((aligned(16)));
static uint8_t stack_b[TEST_STACK_SIZE] __attribute__((aligned(16)));

static cpu_context_t ctx_main;
static cpu_context_t ctx_a;
static cpu_context_t ctx_b;

static void task_b(void);

static void task_a(void) {
    kprintf("[task_a] first entry\n");
    arch_context_switch(&ctx_a, &ctx_b);
    kprintf("[task_a] resumed\n");
    arch_context_switch(&ctx_a, &ctx_main);
    /* should never reach here */
    kprintf("[task_a] ERROR: should not reach here\n");
}


static void task_b(void) {
    kprintf("[task_b] first entry\n");
    arch_context_switch(&ctx_b, &ctx_a);
    /* should never reach here */
    kprintf("[task_b] ERROR: should not reach here\n");
}

void context_test(void) {
    kprintf("[main] initializing contexts\n");
    arch_context_init(
        &ctx_a,
        (uintptr_t) task_a,
        (uintptr_t) (stack_a + TEST_STACK_SIZE)
    );

    arch_context_init(
        &ctx_b,
        (uintptr_t) task_b,
        (uintptr_t) (stack_b + TEST_STACK_SIZE)
    );

    kprintf("[main] switching to task_a\n");
    arch_context_switch(&ctx_main, &ctx_a);
    kprintf("[main] back from tasks, test passed!\n");
}

void task_test(void) {
    // creation
    task_t *a = task_create("task_a", NULL, TASK_PRIORITY_NORM, NULL, NULL);
    task_t *b = task_create("task_b", NULL, TASK_PRIORITY_NORM, NULL, a);

    kprintf("a: pid=%d name=%s parent=%p\n", a->pid, a->name, a->parent);
    kprintf("b: pid=%d name=%s parent=%p\n", b->pid, b->name, b->parent);

    // verify parent relationship
    if (b->parent != a)
        kprintf("ERROR: parent mismatch\n");
    else
        kprintf("parent relationship OK\n");

    // verify kstack is allocated and aligned
    if ((uintptr_t)a->kstack % 16 != 0)
        kprintf("ERROR: kstack misaligned\n");
    else
        kprintf("kstack alignment OK\n");

    // verify context rsp points inside kstack
    uintptr_t rsp = a->context.rsp;
    uintptr_t base = (uintptr_t)a->kstack;
    if (rsp < base || rsp > base + a->kstack_size)
        kprintf("ERROR: rsp outside kstack\n");
    else
        kprintf("rsp inside kstack OK\n");

    // verify PIDs are unique and incrementing
    if (b->pid != a->pid + 1)
        kprintf("ERROR: pid allocation wrong\n");
    else
        kprintf("pid allocation OK\n");

    // delete
    task_delete(b);
    task_delete(a);
    kprintf("task_test done\n");
}

/*
 * @brief Kernel entry point.
 */
void kmain(void)
{
    arch_init();

    arch_interrupt_init();
    arch_interrupt_enable();
    kprintf("[!] Initialized interrupts.\n");

    pmm_init(NULL);
    kprintf("[!] Initialized PMM.\n");

    vmm_init();
    kprintf("[!] Initialized VMM.\n");

    // task_create("test task", (vaddr_t) &kmain, 0, NULL);

    context_test();
    task_test();

    kprintf("[!] Starting infinite loop.\n");
    for (;;);
    return;
}

