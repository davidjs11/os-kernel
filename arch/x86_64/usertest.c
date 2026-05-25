/*   arch/x86_64/usertest.c   */

/* x86_64 user-mode shell loader. */

#include <kernel/arch/usertest.h>
#include <kernel/arch/paging.h>
#include <kernel/platform/mem.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <kernel/task.h>
#include <kernel/types.h>

/* Must match the base address in user/user.ld. */
#define USER_ENTRY_VA 0x400000UL

/* Shell binary (see usertest_blob.S). */
extern const uint8_t user_shell_blob[];
extern const uint8_t user_shell_blob_end[];

task_t *arch_user_test_create(void)
{
    size_t size = (size_t) (user_shell_blob_end - user_shell_blob);
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    vaddr_t hhdm = platform_mem_get_hhdm_vaddr();

    task_t *task = task_create_user(
        "shell",
        (void (*)(void)) USER_ENTRY_VA,
        TASK_PRIORITY_NORM
    );
    if (task == NULL) {
        return NULL;
    }

    /*
     * Copy the binary to new page frames and map them as read and execute.
     * The shell only uses its stack, so code pages don't need to be writable.
     */
    for (size_t p = 0; p < num_pages; p++) {
        paddr_t paddr = pmm_alloc();
        if (paddr == 0) {
            task_delete(task);
            return NULL;
        }

        uint8_t *dst = (uint8_t *) (paddr + hhdm);
        size_t offset = p * PAGE_SIZE;
        size_t chunk = (size - offset < PAGE_SIZE) ? size - offset : PAGE_SIZE;

        for (size_t i = 0; i < PAGE_SIZE; i++) {
            dst[i] = (i < chunk) ? user_shell_blob[offset + i] : 0;
        }

        arch_paging_map(
            task->space->pt_phys,
            USER_ENTRY_VA + offset,
            paddr,
            PROT_READ | PROT_EXEC | PROT_USER | PROT_PRESENT
        );
    }

    return task;
}
