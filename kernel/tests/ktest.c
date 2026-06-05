/*   kernel/tests/ktest.c   */

/* Kernel unit test framework implementation. */

#include <kernel/ktest.h>
#include <kernel/kprintf.h>

static int total_passed = 0;
static int total_failed = 0;
static int case_failures = 0; /* Failed assertions in the current test. */

void ktest_init(void)
{
    total_passed  = 0;
    total_failed  = 0;
    case_failures = 0;
    kprintf("[ktest] ========================================\n");
    kprintf("[ktest] kernel test suite\n");
    kprintf("[ktest] ========================================\n");
}

void ktest_run(const char *suite, const char *name, void (*fn)(void))
{
    case_failures = 0;
    fn();

    if (case_failures == 0) {
        kprintf("[ktest] PASS  %s::%s\n", suite, name);
        total_passed++;
    } else {
        kprintf(
            "[ktest] FAIL  %s::%s  (%d failed)\n",
            suite,
            name,
            case_failures
        );
        total_failed++;
    }
}

int ktest_summary(void)
{
    int total = total_passed + total_failed;

    kprintf("[ktest] ========================================\n");
    if (total_failed == 0) {
        kprintf("[ktest] ALL PASS  %d/%d\n", total_passed, total);
    } else {
        kprintf(
            "[ktest] FAILED    %d/%d passed, %d failed\n",
            total_passed,
            total,
            total_failed
        );
    }
    kprintf("[ktest] ========================================\n");

    return (total_failed > 0) ? 1 : 0;
}

void _ktest_assert(int cond, const char *expr, const char *file, int line)
{
    if (!cond) {
        kprintf("[ktest]   assert failed: %s  (%s:%d)\n", expr, file, line);
        case_failures++;
    }
}

void _ktest_assert_eq(
    uintptr_t a,
    uintptr_t b,
    const char *as,
    const char *bs,
    const char *file,
    int line)
{
    if (a != b) {
        kprintf(
            "[ktest]   assert failed: %s == %s  (%p != %p)  (%s:%d)\n",
            as,
            bs,
            (void *) a,
            (void *) b,
            file,
            line
        );
        case_failures++;
    }
}

void _ktest_assert_ne(
    uintptr_t a,
    uintptr_t b,
    const char *as,
    const char *bs,
    const char *file,
    int line)
{
    if (a == b) {
        kprintf(
            "[ktest]   assert failed: %s != %s  (both %p)  (%s:%d)\n",
            as,
            bs,
            (void *) a,
            file,
            line
        );
        case_failures++;
    }
}

void _ktest_assert_ge(
    uintptr_t a,
    uintptr_t b,
    const char *as,
    const char *bs,
    const char *file,
    int line)
{
    if (a < b) {
        kprintf(
            "[ktest]   assert failed: %s >= %s  (%p < %p)  (%s:%d)\n",
            as,
            bs,
            (void *) a,
            (void *) b,
            file,
            line
        );
        case_failures++;
    }
}
