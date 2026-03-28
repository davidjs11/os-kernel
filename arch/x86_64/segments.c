/*   arch/x86_64/segment.c   */

#include "segments.h"
#include <stddef.h>

/* TODO: move this to another module. */
void *memset(void *s, unsigned char c, size_t n) {
    register unsigned char *ptr = (unsigned char *) s;

    while (n--) {
        *ptr++ = c;
    }

    return s;
};

void segments_init(gdt_t *gdt, tss64_t *tss)
{
    /* Zero the TSS, then set IOPB offset beyond the TSS to disable it. */
    memset(tss, 0, sizeof(*tss));
    tss->iopb_offset = (uint16_t) sizeof(tss64_t);

    /* Populate the GDT. */
    gdt->null  = SEG_NULL;
    gdt->kcode = SEG_KCODE64;
    gdt->kdata = SEG_KDATA64;
    gdt->udata = SEG_UDATA64;
    gdt->ucode = SEG_UCODE64;

    /* Populate the TSS descriptor. */
    gdt->tss = TSS_DESC_LO(tss, sizeof(tss64_t) - 1);
    // gdt->tss.base_63_32 = (uint32_t) ((uint64_t) tss >> 32);
    // gdt->tss._reserved = 0;

    /* Build the GDTR pseudo-descriptor and load it. */
    gdtr_t gdtr = {
        .base  = (uint64_t) gdt,
        .limit = (uint16_t) (sizeof(gdt_t) - 1)
    };

    __asm__ volatile (
        "lgdt %0"
        :
        : "m" (gdtr)
        : "memory"
    );

    /* Reload CS. */
    __asm__ volatile (
        "pushq  %0               \n\t" /* New CS (SEL_KCODE). */
        "leaq   1f(%%rip), %%rax \n\t"
        "pushq  %%rax            \n\t" /* Return address. */
        "lretq                   \n\t" /* Far return: pop RIP and CS. */
        "1:                      \n\t"
        :
        : "i" ((uint64_t) SEL_KCODE)
        : "rax", "memory"
    );

    /* Reload the data and stack segment registers. */
    __asm__ volatile (
        "movw %w0,  %%ss \n\t"
        "movw $0,   %%ax \n\t"
        "movw %%ax, %%ds \n\t"
        "movw %%ax, %%es \n\t"
        "movw %%ax, %%fs \n\t"
        "movw %%ax, %%gs \n\t"
        :
        : "r" ((uint16_t) SEL_KDATA)
        : "memory"
    );

    /* Load the Task Register. */
    __asm__ volatile (
        "ltr %w0"
        :
        : "r" ((uint16_t) SEL_TSS)
        : "memory"
    );
}
