/*   arch/x86_64/include/segment.h   */

/**
 * x86-64 segment (GDT and TSS) management.
 *
 * References
 *   Intel SDM Vol. 3A, Chapter 3  - Protected-Mode Memory Management
 *   Intel SDM Vol. 3A, Chapter 10 - Task Management
 */

#ifndef ARCH_X86_64_SEGMENT_H
#define ARCH_X86_64_SEGMENT_H

#include <stdint.h>

/* GDT selector indices (slot numbers, not byte offsets).               */
#define GDT_SLOT_NULL       0
#define GDT_SLOT_KCODE      1
#define GDT_SLOT_KDATA      2
#define GDT_SLOT_UDATA      3  /* user data below user code for SYSRET. */
#define GDT_SLOT_UCODE      4
#define GDT_SLOT_TSS_LO     5  /* TSS descriptor low 8 bytes.           */
#define GDT_SLOT_TSS_HI     6  /* TSS descriptor high 8 bytes.          */
#define GDT_NUM_SLOTS       7

/* Segment selectors (SDM Vol. 3A 3.4.2). */
#define SEG_SELECTOR(slot, rpl)   (((slot) << 3) | ((rpl) & 0x3))
#define SEL_KCODE   SEG_SELECTOR(GDT_SLOT_KCODE, 0)  /* 0x08 */
#define SEL_KDATA   SEG_SELECTOR(GDT_SLOT_KDATA, 0)  /* 0x10 */
#define SEL_UDATA   SEG_SELECTOR(GDT_SLOT_UDATA, 3)  /* 0x1B */
#define SEL_UCODE   SEG_SELECTOR(GDT_SLOT_UCODE, 3)  /* 0x23 */
#define SEL_TSS     SEG_SELECTOR(GDT_SLOT_TSS_LO, 0) /* 0x28 */

/* Segment descriptor (SDM Vol. 3A Fig. 3-8). */
typedef union __attribute__((packed)) seg_desc {
    uint64_t raw;
    struct {
        uint64_t limit_low  : 16; /* bits 15:0  - limit[15:0]         */
        uint64_t base_low   : 24; /* bits 39:16 - base[23:0]          */
        uint64_t type       :  4; /* bits 43:40 - descriptor type     */
        uint64_t s          :  1; /* bit  44    - 0 = system, 1 = seg */
        uint64_t dpl        :  2; /* bits 46:45 - privilege level     */
        uint64_t present    :  1; /* bit  47    - segment present     */
        uint64_t limit_high :  4; /* bits 51:48 - limit[19:16]        */
        uint64_t avl        :  1; /* bit  52    - available           */
        uint64_t l          :  1; /* bit  53    - 64-bit code seg     */
        uint64_t db         :  1; /* bit  54    - D/B flag            */
        uint64_t granularity:  1; /* bit  55    - granularity         */
        uint64_t base_high  :  8; /* bits 63:56 - base[31:24]         */
    } bits;
} seg_desc_t;

/* Descriptor type field values (SDM Vol. 3A Table 3-1, Table 3-2). */
/* Code/data descriptor types (s = 1).                              */
#define SEG_TYPE_DATA_RO        0x0 /* Read-Only.                   */
#define SEG_TYPE_DATA_RW        0x2 /* Read/Write.                  */
#define SEG_TYPE_CODE_XO        0x8 /* Execute-Only.                */
#define SEG_TYPE_CODE_XR        0xA /* Execute/Read.                */
#define SEG_TYPE_CODE_XR_CONF   0xE /* Execute/Read, conforming.    */

/* System descriptor types (s = 0), 64-bit mode.       */
#define SEG_TYPE_LDT            0x2 /* LDT descriptor. */
#define SEG_TYPE_TSS_AVAIL      0x9 /* TSS available.  */
#define SEG_TYPE_TSS_BUSY       0xB /* TSS busy.       */
#define SEG_TYPE_CALL_GATE      0xC /* Call gate.      */
#define SEG_TYPE_INTERRUPT_GATE 0xE /* Interrupt gate. */
#define SEG_TYPE_TRAP_GATE      0xF /* Trap gate.      */

/* Flat-model segment descriptor constructors. */
/* Null segment. */
#define SEG_NULL    ((seg_desc_t) { .raw = 0 })

/* Kernel 64-bit code: DPL=0, L=1, D/B=0. */
#define SEG_KCODE64 ((seg_desc_t) { .bits = {   \
    .limit_low   = 0xFFFF,                      \
    .base_low    = 0,                           \
    .type        = SEG_TYPE_CODE_XR,            \
    .s           = 1,                           \
    .dpl         = 0,                           \
    .present     = 1,                           \
    .limit_high  = 0xF,                         \
    .avl         = 0,                           \
    .l           = 1,  /* 64-bit mode        */ \
    .db          = 0,  /* Must be 0 when L=1 */ \
    .granularity = 1,                           \
    .base_high   = 0,                           \
}})

