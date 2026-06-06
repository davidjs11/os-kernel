/*   kernel/tests/kcontext_test.c   */

/* Context switching tests. */

#include <kernel/ktest.h>
#include <kernel/arch/context.h>

#define CTX_STACK_SIZE 4096

static uint8_t stack_a[CTX_STACK_SIZE] __attribute__((aligned(16)));
static uint8_t stack_b[CTX_STACK_SIZE] __attribute__((aligned(16)));

static cpu_context_t ctx_main;
static cpu_context_t ctx_a;
static cpu_context_t ctx_b;

/* Set by task_a_fn when it's back from task_b_fn. */
static volatile int ctx_round_trip = 0;

static void task_b_fn(void);

static void task_a_fn(void)
{
    arch_context_switch(&ctx_a, &ctx_b);
    ctx_round_trip = 1;
    arch_context_switch(&ctx_a, &ctx_main);
}

static void task_b_fn(void)
{
    arch_context_switch(&ctx_b, &ctx_a);
}

/* Switches main -> A -> B -> A -> main. */
static void test_context_switch(void)
{
    ctx_round_trip = 0;

    arch_context_init(
        &ctx_a,
        (uintptr_t) task_a_fn,
        (uintptr_t) (stack_a + CTX_STACK_SIZE)
    );

    arch_context_init(
        &ctx_b,
        (uintptr_t) task_b_fn,
        (uintptr_t) (stack_b + CTX_STACK_SIZE)
    );

    arch_context_switch(&ctx_main, &ctx_a);

    KTEST_ASSERT_EQ(ctx_round_trip, 1);
}

void kcontext_tests(void)
{
    KTEST_RUN("context", test_context_switch);
}
