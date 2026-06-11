/*   arch/x86_64/syscall.c   */

/* System call architecture support. */

#include "msr.h"
#include "segment.h"
#include "syscall.h"

extern void syscall_entry(void);

void syscall_init(void)
{
    /* Enable SYSCALL/SYSRET. */
    uint64_t efer = msr_read(MSR_EFER);
    msr_write(MSR_EFER, efer | EFER_SCE);

    /*
     * STAR layout:
     *  bits 63:48 = user CS   (SYSRET  sets CS to this + 16, SS to this + 8)
     *  bits 47:32 = kernel CS (SYSCALL sets CS to this,      SS to this + 8)
     */
    uint64_t star = ((uint64_t) SEL_KCODE << 32) |
                    ((uint64_t) (SEL_UDATA - 8) << 48);
    msr_write(MSR_STAR, star);

    /* Syscall entry point */
    msr_write(MSR_LSTAR, (uint64_t) syscall_entry);

    /* Clear IF on syscall entry (disable interrupts). */
    msr_write(MSR_SFMASK, (1 << 9));
}