/* Kernel data: DPL=0. L and D/B ignored in 64-bit mode for data segs. */
#define SEG_KDATA64 ((seg_desc_t) { .bits = {   \
    .limit_low   = 0xFFFF,                      \
    .base_low    = 0,                           \
    .type        = SEG_TYPE_DATA_RW,            \
    .s           = 1,                           \
    .dpl         = 0,                           \
    .present     = 1,                           \
    .limit_high  = 0xF,                         \
    .avl         = 0,                           \
    .l           = 0,                           \
    .db          = 1,                           \
    .granularity = 1,                           \
    .base_high   = 0,                           \
}})

/* User data: DPL=0. L and D/B ignored in 64-bit mode for data segs. */
/* This segment must come before user code for SYSRET convention. */
#define SEG_UDATA64 ((seg_desc_t) { .bits = {   \
    .limit_low   = 0xFFFF,                      \
    .base_low    = 0,                           \
    .type        = SEG_TYPE_DATA_RW,            \
    .s           = 1,                           \
    .dpl         = 3,                           \
    .present     = 1,                           \
    .limit_high  = 0xF,                         \
    .avl         = 0,                           \
    .l           = 0,                           \
    .db          = 1,                           \
    .granularity = 1,                           \
    .base_high   = 0,                           \
}})

/* User 64-bit code: DPL=3, L=1. */
#define SEG_UCODE64 ((seg_desc_t) { .bits = {   \
    .limit_low   = 0xFFFF,                      \
    .base_low    = 0,                           \
    .type        = SEG_TYPE_CODE_XR,            \
    .s           = 1,                           \
    .dpl         = 3,                           \
    .present     = 1,                           \
    .limit_high  = 0xF,                         \
    .avl         = 0,                           \
    .l           = 1,                           \
    .db          = 0,                           \
    .granularity = 1,                           \
    .base_high   = 0,                           \
}})

/* 64-bit TSS descriptor - 16 bytes (SDM Vol. 3A 10.2.3, Fig. 10-4).    */
typedef struct __attribute__((packed)) tss_desc {
    seg_desc_t lo;         /* standard 8-byte descriptor (type = 9/11). */
    uint32_t   base_63_32; /* base address bits [63:32].                */
    uint32_t   _reserved;  /* must be zero.                             */
} tss_desc_t;

/*
 * @brief Construct a TSS Segment Descriptor for GDT.
 *
 * @param base  Base address of the TSS.
 * @param limit Size of the TSS in bytes.
 */
#define TSS_DESC(base, limit) ((tss_desc_t) {                          \
    .lo = (seg_desc_t) { .bits = {                                     \
        .limit_low   = (uint16_t) ((limit) & 0xffff),                  \
        .base_low    = (uint32_t) (((uintptr_t) (base)) & 0x00ffffff), \
        .type        = SEG_TYPE_TSS_AVAIL,                             \
        .s           = 0,                                              \
        .dpl         = 0,                                              \
        .present     = 1,                                              \
        .limit_high  = (uint8_t) (((limit) >> 16) & 0xf),              \
        .granularity = 0,                                              \
        .base_high   = (uint8_t) ((((uintptr_t) (base)) >> 24) & 0xff) \
    }},                                                                \
    .base_63_32 = (uint32_t) (((uintptr_t) (base)) >> 32),             \
    ._reserved       = 0,                                              \
})

/* 64-bit Task-State Segment (SDM Vol. 3A 10.7, Fig. 10-11).         */
typedef struct __attribute__((packed)) tss {
    uint32_t _reserved0;
    uint64_t rsp[3];     /* Stack Pointers for privilege levels 0-2. */
    uint64_t _reserved1; /* Must be zero.                            */
    uint64_t ist[7];     /* IST1-IST7.                               */
    uint64_t _reserved2; /* Must be zero.                            */
    uint16_t _reserved3; /* Must be zero.                            */
    uint16_t iopb_offset; /* I/O Permission Bitmap base.             */
} tss_t;

/* GDTR pseudo-descriptor (SDM Vol. 3A 3.5.1, Fig. 3-11). */
typedef struct __attribute__((packed)) gdtr {
    uint16_t limit; /* Byte length of GDT minus 1.        */
    uint64_t base;  /* Linear address of GDT.             */
} gdtr_t;

/* Global Descriptor Table (GDT). */
typedef struct __attribute__((packed)) gdt {
    seg_desc_t null;  /* Null descriptor.       */
    seg_desc_t kcode; /* Kernel Code Segment.   */
    seg_desc_t kdata; /* Kernel Data Segment.   */
    seg_desc_t udata; /* User Data Segment.     */
    seg_desc_t ucode; /* User Code Segment.     */
    tss_desc_t tss;   /* 64-bit TSS descriptor. */
} gdt_t;

/*
 * @brief Initialize GDT and TSS for the bootstrap CPU.
 */
void segment_init(void);

#endif /* ARCH_X86_64_SEGMENT_H */
