/*   kernel/tests/ktests.c   */

/* Kernel test suites runner. */

#include <kernel/ktest.h>
#include <kernel/ktests.h>

void kpmm_tests(void);
void kheap_tests(void);
void ktask_tests(void);
void ksched_tests(void);
void kcontext_tests(void);

int ktests_run_all(void)
{
    ktest_init();

    kpmm_tests();
    kheap_tests();
    ktask_tests();
    ksched_tests();

    /*
     * kcontext_tests() is not run here: starting a new context re-enables
     * interrupts (iretq/eret), and the timer is already unmasked, so a tick
     * would run the scheduler in the middle of the test. Context switching is
     * already tested by the multitasking demo.
     */

    return ktest_summary();
}
