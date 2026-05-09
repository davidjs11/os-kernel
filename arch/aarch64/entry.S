/*   arch/aarch64/entry.S   */

/* Architecture-specific entry point (called before kmain). */

.section .text
.global kernel_entry_point
kernel_entry_point:
    /* Use our kernel boot stack instead of bootloader's stack. */
    ldr x30, =_stack_top
    mov sp, x30
    bl kmain

    /* Never reached. */
    b .

.size kernel_entry_point, . - kernel_entry_point
