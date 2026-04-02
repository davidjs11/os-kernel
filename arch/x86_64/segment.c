/*   arch/x86_64/segment.c   */

#include "segment.h"
#include <stddef.h>

/* TODO: move this to another module. */
void *memset(void *s, int c, size_t n)
{
    register unsigned char *ptr = (unsigned char *) s;

    while (n--) {
        *ptr++ = (unsigned char) c;
    }

    return s;
};

/* GDT and TSS declaration. */
__attribute__((aligned(16))) static gdt_t gdt; /* General Descriptor Table. */
__attribute__((aligned(16))) static tss_t tss; /* Task Struct Segment. */

/* Interrupt Stack Table (IST) declaration. */
#define IST_STACK_SIZE 4096
__attribute__((aligned(16))) static uint8_t ist_stack_db[IST_STACK_SIZE];
__attribute__((aligned(16))) static uint8_t ist_stack_nmi[IST_STACK_SIZE];
__attribute__((aligned(16))) static uint8_t ist_stack_df[IST_STACK_SIZE];
__attribute__((aligned(16))) static uint8_t ist_stack_mc[IST_STACK_SIZE];

void segment_init(void)
{
    /* Zero the TSS, set IOPB offset beyond the TSS to disable it. */
    memset(&tss, 0, sizeof(tss_t));
    tss.iopb_offset = (uint16_t) sizeof(tss_t);

    /* Populate IST entries. */
    tss.ist[0] = (uintptr_t) ist_stack_db  + IST_STACK_SIZE;
    tss.ist[1] = (uintptr_t) ist_stack_nmi + IST_STACK_SIZE;
    tss.ist[2] = (uintptr_t) ist_stack_df  + IST_STACK_SIZE;
    tss.ist[3] = (uintptr_t) ist_stack_mc  + IST_STACK_SIZE;

    /* Populate the GDT. */
    gdt.null  = SEG_NULL;
    gdt.kcode = SEG_KCODE64;
    gdt.kdata = SEG_KDATA64;
    gdt.udata = SEG_UDATA64;
    gdt.ucode = SEG_UCODE64;
    gdt.tss   = TSS_DESC(&tss, sizeof(tss_t) - 1);

    /* Build the GDTR pseudo-descriptor and load it. */
    gdtr_t gdtr = {
        .base  = (uintptr_t) &gdt,
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
        "xorw %%ax, %%ax \n\t"
        "movw %%ax, %%ds \n\t"
        "movw %%ax, %%es \n\t"
        "movw %%ax, %%fs \n\t"
        "movw %%ax, %%gs \n\t"
        :
        : "r" ((uint16_t) SEL_KDATA)
        : "ax", "memory"
    );

    /* Load the Task Register. */
    __asm__ volatile (
        "ltr %w0"
        :
        : "r" ((uint16_t) SEL_TSS)
        : "memory"
    );
}
