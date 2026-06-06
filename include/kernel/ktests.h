/*   include/kernel/ktests.h   */

/* Kernel test suites. */

#ifndef KERNEL_API_KTESTS_H
#define KERNEL_API_KTESTS_H

/*
 * @brief Run all the kernel test suites and print a summary. Must be called
 *        before enabling interrupts.
 *
 * @return 0 if all tests passed, 1 otherwise.
 */
int ktests_run_all(void);

#endif /* KERNEL_API_KTESTS_H */
