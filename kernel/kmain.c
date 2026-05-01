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

#define TEST_STACK_SIZE 4096

static uint8_t stack_a[TEST_STACK_SIZE] __attribute__((aligned(16)));
static uint8_t stack_b[TEST_STACK_SIZE] __attribute__((aligned(16)));

static cpu_context_t ctx_main;
static cpu_context_t ctx_a;
static cpu_context_t ctx_b;

static void task_b(void);

void kprintf(const char *str) {
    str; /* Do nothing. */
}

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
            (uintptr_t) (stack_a + TEST_STACK_SIZE),
            false);

    arch_context_init(
            &ctx_b,
            (uintptr_t) task_b,
            (uintptr_t) (stack_b + TEST_STACK_SIZE),
            false);

    kprintf("[main] switching to task_a\n");
    arch_context_switch(&ctx_main, &ctx_a);
    kprintf("[main] back from tasks, test passed!\n");
}

/*
 * @brief Kernel entry point.
 */
void kmain(void)
{
    arch_init();
    arch_interrupt_init();
    arch_interrupt_enable();
    pmm_init(NULL);
    vmm_init();

    task_create("test task", (vaddr_t) &kmain, NULL, 0);

    context_test();

    for (;;);
    return;
}

