/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <kernel/arch/init.h>
#include <kernel/arch/paging.h>
#include <kernel/arch/interrupt.h>
#include <kernel/arch/usertest.h>
#include <kernel/platform/init.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <kernel/list.h>
#include <kernel/kheap.h>
#include <kernel/task.h>
#include <kernel/kprintf.h>
#include <kernel/sched.h>
#include <kernel/ktimer.h>
#include <kernel/ktests.h>
#include <kernel/device/framebuffer/framebuffer.h>

#define DEMO_BAND_COUNT 4

/* Horizontal progress, shared by all the demonstration tasks. */
static uint64_t demo_x = 0;

/*
 * @brief Number of steps of the demonstration: framebuffer width, or a fixed
 *        number of steps if there is no framebuffer.
 */
static uint64_t demo_steps(void)
{
    uint64_t width = framebuffer_get_width();
    return (width != 0) ? width : 256;
}

/*
 * @brief Demonstration task body. Paints its own horizontal band of the
 *        framebuffer, one column per step, until all the columns are painted.
 *        Nothing is printed, so the serial console is left free.
 *
 * @param band  Band index (from top to bottom).
 * @param color Color of the band.
 */
static void demo_band(uint8_t band, uint32_t color)
{
    uint64_t steps = demo_steps();

    while (demo_x < steps) {
        if (framebuffer_has_default()) {
            uint64_t height  = framebuffer_get_height();
            uint64_t y_start = band * height / DEMO_BAND_COUNT;
            uint64_t y_end   = (band + 1) * height / DEMO_BAND_COUNT;
            for (uint64_t y = y_start; y < y_end; y++) {
                framebuffer_set_pixel(demo_x, y, color);
            }
        }

        demo_x++;
        for (volatile int i = 0; i < 500000; i++);
    }

    sched_exit(0);
}

static void task_a(void) { demo_band(0, 0xffffff00); }
static void task_b(void) { demo_band(1, 0xff00ff00); }
static void task_d(void) { demo_band(2, 0xffff0000); }
static void task_e(void) { demo_band(3, 0xff0000ff); }

/*
 * @brief Kernel entry point.
 */
void kmain(void)
{
    arch_init();
    platform_init();
    pmm_init(NULL);
    vmm_init();
    sched_init(NULL);
    ktimer_init(100);

#ifdef DEBUG
    ktests_run_all();
#endif /* DEBUG */

    /* Different priorities, so each one gets a different quantum. */
    task_t *a = task_create_kernel("a", task_a, 1);
    task_t *b = task_create_kernel("b", task_b, 2);
    task_t *d = task_create_kernel("d", task_d, 4);
    task_t *e = task_create_kernel("e", task_e, 8);
    sched_enqueue(a);
    sched_enqueue(b);
    sched_enqueue(d);
    sched_enqueue(e);

    task_t *user = arch_user_test_create();
    if (user != NULL) {
        sched_enqueue(user);
        kprintf("[kmain] User task created.\n");
    } else {
        kprintf("[kmain] User task creation failed.\n");
    }

    arch_interrupt_enable();

    /* Serial input belongs to the user shell, just give the CPU away. */
    for (;;) {
        sched_yield();
    }
}
