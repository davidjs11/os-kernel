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
#include <kernel/sched.h>

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
void kmalloc_test(void) {
    kprintf("[kmalloc_test] start\n");

    /* single allocations of increasing sizes */
    void *p1 = kmalloc(8);
    kprintf("[kmalloc_test] 8b:    %p\n", p1);
    void *p2 = kmalloc(64);
    kprintf("[kmalloc_test] 64b:   %p\n", p2);
    void *p3 = kmalloc(256);
    kprintf("[kmalloc_test] 256b:  %p\n", p3);
    void *p4 = kmalloc(4096);
    kprintf("[kmalloc_test] 4096b: %p\n", p4);
    void *p5 = kmalloc(8192);
    kprintf("[kmalloc_test] 8192b: %p\n", p5);

    /* free and reallocate */
    kfree(p1);
    kfree(p3);
    void *p6 = kmalloc(8);
    kprintf("[kmalloc_test] realloc 8b:   %p (should reuse p1=%p)\n", p6, p1);
    void *p7 = kmalloc(256);
    kprintf("[kmalloc_test] realloc 256b: %p (should reuse p3=%p)\n", p7, p3);

    /* many small allocations */
    void *ptrs[64];
    for (int i = 0; i < 64; i++) {
        ptrs[i] = kmalloc(64);
        if (!ptrs[i]) {
            kprintf("[kmalloc_test] FAIL: alloc %d returned NULL\n", i);
            return;
        }
    }
    kprintf("[kmalloc_test] 64 small allocs OK\n");

    /* free all */
    for (int i = 0; i < 64; i++)
        kfree(ptrs[i]);
    kprintf("[kmalloc_test] 64 small frees OK\n");

    /* two consecutive page-sized allocs — this is what task_create does */
    void *s1 = kmalloc(sizeof(task_t));
    kprintf("[kmalloc_test] task_t sized: %p\n", s1);
    void *s2 = kmalloc(8192);
    kprintf("[kmalloc_test] kstack sized: %p\n", s2);
    if (!s1 || !s2)
        kprintf("[kmalloc_test] FAIL: task_create pattern failed\n");
    else
        kprintf("[kmalloc_test] task_create pattern OK\n");
    kfree(s1);
    kfree(s2);

    kprintf("[kmalloc_test] done\n");
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

    static volatile int order[8];
    static volatile int order_idx = 0;

static void task_a_sched(void) {
    order[order_idx++] = 1;
    kprintf("[task_a] first run\n");
    sched_yield();
    order[order_idx++] = 3;
    kprintf("[task_a] second run\n");
    sched_exit(0);
}

static void task_b_sched(void) {
    order[order_idx++] = 2;
    kprintf("[task_b] first run\n");
    sched_yield();
    order[order_idx++] = 4;
    kprintf("[task_b] second run\n");
    sched_exit(0);
}


void sched_test(void) {
    task_t *a = task_create("task_a", task_a_sched, TASK_PRIORITY_NORM, NULL, NULL);
    kprintf("[sched_test] task_a created\n");
    task_t *b = task_create("task_b", task_b_sched, TASK_PRIORITY_NORM, NULL, NULL);
    kprintf("[sched_test] task_b created\n");

    /* Test 1: basic enqueue and yield between two tasks. */
    sched_enqueue(a);
    sched_enqueue(b);

    kprintf("[sched_test] yielding into task_a\n");
    sched_yield();

    /* Verify execution order. */
    kprintf("[sched_test] verifying order\n");

    if (order[0] != 1) {
        kprintf("[FAIL] expected task_a first, got %d\n", order[0]);
    } else {
        kprintf("[OK]   task_a ran first\n");
    }

    if (order[1] != 2) {
        kprintf("[FAIL] expected task_b second, got %d\n", order[1]);
    } else {
        kprintf("[OK]   task_b ran second\n");
    }

    if (order[2] != 3) {
        kprintf("[FAIL] expected task_a third, got %d\n", order[2]);
    } else {
        kprintf("[OK]   task_a resumed correctly\n");
    }

    if (order[3] != 4) {
        kprintf("[FAIL] expected task_b fourth, got %d\n", order[3]);
    } else {
        kprintf("[OK]   task_b resumed correctly\n");
    }

    kprintf("[sched_test] done\n");
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

    sched_init();
    kprintf("[!] Initialized scheduler.\n");

    // kmalloc_test();
    // context_test();
    // task_test();
    // sched_test();

    sched_yield();
}

