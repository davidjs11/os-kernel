/*   include/kernel/arch/usertest.h   */

/* Architecture-specific user-mode test task. */

#ifndef KERNEL_API_ARCH_USERTEST_H
#define KERNEL_API_ARCH_USERTEST_H

#include <kernel/task.h>

/*
 * @brief Create a user task running the user-space shell embedded in the
 *        kernel image.
 *
 * @return The created task, or NULL on failure.
 */
task_t *arch_user_test_create(void);

#endif /* KERNEL_API_ARCH_USERTEST_H */
