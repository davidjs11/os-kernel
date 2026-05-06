/*   arch/aarch64/platform/qemu-virt/linker.ld   */

/* qemu-virt linker script with Limine bootloader. */

OUTPUT_FORMAT(elf64-littleaarch64)

ENTRY(kernel_entry_point)

PHDRS
{
    limine_requests PT_LOAD;
    text PT_LOAD;
    rodata PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{
    . = 0xffffffff80000000;
    _kernel_image_start = .;

    /* --- limine_requests --- */
    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :limine_requests

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    /* --- text --- */
    .text : {
        *(.text .text.*)
    } :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    /* --- rodata --- */
    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    .note.gnu.build-id : {
        *(.note.gnu.build-id)
    } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    /* --- data --- */
    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(16);
        _stack_bottom = .;
        . += 16 * 1024; /* 16 KiB. */
        _stack_top = .;
    } :data

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }

    _kernel_image_end = .;
}
