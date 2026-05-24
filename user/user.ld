/*   user/user.ld   */

/*
 * User-space programs linker script. Programs are flattened with objcopy and
 * loaded at USER_ENTRY_VA by the kernel, so _start must be the first thing.
 */

ENTRY(_start)

SECTIONS
{
    . = 0x400000;

    .text : {
        *(.text.start)
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }
}
