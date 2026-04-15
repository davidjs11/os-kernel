/*   arch/x86_64/entry.S   */

/* Architecture-specific entry point (called before kmain). */

.extern _stack_top

.global kernel_entry_point
kernel_entry_point:
    /* Use our kernel boot stack instead of bootloader's stack. */
    movq $_stack_top, %rsp
    xor %rbp, %rbp
    call kmain
